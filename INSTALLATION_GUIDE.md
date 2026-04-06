# SS-TOOLS IMPROVED v2.0 - Installation & Usage Guide

## Quick Start (Pre-built Executable)

### 1. System Requirements
- **Windows 10/11** (Build 19041 or later)
- **Administrator privileges** (recommended for all features)
- **4 GB RAM** minimum
- **500 MB disk space**

### 2. Download & Run

```bash
# Extract ss-tools-improved.zip
# Open command prompt as Administrator
cd path\to\ss-tools-improved

# Run quick scan
ss-tools-improved.exe --quick

# Run full scan
ss-tools-improved.exe --full

# Interactive menu
ss-tools-improved.exe
```

## Building from Source

### Prerequisites
1. **Visual Studio 2019+** with C++ support
2. **CMake 3.16+**
3. **Windows SDK**

### Build Steps

```bash
# 1. Navigate to project directory
cd ss-tools-improved

# 2. Create build directory
mkdir build && cd build

# 3. Configure with CMake
cmake .. -G "Visual Studio 17 2022" -A x64

# 4. Build
cmake --build . --config Release

# 5. Run executable
.\bin\Release\ss-tools-improved.exe --help
```

## Usage Guide

### Command-Line Interface

```bash
# Quick scan (processes, task manager, DLL scan)
ss-tools-improved.exe --quick

# Full comprehensive scan (all scanners)
ss-tools-improved.exe --full

# Kernel-level scan only
ss-tools-improved.exe --kernel

# Process Hacker detection
ss-tools-improved.exe --process-hacker

# Show help
ss-tools-improved.exe --help
```

### Interactive Menu

```
=== SS-TOOLS IMPROVED - MAIN MENU ===
1. Quick Scan (Fast)
2. Full Scan (Comprehensive)
3. Kernel Scan (Kernel-level only)
4. Process Scan (Running processes)
5. Check Process Hacker
6. Export Results
7. Exit
```

**Selection**: Type number (1-7) and press Enter

## Understanding Results

### Severity Levels

**🔴 CRITICAL** - Definite threat detected
- High confidence malware/cheat
- Requires immediate action
- Examples: Wurst client running, Process Hacker kernel driver loaded

**🟠 HIGH** - Likely threat
- Strong indicators of malicious activity
- Should investigate
- Examples: Cheat Engine installed, suspicious kernel driver

**🟡 MEDIUM** - Possible threat
- Some suspicious indicators
- May need investigation
- Examples: Generic suspicious DLL, unknown process

**🔵 LOW** - Low priority threat
- Uncertain indicator
- Low confidence
- Examples: Generic keyword match, legitimate mod

**🟢 NONE** - Filtered as false positive
- Context indicates legitimate file
- Whitelisted or confidence too low
- Examples: OptiFine mod, development tools

### Confidence Scores

Displayed as percentage (0-100%):
- **90-100%**: Very High Confidence
- **70-89%**: High Confidence
- **50-69%**: Medium Confidence
- **30-49%**: Low Confidence
- **0-29%**: Very Low Confidence (likely false positive)

### False Positives

Items marked as "FALSE POSITIVE" include:
- Reason for false positive classification
- Context analysis result
- Why it was filtered
- Confidence score showing why it was downgraded

## Interpreting Detections

### Example Detection Output

```
[DETECTION]
Scanner:    Kernel Driver Scanner
File:       dbk64
Desc:       Suspicious kernel driver detected: Cheat Engine kernel driver (64-bit)
Context:    Driver: dbk64
Severity:   CRITICAL
Confidence: 95.0%
```

**This means**:
- Cheat Engine kernel driver is loaded on the system
- Very high confidence this is a real threat
- Action: Should be removed if not legitimately needed

### Example False Positive

```
[DETECTION]
Scanner:    DLL Scanner
File:       C:\Program Files\OptiFine\speed.dll
Desc:       Cheat keyword found: speed
Context:    Category: Rendering | Context: optimization rendering
Severity:   LOW
Confidence: 25.0%
[FALSE POSITIVE] Context analysis suggests false positive
```

**This means**:
- Keyword "speed" found but in optimization context
- OptiFine mod detected (whitelisted)
- Low confidence score
- Automatically filtered as false positive
- No action needed

## Best Practices

### 1. Scan Frequency
- **Weekly**: Run quick scan
- **Monthly**: Run full comprehensive scan
- **Post-Update**: Scan after system updates
- **Suspected Activity**: Run immediately on suspicion

