#pragma once

#include "core.h"
#include <vector>
#include <string>

namespace SSTools {
namespace Scanning {

// ═══════════════════════════════════════════════════════════════════════════
// TEMP & PREFETCH SCANNER
// ═══════════════════════════════════════════════════════════════════════════

class TempPrefetchScanner {
public:
    static TempPrefetchScanner& Instance();
    
    // Scan Windows Temp folder for suspicious files
    ScanResult ScanTempFolder(ScanState& state);
    
    // Scan Prefetch folder for execution history
    ScanResult ScanPrefetchFolder(ScanState& state);
    
    // Scan Recent files for suspicious activity
    ScanResult ScanRecentFiles(ScanState& state);
    
    // Comprehensive temp/prefetch scan
    ScanResult FullTempPrefetchScan(ScanState& state);
    
    // Get MRU (Most Recently Used) files
    std::vector<std::string> GetMRUFiles();
    
    // Check for suspicious temp file patterns
    bool IsSuspiciousTempFile(const std::string& fileName);

private:
    TempPrefetchScanner() = default;
    ~TempPrefetchScanner() = default;
    
    std::string GetTempPath();
    std::string GetPrefetchPath();
    std::string GetRecentFilesPath();
    
    static const std::vector<std::string> SUSPICIOUS_TEMP_PATTERNS;
};

// ═══════════════════════════════════════════════════════════════════════════
// ENHANCED PROCESS SCANNER
// ═══════════════════════════════════════════════════════════════════════════

class EnhancedProcessScanner {
public:
    static EnhancedProcessScanner& Instance();
    
    // Scan all running processes
    ScanResult ScanRunningProcesses(ScanState& state);
    
    // Detect hidden processes (using various techniques)
    ScanResult DetectHiddenProcesses(ScanState& state);
    
    // Scan process command lines for suspicious arguments
    ScanResult ScanProcessCommandLines(ScanState& state);
    
    // Check for process manipulation (tampering indicators)
    ScanResult DetectProcessTampering(ScanState& state);
    
    // Get detailed process information
    ProcessInfo GetProcessInfo(uint32_t pid);
    
    // Get all processes with command lines
    std::vector<ProcessInfo> GetAllProcesses();
    
    // Check if process is suspicious
    bool IsProcessSuspicious(const ProcessInfo& proc);

private:
    EnhancedProcessScanner() = default;
    ~EnhancedProcessScanner() = default;
    
    std::vector<ProcessInfo> GetProcessesFromTasklist();
    std::vector<ProcessInfo> GetProcessesFromWMIC();
    std::vector<uint32_t> GetProcessesFromSnapshot();
    
    bool IsProcessHiddenFromTasklist(uint32_t pid);
    bool IsSuspiciousProcessName(const std::string& name);
    
    static const std::vector<std::string> SUSPICIOUS_PROCESS_NAMES;
    static const std::vector<std::string> SUSPICIOUS_COMMAND_PATTERNS;
};

// ═══════════════════════════════════════════════════════════════════════════
// ADVANCED REGISTRY SCANNER
// ═══════════════════════════════════════════════════════════════════════════

class RegistryScanner {
public:
    static RegistryScanner& Instance();
    
    // Scan for cheat-related registry entries
    ScanResult ScanSuspiciousRegistry(ScanState& state);
    
    // Check for process hiding registry modifications
    ScanResult DetectProcessHidingRegistry(ScanState& state);
    
    // Check for DLL injection registry entries
    ScanResult DetectInjectionRegistry(ScanState& state);
    
    // Scan shell extensions for injection points
    ScanResult ScanShellExtensions(ScanState& state);
    
    // Check for unauthorized kernel drivers
    ScanResult CheckUnauthorizedDrivers(ScanState& state);
    
    // Get registry modification history
    std::vector<std::string> GetRecentRegistryModifications();

private:
    RegistryScanner() = default;
    ~RegistryScanner() = default;
    
