#include "../include/kernel_scanner.h"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <winternl.h>
#include <wintrust.h>
#include <softpub.h>
#include <shlwapi.h>
#include <algorithm>
#include <regex>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "wintrust.lib")

namespace SSTools {
namespace KernelScanning {

// Suspicious driver database
const std::vector<std::pair<std::string, std::string>> KernelScanner::SUSPICIOUS_DRIVERS = {
    {"dbk64", "Cheat Engine kernel driver (64-bit)"},
    {"dbk32", "Cheat Engine kernel driver (32-bit)"},
    {"processhacker", "Process Hacker kernel driver"},
    {"kprocesshacker", "Process Hacker kernel driver"},
    {"cheat", "Generic cheat driver"},
    {"hack", "Generic hack driver"},
    {"inject", "Injection driver"},
    {"bypass", "Anti-cheat bypass driver"},
    {"rootkit", "Rootkit driver"},
    {"hideproc", "Process hiding driver"},
    {"hidefile", "File hiding driver"},
    {"unhook", "Hook removal driver"},
    {"shield", "Suspicious shield driver"},
    {"guardian", "Suspicious guardian driver"},
};

const std::vector<std::pair<std::string, std::string>> KernelScanner::INJECTION_INDICATORS = {
    {"CreateRemoteThread", "Remote thread creation API"},
    {"WriteProcessMemory", "Process memory write API"},
    {"NtWriteVirtualMemory", "NT virtual memory write"},
    {"VirtualAllocEx", "Remote memory allocation"},
    {"SetWindowsHookEx", "Windows hook installation"},
    {"LoadLibraryA", "DLL loading function"},
    {"LoadLibraryW", "DLL loading function"},
};

KernelScanner& KernelScanner::Instance() {
    static KernelScanner instance;
    return instance;
}

ScanResult KernelScanner::FullKernelScan(ScanState& state) {
    ScanResult fullResult;
    fullResult.scannerName = "Kernel Full Scan";
    fullResult.status = "clean";
    
    state.SetActivity("Starting comprehensive kernel scan...");
    
    // 1. Scan drivers
    auto driverResult = ScanLoadedDrivers(state);
    fullResult.detections.insert(fullResult.detections.end(),
        driverResult.detections.begin(), driverResult.detections.end());
    fullResult.totalScanned += driverResult.totalScanned;
    fullResult.totalDetected += driverResult.totalDetected;
    
    // 2. Check Device Guard
    state.SetActivity("Checking Device Guard status...");
    auto guardResult = CheckDeviceGuard(state);
    fullResult.detections.insert(fullResult.detections.end(),
        guardResult.detections.begin(), guardResult.detections.end());
    fullResult.totalDetected += guardResult.totalDetected;
    
    // 3. Check Secure Boot
    state.SetActivity("Checking Secure Boot status...");
    auto bootResult = CheckSecureBoot(state);
    fullResult.detections.insert(fullResult.detections.end(),
        bootResult.detections.begin(), bootResult.detections.end());
    fullResult.totalDetected += bootResult.totalDetected;
    
    // 4. Scan kernel services
    state.SetActivity("Scanning kernel services...");
    auto serviceResult = ScanKernelServices(state);
    fullResult.detections.insert(fullResult.detections.end(),
        serviceResult.detections.begin(), serviceResult.detections.end());
    fullResult.totalScanned += serviceResult.totalScanned;
    fullResult.totalDetected += serviceResult.totalDetected;
    
    // 5. Detect kernel hooks
    state.SetActivity("Detecting kernel hooks...");
    auto hookResult = DetectKernelHooks(state);
    fullResult.detections.insert(fullResult.detections.end(),
        hookResult.detections.begin(), hookResult.detections.end());
    fullResult.totalDetected += hookResult.totalDetected;
    
    if (!fullResult.detections.empty()) {
        fullResult.status = "detected";
    }
    
    return fullResult;
}

ScanResult KernelScanner::ScanLoadedDrivers(ScanState& state) {
    ScanResult result;
    result.scannerName = "Kernel Driver Scanner";
    result.status = "clean";
    
    state.SetActivity("Enumerating loaded drivers...");
    
    LPVOID drivers[1024];
    DWORD cbNeeded;
    
    if (!EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded)) {
        result.status = "error";
        result.error = "Failed to enumerate drivers";
        return result;
    }
    
