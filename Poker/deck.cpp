#include "stdafx.h"
#include "deck.h"

Deck::Deck() {
	for (int i = 1; i <= 13; i++) {
		for (int j = 1; j <= 4; j++) {
			Card card(i, j);
			_cards.push_back(card);
		}
	}
}

Card Deck::topDeck() {
	// Returns the last card in the deck and removes it from the vector
	Card c;
	if (_cards.size() > 0) {
		c = _cards.back();
		_cards.pop_back();
	} else {
		c = Card();
	}
	return c;
}

void Deck::shuffle() {
	// Fisher-Yates algorithm for unbiased random shuffling
	int size = _cards.size();
	for (int i = 0; i < size; i++) {
		int r = i + rand() % (size - i);
		std::swap(_cards[i], _cards[r]);
	}
}

void Deck::returnCard(Card card) {
	_cards.insert(_cards.begin(), card);
}

Deck::~Deck() {
	//nothing to destroy (vectors are great)
}
