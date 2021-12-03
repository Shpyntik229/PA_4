#include <iostream>
#include <fstream>
#include <map>
#include "Genetic.h"

int main()
{
    std::ifstream graphIn("graph.txt");
    size_t graphSize;
    graphIn >> graphSize;
    GraphMatrix graph(graphSize, std::vector<bool>(graphSize));
    std::vector<size_t> pows(graphSize);
    for (int i = 0; i < graphSize; i++)
    {
        size_t r1, r2;
        graphIn >> r1 >> r2;
        graph[r1][r2] = true;
        graph[r2][r1] = true;
        pows[r1]++;
        pows[r2]++;
    }

    size_t desired;
    std::cin >> desired;
    std::vector<std::unique_ptr<Solve>> solves;
    solves.reserve(graphSize);
    std::multimap<size_t, int> solves_s;
    for (int i = 0; i < graphSize; i++)
    {
        if (pows[i] + 1 >= desired)
        {
            solves.push_back(std::make_unique<Solve>(Vertexes({ (size_t)i }), graph, desired));
            solves_s.insert({ solves.back()->getBestResult(), solves.size() - 1 });
        }
    }

    std::mt19937 engine;
    engine.seed(std::time(nullptr));
    bool isSolved = false;
    size_t locMax = 2;
    if (solves.size() > 2 && solves.size() >= desired)
    {
        while (true)
        {
            size_t ind1 = static_cast<size_t>(engine()) % solves.size();
            size_t ind2 = static_cast<size_t>(engine()) % solves.size();
            if (ind1 == ind2)
            {
                if (ind2 != solves.size() - 1)
                {
                    ind2++;
                }
                else
                {
                    ind2--;
                }
            }
            auto interbreeded = std::make_unique<Solve>(interbreed(*solves[ind1], *solves[ind2]));
            if (interbreeded->isSolved())
            {
                isSolved = true;
                break;
            }
            mutate(*interbreeded);
            if (interbreeded->isSolved())
            {
                isSolved = true;
                break;
            }
            if (solves_s.begin()->first < interbreeded->getBestResult())
            {
                size_t bestResult = interbreeded->getBestResult();
                int pos = solves_s.begin()->second;
                solves[pos] = std::move(interbreeded);
                solves_s.erase(solves_s.begin());
                solves_s.insert({ bestResult, pos });
            }
        }
    }
    if (solves.size() > desired)
    {
        isSolved = true;
    }
    if (isSolved)
    {
        std::cout << "Solved!!!";
    }
    else
    {
        std::cout << "Not solved :(";
    }
    return 0;
}