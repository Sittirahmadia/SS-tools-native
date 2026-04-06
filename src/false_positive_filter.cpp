#include "../include/false_positive_filter.h"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace SSTools {
namespace Detection {

// Legitimate mod patterns database
const std::vector<std::string> FalsePositiveFilter::LEGITIMATE_MOD_KEYWORDS = {
    "optifine", "sodium", "lithium", "ferrocene", "starlight",
    "worldedit", "jei", "rei", "waila", "hwyla",
    "fabric", "forge", "liteloader",
    "mod", "library", "api", "core", "helper"
};

const std::vector<std::string> FalsePositiveFilter::LEGITIMATE_CONTEXTS = {
    "rendering", "optimization", "graphics", "performance",
    "fps", "shader", "texture", "recipe", "crafting",
    "item", "build", "development", "debug", "info", "tooltip"
};

const std::vector<std::string> FalsePositiveFilter::DEVELOPMENT_KEYWORDS = {
    "visual studio", "intellij", "eclipse", "gradle",
    "maven", "compiler", "debug", "test", "develop",
    "source", "code", "class", "method", "interface"
};

FalsePositiveFilter& FalsePositiveFilter::Instance() {
    static FalsePositiveFilter instance;
    static bool initialized = false;
    if (!initialized) {
        instance.InitializeWhitelist();
        initialized = true;
    }
    return instance;
}

void FalsePositiveFilter::InitializeWhitelist() {
    // OptiFine
    {
        WhitelistEntry entry;
        entry.name = "optifine";
        entry.keywords = {"speed", "shader", "texture", "optimization"};
        entry.contexts = {"rendering", "graphics", "fps", "performance"};
        entry.maxSeverity = Severity::Low;
        entry.isLegitimate = true;
        whitelistDB["optifine"] = entry;
    }

    // Sodium
    {
        WhitelistEntry entry;
        entry.name = "sodium";
        entry.keywords = {"speed", "optimization", "rendering"};
        entry.contexts = {"graphics", "fps", "performance", "mod"};
        entry.maxSeverity = Severity::Low;
        entry.isLegitimate = true;
        whitelistDB["sodium"] = entry;
    }

    // Lithium
    {
        WhitelistEntry entry;
        entry.name = "lithium";
        entry.keywords = {"optimization"};
        entry.contexts = {"performance", "server", "mod"};
        entry.maxSeverity = Severity::Low;
        entry.isLegitimate = true;
        whitelistDB["lithium"] = entry;
    }

    // WorldEdit
    {
        WhitelistEntry entry;
        entry.name = "worldedit";
        entry.keywords = {"reach", "speed", "step", "selection"};
        entry.contexts = {"edit", "build", "world", "command", "selection"};
        entry.maxSeverity = Severity::Low;
        entry.isLegitimate = true;
        whitelistDB["worldedit"] = entry;
    }

    // JEI (Just Enough Items)
    {
        WhitelistEntry entry;
        entry.name = "jei";
        entry.keywords = {"search", "item", "recipe"};
        entry.contexts = {"crafting", "recipe", "item", "mod"};
        entry.maxSeverity = Severity::Low;
        entry.isLegitimate = true;
        whitelistDB["jei"] = entry;
    }

    // WAILA (What Am I Looking At)
    {
        WhitelistEntry entry;
        entry.name = "waila";
        entry.keywords = {"esp", "info", "tooltip"};
        entry.contexts = {"tooltip", "info", "block", "debug", "development"};
        entry.maxSeverity = Severity::Low;
        entry.isLegitimate = true;
        whitelistDB["waila"] = entry;
    }
}

std::vector<Detection> FalsePositiveFilter::FilterDetections(const std::vector<Detection>& detections) {
    std::vector<Detection> filtered;

    for (auto det : detections) {
        // Check if whitelisted
        if (IsWhitelisted(det.filePath)) {
            det.isFalsePositive = true;
            det.falsePositiveReason = "File is whitelisted as legitimate";
            det.confidence = 0.1f;
            continue;
        }

        // Adjust severity and check if should be filtered
        Severity adjusted = AdjustSeverity(det, det.context);
        if (adjusted == Severity::None) {
            det.isFalsePositive = true;
            det.falsePositiveReason = "Context analysis suggests false positive";
            continue;
        }

        det.severity = adjusted;

        // Calculate confidence
        det.confidence = CalculateConfidence(det, det.context);

        // Filter out low-confidence detections
        if (det.confidence < 0.5f) {
            det.isFalsePositive = true;
            det.falsePositiveReason = "Confidence too low for legitimate detection";
            continue;
        }

        filtered.push_back(det);
    }

    return filtered;
}

bool FalsePositiveFilter::IsFalsePositive(const Detection& detection, const std::string& fileContent) {
    std::string descLower = detection.description;
    std::string contextLower = detection.context;
    std::transform(descLower.begin(), descLower.end(), descLower.begin(), ::tolower);
    std::transform(contextLower.begin(), contextLower.end(), contextLower.begin(), ::tolower);

    std::string contentLower = fileContent;
    std::transform(contentLower.begin(), contentLower.end(), contentLower.begin(), ::tolower);

    // Check for specific keyword false positives
    if (descLower.find("speed") != std::string::npos) {
        return IsLegitimateSpeed(contentLower);
    }
    if (descLower.find("fly") != std::string::npos) {
        return IsLegitimateFly(contentLower);
    }
    if (descLower.find("reach") != std::string::npos) {
        return IsLegitimateReach(contentLower);
    }
    if (descLower.find("esp") != std::string::npos) {
        return IsLegitimateESP(contentLower);
    }
    if (descLower.find("timer") != std::string::npos) {
        return IsLegitimateTimer(contentLower);
    }
    if (descLower.find("step") != std::string::npos) {
        return IsLegitimateStep(contentLower);
    }

    return false;
}

const std::map<std::string, WhitelistEntry>& FalsePositiveFilter::GetWhitelist() const {
    return whitelistDB;
}

void FalsePositiveFilter::AddWhitelistEntry(const std::string& name, const WhitelistEntry& entry) {
    whitelistDB[name] = entry;
}

bool FalsePositiveFilter::IsWhitelisted(const std::string& fileName) {
    std::string nameLower = fileName;
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

    for (const auto& [key, entry] : whitelistDB) {
        std::string keyLower = key;
        std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);

        if (nameLower.find(keyLower) != std::string::npos) {
            return true;
        }

        for (const auto& modName : entry.keywords) {
            std::string modLower = modName;
            std::transform(modLower.begin(), modLower.end(), modLower.begin(), ::tolower);
            if (nameLower.find(modLower) != std::string::npos) {
                return true;
            }
        }
    }

