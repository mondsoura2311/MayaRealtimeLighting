# Build Instructions for Maya Real-time Lighting Plugin

## Prerequisites

### System Requirements

**Windows:**
- Visual Studio 2019 or 2022 (Community, Professional, or Enterprise)
- Windows 10/11
- 10 GB free disk space

**Linux:**
- GCC 7.0+ or Clang 5.0+
- Ubuntu 18.04+, CentOS 7+, or equivalent
- 10 GB free disk space

**macOS:**
- Xcode 12+
- macOS 10.15+
- Apple Silicon M1/M2 (via Rosetta) or Intel
- 10 GB free disk space

### Required Software

1. **CMake** (3.16+)
   - Download: https://cmake.org/download/
   - Add to PATH

2. **Autodesk Maya** (2020 or later)
   - https://www.autodesk.com/products/maya/overview
   - Note the installation path
   - Requires valid license or free student version

3. **NVIDIA CUDA Toolkit** (11.0+)
   - https://developer.nvidia.com/cuda-downloads
   - Requires NVIDIA GPU with Compute Capability 7.0+
   - Latest version recommended

4. **NVIDIA GPU Driver**
   - Minimum: 456.71 (for CUDA 11.0)
   - Download: https://www.nvidia.com/Download/driverDetails.aspx

### Supported NVIDIA GPUs

**Recommended (RTX):**
- GeForce RTX 2060, 2070, 2080, 3060, 3070, 3080, 4070, 4090
- RTX A2000, A4000, A5000, A6000
- Tesla V100, A100

**Minimum (GTX 10 series and newer):**
- GeForce GTX 1050, 1060, 1070, 1080
- Quadro P1000 and above

**NOT Supported:**
- GeForce GTX 900 series and older (Compute Capability < 7.0)
- Integrated graphics

## Installation Steps

### Step 1: Verify CUDA Installation

**Windows (PowerShell):**
```powershell
# Check CUDA version
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\bin\nvcc.exe --version

# Check GPU
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\extras\demo_suite\deviceQuery.exe
```

**Linux/macOS (Terminal):**
```bash
# Check CUDA version
nvcc --version

# Check GPU
/usr/local/cuda/extras/demo_suite/deviceQuery
```

**Expected Output:**
```
CUDA Version 11.8
Device 0: "GeForce RTX 2060"
  Compute Capability: 7.5
```

### Step 2: Verify Maya Installation

**Windows:**
```powershell
# Default path
$MAYA_PATH = "C:\Program Files\Autodesk\Maya2025"
Test-Path $MAYA_PATH
```

**Linux:**
```bash
# Default path
ls ~/maya/2025
```

**macOS:**
```bash
# Default path
ls /Applications/Autodesk/maya2025/
```

### Step 3: Clone Repository

```bash
git clone https://github.com/mondsoura2311/MayaRealtimeLighting.git
cd MayaRealtimeLighting
```

### Step 4: Create Build Directory

```bash
mkdir build
cd build
```

### Step 5: Configure with CMake

**Windows (Visual Studio 2022):**
```powershell
cmake -G "Visual Studio 17 2022" `
  -DMAYA_LOCATION="C:/Program Files/Autodesk/Maya2025" `
  -DCUDA_TOOLKIT_ROOT_DIR="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.8" `
  -DCMAKE_BUILD_TYPE=Release ..
```

**Windows (Visual Studio 2019):**
```powershell
cmake -G "Visual Studio 16 2019" `
  -DMAYA_LOCATION="C:/Program Files/Autodesk/Maya2025" `
  -DCUDA_TOOLKIT_ROOT_DIR="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.8" `
  -DCMAKE_BUILD_TYPE=Release ..
```

**Linux (GCC):**
```bash
cmake -DMAYA_LOCATION="/usr/autodesk/maya2025" \
      -DCMAKE_BUILD_TYPE=Release ..
```

**macOS (Xcode):**
```bash
cmake -G Xcode \
      -DMAYA_LOCATION="/Applications/Autodesk/maya2025" \
      -DCMAKE_BUILD_TYPE=Release ..
```

### Step 6: Build

**Windows (Visual Studio):**
```powershell
# Using CMake
cmake --build . --config Release --parallel 8

# Or open Visual Studio
start MayaRealtimeLighting.sln
# Build > Build Solution (F7)
```

**Linux/macOS (Make/Xcode):**
```bash
cmake --build . --config Release --parallel 8
# Or
make -j8
```

### Step 7: Verify Build

```bash
# Check for plugin file
ls -la Release/plug-ins/MayaRealtimeLighting.mll   # Windows
ls -la Release/plug-ins/MayaRealtimeLighting.so    # Linux
ls -la Release/plug-ins/MayaRealtimeLighting.bundle # macOS
```

