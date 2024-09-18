#include <gtk/gtk.h>
#include <cairo.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "SortingAlgorithms.h"
#include "DataStructures.h"
#include "Timer.h"

using namespace std;

// Datos globales para el gráfico
vector<int> sizes = {100, 1000, 5000, 10000, 20000};
vector<long long> tiemposBubbleSort, tiemposBubbleSortMejor, tiemposBubbleSortPeor;
vector<long long> tiemposSelectionSort, tiemposSelectionSortMejor, tiemposSelectionSortPeor;
vector<long long> tiemposMergeSort, tiemposMergeSortMejor, tiemposMergeSortPeor;
vector<long long> tiemposLinkedListSearch, tiemposLinkedListSearchMejor, tiemposLinkedListSearchPeor;
vector<long long> tiemposBSTInsert, tiemposBSTInsertMejor, tiemposBSTInsertPeor;

// Función para generar un arreglo aleatorio
vector<int> generarArregloAleatorio(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 10000;
    }
    return arr;
}

// Función para generar el mejor caso (arreglo ya ordenado)
vector<int> generarMejorCaso(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = i;  // Generamos un arreglo ordenado de 0 a n-1
    }
    return arr;
}

// Función para generar el peor caso (arreglo ordenado de manera inversa)
vector<int> generarPeorCaso(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = n - i;  // Generamos un arreglo en orden inverso
    }
    return arr;
}

// Función recursiva para insertar en BST de manera balanceada
void insertarBalanceado(BST<int>& bst, const vector<int>& datos, int inicio, int fin) {
    if (inicio > fin) {
        return;  // Caso base, terminamos cuando no quedan más elementos
    }

    // Calcular el punto medio y agregarlo al BST
    int medio = (inicio + fin) / 2;
    bst.insert(datos[medio]);

    // Insertar recursivamente en la mitad izquierda y derecha
    insertarBalanceado(bst, datos, inicio, medio - 1);
    insertarBalanceado(bst, datos, medio + 1, fin);
}

// Función para generar un conjunto de datos balanceado para el mejor caso del BST
void generarMejorCasoBST(BST<int>& bst, int n) {
    vector<int> arrMejor = generarMejorCaso(n);  // Obtener un arreglo ordenado de 0 a n-1
    insertarBalanceado(bst, arrMejor, 0, n - 1); // Insertar de forma balanceada
}

// Función para dibujar etiquetas en la parte superior derecha
void draw_label(cairo_t *cr, const char *text, double x, double y, double r, double g, double b) {
    cairo_set_source_rgb(cr, r, g, b);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, text);
}

// Función para dibujar los ejes X e Y con etiquetas
void draw_axes(cairo_t *cr, int width, int height) {
    // Dibujar el eje X
    cairo_set_source_rgb(cr, 0, 0, 0);  // Negro
    cairo_move_to(cr, 50, height - 50); // Margen de 50 px
    cairo_line_to(cr, width - 50, height - 50);  // De izquierda a derecha
    cairo_stroke(cr);

    // Dibujar el eje Y
    cairo_move_to(cr, 50, height - 50); // Margen de 50 px
    cairo_line_to(cr, 50, 50);  // De abajo hacia arriba
    cairo_stroke(cr);

    // Dibujar divisiones en el eje X (etiquetas para N)
    for (int i = 0; i < sizes.size(); i++) {
        int x_pos = 50 + i * (width - 100) / (sizes.size() - 1);  // Escalar en el rango de la gráfica
        cairo_move_to(cr, x_pos, height - 50);
        cairo_line_to(cr, x_pos, height - 45);
        cairo_stroke(cr);

        // Dibujar etiquetas de N (tamaño del arreglo)
        cairo_move_to(cr, x_pos - 10, height - 30);  // Posicionar la etiqueta
        cairo_show_text(cr, to_string(sizes[i]).c_str());
    }

    // Dibujar divisiones en el eje Y (etiquetas para el tiempo)
    for (int i = 0; i <= 5; i++) {
        int y_pos = height - 50 - i * (height - 100) / 5;  // Dividir en 5 partes iguales
        cairo_move_to(cr, 50, y_pos);
        cairo_line_to(cr, 55, y_pos);
        cairo_stroke(cr);

        // Dibujar etiquetas del tiempo (escala de tiempos en nanosegundos)
        cairo_move_to(cr, 20, y_pos + 5);
        cairo_show_text(cr, to_string(i * (1000000000 / 5)).c_str());  // Escala 1 a 5 en ns
    }

    // Agregar etiquetas de los ejes
    cairo_set_font_size(cr, 16);

    // Etiqueta para el eje X (Tamaño del arreglo)
    cairo_move_to(cr, width / 2 - 50, height - 10);  // Centrar debajo del eje X
    cairo_show_text(cr, "Tamaño del arreglo (N)");

    // Etiqueta para el eje Y (Tiempo)
    cairo_move_to(cr, 10, height / 2);  // Posicionar cerca del centro del eje Y
    cairo_save(cr);
    cairo_rotate(cr, -M_PI / 2);  // Rotar el texto verticalmente
    cairo_show_text(cr, "Tiempo (nanosegundos)");
    cairo_restore(cr);
}

