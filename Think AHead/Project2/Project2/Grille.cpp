#include "Grille.h"
#include "Joueur.h"
#include <iostream>

Grille::Grille(int nbCases, const sf::Font& font, float tailleCellule) : m_tailleCellule(tailleCellule)
{
	m_valeur.setFont(font);
	m_valeur.setFillColor(sf::Color::Black);
	m_valeur.setCharacterSize(20U);
	m_valeurs.resize(nbCases);
	regenerer(nbCases);
	setTailleCellule(tailleCellule);
}

void Grille::regenerer(int nbCases)
{
	m_cases.clear();
	int negatifs(0);
	for (int i = 0; i < nbCases; i++)
	{
		m_cases.push_back(std::vector<std::pair<Cellule, bool>>());
		for (int j = 0; j < nbCases; j++)
		{
			Cellule cel;
			while (negatifs > (nbCases - 1) * 2 && cel.getScore() <= 0)
			{
				cel.changerScore();
			}
			if (cel.getScore() <= 0)
			{
				negatifs++;
			}
			m_cases[i].push_back(std::pair<Cellule, bool>(cel, true));
		}
	}
	m_grille.clear();
	for (int i = 0; i < nbCases + 1; i++)
	{
		m_grille.push_back(sf::Vertex(m_position + sf::Vector2f(i * m_tailleCellule, -1.f), sf::Color(127, 127, 127)));
		m_grille.push_back(sf::Vertex(m_position + sf::Vector2f(i * m_tailleCellule, nbCases * m_tailleCellule), sf::Color(127,127,127)));
		m_grille.push_back(sf::Vertex(m_position + sf::Vector2f(-1.f, i * m_tailleCellule), sf::Color(127, 127, 127)));
		m_grille.push_back(sf::Vertex(m_position + sf::Vector2f(nbCases * m_tailleCellule, i * m_tailleCellule), sf::Color(127, 127, 127)));
	}
	m_valeurs.clear();
	m_valeurs.resize(nbCases);

	for (unsigned int i = 0; i < m_valeurs.size(); i++)
	{
		m_valeurs[i].resize(nbCases);
		for (unsigned int j = 0; j < m_valeurs.size(); j++)
		{
			m_valeurs[i][j] = m_valeur;
			m_valeurs[i][j].setPosition(m_position + sf::Vector2f(m_tailleCellule * i + m_tailleCellule / 2, m_tailleCellule * j + m_tailleCellule / 2));
			m_valeurs[i][j].setString(std::to_string(m_cases[i][j].first.getScore()));
			m_valeurs[i][j].setOrigin(m_valeurs[i][j].getGlobalBounds().width / 2, (float)m_valeurs[i][j].getCharacterSize() / 2);
		}
	}
}

bool Grille::choisirCase(unsigned int colonne, unsigned int ligne, Joueur& joueur)
{
	if (m_cases.size() > ligne && m_cases.size() > colonne)
	{
		if (m_cases[colonne][ligne].second)
		{
			joueur.m_score += m_cases[colonne][ligne].first.getScore();
			m_cases[colonne][ligne].second = false;
			return true;
		}
	}
	return false;
}

bool Grille::VerifierValidite(unsigned int colonne, unsigned int ligne)
{
	if (m_cases.size() > ligne && m_cases.size() > colonne)
	{
		return m_cases[colonne][ligne].second;
	}
	return false;
}

int Grille::scoreDansCase(unsigned int colonne, unsigned int ligne)
{
	if (m_cases.size() > ligne && m_cases.size() > colonne)
	{
		return m_cases[colonne][ligne].first.getScore();
	}
	return 0;
}

void Grille::setPosition(const sf::Vector2f& position)
{
	m_position = position;

	for (unsigned int i = 0; i < m_grille.size() / 4; i++)
	{
		m_grille[i + i * 3].position = m_position + sf::Vector2f(i * m_tailleCellule, -1.f);
		m_grille[i + 1 + i * 3].position = m_position + sf::Vector2f(i * m_tailleCellule, (m_grille.size() / 4 - 1) * m_tailleCellule);
		m_grille[i + 2 + i * 3].position = m_position + sf::Vector2f(-1.f, i * m_tailleCellule);
		m_grille[i + 3 + i * 3].position = m_position + sf::Vector2f((m_grille.size() / 4 - 1) * m_tailleCellule, i * m_tailleCellule);
	}
	for (unsigned int i = 0; i < m_valeurs.size(); i++)
	{
		for (unsigned int j = 0; j < m_valeurs.size(); j++)
		{
			m_valeurs[i][j].setPosition(position + sf::Vector2f(m_tailleCellule * i + m_tailleCellule / 2, m_tailleCellule * j + m_tailleCellule / 2));
		}
	}
}

const sf::Vector2f& Grille::getPosition() const
{
	return m_position;
}

sf::Vector2f Grille::getSize() const
{
	return sf::Vector2f((float)m_cases.size() * m_tailleCellule, (float)m_cases.size() * m_tailleCellule);
}

void Grille::setTailleCellule(float taille)
{
	m_tailleCellule = taille;
	for (unsigned int i = 0; i < m_valeurs.size(); i++)
	{
		for (unsigned int j = 0; j < m_valeurs.size(); j++)
		{
			m_valeurs[i][j].setCharacterSize(m_tailleCellule * (2.f / 5.f));
			m_valeurs[i][j].setOrigin(m_valeurs[i][j].getGlobalBounds().width / 2, (float)m_valeurs[i][j].getCharacterSize() / 2);
		}
	}
	setPosition(getPosition());
}

float Grille::getTailleCellule() const
{
	return m_tailleCellule;
}

void Grille::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_grille.data(), m_grille.size(), sf::PrimitiveType::Lines);
	for (unsigned int i = 0; i < m_valeurs.size(); i++)
	{
		for (unsigned int j = 0; j < m_valeurs.size(); j++)
		{
			target.draw(m_valeurs[i][j]);
		}
	}
}
