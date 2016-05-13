#ifndef CARD_H
#define CARD_H

#include <string>

class Card {
private:
	static const char* _faces[];
	static const char* _suits[];
	int _suit;  
	int _face;
public:
	Card();
	~Card();
	Card(int suit, int face);
	int getFace();
	int getSuit();
	std::string toString();
};

#endif