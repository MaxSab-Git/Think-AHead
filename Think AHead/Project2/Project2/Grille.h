#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include <map>
#include "Cellule.h"

class Joueur;

class Grille : public sf::Drawable
{
public:
	Grille(int nbCases, const sf::Font& font, float tailleCellule = 50);
	void regenerer(int nbCases);
	bool choisirCase(unsigned int colonne, unsigned int ligne, Joueur& joueur);
	bool VerifierValidite(unsigned int colonne, unsigned int ligne);
	int scoreDansCase(unsigned int colonne, unsigned int ligne);
	void setPosition(const sf::Vector2f& position);
	const sf::Vector2f& getPosition() const;
	sf::Vector2f getSize() const;
	void setTailleCellule(float taille);
	float getTailleCellule() const;

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	std::vector<std::vector<std::pair<Cellule, bool>>> m_cases;
	sf::Text m_valeur;
	std::vector<sf::Vertex> m_grille;
	std::vector<std::vector<sf::Text>> m_valeurs;
	sf::Vector2f m_position;
	float m_tailleCellule;
};

