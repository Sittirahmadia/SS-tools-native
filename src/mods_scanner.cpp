#include "../include/mods_scanner.h"
#include <windows.h>
#include <shlobj.h>
#include <filesystem>
#include <algorithm>
#include <regex>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")

namespace fs = std::filesystem;

namespace SSTools {
namespace ModScanning {

ModsScanner::ModsScanner() {}

// Check if file is a mod file (JAR, ZIP, class, etc.)
bool ModsScanner::IsModFile(const std::string& filePath) {
    std::string ext;
    size_t dotPos = filePath.find_last_of(".");
    if (dotPos != std::string::npos) {
        ext = filePath.substr(dotPos);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }
    
    // Supported mod file extensions
    static const std::vector<std::string> modExtensions = {
        ".jar", ".zip", ".class", ".mod", ".litemod", ".fcmod"
    };
    
    return std::find(modExtensions.begin(), modExtensions.end(), ext) != modExtensions.end();
}

// Get file size in human-readable format
std::string ModsScanner::GetFileSize(const std::string& filePath) {
    try {
        auto fileSize = fs::file_size(filePath);
        
        if (fileSize < 1024) {
            return std::to_string(fileSize) + " B";
        } else if (fileSize < 1024 * 1024) {
            return std::to_string(fileSize / 1024) + " KB";
        } else if (fileSize < 1024 * 1024 * 1024) {
            return std::to_string(fileSize / (1024 * 1024)) + " MB";
        } else {
            return std::to_string(fileSize / (1024 * 1024 * 1024)) + " GB";
        }
    } catch (...) {
        return "Unknown";
    }
}

// Get last modified time
std::string ModsScanner::GetLastModifiedTime(const std::string& filePath) {
    try {
        auto lastWriteTime = fs::last_write_time(filePath);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            lastWriteTime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        auto tt = std::chrono::system_clock::to_time_t(sctp);
        
        char buffer[26];
        ctime_s(buffer, sizeof(buffer), &tt);
        std::string timeStr(buffer);
        
        // Remove newline
        if (!timeStr.empty() && timeStr.back() == '\n') {
            timeStr.pop_back();
        }
        return timeStr;
    } catch (...) {
        return "Unknown";
    }
}

// Extract mod metadata from JAR/ZIP
std::string ModsScanner::ExtractModMetadata(const std::string& filePath) {
    try {
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) return "";
        
        // Read first 512 bytes for metadata
        std::vector<char> buffer(512);
        file.read(buffer.data(), buffer.size());
        
        // Search for common mod identifiers
        std::string data(buffer.begin(), buffer.end());
        
        if (data.find("modid") != std::string::npos) return "Forge Mod";
        if (data.find("fabric") != std::string::npos) return "Fabric Mod";
        if (data.find("litemod") != std::string::npos) return "LiteMod";
        if (data.find("forge") != std::string::npos) return "Forge Library";
        
        return "Generic Mod";
    } catch (...) {
        return "Unknown";
    }
}

// Analyze a single mod file
std::vector<Detection> ModsScanner::AnalyzeMod(const std::string& filePath, ScanState& state) {
    std::vector<Detection> detections;
    state.SetActivity("Analyzing: " + fs::path(filePath).filename().string());
    
    try {
        // Read file content for keyword scanning
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) return detections;
        
        std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::string content(buffer.begin(), buffer.end());
        
        // Suspicious keywords/patterns
        struct SuspiciousPattern {
            std::string keyword;
            std::string description;
            Severity severity;
        };
        
        static const std::vector<SuspiciousPattern> patterns = {
            {"VirtualAllocEx", "Memory allocation for injection", Severity::Critical},
            {"WriteProcessMemory", "Process memory writing", Severity::Critical},
            {"CreateRemoteThread", "Remote thread creation", Severity::Critical},
            {"GetModuleHandle", "Module enumeration", Severity::High},
            {"CreateProcess", "Process creation", Severity::High},
            {"RegOpenKey", "Registry access", Severity::Medium},
            {"SetWindowsHookEx", "Windows hook installation", Severity::Critical},
            {"UnhookWindowsHookEx", "Hook removal/hiding", Severity::Critical},
            {"keybd_event", "Keyboard simulation", Severity::Medium},
            {"mouse_event", "Mouse simulation", Severity::Medium},
            {"send message", "Window messaging", Severity::Low},
            {"readprocessmemory", "Memory reading", Severity::High},
            {"shellexecute", "Shell command execution", Severity::High},
        };
        
        // Check for suspicious patterns
        for (const auto& pattern : patterns) {
            std::regex regex(pattern.keyword, std::regex::icase);
            if (std::regex_search(content, regex)) {
                Detection det;
                det.scanner = "ModsScanner";
                det.filePath = filePath;
                det.description = "Suspicious pattern detected: " + pattern.description;
                det.severity = pattern.severity;
                det.confidence = 0.65f;
                det.timestamp = std::to_string(std::time(nullptr));
                detections.push_back(det);
            }
        }
        
        file.close();
        
        // Check file size anomalies
        auto fileSize = fs::file_size(filePath);
        if (fileSize > 100 * 1024 * 1024) { // > 100MB
            Detection det;
            det.scanner = "ModsScanner";
            det.filePath = filePath;
            det.description = "Unusually large mod file (" + std::to_string(fileSize / (1024 * 1024)) + " MB)";
            det.severity = Severity::Medium;
            det.confidence = 0.5f;
            det.timestamp = std::to_string(std::time(nullptr));
            detections.push_back(det);
        }
        
    } catch (const std::exception& e) {
        // Log error but continue
        std::cerr << "Error analyzing mod: " << e.what() << std::endl;
    }
    
    return detections;
}