## Installation in Maya

### Method 1: Manual Plugin Folder

**Windows:**
```powershell
# Create plugins folder if needed
$pluginDir = "$env:USERPROFILE/Documents/maya/2025/plug-ins"
New-Item -ItemType Directory -Force -Path $pluginDir

# Copy plugin
Copy-Item "build/Release/MayaRealtimeLighting.mll" $pluginDir
```

**Linux:**
```bash
mkdir -p ~/maya/2025/plug-ins
cp build/MayaRealtimeLighting.so ~/maya/2025/plug-ins/
```

**macOS:**
```bash
mkdir -p ~/Library/Preferences/Autodesk/maya/2025/plug-ins
cp build/MayaRealtimeLighting.bundle ~/Library/Preferences/Autodesk/maya/2025/plug-ins/
```

### Method 2: Maya Plug-in Manager

1. Open Maya
2. Go to **Windows > Settings/Preferences > Plug-in Manager**
3. Click **Browse** and select the compiled plugin file
4. Check **Loaded** and **Auto load** checkboxes
5. Click **Apply** and **Close**

### Method 3: CMake Install

```bash
cd build
cmake --install . --config Release
```

## Troubleshooting

### CMake Configuration Issues

**Error: "Maya not found"**
```bash
# Explicitly set Maya path
cmake -DMAYA_LOCATION="C:/Program Files/Autodesk/Maya2025" ..
```

**Error: "CUDA not found"**
```bash
# Set CUDA path
cmake -DCUDA_TOOLKIT_ROOT_DIR="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.8" ..
```

### Compilation Errors

**CUDA Compilation Error (sm_70)**
- Update GPU driver to latest
- Upgrade CUDA to 11.8+
- Check compute capability: `deviceQuery`

**Visual Studio Linker Error**
- Ensure Maya include/lib paths are correct
- Check if Maya development kit is installed
- Try `cmake --build . --verbose` to see full errors

### Runtime Issues

**Plugin Won't Load**
1. Check Maya output window for error messages
2. Verify plugin is in correct directory
3. Confirm NVIDIA GPU drivers are up to date
4. Try loading in Terminal/PowerShell for more details

**"GPU not supported" Error**
- Check compute capability: `deviceQuery | grep "Compute Capability"`
- Minimum required: 7.0 (RTX cards and GTX 10 series)
- Update GPU driver

**Low Performance**
- Monitor GPU usage: `nvidia-smi`
- Check if GPU is being used (GPU Memory should increase)
- Ensure no other GPU-intensive applications running
- Try smaller viewport resolution

## Advanced Configuration

### Custom CUDA Architecture

```bash
# Target specific GPU architectures
cmake -DCMAKE_CUDA_ARCHITECTURES="70;75;80;86" ..

# For older GPUs (requires CUDA 11.0)
cmake -DCMAKE_CUDA_ARCHITECTURES="60;61;70" ..
```

### Optimized Build

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native" \
      -DCMAKE_CUDA_FLAGS_RELEASE="-O3 -lineinfo" ..
```

### Debug Build

```bash
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CUDA_FLAGS="-G -g -O0" ..
```

## Testing Installation

### Load Plugin in Maya

1. Start Maya
2. **Windows > Plug-in Manager**
3. Search for "MayaRealtimeLighting"
4. If found, click **Load**
5. Check for confirmation message

### Run Test Scene

```bash
# From Maya's Python console
import sys
sys.path.append("path/to/repo/examples")
import test_scene
test_scene.create_test_scene()
```

## Updating and Rebuilding

```bash
# Pull latest changes
git pull origin main

# Clean build
cd build
rm -rf *  # or del *.* on Windows

# Reconfigure
cmake -DMAYA_LOCATION="..." ..

# Rebuild
cmake --build . --config Release --clean-first
```

## System-Specific Notes

### Windows with NVLink (Multi-GPU)
Currently single GPU only. Multi-GPU support planned for future release.

### Linux with EGL Rendering
Ensure X11 libraries are installed:
```bash
sudo apt-get install libx11-dev libxrandr-dev libxcb-randr0-dev
```

### macOS Metal Rendering
OpenGL used for display. Metal support planned.

## Getting Help

- **GitHub Issues**: https://github.com/mondsoura2311/MayaRealtimeLighting/issues
- **NVIDIA CUDA Forum**: https://forums.developer.nvidia.com/c/cuda/
- **Maya Dev Forum**: https://forums.autodesk.com/t5/Maya-Forums/ct-p/area-p3379
