#include "../include/SearchEngine.h"
#include "../include/CsvReader.h"
#include "../include/Utils.h"
#include <algorithm>
#include <iostream>

using namespace std;

bool SearchEngine::load(const string& filepath) {
    CsvReader reader;
    movies = reader.readMovies(filepath);

    if (movies.empty()) {
        cerr << "No se cargaron peliculas.\n";
        return false;
    }

    for (int i = 0; i < (int)movies.size(); i++) {
        movies[i].id = i;
        indexMovie(movies[i]);
    }

    cout << movies.size() << " peliculas cargadas.\n";
    return true;
}

void SearchEngine::indexMovie(const Movie& m) {
    auto insertField = [&](const string& field) {
        for (const string& word : Trie::tokenize(field))
            trie.insert(word, m.id);
    };

    insertField(m.title);
    insertField(m.director);
    insertField(m.cast);
    insertField(m.genre);
    insertField(m.plot);
}

vector<const Movie*> SearchEngine::searchText(const string& query, int limit) const {
    vector<string> words = Trie::tokenize(query);
    if (words.empty()) return {};

    unordered_map<int, double> scores;

    for (const string& word : words) {
        vector<int> ids = trie.searchBySubstring(word);
        for (int id : ids)
            scores[id] += 1.0;
    }

    if (scores.empty()) {
        string flat;
        for (unsigned char c : query)
            if (isalnum(c)) flat += tolower(c);
        for (int id : trie.searchByPrefix(flat))
            scores[id] += 1.0;
    }

    return topMovies(scores, limit);
}

vector<const Movie*> SearchEngine::searchByTag(const string& tag,
                                                const string& value,
                                                int limit) const {
    string normVal = Utils::cleanText(value);
    unordered_map<int, double> scores;

    for (const Movie& m : movies) {
        string field;

        if      (tag == "director")                 field = Utils::cleanText(m.director);
        else if (tag == "cast" || tag == "casting") field = Utils::cleanText(m.cast);
        else if (tag == "genre" || tag == "genero") field = Utils::cleanText(m.genre);
        else if (tag == "year"  || tag == "anio")   field = to_string(m.releaseYear);
        else                                        field = Utils::cleanText(m.title);

        if (field.find(normVal) != string::npos)
            scores[m.id] += 1.0;
    }

    return topMovies(scores, limit);
}

vector<const Movie*> SearchEngine::topMovies(
    const unordered_map<int, double>& scores, int limit) const {

    vector<pair<double, int>> ranked;
    ranked.reserve(scores.size());

    for (const auto& [id, sc] : scores)
        ranked.push_back({sc, id});

    sort(ranked.begin(), ranked.end(),
         [](const pair<double,int>& a, const pair<double,int>& b) {
             return a.first > b.first;
         });

    vector<const Movie*> result;
    int n = min(limit, (int)ranked.size());
    for (int i = 0; i < n; i++)
        result.push_back(&movies[ranked[i].second]);

    return result;
}