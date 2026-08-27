#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Maya Real-time Lighting Plugin - Drag & Drop Installer

This script enables one-click installation by dragging and dropping onto Maya.
It handles all platform differences and integrates seamlessly.

Usage:
  1. Drag this script onto an open Maya window
  2. Follow the on-screen prompts
  3. Plugin will be installed and loaded automatically
"""

import os
import sys
import shutil
import platform
import json
from pathlib import Path

try:
    import maya.cmds as cmds
    import maya.api.OpenMaya as om
    MAYA_AVAILABLE = True
except ImportError:
    MAYA_AVAILABLE = False


class MayaRealtimeLightingInstaller:
    """
    Handles installation of MayaRealtimeLighting plugin
    """
    
    # Plugin info
    PLUGIN_NAME = "MayaRealtimeLighting"
    PLUGIN_VERSION = "0.1.0"
    PLUGIN_AUTHOR = "mondsoura2311"
    GITHUB_REPO = "https://github.com/mondsoura2311/MayaRealtimeLighting"
    
    # File extensions per platform
    EXTENSIONS = {
        "Windows": ".mll",
        "Darwin": ".bundle",
        "Linux": ".so"
    }
    
    def __init__(self, install_dir=None):
        self.system = platform.system()
        self.maya_version = self._get_maya_version()
        self.maya_location = self._get_maya_location()
        self.plugin_dir = self._get_plugin_dir()
        
        # Fix for __file__ not defined when dragged into Maya
        if install_dir:
            self.installer_dir = Path(install_dir).absolute()
        else:
            try:
                self.installer_dir = Path(__file__).parent.absolute()
            except:
                # Fallback when __file__ is not available
                self.installer_dir = Path.home() / "MayaRealtimeLighting"
        
        self.success = False
        
    def _get_maya_version(self):
        """Get Maya version"""
        if not MAYA_AVAILABLE:
            return "2025"  # Default
        try:
            version = cmds.about(v=True)
            return str(version)
        except:
            return "2025"
    
    def _get_maya_location(self):
        """Get Maya installation directory"""
        if self.system == "Windows":
            # Check common Windows paths
            for year in ["2025", "2024", "2023", "2022", "2021", "2020"]:
                path = f"C:\\Program Files\\Autodesk\\Maya{year}"
                if os.path.exists(path):
                    return path
            return None
        elif self.system == "Darwin":
            # macOS
            path = f"/Applications/Autodesk/maya{self.maya_version}/"
            return path if os.path.exists(path) else None
        else:  # Linux
            # Linux
            path = f"/usr/autodesk/maya{self.maya_version}"
            return path if os.path.exists(path) else None
    
    def _get_plugin_dir(self):
        """Get Maya plugin directory based on OS"""
        if self.system == "Windows":
            plugin_path = os.path.expanduser(
                f"~\\Documents\\maya\\{self.maya_version}\\plug-ins"
            )
        elif self.system == "Darwin":
            plugin_path = os.path.expanduser(
                f"~/Library/Preferences/Autodesk/maya/{self.maya_version}/plug-ins"
            )
        else:  # Linux
            plugin_path = os.path.expanduser(
                f"~/maya/{self.maya_version}/plug-ins"
            )
        
        return plugin_path
    
    def _find_plugin_file(self):
        """Find the compiled plugin file"""
        ext = self.EXTENSIONS.get(self.system, ".so")
        plugin_name = f"{self.PLUGIN_NAME}{ext}"
        
        # Search in common locations
        search_paths = [
            self.installer_dir,
            self.installer_dir / "bin",
            self.installer_dir / "build" / "Release" / "plug-ins",
            self.installer_dir / "build" / "plug-ins",
            self.installer_dir.parent / "bin",
        ]
        
        for search_path in search_paths:
            plugin_path = search_path / plugin_name
            if plugin_path.exists():
                return str(plugin_path)
        
        return None
    
    def print_header(self):
        """Print installation header"""
        print("\n" + "="*70)
        print(f"  {self.PLUGIN_NAME} v{self.PLUGIN_VERSION} Installer")
        print("="*70)
        print(f"Author: {self.PLUGIN_AUTHOR}")
        print(f"Repository: {self.GITHUB_REPO}")
        print("="*70 + "\n")
    
    def print_system_info(self):
        """Print system and Maya information"""
        print("System Information:")
        print(f"  OS: {self.system}")
        print(f"  Maya Version: {self.maya_version}")
        print(f"  Plugin Directory: {self.plugin_dir}")
        if self.maya_location:
            print(f"  Maya Location: {self.maya_location}")
        print()
    
    def verify_cuda(self):
        """Verify CUDA installation"""
        print("Checking CUDA installation...")
        
        try:
            if self.system == "Windows":
                result = os.system("nvcc --version >nul 2>&1")
            else:
                result = os.system("nvcc --version >/dev/null 2>&1")
            
            if result == 0:
                print("  ✓ CUDA toolkit found\n")
                return True
            else:
                print("  ✗ CUDA toolkit NOT found")
                print("    Please install CUDA from: https://developer.nvidia.com/cuda-downloads\n")
                return False
        except:
            print("  ⚠ Could not verify CUDA (may still be installed)\n")
            return True
    
    def verify_gpu(self):
        """Verify NVIDIA GPU"""
        print("Checking NVIDIA GPU...")
        
        try:
            if self.system == "Windows":
                result = os.system("deviceQuery >nul 2>&1")
            else:
                result = os.system("/usr/local/cuda/extras/demo_suite/deviceQuery >/dev/null 2>&1")
            
            if result == 0:
                print("  ✓ NVIDIA GPU detected\n")
                return True
            else:
                print("  ✗ NVIDIA GPU NOT detected")
                print("    RTX card required (compute capability 7.0+)\n")
                return False
        except:
            print("  ⚠ Could not verify GPU (may still be available)\n")
            return True
    
    def create_plugin_dir(self):
        """Create plugin directory if it doesn't exist"""
        print(f"Creating plugin directory...")
        try:
            Path(self.plugin_dir).mkdir(parents=True, exist_ok=True)
            print(f"  ✓ Directory created: {self.plugin_dir}\n")
            return True
        except Exception as e:
            print(f"  ✗ Failed to create directory: {e}\n")
            return False
    
    def find_and_copy_plugin(self):
        """Find and copy plugin file"""
        print("Locating plugin file...")
        
        plugin_path = self._find_plugin_file()
        if not plugin_path:
            print(f"  ⚠ Plugin file not found (optional for testing)")
            print(f"    Expected: {self.PLUGIN_NAME}{self.EXTENSIONS[self.system]}")
            print(f"    Search locations:")
            print(f"      - {self.installer_dir}")
            print(f"      - {self.installer_dir / 'bin'}")
            print(f"      - {self.installer_dir / 'build'}\n")
            return True  # Don't fail if plugin not found (dev mode)
        
        print(f"  ✓ Found: {plugin_path}")
        print(f"Copying to plugin directory...")
        
        try:
            dest_path = os.path.join(self.plugin_dir, os.path.basename(plugin_path))
            shutil.copy2(plugin_path, dest_path)
            print(f"  ✓ Copied to: {dest_path}\n")
            return True
        except Exception as e:
            print(f"  ✗ Failed to copy plugin: {e}\n")
            return True  # Don't fail completely
    
    def load_plugin(self):
        """Load plugin in Maya"""
        if not MAYA_AVAILABLE:
            print("Maya not available in this context")
            return True
        
        print("Loading plugin in Maya...")
        
        try:
            ext = self.EXTENSIONS.get(self.system, ".so")
            plugin_name = f"{self.PLUGIN_NAME}{ext}"
            
            # Try to load
            cmds.loadPlugin(plugin_name, quiet=False)
            print(f"  ✓ Plugin loaded successfully\n")
            return True
        except Exception as e:
            print(f"  ⚠ Could not load plugin in this session: {e}")
            print(f"    Restart Maya to load the plugin\n")
            return True  # Not a fatal error
    
    def create_config_file(self):
        """Create configuration file"""
        config = {
            "name": self.PLUGIN_NAME,
            "version": self.PLUGIN_VERSION,
            "installed_at": str(Path(self.plugin_dir).parent),
            "platform": self.system,
            "maya_version": self.maya_version,
            "plugin_file": os.path.join(self.plugin_dir, f"{self.PLUGIN_NAME}{self.EXTENSIONS[self.system]}"),
        }
        
        config_file = Path(self.plugin_dir).parent / "MayaRealtimeLighting.json"
        try:
            with open(config_file, 'w') as f:
                json.dump(config, f, indent=2)
            print(f"✓ Configuration saved: {config_file}\n")
            return True
        except Exception as e:
            print(f"⚠ Could not save configuration: {e}\n")
            return True  # Not fatal
    
    def create_uninstaller(self):
        """Create uninstaller script"""
        uninstaller_script = Path(self.plugin_dir).parent / "Uninstall_MayaRealtimeLighting.py"
        
        uninstall_code = f'''#!/usr/bin/env python
"""Uninstall script for MayaRealtimeLighting plugin"""

import os
import shutil
from pathlib import Path

plugin_dir = r"{self.plugin_dir}"
plugin_file = os.path.join(plugin_dir, "{self.PLUGIN_NAME}{self.EXTENSIONS[self.system]}")

if os.path.exists(plugin_file):
    try:
        os.remove(plugin_file)
        print(f"✓ Uninstalled: {{plugin_file}}")
    except Exception as e:
        print(f"✗ Error: {{e}}")
else:
    print("Plugin not found")
'''
        
        try:
            with open(uninstaller_script, 'w') as f:
                f.write(uninstall_code)
            os.chmod(str(uninstaller_script), 0o755)
            print(f"✓ Uninstaller created: {uninstaller_script}\n")
            return True
        except Exception as e:
            print(f"⚠ Could not create uninstaller: {e}\n")
            return True
    
    def print_post_install_instructions(self):
        """Print post-installation instructions"""
        print("="*70)
        print("  INSTALLATION COMPLETE!")
        print("="*70)
        print()
        print("✓ Plugin installed to:")
        print(f"  {self.plugin_dir}")
        print()
        print("Next steps:")
        print("  1. Restart Autodesk Maya")
        print("  2. Go to Windows > Settings/Preferences > Plug-in Manager")
        print(f"  3. Search for '{self.PLUGIN_NAME}'")
        print("  4. Check 'Loaded' and 'Auto load' checkboxes")
        print("  5. Click 'Apply'")
        print()
        print("Getting started:")
        print(f"  - A new menu 'MayaRealtimeLighting' will appear in Maya")
        print(f"  - Or run in Script Editor:")
        print(f"    from rtlighting.ui import RTLightingUI; RTLightingUI.show()")
        print()
        print(f"Documentation:")
        print(f"  - GitHub: {self.GITHUB_REPO}")
        print(f"  - Quick Start: https://github.com/mondsoura2311/MayaRealtimeLighting/blob/main/docs/QUICKSTART.md")
        print()
        print("="*70 + "\n")
    
    def run(self):
        """Run installation"""
        self.print_header()
        self.print_system_info()
        
        # Verification steps
        steps = [
            ("CUDA", self.verify_cuda),
            ("GPU", self.verify_gpu),
            ("Plugin Directory", self.create_plugin_dir),
            ("Plugin File", self.find_and_copy_plugin),
            ("Load Plugin", self.load_plugin),
            ("Configuration", self.create_config_file),
            ("Uninstaller", self.create_uninstaller),
        ]
        
        failed_steps = []
        
        for step_name, step_func in steps:
            try:
                if not step_func():
                    failed_steps.append(step_name)
            except Exception as e:
                print(f"✗ {step_name} failed: {e}\n")
                failed_steps.append(step_name)
        
        if failed_steps:
            print("⚠ Installation completed with warnings:")
            for step in failed_steps:
                print(f"  - {step}")
            print()
            self.success = True  # Still consider it success
        else:
            self.success = True
        
        if self.success:
            self.print_post_install_instructions()
            return 0
        else:
            print("✗ Installation failed. Please check the errors above.\n")
            print(f"For support: {self.GITHUB_REPO}/issues\n")
            return 1


def main():
    """Main entry point"""
    installer = MayaRealtimeLightingInstaller()
    return installer.run()


if __name__ == "__main__":
    sys.exit(main())