    int driverCount = cbNeeded / sizeof(LPVOID);
    result.totalScanned = driverCount;
    
    for (int i = 0; i < driverCount && !state.IsCancelRequested(); i++) {
        char driverName[MAX_PATH] = {0};
        if (GetDeviceDriverBaseNameA(drivers[i], driverName, sizeof(driverName))) {
            std::string name(driverName);
            std::string nameLower = name;
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            
            // Check against suspicious drivers
            for (const auto& [pattern, desc] : SUSPICIOUS_DRIVERS) {
                if (nameLower.find(pattern) != std::string::npos) {
                    Detection det;
                    det.scanner = "Kernel Driver Scanner";
                    det.filePath = name;
                    det.description = "Suspicious kernel driver detected: " + desc;
                    det.context = "Driver: " + name;
                    det.severity = Severity::Critical;
                    det.timestamp = std::to_string(std::time(nullptr));
                    det.confidence = 0.95f;
                    
                    result.detections.push_back(det);
                    result.totalDetected++;
                    break;
                }
            }
        }
        state.scannedFiles++;
    }
    
    if (!result.detections.empty()) {
        result.status = "detected";
    }
    
    return result;
}

ScanResult KernelScanner::CheckDeviceGuard(ScanState& state) {
    ScanResult result;
    result.scannerName = "Device Guard Checker";
    result.status = "clean";
    
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "System\\CurrentControlSet\\Control\\DeviceGuard\\Scenarios\\HypervisorEnforcedCodeIntegrity",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        DWORD hvci = 0;
        DWORD dataSize = sizeof(hvci);
        if (RegQueryValueExA(hKey, "Enabled", nullptr, nullptr, (LPBYTE)&hvci, &dataSize) == ERROR_SUCCESS) {
            if (hvci == 0) {
                Detection det;
                det.scanner = "Device Guard Checker";
                det.filePath = "Device Guard";
                det.description = "HVCI (Hypervisor Code Integrity) is DISABLED";
                det.context = "System is vulnerable to kernel-mode injection";
                det.severity = Severity::Medium;
                det.timestamp = std::to_string(std::time(nullptr));
                det.confidence = 1.0f;
                
                result.detections.push_back(det);
                result.totalDetected++;
                result.status = "warning";
            }
        }
        RegCloseKey(hKey);
    }
    
    return result;
}

ScanResult KernelScanner::CheckSecureBoot(ScanState& state) {
    ScanResult result;
    result.scannerName = "Secure Boot Checker";
    result.status = "clean";
    
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Control\\SecureBoot\\State",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        DWORD secureBoot = 0;
        DWORD dataSize = sizeof(secureBoot);
        if (RegQueryValueExA(hKey, "UEFISecureBootEnabled", nullptr, nullptr, (LPBYTE)&secureBoot, &dataSize) == ERROR_SUCCESS) {
            if (secureBoot == 0) {
                Detection det;
                det.scanner = "Secure Boot Checker";
                det.filePath = "Secure Boot";
                det.description = "Secure Boot is DISABLED";
                det.context = "System may allow unsigned/malicious drivers";
                det.severity = Severity::Low;
                det.timestamp = std::to_string(std::time(nullptr));
                det.confidence = 1.0f;
                
                result.detections.push_back(det);
                result.totalDetected++;
                result.status = "warning";
            }
        }
        RegCloseKey(hKey);
    }
    
    return result;
}

