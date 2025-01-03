#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include "../headers/Greedy.h"
#include "../headers/TabuSearch.h"
#include "../headers/SimulatedAnnealing.h"

using namespace std;

//-------------------------------------------------------------------------------------------------

void showMenu()
{
    int choice = 0, n = 0, choiceNeighborhood = 0, choiceAlfa = 0;
    string filename;
    bool quit = false;

    // Macierz kosztow
    int **costMatrix = nullptr;

    // Obiekty klas reprezentujacych algorytmy
    Greedy greedy;
    TabuSearch tabuSearch;
    SimulatedAnnealing simulatedAnnealing;

    // Kryterium stopu dla algorytmow
    int stopCriterion = 0;

    // Ostatnie znalezione rozwiazanie
    vector<int> result;
    result.clear();

    do {
        cout << endl << "Wybierz operacje: " << endl;
        cout << "----------------------------------------------------------" << endl;
        cout << "1) Wczytaj dane z pliku" << endl;
        cout << "2) Ustaw kryterium stopu dla algorytmow" << endl;
        cout << "3) Wyznacz rozwiazanie metoda zachlanna" << endl;
        cout << "----------------------------------------------------------" << endl;
        cout << "4) Wybierz definicje sasiedztwa dla algorytmu Tabu-search" << endl;
        cout << "5) Wykonaj algorytm Tabu-search" << endl;
        cout << "----------------------------------------------------------" << endl;
        cout << "6) Ustaw wspolczynnik zmiany temperatury dla algorytmu SW" << endl;
        cout << "7) Wykonaj algorytm Symulowanego Wyzarzania" << endl;
        cout << "----------------------------------------------------------" << endl;
        cout << "8) Zapisz sciezke rozwiazania do pliku .txt" << endl;
        cout << "9) Wczytaj sciezke z pliku .txt i oblicz koszt" << endl;
        cout << "10) Zakoncz" << endl;
        cout << "----------------------------------------------------------" << endl;
        cout << "Wybor:";
        cin >> choice;

        switch(choice) {
            case 1: // wczytaj tablice z pliku XML
            {
                cout << endl << "Podaj nazwe pliku: " << endl;
                cin >> filename;
                ifstream file(filename);

                if(!file.good()) {
                    cout << endl << "W projekcie nie ma pliku o takiej nazwie." << endl;
                    file.close();
                    break;
                }

                // Usuniecie poprzedniej macierzy przed zapisaniem nowego rozmiaru n
                if(costMatrix != nullptr) {
                    for (int i = 0; i < n; ++i) {
                        delete[] costMatrix[i];
                    }
                    delete[] costMatrix;
                }

                std::string line;
                n = 0;

                while (std::getline(file, line)) {
                    if (line.find("DIMENSION") != std::string::npos) {
                        // Zczytanie rozmiaru macierzy z linijki z "DIMENSION"
                        n = std::stoi(line.substr(line.find(":") + 1));
                    }
                    if (line.find("EDGE_WEIGHT_SECTION") != std::string::npos) {
                        break;
                    }
                }

                if (n == 0) {
                    std::cerr << "Error: Rozmiar macierzy nie zostal znaleziony." << std::endl;
                    return;
                }

                // Utworzenie nowej dynamicznej tablicy 2d
                costMatrix = new int*[n];
                for (int i = 0; i < n; ++i) {
                    costMatrix[i] = new int[n];
                }

                // Zczytywanie wartosci macierzy
                int row = 0, col = 0;
                while (std::getline(file, line) && row < n) {
                    std::istringstream stream(line);
                    int value;
                    while (stream >> value) {
                        costMatrix[row][col++] = value;
                        if (col == n) {
                            col = 0;
                            row++;
                        }
                    }
                }

                file.close();

                // Wyswietlenie macierzy
                cout << endl << "Macierz kosztow:" << endl;
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < n; j++) {
                        int number = costMatrix[i][j];
                        cout << number << " ";
                        if (number < 100) cout << " ";
                        if (number > 0 && number < 10) cout << " ";
                    }
                    cout << endl;
                }

                break;
            }

            case 2: // ustaw kryterium stopu
            {
                cout << endl << "Podaj wartosc kryterium stopu dla algorytmow (w pelnych sekundach): ";
                cin >> stopCriterion;

                if(stopCriterion > 0) {
                    tabuSearch.setTimeLimit(stopCriterion);
                    simulatedAnnealing.setTimeLimit(stopCriterion);
                    cout << endl << "Kryterium stopu zostalo ustawione dla algorytmow TS i SW." << endl;
                } else {
                    cout << endl << "Bledna wartosc. Kryterium stopu nie zostalo ustawione." << endl;
                }

                break;
            }

            case 3: // rozwiazanie metoda zachlanna
            {
                if (costMatrix != nullptr) {

                    greedy.setN(n);
                    greedy.setCostMatrix(costMatrix);

                    greedy.greedyAlgorithm();
                    cout << "Wynik dzialania algorytmu zachlannego: " << greedy.getResult() << endl;
                    cout << "Najlepsza sciezka: ";
                    greedy.printResultVertices();
                    cout << endl;

                } else {
                    cout << endl << "Nie wczytano macierzy kosztow. Wczytaj macierz, aby wykonac algorytm." << endl;
                }
                break;
            }

            case 4: // wybor definicji sasiedztwa dla TS
            {
                do {

                    cout << endl << "Wybierz definicje sasiedztwa:" << endl;
                    cout << "----------------------------------------------------------------------------------" << endl;
                    cout << "1) Insert" << endl;
                    cout << "2) Swap" << endl;
                    cout << "3) Invert" << endl;
                    cout << "Wybor:";
                    cin >> choiceNeighborhood;

                    if(choiceNeighborhood < 1 || choiceNeighborhood > 3) {
                        cout << endl << "Nieprawidlowy numer wybranej opcji. Sprobuj ponownie." << endl;
                        choiceNeighborhood = 0;
                    }

                } while (choiceNeighborhood == 0);

                tabuSearch.setNeighborhoodDef(choiceNeighborhood);

                break;
            }

            case 5: // Algorytm Tabu-search
            {
                if (costMatrix != nullptr) {

                    tabuSearch.setN(n);
                    tabuSearch.setCostMatrix(costMatrix);

                    switch (n) {
                        case 56:
                            tabuSearch.setTabuCadency(10);
                            tabuSearch.setIterationsLimit(50);
                            cout << endl << "Ustawiono parametry dla ftv55." << endl;
                            break;
                        case 171:
                            tabuSearch.setTabuCadency(15);
                            tabuSearch.setIterationsLimit(75);
                            cout << endl << "Ustawiono parametry dla ftv170." << endl;

                            break;
                        case 358:
                            tabuSearch.setTabuCadency(20);
                            tabuSearch.setIterationsLimit(150);
                            cout << endl << "Ustawiono parametry dla rbg358." << endl;
                            break;
                        default:
                            tabuSearch.setTabuCadency(10);
                            tabuSearch.setIterationsLimit(50);
                            cout << endl << "Ustawiono domyslne parametry algorytmu TS." << endl;
                            break;
                    }

                    tabuSearch.algorithm();
                    result = tabuSearch.getResultVector();
                    cout << "Koszt znalezionego rozwiazania: " << tabuSearch.calculatePathCost(result) << endl;
                    cout << "Znalezione rozwiazanie: ";
                    for(const int& vertice : result)
                        cout << vertice << ", ";
                    cout << result[0];

                    cout << endl;

                } else {
                    cout << endl << "Nie wczytano macierzy kosztow. Wczytaj macierz, aby wykonac algorytm." << endl;
                }
                break;
            }

            case 6: // ustaw wspolczynnik zmiany temperatury dla SW
            {
                do {

                    cout << endl << "Wybierz wspolczynnik zmiany temperatury a dla algorytmu SW:" << endl;
                    cout << "----------------------------------------------------------------------------------" << endl;
                    cout << "1) a = 0.995" << endl;
                    cout << "2) a = 0.99" << endl;
                    cout << "3) a = 0.9" << endl;
                    cout << "Wybor:";
                    cin >> choiceAlfa;

                    if(choiceAlfa < 1 || choiceAlfa > 3) {
                        cout << endl << "Nieprawidlowy numer wybranej opcji. Sprobuj ponownie." << endl;
                        choiceAlfa = 0;
                    }

                } while (choiceAlfa == 0);

                switch (choiceAlfa) {
                    case 1:
                        simulatedAnnealing.setAlfa(0.995);
                        break;
                    case 2:
                        simulatedAnnealing.setAlfa(0.99);
                        break;
                    case 3:
                        simulatedAnnealing.setAlfa(0.9);
                        break;
                    default:
                        cout << "Wystapil blad. Przypisano wartosc domyslna wspolczynnika alfa (0.99)." << endl;
                        simulatedAnnealing.setAlfa(0.99);
                        break;
                }

                cout << endl << "Wartosc wspolczynnika alfa zostala ustawiona." << endl;

                break;
            }

            case 7: // Algorytm Symulowanego Wyzarzania

                if (costMatrix != nullptr) {

                    simulatedAnnealing.setN(n);
                    simulatedAnnealing.setCostMatrix(costMatrix);

                    simulatedAnnealing.algorithm();
                    result = simulatedAnnealing.getResultVector();
                    cout << "Koszt znalezionego rozwiazania: " << simulatedAnnealing.calculatePathCost(result) << endl;
                    cout << "Znalezione rozwiazanie: ";
                    for(const int& vertice : result)
                        cout << vertice << ", ";
                    cout << result[0];

                    cout << endl;

                } else {
                    cout << endl << "Nie utworzono macierzy kosztow. Utworz macierz, aby wykonac algorytm." << endl;
                }

                break;

            case 8: // Zapis sciezki rozwiazania do pliku .txt

                if (!result.empty()) {

                    cout << endl << "Podaj nazwe pliku do zapisania sciezki rozwiazania: ";
                    cin >> filename;

                    std::ofstream outFile(filename);
                    if (!outFile) {
                        cerr << "Error: Nie mozna otworzyc pliku " << filename << endl;
                        return;
                    }

                    // Wypisanie liczby wierzcholkow w pierwszej linijce pliku
                    outFile << result.size() << std::endl;

                    // Kolejne wierzcholki w kolejnych liniach
                    for (const int& vertex : result) {
                        outFile << vertex << "\n";
                    }

                    // Dodanie wierzcholka startowego na koncu trasy
                    outFile << result.front() << "\n";

                    outFile.close();
                    cout << endl << "Sciezka zapisana do pliku: " << filename << endl;

                } else {
                    cout << endl << "Wykonaj algorytm TS lub SW, aby zapisac wynik do pliku." << endl;
                }

                break;

            case 9: // wczytanie sciezki z pliku .txt
            {
                if(costMatrix != nullptr) {

                    cout << endl << "Podaj nazwe pliku do odczytania sciezki rozwiazania: ";
                    cin >> filename;

                    std::ifstream inFile(filename);
                    if (!inFile) {
                        cout << endl << "Brak pliku o podanej nazwie. Podaj prawidlowa nazwe pliku." << endl;
                        break;
                    }

                    // Odczytanie liczby wierzcholkow
                    int numVertices;
                    if (!(inFile >> numVertices)) {
                        cout << endl << "Blad. Nie mozna odczytac dlugosci sciezki. Sprobuj ponownie." << endl;
                        break;
                    }

                    // Odczytanie sciezki
                    int *path = new int[n + 1];
                    int vertex, index = 0;
                    while (inFile >> vertex) {
                        path[index++] = vertex;
                    }

                    inFile.close();


                    // Obliczenie kosztu sciezki na podstawie macierzy kosztow
                    int pathCost = 0;
                    for (int i = 0; i < numVertices; i++) {
                        pathCost += costMatrix[path[i]][path[i + 1]];
                    }

                    cout << endl << "Odczytana sciezka:" << endl;
                    for (int i = 0; i < numVertices + 1; i++) {
                        cout << path[i] << " ";
                    }
                    cout << endl << "Calkowity koszt scieki: " << pathCost << endl;

                    // Zwolnienie pamieci
                    delete [] path;

                } else {

                    cout << endl << "Wczytaj macierz kosztow, aby moc wyznaczyc koszt sciezki zapisanej w pliku." << endl;

                }

                break;
            }
            case 10: // Zakoncz

                quit = true;
                break;

            default:
                cout << endl << "Niepoprawny numer wybranej opcji. Wybierz ponownie." << endl;
                break;
        }

    } while (!quit);
}

//-------------------------------------------------------------------------------------------------

int main() {
    showMenu();
    return 0;
}

