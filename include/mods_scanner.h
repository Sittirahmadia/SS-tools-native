#pragma once

#include "core.h"
#include <vector>
#include <string>
#include <memory>

namespace SSTools {
namespace ModScanning {

// Mod file information
struct ModFileInfo {
    std::string fileName;
    std::string filePath;
    std::string fileSize;
    std::string lastModified;
    bool isSelected;
    std::vector<Detection> detections;
    
    ModFileInfo() : isSelected(false) {}
};

// Mods scanner folder/file selection
class ModsScanner {
public:
    static ModsScanner& Instance() {
        static ModsScanner instance;
        return instance;
    }

    // Folder operations
    std::vector<std::string> BrowseFolder(const std::string& startPath = "");
    std::string GetFolderPath(const std::string& prompt = "Select folder to scan");
    
    // File operations
    std::vector<ModFileInfo> ListModsInFolder(const std::string& folderPath);
    std::string SelectSingleFile(const std::string& folderPath = "");
    
    // Scanning operations
    ScanResult ScanModFolder(const std::string& folderPath, ScanState& state);
    ScanResult ScanSingleMod(const std::string& filePath, ScanState& state);
    std::vector<ScanResult> ScanMultipleSelectedMods(const std::vector<std::string>& filePaths, ScanState& state);
    
    // Utility
    std::vector<ModFileInfo> GetSelectedMods() const { return selectedMods; }
    void SelectMod(const std::string& filePath, bool selected = true);
    void ClearSelection();
    
private:
    ModsScanner();
    ~ModsScanner() = default;
    
    std::vector<ModFileInfo> selectedMods;
    
    // Helper functions
    bool IsModFile(const std::string& filePath);
    std::string GetFileSize(const std::string& filePath);
    std::string GetLastModifiedTime(const std::string& filePath);
    std::vector<Detection> AnalyzeMod(const std::string& filePath, ScanState& state);
    std::string ExtractModMetadata(const std::string& filePath);
};

} // namespace ModScanning
} // namespace SSTools
