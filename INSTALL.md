# Maya Real-time Lighting Plugin - Drag & Drop Installation Guide

## What is Drag & Drop Installation?

Instead of manual file copying, you can simply **drag this folder onto a Maya window** and the plugin will install automatically! This works on all platforms.

## Installation Methods

### Method 1: Run Python Installer (Recommended)

**Windows:**
1. Open Terminal/PowerShell in this folder
2. Run: `python install_plugin.py`
3. Restart Maya

**macOS/Linux:**
1. Open Terminal in this folder
2. Run: `python install_plugin.py`
3. Restart Maya

### Method 2: Drag Python Script onto Maya

**All Platforms:**
1. Locate `install_plugin.py`
2. Drag and drop it onto an **open Maya viewport**
3. Python console will execute the installation
4. Follow the on-screen prompts
5. Restart Maya

### Method 3: Run Shell/Batch Scripts

**Windows:**
```cmd
install_plugin.bat
```

**macOS/Linux:**
```bash
chmod +x install_plugin.sh
./install_plugin.sh
```

### Method 4: Manual Installation (Always Works)

If automated installation doesn't work:

**Windows:**
```
1. Copy MayaRealtimeLighting.mll
2. Paste into: C:\Users\[YourName]\Documents\maya\2025\plug-ins\
```

**macOS:**
```
1. Copy MayaRealtimeLighting.bundle
2. Paste into: ~/Library/Preferences/Autodesk/maya/2025/plug-ins/
```

**Linux:**
```
1. Copy MayaRealtimeLighting.so
2. Paste into: ~/maya/2025/plug-ins/
```

## After Installation

1. **Restart Maya** completely
2. Open **Windows > Settings/Preferences > Plug-in Manager**
3. Search for "MayaRealtimeLighting"
4. Check **Loaded** and **Auto load**
5. Click **Apply**

You should see a success message in the Script Editor.

## Troubleshooting

### Script won't execute when dragged

- Make sure Maya's Python is properly configured
- Try running from Terminal instead
- Check Maya's Script Editor for error messages

### "Permission Denied" on macOS/Linux

```bash
chmod +x install_plugin.py
chmod +x install_plugin.sh
```

### Plugin not found after installation

- Verify plugin file is in the installer folder
- Check that Maya version matches (e.g., 2025, 2024)
- Try manual installation method

### "CUDA not found" error

- Install CUDA Toolkit: https://developer.nvidia.com/cuda-downloads
- Ensure NVIDIA driver is up to date
- Reboot computer after CUDA installation

## What Gets Installed

✓ Plugin file (`MayaRealtimeLighting.mll/.so/.bundle`)
✓ Configuration file
✓ Uninstall script
✓ Auto-load preferences

## Uninstalling

Run the uninstall script that was created:

**Windows:**
```
C:\Users\[YourName]\Documents\maya\2025\Uninstall_MayaRealtimeLighting.py
```

**macOS/Linux:**
```bash
~/.maya/2025/Uninstall_MayaRealtimeLighting.py
```

Or manually delete the plugin file from your plugin directory.

## System Requirements

- ✓ Autodesk Maya 2020+
- ✓ NVIDIA RTX GPU (RTX 2060 minimum)
- ✓ CUDA Toolkit 11.0+
- ✓ Latest NVIDIA driver

## Support

If installation fails:

1. Check error messages in Maya's Script Editor
2. Verify CUDA installation
3. Check GPU compatibility
4. Visit: https://github.com/mondsoura2311/MayaRealtimeLighting/issues

---

**That's it! Enjoy real-time lighting in Maya! 🎨**
