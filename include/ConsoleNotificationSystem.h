#pragma once

#include <string>
#include <vector>
#include "MovieStateObserver.h"

class ConsoleNotificationSystem : public MovieStateObserver {
public:
    // Métodos heredados del Observer
    void onMovieLikeChanged(int movieId, int newLikes) override;
    void onMovieWatchLaterChanged(int movieId, bool isWatchLater) override;

    // NUEVO: Funciones para que la interfaz controle las notificaciones
    std::vector<std::string> obtenerNotificacionesPendientes() const;
    void limpiarNotificaciones();

private:
    // Contenedor para guardar los mensajes en memoria de forma silenciosa
    std::vector<std::string> notificacionesPendientes;
};
