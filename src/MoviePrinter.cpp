#include "../include/MoviePrinter.h"
#include <iostream>

// Función utilitaria local para recortar texto sin alterar otras clases
std::string truncateText(const std::string& text, size_t maxLength) {
    if (text.size() <= maxLength) {
        return text;
    }
    return text.substr(0, maxLength - 3) + "...";
}

// --- IMPRESIÓN BASE CONCRETA ---
void BaseMoviePrinter::print(int index, const Movie& movie) const {
    // Dibujamos exactamente la estructura limpia que ya tenías en tu main.cpp
    std::cout << " [" << index << "] " << truncateText(movie.title, 44)
              << " (" << movie.releaseYear << ")\\n";
              
    std::cout << "      Dir: " << truncateText(movie.director, 28)
              << "  |  " << truncateText(movie.genre, 18) << "\\n";
}

// --- DECORADOR BASE ---
MoviePrinterDecorator::MoviePrinterDecorator(std::unique_ptr<MoviePrinter> printer) {
    wrappedPrinter = std::move(printer);
}

void MoviePrinterDecorator::print(int index, const Movie& movie) const {
    if (wrappedPrinter != nullptr) {
        wrappedPrinter->print(index, movie);
    }
}

// --- DECORADOR CONCRETO: LIKES ---
void LikeMovieDecorator::print(int index, const Movie& movie) const {
    // 1. Primero dejamos que se imprima todo lo que viene adentro del envoltorio
    MoviePrinterDecorator::print(index, movie);
    
    // 2. Añadimos nuestra decoración si cumple con la condición
    if (movie.likes > 0) {
        std::cout << "      [❤️] Likes de la comunidad: " << movie.likes << "\\n";
    }
}

// --- DECORADOR CONCRETO: WATCH LATER ---
void WatchLaterMovieDecorator::print(int index, const Movie& movie) const {
    // 1. Primero dejamos que se imprima todo lo que viene adentro del envoltorio
    MoviePrinterDecorator::print(index, movie);
    
    // 2. Añadimos nuestra decoración si cumple con la condición
    if (movie.watchLater == true) {
        std::cout << "      [⏳] Esta pelicula esta en tu lista de 'Ver mas tarde'\\n";
    }
}
