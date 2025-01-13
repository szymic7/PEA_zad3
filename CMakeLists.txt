cmake_minimum_required(VERSION 3.27)
project(PEA_zad3)

set(CMAKE_CXX_STANDARD 17)

add_executable(PEA_zad3 sources/main.cpp
        sources/Algorithm.cpp
        headers/Algorithm.h
        sources/Greedy.cpp
        headers/Greedy.h
        sources/SimulatedAnnealing.cpp
        headers/SimulatedAnnealing.h
        sources/TabuSearch.cpp
        headers/TabuSearch.h
        sources/Genetic.cpp
        headers/Genetic.h)
