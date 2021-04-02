// Think AHead 0.5.3

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include "Grille.h"
#include "Joueur.h"
#include "Sauvegarde.h"
#include <iostream>
#include <random>
#include <chrono>

using namespace sf;
using namespace std;

enum class ModeDeJeu
{
	Normal,
	ContreLaMontre
};

struct Bonus
{
	Bonus(Joueur &joueur, String texte, int scoreBonus) : cible(joueur), type(texte), score(scoreBonus)
	{

	}

	Joueur &cible;
	String type;
	int score;
};

void ouverture();
void menuPrincipal();
void classement();
void lancerLaPartie();
vector<Joueur> selectionNomJoueurs(unsigned int nbJoueur);
int jeu(int tailleGrille, vector<Joueur>& joueurs, ModeDeJeu modeDeJeu);
int victoire(const Texture& ecran, vector<Joueur>& joueurs, const vector<Bonus>& bonus);

RenderWindow window;
Font font;
Cursor arrow, hand, textbar;
Texture logoThinkAHead;

SoundBuffer selectButton;
SoundBuffer backButton;
SoundBuffer changeParameter;
SoundBuffer changeMode;
Sound son;

int main()
{
	// création d'une fenêtre en plein écran avec la syncronisation verticale activée
	window.create(VideoMode::getFullscreenModes()[0], "Think AHead", Style::Fullscreen);
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);

	// chargement des ressourses nescessaires au bon fonctinnement de l'application
	if (!font.loadFromFile("arial.ttf"))
	{
		return -1;
	}
	if (!hand.loadFromSystem(Cursor::Type::Hand))
	{
		return -2;
	}
	if (!arrow.loadFromSystem(Cursor::Type::Arrow))
	{
		return -3;
	}
	if (!textbar.loadFromSystem(Cursor::Type::Text))
	{
		return -4;
	}
	if (!logoThinkAHead.loadFromFile("images/logo-Think-AHead.png"))
	{
		return -5;
	}
	if (!selectButton.loadFromFile("sons/selectButton.wav"))
	{
		return -7;
	}
	if (!backButton.loadFromFile("sons/backButton.wav"))
	{
		return -8;
	}
	if (!changeParameter.loadFromFile("sons/changeParameter.wav"))
	{
		return -9;
	}
	if (!changeMode.loadFromFile("sons/changeMode.wav"))
	{
		return -10;
	}
	logoThinkAHead.setSmooth(true);
	ouverture();
	menuPrincipal();
	return 0;
}

void ouverture()
{
	// chargement du logo SFML
	Texture logoSFML;
	if (!logoSFML.loadFromFile("images/sfml-logo-big.png"))
	{
		exit(-6);
	}
	logoSFML.setSmooth(true);

	// initialisation de l'image à afficher
	Sprite spriteLogoSFML(logoSFML);
	spriteLogoSFML.setPosition(window.getView().getCenter());
	spriteLogoSFML.setOrigin(spriteLogoSFML.getGlobalBounds().width / 2, spriteLogoSFML.getGlobalBounds().height / 2);
	spriteLogoSFML.setScale(0.7f, 0.7f);
	spriteLogoSFML.setColor(Color(255, 255, 255, 0));

	Event e;
	Clock c;
	// début de la boucle de jeu
	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case Event::Closed:
			{
				window.close();
				break;
			}
			}
		}
		// éxécution de l'animation de fondu
		float t(c.getElapsedTime().asSeconds());
		if (t > 0.5f && t < 1.5f)
		{
			// apparition du logo en fondu
			t -= 0.5f;
			spriteLogoSFML.setColor(Color(255, 255, 255, Uint8(255 * t)));
		}
		else if (t > 1.5f && t < 2.5f)
		{
			spriteLogoSFML.setColor(Color(255, 255, 255, 255));
		}
		else if(t > 2.5f && t < 3.5f)
		{
			// disparition du logo en fondu
			t -= 2.5f;
			spriteLogoSFML.setColor(Color(255, 255, 255, 255 - Uint8(255.f * t)));
		}
		else if (t > 4.f)
		{
			// on quitte la fonction
			break;
		}
		else
		{
			spriteLogoSFML.setColor(Color(255, 255, 255, 0));
		}
		// on dessine
		window.clear(Color::White);
		window.draw(spriteLogoSFML);
		window.display();
	}
}

