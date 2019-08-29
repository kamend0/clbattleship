//
//  assets.h
//  Battleship 1.2
//
//  Created by Kollin Mendoza on 8/7/19.
//  Copyright © 2019 km. All rights reserved.
//

#ifndef assets_h
#define assets_h
#include <vector>


// --------------------------------------------------------------------------------------------------------------------------------
/*
        - game logic assets -
 coordinate (struct) : represents point on cartesian plane (x,y)
 placement (struct) : two coordinates (start, end) glued together
 battleship (class) : represents every battleship, contains methods to place randomly, etc.
 board (class) : represents gameboard, contains methods to alter data, print to screen, etc.
*/
// --------------------------------------------------------------------------------------------------------------------------------


struct coordinate {
    // (x,y) coordinate
    int x = 0;
    int y = 0;
};

struct placement {
    coordinate start;
    coordinate end;
};

class battleship {
    std::string shipTitles [5] = {"Patrol Boat", "Submarine",
        "Destroyer", "Battleship", "Carrier"};
    char shipCodes [5] = {'p', 's', 'd', 'b', 'c'};
    int shipLengths [5] = { 2, 3, 3, 4, 5 };
    
public:
    std::string title;
    char code;
    int length;
    coordinate startPoint;
    coordinate endPoint;
    std::vector<coordinate> locationCoords;
    
    void assignShipType(int typeIdentifier) {
        title = shipTitles[typeIdentifier];
        code = shipCodes[typeIdentifier];
        length = shipLengths[typeIdentifier];
    }
    
    void assignRandomPlacement() {
        locationCoords.clear();
        coordinate startCoord, endCoord;
        bool isHorizontal = rand() % 2; // randomly chooses a direction
        int sameAxis = rand() % 10;
        int dAxisStart = rand() % (11 - length);
        int dAxisEnd = dAxisStart + length - 1;
        
        if (isHorizontal) {
            startCoord.x = sameAxis;
            endCoord.x = sameAxis;
            startCoord.y = dAxisStart;
            endCoord.y = dAxisEnd;
        }
        else {
            startCoord.y = sameAxis;
            endCoord.y = sameAxis;
            startCoord.x = dAxisStart;
            endCoord.x = dAxisEnd;
        }
        startPoint = startCoord;
        endPoint = endCoord;
        
        coordinate locCoord;
        for (int h = startCoord.x; h <= endCoord.x; h++) {
            for (int v = startCoord.y; v <= endCoord.y; v++) {
                locCoord.x = h;
                locCoord.y = v;
                locationCoords.push_back(locCoord);
            }
        }
    }
    
    void assignPlacement(coordinate startCoord, coordinate endCoord) {
        coordinate locCoord;
        startPoint = startCoord;
        endPoint = endCoord;
        std::cout << "in assignPlacement, startCoord = " << startCoord.x << ", " << startCoord.y << '\n';
        std::cout << " and endCoord = " << endCoord.x << ", " << endCoord.y << '\n';
        for (int h = startCoord.x; h <= endCoord.x; h++) {
            for (int v = startCoord.y; v <= endCoord.y; v++) {
                locCoord.x = h;
                locCoord.y = v;
                locationCoords.push_back(locCoord);
            }
        }
    }
};


class board {
    /*
     gamestate char codes:
     'o' : open water
     'h' : hit
     'm' : miss
     others: correspond to battleship char array "shipCodes"
    */
    
    static const int xDim = 10;
    static const int yDim = 10;
    static const int numShips = 5;
    char gamestate[yDim][xDim];
    
public:
    std::vector<battleship> shipsInPlay;
    std::vector<battleship> sunkShips;
    
    void modifyState(coordinate c, char newVal) {
        // for using coordinate object instead of directly modifying gamestate
        gamestate[c.y][c.x] = newVal;
    }
    
    void cleanBoard() {
        for (int x = 0; x < xDim; x++) {
            for (int y = 0; y < yDim; y++) {
                gamestate[y][x] = 'o'; // make board all open water
            }
        }
    }
    
    bool isPopulated() {
        if (shipsInPlay.size() != 5) return false;
        return true;
    }
    
