#include "stdafx.h"
#include "dealer.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include "ai.h"

Dealer::Dealer(int players) {
	// Generate a set amount of players (default 4)
	_players.push_back(new Player(false, "PLAYER"));
	for (int i = 0; i < std::max(std::min(players - 1, 7), 1); i++) {
		_players.push_back(new Player(true, std::string("BOT ") + std::to_string(i + 1)));
	}
	std::cout
		<< "##################################################" << std::endl
		<< "#       #####    ####  ####  ##        ##       ##" << std::endl
		<< "#  ####  ###  ##  ###  ###  ###  ########  ####  #" << std::endl
		<< "#  ####  ##  ####  ##  ##  ####  ########  ####  #" << std::endl
		<< "#  ####  ##  ####  ##  #  #####  ########  ###   #" << std::endl
		<< "#       ###  ####  ##     #####      ####      ###" << std::endl
		<< "#  ########  ####  ##  ##  ####  ########  ####  #" << std::endl
		<< "#  ########  ####  ##  ###  ###  ########  ####  #" << std::endl
		<< "#  #########  ##  ###  ####  ##  ########  ####  #" << std::endl
		<< "#  ##########    ####  ####  ##        ##  ####  #" << std::endl
		<< "##################################################" << std::endl << std::endl;
	std::cout << "DEALER > set up table for " << _players.size() << " players" << std::endl;
}

Dealer::Dealer() : Dealer{ 4 } {}

Dealer::~Dealer() {
	delete _deck;
	for (int i = 0; i < _players.size(); ++i)
		delete _players[i];  
}

void Dealer::newDeck() {
	// Generate a new deck for a new round, and delete old deck object if exists
	if (_deck != nullptr)
		delete _deck;
	std::cout << std::endl << "DEALER > creating deck..." << std::endl;
	_deck = new Deck();
	std::cout << "DEALER > shuffling deck..." << std::endl;
	_deck->shuffle();
} 

int Dealer::getMin() {  
	return _min_bet;
}

void Dealer::dealCards() {
	std::cout << "DEALER > dealing cards..." << std::endl;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < _players.size(); j++) {
			_players[j]->addCard(_deck->topDeck());
		}
	}
}

void Dealer::bettingRound(int min_bet, int bet_unit) {
	// Displays information relevant to round and calls betting method for each player object
	bool *raised = new bool[_players.size()](); // Dynamic array to keep track of which players have already raised
	Player *you = _players[0];
	std::cout << std::endl << "-+++- BETTING ROUND -+++-" << std::endl;
	std::cout << std::endl << "YOUR HAND: " << std::endl;
	you->printHand();
	std::cout << std::endl << std::endl << std::setw(36) << std::left << "OPTIONS:" << std::setw(24) << "BALANCE:" << "MINIMUM BET:" << std::endl
		<< std::setw(36) << "[1] call [2] raise [3] fold" << std::setw(24) << you->getBalance() << min_bet << std::endl;

	int i = 0;
	// Gets betting choices for each player, human or ai.
	// If a player raises, then loop through all players again until no more raises are done
	while (i < _players.size()) {
		int player_action = -1;
		if (_players[i]->isActive()) {
			player_action = _players[i]->bet(min_bet, bet_unit, raised[i]);
			switch (player_action) {
			default:
			case -1: //fold
				_players[i]->setBalance(-min_bet + bet_unit);
				_players[i]->setActive(false);
				std::cout << _players[i]->getName() << " folds" << std::endl;
				_pot += min_bet - bet_unit;
				break;
			case 0: //call
				std::cout << _players[i]->getName() << " matches current bet" << std::endl;
				break;
			case 1: //raise
				std::cout << _players[i]->getName() << " raises bet to " << min_bet + bet_unit << std::endl;
				min_bet += bet_unit;
				raised[i] = true;
				i = -1;
				break;
			}
		}
		i++;
	}

	delete[] raised;

	std::cout << std::endl << "DEALER > betting finalized..." << std::endl << "ACTIVE PLAYERS: ";
	for (int i = 0; i < _players.size(); i++) {
		// Display all players who are still active and adjust pot and balances accordingly
		if (_players[i]->isActive()) {
			std::cout << "[" << _players[i]->getName() << "] ";
			_pot += min_bet;
			_players[i]->setBalance(-min_bet);
		}
	}
	std::cout << std::endl << "CURRENT POT: " << _pot << std::endl;
	_min_bet = min_bet;
}

void Dealer::mulliganRound() {
	// Iterate through all players and call card draw method
	std::cout << std::endl << "-+++- DRAW ROUND -+++-" << std::endl << std::endl <<
		"DEALER > enter the index of cards you want to replace (max 3), enter 0 when done" << std::endl;
	for (int i = 0; i < _players.size(); i++) {
		if (_players[i]->isActive()) {
			_players[i]->mulligan(_deck);
		}
		else {
			std::cout << _players[i]->getName() << " is no longer in the round" << std::endl;
		}
	}
}

void Dealer::showdown() {
	Player *you = _players[0];
	std::vector<Player*> winners; // Create a vector of players to store winners (in case of tie)
	int max_score = 0;
	std::cout << std::endl << "-+++- SHOWDOWN -+++-" << std::endl;
	// Iterate through all players that have not folded to determine hand with highest value
	for (int i = 0; i < _players.size(); i++) {
		if (_players[i]->isActive()) {
			// A lot of outputting text
			std::cout << std::endl << _players[i]->getName() << "'S HAND:" << std::endl;
			_players[i]->printHand();
			std::cout << std::endl;
			int score = _players[i]->scoreHand();
			std::cout << Ai::evalHand(score) << std::endl;
			// Add player to winners if their hand score matches the max
			// Otherwise clear winners and put player in if their score is higher than current max
			if (score == max_score) {
				winners.push_back(_players[i]);
			}
			else if (score > max_score) {
				winners.clear();
				winners.push_back(_players[i]);
				max_score = score;
			}
		}
	}

	// Roud done; distribute pot between all players with highest hand score
	std::cout << std::endl << "DEALER > determining winner(s)..." << std::endl;
	for (int i = 0; i < winners.size(); i++) {
		winners[i]->setBalance(_pot / winners.size());
		std::cout << std::setw(20) << winners[i]->getName() + " wins " + std::to_string(_pot / winners.size()) + "!";
	}

	// Display information for end of round and reset variables
	std::cout << std::endl << std::endl << "-+++- END OF ROUND -+++-" << std::endl
		<< std::endl << "BALANCES:" << std::endl;
	for (int i = 0; i < _players.size(); i++) {
		_players[i]->setActive(true);
		_players[i]->clearHand();
		std::cout << std::setw(15) << _players[i]->getName() + ": " + std::to_string(_players[i]->getBalance());
	}
	_pot = 0;
	_min_bet = 5;

	// Check if player wants to/is able to continue playing, restart if so, quit if not
	char c = ' ';
	if (you->getBalance() <= 0) {
		std::cout << std::endl << std::endl << "OH NO, YOU ARE BANKRUPT! (any key to quit): ";
		std::cin >> c;
		exit(0);
	}
	else {
		while (c != 'y' && c != 'n') {
			std::cout << std::endl << std::endl << "WOULD YOU LIKE TO CONTINUE PLAYING? (y/n): ";
			std::cin >> c;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		if (c == 'y') {
			std::cout << std::endl << std::endl << "-+++- NEW ROUND -+++-" << std::endl;
		}
		else {
			exit(0);
		}
	}
}