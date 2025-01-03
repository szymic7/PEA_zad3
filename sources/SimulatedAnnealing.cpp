#include "../headers/SimulatedAnnealing.h"
#include "../headers/Greedy.h"
#include <vector>
#include <random>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <iostream>

using namespace std;

//--------------------------------------------------------------------------------------------------

SimulatedAnnealing::SimulatedAnnealing() : Algorithm(), gen(std::random_device{}()) {
    timeLimit = 0;
    alfa = 0.99;
    //Lk = 100;
}

//--------------------------------------------------------------------------------------------------

void SimulatedAnnealing::setTimeLimit(int time) {
    timeLimit = time;
}

//--------------------------------------------------------------------------------------------------

void SimulatedAnnealing::setAlfa(float a) {
    alfa = a;
}

//--------------------------------------------------------------------------------------------------

void SimulatedAnnealing::algorithm() {

    // Wyczyszczenie rozwiazania
    resultVector.clear();
    double timeOfResult = 0.0;

    // Rozwiazanie poczatkowe - wygenerowane losowo
    std::vector<int> currentSolution;
    currentSolution.resize(n);
    std::iota(currentSolution.begin(), currentSolution.end(), 0);
    std::shuffle(currentSolution.begin(), currentSolution.end(), gen);

    std::vector<int> bestSolution = currentSolution;
    int bestCost = calculatePathCost(bestSolution);
    int currentCost = bestCost;

    // Wartosc poczatkowa T0 jest wartoscia najdrozszej krawedzi w macierzy kosztow
    int maxCost = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j) {
                maxCost = std::max(maxCost, costMatrix[i][j]);
            }
        }
    }
    double temperature = maxCost;

    auto startTime = std::chrono::steady_clock::now();

    Lk = n * n;

    while (std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count() < timeLimit && temperature > 1e-8) {

        for (int l = 0; l < Lk; l++) {
            std::vector<int> neighbor;

            // neighbor - znalezienie rozwiazania w sasiedztwie currentSolution
            neighbor = getRandomNeighbor(currentSolution, gen);
            int neighborCost = calculatePathCost(neighbor);

            // Wyznaczenie prawdopodobienstwa zaakceptowania rozwiazania
            // Funkcja celu: delta_E = x_k - x_a
            double delta = neighborCost - currentCost;
            double acceptanceProbability = delta < 0 ? 1.0 : std::exp(-delta / temperature);

            // Akceptacja nowego rozwiazania, jesli neighborCost < currentCost lub na podstawie prawdopodobienstwa
            if (std::generate_canonical<double, 10>(gen) < acceptanceProbability) {
                currentSolution = neighbor;
                currentCost = neighborCost;
            }

            // Aktualizacja najlepszego rozwiazania
            if (currentCost < bestCost) {
                timeOfResult = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count();
                bestSolution = currentSolution;
                cout << bestCost << " " << timeOfResult << endl;
                bestCost = currentCost;
                cout << bestCost << " " << timeOfResult << endl;
            }
        }

        // Aktualizacja parametru kontrolnego T
        temperature *= alfa;

    }

    cout << endl << "Wartosc koncowa parametru kontrolnego Tk: " << temperature << endl;
    cout << "Wartosc wyrazenia exp(-1/Tk): " << std::exp(-1 / temperature) << endl;
    cout << "Czas znalezienia najlepszego rozwiazania: " << timeOfResult << " sekund" << endl;

    resultVector = bestSolution;
}

//--------------------------------------------------------------------------------------------------

std::vector<int> SimulatedAnnealing::getRandomNeighbor(const std::vector<int>& solution, std::mt19937& gen) {

    // Rozklad rownomierny liczb z zakresu [0; size-1]
    std::uniform_int_distribution<> dist(0, solution.size() - 1);
    int i = dist(gen);
    int j = dist(gen);
    while (i == j) {
        j = dist(gen);
    }

    std::vector<int> neighbor = solution;
    int temp = neighbor[i];
    neighbor.erase(neighbor.begin() + i);
    neighbor.insert(neighbor.begin() + j, temp);

    return neighbor;
}

//--------------------------------------------------------------------------------------------------

int SimulatedAnnealing::calculatePathCost(const std::vector<int>& path) {
    int cost = 0;

    for (int i = 0; i < path.size() - 1; i++) {
        cost += costMatrix[path[i]][path[i + 1]];
    }
    cost += costMatrix[path[n - 1]][path[0]];

    return cost;
}

//--------------------------------------------------------------------------------------------------