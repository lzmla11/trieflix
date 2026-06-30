#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "../include/Movie.h"
#include "../include/SearchEngine.h"
#include "../include/SessionManager.h"
#include "../include/ConsoleNotificationSystem.h"
#include "../include/MoviePrinter.h"
#include "../include/SearchQuery.h"

using namespace std;

// =====================================================================
//  FUNCIONES UTILITARIAS DE CONSOLA
// =====================================================================

string getInput(const string& prompt = "") {
    if (!prompt.empty()) {
        cout << prompt;
    }
    string line;
    getline(cin, line);
    return line;
}

void printSeparator() {
    for (int i = 0; i < 75; i++) {
        cout << "-";
    }
    cout << "\n";
}

void printHeader() {
    cout << "\n";
    printSeparator();
    cout << "         TrieFlix - Motor de Busqueda \n";
    printSeparator();
}

// Coloca la "barra de notificaciones" en pantalla y luego la limpia
void mostrarNotificacionesSistema(ConsoleNotificationSystem& sistemaNotificaciones) {
    vector<string> listaAlertas = sistemaNotificaciones.obtenerNotificacionesPendientes();

    if (listaAlertas.empty()) {
        return;
    }

    cout << "  NOTIFICACIONES RECIENTES:\n";
    for (const string& mensajeActual : listaAlertas) {
        cout << "  -> " << mensajeActual << "\n";
    }
    printSeparator();

    sistemaNotificaciones.limpiarNotificaciones();
}

// =====================================================================
//  DETALLE DE PELICULA (Conectado de verdad al flujo de la lista)
// =====================================================================

// Muestra la ficha completa de una película y, desde ahí mismo, deja que
// el usuario decida si quiere darle Like o agregarla/quitarla de
// 'Ver mas tarde'. Antes esta función existía pero nunca se llamaba desde
// ningún lado; ahora es el único lugar donde se hacen esas acciones.
void showMovieDetails(SearchEngine& engine, int movieId) {
    // Buscamos la película en el vector real del engine (no en un puntero
    // que pudo haber quedado desactualizado por otra búsqueda previa)
    Movie* movieFound = nullptr;
    for (Movie& currentMovie : engine.movies) {
        if (currentMovie.id == movieId) {
            movieFound = &currentMovie;
            break;
        }
    }

    if (movieFound == nullptr) {
        cout << "\n  [!] No se encontro una pelicula con ese ID.\n";
        getInput("  Presiona Enter para continuar...");
        return;
    }

    while (true) {
        printHeader();
        cout << "  [DETALLE DE LA PELICULA]\n";
        printSeparator();

        cout << "  Titulo:           " << movieFound->title << " (" << movieFound->releaseYear << ")\n";
        cout << "  Director:         " << movieFound->director << "\n";
        cout << "  Genero:           " << movieFound->genre << "\n";
        cout << "  Pais de Origen:   " << movieFound->origin << "\n";
        cout << "  Likes Totales:    " << movieFound->likes << "\n";

        cout << "  Ver mas tarde?:   ";
        if (movieFound->watchLater) {
            cout << "Si (Guardada)\n";
        } else {
            cout << "No\n";
        }

        printSeparator();
        cout << "  ELENCO / CAST:\n";
        cout << "  " << movieFound->cast << "\n";

        printSeparator();
        cout << "  SINOPSIS / RESUMEN:\n";
        cout << "  " << movieFound->plot << "\n";

        printSeparator();
        cout << "  Enlace de Wikipedia: " << movieFound->wikiPage << "\n";
        printSeparator();

        cout << "  [L] Dar Like  |  [W] Agregar/Quitar de 'Ver mas tarde'  |  [0] Volver\n";
        string opt = getInput("  Accion: ");

        if (opt == "0") {
            return;
        } else if (opt == "L" || opt == "l") {
            SessionManager::getInstance().giveLikeToMovie(movieFound->id);
            getInput("  Presiona Enter para continuar...");
        } else if (opt == "W" || opt == "w") {
            SessionManager::getInstance().toggleWatchLater(movieFound->id);
            getInput("  Presiona Enter para continuar...");
        } else {
            cout << "  [!] Opcion invalida.\n";
            getInput("  Presiona Enter para continuar...");
        }
    }
}

// =====================================================================
//  LISTADO PAGINADO DE RESULTADOS (Integra Decorator)
// =====================================================================