    return false;
}

Severity FalsePositiveFilter::AdjustSeverity(const Detection& detection, const std::string& context) {
    Severity baseSeverity = detection.severity;
    std::string contextLower = context;
    std::transform(contextLower.begin(), contextLower.end(), contextLower.begin(), ::tolower);

    // Check for development context
    if (CheckDevelopmentToolContext(detection.filePath, context)) {
        return Severity::None;  // Not a cheat in development context
    }

    // Check for legitimate mod context
    std::string descLower = detection.description;
    std::transform(descLower.begin(), descLower.end(), descLower.begin(), ::tolower);

    if (descLower.find("keyword") != std::string::npos) {
        // Check if keyword appears in legitimate context
        if (CheckLegitimateModContext(descLower, context)) {
            return Severity::Low;
        }
    }

    return baseSeverity;
}

float FalsePositiveFilter::CalculateConfidence(const Detection& detection, const std::string& context) {
    float confidence = 0.5f;

    std::string descLower = detection.description;
    std::string contextLower = context;
    std::transform(descLower.begin(), descLower.end(), descLower.begin(), ::tolower);
    std::transform(contextLower.begin(), contextLower.end(), contextLower.begin(), ::tolower);

    // Higher confidence for critical signatures
    if (descLower.find("cheat") != std::string::npos ||
        descLower.find("inject") != std::string::npos ||
        descLower.find("bypass") != std::string::npos) {
        confidence = 0.85f;
    }

    // Lower confidence for generic keywords
    if (descLower.find("speed") != std::string::npos ||
        descLower.find("timer") != std::string::npos ||
        descLower.find("search") != std::string::npos) {
        confidence = 0.4f;
    }

    // Adjust based on file location
    std::string pathLower = detection.filePath;
    std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(), ::tolower);

    if (pathLower.find("minecraft") != std::string::npos ||
        pathLower.find("launcher") != std::string::npos) {
        confidence *= 1.2f;  // Higher in suspicious locations
    }

    if (pathLower.find("system32") != std::string::npos ||
        pathLower.find("program files") != std::string::npos) {
        confidence *= 0.3f;  // Lower in legitimate system locations
    }

    // Clamp to 0-1 range
    if (confidence > 1.0f) confidence = 1.0f;
    if (confidence < 0.0f) confidence = 0.0f;

    return confidence;
}

bool FalsePositiveFilter::CheckLegitimateModContext(const std::string& keyword, const std::string& context) {
    std::string contextLower = context;
    std::transform(contextLower.begin(), contextLower.end(), contextLower.begin(), ::tolower);

    for (const auto& legitContext : LEGITIMATE_CONTEXTS) {
        std::string ctxLower = legitContext;
        std::transform(ctxLower.begin(), ctxLower.end(), ctxLower.begin(), ::tolower);
        if (contextLower.find(ctxLower) != std::string::npos) {
            return true;
        }
    }

    return false;
}

