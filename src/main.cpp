#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#include "../include/Movie.h"
#include "../include/SearchEngine.h"
#include "../include/SessionManager.h"
#include "../include/ConsoleNotificationSystem.h"
#include "../include/MoviePrinter.h"
#include "../include/SearchQuery.h"

using namespace std;

// --- FUNCIONES UTILITARIAS DE CONSOLA ---
string getInput(const string& prompt = "") {
    if (!prompt.empty()) cout << prompt;
    string line;
    getline(cin, line);
    return line;
}

void printSeparator() {
    for (int i = 0; i < 75; i++) cout << "-";
    cout << "\n";
}

void printHeader() {
    cout << "\n";
    printSeparator();
    cout << "         TrieFlix - Motor de Busqueda \n";
    printSeparator();
}

void showMovieDetails(const Movie* movie) {
    if (movie == nullptr) {
        cout << "\n  [!] Error: No se pudo cargar la informacion de la pelicula.\n";
        getInput("  Presiona Enter para continuar...");
        return;
    }

    printHeader();
    cout << "  [DETALLE DE LA PELICULA]\n";
    printSeparator();
    
    // Información principal
    cout << "  Titulo:           " << movie->title << " (" << movie->releaseYear << ")\n";
    cout << "  Director:         " << movie->director << "\n";
    cout << "  Genero:           " << movie->genre << "\n";
    cout << "  Pais de Origen:   " << movie->origin << "\n";
    cout << "  Likes Totales:    " << movie->likes << "\n";
    
    // Estado de la lista de seguimiento
    cout << "  ¿Ver mas tarde?:  ";
    if (movie->watchLater == true) {
        cout << "Si (Guardada)\n";
    } else {
        cout << "No\n";
    }
    
    printSeparator();
    cout << "  ELENCO / CAST:\n";
    cout << "  " << movie->cast << "\n";
    
    printSeparator();
    cout << "  SINOPSIS / RESUMEN:\n";
    cout << "  " << movie->plot << "\n";
    
    printSeparator();
    cout << "  Enlace de Wikipedia: " << movie->wikiPage << "\n";
    printSeparator();
    
    // Pausa para que el usuario pueda leer con calma
    getInput("  Presiona Enter para regresar a la lista...");
}

// Coloca esta función en tu main.cpp para pintar la "barra de notificaciones"
void mostrarNotificacionesSistema(ConsoleNotificationSystem& sistemaNotificaciones) {
    std::vector<std::string> listaAlertas = sistemaNotificaciones.obtenerNotificacionesPendientes();
    
    // Si no hay alertas nuevas, no pintamos nada y salimos de la función
    if (listaAlertas.empty() == true) {
        return;
    }

    // Si hay alertas, las mostramos de forma muy elegante y limpia
    cout << "  🔔 NOTIFICACIONES RECIENTES:\n";
    for (const std::string& mensajeActual : listaAlertas) {
        cout << "  -> " << mensajeActual << "\n";
    }
    printSeparator();

    // Una vez mostradas, las limpiamos para que no vuelvan a aparecer en la siguiente pantalla
    sistemaNotificaciones.limpiarNotificaciones();
}

// --- FLUJO DE VISTA DE LISTA (Integra Decorator y Observer) ---
void listFlow(SearchEngine& engine, const vector<const Movie*>& list, const string& title) {
    if (list.empty()) {
        cout << "\n  [!] No se encontraron resultados.\n";
        return;
    }

    int page = 0;
    int pageSize = 5;
    int totalPages = (list.size() + pageSize - 1) / pageSize;
    while (true) {
        printHeader();
        cout << "  " << title << "\n";
        
        int start = page * pageSize;
        int end = min(start + pageSize, (int)list.size());

        cout << "\n  Mostrando resultados " << start + 1 << " - " << end 
             << " de " << list.size() << " (Pagina " << page + 1 << " de " << totalPages << ")\n";
        printSeparator();

        for (int i = start; i < end; i++) {
            const Movie& currentMovie = *list[i];
            
            // --- PATRÓN DECORATOR EN ACCIÓN ---
            // Creamos la impresora base
            std::unique_ptr<MoviePrinter> dynamicPrinter = std::make_unique<BaseMoviePrinter>();
            
            // La envolvemos si tiene Likes
            if (currentMovie.likes > 0) {
                dynamicPrinter = std::make_unique<LikeMovieDecorator>(std::move(dynamicPrinter));
            }
            // La envolvemos si está en Watch Later
            if (currentMovie.watchLater) {
                dynamicPrinter = std::make_unique<WatchLaterMovieDecorator>(std::move(dynamicPrinter));
            }
            
            // Imprimimos pasándole el ID real de la película (para que el usuario sepa qué ID usar)
            dynamicPrinter->print(currentMovie.id, currentMovie);
            printSeparator();
        }

        cout << "  [N] Sig. Pag | [P] Ant. Pag | [L <id>] Like | [W <id>] Ver mas tarde | [0] Volver\n";
        string opt = getInput("  Accion (Ej: 'L 42'): ");

        if (opt == "0") break;
        if ((opt == "N" || opt == "n") && page < totalPages - 1) page++;
        else if ((opt == "P" || opt == "p") && page > 0) page--;
        else if (opt.size() >= 2 && (toupper(opt[0]) == 'L' || toupper(opt[0]) == 'W')) {
            // === INTERACCIÓN DEL USUARIO CON SINGLETON Y OBSERVER ===
            int targetId = 0;
            try {
                targetId = stoi(opt.substr(2)); // Extraemos el ID después de la letra y el espacio
                if (toupper(opt[0]) == 'L') {
                    SessionManager::getInstance().giveLikeToMovie(targetId);
                } else {
                    SessionManager::getInstance().toggleWatchLater(targetId);
                }
                getInput("\n  Presiona Enter para continuar...");
            } catch (...) {
                cout << "  [!] Formato invalido. Usa la letra, un espacio y el ID. Ej: L 42\n";
                getInput("\n  Presiona Enter para continuar...");
            }
        }
    }
}