// Muestra una lista de películas, paginada de 5 en 5. Desde aquí el
// usuario puede entrar al detalle de una película con [V <id>]; el Like y
// el Watch Later directos se hicieron a un lado a propósito, porque el
// enunciado pide que esas acciones ocurran luego de ver la sinopsis.
void listFlow(SearchEngine& engine, const vector<const Movie*>& list, const string& title) {
    if (list.empty()) {
        cout << "\n  [!] No se encontraron resultados.\n";
        getInput("  Presiona Enter para continuar...");
        return;
    }

    int page = 0;
    int pageSize = 5;
    int totalPages = (static_cast<int>(list.size()) + pageSize - 1) / pageSize;

    while (true) {
        printHeader();
        cout << "  " << title << "\n";

        int start = page * pageSize;
        int end = min(start + pageSize, static_cast<int>(list.size()));

        cout << "\n  Mostrando resultados " << (start + 1) << " - " << end
             << " de " << list.size() << " (Pagina " << (page + 1) << " de " << totalPages << ")\n";
        printSeparator();

        for (int i = start; i < end; i++) {
            const Movie& currentMovie = *list[i];

            // --- PATRON DECORATOR EN ACCION ---
            unique_ptr<MoviePrinter> dynamicPrinter = make_unique<BaseMoviePrinter>();

            if (currentMovie.likes > 0) {
                dynamicPrinter = make_unique<LikeMovieDecorator>(std::move(dynamicPrinter));
            }
            if (currentMovie.watchLater) {
                dynamicPrinter = make_unique<WatchLaterMovieDecorator>(std::move(dynamicPrinter));
            }

            dynamicPrinter->print(currentMovie.id, currentMovie);
            printSeparator();
        }

        cout << "  [N] Siguiente pagina  |  [P] Pagina anterior  |  [V <id>] Ver detalle  |  [0] Volver\n";
        string opt = getInput("  Accion (Ej: 'V 42'): ");

        if (opt == "0") {
            break;
        } else if ((opt == "N" || opt == "n") && page < totalPages - 1) {
            page++;
        } else if ((opt == "P" || opt == "p") && page > 0) {
            page--;
        } else if (opt.size() >= 2 && (toupper(opt[0]) == 'V')) {
            // Extraemos el ID que viene despues de la letra y el espacio
            string idText = opt.substr(2);
            try {
                int targetId = stoi(idText);
                showMovieDetails(engine, targetId);
            } catch (...) {
                cout << "  [!] Formato invalido. Usa la letra, un espacio y el ID. Ej: V 42\n";
                getInput("  Presiona Enter para continuar...");
            }
        } else {
            cout << "  [!] Opcion invalida.\n";
            getInput("  Presiona Enter para continuar...");
        }
    }
}

// =====================================================================
//  ALGORITMO DE RECOMENDACIONES ("similares a tus Likes")
// =====================================================================

// Algoritmo propio y simple: para cada película que el usuario likeó,
// buscamos otras películas que compartan genero y/o director. Sumamos
// puntaje por cada coincidencia (genero pesa menos que director, porque
// el director es una señal de gusto mas especifica) y excluimos las
// peliculas que el usuario ya likeo, para no recomendarle lo que ya vio.
vector<const Movie*> buildLikedRecommendations(SearchEngine& engine, int limit) {
    const vector<int>& likedIds = SessionManager::getInstance().getLikedMovieIds();

    if (likedIds.empty()) {
        return {};
    }

    // Armamos una copia ordenada para poder buscar rapido si una
    // pelicula ya fue likeada
    vector<int> likedIdsSorted = likedIds;
    sort(likedIdsSorted.begin(), likedIdsSorted.end());

    unordered_map<int, double> scores;

    for (int likedId : likedIds) {
        // Ubicamos la pelicula likeada dentro del vector real del engine
        const Movie* likedMovie = nullptr;
        for (const Movie& currentMovie : engine.movies) {
            if (currentMovie.id == likedId) {
                likedMovie = &currentMovie;
                break;
            }
        }
        if (likedMovie == nullptr) {
            continue;
        }

        for (const Movie& candidate : engine.movies) {
            // No nos recomendamos a nosotros mismos
            if (candidate.id == likedMovie->id) {
                continue;
            }
            // No recomendamos algo que el usuario ya likeo
            bool yaLikeada = binary_search(likedIdsSorted.begin(), likedIdsSorted.end(), candidate.id);
            if (yaLikeada) {
                continue;
            }

            if (candidate.genre == likedMovie->genre && likedMovie->genre != "unknown") {
                scores[candidate.id] += 3.0;
            }
            if (candidate.director == likedMovie->director && likedMovie->director != "unknown") {
                scores[candidate.id] += 5.0;
            }
        }
    }

    // Ordenamos por puntaje y, en empate, por cantidad de likes de la comunidad
    vector<pair<double, int>> ranked;
    ranked.reserve(scores.size());
    for (const auto& entry : scores) {
        ranked.push_back({entry.second, entry.first});
    }

    sort(ranked.begin(), ranked.end(),
         [&engine](const pair<double, int>& a, const pair<double, int>& b) {
             if (a.first != b.first) {
                 return a.first > b.first;
             }
             return engine.movies[a.second].likes > engine.movies[b.second].likes;
         });

    int numResults = min(limit, static_cast<int>(ranked.size()));
    vector<const Movie*> result;
    result.reserve(numResults);
    for (int i = 0; i < numResults; i++) {
        result.push_back(&engine.movies[ranked[i].second]);
    }

    return result;
}

