//
//  main.cpp
//  Battleship 1.2
//
//  Created by Kollin Mendoza on 8/7/19.
//  Copyright © 2019 km. All rights reserved.
//

#include <iostream>
#include "assets.h"
#include "ioInterpretation.h"
#include "controller.h"

int main(int argc, char *argv[]) {
    srand(int(time(0)));
    
    std::string response = argv[1];
    response = tolower(response);
    bool continuing = true;
    
    while (continuing) {
        if (response == "1player") {
            playGame(1);
        }
        else if (response == "2player") {
            playGame(2);
        }
        else if (response == "help") {
            printTxtFile("help.txt");
            return 0;
        }
        else if (response == "rules") {
            printTxtFile("rules.txt");
            return 0;
        }
        else return 1;
        
        std::cout << "play again? y/n: ";
        std::getline(std::cin, response);
        response = tolower(response);
        if (response == "y" || response == "yes") continue;
        else if (response == "n" || response == "no") continuing = false;
        else response = "?"; // asks again
    }
     
    return 0;
}
