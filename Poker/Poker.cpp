#include "stdafx.h"
#include <iostream>
#include <time.h>

#include "dealer.h"

int main() {
	srand(time(NULL));
	int players = 4;
	Dealer *dealer;
	// Prompt player to enter number of players
	std::cout << "NUMBER OF PLAYERS (2 - 8): ";
	if (std::cin >> players) {
		dealer = new Dealer(players);
	}
	else {
		dealer = new Dealer(); 
	}

	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	

	// Main loop to run game indefinitely/until player stops
	while (true) {
		dealer->newDeck();
		dealer->dealCards();
		dealer->bettingRound(dealer->getMin(), 5);
		dealer->mulliganRound();
		dealer->bettingRound(dealer->getMin(), 5);
		dealer->showdown();
	}

	delete dealer; // We should never reach this point anyway
    return 0;
}

