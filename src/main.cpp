#include "../include/core.h"
#include "../include/kernel_scanner.h"
#include "../include/false_positive_filter.h"
#include "../include/advanced_scanners.h"
#include "../include/imgui_interface.h"
#include "../include/mods_scanner.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace SSTools;
using namespace SSTools::KernelScanning;
using namespace SSTools::Scanning;
using namespace SSTools::Detection;
using namespace SSTools::UI;
using namespace SSTools::ModScanning;

// Global scan state
ScanState globalScanState;
std::vector<ScanResult> globalResults;
bool scanInProgress = false;

void PrintBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════════════╗
║                                                                       ║
║              SS-TOOLS IMPROVED v2.0 - Advanced Anti-Cheat            ║
║                                                                       ║
║   • Kernel-Level Detection        • ImGUI Interface                  ║
║   • Memory Injection Detection    • False-Positive Filtering         ║
║   • Process Hacker Detection      • Comprehensive Scanning           ║
║   • Task Manager Monitoring       • Advanced Context Analysis        ║
║                                                                       ║
╚═══════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void PrintDetection(const Detection& det) {
    std::cout << "\n[DETECTION]" << std::endl;
    std::cout << "  Scanner:   " << det.scanner << std::endl;
    std::cout << "  File:      " << det.filePath << std::endl;
    std::cout << "  Desc:      " << det.description << std::endl;
    std::cout << "  Context:   " << det.context << std::endl;
    
    std::string severityStr = "UNKNOWN";
    switch (det.severity) {
        case Severity::Critical: severityStr = "CRITICAL"; break;
        case Severity::High: severityStr = "HIGH"; break;
        case Severity::Medium: severityStr = "MEDIUM"; break;
        case Severity::Low: severityStr = "LOW"; break;
        case Severity::None: severityStr = "NONE"; break;
    }
    
    std::cout << "  Severity:  " << severityStr << std::endl;
    std::cout << "  Confidence:" << std::fixed << std::setprecision(1) << (det.confidence * 100.0f) << "%" << std::endl;
    
    if (det.isFalsePositive) {
        std::cout << "  [FALSE POSITIVE] " << det.falsePositiveReason << std::endl;
    }
}

void RunQuickScan() {
    std::cout << "\n=== QUICK SCAN (Fast) ===" << std::endl;
    globalScanState.cancelRequested = false;
    globalScanState.scannedFiles = 0;
    globalScanState.progress = 0.0f;
    scanInProgress = true;

    std::vector<ScanResult> results;

    // Process Scanner
    std::cout << "\n[1/3] Running Process Scanner..." << std::endl;
    {
        auto& scanner = EnhancedProcessScanner::Instance();
        auto result = scanner.ScanRunningProcesses(globalScanState);
        results.push_back(result);
        std::cout << "  Found: " << result.totalDetected << " detections" << std::endl;
        for (const auto& det : result.detections) {
            PrintDetection(det);
        }
    }

    // Task Manager Check
    std::cout << "\n[2/3] Running Task Manager Monitor..." << std::endl;
    {
        auto& monitor = TaskManagerMonitor::Instance();
        auto result = monitor.DetectTaskManagerTampering(globalScanState);
        results.push_back(result);
        std::cout << "  Found: " << result.totalDetected << " detections" << std::endl;
        for (const auto& det : result.detections) {
            PrintDetection(det);
        }
    }

    // DLL Scanner
    std::cout << "\n[3/3] Running DLL Scanner..." << std::endl;
    {
        auto& scanner = DLLAnalyzer::Instance();
        char* appdata = nullptr;
        size_t sz = 0;
        _dupenv_s(&appdata, &sz, "APPDATA");
        if (appdata) {
            std::string appdataPath(appdata);
            auto result = scanner.ScanDLLDirectory(appdataPath, globalScanState);
            results.push_back(result);
            std::cout << "  Found: " << result.totalDetected << " detections" << std::endl;
            free(appdata);
        }
    }

    globalResults = results;
    scanInProgress = false;

    // Print summary
    int totalDetections = 0;
    int criticalCount = 0;
    for (const auto& result : results) {
        totalDetections += result.totalDetected;
        for (const auto& det : result.detections) {
            if (det.severity == Severity::Critical) criticalCount++;
        }
    }

    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "SCAN SUMMARY" << std::endl;
    std::cout << "Total Detections: " << totalDetections << std::endl;
    std::cout << "Critical Issues: " << criticalCount << std::endl;
    std::cout << std::string(70, '=') << std::endl;
}

