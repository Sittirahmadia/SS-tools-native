# SS-TOOLS IMPROVED v2.0

Advanced Anti-Cheat Scanner with Kernel-Level Detection & ImGUI Interface

## 🎯 Features

### Core Detection Capabilities

- **Kernel-Level Scanning**
  - Loaded driver enumeration and analysis
  - Device Guard (HVCI) status checking
  - Secure Boot verification
  - Suspicious kernel service detection
  - Kernel hook pattern analysis
  - Rootkit detection indicators

- **Memory Injection Detection**
  - DLL injection via VirtualAllocEx/WriteProcessMemory
  - Java agent injection (-javaagent flags)
  - Process hollowing detection
  - Inline hooks detection
  - Suspicious memory allocation patterns

- **Process Hacker Detection**
  - Installation detection across common locations
  - Running process detection
  - Kernel driver presence check (kprocesshacker)
  - Real-time monitoring capability

- **Advanced Process Scanning**
  - Running process enumeration
  - Hidden process detection (snapshot vs tasklist comparison)
  - Process command line analysis
  - Suspicious argument pattern matching
  - Process tampering indicators

- **Temp & Prefetch Scanning**
  - Windows Temp folder analysis
  - Prefetch execution history examination
  - Recent files tracking
  - MRU (Most Recently Used) file detection
  - Suspicious temporary file patterns

- **DLL & Binary Analysis**
  - Directory-based DLL scanning
  - Binary string extraction and analysis
  - Dangerous API import detection
  - Injection pattern recognition
  - AppData suspicious DLL detection

- **Task Manager Monitoring**
  - Disabled status detection
  - Registry tampering identification
  - Process hiding technique detection
  - Windows hook injection checks
  - AppInit_DLLs analysis

### Advanced Detection Features

- **False Positive Filtering**
  - Context-aware detection analysis
  - Legitimate mod whitelist database
  - Development tool context detection
  - Metadata analysis (fabric.json, mods.toml)
  - Confidence scoring system (0.0-1.0)

- **Context Analysis**
  - Keyword extraction and analysis
  - Surrounding context evaluation
  - Development vs cheat context distinction
  - File location risk assessment
  - Binary signature analysis

- **Severity Adjustment**
  - Dynamic severity scoring based on context
  - False positive downgrading
  - Whitelisted mod handling
  - Clear cheat context upgrading
  - Confidence-based filtering

### User Interface

- **ImGUI-based Modern Interface**
  - Real-time scan progress visualization
  - Detection result browsing
  - Severity-based filtering
  - Scanner-specific result viewing
  - Process list display
  - Kernel driver enumeration view

- **Report Generation**
  - JSON export format
  - CSV export format
  - HTML report generation
  - Timestamped result archiving
  - Executive summary creation

## 🚀 Quick Start

### Command Line Usage

```bash
# Quick scan (fast)
ss-tools-improved.exe --quick

# Full comprehensive scan
ss-tools-improved.exe --full

# Kernel-level scan only
ss-tools-improved.exe --kernel

# Process Hacker detection only
ss-tools-improved.exe --process-hacker

# Show help
ss-tools-improved.exe --help
```

### Interactive Menu

```bash
ss-tools-improved.exe
```

Then choose from:
1. Quick Scan (Fast)
2. Full Scan (Comprehensive)
3. Kernel Scan (Kernel-level only)
4. Process Scan (Running processes)
5. Check Process Hacker
6. Export Results
7. Exit

## 🔧 Building

### Requirements

- Windows 10/11
- Visual Studio 2019 or later
- CMake 3.16+
- Windows SDK

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build . --config Release

