#include "../include/ConsoleNotificationSystem.h"
#include <iostream>

void ConsoleNotificationSystem::onMovieLikeChanged(int movieId, int newLikes) {
    std::cout << "\\n  [SISTEMA DE NOTIFICACIONES] Alguien le dio Like a la pelicula ID: " 
              << movieId << ". Ahora tiene " << newLikes << " likes en total.\\n";
}

void ConsoleNotificationSystem::onMovieWatchLaterChanged(int movieId, bool isWatchLater) {
    std::cout << "\\n  [SISTEMA DE NOTIFICACIONES] El estado de 'Ver mas tarde' de la pelicula ID: " 
              << movieId << " cambio a: ";
              
    if (isWatchLater == true) {
        std::cout << "AGREGADA.\\n";
    } else {
        std::cout << "ELIMINADA.\\n";
    }
}
