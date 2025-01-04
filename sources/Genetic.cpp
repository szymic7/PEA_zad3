#include "../headers/Genetic.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>

using namespace std;

//-------------------------------------------------------------------------------------

Genetic::Genetic() : Algorithm(), gen(std::random_device{}()) {

    timeLimit = 0;

    populationSize = 300; // wartosc domyslna
    mutationRate = 0.1; // wartosc domyslna
    crossoverRate = 0.9; // wartosc domyslna

    mutationMethod = 1; // wartosc domyslna
    crossoverMethod = 1; // wartosc domyslna
}

//-------------------------------------------------------------------------------------

void Genetic::setPopulationSize(int popSize) {
    populationSize = popSize;
}

//-------------------------------------------------------------------------------------

void Genetic::setMutationRate(float rate) {
    mutationRate = rate;
}

//-------------------------------------------------------------------------------------

void Genetic::setCrossoverRate(float rate) {
    crossoverRate = rate;
}

//-------------------------------------------------------------------------------------

void Genetic::setMutationMethod(int method) {
    mutationMethod = method;
}

//-------------------------------------------------------------------------------------

void Genetic::setCrossoverMethod(int method) {
    crossoverMethod = method;
}

//-------------------------------------------------------------------------------------

individual Genetic::tournamentSelection(const std::vector<individual> &population, int tournamentSize) {
    std::uniform_int_distribution<> dist(0, population.size() - 1);

    // Pick the first random individual as 'best' initially
    individual best = population[dist(gen)];

    // Compare against 'tournamentSize - 1' more random picks
    for(int i = 1; i < tournamentSize; i++) {
        const individual &competitor = population[dist(gen)];
        if (competitor.fitness < best.fitness) {
            best = competitor;
        }
    }
    return best;
}

//-------------------------------------------------------------------------------------

individual Genetic::crossoverMethod1(const individual& parent1, const individual& parent2) {
    individual child;
    child.chromosome.resize(n);

    // e.g., pick a random cut between 0 and n-1
    std::uniform_int_distribution<> cutDist(0, n - 1);
    int cut = cutDist(gen);

    // 1) copy from p1 up to 'cut'
    for (int i = 0; i <= cut; i++) {
        child.chromosome[i] = parent1.chromosome[i];
    }

    // 2) fill from p2 (skipping duplicates)
    int index = cut + 1;
    for (int i = 0; i < n; i++) {
        int gene = parent2.chromosome[i];
        // check if gene is already in child
        if (std::find(child.chromosome.begin(), child.chromosome.begin() + (cut + 1), gene)
            == child.chromosome.begin() + (cut + 1))
        {
            child.chromosome[index] = gene;
            index++;
            if (index >= n) break;
        }
    }

    return child;
}

//-------------------------------------------------------------------------------------

individual Genetic::crossoverOX(const individual& parent1, const individual& parent2) {

    individual child;
    child.chromosome.resize(n, -1);

    std::uniform_int_distribution<> dist(0, n - 1);
    int start, end;
    do {
        start = dist(gen);
        end = dist(gen);
    } while(start == end);
    if (start > end) std::swap(start, end);

    // 1) Copy subrange from parent1
    for (int i = start; i <= end; i++) {
        child.chromosome[i] = parent1.chromosome[i];
    }

    // 2) Fill remaining from parent2 in order, skipping duplicates
    int currentIndex = (end + 1) % n;
    for (int i = 0; i < n; i++) {
        int gene = parent2.chromosome[i];
        // if gene not already in child
        if (std::find(child.chromosome.begin(), child.chromosome.end(), gene) == child.chromosome.end()) {
            child.chromosome[currentIndex] = gene;
            currentIndex = (currentIndex + 1) % n;
        }
    }

    return child;
}

//-------------------------------------------------------------------------------------

void Genetic::mutationMethod1(individual &ind) {
    std::uniform_int_distribution<> dist(0, n - 1);
    int pos1, pos2;
    do {
        pos1 = dist(gen);
        pos2 = dist(gen);
    } while (pos1 == pos2);

    std::swap(ind.chromosome[pos1], ind.chromosome[pos2]);
}

//-------------------------------------------------------------------------------------

