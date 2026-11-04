#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Trie {
private:
    // Stores children and matching title indexes.
    struct TrieNode {
        std::unordered_map<char, std::unique_ptr<TrieNode>> children;
        std::vector<std::size_t> titleIndexes;
    };

    TrieNode root;

    // Converts text into lowercase searchable text.
    static std::string normalize(const std::string& text);

    // Collects indexes beneath the current node.
    void collectSuggestions(const TrieNode* node, std::vector<std::size_t>& results, std::size_t limit) const;

public:
    // Adds one title and its index.
    void insert(const std::string& title, std::size_t titleIndex);

    // Returns indexes matching the provided prefix.
    std::vector<std::size_t> searchList(const std::string& prefix, std::size_t limit) const;

    // Checks whether the trie contains titles.
    bool empty() const;
};