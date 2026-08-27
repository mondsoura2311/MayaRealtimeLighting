# CUDA Setup Guide

## NVIDIA GPU Requirements

### Compute Capability

Minimum required: **7.0** (RTX cards, GTX 10-series)

| Architecture | Compute Capability | Example GPUs |
|--------------|-------------------|---------------|
| Ampere | 8.0-8.6 | RTX 3060-3090, RTX A2000-A6000 |
| Turing | 7.5 | RTX 2060-2080 Ti, Quadro RTX |
| Volta | 7.0 | Tesla V100 |
| Pascal | 6.1 | GTX 1050-1080 Ti, Quadro P |
| **NOT SUPPORTED** | < 6.0 | GTX 900 series, GTX Titan X (original) |

### Memory Requirements

- **Minimum**: 2 GB VRAM
- **Recommended**: 6-8 GB VRAM
- **Optimal**: 10+ GB VRAM

### Driver Requirements

| CUDA Version | Minimum Driver |
|--------------|----------------|
| 11.8 | 456.71 |
| 11.7 | 456.38 |
| 11.0 | 450.51 |

## Installation

### Windows

1. **Download CUDA Toolkit**
   - https://developer.nvidia.com/cuda-downloads
   - Select: Windows > x86_64 > Windows 10/11 > installer type

2. **Run Installer**
   ```
   cuda_11.8.0_windows_network.exe
   ```
   - Choose: CUDA Toolkit + NVIDIA Graphics Driver
   - Installation path: `C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8`

3. **Verify Installation**
   ```powershell
   # Add to PATH
   $env:Path += ";C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\bin"
   
   # Check version
   nvcc --version
   ```

4. **Download cuDNN** (Optional, for future ML features)
   - https://developer.nvidia.com/cudnn
   - Extract to CUDA toolkit directory

### Linux

**Ubuntu/Debian:**
```bash
# Download and install
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-ubuntu2204.pin
sudo mv cuda-ubuntu2204.pin /etc/apt/preferences.d/cuda-repository-pin-600

# Add repository
wget https://developer.download.nvidia.com/compute/cuda/11.8.0/local_installers/cuda-repo-ubuntu2204-11-8-local_11.8.0-1_amd64.deb
sudo dpkg -i cuda-repo-ubuntu2204-11-8-local_11.8.0-1_amd64.deb

# Install
sudo apt-get update
sudo apt-get install cuda cuda-toolkit-11-8

# Add to PATH (.bashrc)
export PATH=/usr/local/cuda/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
```

**CentOS/RHEL:**
```bash
wget https://developer.download.nvidia.com/compute/cuda/repos/rhel8/x86_64/cuda-repo-rhel8-11.8.0-1.el8.x86_64.rpm
sudo rpm -i cuda-repo-rhel8-11.8.0-1.el8.x86_64.rpm
sudo yum clean all
sudo yum install cuda
```

### macOS

1. **Download CUDA Toolkit**
   - https://developer.nvidia.com/cuda-downloads
   - macOS > x86_64 or arm64

2. **Run Installer**
   ```bash
   sudo installer -pkg cuda_11.8.0_macos.pkg -target /
   ```

3. **Add to PATH** (`~/.zshrc`):
   ```bash
   export PATH=/usr/local/cuda/bin:$PATH
   export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
   ```

## Verification

### Check CUDA Installation

```bash
# Compiler version
nvcc --version

# Runtime version
/usr/local/cuda/extras/demo_suite/deviceQuery

# Bandwidthtest
/usr/local/cuda/extras/demo_suite/bandwidthTest
```

### Expected Output

