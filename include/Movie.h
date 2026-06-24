#pragma once

#include <string>

struct Movie {
public:
    int id          = 0;
    int releaseYear = 0;
    std::string title;
    std::string origin;
    std::string director;
    std::string cast;
    std::string genre;
    std::string wikiPage; 
    std::string plot;
    int  likes      = 0;
    bool watchLater = false;
};
