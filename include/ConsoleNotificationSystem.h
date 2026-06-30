#pragma once
#include "MovieStateObserver.h"

// Esta clase hereda de la interfaz, prometiendo implementar sus métodos
class ConsoleNotificationSystem : public MovieStateObserver {
public:
    void onMovieLikeChanged(int movieId, int newLikes) override;
    void onMovieWatchLaterChanged(int movieId, bool isWatchLater) override;
};
