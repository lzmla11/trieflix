#pragma once

#include <string>

// --- EL PRODUCTO ---
// Estructura limpia que empaqueta todos los filtros posibles de una búsqueda
class SearchQuery {
public:
    std::string textKeyword = "";
    std::string genreFilter = "";
    std::string directorFilter = "";
    int yearFilter = 0; // 0 significa que no se filtra por año
};

// --- EL BUILDER ---
// Interfaz fluida para configurar los filtros paso a paso
class SearchQueryBuilder {
private:
    SearchQuery query; // El objeto que estamos construyendo internamente

public:
    SearchQueryBuilder() = default;

    // Métodos de configuración que retornan una referencia a sí mismos (*this)
    SearchQueryBuilder& withTextKeyword(const std::string& text);
    SearchQueryBuilder& withGenre(const std::string& genre);
    SearchQueryBuilder& withDirector(const std::string& director);
    SearchQueryBuilder& withYear(int year);

    // El método definitivo que entrega el producto finalizado
    SearchQuery build();
};
