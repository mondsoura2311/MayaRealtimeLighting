# Quick Start Guide - Maya Real-time Lighting Plugin

## Installation (5 minutes)

### 1. Prerequisites

Verify you have:
- ✅ Autodesk Maya 2020 or later
- ✅ NVIDIA RTX GPU (RTX 2060 minimum)
- ✅ Latest NVIDIA driver

**Check your GPU:**
```bash
# Windows PowerShell
deviceQuery  # Part of CUDA toolkit

# Linux/macOS
/usr/local/cuda/extras/demo_suite/deviceQuery
```

Look for:
```
CUDA Capability Major/Minor version number: 7.x
Total amount of global memory: XXXX MBytes
```

### 2. Download Pre-built Plugin

Download from GitHub Releases:
```
https://github.com/mondsoura2311/MayaRealtimeLighting/releases
```

Choose your platform:
- `MayaRealtimeLighting-Windows.zip` (Maya 2025, CUDA 11.8)
- `MayaRealtimeLighting-Linux.tar.gz` (Ubuntu 20.04+)
- `MayaRealtimeLighting-macOS.dmg` (Intel/Apple Silicon)

### 3. Install Plugin

**Windows:**
```powershell
# Extract zip file
# Copy MayaRealtimeLighting.mll to:
# C:\Users\[YourUsername]\Documents\maya\2025\plug-ins\

$pluginPath = "$env:USERPROFILE\Documents\maya\2025\plug-ins"
Copy-Item "MayaRealtimeLighting.mll" $pluginPath
```

**Linux:**
```bash
# Extract tar.gz
tar xzf MayaRealtimeLighting-Linux.tar.gz

# Copy to Maya plugins
mkdir -p ~/maya/2025/plug-ins
cp MayaRealtimeLighting.so ~/maya/2025/plug-ins/
```

**macOS:**
```bash
# Extract dmg and copy
cp MayaRealtimeLighting.bundle ~/Library/Preferences/Autodesk/maya/2025/plug-ins/
```

### 4. Load in Maya

1. Open **Autodesk Maya**
2. Go to **Windows > Settings/Preferences > Plug-in Manager**
3. Search for "MayaRealtimeLighting"
4. Check **Loaded** and **Auto load**
5. Click **Apply**

You should see:
```
[INFO] MayaRealtimeLighting plugin loaded successfully
```

---

## First Test (2 minutes)

### Step 1: Create Test Scene

In Maya's **Script Editor** (Python console):

```python
import sys
sys.path.append("path/to/plugin/examples")
import test_scene
test_scene.create_test_scene()
```

This creates:
- Red sphere (metallic)
- Blue cube (matte)
- Gray ground plane
- 3 lights
- Test camera

### Step 2: Enable Real-time Lighting

In the **Script Editor**:

```python
# Enable real-time lighting
cmds.rtLightingToggle(state=True)
```

Or in **MEL** console:
```mel
rtLightingToggle -state true;
```

### Step 3: View Result

- Look at the viewport
- You should see **real-time lighting** with shadows
- Camera: Use middle mouse button to rotate
- Zoom: Scroll wheel
- Pan: Shift + middle mouse button

### Step 4: Adjust Settings

In Python:

```python
# Get the lighting engine
engine = rtLightingGetEngine()

# Adjust ray bounces (1-5)
engine.setMaxBounces(3)

# Adjust samples per pixel (1-8)
engine.setSamplesPerPixel(2)

# Reset accumulation (for cleaner image)
engine.resetAccumulation()
```

---

## Common Tasks

### Disable Real-time Lighting

```python
cmds.rtLightingToggle(state=False)
```

### Add New Mesh to Scene

```python
# Create a polygon in Maya
cmds.polySphere(r=2, name="MySphere")

# Apply a material
mat = cmds.shadingNode('standardSurface', asShader=True)
cmds.setAttr(mat + ".baseColor", 0.8, 0.2, 0.2, type="double3")
cmds.setAttr(mat + ".metalness", 0.5)

# Select sphere and assign material
cmds.select("MySphere")
cmds.hyperShade(assign=mat)

# Real-time lighting will pick it up automatically
```

### Change Lighting Settings

```python
# High quality (slower)
engine.setMaxBounces(4)
engine.setSamplesPerPixel(4)

# Fast (interactive)
engine.setMaxBounces(1)
engine.setSamplesPerPixel(1)

# Enable/disable temporal accumulation
engine.setAccumulationEnabled(True)
```

### Move Camera

**Maya Viewport Navigation:**
- **Rotate:** Middle mouse button + drag
- **Pan:** Shift + middle mouse button + drag
- **Zoom:** Scroll wheel or right mouse button + drag

**Programmatic:**
```python
# Move camera
cmds.move(0, 5, 15, "persp")  # Position
cmds.rotate(-15, 0, 0, "persp")  # Rotation
```

---

## Troubleshooting

### Plugin Won't Load

**Error:** "MayaRealtimeLighting: Error opening file"

**Solution:**
1. Verify file is in correct folder
2. Check Maya version matches plugin build
3. Check NVIDIA driver is installed
4. Look for error in Script Editor

```python
# Check plugin directory
import maya.cmds as cmds
print(cmds.getenv("MAYA_PLUG_IN_PATH"))
```

### "GPU not supported" Error

**Cause:** GPU Compute Capability < 7.0

