#include "../include/SearchQuery.h"

SearchQueryBuilder& SearchQueryBuilder::withTextKeyword(const std::string& text) {
    query.textKeyword = text;
    return *this; // Retornamos el mismo builder para poder encadenar llamadas
}

SearchQueryBuilder& SearchQueryBuilder::withGenre(const std::string& genre) {
    query.genreFilter = genre;
    return *this;
}

SearchQueryBuilder& SearchQueryBuilder::withDirector(const std::string& director) {
    query.directorFilter = director;
    return *this;
}

SearchQueryBuilder& SearchQueryBuilder::withYear(int year) {
    query.yearFilter = year;
    return *this;
}

SearchQuery SearchQueryBuilder::build() {
    // Entregamos una copia del producto configurado
    return query;
}
