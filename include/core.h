#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <mutex>

// ═══════════════════════════════════════════════════════════════════════════
//  CORE TYPES AND STRUCTURES
// ═══════════════════════════════════════════════════════════════════════════

namespace SSTools {

// Severity levels
enum class Severity {
    None = 0,
    Low = 1,
    Medium = 2,
    High = 3,
    Critical = 4
};

// Scanner types
enum class ScannerType {
    ModScanner,
    DLLScanner,
    KernelScanner,
    ProcessScanner,
    MemoryScanner,
    RegistryScanner,
    TaskManagerScanner,
    TempPrefetchScanner,
    ChromeHistoryScanner,
    JournalScanner,
    ProcessHackerDetector
};

// Detection result structure
struct Detection {
    std::string scanner;
    std::string filePath;
    std::string description;
    std::string context;
    Severity severity;
    std::string timestamp;
    bool isFalsePositive;
    std::string falsePositiveReason;
    float confidence;  // 0.0 - 1.0
    
    Detection() 
        : severity(Severity::Medium), isFalsePositive(false), confidence(0.8f) {}
};

// Scan result structure
struct ScanResult {
    std::string scannerName;
    std::string status;  // "clean", "detected", "error", "warning"
    std::vector<Detection> detections;
    int totalScanned;
    int totalDetected;
    double elapsedMs;
    std::string error;
    
    ScanResult() 
        : status("clean"), totalScanned(0), totalDetected(0), elapsedMs(0.0) {}
};

// Scan state for cancellation and progress
struct ScanState {
    bool cancelRequested;
    int scannedFiles;
    std::string currentActivity;
    float progress;
    std::mutex stateMutex;
    
    ScanState() 
        : cancelRequested(false), scannedFiles(0), progress(0.0f) {}
    
    void SetActivity(const std::string& activity) {
        std::lock_guard<std::mutex> lock(stateMutex);
        currentActivity = activity;
    }
    
    std::string GetActivity() {
        std::lock_guard<std::mutex> lock(stateMutex);
        return currentActivity;
    }
    
    void Cancel() {
        std::lock_guard<std::mutex> lock(stateMutex);
        cancelRequested = true;
    }
    
    bool IsCancelRequested() {
        std::lock_guard<std::mutex> lock(stateMutex);
        return cancelRequested;
    }
};

// Whitelist entry for false-positive filtering
struct WhitelistEntry {
    std::string name;
    std::vector<std::string> keywords;
    std::vector<std::string> contexts;
    Severity maxSeverity;
    bool isLegitimate;
    
    WhitelistEntry() : maxSeverity(Severity::None), isLegitimate(true) {}
};

// Process information
struct ProcessInfo {
    uint32_t pid;
    std::string name;
    std::string commandLine;
    std::string executablePath;
    std::vector<std::string> loadedDLLs;
    bool isElevated;
    bool isSuspicious;
    std::string suspiciousReason;
    
    ProcessInfo() : pid(0), isElevated(false), isSuspicious(false) {}
};

// Kernel driver information
struct DriverInfo {
    std::string name;
    std::string path;
    std::string description;
    bool isSuspicious;
    std::string suspiciousReason;
    
    DriverInfo() : isSuspicious(false) {}
};

// Memory injection detection result
struct MemoryInjectionResult {
    uint32_t targetPID;
    std::string targetProcess;
    bool injectionDetected;
    std::vector<std::string> injectedDLLs;
    std::vector<std::string> suspiciousPatterns;
    std::string detectionMethod;  // "VirtualAllocEx", "WriteProcessMemory", etc.
    
    MemoryInjectionResult() : targetPID(0), injectionDetected(false) {}
};

// Task Manager monitoring result
struct TaskManagerResult {
    bool isDisabled;
    bool isModified;
    std::vector<std::string> suspiciousRegModifications;
    bool hasWindowsHookInjection;
    
    TaskManagerResult() 
        : isDisabled(false), isModified(false), hasWindowsHookInjection(false) {}
};

} // namespace SSTools
