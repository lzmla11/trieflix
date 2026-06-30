#include "../include/ConsoleNotificationSystem.h"
#include <string>

void ConsoleNotificationSystem::onMovieLikeChanged(int movieId, int newLikes) {
    // Construimos el mensaje de forma clara
    std::string mensajeLog = "[NOTIFICACION] La pelicula con ID " + std::to_string(movieId) 
                             + " ahora tiene " + std::to_string(newLikes) + " likes.";
    
    // Lo guardamos silenciosamente en el historial
    notificacionesPendientes.push_back(mensajeLog);
}

void ConsoleNotificationSystem::onMovieWatchLaterChanged(int movieId, bool isWatchLater) {
    std::string estado = "";
    if (isWatchLater == true) {
        estado = "AGREGADA a 'Ver mas tarde'.";
    } else {
        estado = "ELIMINADA de 'Ver mas tarde'.";
    }

    std::string mensajeLog = "[NOTIFICACION] La pelicula con ID " + std::to_string(movieId) + " fue " + estado;
    
    // Lo guardamos silenciosamente en el historial
    notificacionesPendientes.push_back(mensajeLog);
}

// Devuelve una copia de las notificaciones acumuladas hasta el momento
std::vector<std::string> ConsoleNotificationSystem::obtenerNotificacionesPendientes() const {
    return notificacionesPendientes;
}

// Vacía el contenedor para no repetir notificaciones viejas
void ConsoleNotificationSystem::limpiarNotificaciones() {
    notificacionesPendientes.clear();
}