    void place(battleship b) {
        shipsInPlay.push_back(b);
        for (int i = 0; i < b.locationCoords.size(); i++) {
            std::cout << "placing " << b.locationCoords[i].x << ", " << b.locationCoords[i].y << '\n';
            modifyState(b.locationCoords[i], b.code);
        }
    }
    
    bool checkCollision(battleship b) {
        for (int xPos = b.startPoint.x; xPos <= b.endPoint.x; xPos++) {
            for (int yPos = b.startPoint.y; yPos <= b.endPoint.y; yPos++) {
                // if there is anything besides open water, there must be another ship there
                if (gamestate[yPos][xPos] != 'o') return true;
            }
        }
        return false;
    }
    
    void populateRandomly() {
        // assign random placement of ships
        for (int shipType = 0; shipType < numShips; shipType++) {
            battleship currentShip;
            currentShip.assignShipType(shipType);
            currentShip.assignRandomPlacement();
            
            // ensure no collisions, re-trying if there are
            while (checkCollision(currentShip)) currentShip.assignRandomPlacement();
            
            shipsInPlay.push_back(currentShip);
            place(currentShip);
        }
    }
    
    void hideShips() {
        // allowing player to assign placement of ships
        for (int shipType = 0; shipType < numShips; shipType++) {
            battleship currentShip;
            currentShip.assignShipType(shipType);
        }
    }
    
    bool checkIfSunk(char ship) {
        // check if ship still exists on board. if not, it's sunk.
        for (int x = 0; x < xDim; x++) {
            for (int y = 0; y < yDim; y++) {
                if (gamestate[y][x] == ship) return false;
            }
        }
        return true;
    }
    
    void sunk(char bCode) {
        for (int i = 0; i < shipsInPlay.size(); i++) {
            if (shipsInPlay[i].code == bCode) {
                sunkShips.push_back(shipsInPlay[i]);
                shipsInPlay.erase(shipsInPlay.begin() + i);
            }
        }
    }
    
    char registerStrike(coordinate strike) {
        /*
         returns:
         'm' if player misses;
         'h' if player hits hidden ship;
         's' if player sinks the ship;
         'r' if repeated strike (no effect)
        */
        char strikeZone = gamestate[strike.y][strike.x];
        switch (strikeZone) {
            case 'o':                   // player misses, hitting open water
                modifyState(strike, 'm');
                return 'm';
            case 'h':                   // player already hit or missed this area
            case 'm':
                return 'r';
            default:                    // every other char indicates hidden ship
                modifyState(strike, 'h');
                if (checkIfSunk(strikeZone)) {
                    sunk(strikeZone);
                    return 's';
                }
                return 'h';
        }
    }
    
    bool check1pWin() {
        if (shipsInPlay.size() == 0) return true;
        return false;
    }
    
    // method to print game board to screen
    void showBoard(int debug) {
        // debug = 1 for debug board that shows hidden ships; = 0 for normal
        
        int lefthandBuffer = 7;                                     // lefthand border offset (after labels)
        int verticalBuffer = 4;                                     // vertical spacing between rows
        int cellSpacing = 6;                                        // horizontal spacing between cells
        std::string space = std::string(cellSpacing, ' ');
        
        std::cout << std::string(3+lefthandBuffer, ' ');            // top row, alphabetic column labels
        for (char l = 'A'; l < 'K'; l++) std::cout << l << std::string(2+cellSpacing, ' ');
        for (int i = 0; i < verticalBuffer; i++) std::cout << '\n';
        
        for (int colInd = 0; colInd < xDim; colInd++) {
            if (colInd < 9) std::cout << ' ' << colInd+1;           // lefthand side, numeric row label
            else std::cout << colInd+1;
            
            std::cout << std::string(lefthandBuffer, ' ');          // printing content of row (cells)
            for (int rowInd = 0; rowInd < yDim; rowInd++) {
                switch (gamestate[colInd][rowInd]) {
                    case 'h':
                        std::cout << "[X]" << space;                    // hit
                        break;
                    case 'm':
                        std::cout << "[O]" << space;                    // miss
                        break;
                    case 'o':
                        std::cout << "[ ]" << space;                    // open water or hidden ship w/o debug behavior
                        break;
                    default:                                            // hidden ship, but with debug behavior
                        // if debugging, show exactly what ship is where
                        if (debug == 1) {
                            std::cout << "[" << gamestate[colInd][rowInd] << "]" << space;
                            break;
                        }
                        else std::cout << "[ ]" << space;
                        break;
                        
                }
            }
            for (int rowBuf = 0; rowBuf < verticalBuffer-1; rowBuf++) std::cout << '\n';
        }
    }
};