// =====================================================================
//  PANTALLA DE INICIO (Watch Later + Recomendaciones)
// =====================================================================

// Esta pantalla se muestra una sola vez, justo despues del login, antes
// de entrar al menu principal. Cumple con el requisito de mostrar la
// lista de 'Ver mas tarde' y las recomendaciones basadas en los Likes.
void showStartupScreen(SearchEngine& engine) {
    printHeader();
    cout << "  Bienvenido de vuelta, " << SessionManager::getInstance().getCurrentUser() << "\n";
    printSeparator();

    // --- Lista de 'Ver mas tarde' ---
    vector<const Movie*> watchLaterList;
    for (const Movie& currentMovie : engine.movies) {
        if (currentMovie.watchLater) {
            watchLaterList.push_back(&currentMovie);
        }
    }

    cout << "\n  Tu lista de 'Ver mas tarde' tiene " << watchLaterList.size() << " pelicula(s).\n";
    if (!watchLaterList.empty()) {
        string verAhora = getInput("  Deseas verla ahora? (s/n): ");
        if (verAhora == "s" || verAhora == "S") {
            listFlow(engine, watchLaterList, "Tu lista: Ver mas tarde");
        }
    }

    // --- Recomendaciones basadas en Likes ---
    vector<const Movie*> recommendations = buildLikedRecommendations(engine, 10);

    printHeader();
    cout << "  Recomendado para ti, basado en tus Likes\n";
    printSeparator();

    if (recommendations.empty()) {
        cout << "\n  Aun no tienes recomendaciones. Dale Like a alguna pelicula\n";
        cout << "  y la proxima vez te mostraremos sugerencias similares.\n";
        getInput("\n  Presiona Enter para continuar al menu principal...");
    } else {
        listFlow(engine, recommendations, "Recomendado para ti");
    }
}

// =====================================================================
//  FLUJO DE BUSQUEDA POR TAG
// =====================================================================

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

        if (opt == "0") {
            return;
        } else if (opt == "1") {
            tag = "title";
        } else if (opt == "2") {
            tag = "director";
        } else if (opt == "3") {
            tag = "cast";
        } else {
            cout << "\n  [!] Opcion invalida. Por favor, selecciona un numero del 0 al 3.\n";
            getInput("  Presiona Enter para continuar...");
            continue;
        }

        cout << "\n";
        string value = getInput("  Ingresa el valor a buscar: ");

        if (value.empty()) {
            cout << "  [!] El valor de busqueda no puede estar vacio.\n";
            getInput("  Presiona Enter para continuar...");
            continue;
        }

        vector<const Movie*> results = engine.searchByTag(tag, value, 1000);

        string label = (opt == "1") ? "Titulo: " : (opt == "2") ? "Director: " : "Elenco: ";
        listFlow(engine, results, label + value);
        break;
    }
}

// =====================================================================
//  FLUJO DE BUSQUEDA AVANZADA (Integra el Builder)
// =====================================================================

