cmake_minimum_required(VERSION 3.10)
project(untitled)

# Establecer el estándar de C++
set(CMAKE_CXX_STANDARD 17)

# Encontrar las bibliotecas usando pkg-config
find_package(PkgConfig REQUIRED)

# Buscar GTK y Cairo
pkg_check_modules(GTK REQUIRED gtk+-3.0)
pkg_check_modules(CAIRO REQUIRED cairo)

# Incluir los directorios de GTK y Cairo
include_directories(${GTK_INCLUDE_DIRS})
include_directories(${CAIRO_INCLUDE_DIRS})

# Vincular las bibliotecas de GTK y Cairo
link_directories(${GTK_LIBRARY_DIRS})
link_directories(${CAIRO_LIBRARY_DIRS})

# Definir las banderas adicionales de compilación que necesita GTK y Cairo
add_definitions(${GTK_CFLAGS_OTHER})
add_definitions(${CAIRO_CFLAGS_OTHER})

# Añadir el ejecutable y los archivos fuente
add_executable(untitled main.cpp SortingAlgorithms.h Timer.h DataStructures.h)

# Vincular las bibliotecas de GTK y Cairo al ejecutable
target_link_libraries(untitled ${GTK_LIBRARIES} ${CAIRO_LIBRARIES})
