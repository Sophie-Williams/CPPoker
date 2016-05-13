#include "stdafx.h"
#include "ai.h"

#include <algorithm> // std::sort
#include <iostream>


const char* Ai::_hand_states[] = { "HIGH CARD", "ONE PAIR", "TWO PAIR", "THREE OF A KIND", "STRAIGHT", "FLUSH", "FULL HOUSE", "FOUR OF A KIND", "STRAIGHT FLUSH", "ROYAL FLUSH" };

std::string Ai::evalHand(int score) {
	// Convert base 10 score to base 14 and return hand type + score in a string
	char base14[16];
	_itoa_s(score, base14, 16, 14);
	return _hand_states[score / int(pow(_BASE, 5))] + std::string(" (") + std::string(base14) + std::string(")");
}

bool compareCards(Card a, Card b) {
	return (a.getFace() > b.getFace());
}

int Ai::evalScore(std::vector<Card> *hand) {
	// The card evaluation system assigns a base 14 integer to each card in the hand, along with a main score for the hand type.
	// Giving extra weight to what is evaluated first (hand type, quads, triples, pairs, cards in descending order, etc.)
	// e.g: score of 3220365 is 5DB861, or 5 13 11 8 6 1
	// The first digit (5) is the value of the hand itself (a straight)
	// the rest are espective values of each of the highest cards in descending order
	// This hand would contain [A Q 9 7 2] of the same suit.
	// For full house etc. the leftmost digits will be the pair/three evauated first
	// e.g. [2 2 2 K K] will be 6111CC rather than 6CC111 because the three are evaluated before the pair

	// Sort hand by face value (will be needed to find straights/determine high cards)
	std::sort(hand->begin(), hand->end(), compareCards);

	// Create a histogram to hold the number of each face value in order to determine pairs/triples/quads
	int hist[14] = { 0 };
	for (std::vector<Card>::iterator it = hand->begin(); it != hand->end(); ++it) {
		hist[it->getFace()] += 1;
	}

	int score = 0;
	int count[5] = { 0 };
	int iterated[5] = { 0 };
	bool straight = false, flush = false;

	// Iterate backwards through histogram to identify combos, then set the high card to the face value of the last combo
	for (int i = 0; i < 14; i++) {
		switch (hist[i]) {
		case 1:
			count[1] += 1;
			break;
		case 2:
			count[2] += 1;
			break;
		case 3:
			count[3] += 1;
			break;
		case 4:
			count[4] += 1;
			break;
		}
	}

	// Assign scores to cards according to their weights - this is used to determine tiebreakers
	// e.g: if  players have 2 pairs, we will need to compare the two highest pairs, followed by the second pair, followed by the last card.
	// This is eZ since the hand is already sorted, so we just need to iterate through it backwards to go from hi to low face value
	for (int i = 13; i >= 0; i--) {
		switch (hist[i]) {
		case 1:
			if (count[2] == 2 || count[4] == 1) {
				score += i;
			}
			else if (count[3] == 1) {
				score += pow(_BASE, 1 - iterated[1])*i;
				iterated[1] += 1;
			}
			else if (count[2] == 1) {
				score += pow(_BASE, 2 - iterated[1])*i;
				iterated[1] += 1;
			}
			else {
				score += pow(_BASE, 4 - iterated[1])*i;
				iterated[1] += 1;
			}
			break;
		case 2:
			if (iterated[2] == 1 || count[3] == 1) {
				score += pow(_BASE, 3)*i;
			}
			else {
				score += pow(_BASE, 4)*i;
				iterated[2] += 1;
			}
			break;
		case 3:
			score += pow(_BASE, 4)*i;
			break;
		case 4:
			score += pow(_BASE, 4)*i;
			break;
		}
	}
	// If no pairs etc. check for flushes and straights
	if (count[2] == 0 && count[3] == 0 && count[4] == 0) {
		if ((*hand)[0].getFace() == (*hand)[4].getFace() + 4) {
			// Check for conventional straight
			score += (*hand)[0].getFace() * pow(_BASE, 4);
			straight = true;
		}
		else if ((*hand)[0].getFace() == 13 && (*hand)[1].getFace() == 4) {
			// Check for straight with A-2-3-4-5 with 5 as high card
			score += (*hand)[1].getFace() * pow(_BASE, 4);
			straight = true;
		}
	}

	// Check for flush by iterating through hand to look for cards that are different from first
	flush = true;
	for (int i = 1; i < 5; i++) {
		if ((*hand)[i].getSuit() != (*hand)[0].getSuit()) {
			flush = false;
		}
	}
	// Give score to flush based on the value of the cards in descending order (highest card gets more weight)
	if (flush) {
		score = 0;
		for (int i = 0; i < 5; i++) {
			score += (*hand)[i].getFace()*pow(_BASE, 4 - i);
		}
	}

	// Give score to hand based on type
	// in ascending order: "HIGH CARD", "ONE PAIR", "TWO PAIRS", "THREE OF A KIND", "STRAIGHT", "FLUSH", "FULL HOUSE", "FOUR OF A KIND", "STRAIGHT FLUSH", "ROYAL FLUSH"
	const int HAND_BONUS = pow(_BASE, 5); // Hand bonus is the first/highest digit of our base-14 number.

	if (straight && flush) {
		if ((*hand)[4].getFace() == 9) { // Royal flush
			score += 9 * HAND_BONUS;
		}
		else { // Straight flush
			score += 8 * HAND_BONUS;
		}
	}
	else if (count[4] == 1) { // Four of a kind
		score += 7 * HAND_BONUS;
	}
	else if (count[3] == 1 && count[2] == 1) { // Full house
		score += 6 * HAND_BONUS;
	}
	else if (flush) { // Flush
		score += 5 * HAND_BONUS;
	}
	else if (straight) { // Straight
		score += 4 * HAND_BONUS;
	}
	else if (count[3] == 1) { // Three of a kind
		score += 3 * HAND_BONUS;
	}
	else if (count[2] == 2) { // Two pairs
		score += 2 * HAND_BONUS;
	}
	else if (count[2] == 1) { // One pair
		score += HAND_BONUS;
	}

	return score;
}

int Ai::chooseReplace(int *replaced) {
	// Currently uses disgusting RNG; TODO: make actual ai for replace
	int roll = rand() % 5;
	while (std::find(replaced, replaced + 3, roll) == replaced + 3) {
		roll = rand() % 5;
	}
	return roll;
}

int Ai::chooseBet(int score, int aggro, bool raised) {
	// Uses weighted RNG to roll raises and folds based on hand score
	// e.g: 100% raise if hand is royal flush, significantly lower for high card, etc.
	// Will roll n times based on the aggressiveness of the bot (usually 3)
	// Will (very rarely) fold if raise roll fails, depending on aggressiveness
	int n = 0;
	int roll;
	if (!raised) {
		while (n < aggro) {
			roll = rand() % 9 * pow(_BASE, 5) + 1;
			if (score > roll) {
				return 1;
			}
			n += 1;
		}
	}
	roll = rand() % (30 / aggro); // Fold once every (30 / aggro level)
	if (roll == 0) {
		return -1;
	}

	return 0;
}