void menuPrincipal()
{
	// actions possibles
	enum class Commande
	{
		LancerUnePartie,
		Classement,
		Quitter
	};
	// logo ThinkAHead
	Sprite spriteLogoThinkAHead(logoThinkAHead);
	spriteLogoThinkAHead.setPosition(window.getView().getCenter());
	spriteLogoThinkAHead.setOrigin(spriteLogoThinkAHead.getGlobalBounds().width / 2, spriteLogoThinkAHead.getGlobalBounds().height / 2);
	spriteLogoThinkAHead.setScale(1.2f, 1.2f);

	// Boutons et textes sur boutons ainsi que les endroit ou cliquer

	vector<RectangleShape> boutons(3);
	vector<Text> textesBoutons(boutons.size());
	textesBoutons[0] = Text("lancer une partie", font);
	textesBoutons[1] = Text("classement", font);
	textesBoutons[2] = Text("quitter", font);
	for (unsigned int i = 0; i < boutons.size(); i++)
	{
		boutons[i].setSize(Vector2f(240.f, 50.f));
		boutons[i].setPosition(spriteLogoThinkAHead.getPosition() + Vector2f(10.f, 10.f));
		boutons[i].setFillColor(Color(255, 127, 127, 0));
		boutons[i].setOutlineColor(Color(127, 127, 127, 0));
		boutons[i].setOutlineThickness(1);
		boutons[i].setOrigin(boutons[i].getSize().x / 2, 0);
		textesBoutons[i].setPosition(boutons[i].getPosition());
		textesBoutons[i].setOrigin(textesBoutons[i].getGlobalBounds().width / 2, -5.f);
		textesBoutons[i].setFillColor(Color(0, 0, 0, 0));
	}

	Event e;
	Clock c;
	//début de la boucle de jeu
	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case Event::Closed:
			{
				window.close();
				break;
			}
			case Event::MouseButtonPressed:
			{
				switch (e.mouseButton.button)
				{
				case Mouse::Button::Left:
				{
					for (unsigned int i = 0; i < boutons.size(); i++)
					{
						if (boutons[i].getGlobalBounds().contains(Vector2f(e.mouseButton.x, e.mouseButton.y)))
						{
							son.stop();
							son.setBuffer(selectButton);
							son.play();
							switch (Commande(i))
							{
							case Commande::LancerUnePartie:
							{
								window.setMouseCursor(arrow);
								lancerLaPartie();
								break;
							}
							case Commande::Classement:
							{
								classement();
								break;
							}
							case Commande::Quitter:
							{
								window.close();
								return;
							}
							}
						}
					}
					break;
				}
				}
			}
			}
		}
		// début des animations
		float t = c.getElapsedTime().asSeconds() - 2.f;
		if (t < -1.f)
		{
			// apparition du logo Think AHead
			t *= -1;
			t -= 1.f;
			spriteLogoThinkAHead.setColor(Color(255, 255, 255, 255 - Uint8(255 * t)));
		}
		else if (t > -0.5f && t < 0.f)
		{
			// déplacement du logo Think AHead
			spriteLogoThinkAHead.setPosition(window.getView().getCenter() + Vector2f(0.f, -350.f * (1.f - (t * -2))));
			spriteLogoThinkAHead.setColor(Color(255, 255, 255, 255));
		}
		else if (t < 0.25f && t > 0.f)
		{
			// déploiment des boutons
			spriteLogoThinkAHead.setPosition(window.getView().getCenter() + Vector2f(0.f, -350.f));
			for (unsigned int i = 0; i < boutons.size(); i++)
			{
				boutons[i].setPosition(window.getView().getCenter() + Vector2f(0.f, -350.f + 250.f * t * 4));
				textesBoutons[i].setPosition(boutons[i].getPosition());
			}

			for (unsigned int i = 0; i < boutons.size(); i++)
			{
				Color dedans(boutons[i].getFillColor()), dehors(boutons[i].getOutlineColor()), texte(textesBoutons[i].getFillColor());
				dedans.a = Uint8(255);
				dehors.a = Uint8(255);
				texte.a = Uint8(255);
				boutons[i].setFillColor(dedans);
				boutons[i].setOutlineColor(dehors);
				textesBoutons[i].setFillColor(texte);
			}
		}
		else if (t < 0.5f && t > 0.25f)
		{
			// mise en place des boutons
			t -= 0.25f;
			for (unsigned int i = 0; i < boutons.size(); i++)
			{
				if (t * 4 < float(i) / float(boutons.size() - 1))
				{
					boutons[i].setPosition(window.getView().getCenter() + Vector2f(0.f, -100.f  + 150.f * (boutons.size() - 1) * t * 4));
					textesBoutons[i].setPosition(boutons[i].getPosition());
				}
				else
				{
					boutons[i].setPosition(window.getView().getCenter() + Vector2f(0.f, -100.f + 150.f * i));
					textesBoutons[i].setPosition(boutons[i].getPosition());
				}
			}
		}
		else if (t > 0.5f)
		{
			for (unsigned int i = 0; i < boutons.size(); i++)
			{
				boutons[i].setPosition(window.getView().getCenter() + Vector2f(0.f, -100.f + 150.f * i));
				boutons[i].setFillColor(Color(127, 255, 127, 255));
				textesBoutons[i].setPosition(boutons[i].getPosition());
			}
			for (unsigned int i = 0; i < boutons.size(); i++)
			{
				if (boutons[i].getGlobalBounds().contains(Vector2f(Mouse::getPosition(window))))
				{
					window.setMouseCursor(hand);
					break;
				}
				else if (i == boutons.size() - 1)
				{
					window.setMouseCursor(arrow);
				}
			}
		}
		// on dessine
		window.clear(Color::White);
		for (unsigned int i = 0; i < boutons.size(); i++)
		{
			window.draw(boutons[boutons.size() - i - 1]);
			window.draw(textesBoutons[boutons.size() - i - 1]);
		}
		window.draw(spriteLogoThinkAHead);
		window.display();
	}
}

void classement()
{
	RectangleShape retour(Vector2f(225.f, 50.f));
	retour.setFillColor(Color(127, 255, 127));
	retour.setOutlineColor(Color(127, 127, 127));
	retour.setOutlineThickness(1);
	retour.setPosition(window.mapPixelToCoords(Vector2i(20, 20)));

	RectangleShape modeNormal(Vector2f(400.f, 50.f));
	modeNormal.setFillColor(Color(127, 255, 127));
	modeNormal.setOutlineColor(Color(127, 127, 127));
	modeNormal.setOutlineThickness(1);
	modeNormal.setPosition(window.mapPixelToCoords(Vector2i(window.getView().getSize().x / 2 - modeNormal.getSize().x, 100)));

	RectangleShape modeContreLaMontre(Vector2f(400.f, 50.f));
	modeContreLaMontre.setFillColor(Color(255, 127, 127));
	modeContreLaMontre.setOutlineColor(Color(127, 127, 127));
	modeContreLaMontre.setOutlineThickness(1);
	modeContreLaMontre.setPosition(window.mapPixelToCoords(Vector2i(window.getView().getSize().x / 2, 100)));

	vector<Text> texteBoutons;
	texteBoutons.push_back(Text("retour", font));
	texteBoutons.back().setPosition(retour.getPosition() + Vector2f(retour.getSize().x / 2, 15.f));
	texteBoutons.push_back(Text("mode normal", font));
	texteBoutons.back().setPosition(modeNormal.getPosition() + Vector2f(modeNormal.getSize().x / 2, 15.f));
	texteBoutons.push_back(Text("mode contre-la-montre", font));
	texteBoutons.back().setPosition(modeContreLaMontre.getPosition() + Vector2f(modeContreLaMontre.getSize().x / 2, 15.f));

	for (int i = 0; i < texteBoutons.size(); i++)
	{
		texteBoutons[i].setFillColor(Color::Black);
		texteBoutons[i].setOrigin(texteBoutons[i].getGlobalBounds().width / 2, texteBoutons[i].getGlobalBounds().height / 2);
	}

	vector<FloatRect> cliqueBox;
	cliqueBox.push_back(retour.getGlobalBounds());
	cliqueBox.push_back(modeNormal.getGlobalBounds());
	cliqueBox.push_back(modeContreLaMontre.getGlobalBounds());

	enum class Commande
	{
		Retour,
		ModeNormal,
		ModeContreLaMontre
	};

	Event e;
	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case Event::Closed:
			{
				window.close();
				break;
			}
			case Event::MouseButtonPressed:
			{
				switch (e.mouseButton.button)
				{
				case Mouse::Button::Left:
				{

					for (size_t i(0); i < cliqueBox.size(); i++)
					{
						if (cliqueBox[i].contains(Vector2f(e.mouseButton.x, e.mouseButton.y)))
						{
							son.stop();
							switch (Commande(i))
							{
							case Commande::Retour:
							{
								son.setBuffer(backButton);
								son.play();
								return;
							}
							case Commande::ModeNormal:
							{
								if (modeNormal.getFillColor() == Color(255, 127, 127))
								{
									son.setBuffer(changeMode);
									son.play();
									modeNormal.setFillColor(Color(127, 255, 127));
									modeContreLaMontre.setFillColor(Color(255, 127, 127));
								}
								break;
							}
							case Commande::ModeContreLaMontre:
							{
								if (modeContreLaMontre.getFillColor() == Color(255, 127, 127))
								{
									son.setBuffer(changeMode);
									son.play();
									modeContreLaMontre.setFillColor(Color(127, 255, 127));
									modeNormal.setFillColor(Color(255, 127, 127));
								}
								break;
							}
							}
						}
					}
					break;
				}
				}
				break;
			}
			}
		}

		for (size_t i(0); i < cliqueBox.size(); i++)
		{
			if (cliqueBox[i].contains(Vector2f(Mouse::getPosition(window))))
			{
				window.setMouseCursor(hand);
				break;
			}
			else
			{
				window.setMouseCursor(arrow);
			}
		}

		window.clear(Color::White);
		window.draw(retour);
		window.draw(modeNormal);
		window.draw(modeContreLaMontre);
		for (unsigned int i = 0; i < texteBoutons.size(); i++)
		{
			window.draw(texteBoutons[i]);
		}
		window.display();
	}
}

