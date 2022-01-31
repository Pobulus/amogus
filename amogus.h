#pragma once
#include <ncurses.h>
#include <vector>
#include <map>
#include <fstream>
#include <string>

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
#define MAX 80
#define SA struct sockaddr
#define FOVX 64
#define FOVY 16
#define ALIVE 1
#define DEAD 0



struct crewmate {
    int x;
    int y;
    int status;
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
void await(int sockfd, const int id, std::map<int, std::pair<int, int>>  &positions, std::vector<std::string> &gamemap, std::string crewmate);