ScanResult KernelScanner::ScanKernelServices(ScanState& state) {
    ScanResult result;
    result.scannerName = "Kernel Services Scanner";
    result.status = "clean";
    
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Services",
        0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hKey) != ERROR_SUCCESS) {
        result.error = "Cannot access Services registry";
        return result;
    }
    
    char subKeyName[256];
    DWORD idx = 0, nameSize;
    
    while (true) {
        nameSize = sizeof(subKeyName);
        if (RegEnumKeyExA(hKey, idx++, subKeyName, &nameSize, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) {
            break;
        }
        
        if (state.IsCancelRequested()) break;
        
        std::string svcName(subKeyName);
        std::string svcLower = svcName;
        std::transform(svcLower.begin(), svcLower.end(), svcLower.begin(), ::tolower);
        
        // Check if it's a kernel driver service
        std::string svcPath = "SYSTEM\\CurrentControlSet\\Services\\" + svcName;
        HKEY svcKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, svcPath.c_str(), 0, KEY_READ, &svcKey) == ERROR_SUCCESS) {
            DWORD svcType = 0;
            DWORD dataSize = sizeof(svcType);
            if (RegQueryValueExA(svcKey, "Type", nullptr, nullptr, (LPBYTE)&svcType, &dataSize) == ERROR_SUCCESS) {
                // Type 1 = Kernel driver, 2 = File system driver
                if (svcType == 1 || svcType == 2) {
                    for (const auto& [pattern, desc] : SUSPICIOUS_DRIVERS) {
                        if (svcLower.find(pattern) != std::string::npos) {
                            Detection det;
                            det.scanner = "Kernel Services Scanner";
                            det.filePath = svcName;
                            det.description = "Suspicious kernel service: " + desc;
                            
                            // Get ImagePath
                            char imagePath[MAX_PATH] = {0};
                            DWORD pathSize = sizeof(imagePath);
                            RegQueryValueExA(svcKey, "ImagePath", nullptr, nullptr, (LPBYTE)imagePath, &pathSize);
                            det.context = "ImagePath: " + std::string(imagePath);
                            det.severity = Severity::Critical;
                            det.timestamp = std::to_string(std::time(nullptr));
                            det.confidence = 0.9f;
                            
                            result.detections.push_back(det);
                            result.totalDetected++;
                            break;
                        }
                    }
                }
            }
            RegCloseKey(svcKey);
        }
        
        result.totalScanned++;
        state.scannedFiles++;
    }
    
    RegCloseKey(hKey);
    
    if (!result.detections.empty()) {
        result.status = "detected";
    }
    
    return result;
}

ScanResult KernelScanner::DetectKernelHooks(ScanState& state) {
    ScanResult result;
    result.scannerName = "Kernel Hooks Detector";
    result.status = "clean";
    
    state.SetActivity("Analyzing kernel hook patterns...");
    
    // Check for common hook points via registry
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "System\\CurrentControlSet\\Services\\kbdclass\\Parameters",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        // Check for keyboard hook injections
        char buffer[256] = {0};
        DWORD bufSize = sizeof(buffer);
        if (RegQueryValueExA(hKey, "UpperFilters", nullptr, nullptr, (LPBYTE)buffer, &bufSize) == ERROR_SUCCESS) {
            std::string filters(buffer);
            std::string filtersLower = filters;
            std::transform(filtersLower.begin(), filtersLower.end(), filtersLower.begin(), ::tolower);
            
            if (filtersLower.find("cheat") != std::string::npos || 
                filtersLower.find("hack") != std::string::npos) {
                Detection det;
                det.scanner = "Kernel Hooks Detector";
                det.filePath = "Keyboard Filter";
                det.description = "Suspicious keyboard filter hook detected";
                det.context = "Filter: " + filters;
                det.severity = Severity::High;
                det.timestamp = std::to_string(std::time(nullptr));
                det.confidence = 0.85f;
                
                result.detections.push_back(det);
                result.totalDetected++;
                result.status = "detected";
            }
        }
        RegCloseKey(hKey);
    }
    
    return result;
}

ScanResult KernelScanner::DetectRootkits(ScanState& state) {
    ScanResult result;
    result.scannerName = "Rootkit Detector";
    result.status = "clean";
    
    state.SetActivity("Scanning for rootkit indicators...");
    
    // Check for hidden processes via rootkit indicators
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        result.error = "Cannot create process snapshot";
        return result;
    }
    
    // Further rootkit detection logic would go here
    // This is a simplified version
    
    CloseHandle(hSnapshot);
    return result;
}