void lancerLaPartie()
{
	// commande pour les boutons
	enum class Commande
	{
		MoinsDeCases,
		PlusDeCases,
		LancerLeJeu,
		RetourAuMenu,
		ModeNormal,
		ModeContreLaMontre
	};

	// cadre pour l'affichage d'une valeur
	RectangleShape afficheur(Vector2f(100.f, 100.f));
	afficheur.setPosition(window.mapPixelToCoords(Vector2i(int(window.getView().getSize().x / 2), 100)));
	afficheur.setOrigin(afficheur.getSize().x / 2, 0);
	afficheur.setFillColor(Color(0, 0, 0, 32));
	afficheur.setOutlineColor(Color::Black);
	afficheur.setOutlineThickness(1);

	//endroit ou cliquer
	vector<FloatRect> cliqueBox;

	// créations des flèche bleus
	vector<vector<Vertex>> fleches;
	fleches.push_back(vector<Vertex>());
	fleches.back().push_back(Vertex(afficheur.getPosition() + Vector2f(-60, 0), Color(0, 0, 80)));
	fleches.back().push_back(Vertex(afficheur.getPosition() + Vector2f(-60, 100), Color(0, 0, 80)));
	fleches.back().push_back(Vertex(afficheur.getPosition() + Vector2f(-85, 50), Color(0, 0, 80)));
	cliqueBox.push_back(FloatRect(afficheur.getPosition() + Vector2f(-85, 0), Vector2f(25, 100)));

	fleches.push_back(vector<Vertex>());
	fleches.back().push_back(Vertex(afficheur.getPosition() + Vector2f(60, 0), Color::Blue));
	fleches.back().push_back(Vertex(afficheur.getPosition() + Vector2f(60, 100), Color::Blue));
	fleches.back().push_back(Vertex(afficheur.getPosition() + Vector2f(85, 50), Color::Blue));
	cliqueBox.push_back(FloatRect(afficheur.getPosition() + Vector2f(60, 0), Vector2f(25, 100)));

	// crétion des boutons
	RectangleShape boutonLancerJeu(Vector2f(225.f, 50.f));
	boutonLancerJeu.setOrigin(boutonLancerJeu.getSize().x / 2, 0);
	boutonLancerJeu.setPosition(window.mapPixelToCoords(Vector2i(int(window.getView().getSize().x / 2), int(window.getView().getSize().y - 70.f))));
	boutonLancerJeu.setFillColor(Color(0, 255, 0, 127));
	boutonLancerJeu.setOutlineColor(Color(127, 127, 127));
	boutonLancerJeu.setOutlineThickness(1);
	cliqueBox.push_back(boutonLancerJeu.getGlobalBounds());

	RectangleShape boutonRetour(Vector2f(225.f, 50.f));
	boutonRetour.setFillColor(Color(127, 255, 127));
	boutonRetour.setOutlineColor(Color(127, 127, 127));
	boutonRetour.setOutlineThickness(1);
	boutonRetour.setPosition(window.mapPixelToCoords(Vector2i(20, 20)));
	cliqueBox.push_back(boutonRetour.getGlobalBounds());

	RectangleShape boutonModeNormal(Vector2f(300.f, 50.f));
	boutonModeNormal.setFillColor(Color(127, 255, 127));
	boutonModeNormal.setOutlineColor(Color(127, 127, 127));
	boutonModeNormal.setOutlineThickness(1);
	boutonModeNormal.setOrigin(boutonModeNormal.getSize().x / 2, 0);
	boutonModeNormal.setPosition(window.mapPixelToCoords(Vector2i(int(window.getView().getSize().x / 2 - boutonModeNormal.getSize().x / 2), 400)));
	cliqueBox.push_back(boutonModeNormal.getGlobalBounds());

	RectangleShape boutonModeContreLaMontre(boutonModeNormal);
	boutonModeContreLaMontre.move(boutonModeNormal.getSize().x, 0);
	boutonModeContreLaMontre.setFillColor(Color(255, 127, 127));
	cliqueBox.push_back(boutonModeContreLaMontre.getGlobalBounds());

	Text retour("retour", font);
	retour.setPosition(boutonRetour.getPosition() + Vector2f(boutonRetour.getSize().x / 2, 15.f));
	retour.setOrigin(retour.getGlobalBounds().width / 2, retour.getGlobalBounds().height / 2);
	retour.setFillColor(Color::Black);

	unsigned int nombreCases(5);

	// initialisation des textes
	Text lancerLeJeu("lancer le jeu", font, 30U);
	lancerLeJeu.setPosition(boutonLancerJeu.getPosition() + Vector2f(0, boutonLancerJeu.getSize().y / 2));
	lancerLeJeu.setOrigin(lancerLeJeu.getGlobalBounds().width / 2, lancerLeJeu.getCharacterSize() / 1.75f);
	lancerLeJeu.setFillColor(Color::Black);

	Text afficheValeur(to_string(nombreCases), font, 50U);
	afficheValeur.setPosition(afficheur.getPosition() + Vector2f(0, afficheur.getSize().y / 2));
	afficheValeur.setOrigin(afficheValeur.getGlobalBounds().width / 1.75f, afficheValeur.getCharacterSize() / 1.75f);
	afficheValeur.setFillColor(Color::Black);

	Text tailleDeLaGrille("taille de la grille :", font);
	tailleDeLaGrille.setFillColor(Color::Black);
	tailleDeLaGrille.setOrigin(tailleDeLaGrille.getGlobalBounds().width / 2, 0);
	tailleDeLaGrille.setPosition(window.mapPixelToCoords(Vector2i(int(window.getView().getSize().x / 2), 60)));

	Text texteModeNormal("normal", font);
	texteModeNormal.setFillColor(Color::Black);
	texteModeNormal.setOrigin(texteModeNormal.getGlobalBounds().width / 2, -5.f);
	texteModeNormal.setPosition(boutonModeNormal.getPosition());

	Text texteModeContreLaMontre("contre-la-montre", font);
	texteModeContreLaMontre.setFillColor(Color::Black);
	texteModeContreLaMontre.setOrigin(texteModeContreLaMontre.getGlobalBounds().width / 2, -5.f);
	texteModeContreLaMontre.setPosition(boutonModeContreLaMontre.getPosition());

	ModeDeJeu mode(ModeDeJeu::Normal);

	Event e;
	// début de la boucle de jeu
	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case Event::Closed:
			{
				window.close();
				return;
			}
			case Event::MouseButtonPressed:
			{
				switch (e.mouseButton.button)
				{
				case Mouse::Button::Left:
				{
					for (unsigned int i = 0; i < cliqueBox.size(); i++)
					{
						// vérifie que l'endroit peut être cliquer
						if (cliqueBox[i].contains(Vector2f(e.mouseButton.x, e.mouseButton.y)))
						{
							// vérifie que le bouton gauche de la souris a été pressé
							son.stop();
							switch (Commande(i))
							{
							case Commande::MoinsDeCases:
							{
								// enlève des cases à la future grille
								if (nombreCases > 5)
								{
									son.setBuffer(changeParameter);
									son.play();
									nombreCases--;
									if (nombreCases == 5)
									{
										for (unsigned int i = 0; i < fleches[0].size(); i++)
										{
											fleches[0][i].color = Color(0, 0, 80);
										}
										window.setMouseCursor(arrow);
									}
									for (unsigned int i = 0; i < fleches[1].size(); i++)
									{
										fleches[1][i].color = Color(0, 0, 255);
									}
								}
								break;
							}
							case Commande::PlusDeCases:
							{
								// ajoute des cases à la future grille
								if (nombreCases < 10)
								{
									son.setBuffer(changeParameter);
									son.play();
									nombreCases++;
									if (nombreCases == 10)
									{
										for (unsigned int i = 0; i < fleches[1].size(); i++)
										{
											fleches[1][i].color = Color(0, 0, 80);
										}
										window.setMouseCursor(arrow);
									}
									for (unsigned int i = 0; i < fleches[0].size(); i++)
									{
										fleches[0][i].color = Color(0, 0, 255);
									}
								}
								break;
							}
							case Commande::LancerLeJeu:
							{
								// lance le jeu avec les paramètres donnés
								window.setMouseCursor(arrow);
								son.setBuffer(selectButton);
								son.play();
								vector<Joueur> joueurs = selectionNomJoueurs(2);
								if (joueurs.empty())
								{
									break;
								}
								while (jeu(nombreCases, joueurs, mode) == 0)
								{
									for (unsigned int i(0); i < joueurs.size(); i++)
									{
										joueurs[i].scoreZero();
										joueurs[i].chrono().reset();
									}
								}
								window.setMouseCursor(arrow);
								return;
							}
							case Commande::RetourAuMenu:
							{
								window.setMouseCursor(arrow);
								son.setBuffer(backButton);
								son.play();
								return;
							}
							case Commande::ModeNormal:
							{
								if (boutonModeNormal.getFillColor() != Color(127, 255, 127))
								{
									son.setBuffer(changeMode);
									son.play();
									mode = ModeDeJeu::Normal;
									boutonModeContreLaMontre.setFillColor(Color(255, 127, 127));
									boutonModeNormal.setFillColor(Color(127, 255, 127));
								}
								break;
							}
							case Commande::ModeContreLaMontre:
							{
								if (boutonModeContreLaMontre.getFillColor() != Color(127, 255, 127))
								{
									son.setBuffer(changeMode);
									son.play();
									mode = ModeDeJeu::ContreLaMontre;
									boutonModeContreLaMontre.setFillColor(Color(127, 255, 127));
									boutonModeNormal.setFillColor(Color(255, 127, 127));
								}
								break;
							}
							}
							afficheValeur.setString(to_string(nombreCases));
							afficheValeur.setOrigin(afficheValeur.getGlobalBounds().width / 1.75f, afficheValeur.getCharacterSize() / 1.75f);

							tailleDeLaGrille.setPosition(window.mapPixelToCoords(Vector2i(int(window.getView().getSize().x / 2), 60)));
							break;
						}
					}
					break;
				}
				}
				break;
			}
			}
		}

		for (unsigned int i = 0; i < cliqueBox.size(); i++)
		{
			// vérifie que l'endroit peut être cliquer
			if (cliqueBox[i].contains(Vector2f(Mouse::getPosition(window))))
			{
				Commande commande = Commande(i);
				if (!(commande == Commande::PlusDeCases && nombreCases >= 10)
					|| !(commande == Commande::MoinsDeCases && nombreCases <= 5))
				{
					window.setMouseCursor(hand);
				}
				break;
			}
			if (i == cliqueBox.size() - 1)
			{
				window.setMouseCursor(arrow);
			}
		}
		// on dessine
		window.clear(Color::White);
		for_each(fleches.begin(), fleches.end(), [](const vector<Vertex> &vertices) {window.draw(vertices.data(), vertices.size(), PrimitiveType::Triangles); });
		window.draw(tailleDeLaGrille);
		window.draw(afficheur);
		window.draw(afficheValeur);
		window.draw(boutonLancerJeu);
		window.draw(lancerLeJeu);
		window.draw(boutonRetour);
		window.draw(retour);
		window.draw(boutonModeNormal);
		window.draw(texteModeNormal);
		window.draw(boutonModeContreLaMontre);
		window.draw(texteModeContreLaMontre);
		window.display();
	}
}

