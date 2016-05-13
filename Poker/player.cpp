#include "stdafx.h"
#include "player.h"

#include <iostream>
#include "ai.h"

Player::Player() {
	_ai = false;
	_name = "BOT";
}

Player::Player(bool ai, std::string name) {
	_ai = ai;
	_name = name;
}

Player::~Player() {} // Nothing to destroy (yet)

bool Player::isActive() { // Returns whether or not the player has folded in the current round
	return _active;
}

void Player::setActive(bool active) {
	_active = active;
}

std::string Player::getName() {
	return _name;
}

int Player::getBalance() {
	return _bal;
}

void Player::setBalance(int bal) {
	_bal += bal;
}

int Player::scoreHand() {
	return Ai::evalScore(&_hand);
}

void Player::addCard(Card card) {
	_hand.push_back(card);
}

void Player::printHand() {
	for (int i = 0; i < _hand.size(); i++) {
		std::cout << "[" << _hand[i].toString() << "] ";
	}
}

int Player::bet(int min, int increment, bool raised) {
	// Called to place a bet, calls bet evaluation method if AI, otherwise prompt player with options to bet
	if (_ai) {
		return Ai::chooseBet(Ai::evalScore(&_hand), 3, raised);
	}
	else {
		char c = ' ';
		// Prompt player for input
		while (c != '1' && c != '2' && c != '3') {
			std::cout << std::endl << _name << " > take action: ";
			std::cin >> c;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		switch (c) {
		default:
			return -1;
		case '1':
			// Match bet if possible, otherwise fold
			if (_bal < min) {
				std::cout << "NOPE > balance too low to match bet" << std::endl;
				return -1;
			}
			else {
				return 0;
			}
		case '2':
			// Raise if possible, otherwise call or fold
			if (_bal < min) {
				std::cout << "NOPE > balance too low to raise or match" << std::endl;
				return -1;
			}
			else if (_bal < min + increment) {
				std::cout << "NOPE > balance too low to raise" << std::endl;
				return 0;
			}
			else if (raised) {
				std::cout << "NOPE > already raised this round" << std::endl;
				return 0;
			}
			else {
				return 1;
			}
		case '3':
			return -1;
		}
	}
}

void Player::mulligan(Deck *deck) {
	// Use ai class to choose cards to if not player, otherwise prompt player to switch cards
	if (_ai) {
		// Chooses to replace a card using ai up to three times, keeping track of cards that have been already replaced 
		int r[3] = { 0 };
		int replaced = 0;
		int limit = rand() % 4; // Max of 3 cards replaced
		for (int i = 0; i < limit; i++) {
			_hand[Ai::chooseReplace(r)] = deck->topDeck();
			replaced += 1;
		}
		std::cout << std::endl << _name << " replaced " << replaced << " card(s)." << std::endl;
	}
	else {
		// Display current hand
		for (int i = 0; i < _hand.size(); i++) {
			std::cout << "[" << i + 1 << "][" << _hand[i].toString() << "] ";
		}
		int replaced = 0;
		int index = 1;
		std::cout << std::endl;
		// Prompt for the index of the card to replace, loop until three cards have been selected or user inputs otherwise
		while (replaced < 3 && index > 0) {
			std::cout << std::endl << _name << " > replace card #" << replaced + 1 << ": ";
			while (!(std::cin >> index)) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "try again: ";
			}
			if (index == 0) {
				break;
			}
			// If index is specified, return card to deck and set new card to top card of deck using topdeck()
			else {
				deck->returnCard(_hand[(index - 1) % _hand.size()]);
				_hand[(index - 1) % _hand.size()] = deck->topDeck();
				replaced += 1;
			}
		}
		std::cout << std::endl << "NEW HAND [replaced " << replaced << " card(s)]: " << std::endl;
		printHand();
		std::cout << std::endl;
	}
}

void Player::clearHand() {
	_hand.clear();
}