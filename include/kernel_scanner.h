#pragma once

#include "core.h"
#include <vector>
#include <string>

namespace SSTools {
namespace KernelScanning {

// ═══════════════════════════════════════════════════════════════════════════
// KERNEL-LEVEL DETECTION
// ═══════════════════════════════════════════════════════════════════════════

class KernelScanner {
public:
    static KernelScanner& Instance();
    
    // Scan kernel drivers
    ScanResult ScanLoadedDrivers(ScanState& state);
    
    // Check Device Guard / HVCI status
    ScanResult CheckDeviceGuard(ScanState& state);
    
    // Check Secure Boot status
    ScanResult CheckSecureBoot(ScanState& state);
    
    // Enumerate and check kernel services
    ScanResult ScanKernelServices(ScanState& state);
    
    // Check for kernel-level hooks
    ScanResult DetectKernelHooks(ScanState& state);
    
    // Scan for rootkits via kernel inspection
    ScanResult DetectRootkits(ScanState& state);
    
    // Check integrity of critical system drivers
    ScanResult VerifyDriverIntegrity(ScanState& state);
    
    // Get all loaded drivers
    std::vector<DriverInfo> GetLoadedDrivers();
    
    // Check if specific driver is loaded
    bool IsDriverLoaded(const std::string& driverName);
    
    // Comprehensive kernel scan
    ScanResult FullKernelScan(ScanState& state);

private:
    KernelScanner() = default;
    ~KernelScanner() = default;
    
    // Helper functions
    bool CheckDeviceGuardEnabled();
    bool CheckSecureBootEnabled();
    bool IsKernelPathSuspicious(const std::string& path);
    bool IsSuspiciousDriver(const std::string& driverName);
    
    // Suspicious driver database
    static const std::vector<std::pair<std::string, std::string>> SUSPICIOUS_DRIVERS;
    static const std::vector<std::pair<std::string, std::string>> INJECTION_INDICATORS;
};

// ═══════════════════════════════════════════════════════════════════════════
// MEMORY INJECTION DETECTOR
// ═══════════════════════════════════════════════════════════════════════════

class MemoryInjectionDetector {
public:
    static MemoryInjectionDetector& Instance();
    
    // Detect DLL injection via VirtualAllocEx/WriteProcessMemory
    ScanResult DetectDLLInjection(ScanState& state);
    
    // Detect Java agent injection
    ScanResult DetectJavaAgentInjection(ScanState& state);
    
    // Detect process hollowing
    ScanResult DetectProcessHollowing(ScanState& state);
    
    // Analyze specific process for injection
    MemoryInjectionResult AnalyzeProcess(uint32_t pid, const std::string& processName);
    
    // Check if process has suspicious memory allocations
    bool HasSuspiciousMemoryPatterns(uint32_t pid);
    
    // Detect inline hooks
    ScanResult DetectInlineHooks(ScanState& state);

private:
    MemoryInjectionDetector() = default;
    ~MemoryInjectionDetector() = default;
    
    bool IsSuspiciousDLL(const std::string& dllPath);
    std::vector<std::string> GetProcessLoadedDLLs(uint32_t pid);
};

// ═══════════════════════════════════════════════════════════════════════════
// PROCESS HACKER DETECTOR
// ═══════════════════════════════════════════════════════════════════════════

class ProcessHackerDetector {
public:
    static ProcessHackerDetector& Instance();
    
    // Check if Process Hacker is installed
    bool IsProcessHackerInstalled();
    
    // Check if Process Hacker is running
    bool IsProcessHackerRunning();
    
    // Check for Process Hacker kernel driver
    bool HasProcessHackerDriver();
    
    // Detect Process Hacker injection capability
    ScanResult DetectProcessHackerThreat(ScanState& state);
    
    // Get Process Hacker installation path
    std::string GetProcessHackerPath();

private:
    ProcessHackerDetector() = default;
    ~ProcessHackerDetector() = default;
    
    static const std::vector<std::string> PROCESS_HACKER_LOCATIONS;
    static const std::vector<std::string> PROCESS_HACKER_DRIVERS;
};

// ═══════════════════════════════════════════════════════════════════════════
// TASK MANAGER MONITOR
// ═══════════════════════════════════════════════════════════════════════════

class TaskManagerMonitor {
public:
    static TaskManagerMonitor& Instance();
    
    // Check if Task Manager is disabled
    bool IsTaskManagerDisabled();
    
    // Check for Task Manager tampering
    ScanResult DetectTaskManagerTampering(ScanState& state);
    
    // Check for registry modifications that hide processes
    ScanResult DetectProcessHiding(ScanState& state);
    
    // Check for Windows Hook injections affecting Task Manager
    ScanResult DetectWindowsHookInjections(ScanState& state);
    
    // Get Task Manager status
    TaskManagerResult GetTaskManagerStatus();

private:
    TaskManagerMonitor() = default;
    ~TaskManagerMonitor() = default;
    
    bool CheckTaskManagerRegistry();
    bool CheckProcessHidingTechniques();
};

} // namespace KernelScanning
} // namespace SSTools
