#ifndef PEA_ZAD3_TABUSEARCH_H
#define PEA_ZAD3_TABUSEARCH_H

#endif //PEA_ZAD3_TABUSEARCH_H

#include "Algorithm.h"
#include <iostream>
#include <vector>
#include <set>
#include <list>
#include <random>

using namespace std;

class TabuSearch : public Algorithm {

private:

    // Kryterium stopu
    int timeLimit;
    int iterationsWithoutImprovementLimit;

    // Definicja sasiedztwa; 1 - insert, 2 - swap, 3 - invert
    int neighborhoodDef;

    // Licznik iteracji bez poprawy rozwiazania
    int noImprovementIterations;

    // Lista tabu
    int** tabuList;

    // Kadencja dla rozwiazan na liscie tabu
    int tabuCadency;

    std::mt19937 gen;

    void updateTabuList(int bestI, int bestJ);
    void diversify(std::vector<int>& currentSolution);


    std::vector<int> findBestNeighborInsert(const std::vector<int>& solution, int& bestCost, int& bestI, int& bestJ);
    std::vector<int> findBestNeighborSwap(const std::vector<int>& solution, int& bestCost, int& bestI, int& bestJ);
    std::vector<int> findBestNeighborInvert(const std::vector<int>& solution, int& bestCost, int& bestI, int& bestJ);


public:

    TabuSearch();
    void setN(int size);
    void setTimeLimit(int t);
    void setNeighborhoodDef(int nDef);
    void setTabuCadency(int cadency);
    void setIterationsLimit(int iterationsLimit);
    void algorithm();
    int calculatePathCost(vector <int> path);

};