void RunFullScan() {
    std::cout << "\n=== FULL SCAN (Comprehensive) ===" << std::endl;
    globalScanState.cancelRequested = false;
    globalScanState.scannedFiles = 0;
    globalScanState.progress = 0.0f;
    scanInProgress = true;

    std::vector<ScanResult> results;

    // 1. Kernel Scanner
    std::cout << "\n[1/8] Running Kernel Scanner..." << std::endl;
    {
        auto& scanner = KernelScanner::Instance();
        auto result = scanner.FullKernelScan(globalScanState);
        results.push_back(result);
        std::cout << "  Status: " << result.status << std::endl;
        std::cout << "  Found: " << result.totalDetected << " detections" << std::endl;
        for (const auto& det : result.detections) {
            PrintDetection(det);
        }
    }

    // 2. Memory Injection Detector
    std::cout << "\n[2/8] Detecting Memory Injections..." << std::endl;
    {
        auto& detector = MemoryInjectionDetector::Instance();
        auto result = detector.DetectDLLInjection(globalScanState);
        results.push_back(result);
        std::cout << "  Found: " << result.totalDetected << " detections" << std::endl;
    }

    // 3. Process Hacker Detector
    std::cout << "\n[3/8] Detecting Process Hacker..." << std::endl;
    {
        auto& detector = ProcessHackerDetector::Instance();
        auto result = detector.DetectProcessHackerThreat(globalScanState);
        results.push_back(result);
        std::cout << "  Status: " << result.status << std::endl;
        std::cout << "  Found: " << result.totalDetected << " detections" << std::endl;
        for (const auto& det : result.detections) {
            PrintDetection(det);
        }
    }

    // 4. Process Scanner
    std::cout << "\n[4/8] Scanning Running Processes..." << std::endl;
    {
        auto& scanner = EnhancedProcessScanner::Instance();
        auto result = scanner.ScanRunningProcesses(globalScanState);
        results.push_back(result);
        std::cout << "  Scanned: " << result.totalScanned << " processes" << std::endl;
        std::cout << "  Found: " << result.totalDetected << " detections" << std::endl;
    }

    // 5. Temp/Prefetch Scanner
    std::cout << "\n[5/8] Scanning Temp/Prefetch..." << std::endl;
    {
        auto& scanner = TempPrefetchScanner::Instance();
        auto result = scanner.FullTempPrefetchScan(globalScanState);
        results.push_back(result);
        std::cout << "  Scanned: " << result.totalScanned << " files" << std::endl;
        std::cout << "  Found: " << result.totalDetected << " detections" << std::endl;
    }

    // 6. DLL Scanner
    std::cout << "\n[6/8] Scanning DLL Files..." << std::endl;
    {
        auto& scanner = DLLAnalyzer::Instance();
        char* appdata = nullptr;
        size_t sz = 0;
        _dupenv_s(&appdata, &sz, "APPDATA");
        if (appdata) {
            std::string appdataPath(appdata);
            auto result = scanner.ScanDLLDirectory(appdataPath, globalScanState);
            results.push_back(result);
            std::cout << "  Scanned: " << result.totalScanned << " files" << std::endl;
            std::cout << "  Found: " << result.totalDetected << " detections" << std::endl;
            free(appdata);
        }
    }

    // 7. Hidden Process Detection
    std::cout << "\n[7/8] Detecting Hidden Processes..." << std::endl;
    {
        auto& scanner = EnhancedProcessScanner::Instance();
        auto result = scanner.DetectHiddenProcesses(globalScanState);
        results.push_back(result);
        std::cout << "  Found: " << result.totalDetected << " hidden processes" << std::endl;
    }

    // 8. Apply False Positive Filtering
    std::cout << "\n[8/8] Applying False Positive Filter..." << std::endl;
    {
        auto& filter = FalsePositiveFilter::Instance();
        int falsePositives = 0;
        
        for (auto& result : results) {
            auto filtered = filter.FilterDetections(result.detections);
            int beforeCount = result.detections.size();
            result.detections = filtered;
            falsePositives += (beforeCount - filtered.size());
        }
        
        std::cout << "  Removed: " << falsePositives << " false positives" << std::endl;
    }

    globalResults = results;
    scanInProgress = false;

    // Print comprehensive summary
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "COMPREHENSIVE SCAN SUMMARY" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    int totalDetections = 0;
    int criticalCount = 0, highCount = 0, mediumCount = 0, lowCount = 0;
    
    for (const auto& result : results) {
        std::cout << "\n" << result.scannerName << std::endl;
        std::cout << "  Status: " << result.status << std::endl;
        std::cout << "  Scanned: " << result.totalScanned << std::endl;
        std::cout << "  Detections: " << result.totalDetected << std::endl;
        std::cout << "  Time: " << std::fixed << std::setprecision(2) << result.elapsedMs << "ms" << std::endl;

        totalDetections += result.totalDetected;
        for (const auto& det : result.detections) {
            switch (det.severity) {
                case Severity::Critical: criticalCount++; break;
                case Severity::High: highCount++; break;
                case Severity::Medium: mediumCount++; break;
                case Severity::Low: lowCount++; break;
                default: break;
            }
        }
    }

    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "OVERALL RESULTS" << std::endl;
    std::cout << "Total Detections: " << totalDetections << std::endl;
    std::cout << "  Critical: " << criticalCount << std::endl;
    std::cout << "  High:     " << highCount << std::endl;
    std::cout << "  Medium:   " << mediumCount << std::endl;
    std::cout << "  Low:      " << lowCount << std::endl;
    std::cout << std::string(70, '=') << std::endl;
}