vector<Joueur> selectionNomJoueurs(unsigned int nbJoueurs)
{
	// initialisation des cadres d'ecritures ainsi que du texte et du bouton permettant de confirmer les noms donnés
	vector<Text> nomsJoueurs(nbJoueurs), nomDuJoueurX(nbJoueurs);
	vector<RectangleShape> caseEcriture(nbJoueurs);
	vector<FloatRect> cliqueBox(nbJoueurs);
	for (unsigned int i = 0; i < nbJoueurs; i++)
	{
		float decalage(-100 * int(nbJoueurs) + 200.f * i);

		nomsJoueurs[i].setString("");
		nomsJoueurs[i].setFont(font);
		nomsJoueurs[i].setFillColor(Color::Black);
		nomsJoueurs[i].setPosition(window.mapPixelToCoords(Vector2i(int(window.getView().getSize().x / 2), int(window.getView().getSize().y / 2))) + Vector2f(0.f, 0.f + decalage));
		nomsJoueurs[i].setOrigin(nomsJoueurs[i].getGlobalBounds().width / 2, 0);

		nomDuJoueurX[i].setString(L"Entrez le nom du joueur " + to_string(i + 1));
		nomDuJoueurX[i].setFont(font);
		nomDuJoueurX[i].setFillColor(Color::Black);
		nomDuJoueurX[i].setPosition(window.mapPixelToCoords(Vector2i(int(window.getView().getSize().x / 2), int(window.getView().getSize().y / 2))) + Vector2f(0.f, -50.f + decalage));
		nomDuJoueurX[i].setOrigin(nomDuJoueurX[i].getGlobalBounds().width / 2, 0);

		caseEcriture[i].setSize(Vector2f(325.f, 50.f));
		caseEcriture[i].setFillColor(Color::White);
		caseEcriture[i].setOutlineColor(Color(127, 127, 127));
		caseEcriture[i].setOutlineThickness(1);
		caseEcriture[i].setPosition(window.mapPixelToCoords(Vector2i(int(window.getView().getSize().x / 2), int(window.getView().getSize().y / 2))) + Vector2f(0.f, 15.f + decalage));
		caseEcriture[i].setOrigin(caseEcriture[i].getSize().x / 2, caseEcriture[i].getSize().y / 2);

		cliqueBox[i] = caseEcriture[i].getGlobalBounds();
	}

	RectangleShape boutonConfirmer(Vector2f(225.f, 50.f));
	boutonConfirmer.setFillColor(Color(255, 127, 127));
	boutonConfirmer.setOutlineColor(Color(127, 127, 127));
	boutonConfirmer.setOutlineThickness(1);
	boutonConfirmer.setOrigin(boutonConfirmer.getSize().x / 2, 0);
	boutonConfirmer.setPosition(window.getView().getSize().x / 2, window.getView().getSize().y - 100.f);

	Text confirmer("confirmer", font);
	confirmer.setPosition(boutonConfirmer.getPosition() + Vector2f(0, boutonConfirmer.getSize().y / 2));
	confirmer.setOrigin(confirmer.getGlobalBounds().width / 2, confirmer.getCharacterSize() / 1.75f);
	confirmer.setFillColor(Color::Black);

	RectangleShape boutonRetour(Vector2f(225.f, 50.f));
	boutonRetour.setFillColor(Color(127, 255, 127));
	boutonRetour.setOutlineColor(Color(127, 127, 127));
	boutonRetour.setOutlineThickness(1);
	boutonRetour.setPosition(window.mapPixelToCoords(Vector2i(20, 20)));

	Text retour("retour", font);
	retour.setPosition(boutonRetour.getPosition() + Vector2f(boutonRetour.getSize().x / 2, 15.f));
	retour.setOrigin(retour.getGlobalBounds().width / 2, retour.getGlobalBounds().height / 2);
	retour.setFillColor(Color::Black);

	vector<Text>::iterator ptrText(nomsJoueurs.begin());

	FloatRect rectText(ptrText->getGlobalBounds());
	Vertex barre[2] =
	{
		Vertex(ptrText->getPosition() - ptrText->getOrigin() + Vector2f(rectText.width + 5.f, -5.f), Color::Black),
		Vertex(ptrText->getPosition() - ptrText->getOrigin() + Vector2f(rectText.width + 5.f, float(ptrText->getCharacterSize() + 5.f)), Color::Black)
	};
	int clique(0);
	Clock c;
	Event e;
	// début de la boucle de jeu
	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case Event::Closed:
			{
				window.close();
				return vector<Joueur>();
			}
			case Event::MouseButtonPressed:
			{
				switch (e.mouseButton.button)
				{
				case Mouse::Button::Left:
				{
					son.stop();
					// vérifie si le bouton peut être cliquer
					if (boutonConfirmer.getGlobalBounds().contains(Vector2f(e.mouseButton.x, e.mouseButton.y)) && boutonConfirmer.getFillColor() == Color(127, 255, 127))
					{
						window.setMouseCursor(arrow);
						son.setBuffer(selectButton);
						son.play();
						vector<Joueur> joueurs;
						for (unsigned int i = 0; i < nomsJoueurs.size(); i++)
						{
							joueurs.push_back(Joueur(nomsJoueurs[i].getString()));
						}
						return joueurs;
					}
					else if (boutonRetour.getGlobalBounds().contains(Vector2f(e.mouseButton.x, e.mouseButton.y)))
					{
						window.setMouseCursor(arrow);
						son.setBuffer(backButton);
						son.play();
						return vector<Joueur>();
					}
					for (unsigned int i = 0; i < cliqueBox.size(); i++)
					{
						if (cliqueBox[i].contains(Vector2f(e.mouseButton.x, e.mouseButton.y)))
						{
							ptrText = nomsJoueurs.begin() + i;
							if (ptrText != nomsJoueurs.end())
							{
								ptrText->setOrigin(ptrText->getGlobalBounds().width / 2, 0);

								rectText = ptrText->getGlobalBounds();
								barre[0].position = ptrText->getPosition() - ptrText->getOrigin() + Vector2f(rectText.width + 5.f, -5.f);
								barre[1].position = ptrText->getPosition() - ptrText->getOrigin() + Vector2f(rectText.width + 5.f, float(ptrText->getCharacterSize() + 5.f));
							}
							break;
						}
					}
					break;
				}
				}
				break;
			}
			case Event::TextEntered:
			{
				// vérifie si du texte est entré
				if (ptrText != nomsJoueurs.end())
				{
					// filtre les charactère non désirable
					if (ptrText->getString().getSize() < 14 && ((e.text.unicode >= 32 && e.text.unicode < 127) || e.text.unicode >= 161))
					{
						ptrText->setString(ptrText->getString() + e.text.unicode);
						ptrText->setOrigin(ptrText->getGlobalBounds().width / 2, 0);

						rectText = ptrText->getGlobalBounds();
						barre[0].position = ptrText->getPosition() - ptrText->getOrigin() + Vector2f(rectText.width + 5.f, -5.f);
						barre[1].position = ptrText->getPosition() - ptrText->getOrigin() + Vector2f(rectText.width + 5.f, float(ptrText->getCharacterSize() + 5.f));
					}
					if (find_if(nomsJoueurs.begin(), nomsJoueurs.end(), [](const Text &a) {return a.getString() == ""; }) == nomsJoueurs.end())
					{
						// colore le bouton en vert si tous les noms sont donnés...
						boutonConfirmer.setFillColor(Color(127, 255, 127));
						if (boutonConfirmer.getGlobalBounds().contains(Vector2f(Mouse::getPosition(window))))
						{
							window.setMouseCursor(hand);
						}
					}
					else
					{
						// ...ou le colore en rouge dans le cas contraire.
						boutonConfirmer.setFillColor(Color(255, 127, 127));
						if (cliqueBox.back().contains(Vector2f(Mouse::getPosition(window))))
						{
							window.setMouseCursor(arrow);
						}
					}
				}
				break;
			}
			case Event::KeyPressed:
			{
				switch (e.key.code)
				{
				case Keyboard::Key::Tab:
				case Keyboard::Key::Enter:
				{
					// passe à l'autre cadre si la touche Tab ou Enter est pressée
					if (ptrText != nomsJoueurs.end())
					{
						ptrText++;
						if (ptrText != nomsJoueurs.end())
						{
							rectText = ptrText->getGlobalBounds();
							barre[0].position = ptrText->getPosition() - ptrText->getOrigin() + Vector2f(rectText.width + 5.f, -5.f);
							barre[1].position = ptrText->getPosition() - ptrText->getOrigin() + Vector2f(rectText.width + 5.f, float(ptrText->getCharacterSize() + 5.f));
						}
					}
					break;
				}
				case Keyboard::Key::BackSpace:
				{
					// efface le dernier charactère si la touche BackSpace est pressée
					if (ptrText != nomsJoueurs.end())
					{
						if (ptrText->getString().getSize() != 0)
						{
							String temp(ptrText->getString());
							temp.erase(temp.getSize() - 1);
							ptrText->setString(temp);

							ptrText->setOrigin(ptrText->getGlobalBounds().width / 2, 0);

							rectText = ptrText->getGlobalBounds();
							barre[0].position = ptrText->getPosition() - ptrText->getOrigin() + Vector2f(rectText.width + 5.f, -5.f);
							barre[1].position = ptrText->getPosition() - ptrText->getOrigin() + Vector2f(rectText.width + 5.f, float(ptrText->getCharacterSize() + 5.f));
						}
					}
					break;
				}
				}
			}
			}
		}

		if ((boutonConfirmer.getGlobalBounds().contains(Vector2f(Mouse::getPosition(window))) && boutonConfirmer.getFillColor() == Color(127, 255, 127))
			|| boutonRetour.getGlobalBounds().contains(Vector2f(Mouse::getPosition(window))))
		{
			window.setMouseCursor(hand);
		}
		else
		{
			for (unsigned int i = 0; i < cliqueBox.size(); i++)
			{
				// vérifie si l'endroit peut être cliquer
				if (cliqueBox[i].contains(Vector2f(Mouse::getPosition(window))))
				{
					window.setMouseCursor(textbar);
					break;
				}
				else if (i == cliqueBox.size() - 1)
				{
					window.setMouseCursor(arrow);
				}
			}
		}
		// on dessine
		window.clear(Color::White);
		for (unsigned int i = 0; i < nbJoueurs; i++)
		{
			window.draw(nomDuJoueurX[i]);
			window.draw(caseEcriture[i]);
			window.draw(nomsJoueurs[i]);
		}
		window.draw(boutonConfirmer);
		window.draw(confirmer);
		window.draw(boutonRetour);
		window.draw(retour);
		if (c.getElapsedTime().asMilliseconds() % 1000 <= 500 && ptrText != nomsJoueurs.end())
		{
			window.draw(barre, 2, PrimitiveType::Lines);
		}
		window.display();
	}
	return vector<Joueur>();
}

