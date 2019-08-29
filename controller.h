//
//  controller.h
//  Battleship 1.2
//
//  Created by Kollin Mendoza on 8/7/19.
//  Copyright © 2019 km. All rights reserved.
//

#ifndef controller_h
#define controller_h
#include "assets.h"
#include "ioInterpretation.h"

/*
 functions to control the flow of the game
 take players' inputs, arrange them, initiate proper logic.
*/


void playGame(int numPlayers) {
    
    // setup --------------------------------------------------------------------------------------------------------------------------------
    
    int debug = 0; // make 1 to see hidden ships
    
    board gb;
    gb.cleanBoard();
    infoBar statusBar;
    
    player p1;
    player p2;
    
    clearScreen();
    std::cout << "\nwelcome to cl battleship!\n\n";
    
    if (numPlayers == 1) {
        p1.name = getName(1);
        p2.name = "cpu";
        gb.populateRandomly();
        showInterface(gb, p1, p2, statusBar, 'o', debug);
        p1.switchTurns();
    }
    else if (numPlayers == 2) {
        p1.name = getName(1);
        p2.name = getName(2);
        showInterface(gb, p1, p2, statusBar, 't', debug);
        p2.switchTurns();
    }
    
    // start game --------------------------------------------------------------------------------------------------------------------------------
    
    // second player's (concealer's) turn
    if (p2.turn) {
        int shipNum = 0;
        while (!gb.isPopulated()) {
            battleship givenShip;
            
            for ( ; shipNum < 5; shipNum++) {
                givenShip = choosePlacement(p2, shipNum);
                gb.place(givenShip);
                showInterface(gb, p1, p2, statusBar, 'p', debug);
            }
        }
        p2.switchTurns();
        p1.switchTurns();
    }
    
    // first player's (striker's) turn
    if (p1.turn) {
        while ( (!gb.check1pWin()) && (p1.missesRemaining > 0) ) {
            coordinate p1move = interpretMoveInput(getStrikeOrder(p1));
            
            // catch invalid moves
            if ( (p1move.x < 0) || (p1move.y < 0) ) {
                showInterface(gb, p1, p2, statusBar, 'e', debug);
                continue;
            }
            
            // if valid, respond to move
            char behavior = gb.registerStrike(p1move);
            if (behavior == 'm') {
                p1.missed();
                p2.gainPoint();
            }
            else if (behavior == 'h') {
                p1.gainPoint();
                p2.losePoint();
            }
            
            showInterface(gb, p1, p2, statusBar, behavior, debug);
        }
    }
    
    // game over --------------------------------------------------------------------------------------------------------------------------------
    
    if (numPlayers == 1) {
        if (gb.check1pWin()) statusBar.respond(gb, p1, p2, 'w');
        else statusBar.respond(gb, p1, p2, 'l');
    }
    else if (numPlayers == 2) {
        if (gb.check1pWin()) statusBar.respond(gb, p1, p2, 'f');
        else statusBar.respond(gb, p1, p2, 'g');
    }
}


#endif /* controller_h */
