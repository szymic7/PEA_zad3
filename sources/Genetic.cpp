#include "../headers/Genetic.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <cassert>

using namespace std;

//-------------------------------------------------------------------------------------

Genetic::Genetic() : Algorithm(), gen(std::random_device{}()) {

    timeLimit = 0;

    populationSize = 1000; // wartosc domyslna
    mutationRate = 0.01; // wartosc domyslna
    crossoverRate = 0.8; // wartosc domyslna

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

    // Wybranie losowego uczestnikow turnieju jako poczatkowego
    individual best = population[dist(gen)];

    // Porownanie z 'tournamentSize - 1' osobnikami, losowo wybranymi z populacji
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

void Genetic::crossoverPMX(const individual& parent1, const individual& parent2, individual& c1, individual& c2) {

    struct individual child1, child2;
    child1.chromosome.assign(n, -1);
    child2.chromosome.assign(n, -1);
    std::vector<int> map1(n, -1), map2(n, -1);


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
        //cout << "Wchodzimy do 1. petli for - wstawianie genow dla child1." << endl;
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
        //cout << "Wchodzimy do 2. petli for - wstawianie genow dla child2." << endl;
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

    c1 = child1;
    c2 = child2;

    //map1.clear();
    //map2.clear();

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

individual Genetic::crossoverERX(const individual& parent1, const individual& parent2) {
    individual child;
    child.chromosome.resize(n, -1); // Initialize child chromosome with -1

    // 1) Utworzenie listy sasiedztwa dla wszystkich wierzcholkow
    std::vector<std::vector<int>> edgeTable(n);

    // Dodanie sasiadow z parent1 do listy
    for (int i = 0; i < n; i++) {
        int prev = (i == 0) ? parent1.chromosome[n - 1] : parent1.chromosome[i - 1];
        int next = (i == n - 1) ? parent1.chromosome[0] : parent1.chromosome[i + 1];
        edgeTable[parent1.chromosome[i]].push_back(prev);
        edgeTable[parent1.chromosome[i]].push_back(next);
    }

    // Dodanie sasiadow z parent2 do listy
    for (int i = 0; i < n; i++) {
        int prev = (i == 0) ? parent2.chromosome[n - 1] : parent2.chromosome[i - 1];
        int next = (i == n - 1) ? parent2.chromosome[0] : parent2.chromosome[i + 1];
        edgeTable[parent2.chromosome[i]].push_back(prev);
        edgeTable[parent2.chromosome[i]].push_back(next);
    }


    // Usuniecie powtarzajacych sie sasiadow
    for (int i = 0; i < n; i++) {
        std::sort(edgeTable[i].begin(), edgeTable[i].end());
        edgeTable[i].erase(std::unique(edgeTable[i].begin(), edgeTable[i].end()), edgeTable[i].end());
    }

    // 2) Utworznie chromosomu potomka
    std::vector<bool> visited(n, false); // Track visited cities
    int currentCity = parent1.chromosome[0]; // Start from the first city in parent1
    //int currentCity = gen() % n; // Random first city
    child.chromosome[0] = currentCity;
    visited[currentCity] = true;

    for (int i = 1; i < n; i++) {

        // Usuwamy wszystkie wystapienia wierzcholka currentCity w liscie sasiedztwa
        for (int& neighbor : edgeTable[currentCity]) {
            auto it = std::find(edgeTable[neighbor].begin(), edgeTable[neighbor].end(), currentCity);
            if (it != edgeTable[neighbor].end()) {
                edgeTable[neighbor].erase(it);
            }
        }

        // Wybor nastepnego wierzcholka
        int nextCity = -1;
        if (!edgeTable[currentCity].empty())  // Jesli lista wierzcholka currentCity nie jest pusta
        {
            // Wybieramy sasiada currentCity z najkrotsza lista sasiedztwa
            nextCity = edgeTable[currentCity][0];
            for (int neighbor : edgeTable[currentCity]) {
                if (edgeTable[neighbor].size() < edgeTable[nextCity].size()) {
                    nextCity = neighbor;
                }
            }
        }
        else // Jesli lista sasiadow wierzcholka currentCity jest pusta
        {
            // Losowy wybor nastepnego wierzcholka
            /*do {
                nextCity = gen() % n;
            } while (visited[nextCity]);*/

            // Wstawienie nastepnego miasta jesli nie wystepuje jeszcze w chromosomie dziecka
            for (int city = 0; city < n; city++) {
                if (!visited[city]) {
                    nextCity = city;
                    break;
                }
            }
        }

        // Zapisanie miasta (genu) do chromosomu
        child.chromosome[i] = nextCity;
        visited[nextCity] = true;
        currentCity = nextCity;
    }

    return child;
}

//-------------------------------------------------------------------------------------

bool Genetic::isValidChromosome(const std::vector<int>& chromosome) {
    std::vector<int> counts(n, 0);
    for (int gene : chromosome) {
        if (gene < 0 || gene >= n) return false; // Invalid gene
        counts[gene]++;
    }
    for (int count : counts) {
        if (count != 1) return false; // Missing or duplicate gene
    }
    return true;
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

void Genetic::mutationInsert(individual &ind) {
    std::uniform_int_distribution<> dist(0, n - 1);
    int source, target;
    do {
        source = dist(gen);
        target = dist(gen);
    } while (source == target);

    // Usuniecie genu o indeksie source i przechowanie go w zmiennej tymczasowej
    int gene = ind.chromosome[source];
    ind.chromosome.erase(ind.chromosome.begin() + source);

    // Dostosowanie indeksu target, jesli source < target (po usunieciu z pozycji source nastepuje przesuniecie indeksow)
    if (source < target) {
        target--;
    }

    // Wstawienie usunietego genu na odpowiednia pozycje
    ind.chromosome.insert(ind.chromosome.begin() + target, gene);
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
    auto lastPrintTime = startTime; // Track the last time fitness was printed

    cout << endl << "Wchodzimy do petli. Na razie wszystko ok." << endl;

    // Glowna petla
    while (std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count() < timeLimit) {

        vector<individual> subpopulation;
        subpopulation.reserve(populationSize);

        // Tworzenie nowej populacji
        while (static_cast<int>(subpopulation.size()) < populationSize)
        {
            // Selekcja
            individual parent1 = tournamentSelection(population, 5);
            individual parent2 = tournamentSelection(population, 5);

            individual child1, child2;

            // Krzyzowanie
            uniform_real_distribution<float> floatDist(0.0f, 1.0f);
            float crossoverRoll = floatDist(gen);

            if (crossoverRoll < crossoverRate) {

                // Metoda krzyzowania w zaleznosci od parametru crossoverMethod
                if (crossoverMethod == 1) {
                    child1 = crossoverOX(parent1, parent2);
                    child2 = crossoverOX(parent2, parent1);
                } else {
//                    do {
//                        crossoverPMX(parent1, parent2, child1, child2);
//                    } while(!isValidChromosome(child1.chromosome) || !isValidChromosome(child2.chromosome));
                    child1 = crossoverERX(parent1, parent2);
                    child2 = crossoverERX(parent2, parent1);
                }
            }
            else {
                // Brak krzyzowania - potomkowie to kopie rodzicow
                child1 = parent1;
                child2 = parent2;
            }

            // Mutacje
            float mutationRoll1 = floatDist(gen);
            if (mutationRoll1 < mutationRate) {
                if (mutationMethod == 1)
                    mutationSwap(child1);
                else
                    mutationInsert(child1);
            }

            float mutationRoll2 = floatDist(gen);
            if (mutationRoll2 < mutationRate) {
                if (mutationMethod == 1)
                    mutationSwap(child2);
                else
                    mutationInsert(child1);
            }

            // Obliczenie wartosci funkcji przystosowania po operacjach krzyzowania i mutacji
            child1.fitness = calculateFitness(child1);
            child2.fitness = calculateFitness(child2);

            // Dodanie potomkow do subpopulacji
            subpopulation.push_back(child1);
            if (static_cast<int>(subpopulation.size()) < populationSize) {
                subpopulation.push_back(child2);
            }
        }

        // Sukcesja
        //population = subpopulation;
        succession(population, subpopulation);


        // Znalezienie najlepiej przystosowanego osobnika w populacji
        for (auto &ind : population) {
            if (ind.fitness < bestIndividual.fitness) {
                bestIndividual = ind;
            }
        }

        // Wyswietlenie najlepszego aktualnego rozwiazania co 10 sekund
        auto currentTime = std::chrono::steady_clock::now();
        if (std::chrono::duration<double>(currentTime - lastPrintTime).count() >= 10.0) {
            cout << "Czas: "
                 << std::chrono::duration<double>(currentTime - startTime).count()
                 << " s, Najlepszy koszt: " << bestIndividual.fitness << endl;
            lastPrintTime = currentTime;
        }

    }

    cout << "Koszt znalezionego rozwiazania: " << bestIndividual.fitness << endl;
    cout << "Znalezione rozwiazanie: ";
    for (auto city : bestIndividual.chromosome) {
         cout << city << ", ";
    }
    cout << bestIndividual.chromosome[0] << endl;

}

//-------------------------------------------------------------------------------------

void Genetic::succession(std::vector<individual> &population, std::vector<individual> &subPopulation) {

    // Zostawiamy 10% najlepiej przystosowanych osobnikow z populacji
    int elitismCount = populationSize * 0.1;

    // Sort the current population by fitness (ascending: lower is better)
    std::sort(population.begin(), population.end(), [](const individual& a, const individual& b) {
        return a.fitness < b.fitness;
    });

    // Sort the subpopulation population by fitness
    std::sort(subPopulation.begin(), subPopulation.end(), [](const individual& a, const individual& b) {
        return a.fitness < b.fitness;
    });

    // Retain the top elitismCount individuals in the current population
    for (int i = elitismCount; i < population.size(); i++) {
        population[i] = subPopulation[i - elitismCount]; // Replace the rest with offspring
    }
}

//----------------------------------------------------------------------------------------