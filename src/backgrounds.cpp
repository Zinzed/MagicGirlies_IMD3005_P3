#include "backgrounds.h"
#include "CVC.h"

void Backgrounds::setBgs(string start, string instrucP1, string instrucP2, string endScreen)
{
	m_startScreen.load(start);
	m_startScreen.resize(1920, 1020);

	m_instrucScreenP1.load(instrucP1);
	m_instrucScreenP1.resize(1920, 1020);

	m_instrucScreenP2.load(instrucP2);
	m_instrucScreenP2.resize(1920, 1020);

	m_endScreen.load(endScreen);
	m_endScreen.resize(1920, 1020);
}

void Backgrounds::drawStart()
{
	m_startScreen.draw(0, 0);
}

void Backgrounds::drawInstrucP1()
{
	m_instrucScreenP1.draw(0, 0);
}

void Backgrounds::drawInstrucP2()
{
	m_instrucScreenP2.draw(0, 0);
}

void Backgrounds::drawEnd()
{
	m_endScreen.draw(0, 0);
}