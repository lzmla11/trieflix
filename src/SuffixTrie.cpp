#include "../include/SuffixTrie.h"

using namespace std;

SuffixTrie::SuffixTrie() : root(make_unique<SuffixTrieNode>()) {}

vector<string> SuffixTrie::generateSuffixes(const string& word) const {
    vector<string> suffixes;
    for (size_t i = 0; i < word.size(); i++) {
        string suffix = word.substr(i);
        if (suffix.size() >= 3)
            suffixes.push_back(suffix);
    }
    return suffixes;
}

void SuffixTrie::insert(const string& word, int movieId) {
    for (const string& suffix : generateSuffixes(word)) {
        SuffixTrieNode* cur = root.get();
        for (char c : suffix) {
            if (!cur->children.count(c))
                cur->children[c] = make_unique<SuffixTrieNode>();
            cur = cur->children[c].get();
        }
        // unordered_set ignora duplicados automáticamente
        cur->movieIds.insert(movieId);
    }
}

vector<int> SuffixTrie::searchByPrefix(const string& prefix) const {
    const SuffixTrieNode* cur = root.get();
    for (char c : prefix) {
        auto it = cur->children.find(c);
        if (it == cur->children.end()) return {};
        cur = it->second.get();
    }

    unordered_set<int> seen;
    vector<int> result;
    collectIds(cur, seen, result);
    return result;
}

void SuffixTrie::collectIds(const SuffixTrieNode* node,
                             unordered_set<int>& seen,
                             vector<int>& result) const {
    if (!node) return;
    for (int id : node->movieIds)
        if (seen.insert(id).second)
            result.push_back(id);
    for (const auto& [c, child] : node->children)
        collectIds(child.get(), seen, result);
}
