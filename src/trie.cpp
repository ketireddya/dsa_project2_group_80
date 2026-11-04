#include "Trie.h"

#include <cctype> 
// For std::tolower

// Converts text into lowercase searchable text.
std::string Trie::normalize(const std::string& text) {
    std::string normalized;
    normalized.reserve(text.size());

    for (unsigned char character : text) {
        if (std::isalnum(character)) {
            normalized += static_cast<char>(std::tolower(character));
        }
        else if (std::isspace(character) && !normalized.empty() && normalized.back() != ' ') {
            normalized += ' ';
        }
    }

    if (!normalized.empty() && normalized.back() == ' ') {
        normalized.pop_back();
    }

    return normalized;
}

// Adds one title and its index.
void Trie::insert(const std::string& title, std::size_t titleIndex) {
    std::string normalizedTitle = normalize(title);

    if (normalizedTitle.empty()) {
        return;
    }

    TrieNode* currentNode = &root;

    for (char character : normalizedTitle) {
        std::unique_ptr<TrieNode>& child = currentNode->children[character];

        if (!child) {
            child = std::make_unique<TrieNode>();
        }

        currentNode = child.get();
    }

    currentNode->titleIndexes.push_back(titleIndex);
}

// Collects indexes beneath the current node.
void Trie::collectSuggestions(const TrieNode* node, std::vector<std::size_t>& results, std::size_t limit) const {
    if (node == nullptr || results.size() >= limit) {
        return;
    }

    for (std::size_t titleIndex : node->titleIndexes) {
        if (results.size() >= limit) {
            return;
        }

        results.push_back(titleIndex);
    }

    for (const auto& childPair : node->children) {
        if (results.size() >= limit) {
            return;
        }

        collectSuggestions(childPair.second.get(), results, limit);
    }
}

// Returns indexes matching the provided prefix.
std::vector<std::size_t> Trie::searchList(const std::string& query, std::size_t limit) const {
    std::vector<std::size_t> results;

    if (limit == 0) {
        return results;
    }

    std::string normalizedPrefix = normalize(query);

    if (normalizedPrefix.empty()) {
        return results;
    }

    const TrieNode* currentNode = &root;

    for (char character : normalizedPrefix) {
        auto child = currentNode->children.find(character);

        if (child == currentNode->children.end()) {
            return results;
        }

        currentNode = child->second.get();
    }

    collectSuggestions(currentNode, results, limit);

    return results;
}

// Checks whether the trie contains titles.
bool Trie::empty() const {
    return root.children.empty();
}