void Genetic::mutationMethod2(individual &ind) {
    std::uniform_int_distribution<> dist(0, n - 1);
    int start, end;
    do {
        start = dist(gen);
        end = dist(gen);
    } while (start == end);
    if (start > end) std::swap(start, end);

    std::reverse(ind.chromosome.begin() + start, ind.chromosome.begin() + end + 1);
}

//-------------------------------------------------------------------------------------

int Genetic::calculateFitness(individual ind) {
    int cost = 0;

    for(int i = 0; i < ind.chromosome.size(); i++) {
        cost += costMatrix[ind.chromosome[i]][ind.chromosome[i + 1]];
    }
    cost += costMatrix[ind.chromosome[n - 1]][ind.chromosome[0]];

    return cost;
}

//-------------------------------------------------------------------------------------

void Genetic::algorithm() {

    vector<individual> population;
    population.resize(populationSize);

    // Tworzenie populacji poczatkowej
    for(int i = 0; i < populationSize; i++) {
        individual ind;

        // Losowy uklad genow (miast)
        ind.chromosome.resize(n);
        std::iota(ind.chromosome.begin(), ind.chromosome.end(), 0);
        std::shuffle(ind.chromosome.begin(), ind.chromosome.end(), gen);

        // Wyznaczenie wartosci funkcji przystosowania (kosztu sciezki)
        ind.fitness = calculateFitness(ind);

        // Dodanie osobnika do polulacji
        population.push_back(ind);
    }

    individual bestIndividual = population[0];

    auto startTime = std::chrono::steady_clock::now();

    // Glowna petla
    while (std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count() < timeLimit) {

        vector<individual> newPopulation;
        newPopulation.reserve(populationSize);

        // 2a) Create new population
        while (static_cast<int>(newPopulation.size()) < populationSize)
        {
            // --- Selection ---
            individual parent1 = tournamentSelection(population, 5); // e.g. tournament size = 5
            individual parent2 = tournamentSelection(population, 5);

            individual offspring1, offspring2;

            // --- Crossover ---
            // Generate random float in [0,1)
            uniform_real_distribution<float> floatDist(0.0f, 1.0f);
            float crossoverRoll = floatDist(gen);

            if (crossoverRoll < crossoverRate) {
                // Perform chosen crossover method
                if (crossoverMethod == 1) {
                    offspring1 = crossoverMethod1(parent1, parent2);
                    offspring2 = crossoverMethod1(parent2, parent1);
                } else {
                    offspring1 = crossoverOX(parent1, parent2);
                    offspring2 = crossoverOX(parent2, parent1);
                }
            }
            else {
                // No crossover; offspring = exact copies
                offspring1 = parent1;
                offspring2 = parent2;
            }

            // --- Mutation ---
            float mutationRoll1 = floatDist(gen);
            if (mutationRoll1 < mutationRate) {
                if (mutationMethod == 1)
                    mutationMethod1(offspring1);
                else
                    mutationMethod2(offspring1);
            }

            float mutationRoll2 = floatDist(gen);
            if (mutationRoll2 < mutationRate) {
                if (mutationMethod == 1)
                    mutationMethod1(offspring2);
                else
                    mutationMethod2(offspring2);
            }

            // Recalculate fitness after crossover/mutation
            offspring1.fitness = calculateFitness(offspring1);
            offspring2.fitness = calculateFitness(offspring2);

            // Add to new population
            newPopulation.push_back(offspring1);
            if (static_cast<int>(newPopulation.size()) < populationSize) {
                newPopulation.push_back(offspring2);
            }
        }

        // 2b) Replace old population
        population = newPopulation;

        // 2c) Track the best individual in the new population
        for (auto &ind : population) {
            if (ind.fitness < bestIndividual.fitness) {
                bestIndividual = ind;
            }
        }

        // (Optional) Print or log current best
        // std::cout << "Current best fitness = " << bestIndividual.fitness << std::endl;

    }

    // Algorithm finished - 'bestIndividual' holds the best found solution
    cout << "Koszt znalezionego rozwiazania: " << bestIndividual.fitness << endl;
    cout << "Znalezione rozwiazanie: ";
    for (auto city : bestIndividual.chromosome) {
         cout << city << ", ";
    }
    cout << bestIndividual.chromosome[0] << endl;

}

//-------------------------------------------------------------------------------------