#include "../include/advanced_scanners.h"
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <shlobj.h>
#include <algorithm>
#include <regex>

namespace fs = std::filesystem;

namespace SSTools {
namespace Scanning {

// ═══════════════════════════════════════════════════════════════════════════
// TEMP & PREFETCH SCANNER
// ═══════════════════════════════════════════════════════════════════════════

const std::vector<std::string> TempPrefetchScanner::SUSPICIOUS_TEMP_PATTERNS = {
    "cheat", "hack", "inject", "bypass", "wurst", "meteor", "impact",
    "198macro", "zenithmacro", "exploit", "rootkit", "hidden"
};

TempPrefetchScanner& TempPrefetchScanner::Instance() {
    static TempPrefetchScanner instance;
    return instance;
}

std::string TempPrefetchScanner::GetTempPath() {
    char tempPath[MAX_PATH];
    if (GetTempPathA(MAX_PATH, tempPath)) {
        return std::string(tempPath);
    }
    return "C:\\Windows\\Temp";
}

std::string TempPrefetchScanner::GetPrefetchPath() {
    return "C:\\Windows\\Prefetch";
}

std::string TempPrefetchScanner::GetRecentFilesPath() {
    char appdata[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_RECENT, NULL, 0, appdata))) {
        return std::string(appdata);
    }
    return "";
}

ScanResult TempPrefetchScanner::ScanTempFolder(ScanState& state) {
    ScanResult result;
    result.scannerName = "Temp Folder Scanner";
    result.status = "clean";

    std::string tempPath = GetTempPath();
    state.SetActivity("Scanning Temp folder: " + tempPath);

    if (!fs::exists(tempPath)) {
        result.error = "Temp folder not found";
        return result;
    }

    try {
        for (const auto& entry : fs::directory_iterator(tempPath)) {
            if (state.IsCancelRequested()) break;
            
            std::string fileName = entry.path().filename().string();
            std::string fileLower = fileName;
            std::transform(fileLower.begin(), fileLower.end(), fileLower.begin(), ::tolower);

            if (IsSuspiciousTempFile(fileLower)) {
                Detection det;
                det.scanner = "Temp Folder Scanner";
                det.filePath = entry.path().string();
                det.description = "Suspicious file in Temp folder";
                det.context = "File: " + fileName;
                det.severity = Severity::High;
                det.timestamp = std::to_string(std::time(nullptr));
                det.confidence = 0.7f;

                result.detections.push_back(det);
                result.totalDetected++;
            }

            result.totalScanned++;
            state.scannedFiles++;
        }
    } catch (...) {
        result.error = "Error scanning temp folder";
    }

    if (!result.detections.empty()) {
        result.status = "detected";
    }

    return result;
}

ScanResult TempPrefetchScanner::ScanPrefetchFolder(ScanState& state) {
    ScanResult result;
    result.scannerName = "Prefetch Scanner";
    result.status = "clean";

    std::string prefetchPath = GetPrefetchPath();
    state.SetActivity("Scanning Prefetch folder: " + prefetchPath);

    if (!fs::exists(prefetchPath)) {
        result.error = "Prefetch folder not found";
        return result;
    }

    try {
        for (const auto& entry : fs::directory_iterator(prefetchPath)) {
            if (state.IsCancelRequested()) break;
            if (!entry.is_regular_file()) continue;

            std::string fileName = entry.path().filename().string();
            std::string fileLower = fileName;
            std::transform(fileLower.begin(), fileLower.end(), fileLower.begin(), ::tolower);

            // Prefetch files are .pf files that record execution history
            if (fileLower.find("cheat") != std::string::npos ||
                fileLower.find("hack") != std::string::npos ||
                fileLower.find("inject") != std::string::npos) {

                Detection det;
                det.scanner = "Prefetch Scanner";
                det.filePath = entry.path().string();
                det.description = "Prefetch evidence of suspicious program execution";
                det.context = "Prefetch File: " + fileName;
                det.severity = Severity::High;
                det.timestamp = std::to_string(std::time(nullptr));
                det.confidence = 0.75f;

                result.detections.push_back(det);
                result.totalDetected++;
            }

            result.totalScanned++;
            state.scannedFiles++;
        }
    } catch (...) {
        result.error = "Error scanning prefetch folder";
    }

    if (!result.detections.empty()) {
        result.status = "detected";
    }

    return result;
}