void advancedSearchFlow(SearchEngine& engine) {
    printHeader();
    cout << "  [MODO BUSQUEDA AVANZADA]\n";
    printSeparator();
    cout << "  Enter para omitir un campo\n";
    string keyword  = getInput("  Palabra clave en titulo/plot: ");
    string genre    = getInput("  Genero: ");
    string director = getInput("  Director: ");
    string yearStr  = getInput("  Ano: ");

    int year = 0;
    try {
        if (!yearStr.empty()) {
            year = stoi(yearStr);
        }
    } catch (...) {
        year = 0;
    }

    // --- PATRON BUILDER EN ACCION ---
    SearchQueryBuilder queryBuilder;

    if (!keyword.empty())  queryBuilder.withTextKeyword(keyword);
    if (!genre.empty())    queryBuilder.withGenre(genre);
    if (!director.empty()) queryBuilder.withDirector(director);
    if (year != 0)         queryBuilder.withYear(year);

    SearchQuery complexQuery = queryBuilder.build();

    vector<const Movie*> results = engine.searchAdvanced(complexQuery, 1000);
    listFlow(engine, results, "Resultados de Filtro Avanzado");
}

// =====================================================================
//  MENU PRINCIPAL
// =====================================================================

// Devuelve true si el usuario eligio salir del programa por completo,
// y false si eligio cerrar sesion (para volver a pedir login).
bool mainMenuFlow(SearchEngine& engine, ConsoleNotificationSystem& notifier) {
    while (true) {
        printHeader();
        mostrarNotificacionesSistema(notifier);

        cout << "  Hola, " << SessionManager::getInstance().getCurrentUser() << " | Que deseas hacer?\n";
        printSeparator();
        cout << "  [1] Busqueda Rapida (Texto)\n";
        cout << "  [2] Busqueda por Tag Especifico\n";
        cout << "  [3] Busqueda Avanzada (Patron Builder + Paralelismo)\n";
        cout << "  [4] Cerrar Sesion\n";
        cout << "  [0] Salir del Programa\n";
        printSeparator();

        string opt = getInput("  Opcion: ");

        if (opt == "0") {
            cout << "  Hasta luego!\n";
            return true;
        } else if (opt == "1") {
            string query = getInput("  Buscar: ");
            if (!query.empty()) {
                vector<const Movie*> results = engine.searchText(query, 1000);
                listFlow(engine, results, "Resultados para: " + query);
            }
        } else if (opt == "2") {
            searchTagFlow(engine);
        } else if (opt == "3") {
            advancedSearchFlow(engine);
        } else if (opt == "4") {
            // Patron Observer: al hacer logout, dejamos de escuchar
            // notificaciones de esta sesion y volvemos a pedir login
            SessionManager::getInstance().detachObserver(&notifier);
            SessionManager::getInstance().logoutUser();
            cout << "  Sesion cerrada.\n";
            return false;
        } else {
            cout << "  [!] Opcion no valida.\n";
            getInput("  Presiona Enter para continuar...");
        }
    }
}

// =====================================================================
//  PUNTO DE ENTRADA
// =====================================================================

int main(int argc, char* argv[]) {
    string csvPath = "../data/raw/movies.csv"; // Ajusta la ruta a donde tengas el archivo al compilar
    if (argc > 1) {
        csvPath = argv[1];
    }

    printHeader();
    cout << "  Cargando base de datos...\n";

    SearchEngine engine;
    if (!engine.load(csvPath)) {
        return 1;
    }

    // === PATRON SINGLETON ===
    SessionManager::getInstance().setSearchEngine(&engine);

    // === PATRON OBSERVER ===
    static ConsoleNotificationSystem notifier;

    // El programa corre en un bucle externo para permitir multiples
    // sesiones de login/logout sin tener que reiniciar el ejecutable
    while (true) {
        printHeader();
        string username = getInput("  Ingresa tu nombre de usuario para iniciar sesion: ");
        if (username.empty()) {
            username = "HermanoBuscador";
        }

        SessionManager::getInstance().loginUser(username);
        SessionManager::getInstance().attachObserver(&notifier);

        // Pantalla de inicio: Watch Later + Recomendaciones por Likes
        showStartupScreen(engine);

        // Menu principal de busqueda
        bool userExitedProgram = mainMenuFlow(engine, notifier);

        if (userExitedProgram) {
            break;
        }

        // El usuario hizo logout explicito (opcion 4): preguntamos si
        // quiere iniciar sesion con otro usuario o cerrar el programa
        string seguir = getInput("  Deseas iniciar sesion con otro usuario? (s/n): ");
        if (seguir != "s" && seguir != "S") {
            break;
        }
    }

    return 0;
}
