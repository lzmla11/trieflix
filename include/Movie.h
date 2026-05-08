#pragma once

#include <string>

using namespace std;

class Movie {
public:
    int id          = 0;
    int releaseYear = 0;
    string title;
    string origin;
    string director;
    string cast;
    string genre;
    string plot;
    int  likes      = 0;
    bool watchLater = false;
};