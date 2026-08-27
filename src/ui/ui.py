"""
Maya Real-time Lighting - UI Module

Provides a user-friendly interface for controlling real-time lighting in Maya.
"""

import maya.cmds as cmds
import maya.mel as mel
from functools import partial


class RTLightingUI:
    """
    Main UI class for MayaRealtimeLighting plugin
    """
    
    PLUGIN_NAME = "MayaRealtimeLighting"
    WINDOW_NAME = "RTLightingWindow"
    MENU_NAME = "RTLightingMenu"
    
    # Settings
    enabled = False
    max_bounces = 2
    samples_per_pixel = 1
    accumulation_enabled = True
    
    @classmethod
    def show(cls):
        """Show the main UI window"""
        # Delete existing window
        if cmds.window(cls.WINDOW_NAME, exists=True):
            cmds.deleteUI(cls.WINDOW_NAME)
        
        # Create window
        window = cmds.window(
            cls.WINDOW_NAME,
            title="Maya Real-time Lighting",
            widthHeight=(400, 500),
            resizeToFitChildren=True
        )
        
        # Main layout
        main_layout = cmds.columnLayout(adjustableColumn=True, rowSpacing=10)
        
        # Header
        cmds.text(label="Real-time Global Illumination", font="boldLabelFont", height=30)
        cmds.separator(height=20, style="in")
        
        # === STATUS SECTION ===
        cmds.text(label="Status", font="boldLabelFont", height=20)
        cls.status_text = cmds.text(
            label="○ Disabled",
            backgroundColor=(1, 0.2, 0.2),
            height=25
        )
        
        cmds.separator(height=15, style="in")
        
        # === ENABLE/DISABLE SECTION ===
        cmds.text(label="Controls", font="boldLabelFont", height=20)
        
        button_layout = cmds.rowColumnLayout(numberOfColumns=2, columnWidth=[(1, 200), (2, 200)])
        
        cmds.button(
            label="✓ Enable Lighting",
            backgroundColor=(0.2, 0.7, 0.2),
            height=40,
            command=partial(cls.enable_lighting)
        )
        
        cmds.button(
            label="✗ Disable Lighting",
            backgroundColor=(0.7, 0.2, 0.2),
            height=40,
            command=partial(cls.disable_lighting)
        )
        
        cmds.setParent(main_layout)
        cmds.separator(height=15, style="in")
        
        # === QUALITY SETTINGS ===
        cmds.text(label="Quality Settings", font="boldLabelFont", height=20)
        
        # Max Bounces
        cmds.rowColumnLayout(numberOfColumns=3, columnWidth=[(1, 150), (2, 100), (3, 150)])
        cmds.text(label="Ray Bounces:")
        cls.bounces_slider = cmds.intSlider(
            min=1,
            max=8,
            value=cls.max_bounces,
            step=1,
            dragCommand=partial(cls.update_bounces)
        )
        cls.bounces_text = cmds.text(label=str(cls.max_bounces))
        
        cmds.setParent(main_layout)
        
        # Samples per pixel
        cmds.rowColumnLayout(numberOfColumns=3, columnWidth=[(1, 150), (2, 100), (3, 150)])
        cmds.text(label="Samples per Pixel:")
        cls.samples_slider = cmds.intSlider(
            min=1,
            max=8,
            value=cls.samples_per_pixel,
            step=1,
            dragCommand=partial(cls.update_samples)
        )
        cls.samples_text = cmds.text(label=str(cls.samples_per_pixel))
        
        cmds.setParent(main_layout)
        cmds.separator(height=15, style="in")
        
        # === ADVANCED OPTIONS ===
        cmds.text(label="Advanced Options", font="boldLabelFont", height=20)
        
        # Accumulation
        cls.accumulation_checkbox = cmds.checkBox(
            label="Temporal Accumulation (improves quality over time)",
            value=cls.accumulation_enabled,
            onCommand=partial(cls.toggle_accumulation, True),
            offCommand=partial(cls.toggle_accumulation, False)
        )
        
        # Reset button
        cmds.button(
            label="Reset Accumulation",
            height=30,
            command=partial(cls.reset_accumulation)
        )
        
        cmds.separator(height=15, style="in")
        
        # === PRESET QUALITY MODES ===
        cmds.text(label="Quality Presets", font="boldLabelFont", height=20)
        
        preset_layout = cmds.columnLayout(adjustableColumn=True, rowSpacing=5)
        
        cmds.button(
            label="⚡ Fast (Interactive)",
            backgroundColor=(0.7, 0.7, 0),
            height=30,
            command=partial(cls.set_preset, "fast")
        )
        cmds.button(
            label="⚙ Balanced",
            backgroundColor=(0.7, 0.5, 0),
            height=30,
            command=partial(cls.set_preset, "balanced")
        )
        cmds.button(
            label="🎨 High Quality",
            backgroundColor=(0, 0.5, 0.7),
            height=30,
            command=partial(cls.set_preset, "quality")
        )
        
        cmds.setParent(main_layout)
        cmds.separator(height=15, style="in")
        
        # === TEST SCENE ===
        cmds.text(label="Test Scene", font="boldLabelFont", height=20)
        cmds.button(
            label="Create Test Scene",
            backgroundColor=(0.3, 0.3, 0.7),
            height=35,
            command=partial(cls.create_test_scene)
        )
        
        cmds.separator(height=15, style="in")
        
        # === INFO ===
        info_text = (
            "v0.1.0 | Real-time Global Illumination\n"
            "GitHub: github.com/mondsoura2311/MayaRealtimeLighting\n\n"
            "Requirements: NVIDIA RTX GPU + CUDA 11.0+"
        )
        cmds.text(label=info_text, height=60, wordWrap=True)
        
        # Show window
        cmds.showWindow(window)
    
    @classmethod
    def enable_lighting(cls, *args):
        """Enable real-time lighting"""
        cls.enabled = True
        cmds.text(cls.status_text, edit=True, label="● ENABLED", backgroundColor=(0.2, 0.7, 0.2))
        cmds.inViewMessage(
            amg="Real-time Lighting Enabled",
            pos="topCenter",
            fade=True,
            fit=True
        )
        print("✓ Real-time lighting enabled")
    
    @classmethod
    def disable_lighting(cls, *args):
        """Disable real-time lighting"""
        cls.enabled = False
        cmds.text(cls.status_text, edit=True, label="○ DISABLED", backgroundColor=(0.7, 0.2, 0.2))
        cmds.inViewMessage(
            amg="Real-time Lighting Disabled",
            pos="topCenter",
            fade=True,
            fit=True
        )
        print("✗ Real-time lighting disabled")
    
    @classmethod
    def update_bounces(cls, value):
        """Update ray bounces"""
        cls.max_bounces = int(value)
        cmds.text(cls.bounces_text, edit=True, label=str(cls.max_bounces))
        print(f"Max bounces: {cls.max_bounces}")
    
    @classmethod
    def update_samples(cls, value):
        """Update samples per pixel"""
        cls.samples_per_pixel = int(value)
        cmds.text(cls.samples_text, edit=True, label=str(cls.samples_per_pixel))
        print(f"Samples per pixel: {cls.samples_per_pixel}")
    
    @classmethod
    def toggle_accumulation(cls, enabled, *args):
        """Toggle temporal accumulation"""
        cls.accumulation_enabled = enabled
        status = "enabled" if enabled else "disabled"
        print(f"Accumulation {status}")
    
    @classmethod
    def reset_accumulation(cls, *args):
        """Reset accumulation buffer"""
        cmds.inViewMessage(
            amg="Accumulation Buffer Reset",
            pos="topCenter",
            fade=True,
            fit=True
        )
        print("Accumulation buffer reset")
    
    @classmethod
    def set_preset(cls, preset, *args):
        """Set quality preset"""
        presets = {
            "fast": {"bounces": 1, "samples": 1},
            "balanced": {"bounces": 2, "samples": 2},
            "quality": {"bounces": 4, "samples": 4},
        }
        
        if preset in presets:
            config = presets[preset]
            cls.max_bounces = config["bounces"]
            cls.samples_per_pixel = config["samples"]
            
            cmds.intSlider(cls.bounces_slider, edit=True, value=cls.max_bounces)
            cmds.intSlider(cls.samples_slider, edit=True, value=cls.samples_per_pixel)
            cmds.text(cls.bounces_text, edit=True, label=str(cls.max_bounces))
            cmds.text(cls.samples_text, edit=True, label=str(cls.samples_per_pixel))
            
            preset_names = {"fast": "Fast", "balanced": "Balanced", "quality": "High Quality"}
            cmds.inViewMessage(
                amg=f"Preset: {preset_names[preset]}",
                pos="topCenter",
                fade=True,
                fit=True
            )
            print(f"Preset: {preset_names[preset]} (Bounces: {cls.max_bounces}, Samples: {cls.samples_per_pixel})")
    
    @classmethod
    def create_test_scene(cls, *args):
        """Create a test scene"""
        try:
            import sys
            sys.path.append("examples")
            from test_scene import create_test_scene
            create_test_scene()
            cmds.inViewMessage(
                amg="Test Scene Created",
                pos="topCenter",
                fade=True,
                fit=True
            )
        except Exception as e:
            cmds.confirmDialog(
                title="Error",
                message=f"Could not create test scene: {str(e)}",
                button="OK"
            )
    
    @classmethod
    def create_menu(cls):
        """Create main menu in Maya"""
        # Get main window
        main_window = mel.eval("$temp = $gMainWindow")
        
        # Create menu
        menu = cmds.menu(
            cls.MENU_NAME,
            label="MayaRealtimeLighting",
            parent=main_window
        )
        
        # Menu items
        cmds.menuItem(
            label="Show UI",
            command=partial(cls.show),
            annotation="Open Real-time Lighting UI"
        )
        cmds.menuItem(divider=True)
        cmds.menuItem(
            label="Enable Lighting",
            command=partial(cls.enable_lighting),
            annotation="Enable real-time lighting"
        )
        cmds.menuItem(
            label="Disable Lighting",
            command=partial(cls.disable_lighting),
            annotation="Disable real-time lighting"
        )
        cmds.menuItem(divider=True)
        cmds.menuItem(
            label="Create Test Scene",
            command=partial(cls.create_test_scene),
            annotation="Create sample scene for testing"
        )
        cmds.menuItem(divider=True)
        cmds.menuItem(
            label="Documentation",
            command=lambda *args: print("Visit: https://github.com/mondsoura2311/MayaRealtimeLighting/blob/main/docs/QUICKSTART.md")
        )
        
        print(f"✓ {cls.PLUGIN_NAME} menu created in Maya")


def initialize():
    """Initialize plugin UI"""
    try:
        RTLightingUI.create_menu()
        print("Real-time Lighting plugin UI initialized")
    except Exception as e:
        print(f"Error initializing UI: {e}")


if __name__ == "__main__":
    RTLightingUI.show()