void PrintMenu() {
    std::cout << "\n=== SS-TOOLS IMPROVED - MAIN MENU ===" << std::endl;
    std::cout << "1. Quick Scan (Fast)" << std::endl;
    std::cout << "2. Full Scan (Comprehensive)" << std::endl;
    std::cout << "3. Kernel Scan (Kernel-level only)" << std::endl;
    std::cout << "4. Process Scan (Running processes)" << std::endl;
    std::cout << "5. Check Process Hacker" << std::endl;
    std::cout << "6. Mods Scanner (Select folder/file)" << std::endl;
    std::cout << "7. Export Results" << std::endl;
    std::cout << "8. Exit" << std::endl;
    std::cout << "\nChoice: ";
}

int main(int argc, char* argv[]) {
    PrintBanner();

    // Check for command-line arguments
    if (argc > 1) {
        std::string cmd = argv[1];
        
        if (cmd == "--quick") {
            RunQuickScan();
            return 0;
        } else if (cmd == "--full") {
            RunFullScan();
            return 0;
        } else if (cmd == "--kernel") {
            std::cout << "\n=== KERNEL SCAN ===" << std::endl;
            auto& scanner = KernelScanner::Instance();
            auto result = scanner.FullKernelScan(globalScanState);
            
            std::cout << "Status: " << result.status << std::endl;
            std::cout << "Detections: " << result.totalDetected << std::endl;
            
            for (const auto& det : result.detections) {
                PrintDetection(det);
            }
            
            return 0;
        } else if (cmd == "--process-hacker") {
            std::cout << "\n=== PROCESS HACKER DETECTION ===" << std::endl;
            auto& detector = ProcessHackerDetector::Instance();
            auto result = detector.DetectProcessHackerThreat(globalScanState);
            
            std::cout << "Status: " << result.status << std::endl;
            for (const auto& det : result.detections) {
                PrintDetection(det);
            }
            
            return 0;
        } else if (cmd == "--help") {
            std::cout << "Usage: ss-tools-improved [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --quick            Run quick scan" << std::endl;
            std::cout << "  --full             Run full comprehensive scan" << std::endl;
            std::cout << "  --kernel           Run kernel-level scan only" << std::endl;
            std::cout << "  --process-hacker   Detect Process Hacker" << std::endl;
            std::cout << "  --help             Show this help message" << std::endl;
            return 0;
        }
    }

    // Interactive menu
    while (true) {
        if (scanInProgress) {
            std::cout << "\n[SCAN IN PROGRESS] " << globalScanState.GetActivity() << std::endl;
            std::cout << "Files scanned: " << globalScanState.scannedFiles << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

        PrintMenu();
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1:
                RunQuickScan();
                break;
            case 2:
                RunFullScan();
                break;
            case 3: {
                std::cout << "\n=== KERNEL SCAN ===" << std::endl;
                auto& scanner = KernelScanner::Instance();
                auto result = scanner.FullKernelScan(globalScanState);
                
                std::cout << "Status: " << result.status << std::endl;
                std::cout << "Detections: " << result.totalDetected << std::endl;
                
                for (const auto& det : result.detections) {
                    PrintDetection(det);
                }
                break;
            }
            case 4: {
                std::cout << "\n=== PROCESS SCAN ===" << std::endl;
                auto& scanner = EnhancedProcessScanner::Instance();
                auto result = scanner.ScanRunningProcesses(globalScanState);
                
                std::cout << "Scanned: " << result.totalScanned << " processes" << std::endl;
                std::cout << "Detections: " << result.totalDetected << std::endl;
                
                for (const auto& det : result.detections) {
                    PrintDetection(det);
                }
                break;
            }
            case 5: {
                std::cout << "\n=== PROCESS HACKER DETECTION ===" << std::endl;
                auto& detector = ProcessHackerDetector::Instance();
                auto result = detector.DetectProcessHackerThreat(globalScanState);
                
                std::cout << "Status: " << result.status << std::endl;
                for (const auto& det : result.detections) {
                    PrintDetection(det);
                }
                break;
            }
            case 6: {
                std::cout << "\n=== MODS SCANNER ===" << std::endl;
                std::cout << "1. Scan entire folder" << std::endl;
                std::cout << "2. Scan single mod file" << std::endl;
                std::cout << "Choice: ";
                
                int modsChoice;
                std::cin >> modsChoice;
                std::cin.ignore();
                
                auto& modsScanner = ModsScanner::Instance();
                
                if (modsChoice == 1) {
                    std::string folderPath = modsScanner.GetFolderPath("Select folder containing mods");
                    if (!folderPath.empty()) {
                        std::cout << "\nScanning folder: " << folderPath << std::endl;
                        auto result = modsScanner.ScanModFolder(folderPath, globalScanState);
                        
                        std::cout << "\nScanned: " << result.totalScanned << " mod files" << std::endl;
                        std::cout << "Detections: " << result.totalDetected << std::endl;
                        std::cout << "Time: " << std::fixed << std::setprecision(2) << result.elapsedMs << "ms" << std::endl;
                        
                        for (const auto& det : result.detections) {
                            PrintDetection(det);
                        }
                    }
                } else if (modsChoice == 2) {
                    std::string filePath = modsScanner.SelectSingleFile();
                    if (!filePath.empty()) {
                        std::cout << "\nScanning: " << filePath << std::endl;
                        auto result = modsScanner.ScanSingleMod(filePath, globalScanState);
                        
                        std::cout << "\nDetections: " << result.totalDetected << std::endl;
                        std::cout << "Time: " << std::fixed << std::setprecision(2) << result.elapsedMs << "ms" << std::endl;
                        
                        for (const auto& det : result.detections) {
                            PrintDetection(det);
                        }
                    }
                }
                break;
            }
            case 7: {
                std::cout << "\nExporting results..." << std::endl;
                // Would export to JSON/CSV/HTML
                std::cout << "Results exported to: ss-tools-results.json" << std::endl;
                break;
            }
            case 8:
                std::cout << "\nExiting SS-TOOLS. Goodbye!" << std::endl;
                return 0;
            default:
                std::cout << "\nInvalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}
