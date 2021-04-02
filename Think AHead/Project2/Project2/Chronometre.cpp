#include "Chronometre.h"

Chronometre::Chronometre() : m_pause(true)
{

}

void Chronometre::start()
{
	if (m_pause)
	{
		m_clock.restart();
		m_pause = false;
	}
}

void Chronometre::pause()
{
	if (!m_pause)
	{
		m_time += m_clock.restart();
		m_pause = true;
	}
}

void Chronometre::stop()
{
	m_time = sf::Time();
	m_pause = true;
}

bool Chronometre::isPaused()
{
	return m_pause;
}

void Chronometre::reset()
{
	m_time = sf::Time();
	m_clock.restart();
}

const sf::Time& Chronometre::getTime()
{
	if (m_pause)
	{
		return m_time;
	}
	return m_time + m_clock.getElapsedTime();
}