ScanResult TempPrefetchScanner::ScanRecentFiles(ScanState& state) {
    ScanResult result;
    result.scannerName = "Recent Files Scanner";
    result.status = "clean";

    std::string recentPath = GetRecentFilesPath();
    if (recentPath.empty()) {
        result.error = "Cannot find Recent files folder";
        return result;
    }

    state.SetActivity("Scanning Recent files: " + recentPath);

    try {
        for (const auto& entry : fs::directory_iterator(recentPath)) {
            if (state.IsCancelRequested()) break;
            if (!entry.is_regular_file()) continue;

            std::string fileName = entry.path().filename().string();
            std::string fileLower = fileName;
            std::transform(fileLower.begin(), fileLower.end(), fileLower.begin(), ::tolower);

            // Check for suspicious files opened recently
            if (fileLower.find(".jar") != std::string::npos ||
                fileLower.find(".exe") != std::string::npos ||
                fileLower.find(".dll") != std::string::npos) {

                if (IsSuspiciousTempFile(fileLower)) {
                    Detection det;
                    det.scanner = "Recent Files Scanner";
                    det.filePath = entry.path().string();
                    det.description = "Recent access to suspicious file";
                    det.context = "File: " + fileName;
                    det.severity = Severity::Medium;
                    det.timestamp = std::to_string(std::time(nullptr));
                    det.confidence = 0.6f;

                    result.detections.push_back(det);
                    result.totalDetected++;
                }
            }

            result.totalScanned++;
            state.scannedFiles++;
        }
    } catch (...) {
        result.error = "Error scanning recent files";
    }

    if (!result.detections.empty()) {
        result.status = "detected";
    }

    return result;
}

ScanResult TempPrefetchScanner::FullTempPrefetchScan(ScanState& state) {
    ScanResult fullResult;
    fullResult.scannerName = "Temp/Prefetch Full Scan";
    fullResult.status = "clean";

    // Scan temp
    state.SetActivity("Starting Temp/Prefetch comprehensive scan...");
    auto tempResult = ScanTempFolder(state);
    fullResult.detections.insert(fullResult.detections.end(),
        tempResult.detections.begin(), tempResult.detections.end());
    fullResult.totalScanned += tempResult.totalScanned;
    fullResult.totalDetected += tempResult.totalDetected;

    // Scan prefetch
    auto prefetchResult = ScanPrefetchFolder(state);
    fullResult.detections.insert(fullResult.detections.end(),
        prefetchResult.detections.begin(), prefetchResult.detections.end());
    fullResult.totalScanned += prefetchResult.totalScanned;
    fullResult.totalDetected += prefetchResult.totalDetected;

    // Scan recent
    auto recentResult = ScanRecentFiles(state);
    fullResult.detections.insert(fullResult.detections.end(),
        recentResult.detections.begin(), recentResult.detections.end());
    fullResult.totalScanned += recentResult.totalScanned;
    fullResult.totalDetected += recentResult.totalDetected;

    if (!fullResult.detections.empty()) {
        fullResult.status = "detected";
    }

    return fullResult;
}

std::vector<std::string> TempPrefetchScanner::GetMRUFiles() {
    std::vector<std::string> mruFiles;
    // Would enumerate MRU list from registry
    return mruFiles;
}

