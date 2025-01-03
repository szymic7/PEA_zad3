#include "../headers/TabuSearch.h"
#include "../headers/Greedy.h"

#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

//--------------------------------------------------------------------------------------------------

TabuSearch::TabuSearch() : Algorithm(), gen(std::random_device{}())
{
    timeLimit = 0;
    noImprovementIterations = 0;

    neighborhoodDef = 1; // domyslna definicja sasiedztwa - Insert

    // Wartosci domyslne - dla ftv55.atsp
    iterationsWithoutImprovementLimit = 50;
    tabuCadency = 10;

    tabuList = nullptr;
}

//--------------------------------------------------------------------------------------------------

void TabuSearch::setN(int size) {
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

        // Czyszczenie listy tabu
        if (tabuList != nullptr) {
            for (int i = 0; i < n; i++) {
                delete[] tabuList[i];
            }
            delete[] tabuList;
        }
        tabuList = nullptr;
    }

    // Przypisanie nowej wartosci n
    n = size;
}

//--------------------------------------------------------------------------------------------------

void TabuSearch::setTimeLimit(int t) {
    timeLimit = t;
}

//--------------------------------------------------------------------------------------------------

void TabuSearch::setNeighborhoodDef(int nDef) {
    neighborhoodDef = nDef;
}

//---------------------------------------------------------------------------------------------

void TabuSearch::setTabuCadency(int cadency) {
    tabuCadency = cadency;
}

//---------------------------------------------------------------------------------------------

void TabuSearch::setIterationsLimit(int iterationsLimit) {
    iterationsWithoutImprovementLimit = iterationsLimit;
}

//---------------------------------------------------------------------------------------------

void TabuSearch::algorithm() {

    // Wyczyszczenie rozwiazania
    resultVector.clear();
    double timeOfResult = 0.0;
    noImprovementIterations = 0;

    // Wyczyszczenie listy tabu
    if (tabuList != nullptr) {
        for (int i = 0; i < n; ++i) {
            delete [] tabuList[i];
        }
        delete [] tabuList;
    }

    // Inicjalizacja nowej listy tabu
    tabuList = new int*[n];
    for (int i = 0; i < n; ++i) {
        tabuList[i] = new int[n];
        for(int j = 0; j < n; j++) {
            tabuList[i][j] = 0; // Poczatkowo tabu list wypelniona wartosciami 0
        }
    }

    // Rozwiazanie poczatkowe
    std::vector<int> currentSolution;
    Greedy greedy;
    greedy.setN(n);
    greedy.setCostMatrix(costMatrix);
    greedy.greedyAlgorithm();

    int* resultVertices = greedy.getResultVertices();
    currentSolution.resize(n);
    for (int i = 0; i < n; ++i) {
        currentSolution[i] = resultVertices[i];
    }
    delete [] resultVertices;

    std::vector<int> bestSolution = currentSolution;
    int bestCost = calculatePathCost(bestSolution);

    cout << bestCost << " " << timeOfResult << endl;

    // Wierzcholki i, j do dodania do listy tabu
    int bestI, bestJ;

    auto startTime = std::chrono::steady_clock::now();

    while (std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count() < timeLimit) {
        int currentCost = std::numeric_limits<int>::max();
        std::vector<int> bestNeighbor;

        switch (neighborhoodDef) {
            case 1: // insert
                bestNeighbor = findBestNeighborInsert(currentSolution, currentCost, bestI, bestJ);
                break;
            case 2: // swap
                bestNeighbor = findBestNeighborSwap(currentSolution, currentCost, bestI, bestJ);
                break;
            case 3: // invert
                bestNeighbor = findBestNeighborInvert(currentSolution, currentCost, bestI, bestJ);
                break;
            default:
                bestNeighbor = findBestNeighborInsert(currentSolution, currentCost, bestI, bestJ);
                cout << endl << "Ustawiono domyslna definicje sasiedztwa - Insert." << endl;
                break;
        }

        currentSolution = bestNeighbor;

        if (currentCost < bestCost) { // Nowe rozwiazanie jest dotychczas najlepszym
            timeOfResult = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count();
            cout << bestCost << " " << timeOfResult << endl;
            bestCost = currentCost;
            cout << bestCost << " " << timeOfResult << endl;
            bestSolution = bestNeighbor;
            noImprovementIterations = 0;
        } else {
            noImprovementIterations++;
        }

        // Aktualizacja listy tabu
        updateTabuList(bestI, bestJ);

        // Ewentualna dywersyfikacja
        if (noImprovementIterations >= iterationsWithoutImprovementLimit) {
            diversify(currentSolution);
        }

    }

    resultVector = bestSolution;

    cout << "Czas znalezienia najlepszego rozwiazania: " << timeOfResult << " sekund" << endl;
}

//---------------------------------------------------------------------------------------------

