#pragma once
#include <array>
#include <unordered_set>
#include <set>
#include <vector>
#include <random>
#include <ctime>
#include <memory>

using GraphMatrix = std::vector<std::vector<bool>>;
using Vertexes = std::set<size_t>;

bool checkOnLink(const Vertexes& vertexes, size_t vertex, const GraphMatrix& graph);

class Solve
{
public:
	Solve(Vertexes&& ver, const GraphMatrix& graph, size_t desired)
		: m_vertexes(std::move(ver)), m_graph(graph), m_desired(desired)
	{
		calcBestVariant();
	}

	bool operator<(const Solve& sec) const
	{
		return getBestResult() < sec.getBestResult();
	}

	int getBestResult() const { return m_maxValue; }

	bool isSolved() const { return m_solved; }
private:
	void calcBestVariant()
	{
		if (m_vertexes.empty())
		{
			m_maxValue = 0;
			return;
		}
		const size_t graphSize = m_graph.size();
		std::set<size_t> temp;
		temp.insert(*m_vertexes.begin());
		size_t locMax = 1;
		for (auto itBase = m_vertexes.begin(); itBase!=--m_vertexes.end(); )
		{
			size_t baseVer = *itBase;
			temp.clear();
			temp.insert(baseVer);
			for (auto it = ++itBase; true;)
			{
				if (checkOnLink(temp, *it))
				{
					temp.insert(*it);
					locMax = std::max(locMax, temp.size());
					if (locMax >= m_desired) break;
				}
				if (++it == m_vertexes.end())
				{
					if (temp.size() == 1)
					{
						break;
					}
					auto tempIt = --temp.end();
					it = ++m_vertexes.find(*tempIt);
					temp.erase(tempIt);
					if (it == m_vertexes.end()) break;
				}
			}
		}
		m_maxValue = locMax;
		if (locMax >= m_desired)
		{
			m_solved = true;
		}
	}
	bool checkOnLink(const Vertexes& vertexes, size_t vertex)
	{
		return ::checkOnLink(vertexes, vertex, m_graph);
	}

	int m_maxValue = 0;
	Vertexes m_vertexes;
	const GraphMatrix& m_graph;
	const size_t m_desired;
	bool m_solved = false;

	friend Solve interbreed(const Solve& first, const Solve& second);
	friend void mutate(Solve& sol);
};


bool checkOnLink(const Vertexes& vertexes, size_t vertex, const GraphMatrix& graph)
{
	for (size_t v : vertexes)
	{
		if (!graph[v][vertex])
		{
			return false;
		}
	}
	return true;
}

Solve interbreed(const Solve& first, const Solve& second)
{
	const size_t graphSize = first.m_graph.size();

	std::mt19937 engine;
	engine.seed(std::time(nullptr));
	const Vertexes& fVer = first.m_vertexes, sVer = second.m_vertexes;
	Vertexes newVertexes;
	for (auto it = fVer.begin(); it!=fVer.end(); it++)
	{
		size_t v = *it;
		if (sVer.count(v))
		{
			newVertexes.insert(v);
		}
		else
		{
			if (engine() % 2)
			{
				newVertexes.insert(v);
			}
		}
	}
	for (auto it = sVer.begin(); it != sVer.end(); it++)
	{
		size_t v = *it;
		if (engine() % 2)
		{
			newVertexes.insert(v);
		}
	}
	return Solve(std::move(newVertexes), first.m_graph, first.m_desired);
}

void mutate(Solve& sol)
{
	const size_t graphSize = sol.m_graph.size();
	std::mt19937 engine;
	engine.seed(std::time(nullptr));
	const auto& ver = sol.m_vertexes;

	size_t v = 0;
	for (auto it = ver.begin(); it != ver.end(); it++)
	{
		size_t currVer = *it;
		for (; v < currVer; v++)
		{
			if (!(static_cast<size_t>(engine()) % graphSize))
			{
				sol.m_vertexes.insert(v);
			}
		}
	}
	sol.calcBestVariant();
}