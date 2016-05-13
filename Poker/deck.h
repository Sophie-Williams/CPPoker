#ifndef DECK_H
#define DECK_H

#include <vector>
#include "card.h"

class Deck {
private:
	std::vector<Card> _cards;
public:
	Deck();
	~Deck();
	Card topDeck();
	void returnCard(Card card);
	void shuffle();
};

#endif