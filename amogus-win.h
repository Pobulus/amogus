#pragma once
#include <curses.h>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <cmath>

#include <string.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <winsock.h>

#include <unistd.h>
#include <string.h>
#define PORT 8888

#include <stdlib.h>
#define MAX 80
#define SA struct sockaddr
#define TOPOFFSET 1
#define FOVX 64
#define FOVY 16
#define RADIUS 30
#define ALIVE 1
#define DEAD 0
#define KILL_TIMEOUT 100
#define KILL_RADIUS 7

#define bzero(b, len) (memset((b), '\0', (len)), (void) 0)
struct crewmate {
    int x;
    int y;
    int status;
};
struct playermodel {
    std::string alive;
    std::string body;
    std::string ghost;
    
    
};
const std::string impostor = 
"YOU ARE THE \n"
"         ___ __  __ ___  ___  ___ _____ ___  ___ \n"
"        |_ _|  \\/  | _ \\/ _ \\/ __|_   _/ _ \\| _ \\ \n"
"         | || |\\/| |  _/ (_) \\__ \\ | || (_) |   / \n"
"        |___|_|  |_|_|  \\___/|___/ |_| \\___/|_|_\\ \n"
"               Kill other crewmates to win.";
const std::string crewm8 = 
"YOU ARE THE\n"
"          ___ ___ _____      ____  __   _ _____ ___ \n"
"         / __| _ \\ __\\ \\    / /  \\/  | /_\\_   _| __|\n"
"        | (__|   / _| \\ \\/\\/ /| |\\/| |/ _ \\| | | _| \n"
"         \\___|_|_\\___| \\_/\\_/ |_|  |_/_/ \\_\\_| |___|\n"
"               Complete all your tasks to win.";
const std::string banner = 
"  ______                                     __    __   \n"        
" /      \\                                   /  |  /  |          \n"
"/######  | _____  ____    ______    ______  ## |  ## |  _______ \n"
"## |__## |/     \\/    \\  /      \\  /      \\ ## |  ## | /       |\n"
"##    ## |###### ####  |/######  |/######  |## |  ## |/#######/ \n"
"######## |## | ## | ## |## |  ## |## |  ## |## |  ## |##      \\ \n"
"## |  ## |## | ## | ## |## \\__## |## \\__## |## \\__## | ######  |\n"
"## |  ## |## | ## | ## |##    ##/ ##    ## |##    ##/ /     ##/ \n";

const std::string banner2 = "##/   ##/ ##/  ##/  ##/  ######/   ####### | ######/  #######/  \n"
"                                  /  \\__## |\n"                    
"                                  ##    ##/\n"                     
"                                   ######/\n"
"A         Mediocre      Online    Game of   Unending  Suspicion\n";

/**
 * Checks if position b is within the line of sight of position a;
 *@param xa x cooridnate of source postion
 *@param ya y cooridnate of source postion
 *@param xb x cooridnate of destination postion
 *@param yb y cooridnate of destination postion
 *@param wallmap map of walls which block sight
 *@return true if b is visible, false if view of b is blocked
*/
bool LOS(int xa, int ya, int xb, int yb, std::vector<std::string> &wallmap);

/**
 *Shows a message about player's role in the game.
 *@param status status of the player
 */
void displayRole(int status);
/**
 * Reads paramtere's passed to the main function
 *
 */
bool readParameters ( int& argc, char** argv, std::string& ip, bool &a );
std::vector<std::string> loadMap ( std::string mapname ) ;
void drawMap(std::vector<std::string> &gmap, int x, int y);
void cutscene(int colA, int colB, std::string filename);
int distance(int x1, int y1, int x2, int y2);
void printCenter(std::string text, int limX, int limY, int offX=0, int offY=0);
void await(int sockfd, const int id, std::map<int, crewmate>  &positions, crewmate &ghost, std::vector<std::string> &gamemap, std::vector<std::string> &wallmap, playermodel model, std::map<char,std::vector<std::string>> &triggers);
std::map<char,std::vector<std::string>> loadLabels(std::string filename);
