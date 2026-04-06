#pragma once

#include "core.h"
#include <vector>
#include <map>
#include <regex>

namespace SSTools {
namespace Detection {

// ═══════════════════════════════════════════════════════════════════════════
// FALSE POSITIVE FILTER - Advanced Context-Aware Filtering
// ═══════════════════════════════════════════════════════════════════════════

class FalsePositiveFilter {
public:
    static FalsePositiveFilter& Instance();
    
    // Filter detections to remove false positives
    std::vector<Detection> FilterDetections(const std::vector<Detection>& detections);
    
    // Analyze single detection for false positives
    bool IsFalsePositive(const Detection& detection, const std::string& fileContent);
    
    // Get whitelist of legitimate mods/tools
    const std::map<std::string, WhitelistEntry>& GetWhitelist() const;
    
    // Add custom whitelist entry
    void AddWhitelistEntry(const std::string& name, const WhitelistEntry& entry);
    
    // Check if file is whitelisted
    bool IsWhitelisted(const std::string& fileName);
    
    // Adjust severity based on context
    Severity AdjustSeverity(const Detection& detection, const std::string& context);
    
    // Analyze context to determine if detection is legitimate
    float CalculateConfidence(const Detection& detection, const std::string& context);

private:
    FalsePositiveFilter() = default;
    ~FalsePositiveFilter() = default;
    
    void InitializeWhitelist();
    bool CheckLegitimateModContext(const std::string& keyword, const std::string& context);
    bool CheckDevelopmentToolContext(const std::string& fileName, const std::string& context);
    bool CheckModMetadata(const std::string& content);
    
    // Context analyzers for specific keywords
    bool IsLegitimateSpeed(const std::string& context);
    bool IsLegitimateFly(const std::string& context);
    bool IsLegitimateReach(const std::string& context);
    bool IsLegitimateESP(const std::string& context);
    bool IsLegitimateTimer(const std::string& context);
    bool IsLegitimateStep(const std::string& context);
    
    std::map<std::string, WhitelistEntry> whitelistDB;
    
    // Legitimate mod patterns
    static const std::vector<std::string> LEGITIMATE_MOD_KEYWORDS;
    static const std::vector<std::string> LEGITIMATE_CONTEXTS;
    static const std::vector<std::string> DEVELOPMENT_KEYWORDS;
};

// ═══════════════════════════════════════════════════════════════════════════
// CONTEXT ANALYZER
// ═══════════════════════════════════════════════════════════════════════════

class ContextAnalyzer {
public:
    static ContextAnalyzer& Instance();
    
    // Extract keywords from content
    std::vector<std::string> ExtractKeywords(const std::string& content);
    
    // Analyze surrounding context of keyword
    std::string GetContext(const std::string& content, const std::string& keyword, int radius = 100);
    
    // Check if context indicates development/legitimate use
    bool IsDevelopmentContext(const std::string& context);
    
    // Check if context indicates cheat use
    bool IsCheatContext(const std::string& context);
    
    // Analyze file metadata
    std::map<std::string, std::string> AnalyzeFileMetadata(const std::string& filePath);
    
    // Parse fabric.json or mods.toml
    std::map<std::string, std::string> ParseModMetadata(const std::string& content);

private:
    ContextAnalyzer() = default;
    ~ContextAnalyzer() = default;
    
    static const std::vector<std::string> CHEAT_CONTEXT_KEYWORDS;
    static const std::vector<std::string> LEGITIMATE_CONTEXT_KEYWORDS;
};

// ═══════════════════════════════════════════════════════════════════════════
// CONFIDENCE SCORER
// ═══════════════════════════════════════════════════════════════════════════

class ConfidenceScorer {
public:
    static ConfidenceScorer& Instance();
    
    // Calculate overall detection confidence (0.0 - 1.0)
    float ScoreDetection(const Detection& detection, const std::string& context);
    
    // Score individual factors
    float ScoreKeywordContext(const std::string& keyword, const std::string& context);
    float ScoreFileLocation(const std::string& filePath);
    float ScoreFileName(const std::string& fileName);
    float ScoreBinarySignatures(const std::string& content);
    
    // Get confidence explanation
    std::string GetConfidenceExplanation(float confidence);

private:
    ConfidenceScorer() = default;
    ~ConfidenceScorer() = default;
    
    // Scoring weights
    static constexpr float KEYWORD_WEIGHT = 0.35f;
    static constexpr float LOCATION_WEIGHT = 0.20f;
    static constexpr float FILENAME_WEIGHT = 0.20f;
    static constexpr float SIGNATURE_WEIGHT = 0.25f;
};

} // namespace Detection
} // namespace SSTools
