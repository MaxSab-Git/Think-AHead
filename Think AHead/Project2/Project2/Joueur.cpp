#include "Joueur.h"

Joueur::Joueur(sf::String nom) : m_nom(nom), m_score(0)
{
	
}

const sf::String& Joueur::getNom() const
{
	return m_nom;
}

int Joueur::getScore() const
{
	return m_score;
}

void Joueur::ajouterScore(int score)
{
	m_score += score;
}

void Joueur::scoreZero()
{
	m_score = 0;
}

Chronometre& Joueur::chrono()
{
	return m_chrono;
}