void searchTagFlow(SearchEngine& engine) {
    while (true) {
        printHeader();
        cout << "  [BUSQUEDA POR ATRIBUTO]\n";
        printSeparator();
        cout << "  Selecciona el atributo por el que deseas filtrar:\n\n";
        cout << "  [1] Titulo\n";
        cout << "  [2] Director\n";
        cout << "  [3] Elenco / Cast\n";
        cout << "  [0] Volver al Menu Principal\n";
        printSeparator();

        string opt = getInput("  Opcion: ");
        string tag = "";

        // 1. Mapeo seguro y encapsulado en la Vista
        if (opt == "0") {
            return; // Regresa al menú principal
        } else if (opt == "1") {
            tag = "title";
        } else if (opt == "2") {
            tag = "director";
        } else if (opt == "3") {
            tag = "cast";
        } else {
            cout << "\n  [!] Opcion invalida. Por favor, selecciona un numero del 0 al 3.\n";
            getInput("  Presiona Enter para continuar...");
            continue; // Vuelve a pintar este submenú
        }

        // 2. Solicitar el valor de búsqueda
        cout << "\n";
        string value = getInput("  Ingresa el valor a buscar: ");
        
        if (value.empty()) {
            cout << "  [!] El valor de busqueda no puede estar vacio.\n";
            getInput("  Presiona Enter para continuar...");
            continue;
        }

        // 3. El motor resuelve la búsqueda sin que la vista sepa CÓMO lo hace
        vector<const Movie*> results = engine.searchByTag(tag, value, 1000);
        
        // 4. Se envían los resultados a la vista paginada
        string label = (opt == "1") ? "Titulo: " : (opt == "2") ? "Director: " : "Elenco: ";
        listFlow(engine, results, label + value);
        break; // Al salir de la lista, regresa al menú principal
    }
}

// --- FLUJO DE BÚSQUEDA AVANZADA (Integra el Builder) ---
void advancedSearchFlow(SearchEngine& engine) {
    printHeader();
    cout << "  [MODO BUSQUEDA AVANZADA]\n";
    printSeparator();
    cout << "Enter para omitir\n"; 
    string keyword  = getInput("  Palabra clave en titulo/plot: ");
    string genre    = getInput("  Genero: ");
    string director = getInput("  Director: ");
    string yearStr  = getInput("  Ano: ");
    
    int year = 0;
    try { if (!yearStr.empty()) year = stoi(yearStr); } catch(...) {}

    // --- PATRÓN BUILDER EN ACCIÓN ---
    SearchQueryBuilder queryBuilder;
    
    if (!keyword.empty())  queryBuilder.withTextKeyword(keyword);
    if (!genre.empty())    queryBuilder.withGenre(genre);
    if (!director.empty()) queryBuilder.withDirector(director);
    if (year != 0)         queryBuilder.withYear(year);

    SearchQuery complexQuery = queryBuilder.build();
    // ---------------------------------

    // Llamamos al motor paralelo scatter-gather
    vector<const Movie*> results = engine.searchAdvanced(complexQuery, 1000);
    listFlow(engine, results, "Resultados de Filtro Avanzado");
}

int main(int argc, char* argv[]) {
    string csvPath = "../data/raw/movies.csv"; // Ajusta la ruta a donde tengas el archivo al compilar
    if (argc > 1) csvPath = argv[1];

    printHeader();
    cout << "  Cargando base de datos...\n";

    SearchEngine engine;
    if (!engine.load(csvPath)) return 1;

    // === INICIALIZACIÓN DE PATRONES ===
    // 1. Configuramos el Singleton
    SessionManager::getInstance().setSearchEngine(&engine);
    SessionManager::getInstance().loginUser("HermanoBuscador");

    // 2. Configuramos el Observer
    static ConsoleNotificationSystem notifier;
    SessionManager::getInstance().attachObserver(&notifier);
    // ===================================

while (true) {
        printHeader();

        // CORREGIDO: Ahora usa la variable correcta 'notifier'
        mostrarNotificacionesSistema(notifier);

        cout << "  Hola, " << SessionManager::getInstance().getCurrentUser() << " | Que deseas hacer?\n";
        printSeparator();
        cout << "  [1] Busqueda Rapida (Texto)\n";
        cout << "  [2] Busqueda por Tag Especifico\n";
        cout << "  [3] Busqueda Avanzada (Patron Builder + Paralelismo)\n";
        cout << "  [0] Salir\n";
        printSeparator();

        string opt = getInput("  Opcion: ");

        if (opt == "0") {
            cout << "  ¡Hasta luego!\n";
            break;
        } else if (opt == "1") {
            string query = getInput("  Buscar: ");
            if (!query.empty()) {
                vector<const Movie*> results = engine.searchText(query, 1000);
                listFlow(engine, results, "Resultados para: " + query);
            }
        } else if (opt == "2") {
            // CORREGIDO: Se eliminó la entrada manual redundante y se llama al flujo modular
            searchTagFlow(engine); 
        } else if (opt == "3") {
            advancedSearchFlow(engine);
        } else {
            cout << "  [!] Opcion no valida.\n";
        }
    }

    return 0;
}
