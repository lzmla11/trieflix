#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include "SearchEngine.h"
#include "MovieStateObserver.h"

class SessionManager {
public:
    // Método de acceso global al Singleton 
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

    // Métodos de Interacción con Películas 
    void toggleWatchLater(int movieId);
    void giveLikeToMovie(int movieId);

    // necesario para poder calcular recomendaciones basadas en los
    // gustos del usuario. Guardamos el historial de IDs a los que dio like,
    // sin duplicados, en el orden en que ocurrieron.
    const std::vector<int>& getLikedMovieIds() const;

    // Métodos del patrón Observer para gestionar las subscripciones
    void attachObserver(MovieStateObserver* observer);
    void detachObserver(MovieStateObserver* observer);

private:
    // Constructor privado
    SessionManager() = default;

    // Atributos de la sesión
    std::string currentUsername = "";
    bool isLoggedIn = false;

    // Puntero al motor de búsqueda para poder alterar el vector de películas en memoria
    SearchEngine* searchEngine = nullptr;

    // Historial de películas a las que el usuario actual les dio like
    std::vector<int> likedMovieIds;

    std::unordered_map<std::string, std::vector<int>> userWatchLaterMap;
    std::unordered_map<std::string, std::vector<int>> userLikedMap;

    // Lista de observadores suscritos
    std::vector<MovieStateObserver*> observers;

    // Métodos internos para emitir las alertas a todos los suscritos
    void notifyLikeChanged(int movieId, int newLikes);
    void notifyWatchLaterChanged(int movieId, bool isWatchLater);
};
