#pragma once

#include <string>
#include <vector>
#include "Movie.h"

using namespace std;

class CsvReader {
private:
	vector<string> parseCSVLine(const string& line);

public:
	vector<Movie> readMovies(const string& filename);
};