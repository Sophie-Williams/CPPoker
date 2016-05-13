#ifndef DEALER_H
#define DEALER_H

#include <vector>
#include "deck.h"
#include "player.h"

class Dealer {
private:
	Deck *_deck;
	std::vector<Player*> _players;
	int _pot = 0;
	int _min_bet = 5;
public:
	Dealer();
	Dealer(int players);
	~Dealer();
	void newDeck();
	//void dealCard(Player p);
	void dealCards();
	int getMin();
	void bettingRound(int min_bet, int bet_unit);
	void mulliganRound();
	void showdown();
};

#endif

