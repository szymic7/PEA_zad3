#include "../headers/Genetic.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <unordered_map>

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

//--------------------------------------------------------------------------------------------------

void Genetic::setTimeLimit(int time) {
    timeLimit = time;
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

    // 1) Wyznaczenie sekcji dopasowania w rodzicach parent1 i parent2
    std::uniform_int_distribution<> dist(0, n - 1);
    int start, end;
    do {
        start = dist(gen);
        end = dist(gen);
    } while(start == end);
    if (start > end) std::swap(start, end);

    // 2) Kopiowanie sekcji dopasowania z rodzica parent1
    for (int i = start; i <= end; i++) {
        child.chromosome[i] = parent1.chromosome[i];
    }

    // 3) Wypelnienie brakujacych genow z rodzica parent2 w odpowiedniej kolejnosci, z pominieciem duplikatow
    int currentIndex = (end + 1) % n;
    for (int i = 0; i < n; i++) {
        int gene = parent2.chromosome[i];

        // Jesli gen nie wystepuje w chromosomie dziecka
        if (std::find(child.chromosome.begin(), child.chromosome.end(), gene) == child.chromosome.end()) {
            child.chromosome[currentIndex] = gene;
            currentIndex = (currentIndex + 1) % n;
        }
    }

    return child;
}

//-------------------------------------------------------------------------------------

