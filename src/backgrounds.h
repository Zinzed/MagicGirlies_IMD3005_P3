#pragma once
#include "ofMain.h";

class Backgrounds
{
	private:
		ofImage m_startScreen;
		ofImage m_instrucScreenP1;
		ofImage m_instrucScreenP2;
		ofImage m_endScreen;

	public:
		void setBgs(string start, string instrucP1, string instrucP2, string endScreen);
		void drawStart();
		void drawInstrucP1();
		void drawInstrucP2();
		void drawEnd();
};