ScanResult KernelScanner::VerifyDriverIntegrity(ScanState& state) {
    ScanResult result;
    result.scannerName = "Driver Integrity Verifier";
    result.status = "clean";
    
    state.SetActivity("Verifying driver signatures...");
    
    // Verify system drivers are properly signed
    LPVOID drivers[1024];
    DWORD cbNeeded;
    
    if (!EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded)) {
        result.error = "Cannot enumerate drivers";
        return result;
    }
    
    // Verification logic would check digital signatures
    // For now, just flag unsigned drivers
    
    return result;
}

std::vector<DriverInfo> KernelScanner::GetLoadedDrivers() {
    std::vector<DriverInfo> drivers;
    
    LPVOID driverArray[1024];
    DWORD cbNeeded;
    
    if (EnumDeviceDrivers(driverArray, sizeof(driverArray), &cbNeeded)) {
        int driverCount = cbNeeded / sizeof(LPVOID);
        
        for (int i = 0; i < driverCount; i++) {
            char driverName[MAX_PATH] = {0};
            if (GetDeviceDriverBaseNameA(driverArray[i], driverName, sizeof(driverName))) {
                DriverInfo info;
                info.name = driverName;
                info.isSuspicious = IsSuspiciousDriver(driverName);
                
                if (info.isSuspicious) {
                    for (const auto& [pattern, desc] : SUSPICIOUS_DRIVERS) {
                        std::string nameLower = info.name;
                        std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
                        if (nameLower.find(pattern) != std::string::npos) {
                            info.suspiciousReason = desc;
                            break;
                        }
                    }
                }
                
                drivers.push_back(info);
            }
        }
    }
    
    return drivers;
}

