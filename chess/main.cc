#include "board.h"
#include "computer.h"
#include <string>
#include <iostream>

std::vector<int> convert(std::string coord) {
	std::vector<int> vec = { (int)(coord[0] - 'a'), coord[1] - '0' - 1 };
	return vec;
}
int main() {
	//std::shared_ptr<Xwindow> xw = std::make_shared<Xwindow>();
	//Scoreboard sb;
	float bwin = 0.0;
	float wwin = 0.0;
	while (!std::cin.eof()) {
		std::string input = "";
		std::shared_ptr<Board> game = std::make_shared<Board>();
		Computer::setBoard(game);
		game->setDefault();
		bool gameRunning = false;
		bool breakLoop = false;
		while (std::cin >> input) {
			if (input == "game") {
				std::string p1 = "";
				std::string p2 = "";
				std::cin >> p1;
				std::cin >> p2;
				game->setPlayers(p1, p2);
				gameRunning = true;
				game->printBoard();
				/*game->displayConsts(xw);
				game->displayPieces(xw);
				game->displayMessages(xw, gameRunning);*/
				//sb.setScores(wwin, bwin);
				//sb.displayCurrScore(xw);
			}
			else if (input == "setup" && !gameRunning) {
				game->printBoard();
				/*game->displayConsts(xw);
				game->displayPieces(xw);*/
				std::string op;
				while (true) {
					std::cin >> op;
					if (op == "+") {
						char piece;
						std::cin >> piece;
						std::string pos;
						std::cin >> pos;
						std::vector<int> vec = convert(pos);
						game->place(piece, vec[0], vec[1], false);
						game->printBoard();
						//game->displayPieces(xw);
					}
					if (op == "-") {
						std::string pos;
						std::cin >> pos;
						std::vector<int> vec = convert(pos);
						game->place('t', vec[0], vec[1], false);
						game->printBoard();
						//game->displayPieces(xw);
					}
					if (op == "=") {
						std::string p;
						std::cin >> p;
						game->setCurrPlayer(p == "white" ? 1 : -1);
					}
					if (op == "done") {
						//game->displayMessages(xw, gameRunning);
						if (game->setupCheck()) {
							std::cout << "Finished setup" << std::endl;
							bool status = false;
							if (game->stalemate()) {
								std::cout << (game->getCurrPlayer() == 1 ? "White" : "Black") << " is in stalemate." << std::endl;
								wwin += 0.5;
								bwin += 0.5;
								status = true;
								breakLoop = true;
							}
							game->setCurrPlayer(-game->getCurrPlayer());
							if (game->stalemate() && !status) {
								std::cout << (game->getCurrPlayer() == 1 ? "White" : "Black") << " is in stalemate." << std::endl;
								wwin += 0.5;
								bwin += 0.5;
								breakLoop = true;
							}
							game->setCurrPlayer(-game->getCurrPlayer());
							gameRunning = false;
							break;
						}
						std::cout << "Invalid setup" << std::endl;
					}
				}
				if (breakLoop) {
					break;
				}
			}
			else if (input == "move" && gameRunning) {
				bool moveMade = false;
				if (game->getCurrPlayer() == 1 && game->isWhiteHuman() || game->getCurrPlayer() == -1 && game->isBlackHuman()) {
					std::string pos1;
					std::string pos2;
					std::cin >> pos1 >> pos2;
					std::vector<int> vec1 = convert(pos1);
					std::vector<int> vec2 = convert(pos2);
					if (game->makeMove(vec1[0], vec1[1], vec2[0], vec2[1])) {
						moveMade = true;
						if (game->checkForPromote(vec2[0], vec2[1])) {
							std::cout << "Enter piece to promote pawn to." << std::endl;
							char p;
							while (true) {
								std::cin >> p;
								if ((game->getCurrPlayer() == 1 && p < 'a' || game->getCurrPlayer() == -1 && p >= 'a') && tolower(p) != 'p') {
									game->place(p, vec2[0], vec2[1], true);
									break;
								}
								else {
									if (tolower(p) == 'p') {
										std::cout << "Please enter 'q', 'b', 'n', or 'r' to promote." << std::endl;
									}
									else {
										std::cout << "Please enter an uppercase piece if white or lowercase if black." << std::endl;
									}
								}
							}
						}
					}
				}
				else {
					std::vector<int> vec = Computer::computeMove();
					game->makeMove(vec[0], vec[1], vec[2], vec[3]);
					if (vec.size() == 5) {
						game->place(vec[4], vec[2], vec[3], true);
					}
					moveMade = true;
				}
				if (moveMade) {
					game->setCurrPlayer(-game->getCurrPlayer());
					game->printBoard();
					/*game->displayPieces(xw);
					game->displayMessages(xw, gameRunning);*/
					if (game->checkmate()) {
						std::cout << (game->getCurrPlayer() == 1 ? "White" : "Black") << " is in checkmate." << std::endl;
						if (game->getCurrPlayer() == 1) {
							bwin += 1;
						}
						else {
							wwin += 1;
						}
						//sb.setScores(wwin, bwin);
						//sb.displayCurrScore(xw);
						breakLoop = true;
						break;
					}
					else if (game->stalemate()) {
						std::cout << (game->getCurrPlayer() == 1 ? "White" : "Black") << " is in stalemate." << std::endl;
						bwin += 0.5;
						wwin += 0.5;
						//sb.setScores(wwin, bwin);
						//sb.displayCurrScore(xw);
						breakLoop = true;
						break;
					}
					else if (game->getCurrPlayer() == 1 && game->isCheck(game->getWkPos()[0], game->getWkPos()[1], 1)) {
						std::cout << "White king in check." << std::endl;
						std::cout << (game->getCurrPlayer() == 1 ? "White" : "Black") << "'s turn." << std::endl;
					}
					else if (game->getCurrPlayer() == -1 && game->isCheck(game->getBkPos()[0], game->getBkPos()[1], -1)) {
						std::cout << "Black king in check." << std::endl;
						std::cout << (game->getCurrPlayer() == 1 ? "White" : "Black") << "'s turn." << std::endl;
					}
					else {
						std::cout << (game->getCurrPlayer() == 1 ? "White" : "Black") << "'s turn." << std::endl;
					}
				}
			}
			else if (input == "resign" && gameRunning) {
				if (game->getCurrPlayer() == 1) {
					bwin += 1;
				}
				else {
					wwin += 1;
				}
				breakLoop = true;
				break;
			}
		}
	}
	std::cout << "Final Score: " << std::endl;
	std::cout << "White: " << wwin << std::endl;
	std::cout << "Black: " << bwin << std::endl;
	//sb.setScores(wwin, bwin);
	//sb.displayFinScore(xw);
}
