#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include "SortingAlgorithms.h"
#include "DataStructures.h"
#include "Timer.h"

using namespace std;

vector<int> generarArregloAleatorio(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 10000;
    }
    return arr;
}

vector<int> generarArregloOrdenado(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
    return arr;
}

vector<int> generarArregloInverso(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = n - i;
    }
    return arr;
}

int main() {
    vector<int> size = {100, 1000, 5000, 10000, 20000};

    BubbleSort<int> bubbleSort;
    SelectionSort<int> selectionSort;
    MergeSort<int> mergeSort;

    // Estructuras de datos
    LinkedList<int> linkedList;
    BST<int> bst;

    ofstream outputFile("resultados_gnuplot.dat");
    outputFile << "# Tamaño BubbleSort SelectionSort MergeSort InsertLinkedList SearchLinkedList InsertBST SearchBST\n";

    cout << "Iniciando pruebas...\n";

    for (int n : size) {
        vector<int> arrAleatorio = generarArregloAleatorio(n);

        // Medir tiempo para algoritmos de ordenamiento
        long long tiempoBubbleSort = Timer::medirTiempo([&](vector<int>& a){ bubbleSort.sort(a); }, arrAleatorio);
        cout << "BubbleSort para N=" << n << ": " << tiempoBubbleSort << " ns\n";

        long long tiempoSelectionSort = Timer::medirTiempo([&](vector<int>& a){ selectionSort.sort(a); }, arrAleatorio);
        cout << "SelectionSort para N=" << n << ": " << tiempoSelectionSort << " ns\n";

        long long tiempoMergeSort = Timer::medirTiempo([&](vector<int>& a){ mergeSort.sort(a); }, arrAleatorio);
        cout << "MergeSort para N=" << n << ": " << tiempoMergeSort << " ns\n";

        // Medir tiempo para inserción en LinkedList
        long long tiempoInsertLinkedList = Timer::medirTiempo([&](vector<int>& a){
            for (int val : a) {
                linkedList.insert(val);
            }
        }, arrAleatorio);
        cout << "InsertLinkedList para N=" << n << ": " << tiempoInsertLinkedList << " ns\n";

        // Medir tiempo para búsqueda en LinkedList
        long long tiempoSearchLinkedList = Timer::medirTiempo([&](vector<int>& a){
            for (int val : a) {
                linkedList.search(val);
            }
        }, arrAleatorio);
        cout << "SearchLinkedList para N=" << n << ": " << tiempoSearchLinkedList << " ns\n";

        // Medir tiempo para inserción en BST
        long long tiempoInsertBST = Timer::medirTiempo([&](vector<int>& a){
            for (int val : a) {
                bst.insert(val);
            }
        }, arrAleatorio);
        cout << "InsertBST para N=" << n << ": " << tiempoInsertBST << " ns\n";

        // Medir tiempo para búsqueda en BST
        long long tiempoSearchBST = Timer::medirTiempo([&](vector<int>& a){
            for (int val : a) {
                bst.search(val);
            }
        }, arrAleatorio);
        cout << "SearchBST para N=" << n << ": " << tiempoSearchBST << " ns\n";

        // Escribir los resultados en el archivo para graficar
        outputFile << n << " " << tiempoBubbleSort << " " << tiempoSelectionSort << " " << tiempoMergeSort << " "
                   << tiempoInsertLinkedList << " " << tiempoSearchLinkedList << " "
                   << tiempoInsertBST << " " << tiempoSearchBST << "\n";
    }

    outputFile.close();
    cout << "Archivo de resultados 'resultados_gnuplot.dat' generado correctamente.\n";

    // Generar la gráfica con las líneas teóricas
    system("gnuplot -e \"set terminal png; set encoding utf8; set output 'resultados.png'; "
           "set title 'Comparacion de Algoritmos y Estructuras de Datos'; "
           "set xlabel 'Tamano del arreglo (N)'; "
           "set ylabel 'Tiempo (ns)'; "
           "set ytics auto; "
           "set yrange [0:300000000]; "
           "set grid ytics; "
           "set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5; "
           "set style line 2 lc rgb '#dd181f' lt 1 lw 2 pt 5 ps 1.5; "
           "set style line 3 lc rgb '#00aa00' lt 1 lw 2 pt 9 ps 1.5; "
           "set style line 4 lc rgb '#ffa500' lt 1 lw 2 pt 4 ps 1.5; "
           "set style line 5 lc rgb '#800080' lt 1 lw 2 pt 5 ps 1.5; "
           "set style line 6 lc rgb '#a52a2a' lt 1 lw 2 pt 6 ps 1.5; "
           "set style line 7 lc rgb '#20b2aa' lt 1 lw 2 pt 7 ps 1.5; "
           "set style line 8 lc rgb '#000000' lt 1 lw 2 pt 4 ps 1.5; "
           "set style line 9 lc rgb '#555555' lt 1 lw 2 pt 4 ps 1.5; "
           "plot 'resultados_gnuplot.dat' using 1:2 with lines title 'BubbleSort' ls 1, "
           "'resultados_gnuplot.dat' using 1:3 with lines title 'SelectionSort' ls 2, "
           "'resultados_gnuplot.dat' using 1:4 with lines title 'MergeSort' ls 3, "
           "'resultados_gnuplot.dat' using 1:5 with lines title 'InsertLinkedList' ls 4, "
           "'resultados_gnuplot.dat' using 1:6 with lines title 'SearchLinkedList' ls 5, "
           "'resultados_gnuplot.dat' using 1:7 with lines title 'InsertBST' ls 6, "
           "'resultados_gnuplot.dat' using 1:8 with lines title 'SearchBST' ls 7, "
           "x*x title 'Teorico O(n^2)' with lines ls 8, "
           "x*log(x) title 'Teorico O(n log n)' with lines ls 9;\"");

    cout << "Grafica generada como 'resultados.png'.\n";

    return 0;
}