// Función para dibujar las líneas y las leyendas
void draw_lines_and_legend(cairo_t *cr, int width, int height, const vector<long long>& tiemposBubbleSort, const vector<long long>& tiemposSelectionSort, const vector<long long>& tiemposMergeSort, const vector<long long>& tiemposLinkedListSearch, const vector<long long>& tiemposBSTInsert) {
    // Configuración de escalas
    double max_time = *max_element(tiemposBubbleSort.begin(), tiemposBubbleSort.end());
    double x_scale = (double) (width - 100) / (sizes.back());
    double y_scale = (double) (height - 100) / max_time;

    // Dibujar las líneas de BubbleSort
    cairo_set_source_rgb(cr, 0.6, 0.2, 0.8);  // Púrpura
    cairo_move_to(cr, 50, height - 50 - tiemposBubbleSort[0] * y_scale);
    for (size_t i = 1; i < tiemposBubbleSort.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - tiemposBubbleSort[i] * y_scale);
    }
    cairo_stroke(cr);

    // Dibujar la curva teórica de BubbleSort (O(n^2))
    cairo_set_source_rgb(cr, 0.2, 0.8, 0.6);  // Verde
    double dashes[] = {4.0};
    cairo_set_dash(cr, dashes, 1, 0);  // Línea discontinua
    cairo_move_to(cr, 50, height - 50);
    for (size_t i = 1; i < sizes.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - pow(sizes[i], 2) / pow(sizes.back(), 2) * (height - 100));
    }
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0);  // Eliminar el patrón de línea discontinua

    // Dibujar las líneas de SelectionSort
    cairo_set_source_rgb(cr, 0.8, 0.4, 0.0);  // Naranja
    cairo_move_to(cr, 50, height - 50 - tiemposSelectionSort[0] * y_scale);
    for (size_t i = 1; i < tiemposSelectionSort.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - tiemposSelectionSort[i] * y_scale);
    }
    cairo_stroke(cr);

    // Dibujar la curva teórica de SelectionSort (O(n^2))
    cairo_set_source_rgb(cr, 0.4, 0.2, 0.2);  // Marrón oscuro
    cairo_set_dash(cr, dashes, 1, 0);  // Línea discontinua
    cairo_move_to(cr, 50, height - 50);
    for (size_t i = 1; i < sizes.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - pow(sizes[i], 2) / pow(sizes.back(), 2) * (height - 100));
    }
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0);  // Eliminar el patrón de línea discontinua

    // Dibujar las líneas de MergeSort
    cairo_set_source_rgb(cr, 0.2, 0.6, 0.9);  // Azul celeste
    cairo_move_to(cr, 50, height - 50 - tiemposMergeSort[0] * y_scale);
    for (size_t i = 1; i < tiemposMergeSort.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - tiemposMergeSort[i] * y_scale);
    }
    cairo_stroke(cr);

    // Dibujar la curva teórica de MergeSort (O(n log n))
    cairo_set_source_rgb(cr, 0.5, 0.2, 0.8);  // Lila oscuro
    cairo_set_dash(cr, dashes, 1, 0);  // Línea discontinua
    cairo_move_to(cr, 50, height - 50);
    for (size_t i = 1; i < sizes.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - sizes[i] * log(sizes[i]) / (sizes.back() * log(sizes.back())) * (height - 100));
    }
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0);  // Eliminar el patrón de línea discontinua

    // Dibujar las líneas de LinkedList (Búsqueda)
    cairo_set_source_rgb(cr, 0.8, 0.0, 0.2);  // Rojo
    cairo_move_to(cr, 50, height - 50 - tiemposLinkedListSearch[0] * y_scale);
    for (size_t i = 1; i < tiemposLinkedListSearch.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - tiemposLinkedListSearch[i] * y_scale);
    }
    cairo_stroke(cr);

    // Dibujar las líneas de BST (Inserción)
    cairo_set_source_rgb(cr, 0.0, 0.4, 0.8);  // Azul
    cairo_move_to(cr, 50, height - 50 - tiemposBSTInsert[0] * y_scale);
    for (size_t i = 1; i < tiemposBSTInsert.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - tiemposBSTInsert[i] * y_scale);
    }
    cairo_stroke(cr);

    // Dibujar etiquetas (leyendas) para cada algoritmo
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 13);

    draw_label(cr, "BubbleSort (O(n^2))", width - 180, 30, 0.6, 0.2, 0.8);  // Púrpura
    draw_label(cr, "SelectionSort (O(n^2))", width - 180, 50, 0.8, 0.4, 0.0);  // Naranja
    draw_label(cr, "MergeSort (O(n log n))", width - 180, 70, 0.2, 0.6, 0.9);  // Azul celeste
    draw_label(cr, "LinkedList Search", width - 180, 90, 0.8, 0.0, 0.2);  // Rojo
    draw_label(cr, "BST Insert", width - 180, 110, 0.0, 0.4, 0.8);  // Azul
}

