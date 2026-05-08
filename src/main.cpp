#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include "../include/Movie.h"
#include "../include/SearchEngine.h"

using namespace std;

string truncate(const string& s, size_t maxLen = 50) {
    if (s.size() <= maxLen) return s;
    return s.substr(0, maxLen - 3) + "...";
}

string getInput(const string& prompt = "") {
    if (!prompt.empty()) cout << prompt;
    string line;
    getline(cin, line);
    return line;
}

void printSeparator() {
    for (int i = 0; i < 60; i++) cout << "-";
    cout << "\n";
}

void printHeader() {
    cout << "\n";
    printSeparator();
    cout << "         TrieFlix - Plataforma de Peliculas\n";
    printSeparator();
}

void printCard(int index, const Movie& m) {
    cout << " [" << index << "] " << truncate(m.title, 44)
         << " (" << m.releaseYear << ")\n";
    cout << "      Dir: " << truncate(m.director, 28)
         << "  |  " << truncate(m.genre, 18) << "\n";
}

void printList(const vector<const Movie*>& list, int page, int pageSize = 5) {
    int start = page * pageSize;
    int end   = min(start + pageSize, (int)list.size());
    cout << "  Mostrando " << (start + 1) << " - " << end
         << " de " << list.size() << " resultados\n";
    printSeparator();
    for (int i = start; i < end; i++)
        printCard(i - start + 1, *list[i]);
}

void printDetail(const Movie& m) {
    printSeparator();
    cout << m.title << " (" << m.releaseYear << ")\n";
    printSeparator();
    cout << "Director : " << m.director << "\n";
    cout << "Reparto  : " << truncate(m.cast, 68) << "\n";
    cout << "Genero   : " << m.genre << "\n";
    printSeparator();
    cout << "SINOPSIS:\n";

    const string& plot = m.plot;
    size_t pos = 0;
    while (pos < plot.size()) {
        size_t end = min(pos + 70, plot.size());
        if (end < plot.size())
            while (end > pos && plot[end] != ' ') end--;
        cout << "  " << plot.substr(pos, end - pos) << "\n";
        pos = (end < plot.size()) ? end + 1 : plot.size();
    }
    printSeparator();
}

void listFlow(SearchEngine& engine,
              const vector<const Movie*>& results,
              const string& titulo) {

    if (results.empty()) {
        cout << "  No se encontraron resultados.\n";
        getInput("  [Enter] para continuar...");
        return;
    }

    int page     = 0;
    int pageSize = 5;
    int total    = (int)results.size();
    int totalPgs = (total + pageSize - 1) / pageSize;

    while (true) {
        printHeader();
        cout << "  " << titulo << "\n";
        printList(results, page, pageSize);

        cout << "\n";
        if (page > 0)            cout << "  [P] Anteriores\n";
        if (page < totalPgs - 1) cout << "  [N] Siguientes 5\n";
        cout << "  [1-5] Ver detalle\n";
        cout << "  [B] Volver\n";
        printSeparator();

        string opt = getInput("Opcion: ");
        if (opt.empty()) continue;
        char c = tolower((unsigned char)opt[0]);

        if (c == 'b') break;
        else if (c == 'n' && page < totalPgs - 1) page++;
        else if (c == 'p' && page > 0)            page--;
        else if (isdigit((unsigned char)c)) {
            int sel    = (c - '0') - 1;
            int absIdx = page * pageSize + sel;
            if (absIdx >= 0 && absIdx < total) {
                printHeader();
                printDetail(*results[absIdx]);
                getInput("  [Enter] para continuar...");
            }
        }
    }
}

void searchTextFlow(SearchEngine& engine) {
    printHeader();
    cout << "  Busqueda por texto\n";
    printSeparator();

    string query = getInput("  Buscar: ");
    if (query.empty()) return;

    vector<const Movie*> results = engine.searchText(query, 1000);
    listFlow(engine, results, "Resultados para: " + query);
}

void searchTagFlow(SearchEngine& engine) {
    printHeader();
    cout << "  Busqueda por Tag\n";
    printSeparator();
    cout << "  Tags: director  cast  genre  year\n\n";

    string tag   = getInput("  Tag   : ");
    string value = getInput("  Valor : ");
    if (tag.empty() || value.empty()) return;

    vector<const Movie*> results = engine.searchByTag(tag, value, 1000);
    listFlow(engine, results, tag + ": " + value);
}

int main(int argc, char* argv[]) {
    string csvPath = "../data/raw/movies.csv";
    if (argc > 1) csvPath = argv[1];

    printHeader();
    cout << "  Cargando base de datos...\n";

    SearchEngine engine;
    if (!engine.load(csvPath)) return 1;

    while (true) {
        printHeader();
        cout << "  Que deseas hacer?\n";
        printSeparator();
        cout << "  [1] Buscar pelicula\n";
        cout << "  [2] Buscar por tag\n";
        cout << "  [0] Salir\n";
        printSeparator();

        string opt = getInput("Opcion: ");
        if (opt.empty()) continue;
        char c = opt[0];

        if      (c == '1') searchTextFlow(engine);
        else if (c == '2') searchTagFlow(engine);
        else if (c == '0') {
            cout << "\n  Hasta pronto!\n";
            break;
        }
    }

    return 0;
}