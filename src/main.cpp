#include <iostream>
#include "../include/Utils.h"
#include "../include/CsvReader.h"
#include "../include/Movie.h"

using namespace std; 

int main() {

	CsvReader reader;

    	std::vector<Movie> movies = reader.readMovies("../data/raw/movies.csv");

	std::cout << "Movies loaded: "
		<< movies.size()
		<< std::endl;

    	for (int i = 0; i < 5 && i < movies.size(); i++) {

        std::cout << "\nTITLE: "
                  << movies[i].title
                  << std::endl;

        std::cout << "\nDIRECTOR: "
                  << movies[i].director
                  << std::endl;

        std::cout << "PLOT: "
                << movies[i].plot
                << std::endl;
    	}

    	return 0;
}