int jeu(int tailleGrille, vector<Joueur>& joueurs, ModeDeJeu modeDeJeu)
{
	// on créé la grille

	Grille grille(tailleGrille, font, 80.f);
	grille.setPosition(Vector2f(window.getSize() / 2U) - grille.getSize() / 2.f);

	auto tempsEnTexte = [&modeDeJeu](const Time& temps)
	{
		if (modeDeJeu == ModeDeJeu::ContreLaMontre)
		{
			return String(to_string(temps.asMilliseconds() / 1000 / 60) + " : " + to_string((temps.asMilliseconds() / 1000) % 60) + " : " + to_string(temps.asMilliseconds() % 1000));
		}
		return String();
	};

	// on initialise un itérateur sur le tableau de joueurs donné en paramètre et on créé une map pour afficher le nom des joueurs
	vector<Joueur>::iterator it(joueurs.begin());
	map<String, Text> affichageJoueur;
	for (unsigned int i = 0; i < joueurs.size(); i++)
	{
		string nom(joueurs[i].getNom());
		affichageJoueur[nom] = Text(String(joueurs[i].getNom()) + " : " + to_string(joueurs[i].getScore()), font);
		affichageJoueur[nom].setPosition(window.mapPixelToCoords(Vector2i(0, 35 * i)));
		affichageJoueur[nom].setFillColor(Color(255, 255, 255));
		affichageJoueur[nom].setOutlineColor(Color::Black);
		affichageJoueur[nom].setOutlineThickness(1);
	}
	affichageJoueur[it->getNom()].setFillColor(Color(127, 255, 127));

	map<String, Text> affichageChrono;
	if (modeDeJeu == ModeDeJeu::ContreLaMontre)
	{
		for (unsigned int i = 0; i < joueurs.size(); i++)
		{
			string nom(joueurs[i].getNom());
			affichageChrono[nom] = Text(tempsEnTexte(joueurs[i].chrono().getTime()), font);
			affichageChrono[nom].setPosition(window.mapPixelToCoords(Vector2i(300, 35 * i)));
			affichageChrono[nom].setFillColor(Color(255, 255, 255));
			affichageChrono[nom].setOutlineColor(Color::Black);
			affichageChrono[nom].setOutlineThickness(1);
		}
	}

	// on génère l'aléatoire
	default_random_engine seedVertical((unsigned int)chrono::high_resolution_clock::now().time_since_epoch().count());
	uniform_int_distribution<short> aleaVertical{ 0, 1 };
	bool vertical = (bool)aleaVertical(seedVertical);
	bool tourPossible = true;

	vector<RectangleShape> valide;
	const float cellule(grille.getTailleCellule());

	auto refaireZoneAutorisee = [&](bool vertical, int id)
	{
		// on refait les zones vertes et rouges sur la grille
		pair<int*, int*> indice;
		int i(0);
		if (vertical)
		{
			indice.first = &id;
			indice.second = &i;
		}
		else
		{
			indice.first = &i;
			indice.second = &id;
		}
		valide.clear();
		for (; i < tailleGrille; i++)
		{
			if (grille.VerifierValidite(*indice.first, *indice.second))
			{
				valide.push_back(RectangleShape(Vector2f(cellule, cellule)));
				valide.back().setPosition(grille.getPosition() + grille.getTailleCellule() * Vector2f((float)*indice.first, (float)*indice.second));
				valide.back().setFillColor(Color(0, 255, 0, 127));
			}
		}
		if (valide.empty())
		{
			tourPossible = false;
		}
	};

	pair<int, int> posDansGrille(int((Mouse::getPosition(window).x - (int)grille.getPosition().x) / cellule), int((Mouse::getPosition(window).y - (int)grille.getPosition().y) / cellule));

	auto dansZoneAutorisee = [&](const Vector2f& pos)
	{
		// on vérifie si la zone cible peut être sélectionnée
		for (unsigned int i = 0; i < valide.size(); i++)
		{
			if (valide[i].getGlobalBounds().contains(Vector2f(pos)))
			{
				return true;
			}
		}
		return false;
	};
	RectangleShape selection(Vector2f(cellule, cellule));
	selection.setFillColor(Color(0, 0, 0, 80));
	selection.setPosition(grille.getPosition() + Vector2f((float)posDansGrille.first * cellule, (float)posDansGrille.second * cellule));

	default_random_engine seedLigne((unsigned int)chrono::high_resolution_clock::now().time_since_epoch().count());
	uniform_int_distribution<int> aleaLigne{ 0, tailleGrille - 1 };
	refaireZoneAutorisee(vertical, aleaLigne(seedLigne));
	vector<RectangleShape> nonDispo;

	Text *texteChangeant(0);
	if (modeDeJeu == ModeDeJeu::ContreLaMontre)
	{
		texteChangeant = &affichageChrono[it->getNom()];
	}

	Event e;
	// début de la boucle de jeu
	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case Event::Closed:
			{
				window.close();
				break;
			}
			case Event::MouseMoved:
			{
				posDansGrille.first = int((e.mouseMove.x - (int)grille.getPosition().x) / cellule);
				posDansGrille.second = int((e.mouseMove.y - (int)grille.getPosition().y) / cellule);
				selection.setPosition(grille.getPosition() + Vector2f((float)posDansGrille.first * cellule, (float)posDansGrille.second * cellule));
				break;
			}
			case Event::MouseButtonPressed:
			{
				switch (e.mouseButton.button)
				{
				case Mouse::Button::Left:
				{
					// vérifie si le bouton gauche de la souris a été pressé
					if (dansZoneAutorisee(Vector2f(e.mouseButton.x, e.mouseButton.y)) && grille.choisirCase(posDansGrille.first, posDansGrille.second, *it))
					{
						it->chrono().pause();
						if (modeDeJeu == ModeDeJeu::ContreLaMontre)
						{
							texteChangeant->setString(tempsEnTexte(it->chrono().getTime()));
							texteChangeant->setFillColor(Color(255, 255, 255));
						}
						affichageJoueur[it->getNom()].setString(String(it->getNom()) + " : " + to_string(it->getScore()));
						affichageJoueur[it->getNom()].setFillColor(Color(255, 255, 255));
						it++;
						if (it == joueurs.end())
						{
							it = joueurs.begin();
						}
						if (modeDeJeu == ModeDeJeu::ContreLaMontre)
						{
							texteChangeant = &affichageChrono[it->getNom()];
							texteChangeant->setFillColor(Color(127, 255, 127));
						}
						affichageJoueur[it->getNom()].setFillColor(Color(127, 255, 127));
						// on refait la zone
						vertical = vertical ? false : true;
						refaireZoneAutorisee(vertical, vertical ? posDansGrille.first : posDansGrille.second);
						nonDispo.push_back(RectangleShape(Vector2f(cellule, cellule)));
						nonDispo.back().setFillColor(Color(255, 0, 0, 127));
						nonDispo.back().setPosition(selection.getPosition());
						if (tourPossible)
						{
							it->chrono().start();
						}
						else
						{
							RenderTexture ecran;
							ecran.create(window.getSize().x, window.getSize().y);
							affichageJoueur[it->getNom()].setFillColor(Color(255, 255, 255));
							if (modeDeJeu == ModeDeJeu::ContreLaMontre)
							{
								texteChangeant->setFillColor(Color(255, 255, 255));
							}
							ecran.setView(window.getView());
							ecran.clear(Color(255, 255, 255));
							for_each(nonDispo.begin(), nonDispo.end(), [&ecran](const RectangleShape &rect) { ecran.draw(rect); });
							for_each(affichageJoueur.begin(), affichageJoueur.end(), [&ecran](const pair<String, Text> &texte) { ecran.draw(texte.second); });
							for_each(affichageChrono.begin(), affichageChrono.end(), [&ecran](const pair<String, Text> &texte) { ecran.draw(texte.second); });
							ecran.draw(grille);
							ecran.display();

							vector<Bonus> bonus;
							if (modeDeJeu == ModeDeJeu::ContreLaMontre)
							{
								Joueur *meilleurTemps(&joueurs[0]);
								for (size_t i(0); i < joueurs.size(); i++)
								{
									if (joueurs[i].chrono().getTime() < meilleurTemps->chrono().getTime())
									{
										meilleurTemps = &joueurs[i];
									}
								}
								bonus.push_back(Bonus(*meilleurTemps, "plus rapide", 5));
							}

							return victoire(ecran.getTexture(), joueurs, bonus);
						}
					}
					break;
				}
				}
				break;
			}
			}
		}
		
		if (modeDeJeu == ModeDeJeu::ContreLaMontre)
		{
			const int temps(it->chrono().getTime().asMilliseconds());
			texteChangeant->setString(to_string(temps / 1000 / 60) + " : " + to_string((temps / 1000) % 60) + " : " + to_string(temps % 1000));
		}
		// on dessine
		window.clear(Color::White);
		for_each(valide.begin(), valide.end(), [](const RectangleShape &rect) { window.draw(rect); });
		for_each(nonDispo.begin(), nonDispo.end(), [](const RectangleShape &rect) { window.draw(rect); });
		if (dansZoneAutorisee(Vector2f(Mouse::getPosition(window))))
		{
			window.draw(selection);
		}
		for_each(affichageJoueur.begin(), affichageJoueur.end(), [](const pair<String, Text> &texte) { window.draw(texte.second); });
		for_each(affichageChrono.begin(), affichageChrono.end(), [](const pair<String, Text> &texte) { window.draw(texte.second); });
		window.draw(grille);
		window.display();
	}
	return 1;
}

