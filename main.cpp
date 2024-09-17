#include <gtk/gtk.h>
#include <cairo.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>  // Para max_element
#include "SortingAlgorithms.h"
#include "DataStructures.h"
#include "Timer.h"

using namespace std;

// Datos globales para el gráfico
vector<int> sizes = {100, 1000, 5000, 10000, 20000};
vector<long long> tiemposBubbleSort;
vector<long long> tiemposSelectionSort;
vector<long long> tiemposMergeSort;
vector<long long> tiemposInsertLinkedList;
vector<long long> tiemposSearchLinkedList;
vector<long long> tiemposInsertBST;
vector<long long> tiemposSearchBST;

// Función para generar un arreglo aleatorio
vector<int> generarArregloAleatorio(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 10000;
    }
    return arr;
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

// Función de dibujo para mostrar los resultados con Cairo
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    // Configuración de escalas
    double max_time = *max_element(tiemposBubbleSort.begin(), tiemposBubbleSort.end());
    double x_scale = (double) (width - 100) / (sizes.back());
    double y_scale = (double) (height - 100) / max_time;

    // Dibujar los ejes
    draw_axes(cr, width, height);

    // Definir el patrón de línea discontinua
    double dashes[] = {4.0};

    // Dibujar las líneas de BubbleSort (real)
    cairo_set_source_rgb(cr, 0, 0, 1);  // Azul
    cairo_move_to(cr, 50, height - 50 - tiemposBubbleSort[0] * y_scale);
    for (size_t i = 1; i < tiemposBubbleSort.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - tiemposBubbleSort[i] * y_scale);
    }
    cairo_stroke(cr);

    // Dibujar la curva teórica de BubbleSort (O(n^2))
    cairo_set_source_rgb(cr, 0, 0, 0);  // Negro para líneas teóricas
    cairo_set_line_width(cr, 1.5);
    cairo_set_dash(cr, dashes, 1, 0);  // Línea discontinua
    cairo_move_to(cr, 50, height - 50);
    for (size_t i = 1; i < sizes.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - pow(sizes[i], 2) / pow(sizes.back(), 2) * (height - 100));
    }
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0);  // Eliminar el patrón de línea discontinua

    // Dibujar las líneas de SelectionSort (real)
    cairo_set_source_rgb(cr, 1, 0, 0);  // Rojo
    cairo_move_to(cr, 50, height - 50 - tiemposSelectionSort[0] * y_scale);
    for (size_t i = 1; i < tiemposSelectionSort.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - tiemposSelectionSort[i] * y_scale);
    }
    cairo_stroke(cr);

    // Dibujar la curva teórica de SelectionSort (O(n^2))
    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);  // Gris oscuro para líneas teóricas
    cairo_set_line_width(cr, 1.5);
    cairo_set_dash(cr, dashes, 1, 0);  // Línea discontinua
    cairo_move_to(cr, 50, height - 50);
    for (size_t i = 1; i < sizes.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - pow(sizes[i], 2) / pow(sizes.back(), 2) * (height - 100));
    }
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0);  // Eliminar el patrón de línea discontinua

    // Dibujar las líneas de MergeSort (real)
    cairo_set_source_rgb(cr, 0, 1, 0);  // Verde
    cairo_move_to(cr, 50, height - 50 - tiemposMergeSort[0] * y_scale);
    for (size_t i = 1; i < tiemposMergeSort.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - tiemposMergeSort[i] * y_scale);
    }
    cairo_stroke(cr);

    // Dibujar la curva teórica de MergeSort (O(n log n))
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);  // Gris más oscuro para líneas teóricas
    cairo_set_line_width(cr, 1.5);
    cairo_set_dash(cr, dashes, 1, 0);  // Línea discontinua
    cairo_move_to(cr, 50, height - 50);
    for (size_t i = 1; i < sizes.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - sizes[i] * log(sizes[i]) / (sizes.back() * log(sizes.back())) * (height - 100));
    }
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0);  // Eliminar el patrón de línea discontinua

    // Dibujar las líneas de LinkedList (Inserción)
    cairo_set_source_rgb(cr, 0, 0, 0.6);  // Negro-Azul para LinkedList
    cairo_move_to(cr, 50, height - 50 - tiemposInsertLinkedList[0] * y_scale);
    for (size_t i = 1; i < tiemposInsertLinkedList.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - tiemposInsertLinkedList[i] * y_scale);
    }
    cairo_stroke(cr);

    // Dibujar las líneas de BST (Inserción)
    cairo_set_source_rgb(cr, 0.6, 0, 0.6);  // Púrpura para BST Inserción
    cairo_move_to(cr, 50, height - 50 - tiemposInsertBST[0] * y_scale);
    for (size_t i = 1; i < tiemposInsertBST.size(); i++) {
        cairo_line_to(cr, 50 + sizes[i] * x_scale, height - 50 - tiemposInsertBST[i] * y_scale);
    }
    cairo_stroke(cr);

    // Dibujar etiquetas (leyendas) para cada algoritmo
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 13);

    draw_label(cr, "BubbleSort (O(n^2))", width - 180, 30, 0, 0, 1);  // Azul
    draw_label(cr, "SelectionSort (O(n^2))", width - 180, 50, 1, 0, 0);  // Rojo
    draw_label(cr, "MergeSort (O(n log n))", width - 180, 70, 0, 1, 0);  // Verde
    draw_label(cr, "LinkedList Insert", width - 180, 90, 0, 0, 0.6);  // Negro-Azul
    draw_label(cr, "BST Insert", width - 180, 110, 0.6, 0, 0.6);  // Púrpura

    return FALSE;
}

// Función de callback para el evento de activación de la aplicación
void on_activate(GtkApplication *app, gpointer user_data) {
    // Crear la ventana
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Gráfico con Cairo y GTK");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Crear el área de dibujo para el gráfico
    GtkWidget *darea = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);
    gtk_container_add(GTK_CONTAINER(window), darea);

    gtk_widget_show_all(window);
}

int main(int argc, char *argv[]) {
    // Generar datos
    BubbleSort<int> bubbleSort;
    SelectionSort<int> selectionSort;
    MergeSort<int> mergeSort;
    LinkedList<int> linkedList;
    BST<int> bst;

    for (int n : sizes) {
        vector<int> arrAleatorio = generarArregloAleatorio(n);

        // Medir tiempos para algoritmos de ordenamiento
        tiemposBubbleSort.push_back(Timer::medirTiempo([&](vector<int>& a){ bubbleSort.sort(a); }, arrAleatorio));
        tiemposSelectionSort.push_back(Timer::medirTiempo([&](vector<int>& a){ selectionSort.sort(a); }, arrAleatorio));
        tiemposMergeSort.push_back(Timer::medirTiempo([&](vector<int>& a){ mergeSort.sort(a); }, arrAleatorio));

        // Medir tiempos para LinkedList y BST
        tiemposInsertLinkedList.push_back(Timer::medirTiempo([&](vector<int>& a){
            for (int val : a) { linkedList.insert(val); }
        }, arrAleatorio));

        tiemposInsertBST.push_back(Timer::medirTiempo([&](vector<int>& a){
            for (int val : a) { bst.insert(val); }
        }, arrAleatorio));
    }

    // Inicializar GTK
    GtkApplication *app;
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
