#pragma once

#include <string>
#include <vector>
#include "Movie.h"

class CsvReader {
private: 
	std::vector<std::string> parseCSVLine(const std::string& line); 

public:
	std::vector<Movie> readMovies(const std::string& filename); 
};
