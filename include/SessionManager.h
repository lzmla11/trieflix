#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include "SearchEngine.h"
#include "MovieStateObserver.h"

class SessionManager {
public:
    // Método de acceso global al Singleton (Meyers' Singleton)
    static SessionManager& getInstance();

    // Eliminamos la posibilidad de copiar la instancia para proteger el patrón
    SessionManager(const SessionManager& other) = delete;
    SessionManager& operator=(const SessionManager& other) = delete;

    // Métodos de Gestión de Usuario
    void loginUser(const std::string& username);
    void logoutUser();
    std::string getCurrentUser() const;
    bool isUserLoggedIn() const;

    // Vinculación con el motor de búsqueda para modificar películas en caliente
    void setSearchEngine(SearchEngine* enginePointer);

    // Métodos de Interacción con Películas (Explicicitos y claros)
    void toggleWatchLater(int movieId);
    void giveLikeToMovie(int movieId);

    // Métodos del patrón Observer para gestionar las subscripciones
    void attachObserver(MovieStateObserver* observer);
    void detachObserver(MovieStateObserver* observer);

private:
    // Constructor privado: nadie fuera de esta clase puede hacer un 'new SessionManager()'
    SessionManager() = default;

    // Atributos de la sesión
    std::string currentUsername = "";
    bool isLoggedIn = false;

    // Puntero al motor de búsqueda para poder alterar el vector de películas en memoria
    SearchEngine* searchEngine = nullptr;

    // Lista de observadores suscritos
    std::vector<MovieStateObserver*> observers;

    // Métodos internos para emitir las alertas a todos los suscritos
    void notifyLikeChanged(int movieId, int newLikes);
    void notifyWatchLaterChanged(int movieId, bool isWatchLater);
};