// Función de dibujo para el gráfico (caso promedio)
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    // Dibujar ejes
    draw_axes(cr, width, height);

    // Dibujar líneas y leyendas
    draw_lines_and_legend(cr, width, height, tiemposBubbleSort, tiemposSelectionSort, tiemposMergeSort, tiemposLinkedListSearch, tiemposBSTInsert);

    return FALSE;
}

// Función de dibujo para el gráfico (mejor caso)
static gboolean on_draw_event_mejor(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    // Dibujar ejes
    draw_axes(cr, width, height);

    // Dibujar líneas y leyendas para el mejor caso
    draw_lines_and_legend(cr, width, height, tiemposBubbleSortMejor, tiemposSelectionSortMejor, tiemposMergeSortMejor, tiemposLinkedListSearchMejor, tiemposBSTInsertMejor);

    return FALSE;
}

// Función de dibujo para el gráfico (peor caso)
static gboolean on_draw_event_peor(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    // Dibujar ejes
    draw_axes(cr, width, height);

    // Dibujar líneas y leyendas para el peor caso
    draw_lines_and_legend(cr, width, height, tiemposBubbleSortPeor, tiemposSelectionSortPeor, tiemposMergeSortPeor, tiemposLinkedListSearchPeor, tiemposBSTInsertPeor);

    return FALSE;
}

