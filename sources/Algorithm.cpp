#include "../headers/Algorithm.h"
#include <iostream>

using namespace std;

//----------------------------------------------------------------------------------

Algorithm::Algorithm() {
    n = 0;
    costMatrix = nullptr;
    result = INT_MAX;
    result_vertices = nullptr;
}

//----------------------------------------------------------------------------------

void Algorithm::setN(int size) {
    if(n > 0) {
        // Czyszczenie matrix przed przypisaniem nowej wartosci n
        if (costMatrix != nullptr) {
            for (int i = 0; i < n; i++) {
                delete[] costMatrix[i];
            }
            delete[] costMatrix;
        }

        // Czyszczenie tablicy z wynikowa sciezka
        if (result_vertices != nullptr) {
            delete [] result_vertices;
            result_vertices = nullptr;
        }
    }

    // Przypisanie nowej wartosci n
    n = size;
}

//----------------------------------------------------------------------------------

void Algorithm::setCostMatrix(int **matrix) {
    if(n > 0) {
        // Utworzenie nowej macierzy
        costMatrix = new int*[n];
        for (int i = 0; i < n; i++) {
            costMatrix[i] = new int[n];
            for (int j = 0; j < n; j++) {
                costMatrix[i][j] = matrix[i][j];
            }
        }
    }
}

//----------------------------------------------------------------------------------

int Algorithm::getResult() {
    return result;
}

//----------------------------------------------------------------------------------

int* Algorithm::getResultVertices() {
    return result_vertices;
}

//----------------------------------------------------------------------------------

void Algorithm::printResultVertices() {
    if(result_vertices != nullptr) {
        for (int i = 0; i < n; i++) {
            cout << result_vertices[i] << ", ";
        }
        cout << result_vertices[0] << endl;
    }
}

//----------------------------------------------------------------------------------

std::vector<int> Algorithm::getResultVector() {
    return resultVector;
}

//----------------------------------------------------------------------------------