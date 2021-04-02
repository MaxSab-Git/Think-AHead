#pragma once

#include <sstream>
#include <fstream>
#include <iterator>
#include <map>
#include <iostream>

class TypeInconnu
{
public:
	TypeInconnu()
	{

	}

	template<typename T>
	TypeInconnu(T variable)
	{
		m_variable << variable;
	}

	template<typename T>
	TypeInconnu& operator=(T variable)
	{
		m_variable.clear();
		m_variable << variable;
		return *this;
	}

	template<typename T>
	T get() const
	{
		T retour;
		std::istringstream(m_variable.str()) >> retour;
		return retour;
	}

	friend std::ostream& operator<<(std::ostream& flux, const TypeInconnu& a);
	friend std::istream& operator>>(std::istream& flux, TypeInconnu& a);

private:
	std::ostringstream m_variable;
};

std::ostream& operator<<(std::ostream& flux, const TypeInconnu& a)
{
	flux << a.m_variable.str();
	return flux;
}
std::istream& operator>>(std::istream& flux, TypeInconnu& a)
{
	std::string s;
	flux >> s;
	a.m_variable.clear();
	a.m_variable << s;
	return flux;
}

class Sauvegarde
{
public:
	Sauvegarde(const char* fichier) : m_fichier(fichier), m_categorie("base")
	{
		std::ifstream lecture(fichier);

		if (!lecture)
		{
			lecture.close();
			sauver();
			lecture.open(fichier);
		}

		if (lecture)
		{
			std::string s, categorie;
			std::istream_iterator<std::string> it(lecture);
			while (it != std::istream_iterator<std::string>())
			{
				if (it->front() == '[')
				{
					categorie = *it;
					categorie.erase(categorie.begin());
					categorie.erase(categorie.end() - 1);
					it++;
				}
				else
				{
					s = *it;
					while (*it != "=")
					{
						it++;
					}
					it++;

					m_donnees[categorie][s] = *it;

					it++;
				}
			}
		}
		else
		{
			std::cout << "Erreur lors de la creation de la Sauvegarde" << std::endl;
		}
	}

	void recharger()
	{
		std::ifstream lecture(m_fichier);

		if (lecture)
		{
			effacer();

			std::string s, categorie;
			std::istream_iterator<std::string> it(lecture);
			while (it != std::istream_iterator<std::string>())
			{
				if (it->front() == '[')
				{
					categorie = *it;
					categorie.erase(categorie.begin());
					categorie.erase(categorie.end() - 1);
					it++;
				}
				else
				{
					s = *it;
					while (*it != "=")
					{
						it++;
					}
					it++;

					m_donnees[categorie][s] = *it;

					it++;
				}
			}
		}
		else
		{
			std::cout << "Erreur au moment de la lecture." << std::endl;
		}
	}

	void sauver()
	{
		std::ofstream ecrire(m_fichier);
		if (m_donnees.empty())
		{
			m_donnees[m_categorie] = std::map<std::string, TypeInconnu>();
		}
		for (std::map<std::string, std::map<std::string, TypeInconnu>>::iterator ite(m_donnees.begin()); ite != m_donnees.end(); ite++)
		{
			ecrire << "[" << ite->first << "]" << std::endl;
			for (std::map<std::string, TypeInconnu>::iterator it(m_donnees[ite->first].begin()); it != m_donnees[ite->first].end(); it++)
			{
				ecrire << it->first << " = " << it->second << std::endl;
			}
		}
	}

	template<typename T>
	void charger(const char* nomValeur, T &nombre)
	{
		if (m_donnees[m_categorie].find(nomValeur) != m_donnees[m_categorie].end())
		{
			nombre = m_donnees[m_categorie][nomValeur].get<T>();
		}
		else
		{
			try
			{
				throw std::runtime_error(std::string("Le nom de valeur \"") + nomValeur + "\" n'existe pas.");
			}
			catch (const std::runtime_error &e)
			{
				std::cerr << e.what() << std::endl;
			}
		}
	}

	bool testPresence(const char* nomValeur)
	{
		return m_donnees[m_categorie].find(nomValeur) != m_donnees[m_categorie].end();
	}

	template<typename T>
	void ajouter(const char* nomValeur, T nombre)
	{
		if (m_donnees[m_categorie].find(nomValeur) == m_donnees[m_categorie].end())
		{
			std::string s(nomValeur);
			if (find(s.begin(), s.end(), ' ') != s.end() 
				|| find(s.begin(), s.end(), '\n') != s.end() 
				|| find(s.begin(), s.end(), '[') != s.end()
				|| find(s.begin(), s.end(), ']') != s.end())
			{
				std::cout << "Le nom de valeur \"" << nomValeur << "\" n'est pas valide." << std::endl;
				return;
			}
			m_donnees[m_categorie][nomValeur] = nombre;
		}
		else
		{
			std::cout << "Nom de valeur deja present." << std::endl;
		}
	}

	template<typename T>
	void modifier(const char* nomValeur, T nombre)
	{
		if (m_donnees[m_categorie].find(nomValeur) != m_donnees[m_categorie].end())
		{
			m_donnees[m_categorie][nomValeur] = nombre;
		}
		else
		{
			std::cout << "Le nom de valeur \"" << nomValeur << "\" n'existe pas." << std::endl;
		}
	}

	void effacer()
	{
		m_donnees.clear();
	}

	void effacer(const char* nomValeur)
	{
		if (m_donnees[m_categorie].find(nomValeur) != m_donnees[m_categorie].end())
		{
			m_donnees[m_categorie].erase(nomValeur);
		}
		else
		{
			std::cout << "Le nom de valeur \"" << nomValeur << "\" n'existe pas." << std::endl;
		}
	}

	void nouvelleCategorie(const char* categorie)
	{
		if (m_donnees.find(categorie) == m_donnees.end())
		{
			std::string s(categorie);
			if (find(s.begin(), s.end(), ' ') != s.end()
				|| find(s.begin(), s.end(), '\n') != s.end()
				|| find(s.begin(), s.end(), '[') != s.end()
				|| find(s.begin(), s.end(), ']') != s.end())
			{
				std::cout << "Le nom de valeur \"" << categorie << "\" n'est pas valide." << std::endl;
				return;
			}
			m_donnees[categorie] = std::map<std::string, TypeInconnu>();
		}
	}

	void changerCategorie(const char* categorie)
	{
		if (m_donnees.find(categorie) != m_donnees.end())
		{
			m_categorie = categorie;
		}
		else
		{
			std::cout << "Le nom de categorie \"" << categorie << "\" n'existe pas." << std::endl;
		}
	}

private:
	const char* m_fichier;
	std::map<std::string, std::map<std::string, TypeInconnu>> m_donnees;
	std::string m_categorie;
};
