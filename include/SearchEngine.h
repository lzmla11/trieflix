#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Movie.h"
#include "SuffixTrie.h"

class SearchEngine {
public:
    std::vector<Movie> movies;

    bool load(const std::string& filepath);
    std::vector<const Movie*> searchText(const std::string& query, int limit = 5) const;
    std::vector<const Movie*> searchByTag(const std::string& tag, const std::string& value, int limit = 5) const;

private:
    SuffixTrie trie;

    void indexMovie(const Movie& m);
    std::vector<const Movie*> topMovies(const std::unordered_map<int, double>& scores, int limit) const;
};
