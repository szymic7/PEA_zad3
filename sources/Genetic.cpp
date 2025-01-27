#include "../headers/Genetic.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>

using namespace std;

//-------------------------------------------------------------------------------------

Genetic::Genetic() : Algorithm(), gen(std::random_device{}()) {

    timeLimit = 0;

    populationSize = 2000; // wartosc domyslna
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

    // Wybranie losowego osobnika z populacji jako pierwszego uczestnika
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

    // 3) Wypelnienie chromosomu dziecka brakujacymi genami z parent2, w odpowiedniej kolejnosci, z pominieciem duplikatow
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

individual Genetic::crossoverERX(const individual& parent1, const individual& parent2) {
    individual child; // Inicjalizacja potomka
    child.chromosome.resize(n, -1); // Inicjalizacja chromosomu dziecka wartosciami -1

    // Utworzenie listy sasiedztwa dla wszystkich wierzcholkow
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

    // Utworznie chromosomu potomka
    std::vector<bool> visited(n, false); // Odwiedzone wierzcholki
    int currentCity = parent1.chromosome[0]; // Zaczynamy od pierwszego genu rodzica parent1
    child.chromosome[0] = currentCity;
    visited[currentCity] = true;

    for (int i = 1; i < n; i++) {

        // Usuniecie wszystkich wystapien wierzcholka currentCity w listach sasiedztwa
        for (int& neighbor : edgeTable[currentCity]) {
            auto it = std::find(edgeTable[neighbor].begin(), edgeTable[neighbor].end(), currentCity);
            if (it != edgeTable[neighbor].end()) {
                edgeTable[neighbor].erase(it);
            }
        }

        // Wybor nastepnego wierzcholka
        int nextCity = -1;
        if (!edgeTable[currentCity].empty()) // Jesli lista wierzcholka currentCity nie jest pusta
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
            // Wstawienie nastepnego miasta, jesli nie wystepuje jeszcze w chromosomie dziecka
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

    // TWORZENIE POPULACJI POCZATKOWEJ

    // Pierwszy osobnik
    individual i1;

    // Losowy uklad genow (miast)
    i1.chromosome.resize(n);
    std::iota(i1.chromosome.begin(), i1.chromosome.end(), 0);
    std::shuffle(i1.chromosome.begin(), i1.chromosome.end(), gen);

    // Wyznaczenie wartosci funkcji przystosowania (kosztu sciezki)
    i1.fitness = calculateFitness(i1);

    // Dodanie osobnika do polulacji
    population.push_back(i1);

    // Zapisujemy pierwszego osobnika jako bestIndividual
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

    auto startTime = std::chrono::steady_clock::now();
    auto lastPrintTime = startTime; // Zmienna pomocnicza do wyswietlania najlepszego rozwiazania co 10s

    cout << endl << "Rozwiazanie poczatkowe:" << endl;
    cout << bestIndividual.fitness << endl;

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

                // Metoda mutacji w zaleznosci od paametru mutationMethod
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
            cout << bestIndividual.fitness << endl;
            lastPrintTime = currentTime;
        }

    }

    cout << endl << "Koszt znalezionego rozwiazania: " << bestIndividual.fitness << endl;
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

    // Posortowanie osobnikow obecnej populacji rosnaco wg funkcji przystosowania
    std::sort(population.begin(), population.end(), [](const individual& a, const individual& b) {
        return a.fitness < b.fitness;
    });

    // Posortowanie osobnikow subpopulacji rosnaco wg funkcji przystosowania
    std::sort(subPopulation.begin(), subPopulation.end(), [](const individual& a, const individual& b) {
        return a.fitness < b.fitness;
    });

    // Zastapienie najslabszych 90% populacji osobnikami z subpopulacji
    for (int i = elitismCount; i < population.size(); i++) {
        population[i] = subPopulation[i - elitismCount];
    }
}

//----------------------------------------------------------------------------------------