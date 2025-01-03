#include "../headers/Greedy.h"
#include <climits>

//----------------------------------------------------------------

Greedy::Greedy() : Algorithm() {
    path = nullptr;
    cost = INT_MAX;
}

//----------------------------------------------------------------

void Greedy::greedyAlgorithm() {
    if (result_vertices != nullptr) {
        delete[] result_vertices;
        result_vertices = nullptr;
    }
    result_vertices = new int[n + 1];
    result = INT_MAX;
    cost = INT_MAX;
    path = nullptr;

    // Dla kazdego wierzcholka jako wierzcholka poczatkowego
    for (int i = 0; i < n; ++i) {

        // Wyznaczenie sciezki dla wierzcholka poczatkowego i
        calculatePath(i);

        // Aktualizacja rozwiazania, jesli jest aktualnie najlepsze
        if (cost < result) {
            result = cost;
            for(int j = 0; j < n + 1; j++)
                result_vertices[j] = path[j];
        }

        // Wyczyszczenie pamieci dla zapisanego rozwiazania
        delete [] path;

    }

}

//----------------------------------------------------------------

void Greedy::calculatePath(int start) {

    int* visited = new int [n];
    for (int i = 0; i < n; i++) {
        visited[i] = false;
    }
    visited[start] = true;

    path = new int [n + 1];
    cost = 0;

    int currentVertex = start;
    path[0] = start;

    for (int i = 1; i < n; ++i) {
        int minCost = INT_MAX;
        int nextVertex = -1;

        // Przeszukanie wiersza w poszukiwaniu najlepszej krawedzi
        for (int j = 0; j < n; ++j) {
            if (!visited[j] && costMatrix[currentVertex][j] < minCost) {
                minCost = costMatrix[currentVertex][j];
                nextVertex = j;
            }
        }

        // Aktualizacja sciezki i kosztu
        if (nextVertex != -1) {
            path[i] = nextVertex;
            visited[nextVertex] = true;
            cost += minCost;
            currentVertex = nextVertex;
        }
    }

    // Krawedz powrotna do wierzcholka startowego
    cost += costMatrix[currentVertex][start];
    path[n] = start;

    // Wyczyszczenie tablicy sledzacej odwiedzone wierzcholki
    delete [] visited;

}

//----------------------------------------------------------------