**Solution:**
1. Check compute capability:
   ```bash
   deviceQuery | grep "Compute Capability"
   ```
2. If < 7.0, upgrade GPU or use CPU fallback

### No Lighting Visible

**Causes:**
1. Real-time lighting not enabled
2. No meshes in scene
3. Camera not configured

**Solutions:**
```python
# Verify enabled
engine = rtLightingGetEngine()
if not engine.isEnabled():
    print("Enabling real-time lighting...")
    cmds.rtLightingToggle(state=True)

# Check scene has meshes
meshes = cmds.ls(type="mesh")
print(f"Meshes in scene: {len(meshes)}")

# Reset and re-render
engine.resetAccumulation()
```

### Low Frame Rate

**Causes:**
- Too many samples per pixel
- Ray bounces too high
- Scene too complex

**Solutions:**
```python
# Reduce quality
engine.setMaxBounces(1)  # Single bounce
engine.setSamplesPerPixel(1)  # One sample

# Monitor GPU
# Linux/macOS: nvidia-smi
# Windows: GPU-Z or NVIDIA control panel
```

### Out of GPU Memory

**Error:** "CUDA: out of memory"

**Solutions:**
1. Reduce scene size
2. Close other GPU applications
3. Reduce viewport resolution
4. Simplify meshes (reduce polygon count)

---

## Performance Tips

### For Best Quality

```python
engine.setMaxBounces(4)      # Multiple bounces = better GI
engine.setSamplesPerPixel(8) # More samples = less noise
engine.setAccumulationEnabled(True)  # Accumulate over time
```

**Expected:** 5-10 FPS, high quality after 30 frames

### For Interactive Performance

```python
engine.setMaxBounces(1)      # Single bounce
engine.setSamplesPerPixel(1) # Minimum samples
engine.setAccumulationEnabled(True)  # Let it accumulate
```

**Expected:** 30-60 FPS, improves quality as frames accumulate

### Optimal Balance

```python
engine.setMaxBounces(2)      # 2-3 bounces
engine.setSamplesPerPixel(2) # 2-4 samples
engine.setAccumulationEnabled(True)
```

**Expected:** 15-30 FPS, good quality within seconds

---

## Next Steps

### Learn More

- [Full API Documentation](API.md)
- [Architecture Overview](ARCHITECTURE.md)
- [Build from Source](BUILD.md)
- [CUDA Setup](CUDA_SETUP.md)

### Example Projects

**Simple Character Lighting:**
```python
# Load character model
cmds.file("character.mb", i=True)

# Add three-point lighting
# Key light (main)
key_light = cmds.pointLight(name="KeyLight")
cmds.move(5, 8, 5, key_light)
cmds.setAttr(key_light + ".intensity", 1.5)

# Fill light (soften shadows)
fill_light = cmds.pointLight(name="FillLight")
cmds.move(-5, 4, 0, fill_light)
cmds.setAttr(fill_light + ".intensity", 0.5)

# Back light (separation)
back_light = cmds.pointLight(name="BackLight")
cmds.move(0, 6, -8, back_light)
cmds.setAttr(back_light + ".intensity", 0.8)

# Enable real-time lighting
cmds.rtLightingToggle(state=True)
```

**Product Visualization:**
```python
# Load product
cmds.file("product.mb", i=True)

# Metallic material
mat = cmds.shadingNode('standardSurface', asShader=True)
cmds.setAttr(mat + ".metalness", 1.0)
cmds.setAttr(mat + ".roughness", 0.2)

# Environment lighting
engine.setMaxBounces(3)  # Important for reflections
engine.setSamplesPerPixel(4)

# Enable real-time
cmds.rtLightingToggle(state=True)
```

---

## Support

- **GitHub Issues:** https://github.com/mondsoura2311/MayaRealtimeLighting/issues
- **Documentation:** See `docs/` folder
- **Examples:** See `examples/` folder

---

## Tips & Tricks

### Keyboard Shortcuts (Maya)

| Action | Shortcut |
|--------|----------|
| Pan Viewport | Shift + Middle Mouse |
| Rotate View | Middle Mouse Drag |
| Zoom | Scroll Wheel |
| Frame Selected | F |
| Frame All | A |
| Toggle Wireframe | 4 |
| Toggle Smooth Shading | 6 |

### Script Editor Workflow

1. **Open:** Windows > General Editors > Script Editor
2. **Python Tab:** Write Python code
3. **Execute:** Ctrl + Enter (or click Execute)
4. **Save:** File > Save Script

### Batch Processing

```python
# Process multiple scenes
import os
import maya.cmds as cmds

scenes_dir = "path/to/scenes"
for scene_file in os.listdir(scenes_dir):
    if scene_file.endswith(".ma"):
        cmds.file(os.path.join(scenes_dir, scene_file), open=True)
        
        # Process with real-time lighting
        cmds.rtLightingToggle(state=True)
        # ... render frames ...
        
        cmds.file(save=True)
```

---

## Version History

### v0.1.0 (Current - Prototype)
- ✅ Basic ray tracing
- ✅ Single/multi-bounce GI
- ✅ Point lights
- ✅ PBR materials
- ⏳ Denoising (planned)
- ⏳ Area lights (planned)
- ⏳ Volumetrics (planned)

---

## License

MIT License - See LICENSE file for details

**Summary:** Free for personal and commercial use, attribution appreciated.

---

**Happy lighting! 🎨**
