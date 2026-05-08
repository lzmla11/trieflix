#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Movie.h"
#include "Trie.h"

using namespace std;

class SearchEngine {
public:
    vector<Movie> movies;

    bool load(const string& filepath);
    vector<const Movie*> searchText(const string& query, int limit = 5) const;
    vector<const Movie*> searchByTag(const string& tag, const string& value, int limit = 5) const;

private:
    Trie trie;

    void indexMovie(const Movie& m);
    vector<const Movie*> topMovies(const unordered_map<int, double>& scores, int limit) const;
};