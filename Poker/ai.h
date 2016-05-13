#ifndef AI_H
#define AI_H

#include <string>
#include <vector>
#include "card.h"

class Ai {
private:
	static const int _BASE = 14;
	static const char* _hand_states[];
public:
	static std::string evalHand(int score);
	static int evalScore(std::vector<Card> *hand);
	static int chooseReplace(int *replaced);
	static int chooseBet(int score, int aggro, bool raised);
};

#endif

