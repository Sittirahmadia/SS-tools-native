# SS-TOOLS IMPROVED - Build Instructions

## System Requirements

### Minimum Requirements
- **OS**: Windows 10 (Build 19041) or Windows 11
- **Architecture**: 64-bit (x64)
- **RAM**: 4 GB
- **Disk Space**: 500 MB for build artifacts

### Development Environment
- **Visual Studio 2019 Community** or later
- **CMake 3.16+**
- **Windows 10/11 SDK**
- **Git** (optional, for version control)

## Step-by-Step Build Guide

### 1. Install Build Tools

#### Option A: Using Visual Studio Installer

1. Download Visual Studio 2022 Community Edition
2. Run the installer
3. Select "Desktop development with C++"
4. Ensure the following are checked:
   - MSVC v143 or later
   - Windows 10/11 SDK
   - CMake tools for Windows
5. Complete installation

#### Option B: Command Line (PowerShell as Admin)

```powershell
# Install Visual Studio Build Tools
# Download from: https://visualstudio.microsoft.com/downloads/
# Select "Desktop development with C++"
```

### 2. Install CMake

#### Option A: Using Chocolatey
```powershell
choco install cmake
```

#### Option B: Manual Installation
1. Download from: https://cmake.org/download/
2. Run installer
3. Add CMake to PATH during installation

### 3. Prepare Source Code

```bash
# Clone or extract SS-TOOLS source
cd ss-tools-improved

# Create build directory
mkdir build
cd build
```

### 4. Configure Build

```bash
# Basic configuration (Release build, all features enabled)
cmake .. -G "Visual Studio 17 2022" -A x64

# Or with options
cmake .. -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DENABLE_KERNEL_SCAN=ON ^
  -DENABLE_MEMORY_SCAN=ON ^
  -DENABLE_PROCESS_SCAN=ON
```

### 5. Build

```bash
# Build in Release configuration
cmake --build . --config Release

# Build with verbose output
cmake --build . --config Release --verbose

# Build with multiple threads
cmake --build . --config Release -j 4
```

### 6. Verify Build

```bash
# Check if executable was created
ls bin\Release\ss-tools-improved.exe

# Test basic functionality
.\bin\Release\ss-tools-improved.exe --help
```

## Build Output Structure

After successful build:

```
ss-tools-improved/
├── build/
│   ├── bin/
│   │   └── Release/
│   │       └── ss-tools-improved.exe      ← Main executable
│   ├── lib/                                ← Static libraries
│   └── CMakeFiles/                         ← Build artifacts
├── include/                                ← Header files
├── src/                                    ← Source files
└── resources/                              ← Icons, assets
```

## Build Configurations

### Release Build (Recommended for Production)

```bash
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

**Characteristics**:
- Optimized for performance
- Minimal debug information
- Smaller executable size
- Suitable for deployment

### Debug Build (For Development)

```bash
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```

**Characteristics**:
- Full debug symbols
- Larger executable
- Slower performance
- Better for debugging

## Advanced Build Options

### Enable/Disable Specific Features

```bash
# Disable kernel scanning
cmake .. -DENABLE_KERNEL_SCAN=OFF

# Build with test suite
cmake .. -DBUILD_TESTS=ON

# Custom installation prefix
cmake .. -DCMAKE_INSTALL_PREFIX=C:/SS-Tools-Install
```

### Compiler Optimization Levels

```bash
# Maximum optimization (slower build, faster execution)
cmake .. -DCMAKE_CXX_FLAGS="/O2 /Oi /Ot /GL"

# Balanced optimization
cmake .. -DCMAKE_CXX_FLAGS="/O2"

# No optimization (faster build, slower execution)
cmake .. -DCMAKE_CXX_FLAGS="/O0"
```

## Troubleshooting Build Issues

### Issue: "CMake not found"

**Solution**:
```bash
# Add CMake to PATH
setx PATH "%PATH%;C:\Program Files\CMake\bin"

# Restart terminal and verify
cmake --version
```

### Issue: "Visual Studio not found"

**Solution**:
```bash
# Find Visual Studio installation
"C:\Program Files\Microsoft Visual Studio\Installer\vswhere.exe"

# Or manually specify generator
cmake .. -G "Visual Studio 17 2022"
```

### Issue: "Windows SDK not found"

**Solution**:
1. Open Visual Studio Installer
2. Select your Visual Studio installation
3. Click "Modify"
4. Check "Windows 10/11 SDK"
5. Complete installation

### Issue: Build fails with linker errors

**Solution**:
```bash
# Clean build
cmake --build . --config Release --clean-first

# Or manually clean
rm -r build\*
cmake ..
cmake --build . --config Release
```

### Issue: "Out of memory" during build

**Solution**:
```bash
# Limit parallel build jobs
cmake --build . --config Release -j 2
```

## Installation

### Local Installation

```bash
# Install to default location
cmake --install . --config Release

# Install to custom location
cmake --install . --config Release --prefix C:/SS-Tools-Install
```

### Create Portable Executable

```bash
# Copy executable to new directory
mkdir ss-tools-portable
copy bin\Release\ss-tools-improved.exe ss-tools-portable\

# Done! ss-tools-portable can be moved anywhere
```

## First Run

### Quick Test

```bash
# Navigate to build output
cd build\bin\Release

# Run quick scan
.\ss-tools-improved.exe --quick

# Run full scan
.\ss-tools-improved.exe --full

# Check Process Hacker
.\ss-tools-improved.exe --process-hacker
```

### Administrative Privileges

Some features require administrator privileges:

```bash
# Run as Administrator (PowerShell)
Start-Process -FilePath ".\ss-tools-improved.exe" -Verb RunAs

# Or use Windows Run dialog
Win + R → cmd → right-click → "Run as administrator"
```

## Performance Optimization

### Build Time Optimization

```bash
# Use precompiled headers
cmake .. -DENABLE_PCH=ON

# Use ccache for faster rebuilds (if available)
cmake .. -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
```

### Runtime Optimization

For maximum performance:

1. Use Release build
2. Enable all optimizations
3. Disable debug logging
4. Use Quick Scan for routine checks

## Build Verification Checklist

- [ ] Visual Studio 2019+ installed
- [ ] CMake 3.16+ installed
- [ ] Windows SDK available
- [ ] Source code extracted
- [ ] Build directory created
- [ ] CMake configuration successful
- [ ] Build completed without errors
- [ ] Executable runs with `--help`
- [ ] Quick scan completes
- [ ] Process detection works

## Next Steps

1. **Configuration**: Adjust settings in config files
2. **Testing**: Run test suite if built
3. **Deployment**: Copy executable to target systems
4. **Updates**: Rebuild when source code changes

## Support & Troubleshooting

For build issues:

1. Check that all prerequisites are installed
2. Verify Windows SDK version compatibility
3. Clean build directory and retry
4. Check Visual Studio for updates
5. Review CMake configuration output

## Additional Resources

- CMake Documentation: https://cmake.org/cmake/help/latest/
- Visual Studio Downloads: https://visualstudio.microsoft.com/downloads/
- Windows SDK: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/
- C++ Standard Library: https://learn.microsoft.com/en-us/cpp/

---

**Build System**: CMake 3.16+  
**Compiler**: MSVC v143 or later  
**Platform**: Windows 10/11 64-bit
