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
"A         Mediocre      Online    Game of   Unending  Suspicion\n";


bool readParameters ( int& argc, char** argv, std::string& ip, bool &a );
std::vector<std::string> loadMap ( std::string mapname ) ;
void drawMap(std::vector<std::string> &gmap, int x, int y);
int distance(int x1, int y1, int x2, int y2);
void await(int sockfd, const int id, std::map<int, crewmate>  &positions, crewmate &ghost, std::vector<std::string> &gamemap, std::vector<std::string> &wallmap, playermodel model);