// Función para medir tiempos en los tres casos: Mejor, Peor, Promedio
void medirTiempos() {
    BubbleSort<int> bubbleSort;
    SelectionSort<int> selectionSort;
    MergeSort<int> mergeSort;
    LinkedList<int> linkedList;
    BST<int> bst;

    for (int n : sizes) {
        // Generar los arreglos
        vector<int> arrAleatorio = generarArregloAleatorio(n);
        vector<int> arrMejor = generarMejorCaso(n);   // Mejor caso (ordenado)
        vector<int> arrPeor = generarPeorCaso(n);     // Peor caso (orden inverso)

        // Medir tiempos para algoritmos de ordenamiento en el caso promedio (aleatorio)
        tiemposBubbleSort.push_back(Timer::medirTiempo([&](vector<int>& a){ bubbleSort.sort(a); }, arrAleatorio));
        tiemposSelectionSort.push_back(Timer::medirTiempo([&](vector<int>& a){ selectionSort.sort(a); }, arrAleatorio));
        tiemposMergeSort.push_back(Timer::medirTiempo([&](vector<int>& a){ mergeSort.sort(a); }, arrAleatorio));

        // Medir tiempos para algoritmos de ordenamiento en el mejor caso (ordenado)
        tiemposBubbleSortMejor.push_back(Timer::medirTiempo([&](vector<int>& a){ bubbleSort.sort(a); }, arrMejor));
        tiemposSelectionSortMejor.push_back(Timer::medirTiempo([&](vector<int>& a){ selectionSort.sort(a); }, arrMejor));
        tiemposMergeSortMejor.push_back(Timer::medirTiempo([&](vector<int>& a){ mergeSort.sort(a); }, arrMejor));

        // Medir tiempos para algoritmos de ordenamiento en el peor caso (orden inverso)
        tiemposBubbleSortPeor.push_back(Timer::medirTiempo([&](vector<int>& a){ bubbleSort.sort(a); }, arrPeor));
        tiemposSelectionSortPeor.push_back(Timer::medirTiempo([&](vector<int>& a){ selectionSort.sort(a); }, arrPeor));
        tiemposMergeSortPeor.push_back(Timer::medirTiempo([&](vector<int>& a){ mergeSort.sort(a); }, arrPeor));

        // Medir tiempos para LinkedList Search y BST Insert en los tres casos
        // Insertar los elementos antes de medir la búsqueda e inserción
        for (int val : arrAleatorio) {
            linkedList.insert(val);
            bst.insert(val);
        }

        // LinkedList Search - Promedio
        tiemposLinkedListSearch.push_back(Timer::medirTiempo([&](vector<int>& a){
            for (int val : a) { linkedList.search(val); }  // Buscar todos los elementos
        }, arrAleatorio));

        // LinkedList Search - Mejor caso (primero)
        tiemposLinkedListSearchMejor.push_back(Timer::medirTiempo([&](vector<int>& a){
            linkedList.search(a[0]);  // Buscar el primer elemento
        }, arrAleatorio));

        // LinkedList Search - Peor caso (último)
        tiemposLinkedListSearchPeor.push_back(Timer::medirTiempo([&](vector<int>& a){
            linkedList.search(a[n-1]);  // Buscar el último elemento
        }, arrAleatorio));

        // BST Insert - Promedio
        tiemposBSTInsert.push_back(Timer::medirTiempo([&](vector<int>& a){
            for (int val : a) { bst.insert(val); }
        }, arrAleatorio));

        // BST Insert - Mejor caso (balanceado)
        tiemposBSTInsertMejor.push_back(Timer::medirTiempo([&](vector<int>& a){
            BST<int> bstBalanceado;
            generarMejorCasoBST(bstBalanceado, a.size());  // Insertar de manera balanceada
        }, arrMejor));

        // BST Insert - Peor caso (desbalanceado)
        tiemposBSTInsertPeor.push_back(Timer::medirTiempo([&](vector<int>& a){
            for (int val : a) { bst.insert(val); }
        }, arrPeor));
    }
}

// Función de callback para el evento de activación de la aplicación
void on_activate(GtkApplication *app, gpointer user_data) {
    medirTiempos();  // Medir los tiempos antes de crear los gráficos

    // Ventana para el caso promedio
    GtkWidget *window_promedio = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window_promedio), "Gráfico Caso Promedio");
    gtk_window_set_default_size(GTK_WINDOW(window_promedio), 800, 600);

    GtkWidget *darea_promedio = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(darea_promedio), "draw", G_CALLBACK(on_draw_event), NULL);
    gtk_container_add(GTK_CONTAINER(window_promedio), darea_promedio);
    gtk_widget_show_all(window_promedio);

    // Ventana para el mejor caso
    GtkWidget *window_mejor = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window_mejor), "Gráfico Mejor Caso");
    gtk_window_set_default_size(GTK_WINDOW(window_mejor), 800, 600);

    GtkWidget *darea_mejor = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(darea_mejor), "draw", G_CALLBACK(on_draw_event_mejor), NULL);
    gtk_container_add(GTK_CONTAINER(window_mejor), darea_mejor);
    gtk_widget_show_all(window_mejor);

    // Ventana para el peor caso
    GtkWidget *window_peor = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window_peor), "Gráfico Peor Caso");
    gtk_window_set_default_size(GTK_WINDOW(window_peor), 800, 600);

    GtkWidget *darea_peor = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(darea_peor), "draw", G_CALLBACK(on_draw_event_peor), NULL);
    gtk_container_add(GTK_CONTAINER(window_peor), darea_peor);
    gtk_widget_show_all(window_peor);
}

int main(int argc, char *argv[]) {
    srand(time(nullptr));  // Inicializar el generador de números aleatorios

    // Inicializar GTK
    GtkApplication *app;
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
