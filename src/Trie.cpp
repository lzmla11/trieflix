#include "../include/Trie.h"
#include <cctype>
#include <sstream>

using namespace std;

Trie::Trie() : root(make_unique<TrieNode>()) {}

vector<string> Trie::tokenize(const string& text) {
    string normalized;
    for (unsigned char c : text) {
        if (isalnum(c)) normalized += tolower(c);
        else             normalized += ' ';
    }

    vector<string> tokens;
    istringstream ss(normalized);
    string word;
    while (ss >> word) {
        if (word.size() >= 2)
            tokens.push_back(word);
    }
    return tokens;
}

void Trie::insert(const string& word, int movieId) {
    TrieNode* cur = root.get();

    for (char c : word) {
        if (!cur->children.count(c))
            cur->children[c] = make_unique<TrieNode>();
        cur = cur->children[c].get();
    }

    cur->isEndOfWord = true;

    vector<int>& ids = cur->movieIds;
    if (ids.empty() || ids.back() != movieId)
        ids.push_back(movieId);
}

vector<int> Trie::searchByPrefix(const string& prefix) const {
    string norm;
    for (unsigned char c : prefix)
        if (isalnum(c)) norm += tolower(c);

    const TrieNode* cur = root.get();
    for (char c : norm) {
        auto it = cur->children.find(c);
        if (it == cur->children.end()) return {};
        cur = it->second.get();
    }

    unordered_set<int> seen;
    vector<int> result;
    collectIds(cur, seen, result);
    return result;
}

vector<int> Trie::searchBySubstring(const string& sub) const {
    string norm;
    for (unsigned char c : sub)
        if (isalnum(c)) norm += tolower(c);

    if (norm.empty()) return {};

    unordered_set<int> seen;
    vector<int> result;
    string current;
    substringDFS(root.get(), current, norm, seen, result);
    return result;
}

void Trie::collectIds(const TrieNode* node,
                      unordered_set<int>& seen,
                      vector<int>& result) const {
    if (!node) return;

    for (int id : node->movieIds)
        if (seen.insert(id).second)
            result.push_back(id);

    for (const auto& [c, child] : node->children)
        collectIds(child.get(), seen, result);
}

void Trie::substringDFS(const TrieNode* node,
                         string& current,
                         const string& sub,
                         unordered_set<int>& seen,
                         vector<int>& result) const {
    if (!node) return;

    if (node->isEndOfWord && current.find(sub) != string::npos) {
        for (int id : node->movieIds)
            if (seen.insert(id).second)
                result.push_back(id);
    }

    for (const auto& [c, child] : node->children) {
        current += c;
        substringDFS(child.get(), current, sub, seen, result);
        current.pop_back();
    }
}