### 2. Administrator Privileges
```bash
# Run as Administrator (PowerShell)
Start-Process -FilePath ".\ss-tools-improved.exe" -Verb RunAs

# Or via Command Prompt
# Right-click cmd.exe → Run as administrator
```

### 3. Interpreting Results

- **0 Detections**: System appears clean
- **Few Detections (< 5)**: Review context, likely many false positives
- **Multiple Detections (5-20)**: Investigate each one
- **Many Detections (> 20)**: System likely compromised, multiple threats

### 4. Action on Detections

```
CRITICAL Detections:
1. Quarantine the file
2. Remove the threat
3. Run scan again to verify

HIGH Detections:
1. Review context
2. Research the tool
3. Remove if suspicious
4. Verify with other tools

MEDIUM Detections:
1. Gather more information
2. Check confidence score
3. Investigate if needed

LOW Detections:
1. Usually false positives
2. Check false positive reason
3. Add to whitelist if legitimate
```

## Troubleshooting

### "Permission Denied" Error

**Cause**: Insufficient privileges
**Solution**:
```bash
# Run as Administrator
Right-click CMD → Run as administrator
ss-tools-improved.exe --full
```

### Scan Taking Too Long

**Cause**: System has many files
**Solution**:
- Use Quick Scan instead of Full Scan
- Close unnecessary applications
- Disable antivirus temporarily (if safe)

### False Positives Appearing

**Cause**: Context analysis too aggressive
**Solution**:
- Check confidence scores (< 50% = likely false positive)
- Review context provided
- Add to whitelist if confirmed legitimate

### Process Detection Not Working

**Cause**: Missing elevated privileges
**Solution**:
```bash
# Restart as Administrator
# Some process information requires admin rights
```

## Advanced Usage

### Custom Scanning

Modify `main.cpp` to create custom scanner combinations:

```cpp
// Run only Process Hacker detection
auto& detector = ProcessHackerDetector::Instance();
auto result = detector.DetectProcessHackerThreat(globalScanState);
```

### Adding Whitelisted Mods

```cpp
WhitelistEntry entry;
entry.name = "my-safe-mod";
entry.keywords = {"keyword1", "keyword2"};
entry.contexts = {"legitimate", "context"};
entry.maxSeverity = Severity::Low;
entry.isLegitimate = true;

FalsePositiveFilter::Instance().AddWhitelistEntry("my-safe-mod", entry);
```

### Export Results

```bash
# Results are automatically exported after scan
# Find in output directory:
ss-tools-results.json    # JSON format
ss-tools-results.csv     # CSV format
ss-tools-results.html    # HTML report
```

## Performance Tips

### Speed Up Scanning

1. **Use Quick Scan**
   - Fastest option (~5-15 seconds)
   - Covers most critical areas
   - Good for routine checks

2. **Run During Off-Hours**
   - Less interference from other processes
   - More consistent results
   - Better system performance

3. **Close Unnecessary Apps**
   - Reduces system load
   - Faster file access
   - Cleaner detection results

### Improve Detection

1. **Run Full Scan**
   - More comprehensive
   - Better coverage
   - ~30-60 seconds

2. **Run Scan Regularly**
   - Catch new threats early
   - Build historical data
   - Trend analysis

3. **Run on Clean System**
   - Fewer competing processes
   - More accurate results
   - Better baseline

## Reporting Results

### Generate Report

```bash
# After scan completes
# Choose option 6: Export Results

# Creates three files:
ss-tools-report-YYYYMMDD-HHMMSS.json
ss-tools-report-YYYYMMDD-HHMMSS.csv
ss-tools-report-YYYYMMDD-HHMMSS.html
```

### Report Contents

- Scan timestamp
- Summary statistics
- Detailed findings
- Severity breakdown
- Confidence scores
- False positive annotations

## Getting Help

### Built-in Help

```bash
ss-tools-improved.exe --help
```

### Common Questions

**Q: Is this antivirus software?**
A: No, it's specifically designed for cheat/hack detection

**Q: Does it remove threats automatically?**
A: No, it detects and reports. You must remove manually.

**Q: Can it detect all cheats?**
A: No tool is 100% effective. It catches most common cheats.

**Q: Is it safe to run?**
A: Yes, it only reads system information. It doesn't modify anything.

**Q: Does it work on game servers?**
A: Yes, perfect for server-side cheat detection.

## Support

For issues or questions:
1. Check FEATURES.md for capability details
2. Review BUILD_INSTRUCTIONS.md for build issues
3. Check README.md for general information

---

**Version**: 2.0.0  
**Last Updated**: 2024  
**Platform**: Windows 10/11 64-bit