bool FalsePositiveFilter::CheckDevelopmentToolContext(const std::string& fileName, const std::string& context) {
    std::string filenameLower = fileName;
    std::string contextLower = context;
    std::transform(filenameLower.begin(), filenameLower.end(), filenameLower.begin(), ::tolower);
    std::transform(contextLower.begin(), contextLower.end(), contextLower.begin(), ::tolower);

    // Check filename for development tools
    for (const auto& devKeyword : DEVELOPMENT_KEYWORDS) {
        std::string keyLower = devKeyword;
        std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);
        if (filenameLower.find(keyLower) != std::string::npos) {
            return true;
        }
    }

    // Check context
    if (contextLower.find("development") != std::string::npos ||
        contextLower.find("debug") != std::string::npos ||
        contextLower.find("compiler") != std::string::npos) {
        return true;
    }

    return false;
}

bool FalsePositiveFilter::CheckModMetadata(const std::string& content) {
    // Check for fabric.json or mods.toml patterns
    if (content.find("\"fabric\"") != std::string::npos ||
        content.find("\"modid\"") != std::string::npos ||
        content.find("[mods]") != std::string::npos) {
        return true;
    }
    return false;
}

bool FalsePositiveFilter::IsLegitimateSpeed(const std::string& context) {
    if (context.find("optimization") != std::string::npos ||
        context.find("rendering") != std::string::npos ||
        context.find("fps") != std::string::npos ||
        context.find("shader") != std::string::npos ||
        context.find("optifine") != std::string::npos) {
        return true;
    }
    return false;
}

bool FalsePositiveFilter::IsLegitimateFly(const std::string& context) {
    if (context.find("elytra") != std::string::npos ||
        context.find("creative") != std::string::npos ||
        context.find("butterfly") != std::string::npos ||
        context.find("firefly") != std::string::npos) {
        return true;
    }
    return false;
}

bool FalsePositiveFilter::IsLegitimateReach(const std::string& context) {
    if (context.find("worldedit") != std::string::npos ||
        context.find("build") != std::string::npos ||
        context.find("selection") != std::string::npos ||
        context.find("interaction") != std::string::npos) {
        return true;
    }
    return false;
}

bool FalsePositiveFilter::IsLegitimateESP(const std::string& context) {
    if (context.find("waila") != std::string::npos ||
        context.find("hwyla") != std::string::npos ||
        context.find("debug") != std::string::npos ||
        context.find("development") != std::string::npos ||
        context.find("tooltip") != std::string::npos) {
        return true;
    }
    return false;
}

bool FalsePositiveFilter::IsLegitimateTimer(const std::string& context) {
    if (context.find("timeout") != std::string::npos ||
        context.find("scheduler") != std::string::npos ||
        context.find("task") != std::string::npos ||
        context.find("delay") != std::string::npos ||
        context.find("tick") != std::string::npos) {
        return true;
    }
    return false;
}

