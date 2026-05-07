#include "../include/CsvReader.h"
#include "../include/Utils.h"

#include <fstream>
#include <iostream>

std::vector<std::string> CsvReader::parseCSVLine(const std::string& line) {

    std::vector<std::string> fields;

    std::string currentField;

    bool insideQuotes = false;

    for (char c : line) {

        if (c == '"') {
            insideQuotes = !insideQuotes;
        }

        else if (c == ',' && !insideQuotes) {

            fields.push_back(currentField);

            currentField.clear();
        }

        else {
            currentField += c;
        }
    }

    fields.push_back(currentField);

    return fields;
}

std::vector<Movie> CsvReader::readMovies(const std::string& filename) {

    std::vector<Movie> movies;

    std::ifstream file(filename);

    if (!file.is_open()) {

        std::cerr << "Error opening file: "
                  << filename << std::endl;

        return movies;
    }

    std::string line;

    // skip header
    std::getline(file, line);

    while (std::getline(file, line)) {

        std::vector<std::string> fields =
            parseCSVLine(line);

        if (fields.size() < 8) {
            continue;
        }

        Movie movie;

        try {
            movie.releaseYear =
                std::stoi(fields[0]);
        }
        catch (...) {
            movie.releaseYear = 0;
        }

        movie.title =
            Utils::cleanText(fields[1]);

        movie.director =
            Utils::cleanText(fields[3]);

        movie.cast =
            Utils::cleanText(fields[4]);

        movie.genre =
            Utils::cleanText(fields[5]);

        movie.plot =
            Utils::cleanText(fields[7]);

        movies.push_back(movie);
    }

    return movies;
}