    bool ReadRegistryDWORD(HKEY hKey, const std::string& path, const std::string& value, DWORD& result);
    std::string ReadRegistryString(HKEY hKey, const std::string& path, const std::string& value);
    
    static const std::vector<std::pair<std::string, std::string>> SUSPICIOUS_REGISTRY_PATHS;
};

// ═══════════════════════════════════════════════════════════════════════════
// DLL & BINARY ANALYZER
// ═══════════════════════════════════════════════════════════════════════════

class DLLAnalyzer {
public:
    static DLLAnalyzer& Instance();
    
    // Scan directory for suspicious DLLs
    ScanResult ScanDLLDirectory(const std::string& dirPath, ScanState& state);
    
    // Analyze single DLL file
    ScanResult AnalyzeDLL(const std::string& filePath);
    
    // Extract and analyze strings from binary
    std::vector<std::string> ExtractStringsFromBinary(const std::string& filePath);
    
    // Detect injection patterns in binary
    bool HasInjectionPatterns(const std::string& filePath);
    
    // Check for suspicious imports
    std::vector<std::string> GetSuspiciousImports(const std::string& filePath);
    
    // Scan AppData for suspicious DLLs
    ScanResult ScanAppDataForDLLs(ScanState& state);

private:
    DLLAnalyzer() = default;
    ~DLLAnalyzer() = default;
    
    bool IsSuspiciousDLLName(const std::string& name);
    bool HasSuspiciousImports(const std::vector<std::string>& imports);
    
    static const std::vector<std::string> DANGEROUS_IMPORTS;
    static const std::vector<std::string> SUSPICIOUS_DLL_PATTERNS;
};

// ═══════════════════════════════════════════════════════════════════════════
// ENHANCED MOD SCANNER
// ═══════════════════════════════════════════════════════════════════════════

class EnhancedModScanner {
public:
    static EnhancedModScanner& Instance();
    
    // Scan all launcher instances and mods
    ScanResult ScanAllMods(ScanState& state);
    
    // Scan specific mod folder
    ScanResult ScanModFolder(const std::string& folderPath, ScanState& state);
    
    // Analyze single mod file
    ScanResult AnalyzeMod(const std::string& filePath);
    
    // Extract metadata from mod
    std::map<std::string, std::string> ExtractModMetadata(const std::string& filePath);
    
    // Check if mod is whitelisted
    bool IsModWhitelisted(const std::string& modName);
    
    // Get list of all launcher instances
    std::vector<std::string> GetLauncherInstances();

private:
    EnhancedModScanner() = default;
    ~EnhancedModScanner() = default;
    
    std::string ExtractStringsFromJAR(const std::string& jarPath);
    bool IsModSignedCorrectly(const std::string& jarPath);
    
    static const std::vector<std::string> LEGITIMATE_MODS;
};

// ═══════════════════════════════════════════════════════════════════════════
// CHROME & BROWSER HISTORY SCANNER
// ═══════════════════════════════════════════════════════════════════════════

class BrowserScanner {
public:
    static BrowserScanner& Instance();
    
    // Scan Chrome history for cheat downloads/searches
    ScanResult ScanChromeHistory(ScanState& state);
    
    // Scan Firefox history
    ScanResult ScanFirefoxHistory(ScanState& state);
    
    // Scan Edge history
    ScanResult ScanEdgeHistory(ScanState& state);
    
    // Scan browser downloads
    ScanResult ScanBrowserDownloads(ScanState& state);
    
    // Get search history for specific queries
    std::vector<std::string> GetSearchHistory(const std::string& browser);

private:
    BrowserScanner() = default;
    ~BrowserScanner() = default;
    
    static const std::vector<std::string> SUSPICIOUS_URLS;
    static const std::vector<std::string> SUSPICIOUS_SEARCH_QUERIES;
};

} // namespace Scanning
} // namespace SSTools