bool FalsePositiveFilter::IsLegitimateStep(const std::string& context) {
    if (context.find("stepper") != std::string::npos) {
        return true;  // Not "steppinghack"
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// CONTEXT ANALYZER
// ═══════════════════════════════════════════════════════════════════════════

const std::vector<std::string> ContextAnalyzer::CHEAT_CONTEXT_KEYWORDS = {
    "hack", "cheat", "module", "toggle", "enable", "disable",
    "activate", "deactivate", "keybind", "hotkey", "prefix"
};

const std::vector<std::string> ContextAnalyzer::LEGITIMATE_CONTEXT_KEYWORDS = {
    "recipe", "crafting", "item", "build", "worldedit",
    "development", "debug", "test", "optimization", "performance",
    "mod", "api", "library", "core", "helper"
};

ContextAnalyzer& ContextAnalyzer::Instance() {
    static ContextAnalyzer instance;
    return instance;
}

std::vector<std::string> ContextAnalyzer::ExtractKeywords(const std::string& content) {
    std::vector<std::string> keywords;
    std::istringstream iss(content);
    std::string word;

    while (iss >> word) {
        if (word.length() > 3) {  // Only consider words longer than 3 chars
            keywords.push_back(word);
        }
    }

    return keywords;
}

std::string ContextAnalyzer::GetContext(const std::string& content, const std::string& keyword, int radius) {
    size_t pos = content.find(keyword);
    if (pos == std::string::npos) {
        return "";
    }

    size_t start = (pos > radius) ? (pos - radius) : 0;
    size_t end = std::min(pos + keyword.length() + radius, content.length());

    return content.substr(start, end - start);
}

bool ContextAnalyzer::IsDevelopmentContext(const std::string& context) {
    std::string contextLower = context;
    std::transform(contextLower.begin(), contextLower.end(), contextLower.begin(), ::tolower);

    for (const auto& keyword : LEGITIMATE_CONTEXT_KEYWORDS) {
        if (contextLower.find(keyword) != std::string::npos) {
            return true;
        }
    }

    return false;
}

bool ContextAnalyzer::IsCheatContext(const std::string& context) {
    std::string contextLower = context;
    std::transform(contextLower.begin(), contextLower.end(), contextLower.begin(), ::tolower);

    for (const auto& keyword : CHEAT_CONTEXT_KEYWORDS) {
        if (contextLower.find(keyword) != std::string::npos) {
            return true;
        }
    }

    return false;
}

std::map<std::string, std::string> ContextAnalyzer::AnalyzeFileMetadata(const std::string& filePath) {
    std::map<std::string, std::string> metadata;
    // Would parse file metadata here
    return metadata;
}

std::map<std::string, std::string> ContextAnalyzer::ParseModMetadata(const std::string& content) {
    std::map<std::string, std::string> metadata;
    // Would parse JSON/TOML here
    return metadata;
}

// ═══════════════════════════════════════════════════════════════════════════
// CONFIDENCE SCORER
// ═══════════════════════════════════════════════════════════════════════════

ConfidenceScorer& ConfidenceScorer::Instance() {
    static ConfidenceScorer instance;
    return instance;
}

float ConfidenceScorer::ScoreDetection(const Detection& detection, const std::string& context) {
    float score = 0.0f;

    score += ScoreKeywordContext(detection.description, context) * KEYWORD_WEIGHT;
    score += ScoreFileLocation(detection.filePath) * LOCATION_WEIGHT;
    score += ScoreFileName(detection.filePath) * FILENAME_WEIGHT;
    score += ScoreBinarySignatures(context) * SIGNATURE_WEIGHT;

    return score;
}

float ConfidenceScorer::ScoreKeywordContext(const std::string& keyword, const std::string& context) {
    std::string ctxLower = context;
    std::transform(ctxLower.begin(), ctxLower.end(), ctxLower.begin(), ::tolower);

    // High score for obvious cheat keywords in cheat context
    if ((keyword.find("cheat") != std::string::npos || keyword.find("hack") != std::string::npos) &&
        (ctxLower.find("cheat") != std::string::npos || ctxLower.find("hack") != std::string::npos)) {
        return 1.0f;
    }

    // Low score for generic keywords
    if (keyword.find("speed") != std::string::npos || keyword.find("timer") != std::string::npos) {
        return 0.3f;
    }

    return 0.6f;
}

float ConfidenceScorer::ScoreFileLocation(const std::string& filePath) {
    std::string pathLower = filePath;
    std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(), ::tolower);

    // Suspicious locations
    if (pathLower.find("minecraft") != std::string::npos) return 0.9f;
    if (pathLower.find("appdata") != std::string::npos) return 0.8f;
    if (pathLower.find("temp") != std::string::npos) return 0.7f;
    if (pathLower.find("downloads") != std::string::npos) return 0.8f;

    // Legitimate locations
    if (pathLower.find("system32") != std::string::npos) return 0.2f;
    if (pathLower.find("program files") != std::string::npos) return 0.3f;

    return 0.5f;
}

float ConfidenceScorer::ScoreFileName(const std::string& fileName) {
    std::string nameLower = fileName;
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

    // Suspicious filenames
    if (nameLower.find("cheat") != std::string::npos) return 0.95f;
    if (nameLower.find("hack") != std::string::npos) return 0.9f;
    if (nameLower.find("inject") != std::string::npos) return 0.9f;
    if (nameLower.find("wurst") != std::string::npos) return 0.99f;
    if (nameLower.find("meteor") != std::string::npos) return 0.95f;

    return 0.5f;
}

float ConfidenceScorer::ScoreBinarySignatures(const std::string& content) {
    std::string contentLower = content;
    std::transform(contentLower.begin(), contentLower.end(), contentLower.begin(), ::tolower);

    // Hard API signatures
    if (contentLower.find("createremotethread") != std::string::npos) return 0.95f;
    if (contentLower.find("writeprocessmemory") != std::string::npos) return 0.95f;
    if (contentLower.find("-javaagent") != std::string::npos) return 0.9f;

    return 0.5f;
}

std::string ConfidenceScorer::GetConfidenceExplanation(float confidence) {
    if (confidence > 0.9f) return "Very High Confidence - Likely Cheat";
    if (confidence > 0.7f) return "High Confidence - Probably Cheat";
    if (confidence > 0.5f) return "Medium Confidence - Possible Cheat";
    if (confidence > 0.3f) return "Low Confidence - Uncertain";
    return "Very Low Confidence - Likely False Positive";
}

} // namespace Detection
} // namespace SSTools