individual Genetic::crossoverPMX(const individual& parent1, const individual& parent2, individual& c1, individual& c2) {

    individual child1, child2;
    child1.chromosome.resize(n, -1); // Initialize child chromosome with -1
    child2.chromosome.resize(n, -1); // Initialize child chromosome with -1

    std::vector<int> map1, map2;
    map1.resize(n, -1);
    map2.resize(n, -1);

    // 1) Wyznaczenie sekcji dopasowania w rodzicach parent1 i parent2
    std::uniform_int_distribution<> dist(0, n - 1);
    int start, end, temp;
    do {
        start = dist(gen);
        end = dist(gen);
    } while (start == end);
    if (start > end) std::swap(start, end);

    // 2) Kopiowanie sekcji dopasowania z rodzica parent1
    for (int i = start; i <= end; i++) {
        child1.chromosome[i] = parent1.chromosome[i];
        child2.chromosome[i] = parent2.chromosome[i];
        map1[parent1.chromosome[i]] = parent2.chromosome[i];
        map2[parent2.chromosome[i]] = parent1.chromosome[i];
    }

    for (int i = 0; i < n; i++) {
        cout << "Wchodzimy do 1. petli for - wstawianie genow dla child1." << endl;
        if (child1.chromosome[i] == -1) {
            // Jesli gen z chromosomu parent2 nie wystepuje w chromosomie dziecka child1
            if (std::find(child1.chromosome.begin() + start, child1.chromosome.begin() + end + 1, parent2.chromosome[i]) == child1.chromosome.begin() + end + 1)
                child1.chromosome[i] = parent2.chromosome[i];
            else {
                temp = parent2.chromosome[i];

                do {
                    temp = map1[temp];
                } while (std::find(child1.chromosome.begin() + start, child1.chromosome.begin() + end + 1, temp) != child1.chromosome.begin() + end + 1);

                child1.chromosome[i] = temp;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        cout << "Wchodzimy do 2. petli for - wstawianie genow dla child2." << endl;
        if (child2.chromosome[i] == -1) {
            // Jesli gen z chromosomu parent1 nie wystepuje w chromosomie dziecka child2
            if (std::find(child2.chromosome.begin() + start, child2.chromosome.begin() + end + 1, parent1.chromosome[i]) == child2.chromosome.begin() + end + 1)
                child2.chromosome[i] = parent1.chromosome[i];
            else {
                temp = parent1.chromosome[i];

                do {
                    temp = map2[temp];
                } while (std::find(child2.chromosome.begin() + start, child2.chromosome.begin() + end + 1, temp) != child2.chromosome.begin() + end + 1);

                child2.chromosome[i] = temp;
            }
        }
    }

    cout << "Przed przypisaniem" << endl;

    //child1.fitness = calculateFitness(child1);
    //child2.fitness = calculateFitness(child2);

    c1 = child1;
    c2 = child2;

    cout << "Po przypisaniu" << endl;

    map1.clear();
    map2.clear();

    // 3) Create a mapping table for the subsegment
//    std::unordered_map<int, int> mapping;
//    for (int i = start; i <= end; i++) {
//        mapping[parent1.chromosome[i]] = parent2.chromosome[i];
//    }
//
//    // 4) Fill remaining positions using parent2 and mapping
//    for (int i = 0; i < n; i++) {
//        // Skip positions already filled
//        if (i >= start && i <= end) continue;
//
//        int gene = parent2.chromosome[i];
//        // Resolve conflicts using the mapping
//        while (mapping.count(gene)) {
//            gene = mapping[gene];
//        }
//        child.chromosome[i] = gene;
//    }

    /*
    // 3) Mapowanie pozostalych genow (z parent2), aby potomek nie posiadal powtarzajacych sie genow
    for (int i = start; i <= end; i++) {
        int geneFromParent2 = parent2.chromosome[i];
        //if (std::find(child.chromosome.begin() + start, child.chromosome.begin() + end + 1, geneFromParent2) == child.chromosome.begin() + end + 1) {

        // Jesli potomek nie posiada tego genu
        if (std::find(child.chromosome.begin(), child.chromosome.end(), geneFromParent2) == child.chromosome.end()) {

            int currentGene = geneFromParent2;
            while (true) {
                auto it = std::find(parent1.chromosome.begin() + start, parent1.chromosome.begin() + end + 1, currentGene);
                if (it == parent1.chromosome.begin() + end + 1) break; // Gen nie wystepuje w sekcji dopasowania
                int mappedIndex = std::distance(parent1.chromosome.begin(), it);
                currentGene = parent2.chromosome[mappedIndex];
            }

            // Place the mapped gene in the correct position in the child
            for (int j = 0; j < n; j++) {
                if (child.chromosome[j] == -1) { // Find the first empty slot
                    child.chromosome[j] = currentGene;
                    break;
                }
            }

            // Jesli gen nie wystepuje w sekcji dopasowania, mapujemy go
//            int mappedGene = geneFromParent2;
//            int position = i;
//
//            // Szukamy odpowiadajacej pozycji w parent1
//            while (std::find(child.chromosome.begin() + start, child.chromosome.begin() + end + 1, mappedGene) != child.chromosome.begin() + end + 1) {
//                position = std::distance(parent2.chromosome.begin(),
//                                         std::find(parent1.chromosome.begin(), parent1.chromosome.end(), mappedGene));
//                mappedGene = parent2.chromosome[position];
//            }
//
//            // Wstawienie genu na odpowieniej pozycji w chromosomie potomka
//            child.chromosome[position] = geneFromParent2;
        }
    }

    // 4) Uzupelnienie brakujacych pozycji genami z parent2
    for (int i = 0; i < n; i++) {
        if (child.chromosome[i] == -1) {
            child.chromosome[i] = parent2.chromosome[i];
        }
    }*/

    //return child;
}

//-------------------------------------------------------------------------------------

void Genetic::mutationSwap(individual &ind) {
    std::uniform_int_distribution<> dist(0, n - 1);
    int pos1, pos2;
    do {
        pos1 = dist(gen);
        pos2 = dist(gen);
    } while (pos1 == pos2);

    std::swap(ind.chromosome[pos1], ind.chromosome[pos2]);
}

//-------------------------------------------------------------------------------------

void Genetic::mutationInversion(individual &ind) {
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

    for(int i = 0; i < ind.chromosome.size() - 1; i++) {
        cost += costMatrix[ind.chromosome[i]][ind.chromosome[i + 1]];
    }
    cost += costMatrix[ind.chromosome[n - 1]][ind.chromosome[0]];

    return cost;
}

//-------------------------------------------------------------------------------------

void Genetic::algorithm() {

    vector<individual> population;
    population.reserve(populationSize);

    // Tworzenie populacji poczatkowej

    // Pierwszy osobnik - zapisany jako bestIndividual
    individual i1;

    // Losowy uklad genow (miast)
    i1.chromosome.resize(n);
    std::iota(i1.chromosome.begin(), i1.chromosome.end(), 0);
    std::shuffle(i1.chromosome.begin(), i1.chromosome.end(), gen);

    // Wyznaczenie wartosci funkcji przystosowania (kosztu sciezki)
    i1.fitness = calculateFitness(i1);

    // Dodanie osobnika do polulacji
    population.push_back(i1);

    individual bestIndividual = i1;

    for(int i = 1; i < populationSize; i++) {
        individual ind;

        // Losowy uklad genow (miast)
        ind.chromosome.resize(n);
        std::iota(ind.chromosome.begin(), ind.chromosome.end(), 0);
        std::shuffle(ind.chromosome.begin(), ind.chromosome.end(), gen);

        // Wyznaczenie wartosci funkcji przystosowania (kosztu sciezki)
        ind.fitness = calculateFitness(ind);

        if(ind.fitness < bestIndividual.fitness) {
            bestIndividual = ind;
        }

        // Dodanie osobnika do polulacji
        population.push_back(ind);
    }

    //individual bestIndividual = population[0];

    auto startTime = std::chrono::steady_clock::now();

    cout << endl << "Wchodzimy do petli. Na razie wszystko ok." << endl;

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
                    //offspring1 = crossoverMethod1(parent1, parent2);
                    //offspring2 = crossoverMethod1(parent2, parent1);
                    offspring1 = crossoverOX(parent1, parent2);
                    offspring2 = crossoverOX(parent2, parent1);
                } else {
                    crossoverPMX(parent1, parent2, offspring1, offspring2);
                    //offspring1 = crossoverPMX(parent1, parent2);
                    //offspring2 = crossoverPMX(parent2, parent1);
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
                    mutationSwap(offspring1);
                else
                    mutationInversion(offspring1);
            }

            float mutationRoll2 = floatDist(gen);
            if (mutationRoll2 < mutationRate) {
                if (mutationMethod == 1)
                    mutationSwap(offspring2);
                else
                    mutationInversion(offspring2);
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