std::vector<int> TabuSearch::findBestNeighborInsert(const std::vector<int>& solution, int& bestCost, int& bestI, int& bestJ) {
    std::vector<int> bestNeighbor = solution;
    bestCost = std::numeric_limits<int>::max();
    std::vector<int> neighbor;

    for (int i = 0; i < solution.size(); i++) {
        for (int j = 0; j < solution.size(); j++) {
            if (i != j) {
                neighbor = solution;

                int temp = neighbor[i];
                neighbor.erase(neighbor.begin() + i);

                // Skoryguj pozycję `j`, jeśli jest większa niż `i`
                int adjustedJ = j;
                if (j > i) {
                    adjustedJ--; // Po usunięciu elementu, indeks `j` przesuwa się o 1 w lewo
                }

                neighbor.insert(neighbor.begin() + adjustedJ, temp);

                int cost = calculatePathCost(neighbor);
                if (tabuList[i][j] == 0 || cost < bestCost) {
                    if (cost < bestCost) {
                        bestCost = cost;
                        bestNeighbor = neighbor;
                        bestI = i;
                        bestJ = j;
                    }
                }
            }
        }
    }

    return bestNeighbor;
}

//---------------------------------------------------------------------------------------------

std::vector<int> TabuSearch::findBestNeighborSwap(const std::vector<int>& solution, int& bestCost, int& bestI, int& bestJ) {
    std::vector<int> bestNeighbor = solution;
    bestCost = std::numeric_limits<int>::max();
    std::vector<int> neighbor = solution;

    for (int i = 0; i < solution.size(); i++) {
        for (int j = i + 1; j < solution.size(); j++) {
            std::swap(neighbor[i], neighbor[j]);

            int cost = calculatePathCost(neighbor);
            if (tabuList[i][j] == 0 || cost < bestCost) {
                if (cost < bestCost) {
                    bestCost = cost;
                    bestNeighbor = neighbor;
                    bestI = i;
                    bestJ = j;
                }
            }

            // Przywrocenie stanu
            std::swap(neighbor[i], neighbor[j]);
        }
    }

    return bestNeighbor;
}

//---------------------------------------------------------------------------------------------

std::vector<int> TabuSearch::findBestNeighborInvert(const std::vector<int>& solution, int& bestCost, int& bestI, int& bestJ) {
    std::vector<int> bestNeighbor = solution;
    bestCost = std::numeric_limits<int>::max();
    std::vector<int> neighbor = solution;

    for (int i = 0; i < solution.size(); i++) {
        for (int j = i + 1; j < solution.size(); j++) {

            std::reverse(neighbor.begin() + i, neighbor.begin() + j + 1);

            int cost = calculatePathCost(neighbor);
            if (tabuList[i][j] == 0 || cost < bestCost) {
                if (cost < bestCost) {
                    bestCost = cost;
                    bestNeighbor = neighbor;
                    bestI = i;
                    bestJ = j;
                }
            }

            // Przywrocenie stanu
            std::reverse(neighbor.begin() + i, neighbor.begin() + j + 1);
        }
    }

    return bestNeighbor;
}

//---------------------------------------------------------------------------------------------

void TabuSearch::updateTabuList(/*const std::vector<int>& solution*/ int bestI, int bestJ) {

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (tabuList[i][j] > 0) tabuList[i][j]--;
        }
    }

    // Dodanie ruchu do listy tabu
    tabuList[bestI][bestJ] = tabuCadency;

    // Dla operacji Swap i Invert ruch (i, j) jest rownoznaczny ruchowi (j, i)
    if(neighborhoodDef == 2 || neighborhoodDef == 3) {
        tabuList[bestJ][bestI] = tabuCadency;
    }
}

//---------------------------------------------------------------------------------------------

void TabuSearch::diversify(std::vector<int>& currentSolution) {

    /*currentSolution.resize(n);
    std::iota(currentSolution.begin(), currentSolution.end(), 0);
    std::shuffle(currentSolution.begin(), currentSolution.end(), gen);
    noImprovementIterations = 0;*/

    int numPerturbations = n / 10;
    int i, j;
    for (int k = 0; k < numPerturbations; k++) {

        i = gen() % (n-1) + 1;
        do {
            j = gen() % (n-1) + 1;
        } while (i == j);
        std::swap(currentSolution[i], currentSolution[j]); // Zamiana dwóch losowych miast

    }
    noImprovementIterations = 0;

}

//---------------------------------------------------------------------------------------------

int TabuSearch::calculatePathCost(vector<int> path) {
    int cost = 0;

    for (int i = 0; i < path.size() - 1; i++) {
        cost += costMatrix[path[i]][path[i + 1]];
    }
    cost += costMatrix[path[n-1]][path[0]];

    return cost;
}

//---------------------------------------------------------------------------------------------

