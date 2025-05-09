#include <random>
#include "Algorithm.h"

#ifndef PEA_ZAD3_GENETIC_H
#define PEA_ZAD3_GENETIC_H

#endif //PEA_ZAD3_GENETIC_H

using namespace std;

struct individual {
    vector<int> chromosome;
    int fitness;
};

class Genetic : public Algorithm {

    int timeLimit;
    int populationSize;
    float mutationRate;
    float crossoverRate;
    int mutationMethod;
    int crossoverMethod;
    std::mt19937 gen; // generator liczb losowych

    individual tournamentSelection(const std::vector<individual>& population, int tournamentSize);
    individual crossoverOX(const individual& parent1, const individual& parent2);
    individual crossoverERX(const individual& parent1, const individual& parent2);
    void mutationSwap(individual& ind);
    void mutationInsert(individual &ind);
    int calculateFitness(individual ind);
    void succession(std::vector<individual> &population, std::vector<individual> &subPopulation);

public:

    Genetic();
    void setTimeLimit(int time);
    void setPopulationSize(int popSize);
    void setMutationRate(float rate);
    void setCrossoverRate(float rate);
    void setMutationMethod(int method);
    void setCrossoverMethod(int method);
    void algorithm();

};