int victoire(const Texture& ecran, vector<Joueur>& joueurs, const vector<Bonus>& bonus)
{
	window.setView(window.getDefaultView());
	Sprite fond(ecran);
	fond.setPosition(window.mapPixelToCoords(Vector2i(0, 0)));
	fond.setColor(Color(80, 80, 80));

	Text partieTerminee(L"partie terminée", font, 50U);
	partieTerminee.setPosition(window.mapPixelToCoords(Vector2i(window.getView().getSize().x / 2, 0)));
	partieTerminee.setOrigin(partieTerminee.getGlobalBounds().width / 2, partieTerminee.getGlobalBounds().height / 2);
	partieTerminee.setFillColor(Color(255, 255, 255));
	partieTerminee.setOutlineColor(Color(0, 0, 0));
	partieTerminee.setOutlineThickness(1);

	Text decompteDesScores(L"décompte des scores", font, 50U);
	decompteDesScores.setStyle(Text::Style::Underlined);
	decompteDesScores.setPosition(window.mapPixelToCoords(Vector2i(window.getView().getSize().x / 2, 0)));
	decompteDesScores.setOrigin(decompteDesScores.getGlobalBounds().width / 2, decompteDesScores.getGlobalBounds().height / 2);
	decompteDesScores.setFillColor(Color(255, 255, 255));
	decompteDesScores.setOutlineColor(Color(0, 0, 0));
	decompteDesScores.setOutlineThickness(1);

	Text joueurXAGagne("", font, 50U);
	joueurXAGagne.setFillColor(Color(255, 255, 255));
	joueurXAGagne.setOutlineColor(Color(0, 0, 0));
	joueurXAGagne.setOutlineThickness(1);

	Text typeBonus("", font, 50U);
	typeBonus.setPosition(window.mapPixelToCoords(Vector2i(window.getView().getSize().x / 2, 0)));
	typeBonus.setFillColor(Color(127, 255, 127));
	typeBonus.setOutlineColor(Color(0, 0, 0));
	typeBonus.setOutlineThickness(1);
	typeBonus.setOrigin(typeBonus.getGlobalBounds().width / 2, typeBonus.getGlobalBounds().height / 2);

	Text scoreBonus("", font, 50U);
	scoreBonus.setPosition(window.mapPixelToCoords(Vector2i(window.getView().getSize().x / 2, 0)));
	scoreBonus.setFillColor(Color(255, 255, 255));
	scoreBonus.setOutlineColor(Color(0, 0, 0));
	scoreBonus.setOutlineThickness(1);
	scoreBonus.setOrigin(scoreBonus.getGlobalBounds().width / 2, scoreBonus.getGlobalBounds().height / 2);

	vector<Text> textesJoueurs;
	for (unsigned int i(0); i < joueurs.size(); i++)
	{
		textesJoueurs.push_back(Text(joueurs[i].getNom() + " : " + to_string(joueurs[i].getScore()), font, 50U));
	}
	for (unsigned int i(0); i < textesJoueurs.size(); i++)
	{
		textesJoueurs[i].setPosition(window.mapPixelToCoords(Vector2i(window.getView().getSize().x / 2, 0)));
		textesJoueurs[i].setFillColor(Color(255, 255, 255));
		textesJoueurs[i].setOutlineColor(Color(0, 0, 0));
		textesJoueurs[i].setOutlineThickness(1);
		textesJoueurs[i].setOrigin(textesJoueurs[i].getGlobalBounds().width / 2, textesJoueurs[i].getGlobalBounds().height / 2);
	}

	vector<RectangleShape> boutons;
	vector<Text> textesBoutons;
	textesBoutons.push_back(Text("rejouer", font));
	textesBoutons.push_back(Text("menu principal", font));
	for (unsigned int i(0); i < textesBoutons.size(); i++)
	{
		boutons.push_back(RectangleShape(Vector2f(225.f, 50.f)));
		boutons[i].setOrigin(boutons[i].getSize().x / 2, 0);
		boutons[i].setFillColor(Color(127, 255, 127));
		boutons[i].setOutlineColor(Color(127, 127, 127));
		boutons[i].setOutlineThickness(1);
		textesBoutons[i].setPosition(window.getView().getSize().x / 2, 0);
		textesBoutons[i].setOrigin(textesBoutons[i].getGlobalBounds().width / 2, -5.f);
		textesBoutons[i].setFillColor(Color(0, 0, 0));
	}

	for (unsigned int i(0); i < joueurs.size(); i++)
	{
		textesJoueurs[i].move(0, 100.f * i + 300.f);
	}
	joueurXAGagne.setPosition(textesJoueurs.back().getPosition() + Vector2f(0, 100.f));
	partieTerminee.move(0, window.getView().getSize().y / 2);
	decompteDesScores.move(0.f, textesJoueurs[0].getPosition().y - 200.f);
	textesBoutons[0].move(-300.f, 900.f);
	textesBoutons[1].move(300.f, 900.f);
	for (unsigned int i(0); i < boutons.size(); i++)
	{
		boutons[i].setPosition(textesBoutons[i].getPosition());
	}
	typeBonus.move(0, decompteDesScores.getPosition().y + 70.f);

	vector<FloatRect> cliqueBox;
	bool bonusRecu(false);

	Event e;
	Clock c;
	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case Event::Closed:
			{
				window.close();
				break;
			}
			case Event::MouseButtonPressed:
			{
				switch (e.mouseButton.button)
				{
				case Mouse::Button::Left:
				{
					for (unsigned int i(0); i < cliqueBox.size(); i++)
					{
						if (cliqueBox[i].contains(Vector2f(e.mouseButton.x, e.mouseButton.y)))
						{
							window.setMouseCursor(arrow);
							return i;
						}
					}
					break;
				}
				}
				break;
			}
			}
		}

		for (unsigned int i(0); i < cliqueBox.size(); i++)
		{
			if (cliqueBox[i].contains(Vector2f(Mouse::getPosition(window))))
			{
				window.setMouseCursor(hand);
				break;
			}
			if (i == cliqueBox.size() - 1)
			{
				window.setMouseCursor(arrow);
			}
		}
		
		window.clear(Color(255, 255, 255));
		window.draw(fond);
		const float t(c.getElapsedTime().asSeconds());
		if (t < 2.f)
		{
			window.draw(partieTerminee);
		}
		else if (t > 2.f && t < 4.f)
		{
			window.draw(decompteDesScores);
			for (unsigned int i(0); i < textesJoueurs.size(); i++)
			{
				if (t - 2.f > float(i + 1) / textesJoueurs.size())
				{
					window.draw(textesJoueurs[i]);
				}
			}
		}
		else if (t > 4.f && t < 5.f + bonus.size())
		{
			window.draw(decompteDesScores);
			for (unsigned int i(0); i < textesJoueurs.size(); i++)
			{
				window.draw(textesJoueurs[i]);
			}
			if (t < 4.f + bonus.size())
			{
				const Bonus &actuel(bonus[unsigned int(t - 4.f)]);
				typeBonus.setString(actuel.type);
				typeBonus.setOrigin(typeBonus.getGlobalBounds().width / 2, typeBonus.getGlobalBounds().height / 2);
				window.draw(typeBonus);
				if ((t - 4.f) - int(t - 4.f) < 0.7f)
				{
					bonusRecu = false;
					Text& pos(*find_if(textesJoueurs.begin(), textesJoueurs.end(), [&actuel](const Text& a) {return a.getString().find(actuel.cible.getNom()) != String::InvalidPos; }));
					scoreBonus.setString(String((actuel.score < 0) ? "" : "+") + to_string(actuel.score));
					scoreBonus.setOrigin(scoreBonus.getGlobalBounds().width / 2, scoreBonus.getGlobalBounds().height / 2);
					scoreBonus.setPosition(pos.getPosition() + Vector2f(pos.getGlobalBounds().width + 10.f, 0));
					window.draw(scoreBonus);
				}
				else if (!bonusRecu)
				{
					actuel.cible.ajouterScore(actuel.score);
					for (size_t i(0); i < textesJoueurs.size(); i++)
					{
						textesJoueurs[i].setString(joueurs[i].getNom() + " : " + to_string(joueurs[i].getScore()));
						textesJoueurs[i].setOrigin(textesJoueurs[i].getGlobalBounds().width / 2, textesJoueurs[i].getGlobalBounds().height / 2);
					}
					bonusRecu = true;
				}
			}
		}
		else if (t > 5.f + bonus.size() && t < 6.f + bonus.size())
		{
			window.draw(decompteDesScores);
			if (textesJoueurs[0].getFillColor() == Color(255, 255, 255))
			{
				for (unsigned int i(0); i < textesJoueurs.size(); i++)
				{
					textesJoueurs[i].setFillColor(Color(255, 127, 127));
				}
				unsigned int it(0);
				for (unsigned int i(0); i < joueurs.size(); i++)
				{
					it = joueurs[it].getScore() < joueurs[i].getScore() ? i : it;
				}
				textesJoueurs[it].setFillColor(Color(127, 255, 127));
				joueurXAGagne.setString(joueurs[it].getNom() + L" a remporté la partie.");
				joueurXAGagne.setOrigin(joueurXAGagne.getGlobalBounds().width / 2, joueurXAGagne.getGlobalBounds().height / 2);
			}
			for (unsigned int i(0); i < textesJoueurs.size(); i++)
			{
				window.draw(textesJoueurs[i]);
			}
			window.draw(joueurXAGagne);
		}
		else if (t > 6.f + bonus.size())
		{
			window.draw(decompteDesScores);
			if (cliqueBox.empty())
			{
				for (unsigned int i(0); i < boutons.size(); i++)
				{
					cliqueBox.push_back(boutons[i].getGlobalBounds());
				}
			}
			for (unsigned int i(0); i < textesJoueurs.size(); i++)
			{
				window.draw(textesJoueurs[i]);
			}
			for (unsigned int i(0); i < boutons.size(); i++)
			{
				window.draw(boutons[i]);
				window.draw(textesBoutons[i]);
			}
			window.draw(joueurXAGagne);
		}
		window.display();
	}
	return 1;
}