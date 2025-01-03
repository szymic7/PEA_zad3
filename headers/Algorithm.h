#ifndef PEA_ZAD3_ALGORITHM_H
#define PEA_ZAD3_ALGORITHM_H

#include <vector>

class Algorithm {

protected:

    int n;
    int **costMatrix;
    int result;
    int* result_vertices;
    std::vector<int> resultVector;

public:

    Algorithm();
    void setN(int size);
    void setCostMatrix(int **matrix);
    int getResult();
    int* getResultVertices();
    void printResultVertices();
    std::vector<int> getResultVector();

};

#endif //PEA_ZAD3_ALGORITHM_H
