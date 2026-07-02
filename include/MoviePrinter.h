#pragma once

#include <memory>
#include "Movie.h"

// --- INTERFAZ COMPONENTE BASE ---
class MoviePrinter {
public:
    virtual ~MoviePrinter() = default;
    
    // Contrato sagrado: cualquier impresora de películas debe saber dibujarse
    virtual void print(int index, const Movie& movie) const = 0;
};

// --- COMPONENTE CONCRETO ---
// Se encarga de la impresión básica 
class BaseMoviePrinter : public MoviePrinter {
public:
    void print(int index, const Movie& movie) const override;
};

// --- DECORADOR BASE ABSTRACTO ---
// Mantiene una referencia al printer que está envolviendo
class MoviePrinterDecorator : public MoviePrinter {
protected:
    std::unique_ptr<MoviePrinter> wrappedPrinter;

public:
    // El constructor recibe y toma propiedad del printer interno
    explicit MoviePrinterDecorator(std::unique_ptr<MoviePrinter> printer);
    void print(int index, const Movie& movie) const override;
};

// --- DECORADOR CONCRETO 1: LIKES ---
class LikeMovieDecorator : public MoviePrinterDecorator {
public:
    using MoviePrinterDecorator::MoviePrinterDecorator; // Hereda el constructor
    void print(int index, const Movie& movie) const override;
};

// --- DECORADOR CONCRETO 2: WATCH LATER ---
class WatchLaterMovieDecorator : public MoviePrinterDecorator {
public:
    using MoviePrinterDecorator::MoviePrinterDecorator; // Hereda el constructor
    void print(int index, const Movie& movie) const override;
};
