#include "../include/SessionManager.h"
#include <algorithm>
#include <iostream>

// Implementación del acceso global
SessionManager& SessionManager::getInstance() {
    // Garantiza una única instancia persistente en toda la vida del programa
    static SessionManager instance;
    return instance;
}

void SessionManager::loginUser(const std::string& username) {
    if (username.empty() == false) {
        currentUsername = username;
        isLoggedIn = true;
    }
}

void SessionManager::logoutUser() {
    currentUsername = "";
    isLoggedIn = false;
    // Al cerrar sesión, el historial de likes de este usuario ya no aplica
    likedMovieIds.clear();
}

std::string SessionManager::getCurrentUser() const {
    return currentUsername;
}

bool SessionManager::isUserLoggedIn() const {
    return isLoggedIn;
}

void SessionManager::setSearchEngine(SearchEngine* enginePointer) {
    searchEngine = enginePointer;
}

void SessionManager::toggleWatchLater(int movieId) {
    if (searchEngine == nullptr) {
        std::cerr << "Error: El SessionManager no tiene vinculado el SearchEngine.\n";
        return;
    }

    // Buscamos la película dentro del vector público del SearchEngine
    for (Movie& currentMovie : searchEngine->movies) {
        if (currentMovie.id == movieId) {
            // Togleamos el estado booleano de forma tradicional e informativa
            if (currentMovie.watchLater == true) {
                currentMovie.watchLater = false;
                std::cout << "  Pelicula quitada de 'Ver mas tarde'.\n";
            } else {
                currentMovie.watchLater = true;
                std::cout << "  Pelicula agregada a 'Ver mas tarde'.\n";
            }
            // dispara notificación
            this->notifyWatchLaterChanged(movieId, currentMovie.watchLater);
            return; // Ya encontramos la película, terminamos el bucle
        }
    }
}

void SessionManager::giveLikeToMovie(int movieId) {
    if (searchEngine == nullptr) {
        std::cerr << "Error: El SessionManager no tiene vinculado el SearchEngine.\n";
        return;
    }

    // Buscamos la película para sumarle el Like
    for (Movie& currentMovie : searchEngine->movies) {
        if (currentMovie.id == movieId) {
            currentMovie.likes = currentMovie.likes + 1;
            std::cout << "  ¡Le diste un Like a la pelicula! Total de likes actuales: " << currentMovie.likes << "\n";

            // Guardamos este ID en el historial de gustos del usuario,
            // solo si no estaba registrado todavia, para poder usarlo
            // despues en el algoritmo de recomendaciones.
            bool yaRegistrado = std::find(likedMovieIds.begin(), likedMovieIds.end(), movieId) != likedMovieIds.end();
            if (yaRegistrado == false) {
                likedMovieIds.push_back(movieId);
            }

            // dispara la notificación
            this->notifyLikeChanged(movieId, currentMovie.likes);
            return;
        }
    }
}

const std::vector<int>& SessionManager::getLikedMovieIds() const {
    return likedMovieIds;
}

void SessionManager::attachObserver(MovieStateObserver* observer) {
    if (observer != nullptr) {
        observers.push_back(observer);
    }
}

void SessionManager::detachObserver(MovieStateObserver* observer) {
    // Buscamos al observador usando un bucle tradicional y claro
    for (size_t i = 0; i < observers.size(); i = i + 1) {
        if (observers[i] == observer) {
            // Lo eliminamos de la lista si lo encuentra
            observers.erase(observers.begin() + i);
            return;
        }
    }
}

void SessionManager::notifyLikeChanged(int movieId, int newLikes) {
    // Alertamos a cada uno de los observadores registrados
    for (MovieStateObserver* observer : observers) {
        if (observer != nullptr) {
            observer->onMovieLikeChanged(movieId, newLikes);
        }
    }
}

void SessionManager::notifyWatchLaterChanged(int movieId, bool isWatchLater) {
    for (MovieStateObserver* observer : observers) {
        if (observer != nullptr) {
            observer->onMovieWatchLaterChanged(movieId, isWatchLater);
        }
    }
}
