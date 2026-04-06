#pragma once

#include "core.h"
#include <vector>
#include <string>
#include <memory>
#include <functional>

namespace SSTools {
namespace UI {

// ═══════════════════════════════════════════════════════════════════════════
// IMGUI INTERFACE - Modern UI Layer
// ═══════════════════════════════════════════════════════════════════════════

class ImGuiInterface {
public:
    static ImGuiInterface& Instance();
    
    // Initialize ImGUI with custom theme
    void Initialize();
    
    // Shutdown ImGUI
    void Shutdown();
    
    // Render main window
    void RenderMainWindow(bool& isOpen);
    
    // Render scanner results
    void RenderResultsPanel(const std::vector<ScanResult>& results);
    
    // Render detection details
    void RenderDetectionDetail(const Detection& detection);
    
    // Render progress bar and activity
    void RenderProgressPanel(const ScanState& state);
    
    // Render kernel scan results
    void RenderKernelScanResults(const ScanResult& result);
    
    // Render process list
    void RenderProcessList(const std::vector<ProcessInfo>& processes);
    
    // Render settings panel
    void RenderSettingsPanel();
    
    // Render help/about panel
    void RenderAboutPanel();
    
    // Show modal dialog
    void ShowModal(const std::string& title, const std::string& message);
    
    // Show error dialog
    void ShowErrorDialog(const std::string& title, const std::string& error);
    
    // Show success notification
    void ShowNotification(const std::string& message, float duration = 3.0f);
    
    // Get ImGUI context
    ImGuiContext* GetContext() const;

private:
    ImGuiInterface() = default;
    ~ImGuiInterface() = default;
    
    void SetupTheme();
    void SetupStyleColors();
    void SetupFonts();
    
    ImGuiContext* context;
    std::string currentTab;
    bool showSettings;
    bool showAbout;
};

// ═══════════════════════════════════════════════════════════════════════════
// RESULTS VIEWER - Advanced result visualization
// ═══════════════════════════════════════════════════════════════════════════

class ResultsViewer {
public:
    static ResultsViewer& Instance();
    
    // Add scan result
    void AddScanResult(const ScanResult& result);
    
    // Clear results
    void ClearResults();
    
    // Get all results
    const std::vector<ScanResult>& GetAllResults() const;
    
    // Filter results by severity
    std::vector<Detection> GetDetectionsByServeity(Severity severity);
    
    // Filter results by scanner
    std::vector<Detection> GetDetectionsByScanner(const std::string& scannerName);
    
    // Export results to JSON
    std::string ExportToJSON() const;
    
    // Export results to CSV
    std::string ExportToCSV() const;
    
    // Export results to HTML report
    std::string ExportToHTML() const;
    
    // Get statistics
    struct Statistics {
        int totalDetections;
        int criticalCount;
        int highCount;
        int mediumCount;
        int lowCount;
        int falsePositiveCount;
        float averageConfidence;
    };
    
    Statistics GetStatistics() const;
    
    // Render detection table
    void RenderDetectionTable(const std::vector<Detection>& detections);

private:
    ResultsViewer() = default;
    ~ResultsViewer() = default;
    
    std::vector<ScanResult> results;
};

// ═══════════════════════════════════════════════════════════════════════════
// SCAN MANAGER - Orchestrates all scanners
// ═══════════════════════════════════════════════════════════════════════════

class ScanManager {
public:
    static ScanManager& Instance();
    
    // Start quick scan
    void StartQuickScan();
    
    // Start full scan
    void StartFullScan();
    
    // Start custom scan with specific scanners
    void StartCustomScan(const std::vector<ScannerType>& scanners);
    
    // Cancel current scan
    void CancelScan();
    
    // Check if scan is running
    bool IsScanRunning() const;
    
    // Get current scan progress
    float GetScanProgress() const;
    
    // Get current activity
    std::string GetCurrentActivity() const;
    
    // Get scan results
    const std::vector<ScanResult>& GetScanResults() const;
    
    // Set scan callback
    void SetScanCompleteCallback(std::function<void()> callback);
    
    // Run kernel scan
    ScanResult RunKernelScan();
    
    // Run process scan
    ScanResult RunProcessScan();
    
    // Run memory scan
    ScanResult RunMemoryScan();
    
    // Run mod scan
    ScanResult RunModScan();
    
    // Run DLL scan
    ScanResult RunDLLScan();
    
    // Run temp/prefetch scan
    ScanResult RunTempPrefetchScan();
    
    // Run all scanners
    std::vector<ScanResult> RunAllScanners();

private:
    ScanManager() = default;
    ~ScanManager() = default;
    
    bool scanRunning;
    ScanState currentState;
    std::vector<ScanResult> results;
    std::function<void()> completeCallback;
};

// ═══════════════════════════════════════════════════════════════════════════
// REPORT GENERATOR - Create detailed reports
// ═══════════════════════════════════════════════════════════════════════════

class ReportGenerator {
public:
    static ReportGenerator& Instance();
    
    // Generate detailed scan report
    std::string GenerateScanReport(const std::vector<ScanResult>& results);
    
    // Generate kernel detection report
    std::string GenerateKernelReport(const ScanResult& kernelResult);
    
    // Generate process report
    std::string GenerateProcessReport(const std::vector<ProcessInfo>& processes);
    
    // Generate executive summary
    std::string GenerateExecutiveSummary(const std::vector<ScanResult>& results);
    
    // Save report to file
    bool SaveReportToFile(const std::string& filePath, const std::string& report);
    
    // Generate timestamped report filename
    std::string GenerateReportFilename();

private:
    ReportGenerator() = default;
    ~ReportGenerator() = default;
    
    std::string SeverityToString(Severity sev);
    std::string GetCurrentDateTime();
};

} // namespace UI
} // namespace SSTools
