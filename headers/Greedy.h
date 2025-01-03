#ifndef PEA_ZAD3_GREEDY_H
#define PEA_ZAD3_GREEDY_H

#endif //PEA_ZAD3_GREEDY_H

#include "Algorithm.h"

class Greedy : public Algorithm {

    int* path;  // wyznaczona sciezka dla i jako wierzcholka poczatkowego
    int cost;   // koszt rozwiazania dla i jako wierzcholka poczatkowego

public:
    Greedy();
    void greedyAlgorithm();
    void calculatePath(int start);

};
