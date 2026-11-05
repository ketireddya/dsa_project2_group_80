#include "Trie.h"
#include <cctype>

// lowercase so searching isn't case sensitive, non-English letters are left alone
static char fold(char c) {
    return (char)std::tolower((unsigned char)c);
}

int Trie::findChild(int node, char c) const {
    for (int kid = nodes_[node].firstChild; kid != -1; kid = nodes_[kid].nextSibling)
        if (nodes_[kid].ch == c) return kid;
    return -1;
}


int Trie::findOrAddChild(int node, char c) {
    int kid = findChild(node, c);
    if (kid != -1) return kid;

    // new child goes on the front of the parent's child list
    Node n;
    n.ch = c;
    n.nextSibling = nodes_[node].firstChild;
    nodes_.push_back(n);            
    int id = (int)nodes_.size() - 1;
    nodes_[node].firstChild = id;
    return id;
}


void Trie::insert(const std::string& name, int index) {
    int cur = 0;        // start at the root
    for (char c : name) cur = findOrAddChild(cur, fold(c));

    // mark this title ends here (linked list, since names can repeat)
    endIndex_.push_back(index);
    nextEnd_.push_back(nodes_[cur].firstEnd);
    nodes_[cur].firstEnd = (int)endIndex_.size() - 1;
    ++words_;
}

void Trie::build(const std::vector<Title>& titles) {
    nodes_.reserve(nodes_.size() + titles.size() * 4);   
    endIndex_.reserve(endIndex_.size() + titles.size());
    nextEnd_.reserve(nextEnd_.size() + titles.size());
    for (size_t i = 0; i < titles.size(); ++i) insert(titles[i].name, (int)i);
}



int Trie::walk(const std::string& s) const {
    int cur = 0;
    for (char c : s) {
        cur = findChild(cur, fold(c));
        if (cur == -1) return -1;       // nothing starts with this
    }
    return cur;
}

std::vector<int> Trie::exact(const std::string& name) const {
    std::vector<int> out;
    int node = walk(name);
    if (node == -1) return out;
    for (int e = nodes_[node].firstEnd; e != -1; e = nextEnd_[e]) out.push_back(endIndex_[e]);
    return out;
}



std::vector<int> Trie::startsWith(const std::string& prefix, size_t maxResults) const {
    std::vector<int> out;
    int start = walk(prefix);
    if (start == -1 || maxResults == 0) return out;


    // depth first search for all titles that start with this prefix
    //using our own stack instead of recursion to avoid stack overflow on very deep tries
    std::vector<int> stack = { start };
    while (!stack.empty()) {
        int node = stack.back();
        stack.pop_back();
        for (int e = nodes_[node].firstEnd; e != -1; e = nextEnd_[e]) {
            out.push_back(endIndex_[e]);
            if (out.size() >= maxResults) return out;
        }
        for (int kid = nodes_[node].firstChild; kid != -1; kid = nodes_[kid].nextSibling)
            stack.push_back(kid);
    }
    return out;
}
