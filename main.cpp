#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace std::chrono;

struct Carta {
    int valor;
    int palo;
};


void shuffleDeck(vector<Carta>& deck) {
    random_device rd;
    mt19937 g(rd());
    shuffle(deck.begin(), deck.end(), g);
}

// Merge Sort secuencial
void merge(vector<Carta>& deck, int inicio, int medio, int fin) {
    vector<Carta> temp(fin - inicio + 1);
    int i = inicio, j = medio + 1, k = 0;

    while (i <= medio && j <= fin) {
        if (deck[i].palo < deck[j].palo ||
            (deck[i].palo == deck[j].palo && deck[i].valor <= deck[j].valor)) {
            temp[k++] = deck[i++];
        } else {
            temp[k++] = deck[j++];
        }
    }

    while (i <= medio) {
        temp[k++] = deck[i++];
    }
    while (j <= fin) {
        temp[k++] = deck[j++];
    }

    for (int i = 0; i < temp.size(); ++i) {
        deck[inicio + i] = temp[i];
    }
}

void mergeSort(vector<Carta>& deck, int inicio, int fin) {
    if (inicio >= fin) return;
    int mid = inicio + (fin - inicio) / 2;
    mergeSort(deck, inicio, mid);
    mergeSort(deck, mid + 1, fin);
    merge(deck, inicio, mid, fin);
}

// Merge Sort paralelo
void parallelMergeSort(vector<Carta>& deck, int inicio, int fin, int numHilos) {
    if (numHilos <= 1 || (fin - inicio) <= 50000) {
        mergeSort(deck, inicio, fin);
        return;
    }

    int mid = inicio + (fin - inicio) / 2;
    thread hiloIzq(parallelMergeSort, ref(deck), inicio, mid, numHilos / 2);
    thread hiloDer(parallelMergeSort, ref(deck), mid + 1, fin, numHilos / 2);
    hiloIzq.join();
    hiloDer.join();
    merge(deck, inicio, mid, fin);
}


void crearMazo(vector<Carta>& deck, int numBarajas) {
    deck.clear();
    deck.reserve(numBarajas * 54);
    for (int i = 0; i < numBarajas; ++i) {
        for (int palo = 0; palo < 4; ++palo) {
            for (int valor = 1; valor <= 13; ++valor) {
                deck.push_back({valor, palo});
            }
        }
        deck.push_back({0, 4});
        deck.push_back({0, 4});
    }
}


double calcularSpeedup(double tiempoSecuencial, double tiempoParalelo) {
    return tiempoSecuencial / tiempoParalelo;
}

double calcularEficiencia(double speedup, int numHilos) {
    return (speedup / numHilos) * 100;
}

double calcularThroughput(size_t numTareas, double tiempoTotal) {
    return numTareas / tiempoTotal;
}

double calcularLatencia(double tiempoTotal, size_t numTareas) {
    return tiempoTotal / numTareas;
}

double calcular_OverallSpeedUp(double P, double S) {
    return 1 / ((1 - P) + (P / S));
}

int main() {
    vector<Carta> deck;
    int numBarajas = 4000000 / 54;
    int numHilos = thread::hardware_concurrency();
    if (numHilos == 0) numHilos = 2;
    cout << "hilos disponibles: " << numHilos << endl;
    // secuencial
    crearMazo(deck, numBarajas);
    shuffleDeck(deck); // Barajado sin medir tiempo
    cout << "Mazo barajeado (secuencial)" << endl;
    auto inicioSeq = high_resolution_clock::now();
    mergeSort(deck, 0, deck.size() - 1);
    auto finSeq = high_resolution_clock::now();
    double tiempoSecuencial = duration_cast<milliseconds>(finSeq - inicioSeq).count();

    // paralela
    crearMazo(deck, numBarajas);
    shuffleDeck(deck); // Barajado sin medir tiempo
    cout << "Mazo barajeado (paralelo)" << endl;
    auto inicioParalelo = high_resolution_clock::now();
    parallelMergeSort(deck, 0, deck.size() - 1, numHilos);
    auto finParalelo = high_resolution_clock::now();
    double tiempoParalelo = duration_cast<milliseconds>(finParalelo - inicioParalelo).count();

    double speedup = calcularSpeedup(tiempoSecuencial, tiempoParalelo);
    double eficiencia = calcularEficiencia(speedup, numHilos);
    double throughput = calcularThroughput(deck.size(), tiempoParalelo);
    double latencia = calcularLatencia(tiempoParalelo, deck.size());

    double P = 0.95;
    double OverallSpeedUp = calcular_OverallSpeedUp(P, numHilos);

    cout << "\n--- tiempo de ordenamiento ---" << endl;
    cout << "Tiempo Secuencial: " << tiempoSecuencial << " ms" << endl;
    cout << "Tiempo Paralelo: " << tiempoParalelo << " ms" << endl;
    cout << "Speedup: " << speedup << endl;
    cout << "Eficiencia: " << eficiencia << " %" << endl;
    cout << "Throughput: " << throughput << " cartas/ms" << endl;
    cout << "Latencia: " << latencia << " ms/carta" << endl;
    cout << "OverallSpeedUp " << OverallSpeedUp << endl;

    return 0;
}