```
/usr/local/cuda/extras/demo_suite/deviceQuery
CUDA Device Query (Runtime API) version (CUDART static linking)

Detected 1 CUDA Capable device(s)

Device 0: "GeForce RTX 2060"
  CUDA Driver Version / Runtime Version: 11.8 / 11.8
  CUDA Capability Major/Minor version number: 7.5
  Total amount of global memory: 5904 MBytes
  GPU Clock rate: 1365 MHz
  Memory Clock rate: 6001 Mhz
  Memory Bus Width: 192-bit
  Maximum Texture Dimension Size (x,y,z): (16384, 16384, 16384)
  Maximum Layered Texture Size (dim) x layers: (16384, 16384) x 2048
  Total amount of constant memory: 65536 bytes
  Total amount of shared memory per block: 49152 bytes
  Total number of registers available per block: 65536
  Warp size: 32
  Maximum number of threads per multiprocessor: 1024
  Maximum number of threads per block: 1024
  Max dimension size of a thread block (x,y,z): (1024, 1024, 64)
  Max dimension size of a grid size (x,y,z): (2147483647, 65535, 65535)
  Texture alignment: 512 bytes
  Concurrent copy and kernel execution: Yes
  Run time limit on kernels: Yes
  Integrated GPU sharing Host Memory: No
  Support host page-locked memory mapping: Yes
  Alignment requirement for Surfaces: Yes
  Device has ECC memory: No
  Device supports Unified Addressing (UVA): Yes
  Supports Cooperative Kernel Launch: Yes
  Supports MultiDevice Co-op Kernel Launch: No
  Device PCI Domain ID / Bus ID / Device ID: 0 / 1 / 0
  Compute Mode:
     < Default (multiple host threads can use ::cudaSetDevice() with device simultaneously) >
```

## Environment Variables

### Essential

```bash
# Linux/macOS
export CUDA_HOME=/usr/local/cuda
export PATH=$CUDA_HOME/bin:$PATH
export LD_LIBRARY_PATH=$CUDA_HOME/lib64:$LD_LIBRARY_PATH

# Windows (PowerShell)
$env:CUDA_PATH = "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8"
$env:Path += ";$env:CUDA_PATH\bin"
```

### Optional (Performance Tuning)

```bash
# GPU persistence mode (Linux, requires sudo)
sudo nvidia-smi -pm 1

# Set GPU clocks to maximum (Linux, requires sudo)
sudo nvidia-smi -pm 1
sudo nvidia-smi -lgc 1590  # Lock GPU clock to 1590 MHz

# Monitor GPU
nvidia-smi  # Every 1 second
nvidia-smi dmon  # Detailed metrics
```

## Troubleshooting

### "CUDA: The end of the file was unexpected error"

**Solution:**
- Update NVIDIA driver
- Reinstall CUDA toolkit
- Check CUDA version compatibility

### "CUDA capability is insufficient for this operation"

**Cause:** GPU Compute Capability < 7.0

**Solution:**
```bash
# Check compute capability
deviceQuery | grep "Compute Capability"

# If < 7.0, upgrade GPU or adjust CMake target
cmake -DCMAKE_CUDA_ARCHITECTURES="60;61" ..  # For older GPUs
```

### "Driver/runtime version mismatch"

**Solution:**
```bash
# Update GPU driver to latest
# https://www.nvidia.com/Download/driverDetails.aspx

# Verify versions match
nvcc --version
deviceQuery | grep "Runtime Version"
```

### "Out of memory" During Build

**Solution:**
```bash
# Reduce parallel jobs
cmake --build . -j 2  # Instead of -j 8

# Or set environment variable
export CUDA_LAUNCH_BLOCKING=1
```

### Multi-GPU System Issues

```bash
# List all GPUs
deviceQuery

# Set default GPU (0-indexed)
export CUDA_VISIBLE_DEVICES=0

# Use specific GPU in code
scudaSetDevice(0);
```

## Performance Optimization

### GPU Persistence

```bash
# Linux/Windows (requires admin)
nvidia-smi -pm 1
```

### Memory Optimization

```cpp
// Use pinned memory for faster transfers
cudaMallocHost(&hostPtr, size);
cudaMemcpy(devicePtr, hostPtr, size, cudaMemcpyHostToDevice);
cudaFreeHost(hostPtr);
```

### Kernel Optimization

```cpp
// Use CUDA Occupancy Calculator
// Aim for 50-75% GPU occupancy
// Register usage < 128 per thread
```

## Multi-GPU Setup (Future)

### NVIDIA NVLink Configuration

```bash
# Check NVLink status
nvcc --version | grep NVLink

# Enable peer-to-peer access
cudaEnablePeerAccess(device2, 0);
```

## Learning Resources

- **NVIDIA CUDA Documentation**: https://docs.nvidia.com/cuda/
- **NVIDIA Developer Forum**: https://forums.developer.nvidia.com/c/cuda/
- **CUDA C++ Programming Guide**: https://docs.nvidia.com/cuda/cuda-c-programming-guide/
- **NVIDIA GPU Bootcamp**: https://www.nvidia.com/en-us/gpu-bootcamp/

## Next Steps

Once CUDA is set up, proceed to [BUILD.md](BUILD.md) for plugin compilation.
