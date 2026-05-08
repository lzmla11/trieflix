#include "../include/CsvReader.h"
#include "../include/Utils.h"
#include <fstream>
#include <iostream>

using namespace std;

vector<string> CsvReader::parseCSVLine(const string& line) {
    vector<string> fields;
    string currentField;
    bool insideQuotes = false;

    for (char c : line) {
        if (c == '"') {
            insideQuotes = !insideQuotes;
        } else if (c == ',' && !insideQuotes) {
            fields.push_back(currentField);
            currentField.clear();
        } else {
            currentField += c;
        }
    }

    fields.push_back(currentField);
    return fields;
}

vector<Movie> CsvReader::readMovies(const string& filename) {
    vector<Movie> movies;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return movies;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        int quoteCount = 0;
        for (char c : line) if (c == '"') quoteCount++;

        while (quoteCount % 2 != 0 && !file.eof()) {
            string nextLine;
            getline(file, nextLine);
            line += ' ' + nextLine;
            for (char c : nextLine) if (c == '"') quoteCount++;
        }

        vector<string> fields = parseCSVLine(line);

        if (fields.size() < 8 || fields[1].empty()) continue;

        Movie movie;

        try {
            movie.releaseYear = stoi(fields[0]);
        } catch (...) {
            movie.releaseYear = 0;
        }

        movie.title    = fields[1];
        movie.origin   = fields[2];
        movie.director = fields[3].empty() ? "Unknown" : fields[3];
        movie.cast     = fields[4].empty() ? ""        : fields[4];
        movie.genre    = fields[5].empty() ? "unknown" : fields[5];
        movie.plot     = fields[7];

        movies.push_back(movie);
    }

    return movies;
}