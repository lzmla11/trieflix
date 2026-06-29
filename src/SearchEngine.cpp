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

    // Limpieza de la consulta completa (se usará para el bono de frase exacta)
    string cleanQuery = Utils::cleanText(query);

    // Tokenización de la consulta limpia
    vector<string> words = Utils::tokenize(cleanQuery);
    if (words.empty()) return {};

    unordered_map<int, double> scores;

    // Bucle de puntuación inteligente por cada palabra
    for (const string& word : words) {

        if (Utils::isStopWord(word)) {

            // Es una Stop Word: solo buscamos en títulos con puntaje mínimo
            // para no contaminar los resultados con palabras vacías
            vector<int> titleIds = titleTrie.searchByPrefix(word);
            for (int movieId : titleIds) {
                scores[movieId] += 0.1;
            }

        } else {

            // Es una palabra clave: ponderación completa por campo

            // Coincidencia en Título: Premio Mayor
            vector<int> titleIds = titleTrie.searchByPrefix(word);
            for (int movieId : titleIds) {
                scores[movieId] += 10.0;
            }

            // Coincidencia en Director: Premio Alto
            vector<int> directorIds = directorTrie.searchByPrefix(word);
            for (int movieId : directorIds) {
                scores[movieId] += 5.0;
            }

            // Coincidencia en Reparto: Premio Alto
            vector<int> castIds = castTrie.searchByPrefix(word);
            for (int movieId : castIds) {
                scores[movieId] += 5.0;
            }

            // Coincidencia en Índice General (sinopsis, etc.): Premio Base
            vector<int> generalIds = generalTrie.searchByPrefix(word);
            for (int movieId : generalIds) {
                scores[movieId] += 1.0;
            }
        }
    }

    // Bono de Frase Exacta: si el título limpio contiene la consulta completa
    for (auto& [movieId, currentScore] : scores) {
        const Movie& currentMovie = movies[movieId];
        string cleanTitle = Utils::cleanText(currentMovie.title);

        if (cleanTitle.find(cleanQuery) != string::npos) {
            // La consulta completa aparece literalmente en el título
            currentScore += 100.0;
        }
    }

    return topMovies(scores, limit);
}

vector<const Movie*> SearchEngine::searchByTag(const string& tag,
                                                const string& value,
                                                int limit) const {

    string normVal = Utils::cleanText(value);
    if (normVal.empty()) return {};

    const SuffixTrie* target = nullptr;

    if (tag == "title") {
        target = &titleTrie;
    } else if (tag == "director") {
        target = &directorTrie;
    } else if (tag == "cast" || tag == "casting") {
        target = &castTrie;
    }

    unordered_map<int, double> scores;

    if (target) {
        // === RUTA OPTIMIZADA: BÚSQUEDA POR PALABRAS EN TRIES ===
        vector<string> words = Utils::tokenize(normVal);
        if (words.empty()) return {};

        for (const string& word : words) {
            if (Utils::isStopWord(word)) {
                // Stop Word: puntaje mínimo para no contaminar el ranking
                vector<int> ids = target->searchByPrefix(word);
                for (int movieId : ids) {
                    scores[movieId] += 0.1;
                }
            } else {
                // Palabra clave: puntaje completo de campo
                vector<int> ids = target->searchByPrefix(word);
                for (int movieId : ids) {
                    scores[movieId] += 10.0;
                }
            }
        }

        // Bono de Frase Exacta: requerido porque arriba buscamos palabras sueltas
        for (auto& [movieId, currentScore] : scores) {
            const Movie& currentMovie = movies[movieId];

            string cleanField;
            if (tag == "title") {
                cleanField = Utils::cleanText(currentMovie.title);
            } else if (tag == "director") {
                cleanField = Utils::cleanText(currentMovie.director);
            } else {
                cleanField = Utils::cleanText(currentMovie.cast);
            }

            if (cleanField.find(normVal) != string::npos) {
                currentScore += 100.0;
            }
        }

    } else {
        // === RUTA FALLBACK: ESCANEO LINEAL (genre, year, etc.) ===
        for (const Movie& currentMovie : movies) {

            string cleanField;
            if (tag == "genre" || tag == "genero") {
                cleanField = Utils::cleanText(currentMovie.genre);
            } else if (tag == "year" || tag == "anio") {
                cleanField = to_string(currentMovie.releaseYear);
            } else {
                cleanField = Utils::cleanText(currentMovie.title);
            }

            // Al usar .find() con la variable completa 'normVal', la coincidencia ya es exacta.
            // Asignamos el punto base (1.0) y el bono (100.0) juntos = 101.0 puntos.
            if (cleanField.find(normVal) != string::npos) {
                scores[currentMovie.id] += 101.0;
            }
        }
        // ¡Bucle redundante eliminado con éxito!
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
