#pragma once
#include "Grille.h"
#include "Chronometre.h"

class Joueur
{
public:
	Joueur(sf::String nom);
	const sf::String& getNom() const;
	int getScore() const;
	void ajouterScore(int score);
	void scoreZero();
	Chronometre& chrono();

private:
	bool m_IA;
	sf::String m_nom;
	int m_score;
	Chronometre m_chrono;
	friend bool Grille::choisirCase(unsigned int colonne, unsigned int ligne, Joueur& joueur);

};