// Browse folder and return list of files
std::vector<std::string> ModsScanner::BrowseFolder(const std::string& startPath) {
    std::vector<std::string> files;
    
    try {
        std::string path = startPath.empty() ? "." : startPath;
        
        for (const auto& entry : fs::directory_iterator(path)) {
            if (fs::is_regular_file(entry) && IsModFile(entry.path().string())) {
                files.push_back(entry.path().string());
            }
        }
        
        std::sort(files.begin(), files.end());
    } catch (const std::exception& e) {
        std::cerr << "Error browsing folder: " << e.what() << std::endl;
    }
    
    return files;
}

// Get folder path from user (simplified - returns current directory for demo)
std::string ModsScanner::GetFolderPath(const std::string& prompt) {
    std::cout << "\n" << prompt << std::endl;
    std::cout << "Enter folder path (or press Enter for current directory): ";
    
    std::string path;
    std::getline(std::cin, path);
    
    if (path.empty()) {
        path = ".";
    }
    
    if (fs::exists(path) && fs::is_directory(path)) {
        return fs::absolute(path).string();
    }
    
    std::cout << "Invalid path!" << std::endl;
    return "";
}

// List all mods in a folder
std::vector<ModFileInfo> ModsScanner::ListModsInFolder(const std::string& folderPath) {
    std::vector<ModFileInfo> modList;
    
    if (folderPath.empty() || !fs::exists(folderPath)) {
        return modList;
    }
    
    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (fs::is_regular_file(entry) && IsModFile(entry.path().string())) {
                ModFileInfo info;
                info.fileName = entry.path().filename().string();
                info.filePath = entry.path().string();
                info.fileSize = GetFileSize(entry.path().string());
                info.lastModified = GetLastModifiedTime(entry.path().string());
                info.isSelected = false;
                modList.push_back(info);
            }
        }
        
        std::sort(modList.begin(), modList.end(), 
            [](const ModFileInfo& a, const ModFileInfo& b) {
                return a.fileName < b.fileName;
            });
    } catch (const std::exception& e) {
        std::cerr << "Error listing mods: " << e.what() << std::endl;
    }
    
    return modList;
}

// Select a single file
std::string ModsScanner::SelectSingleFile(const std::string& folderPath) {
    std::string path = folderPath.empty() ? GetFolderPath("Select folder containing mods") : folderPath;
    
    if (path.empty()) {
        return "";
    }
    
    auto modList = ListModsInFolder(path);
    
    if (modList.empty()) {
        std::cout << "\nNo mod files found in: " << path << std::endl;
        return "";
    }
    
    std::cout << "\n=== Select Single Mod ===" << std::endl;
    std::cout << "Available mods:" << std::endl;
    
    for (size_t i = 0; i < modList.size(); ++i) {
        std::cout << i + 1 << ". " << modList[i].fileName 
                  << " (" << modList[i].fileSize << ")" << std::endl;
    }
    
    std::cout << "\nEnter number (0 to cancel): ";
    size_t choice;
    std::cin >> choice;
    std::cin.ignore();
    
    if (choice > 0 && choice <= modList.size()) {
        return modList[choice - 1].filePath;
    }
    
    return "";
}

// Scan single mod file
ScanResult ModsScanner::ScanSingleMod(const std::string& filePath, ScanState& state) {
    ScanResult result;
    result.scannerName = "ModsScanner (Single)";
    result.status = "scanning";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        result.detections = AnalyzeMod(filePath, state);
        result.totalScanned = 1;
        result.totalDetected = result.detections.size();
        result.status = result.totalDetected == 0 ? "clean" : "detected";
    } catch (const std::exception& e) {
        result.status = "error";
        result.error = e.what();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    return result;
}

// Scan mod folder
ScanResult ModsScanner::ScanModFolder(const std::string& folderPath, ScanState& state) {
    ScanResult result;
    result.scannerName = "ModsScanner (Folder)";
    result.status = "scanning";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        auto modList = ListModsInFolder(folderPath);
        result.totalScanned = modList.size();
        
        for (const auto& mod : modList) {
            if (state.IsCancelRequested()) {
                result.status = "cancelled";
                break;
            }
            
            auto detections = AnalyzeMod(mod.filePath, state);
            result.detections.insert(result.detections.end(), detections.begin(), detections.end());
            state.scannedFiles++;
        }
        
        result.totalDetected = result.detections.size();
        result.status = result.totalDetected == 0 ? "clean" : "detected";
    } catch (const std::exception& e) {
        result.status = "error";
        result.error = e.what();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    return result;
}

// Scan multiple selected mods
std::vector<ScanResult> ModsScanner::ScanMultipleSelectedMods(const std::vector<std::string>& filePaths, ScanState& state) {
    std::vector<ScanResult> results;
    
    for (const auto& filePath : filePaths) {
        if (state.IsCancelRequested()) break;
        results.push_back(ScanSingleMod(filePath, state));
    }
    
    return results;
}

// Select mod with checkbox interface
void ModsScanner::SelectMod(const std::string& filePath, bool selected) {
    auto it = std::find_if(selectedMods.begin(), selectedMods.end(),
        [&filePath](const ModFileInfo& info) { return info.filePath == filePath; });
    
    if (it != selectedMods.end()) {
        it->isSelected = selected;
    } else if (selected) {
        ModFileInfo info;
        info.filePath = filePath;
        info.fileName = fs::path(filePath).filename().string();
        info.fileSize = GetFileSize(filePath);
        info.lastModified = GetLastModifiedTime(filePath);
        info.isSelected = true;
        selectedMods.push_back(info);
    }
}

// Clear all selections
void ModsScanner::ClearSelection() {
    selectedMods.clear();
}

} // namespace ModScanning
} // namespace SSTools
