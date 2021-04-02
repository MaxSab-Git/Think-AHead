#pragma once
#include <SFML/System.hpp>
class Chronometre
{
public:
	Chronometre();
	void start();
	void pause();
	void stop();
	bool isPaused();
	void reset();
	const sf::Time& getTime();

private:
	sf::Clock m_clock;
	sf::Time m_time;
	bool m_pause;
};