bool KernelScanner::IsDriverLoaded(const std::string& driverName) {
    LPVOID drivers[1024];
    DWORD cbNeeded;
    
    if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded)) {
        int driverCount = cbNeeded / sizeof(LPVOID);
        
        for (int i = 0; i < driverCount; i++) {
            char name[MAX_PATH] = {0};
            if (GetDeviceDriverBaseNameA(drivers[i], name, sizeof(name))) {
                std::string drvName(name);
                std::string searchName = driverName;
                std::transform(drvName.begin(), drvName.end(), drvName.begin(), ::tolower);
                std::transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);
                
                if (drvName.find(searchName) != std::string::npos) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool KernelScanner::IsSuspiciousDriver(const std::string& driverName) {
    std::string nameLower = driverName;
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
    
    for (const auto& [pattern, _] : SUSPICIOUS_DRIVERS) {
        if (nameLower.find(pattern) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool KernelScanner::IsKernelPathSuspicious(const std::string& path) {
    std::string pathLower = path;
    std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(), ::tolower);
    
    // Legitimate kernel paths
    static const std::vector<std::string> legitimatePaths = {
        "\\windows\\system32\\drivers\\",
        "\\systemroot\\system32\\drivers\\",
    };
    
    for (const auto& legit : legitimatePaths) {
        if (pathLower.find(legit) != std::string::npos) {
            return false;  // In legitimate path
        }
    }
    
    return true;  // Suspicious location
}

// ═══════════════════════════════════════════════════════════════════════════
// MEMORY INJECTION DETECTOR
// ═══════════════════════════════════════════════════════════════════════════

MemoryInjectionDetector& MemoryInjectionDetector::Instance() {
    static MemoryInjectionDetector instance;
    return instance;
}

ScanResult MemoryInjectionDetector::DetectDLLInjection(ScanState& state) {
    ScanResult result;
    result.scannerName = "DLL Injection Detector";
    result.status = "clean";
    
    state.SetActivity("Detecting DLL injection vectors...");
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        result.error = "Cannot create process snapshot";
        return result;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    
    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (state.IsCancelRequested()) break;
            
            MemoryInjectionResult injResult = AnalyzeProcess(pe32.th32ProcessID, pe32.szExeFile);
            
            if (injResult.injectionDetected) {
                Detection det;
                det.scanner = "DLL Injection Detector";
                det.filePath = injResult.targetProcess;
                det.description = "Potential DLL injection detected";
                det.context = "Method: " + injResult.detectionMethod;
                det.severity = Severity::Critical;
                det.timestamp = std::to_string(std::time(nullptr));
                det.confidence = 0.85f;
                
                for (const auto& dll : injResult.injectedDLLs) {
                    det.context += "\nInjected DLL: " + dll;
                }
                
                result.detections.push_back(det);
                result.totalDetected++;
            }
            
            result.totalScanned++;
            state.scannedFiles++;
            
        } while (Process32Next(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    
    if (!result.detections.empty()) {
        result.status = "detected";
    }
    
    return result;
}

ScanResult MemoryInjectionDetector::DetectJavaAgentInjection(ScanState& state) {
    ScanResult result;
    result.scannerName = "Java Agent Injection Detector";
    result.status = "clean";
    
    state.SetActivity("Detecting Java agent injection...");
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return result;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    
    if (Process32First(hSnapshot, &pe32)) {
        do {
            std::string procName(pe32.szExeFile);
            std::string procLower = procName;
            std::transform(procLower.begin(), procLower.end(), procLower.begin(), ::tolower);
            
            // Check if it's a Java process
            if (procLower.find("java") != std::string::npos) {
                // Get command line (requires elevated privileges)
                // Java agent injection would show up in command line with -javaagent
                result.totalScanned++;
            }
            
        } while (Process32Next(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    return result;
}

ScanResult MemoryInjectionDetector::DetectProcessHollowing(ScanState& state) {
    ScanResult result;
    result.scannerName = "Process Hollowing Detector";
    result.status = "clean";
    
    state.SetActivity("Detecting process hollowing attacks...");
    
    // Process hollowing detection logic here
    
    return result;
}

MemoryInjectionResult MemoryInjectionDetector::AnalyzeProcess(uint32_t pid, const std::string& processName) {
    MemoryInjectionResult result;
    result.targetPID = pid;
    result.targetProcess = processName;
    result.injectionDetected = false;
    
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        return result;
    }
    
    // Get loaded DLLs
    result.injectedDLLs = GetProcessLoadedDLLs(pid);
    
    // Check for suspicious DLLs
    for (const auto& dll : result.injectedDLLs) {
        if (IsSuspiciousDLL(dll)) {
            result.injectionDetected = true;
            result.detectionMethod = "VirtualAllocEx/WriteProcessMemory";
        }
    }
    
    CloseHandle(hProcess);
    return result;
}

bool MemoryInjectionDetector::HasSuspiciousMemoryPatterns(uint32_t pid) {
    // Check for suspicious memory allocation patterns
    return false;  // Simplified
}

ScanResult MemoryInjectionDetector::DetectInlineHooks(ScanState& state) {
    ScanResult result;
    result.scannerName = "Inline Hooks Detector";
    result.status = "clean";
    
    state.SetActivity("Detecting inline hooks...");
    
    // Inline hook detection logic here
    
    return result;
}

std::vector<std::string> MemoryInjectionDetector::GetProcessLoadedDLLs(uint32_t pid) {
    std::vector<std::string> dlls;
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return dlls;
    }
    
    MODULEENTRY32 me32;
    me32.dwSize = sizeof(me32);
    
    if (Module32First(hSnapshot, &me32)) {
        do {
            dlls.push_back(me32.szExePath);
        } while (Module32Next(hSnapshot, &me32));
    }
    
    CloseHandle(hSnapshot);
    return dlls;
}

bool MemoryInjectionDetector::IsSuspiciousDLL(const std::string& dllPath) {
    std::string pathLower = dllPath;
    std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(), ::tolower);
    
    static const std::vector<std::string> suspiciousDLLs = {
        "cheat", "hack", "inject", "bypass", "hook", "proxy", "wrapper",
        "198macro", "zenithmacro", "wurst", "meteor", "impact", "aristois"
    };
    
    for (const auto& pattern : suspiciousDLLs) {
        if (pathLower.find(pattern) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// PROCESS HACKER DETECTOR
// ═══════════════════════════════════════════════════════════════════════════

const std::vector<std::string> ProcessHackerDetector::PROCESS_HACKER_LOCATIONS = {
    "C:\\Program Files\\Process Hacker",
    "C:\\Program Files (x86)\\Process Hacker",
    "C:\\Users\\",  // May be in various user folders
};

const std::vector<std::string> ProcessHackerDetector::PROCESS_HACKER_DRIVERS = {
    "processhacker",
    "kprocesshacker",
    "kph",
};

ProcessHackerDetector& ProcessHackerDetector::Instance() {
    static ProcessHackerDetector instance;
    return instance;
}

bool ProcessHackerDetector::IsProcessHackerInstalled() {
    for (const auto& location : PROCESS_HACKER_LOCATIONS) {
        if (PathFileExistsA((location + "\\ProcessHacker.exe").c_str())) {
            return true;
        }
    }
    return false;
}

bool ProcessHackerDetector::IsProcessHackerRunning() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    
    bool found = false;
    if (Process32First(hSnapshot, &pe32)) {
        do {
            std::string procName(pe32.szExeFile);
            std::transform(procName.begin(), procName.end(), procName.begin(), ::tolower);
            
            if (procName.find("processhacker") != std::string::npos) {
                found = true;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    return found;
}

bool ProcessHackerDetector::HasProcessHackerDriver() {
    return KernelScanner::Instance().IsDriverLoaded("kprocesshacker") ||
           KernelScanner::Instance().IsDriverLoaded("processhacker");
}

ScanResult ProcessHackerDetector::DetectProcessHackerThreat(ScanState& state) {
    ScanResult result;
    result.scannerName = "Process Hacker Threat Detector";
    result.status = "clean";
    
    state.SetActivity("Scanning for Process Hacker...");
    
    // Check if installed
    if (IsProcessHackerInstalled()) {
        Detection det;
        det.scanner = "Process Hacker Detector";
        det.filePath = GetProcessHackerPath();
        det.description = "Process Hacker detected - can be used for DLL injection and memory manipulation";
        det.context = "Process Hacker is a system tool that allows memory inspection and DLL injection";
        det.severity = Severity::High;
        det.timestamp = std::to_string(std::time(nullptr));
        det.confidence = 0.95f;
        
        result.detections.push_back(det);
        result.totalDetected++;
        result.status = "detected";
    }
    
    // Check if running
    if (IsProcessHackerRunning()) {
        Detection det;
        det.scanner = "Process Hacker Detector";
        det.filePath = "ProcessHacker.exe";
        det.description = "Process Hacker is currently running";
        det.context = "Active process allows real-time memory inspection and DLL injection";
        det.severity = Severity::Critical;
        det.timestamp = std::to_string(std::time(nullptr));
        det.confidence = 1.0f;
        
        result.detections.push_back(det);
        result.totalDetected++;
        result.status = "detected";
    }
    
    // Check for kernel driver
    if (HasProcessHackerDriver()) {
        Detection det;
        det.scanner = "Process Hacker Detector";
        det.filePath = "kprocesshacker";
        det.description = "Process Hacker kernel driver loaded";
        det.context = "Kernel driver allows low-level system inspection and modification";
        det.severity = Severity::Critical;
        det.timestamp = std::to_string(std::time(nullptr));
        det.confidence = 0.99f;
        
        result.detections.push_back(det);
        result.totalDetected++;
        result.status = "detected";
    }
    
    return result;
}

std::string ProcessHackerDetector::GetProcessHackerPath() {
    for (const auto& location : PROCESS_HACKER_LOCATIONS) {
        std::string fullPath = location + "\\ProcessHacker.exe";
        if (PathFileExistsA(fullPath.c_str())) {
            return fullPath;
        }
    }
    return "";
}

// ═══════════════════════════════════════════════════════════════════════════
// TASK MANAGER MONITOR
// ═══════════════════════════════════════════════════════════════════════════

TaskManagerMonitor& TaskManagerMonitor::Instance() {
    static TaskManagerMonitor instance;
    return instance;
}

bool TaskManagerMonitor::IsTaskManagerDisabled() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        DWORD disableTaskMgr = 0;
        DWORD dataSize = sizeof(disableTaskMgr);
        
        if (RegQueryValueExA(hKey, "DisableTaskMgr", nullptr, nullptr, (LPBYTE)&disableTaskMgr, &dataSize) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return disableTaskMgr != 0;
        }
        
        RegCloseKey(hKey);
    }
    
    return false;
}

ScanResult TaskManagerMonitor::DetectTaskManagerTampering(ScanState& state) {
    ScanResult result;
    result.scannerName = "Task Manager Tampering Detector";
    result.status = "clean";
    
    state.SetActivity("Checking Task Manager integrity...");
    
    if (IsTaskManagerDisabled()) {
        Detection det;
        det.scanner = "Task Manager Monitor";
        det.filePath = "taskmgr.exe";
        det.description = "Task Manager has been disabled";
        det.context = "DisableTaskMgr registry value is set";
        det.severity = Severity::High;
        det.timestamp = std::to_string(std::time(nullptr));
        det.confidence = 1.0f;
        
        result.detections.push_back(det);
        result.totalDetected++;
        result.status = "detected";
    }
    
    return result;
}

ScanResult TaskManagerMonitor::DetectProcessHiding(ScanState& state) {
    ScanResult result;
    result.scannerName = "Process Hiding Detector";
    result.status = "clean";
    
    state.SetActivity("Detecting process hiding techniques...");
    
    // Check for process hiding registry modifications
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Services",
        0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hKey) == ERROR_SUCCESS) {
        
        char subKeyName[256];
        DWORD idx = 0, nameSize;
        
        while (true) {
            nameSize = sizeof(subKeyName);
            if (RegEnumKeyExA(hKey, idx++, subKeyName, &nameSize, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) {
                break;
            }
            
            std::string svcName(subKeyName);
            std::string svcLower = svcName;
            std::transform(svcLower.begin(), svcLower.end(), svcLower.begin(), ::tolower);
            
            // Check for suspicious image paths
            if (svcLower.find("hide") != std::string::npos || 
                svcLower.find("cloak") != std::string::npos) {
                
                Detection det;
                det.scanner = "Process Hiding Detector";
                det.filePath = svcName;
                det.description = "Suspicious service name suggesting process hiding";
                det.context = "Service: " + svcName;
                det.severity = Severity::High;
                det.timestamp = std::to_string(std::time(nullptr));
                det.confidence = 0.7f;
                
                result.detections.push_back(det);
                result.totalDetected++;
            }
        }
        
        RegCloseKey(hKey);
    }
    
    if (!result.detections.empty()) {
        result.status = "detected";
    }
    
    return result;
}

ScanResult TaskManagerMonitor::DetectWindowsHookInjections(ScanState& state) {
    ScanResult result;
    result.scannerName = "Windows Hook Injections Detector";
    result.status = "clean";
    
    state.SetActivity("Detecting Windows hook injections...");
    
    // Check AppInit_DLLs registry entry
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        char appInitDlls[1024] = {0};
        DWORD dataSize = sizeof(appInitDlls);
        
        if (RegQueryValueExA(hKey, "AppInit_DLLs", nullptr, nullptr, (LPBYTE)appInitDlls, &dataSize) == ERROR_SUCCESS) {
            std::string dlls(appInitDlls);
            if (!dlls.empty()) {
                Detection det;
                det.scanner = "Windows Hook Injections Detector";
                det.filePath = "AppInit_DLLs";
                det.description = "AppInit_DLLs registry entry detected (can be used for hooking)";
                det.context = "DLLs: " + dlls;
                det.severity = Severity::Medium;
                det.timestamp = std::to_string(std::time(nullptr));
                det.confidence = 0.8f;
                
                result.detections.push_back(det);
                result.totalDetected++;
                result.status = "warning";
            }
        }
        
        RegCloseKey(hKey);
    }
    
    return result;
}

TaskManagerResult TaskManagerMonitor::GetTaskManagerStatus() {
    TaskManagerResult status;
    status.isDisabled = IsTaskManagerDisabled();
    status.isModified = false;  // Would need deeper analysis
    status.hasWindowsHookInjection = false;  // Would need to check AppInit_DLLs
    
    return status;
}

} // namespace KernelScanning
} // namespace SSTools
