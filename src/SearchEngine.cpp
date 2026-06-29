#include "../include/SearchEngine.h"
#include "../include/CsvReader.h"
#include "../include/Utils.h"
#include <algorithm>
#include <iostream>

using namespace std; 

bool SearchEngine::load(const std::string& filepath) {
    CsvReader reader;
    movies = reader.readMovies(filepath);

    if (movies.empty()) {
        std::cerr << "No se cargaron peliculas.\n";
        return false;
    }

    for (const auto& movie : movies) {
        indexMovie(movie);
    }

    std::cout << movies.size() << " peliculas cargadas e indexadas exitosamente.\n";
    return true;
}

void SearchEngine::indexMovie(const Movie& m) {
    auto indexInTrie = [&](const std::string& fieldText, SuffixTrie& specificTrie) {
        std::string cleaned = Utils::cleanText(fieldText);
        std::vector<std::string> tokens = Utils::tokenize(cleaned);
        
        for (const std::string& word : tokens) {
            specificTrie.insert(word, m.id);
            generalTrie.insert(word, m.id); 
        }
    };

    auto indexInGeneral = [&](const std::string& fieldText) {
        std::string cleaned = Utils::cleanText(fieldText);
        std::vector<std::string> tokens = Utils::tokenize(cleaned);
        
        for (const std::string& word : tokens) {
            generalTrie.insert(word, m.id);
        }
    };

    indexInTrie(m.title, titleTrie);
    indexInTrie(m.director, directorTrie);
    indexInTrie(m.cast, castTrie);
    
    indexInGeneral(m.plot);
}

vector<const Movie*> SearchEngine::searchText(const string& query, int limit) const {
    vector<string> words = Utils::tokenize(Utils::cleanText(query));
    if (words.empty()) return {};

    unordered_map<int, double> scores;

    for (const string& word : words) {
        vector<int> ids = generalTrie.searchByPrefix(word);
        for (int id : ids)
            scores[id] += 1.0;
    }

    return topMovies(scores, limit);
}

vector<const Movie*> SearchEngine::searchByTag(const string& tag,
                                                const string& value,
                                                int limit) const {
    string normVal = Utils::cleanText(value);
    if (normVal.empty()) return {};

    const SuffixTrie* target = nullptr;

    if      (tag == "title"                  ) target = &titleTrie;
    else if (tag == "director"               ) target = &directorTrie;
    else if (tag == "cast" || tag == "casting") target = &castTrie;

    unordered_map<int, double> scores;

   if (target) {
        // En lugar de buscar la frase junta, la tokenizamos por si pusieron nombres compuestos (ej: "Christopher Nolan")
        vector<string> words = Utils::tokenize(normVal);
        if (words.empty()) return {};

        for (const string& word : words) {
            vector<int> ids = target->searchByPrefix(word);
            for (int id : ids)
                scores[id] += 1.0; // Acumula frecuencias por cada palabra encontrada en el TAG
        }
    } else {
        // Tag no reconocido (year, genre, etc.) → fallback lineal
        for (const Movie& m : movies) {
            string field;
            if      (tag == "genre" || tag == "genero") field = Utils::cleanText(m.genre);
            else if (tag == "year"  || tag == "anio"  ) field = to_string(m.releaseYear);
            else                                        field = Utils::cleanText(m.title);

            if (field.find(normVal) != string::npos)
                scores[m.id] += 1.0;
        }
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
