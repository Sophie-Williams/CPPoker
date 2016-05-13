#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "card.h"
#include "deck.h"

class Player {
private:
	std::vector<Card> _hand;
	int _bal = 1000;
	bool _active = true;
	bool _ai;
	std::string _name;
public:
	Player();   
	Player(bool ai, std::string name);
	~Player();
	bool isActive();
	void setActive(bool active);
	std::string getName();
	int getBalance();
	void setBalance(int bal);
	void addCard(Card card);
	void printHand();
	int scoreHand();
	void clearHand();
	void mulligan(Deck *deck);
	int bet(int min, int increment, bool raised);
};

#endif

