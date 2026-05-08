#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_set>

using namespace std;

struct TrieNode {
    map<char, unique_ptr<TrieNode>> children;
    bool isEndOfWord = false;
    vector<int> movieIds;
};

class Trie {
public:
    Trie();

    void insert(const string& word, int movieId);
    vector<int> searchByPrefix(const string& prefix) const;
    vector<int> searchBySubstring(const string& sub) const;
    static vector<string> tokenize(const string& text);

private:
    unique_ptr<TrieNode> root;

    void collectIds(const TrieNode* node,
                    unordered_set<int>& seen,
                    vector<int>& result) const;

    void substringDFS(const TrieNode* node,
                      string& current,
                      const string& sub,
                      unordered_set<int>& seen,
                      vector<int>& result) const;
};