#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_set>

struct SuffixTrieNode {
    std::map<char, std::unique_ptr<SuffixTrieNode>> children;
    std::vector<int> movieIds;
};

class SuffixTrie {
public:
    SuffixTrie();

    void insert(const std::string& word, int movieId);
    std::vector<int> searchByPrefix(const std::string& prefix) const;

private:
    std::unique_ptr<SuffixTrieNode> root;

    std::vector<std::string> generateSuffixes(const std::string& word) const;
    void collectIds(const SuffixTrieNode* node,
                    std::unordered_set<int>& seen,
                    std::vector<int>& result) const;
};
