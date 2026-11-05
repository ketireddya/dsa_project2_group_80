#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include "Title.h"

// prefix tree over title names for search / autocomplete
// case wont matter, ex. "star w" will find "Star Wars"
// it stores positions into the list it was built from so we can reorder that list after building
// (build it from the full loaded list which never gets sorted)

class Trie {
public:
    // adds every title in the list, titles[i] is stored as position i
    void build(const std::vector<Title>& titles);

    // adds one name and remembers which position it came from
    void insert(const std::string& name, int index);

    // positions of every title whose name starts with prefix (up to maxResults)
    std::vector<int> startsWith(const std::string& prefix,
                                size_t maxResults = (size_t)-1) const;

    // positions of every title whose name is exactly this (there can be several "Hamlet"s)
    std::vector<int> exact(const std::string& name) const;

    size_t nodeCount() const { return nodes_.size(); }
    size_t wordCount() const { return words_; }

private:
    // each node keeps its children as a linked list (first child, then next sibling)
    // it's smaller than a 256 slot array or a map per node which is important when we have 1 million+ titles
    struct Node {
        int firstChild = -1;   // -1 = no children
        int nextSibling = -1;  // -1 = last child of its parent
        int firstEnd = -1;     // first title that ends here, -1 = none
        char ch = 0;
    };

    std::vector<Node> nodes_{Node{}};   // nodes_[0] is the root
    std::vector<int> endIndex_;         // title position for each "ends here" entry
    std::vector<int> nextEnd_;          // next entry that ends at the same node, -1 = none
    size_t words_ = 0;

    int findChild(int node, char c) const;
    int findOrAddChild(int node, char c);
    int walk(const std::string& s) const;   // node for this text, -1 if it isn't in the trie
};