bool TempPrefetchScanner::IsSuspiciousTempFile(const std::string& fileName) {
    for (const auto& pattern : SUSPICIOUS_TEMP_PATTERNS) {
        if (fileName.find(pattern) != std::string::npos) {
            return true;
        }
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// ENHANCED PROCESS SCANNER
// ═══════════════════════════════════════════════════════════════════════════

const std::vector<std::string> EnhancedProcessScanner::SUSPICIOUS_PROCESS_NAMES = {
    "cheatengine", "cheat engine", "processhacker", "process hacker",
    "198macro", "zenithmacro", "autohotkey", "ahk",
    "injector", "dll_inject", "wurst", "meteor", "impact",
    "autoclicker", "op auto clicker", "gs auto clicker",
    "x64dbg", "x32dbg", "ollydbg", "ida"
};

const std::vector<std::string> EnhancedProcessScanner::SUSPICIOUS_COMMAND_PATTERNS = {
    "-javaagent", "-agentlib", "-agentpath", "/inject",
    "LoadLibrary", "CreateRemoteThread", "WriteProcessMemory"
};

EnhancedProcessScanner& EnhancedProcessScanner::Instance() {
    static EnhancedProcessScanner instance;
    return instance;
}

ScanResult EnhancedProcessScanner::ScanRunningProcesses(ScanState& state) {
    ScanResult result;
    result.scannerName = "Enhanced Process Scanner";
    result.status = "clean";

    state.SetActivity("Scanning running processes...");

    auto processes = GetAllProcesses();
    result.totalScanned = processes.size();

    for (const auto& proc : processes) {
        if (state.IsCancelRequested()) break;

        if (IsProcessSuspicious(proc)) {
            Detection det;
            det.scanner = "Enhanced Process Scanner";
            det.filePath = proc.name;
            det.description = "Suspicious process detected";
            det.context = "PID: " + std::to_string(proc.pid) + " | CmdLine: " + proc.commandLine;
            det.severity = Severity::Critical;
            det.timestamp = std::to_string(std::time(nullptr));
            det.confidence = 0.85f;

            result.detections.push_back(det);
            result.totalDetected++;
        }

        state.scannedFiles++;
    }

    if (!result.detections.empty()) {
        result.status = "detected";
    }

    return result;
}

ScanResult EnhancedProcessScanner::DetectHiddenProcesses(ScanState& state) {
    ScanResult result;
    result.scannerName = "Hidden Process Detector";
    result.status = "clean";

    state.SetActivity("Detecting hidden processes...");

    // Get processes from snapshot
    auto snapshotProcs = GetProcessesFromSnapshot();

    // Get processes from tasklist
    auto tasklistProcs = GetProcessesFromTasklist();

    // Compare - processes in snapshot but not in tasklist are hidden
    for (uint32_t pid : snapshotProcs) {
        bool found = false;
        for (const auto& proc : tasklistProcs) {
            if (proc.pid == pid) {
                found = true;
                break;
            }
        }

        if (!found) {
            Detection det;
            det.scanner = "Hidden Process Detector";
            det.filePath = std::to_string(pid);
            det.description = "Hidden process detected";
            det.context = "PID " + std::to_string(pid) + " visible in snapshot but not in tasklist";
            det.severity = Severity::Critical;
            det.timestamp = std::to_string(std::time(nullptr));
            det.confidence = 0.9f;

            result.detections.push_back(det);
            result.totalDetected++;
        }
    }

    if (!result.detections.empty()) {
        result.status = "detected";
    }

    return result;
}

ScanResult EnhancedProcessScanner::ScanProcessCommandLines(ScanState& state) {
    ScanResult result;
    result.scannerName = "Process Command Line Scanner";
    result.status = "clean";

    state.SetActivity("Scanning process command lines...");

    auto processes = GetAllProcesses();

    for (const auto& proc : processes) {
        if (state.IsCancelRequested()) break;

        std::string cmdLower = proc.commandLine;
        std::transform(cmdLower.begin(), cmdLower.end(), cmdLower.begin(), ::tolower);

        for (const auto& pattern : SUSPICIOUS_COMMAND_PATTERNS) {
            std::string patternLower = pattern;
            std::transform(patternLower.begin(), patternLower.end(), patternLower.begin(), ::tolower);

            if (cmdLower.find(patternLower) != std::string::npos) {
                Detection det;
                det.scanner = "Process Command Line Scanner";
                det.filePath = proc.name;
                det.description = "Suspicious pattern in command line: " + pattern;
                det.context = "Full CmdLine: " + proc.commandLine;
                det.severity = Severity::Critical;
                det.timestamp = std::to_string(std::time(nullptr));
                det.confidence = 0.85f;

                result.detections.push_back(det);
                result.totalDetected++;
                break;
            }
        }

        result.totalScanned++;
        state.scannedFiles++;
    }

    if (!result.detections.empty()) {
        result.status = "detected";
    }

    return result;
}

ScanResult EnhancedProcessScanner::DetectProcessTampering(ScanState& state) {
    ScanResult result;
    result.scannerName = "Process Tampering Detector";
    result.status = "clean";

    state.SetActivity("Detecting process tampering...");

    // Would implement process tampering detection here
    // (e.g., checking for modified system processes)

    return result;
}

ProcessInfo EnhancedProcessScanner::GetProcessInfo(uint32_t pid) {
    ProcessInfo info;
    info.pid = pid;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess) {
        char exePath[MAX_PATH] = {0};
        if (GetModuleFileNameExA(hProcess, NULL, exePath, MAX_PATH)) {
            info.executablePath = exePath;
            info.name = fs::path(exePath).filename().string();
        }

        CloseHandle(hProcess);
    }

    return info;
}

std::vector<ProcessInfo> EnhancedProcessScanner::GetAllProcesses() {
    std::vector<ProcessInfo> processes;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return processes;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            ProcessInfo info = GetProcessInfo(pe32.th32ProcessID);
            if (info.pid != 0) {
                processes.push_back(info);
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return processes;
}

bool EnhancedProcessScanner::IsProcessSuspicious(const ProcessInfo& proc) {
    std::string nameLower = proc.name;
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

    for (const auto& suspicious : SUSPICIOUS_PROCESS_NAMES) {
        if (nameLower.find(suspicious) != std::string::npos) {
            return true;
        }
    }

    return false;
}

std::vector<ProcessInfo> EnhancedProcessScanner::GetProcessesFromTasklist() {
    std::vector<ProcessInfo> processes;
    // Implementation would use tasklist command
    return processes;
}

std::vector<ProcessInfo> EnhancedProcessScanner::GetProcessesFromWMIC() {
    std::vector<ProcessInfo> processes;
    // Implementation would use WMIC command
    return processes;
}

std::vector<uint32_t> EnhancedProcessScanner::GetProcessesFromSnapshot() {
    std::vector<uint32_t> pids;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return pids;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            pids.push_back(pe32.th32ProcessID);
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return pids;
}

bool EnhancedProcessScanner::IsProcessHiddenFromTasklist(uint32_t pid) {
    return false;  // Simplified
}

bool EnhancedProcessScanner::IsSuspiciousProcessName(const std::string& name) {
    return IsProcessSuspicious({pid: 0, name: name});
}

// ═══════════════════════════════════════════════════════════════════════════
// DLL ANALYZER
// ═══════════════════════════════════════════════════════════════════════════

const std::vector<std::string> DLLAnalyzer::DANGEROUS_IMPORTS = {
    "CreateRemoteThread", "WriteProcessMemory", "VirtualAllocEx",
    "NtWriteVirtualMemory", "SetWindowsHookEx", "LoadLibraryA", "LoadLibraryW"
};

const std::vector<std::string> DLLAnalyzer::SUSPICIOUS_DLL_PATTERNS = {
    "cheat", "hack", "inject", "bypass", "root", "hide", "cloak"
};

DLLAnalyzer& DLLAnalyzer::Instance() {
    static DLLAnalyzer instance;
    return instance;
}

ScanResult DLLAnalyzer::ScanDLLDirectory(const std::string& dirPath, ScanState& state) {
    ScanResult result;
    result.scannerName = "DLL Directory Scanner";
    result.status = "clean";

    if (!fs::exists(dirPath)) {
        result.error = "Directory not found";
        return result;
    }

    state.SetActivity("Scanning DLL directory: " + dirPath);

    try {
        for (const auto& entry : fs::recursive_directory_iterator(dirPath,
             fs::directory_options::skip_permission_denied)) {
            if (state.IsCancelRequested()) break;
            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            if (ext == ".dll" || ext == ".exe" || ext == ".sys") {
                auto fileResult = AnalyzeDLL(entry.path().string());
                if (!fileResult.detections.empty()) {
                    for (auto& det : fileResult.detections) {
                        result.detections.push_back(det);
                    }
                    result.totalDetected += fileResult.totalDetected;
                }
            }

            result.totalScanned++;
            state.scannedFiles++;
        }
    } catch (...) {
        result.error = "Error scanning directory";
    }

    if (!result.detections.empty()) {
        result.status = "detected";
    }

    return result;
}

ScanResult DLLAnalyzer::AnalyzeDLL(const std::string& filePath) {
    ScanResult result;
    result.scannerName = "DLL Analyzer";
    result.status = "clean";
    result.totalScanned = 1;

    std::string fileName = fs::path(filePath).filename().string();
    std::string fileLower = fileName;
    std::transform(fileLower.begin(), fileLower.end(), fileLower.begin(), ::tolower);

    if (IsSuspiciousDLLName(fileLower)) {
        Detection det;
        det.scanner = "DLL Analyzer";
        det.filePath = filePath;
        det.description = "Suspicious DLL name";
        det.context = "File: " + fileName;
        det.severity = Severity::High;
        det.timestamp = std::to_string(std::time(nullptr));
        det.confidence = 0.7f;

        result.detections.push_back(det);
        result.totalDetected++;
        result.status = "detected";
    }

    return result;
}

std::vector<std::string> DLLAnalyzer::ExtractStringsFromBinary(const std::string& filePath) {
    std::vector<std::string> strings;
    // Would extract ASCII strings from binary file
    return strings;
}

bool DLLAnalyzer::HasInjectionPatterns(const std::string& filePath) {
    // Would check for injection patterns
    return false;
}

std::vector<std::string> DLLAnalyzer::GetSuspiciousImports(const std::string& filePath) {
    std::vector<std::string> suspiciousImports;
    // Would parse PE header and extract imports
    return suspiciousImports;
}

ScanResult DLLAnalyzer::ScanAppDataForDLLs(ScanState& state) {
    ScanResult result;
    result.scannerName = "AppData DLL Scanner";
    result.status = "clean";

    char appdata[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appdata))) {
        return ScanDLLDirectory(appdata, state);
    }

    return result;
}

bool DLLAnalyzer::IsSuspiciousDLLName(const std::string& name) {
    for (const auto& pattern : SUSPICIOUS_DLL_PATTERNS) {
        if (name.find(pattern) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool DLLAnalyzer::HasSuspiciousImports(const std::vector<std::string>& imports) {
    for (const auto& import : imports) {
        for (const auto& dangerous : DANGEROUS_IMPORTS) {
            if (import.find(dangerous) != std::string::npos) {
                return true;
            }
        }
    }
    return false;
}

} // namespace Scanning
} // namespace SSTools
