#pragma once
#include <ncurses.h>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <string>
#include <cmath>
#include <time.h>

#include <string.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 8888
#include <netdb.h>
#include <stdlib.h>
#define MAX 256
#define SA struct sockaddr
#define TOPOFFSET 1
#define FOVX 64
#define FOVY 16
#define SIGHT_RADIUS 30
#define ALIVE 1
#define DEAD 0
#define KILL_TIMEOUT 100
#define KILL_RADIUS 7
#define STARTING_POSITION_X 137
#define STARTING_POSITION_Y 7
#define INITIAL_KTIMEOUT 1200
#define BUTTON_TIMEOUT 3000
#define DOOR_TIMEOUT 100
#define SABBOTAGE_TIMEOUT 1500
#define PI 3.14159

/**
 * Holds position and status of a player
 */
struct crewmate {
    int x;
    int y;
    int status;
};
/**
 * Holds text which represents the player in certain conditions
 */
struct playermodel {
    std::string alive;
    std::string body;
    std::string ghost;
};
/**
 * Holds key codes for specific actions
 */
struct keyBinds{
    int moveN;
    int moveE;
    int moveW;
    int moveS;
    int moveNE;
    int moveNW;
    int moveSE;
    int moveSW;
    int middle;
    int kill;
    int use;
    int report;
    int sabbotage;
    int quit;
};

/**
 * Holds countdowns
 */
struct countdown{
    std::map<int, unsigned int> kill;
    unsigned int voting;
    unsigned int button;
    unsigned int sabbotage;
    
};
struct taskStruct{
    char current;
    std::map<char, int> list;
    int done;
    int to_do;
    bool received;
};
/**
 * Holds necessary game status like postitions and flags
 */
struct status{
    std::map<int, crewmate> position;
    std::map<int, taskStruct> tasks;
    std::map<int, crewmate> ghosts;
    std::map<int, int> votes;
    std::map<int, bool> ready;
    bool in_progress;
    unsigned int cameras;
    countdown timer;
    int winner;
    int sabbo;
    bool light_switches[5];
    std::pair<int, int> hand_scanners;
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
"## |  ## |## | ## | ## |##    ##/ ##    ## |##    ##/ /     ##/ \n"
"##/   ##/ ##/  ##/  ##/  ######/   ####### | ######/  #######/  \n"
"                                  /  \\__## |\n"                    
"                                  ##    ##/\n"                     
"                                   ######/\n"
"A         Mediocre      Online    Game of   Unending  Suspicion\n"
"\n"
"               Press any key to start...\n";
/**
 * Checks if position b is within the line of sight of position a;
 *@param xa x cooridnate of source postion
 *@param ya y cooridnate of source postion
 *@param xb x cooridnate of destination postion
 *@param yb y cooridnate of destination postion
 *@param wallmap map of walls which block sight
 *@return true if b is visible, false if view of b is blocked
*/
bool LOS(int xa, int ya, int xb, int yb, std::vector<std::string> &wallmap, int sabbo);

/**
 *Shows a message about player's role in the game.
 *@param status status of the player
 */
void displayRole(int status);
/**
 * Reads parameters passed to the main function
 * @param argc number size of argv
 * @param argv array of arguments
 * @param[out] ip server address
 * @param[out] a ascii mode flag
 * @param[out] kb custom keyBinds flag
 * @return true if any errors
 */
bool readParameters ( int& argc, char** argv, std::string& ip, bool &a, bool &kb );

/**
 * Reads map data from file
 * @param mapname name of the file to open
 * @return vector of strings wchich contains the map
 */
std::vector<std::string> loadMap ( std::string mapname ) ;

/**
 * Draws the map from specified perspecitve
 * @param gmap game map data
 * @param x horizontal position
 * @param y vertical position
 */
void drawMap(std::vector<std::string> &gmap, std::vector<std::string> &wallmap, int x, int y, int sabbo, bool camera=false);

/**
 * prints a file, replacing characters with specified colors. 
 * @param colA color to replace 'a' and 'A'
 * @param colB color to replace 'b' and 'B'
 * @param filename name of the text file to display
 */
void cutscene(int colA, int colB, std::string filename);

/**
 * Calculates the disctance between point 1 and 2
 * @param x1 horizontal position 1
 * @param y1 vertical position 1
 * @param x2 horizontal position 2
 * @param y2 vertical position 2
 * @return distance
 */
int distance(int x1, int y1, int x2, int y2);

/**
 * Displays text in the center of a box
 * @param text the text to display
 * @param limX box width
 * @param limY box height
 * @param offX box offset in x cooridnate (default: 0)
 * @param offY box offset in y cooridnate (default: 0)
 */
void printCenter(std::string text, const int limX, const int limY, const int offX=0, const int offY=0);

/**
 * Handles player input and communication with the server
 */
void await(int sockfd, const int id, std::map<int, crewmate>  &positions, crewmate &ghost, std::vector<std::string> &gamemap, std::vector<std::string> &wallmap, playermodel model, std::map<char,std::vector<std::string>> &triggers, bool kb, taskStruct &tasks);
void drawCharacters(const int x, const int y, std::map<int, crewmate>  &positions, std::vector<std::string> &wallmap, playermodel model, int sabbo);
void printTasks(taskStruct &tasks, std::map<char,std::vector<std::string>> &triggers, int status, int sample_countdown, int sabbo);
std::map<char,std::vector<std::string>> loadLabels(std::string filename);
void sendReply(int sd,int i,  status &game);
void applyMovement(const char ch,  const int i, std::map<int, crewmate>&p, std::vector<std::string> &gamemap, int sabbo, const bool ghost);
void votesResult(status &game);
void cleanDeadBodies(std::map<int, crewmate> &position);
void startGame(status &game);
bool collisionCheck(int id, std::map<int, crewmate> &pos, std::vector<std::string> &gamemap, bool ghost);
bool handleTask(char taskName, std::map<char,std::vector<std::string>> &triggers, const keyBinds kBinds, std::map<int, crewmate>  &positions, std::vector<std::string> &gamemap,std::vector<std::string> &wallmap, playermodel model, int sockfd, int id);
void drawCircle(const int x, const int y, const int r, std::string characters);
void wait(int n);