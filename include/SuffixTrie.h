#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_set>

using namespace std;

struct SuffixTrieNode {
    map<char, unique_ptr<SuffixTrieNode>> children;
    vector<int> movieIds;
};

class SuffixTrie {
public:
    SuffixTrie();

    void insert(const string& word, int movieId);
    vector<int> searchByPrefix(const string& prefix) const;

private:
    unique_ptr<SuffixTrieNode> root;

    vector<string> generateSuffixes(const string& word) const;
    void collectIds(const SuffixTrieNode* node,
                    unordered_set<int>& seen,
                    vector<int>& result) const;
};
