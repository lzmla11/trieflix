#pragma once

class MovieStateObserver {
public:
    // Destructor virtual para asegurar una correcta limpieza en memoria
    virtual ~MovieStateObserver() = default;

    // Método que se ejecutará cuando una película reciba un Like
    virtual void onMovieLikeChanged(int movieId, int newLikes) = 0;

    // Método que se ejecutará cuando una película cambie su estado de "Ver más tarde"
    virtual void onMovieWatchLaterChanged(int movieId, bool isWatchLater) = 0;
};