// --------------------------------------------------------------------------------------------------------------------------------
/*
        - interface assets -
 player (class) : represents each player, containing their name, score, etc.
 infoBar (class) : properly formatted information block for various purposes
*/
// --------------------------------------------------------------------------------------------------------------------------------


class player {
public:
    std::string name;
    int maxMisses = 17;
    int missesRemaining = maxMisses;
    int score = maxMisses;
    bool turn = false;
    
    void setScore(int scr) {
        score = scr;
    }
    
    void gainPoint() {
        score++;
    }
    
    void losePoint() {
        score --;
    }
    
    void switchTurns() {
        turn = !turn;
    }
    
    void missed() {
        missesRemaining--;
        losePoint();
    }
};


class infoBar {
    int lefthandBufferSize = 23;
    int valuesSpacingSize = 15;
    std::string leftBuffer = std::string(lefthandBufferSize, ' ');
    std::string valuesBuffer = std::string(valuesSpacingSize, ' ');
    
public:
    void showScores(player player1, player player2) {
        std::cout << leftBuffer;
        std::cout << player1.name << "\'s score: " << player1.score;
        std::cout << valuesBuffer;
        std::cout << player2.name << "\'s score: " << player2.score << '\n';
        
        std::cout << leftBuffer;
        // this is just for correct grammar
        switch (player1.missesRemaining) {
            case 16:
                // they've only missed once
                std::cout << player1.name << " has missed " << player1.maxMisses-player1.missesRemaining
                << " time; " << player1.missesRemaining << " misses remaining.";
                break;
            case 1:
                // only one miss left
                std::cout << player1.name << " has missed " << player1.maxMisses-player1.missesRemaining
                << " times; only " << player1.missesRemaining << " miss remaining.";
                break;
            default:
                // they've missed more (or less) than once
                std::cout << player1.name << " has missed " << player1.maxMisses-player1.missesRemaining
                << " times; " << player1.missesRemaining << " misses remaining.";
                break;
        }
        
        std::cout << "\n\n\n";
    }
    
    void respond(board gb, player player1, player player2, char responseType) {
        std::cout << std::string(lefthandBufferSize+9, ' ');
        
        switch (responseType) {
            case 'o':
                // message for beginning one-player game
                std::cout << "battleships are in position.\n";
                std::cout << std::string(lefthandBufferSize+9, ' ') << "waiting on your command, " << player1.name << '.';
                break;
            case 't':
                // message for beginning two-player game
                std::cout << player2.name << ", place your battleships.";
                break;
            case 'p':
                // message for successful ship placement
                std::cout << gb.shipsInPlay[gb.shipsInPlay.size()-1].title << " is in position.";
                break;
            case 'r':
                // message for repeat hits on a coordinate
                std::cout << player1.name << " has already hit that coordinate. please try again.";
                break;
            case 'm':
                // message for a miss
                std::cout << "it's a miss, " << player1.name << ".";
                break;
            case 's':
                std::cout << player1.name << " sunk " << player2.name << "\'s " << gb.sunkShips[gb.sunkShips.size()-1].title << "!";
                break;
            case 'h':
                // message for a hit
                std::cout << "it's a hit!";
                break;
            case 'e':
                // message for invalid input
                std::cout << "error: invalid input given. please try again.";
                break;
            case 'l':
                // message for player loss
                std::cout << "sorry, " << player1.name << ", it's a loss this time.";
                break;
            case 'w':
                // message for player win
                std::cout << "congratulations, " << player1.name << ", you win!";
                break;
            case 'f':
                // message for player 1 beating player 2
                std::cout << player1.name << " wins! better luck next time, " << player2.name << ".";
                break;
            case 'g':
                // message for player 2 beating player 1
                std::cout << player2.name << " wins! better luck next time, " << player1.name << ".";
                break;
        }
        
        std::cout << "\n\n\n";
    }
};

#endif /* assets_h */
