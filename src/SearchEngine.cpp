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

std::vector<const Movie*> SearchEngine::topMovies(
    const std::unordered_map<int, double>& scores, int limit) const {

    // Paso 1: Volcamos el mapa a un vector de pares (Puntaje, ID)
    std::vector<std::pair<double, int>> ranked;
    ranked.reserve(scores.size());

    for (const auto& [id, sc] : scores) {
        ranked.push_back({sc, id});
    }

    // Paso 2: Ordenamos de mayor a menor puntuación y desempatamos por LIKES
    // NOTA: Usamos [this] al inicio para que la lambda tenga acceso al vector 'movies'
    std::sort(ranked.begin(), ranked.end(),
         [this](const std::pair<double, int>& a, const std::pair<double, int>& b) {
             // Criterio principal: Mayor puntaje de palabras clave primero
             if (a.first != b.first) {
                 return a.first > b.first;
             }
             
             // Criterio de desempate: Si tienen los mismos puntos, 
             // la película con más likes va primero.
             return movies[a.second].likes > movies[b.second].likes;
         });

    // Paso 3: Calculamos el corte exacto
    int numResults = std::min(limit, static_cast<int>(ranked.size()));

    // Paso 4: Construimos el resultado con punteros reales
    std::vector<const Movie*> result;
    result.reserve(numResults);

    for (int i = 0; i < numResults; i++) {
        int winnerId = ranked[i].second;
        result.push_back(&movies[winnerId]);
    }

    return result;
}
