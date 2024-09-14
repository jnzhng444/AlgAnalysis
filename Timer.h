#ifndef TIMER_H
#define TIMER_H

#include <vector>
#include <chrono>
using namespace std;
using namespace std::chrono;

class Timer {
public:
    template<typename Func, typename T>
    static long long medirTiempo(Func f, vector<T>& arr) {
        auto start = high_resolution_clock::now();
        f(arr); // Ejecuta la función
        auto end = high_resolution_clock::now();
        return duration_cast<nanoseconds>(end - start).count();
    }
};

#endif
