#pragma once
#include <SFML/Graphics.hpp>
#include <random>
class Cellule
{
public:
	Cellule();
	void changerScore();
	int getScore() const;
private:
	std::uniform_int_distribution<int> m_tirage{ 1,100 };
	std::default_random_engine m_seed;
	int m_score;
};