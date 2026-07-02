# 🎬 TrieFlix

**TrieFlix** es un motor de búsqueda de películas por consola, desarrollado en **C++17**. Permite explorar un catálogo de miles de películas con búsquedas rápidas por título, director, elenco, género y año, gracias a una estructura de datos basada en **Trie de sufijos**.

El proyecto también incorpora un sistema de sesiones de usuario, likes, lista de "Ver más tarde" y recomendaciones personalizadas.

## ✨ Características

- 🔎 **Búsqueda rápida por texto**: encuentra películas por coincidencias parciales (subpalabras) en título, director o elenco.
- 🏷️ **Búsqueda por tag específico**: filtra por título, director, elenco, género o año de estreno.
- 🧩 **Búsqueda avanzada**: combina múltiples filtros (palabra clave + género + director + año) en una sola consulta.
- 👤 **Perfil de usuario**: inicio de sesión simple, lista de "Ver más tarde", historial de likes y recomendaciones basadas en tus gustos.
- 🔔 **Notificaciones en consola**: avisos en tiempo real cuando das like o guardas una película, mediante el patrón Observer.
- 🖨️ **Impresión decorada de resultados**: los datos de cada película (likes, guardado) se agregan dinámicamente con el patrón Decorator.

## 🏗️ Arquitectura y patrones de diseño

El proyecto está organizado en módulos desacoplados, aplicando varios patrones de diseño clásicos:

| Componente | Responsabilidad | Patrón |
|---|---|---|
| `SuffixTrie` | Indexación y búsqueda por sufijos/prefijos en O(logN) | Trie |
| `SearchEngine` | Orquesta la carga de datos y las búsquedas sobre los tries | — |
| `SearchQuery` / `SearchQueryBuilder` | Construcción fluida de consultas con múltiples filtros | Builder |
| `SessionManager` | Gestión de la sesión del usuario (único, global) | Singleton |
| `MovieStateObserver` / `ConsoleNotificationSystem` | Notificaciones ante cambios de estado (like, watch later) | Observer |
| `MoviePrinter` y decoradores (`LikeMovieDecorator`, `WatchLaterMovieDecorator`) | Impresión de películas con información adicional configurable | Decorator |
| `CsvReader` | Lectura y parseo del dataset CSV | — |
| `Utils` | Normalización de texto (minúsculas, sin puntuación, tokenización, stop words) | — |

## 📁 Estructura del proyecto

```txt
trieflix/
├── include/          # Headers (.h) de todas las clases
├── src/               # Implementación (.cpp) + main.cpp (interfaz de consola)
├── data/raw/          # Dataset de películas (movies.csv)
├── CMakeLists.txt     # Configuración de build
└── README.md
```

## 🛠️ Tecnologías

- **C++17**
- **STL** (contenedores, smart pointers, hilos)
- **CMake** (>= 3.10)

## 🚀 Compilación y ejecución

### Requisitos previos
- Compilador compatible con C++17 (GCC, Clang o MSVC)
- CMake 3.10 o superior

### Pasos

```bash
# 1. Clonar el repositorio
git clone https://github.com/lzmla11/trieflix.git
cd trieflix

# 2. Configurar y generar el proyecto
mkdir build && cd build
cmake ..

# 3. Compilar
cmake --build .

# 4. Ejecutar
./trieflix        # Linux / macOS
trieflix.exe      # Windows
```

> El programa carga automáticamente el dataset ubicado en `data/raw/movies.csv` al iniciar.

## 📖 Uso

Al ejecutar el programa se muestra un menú principal por consola:

```
[1] Busqueda Rapida (Texto)
[2] Busqueda por Tag Especifico
[3] Busqueda Avanzada
[4] Mi Perfil
[5] Cerrar Sesion
[0] Salir del Programa
```

Desde cualquier resultado de búsqueda puedes ver el detalle de una película, donde es posible:
- Darle **like** (`L`)
- Agregarla o quitarla de **"Ver más tarde"** (`W`)

Desde **Mi Perfil** puedes consultar tu lista de "Ver más tarde", tus películas con like y tus recomendaciones personalizadas.

## 📊 Dataset

El proyecto utiliza un archivo CSV (`data/raw/movies.csv`) con las columnas: `Release Year`, `Title`, `Origin/Ethnicity`, `Director`, `Cast`, `Genre`, `Wiki Page`, `Plot`.

## 🎯 Objetivos del proyecto

- Lectura y preprocesamiento de datos CSV
- Indexación eficiente mediante Trie de sufijos
- Búsqueda de texto rápida y flexible (por subpalabras)
- Ranking y filtrado de resultados
- Sistema de recomendaciones basado en el historial del usuario

## 👥 Integrantes

- Edmundo Andre Centti Torres
- Humberto Ricardo Velito Neira
- Percy Castro Gómez
