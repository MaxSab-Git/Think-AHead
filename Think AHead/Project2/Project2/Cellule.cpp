#include "Cellule.h"
#include <chrono>

Cellule::Cellule() : m_seed(std::chrono::high_resolution_clock::now().time_since_epoch().count())
{
	changerScore();
}

void Cellule::changerScore()
{
	int val(m_tirage(m_seed));
	int chanceObtention[15] = { 9, 9, 9, 8, 8, 7, 7, 7, 6, 6, 6, 5, 5, 4, 4};
	int chance(0);
	for (int i = 0; i < 15; i++)
	{
		if (val <= chanceObtention[i] + chance)
		{
			if (m_tirage(m_seed) <= 50)
			{
				m_score = i + 1;
			}
			else
			{
				m_score = -(i + 1);
			}
			break;
		}
		chance += chanceObtention[i];
	}
}

int Cellule::getScore() const
{
	return m_score;
}