# The executable will be in bin/Release/ss-tools-improved.exe
```

### Build Options

```cmake
# Enable/disable specific scanners
-DENABLE_KERNEL_SCAN=ON      # Kernel-level scanning
-DENABLE_MEMORY_SCAN=ON      # Memory injection detection
-DENABLE_PROCESS_SCAN=ON     # Process scanning
-DBUILD_TESTS=OFF            # Test suite
```

## 📊 Detection Accuracy

### Improved False Positive Reduction

- **Whitelist Database**: 10+ legitimate mods pre-configured
- **Context Analysis**: Multi-factor context evaluation
- **Confidence Scoring**: 0.0-1.0 confidence range
- **Development Tool Detection**: Automatic legitimate tool identification
- **Metadata Analysis**: fabric.json and mods.toml parsing

### Known Legitimate Mods (Whitelisted)

- OptiFine
- Sodium
- Lithium
- Ferrocene
- Starlight
- WorldEdit
- JEI (Just Enough Items)
- REI (Roughly Enough Items)
- WAILA / HWYLA
- Forge / Fabric APIs

## 🔐 Security Features

### Kernel-Level Protections

- **HVCI (Hypervisor Code Integrity) Monitoring**
  - Checks if Device Guard is enabled
  - Alerts on HVCI disabled state
  - Warns about vulnerability to kernel injection

- **Secure Boot Verification**
  - Validates UEFI Secure Boot status
  - Detects disabled signatures
  - Identifies unsigned driver risks

- **Kernel Service Scanning**
  - Enumerates all kernel drivers
  - Checks service registry entries
  - Identifies suspicious service types
  - Analyzes ImagePath entries

### Memory Protection

- **Injection Vector Detection**
  - CreateRemoteThread monitoring
  - WriteProcessMemory detection
  - VirtualAllocEx tracking
  - NtWriteVirtualMemory analysis

- **Process Integrity**
  - Memory allocation pattern analysis
  - Suspicious module loading detection
  - DLL injection vector identification
  - Java agent injection scanning

### Process Monitoring

- **Hidden Process Detection**
  - Comparison between snapshot and tasklist
  - Multi-method process enumeration
  - Rootkit-style hiding detection
  - TaskManager bypass identification

- **Process Command Line Analysis**
  - Java agent flag detection (-javaagent, -agentlib)
  - DLL injection parameter scanning
  - Suspicious argument pattern matching
  - Environment variable analysis

## 📈 Performance

- **Quick Scan**: ~5-15 seconds
  - Process scanning
  - Task Manager check
  - Basic DLL scan

- **Full Scan**: ~30-60 seconds
  - All scanners running
  - Comprehensive analysis
  - False positive filtering
  - Result generation

- **Optimizations**
  - Parallel file scanning
  - Cached kernel information
  - Efficient string extraction
  - Smart directory traversal

## 🐛 Troubleshooting

### Elevated Privileges Required

Some features require administrator privileges:
- Kernel driver enumeration
- Process information retrieval
- Registry access
- System driver scanning

**Solution**: Run as Administrator

### Slow Scanning

If scanning is slow:
- Close unnecessary applications
- Disable antivirus temporarily
- Reduce folder scan depth
- Use Quick Scan instead of Full Scan

### False Positives

If legitimate mods are flagged:
1. Check confidence score (< 0.5 = likely false positive)
2. Add to whitelist if safe
3. Report to development team
4. Use custom scan with selected scanners

## 📋 Scan Result Interpretation

### Severity Levels

- **CRITICAL** (Red): Definite threat, high confidence
- **HIGH** (Orange): Likely threat, action needed
- **MEDIUM** (Yellow): Possible threat, investigate
- **LOW** (Blue): Low priority threat, low confidence
- **NONE** (Green): Filtered as false positive

### Confidence Scores

- **0.9-1.0**: Very High - Likely Cheat
- **0.7-0.9**: High - Probably Cheat
- **0.5-0.7**: Medium - Possible Cheat
- **0.3-0.5**: Low - Uncertain
- **0.0-0.3**: Very Low - Likely False Positive

## 🔄 Updates & Maintenance

### Signature Updates

The tool includes:
- Hardcoded cheat client signatures
- Macro tool patterns
- Injection technique indicators
- Known malware signatures

Updates can be applied by:
1. Rebuilding from updated source
2. Deploying new executable
3. Updating signature database

### Whitelisting

To add legitimate files to whitelist:

```cpp
WhitelistEntry entry;
entry.name = "my-mod";
entry.keywords = {"keyword1", "keyword2"};
entry.contexts = {"context1", "context2"};
entry.maxSeverity = Severity::Low;
entry.isLegitimate = true;
FalsePositiveFilter::Instance().AddWhitelistEntry("my-mod", entry);
```

## 📝 Output Formats

### JSON Export

```json
{
  "scan_time": "2024-01-01T12:00:00Z",
  "total_detections": 5,
  "critical": 2,
  "high": 1,
  "medium": 2,
  "results": [...]
}
```

### HTML Report

Comprehensive formatted report with:
- Executive summary
- Detection statistics
- Detailed findings
- Severity breakdown
- Timeline information

### CSV Export

Spreadsheet-compatible format with:
- Scanner name
- File path
- Description
- Severity
- Confidence
- Timestamp

## ⚖️ Legal & Ethical

This tool is designed for:
- Server administrators detecting cheats on game servers
- Anti-cheat system development and testing
- Security research and analysis
- System integrity verification

This tool should NOT be used for:
- Unauthorized system monitoring
- Privacy violation
- Malicious purposes
- Circumventing security measures

## 🤝 Contributing

Report issues and suggest improvements:
- Bug reports
- False positive submissions
- Signature updates
- Performance improvements

## 📄 License

SS-TOOLS Improved v2.0
Advanced Anti-Cheat Scanner

---

**Version**: 2.0.0  
**Last Updated**: 2024  
**Platform**: Windows 10/11 (64-bit)  
**Status**: Production Ready
