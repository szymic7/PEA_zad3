#ifndef PEA_ZAD3_SIMULATEDANNEALING_H
#define PEA_ZAD3_SIMULATEDANNEALING_H

#endif //PEA_ZAD3_SIMULATEDANNEALING_H

#include <random>
#include "Algorithm.h"

class SimulatedAnnealing : public Algorithm {
    int timeLimit;
    float alfa; // Wspolczynnik zmiany temperatury
    int Lk; // Limit iteracji dla danej wartosci temperatury

    std::mt19937 gen;

    std::vector<int> getRandomNeighbor(const std::vector<int>& solution, std::mt19937& gen);

public:
    SimulatedAnnealing();
    void setAlfa(float a);
    void setTimeLimit(int time);
    void algorithm();
    int calculatePathCost(const std::vector<int>& path);
};
