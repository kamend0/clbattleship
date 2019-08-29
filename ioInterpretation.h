//
//  ioInterpretation.h
//  Battleship 1.2
//
//  Created by Kollin Mendoza on 8/7/19.
//  Copyright © 2019 km. All rights reserved.
//

#ifndef ioInterpretation_h
#define ioInterpretation_h
#include <fstream>
#include <cmath>
#include "assets.h"


// --------------------------------------------------------------------------------------------------------------------------------
/*
        - Output functions -
 printTxtFile : utility for showing the contents of text files
 welcome : print formatted welcome message
 clearScreen : ...clear(s)Screen
 showInterface : combines clearScreen, as well as board and infoBar methods to show full gamestate to players
*/
// --------------------------------------------------------------------------------------------------------------------------------


void printTxtFile(std::string filename) {
    std::ifstream f(filename);
    if (f.is_open()) std::cout << f.rdbuf();
    f.close();
}

void clearScreen() {
    for (int i = 0; i < 128; i++) std::cout << '\n';
}

std::string tolower(std::string str) {
    // utility function to make all chars in string lowercase
    std::string returnStr;
    for (int i = 0; i < str.length(); i++) returnStr += tolower(str[i]);
    return returnStr;
}

void showInterface(board g, player a, player b, infoBar s, char behavior, int debug) {
    clearScreen();
    
    switch (behavior) {
        case 'n':   // no behavior
            break;
        default:    // apply appropriate behavior (infoBar response)
            s.respond(g, a, b, behavior);
    }
    
    g.showBoard(debug);
    s.showScores(a, b);
}


// --------------------------------------------------------------------------------------------------------------------------------
/*
        - Player input interpretation functions -
 getName : simply polls the player for their name and returns it as a string
 getMove : general function for polling player input for strike, and ship placement
 interpretMoveInput : takes string arg and returns the corresponding coordinate object
 checkCoordinateValidity : checks if the returned coordinate is valid
 placeShips : for two player games; let player input placement for ships
 */
// --------------------------------------------------------------------------------------------------------------------------------


std::string getName(int playerNum) {
    std::string nm;
    std::cout << '\n';
    
    if (playerNum == 1) std::cout << "player 1 will be the striker. enter a name for player 1: ";
    else std::cout << "player 2 will be the concealer. enter a name for player 2: ";
    
    std::getline(std::cin, nm);
    return nm;
}

std::string getStrikeOrder(player p) {
    std::string move;
    std::cout << p.name << ", enter a coordinate to strike: ";
    std::getline(std::cin, move);
    return move;
}

std::string getPositionOrder(player p, char q, battleship b) {
    /*
     char q controls flow. if:
     q == s : "start"--player is inputting a starting position to place a ship.
     q == e : "end"--player is inputting an ending position for a ship.
     */
    
    std::string position;
    switch(q) {
        case 's':
            std::cout << p.name << ", place your " << b.title << ". it has a length of " << b.length << " units.\n";
            std::cout << "enter the starting coordinate for your ship: ";
            break;
        case 'e':
            std::cout << "enter the ending coordinate for your ship: ";
            break;
    }
    std::getline(std::cin, position);
    return position;
}

coordinate interpretMoveInput(std::string givenMove) {
    coordinate c;
    coordinate cErr; // error return value
    cErr.x = -1;
    cErr.y = -1;
    /*
     input validity: if not length 2 or 3; or,
     length 3, but with the letter in the middle (ex. 1a2)
    */
    if ( ((givenMove.length() != 2) && (givenMove.length() != 3))
        || ((givenMove.length() == 3) && (std::isalpha(givenMove[1]))) ) {
        return cErr;
    }
    // input must contain a (valid) number and a (valid) letter
    bool charFound = false;
    bool numFound = false;
    
    char lowerLetters[10];
    char upperLetters[10];
    int letterValues[10];
    for (int val = 0; val < 10; val++) {
        letterValues[val] =  0  + val;
        lowerLetters[val] = 'a' + val;
        upperLetters[val] = 'A' + val;
    }
    
    for (int i = 0; i < givenMove.length(); i++) {
        // if it's a letter, assign the appropriate value to x (letters are horizontal axis)
        if (std::isalpha(givenMove[i])) {
            // find which letter it is, making sure it's a valid one
            for (int letterInd = 0; letterInd < 10; letterInd++) {
                if ((lowerLetters[letterInd] == givenMove[i]) || (upperLetters[letterInd] == givenMove[i])) {
                    charFound = true;
                    c.x = letterValues[letterInd];
                }
            }
        }
        // if it's a number (which could be 2-digit), assign value to y (numbers are vertical axis)
        else if (std::isdigit(givenMove[i])) {
            numFound = true;
            // if it's of length 2, the number is right there
            if (givenMove.length() == 2) c.y = std::stoi(givenMove.substr(i,1)) - 1;
            else {
                // if not, we need to be careful: next char must be a number also
                if (std::isdigit(givenMove[i+1])) {
                    c.y = std::stoi(givenMove.substr(i,2)) - 1;
                    i += 1;
                }
                else numFound = false;
            }
        }
        else return cErr;
    }
    // now make sure we got the right input and that the raw numbers check out
    if ( (charFound && numFound) &&
         ((c.x>=0)  && (c.y>=0)  && (c.x<10) && (c.y<10)) ) return c;
    return cErr;
}

bool isPlacementValid(placement p, battleship bToPlace) {
    // either both x's, OR both y's need to be equal; otherwise, ship is diagonal
    if ((p.start.x != p.end.x) &&
        (p.start.y != p.end.y)) {
        std::cout << "\nerror: ship is diagonal. try again.\n\n";
        return false;
    }
    
    // check for correct length; if x's are incorrect length, AND y's are incorrect length, false
    if ((std::abs(p.start.x - p.end.x) != bToPlace.length-1) &&
        (std::abs(p.start.y - p.end.y) != bToPlace.length-1)) {
        std::cout << "\nerror: ship is not of correct length. " << bToPlace.title
                  << "s are " << bToPlace.length << " units long. please try again.\n\n";
        return false;
    }
    
    return true;
}

placement cleanPlacement(placement p) {
    // for the logic to work out, it's important that the startpoints < endpoints;
    //  if that isn't the case, we just swap them
    if (p.start.x > p.end.x) {            // checking x values
        int temp = p.start.x;
        p.start.x = p.end.x;
        p.end.x = temp;
    }
    
    if (p.start.y > p.end.y) {            // checking y values
        int temp = p.start.y;
        p.start.y = p.end.y;
        p.end.y = temp;
    }
    
    return p;
}

battleship choosePlacement(player p, int shipNum) {
    // letting player choose the placements of battleships
    //  I don't like this one. FIXME
    battleship currentShip;
    currentShip.startPoint.x = -1; // for error handling
    coordinate startPt, endPt;
    placement givenPlacement;
    
    while (currentShip.startPoint.x == -1) {
        currentShip.assignShipType(shipNum);
        startPt = interpretMoveInput(getPositionOrder(p, 's', currentShip));
        endPt = interpretMoveInput(getPositionOrder(p, 'e', currentShip));
        givenPlacement.start = startPt;
        givenPlacement.end = endPt;
        
        if (isPlacementValid(givenPlacement, currentShip)) {
            givenPlacement = cleanPlacement(givenPlacement);
            currentShip.assignPlacement(givenPlacement.start, givenPlacement.end);
        }
         else continue;
    }
    return currentShip;
}


#endif /* ioInterpretation_h */
