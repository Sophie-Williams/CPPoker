#include "stdafx.h"
#include "card.h"

const char* Card::_faces[] = { "0", "2", "3", "4", "5", "6", "7", "8", "9", "10", "jack", "queen", "king", "ace" };
const char* Card::_suits[] = { "0", "diamonds", "clubs", "hearts", "spades" };

Card::Card() {
	_suit = 0;
	_face = 0;
}

Card::~Card() {} // Nothing to destroy (yet)

Card::Card(int face, int suit) {
	_suit = suit;
	_face = face;
}

int Card::getFace() {
	return _face;
}

int Card::getSuit() {
	return _suit;
}

std::string Card::toString() {
	return (_face == 0 || _suit == 0) ?
		std::string("joker") : _faces[_face] + std::string(" of ") + _suits[_suit];
}