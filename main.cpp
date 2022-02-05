#include <iostream>
#include "amogus.h"

/* ______                                     __    __           
 /      \                                   /  |  /  |          
/######  | _____  ____    ______    ______  ## |  ## |  _______ 
## |__## |/     \/    \  /      \  /      \ ## |  ## | /       |
##    ## |###### ####  |/######  |/######  |## |  ## |/#######/ 
######## |## | ## | ## |## |  ## |## |  ## |## |  ## |##      \ 
## |  ## |## | ## | ## |## \__## |## \__## |## \__## | ######  |
## |  ## |## | ## | ## |##    ##/ ##    ## |##    ##/ /     ##/ 
##/   ##/ ##/  ##/  ##/  ######/   ####### | ######/  #######/  
                                  /  \__## |                    
                                  ##    ##/                     
                                   ######/
A         Mediocre      Online    Game of   Unending  Suspicion
                                   
                                   */

int main(int argc, char **argv) {
    bool ascii;
     std::string ip;
       if(readParameters(argc, argv, ip, ascii)){
        return 1;
    }
    setlocale(LC_ALL, "");
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    curs_set(0);
    
    if(!has_colors()) {
        endwin();
        printf("No color");
        return 0;
    }
    start_color();
    init_pair(10, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_BLACK, COLOR_BLACK);
    mvprintw(0,0, banner.c_str());
    
   std::map<char,std::vector<std::string>> triggers;
    std::vector<std::string> gamemap;
    std::vector<std::string> wallmap;
    
    
    gamemap = loadMap("map.txt");
    wallmap = loadMap("mapwalls.txt");
    
    
    triggers = loadLabels("cha_list.txt");
    triggers[' '] = {"", ""};
    std::map<int,  crewmate> positions;
    
 
    
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
   
    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printw("socket creation failed...\n");
        exit(0);
    }
    else
        printw("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    servaddr.sin_port = htons(PORT);
   
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printw("connection with the server failed...\n");
        getch();
        endwin();
        close(sockfd);
        return 1;
    }
    else
        printw("connected to the server..\n");
    
    getch();
    timeout(100);
    clear();
    // function for connections
     char buff[MAX];


        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        int playertag = buff[0]-48;
        mvprintw(10,0,"playertag: %s\n", buff);
    crewmate ghst; //pozycja gracza kiedy jest duchem
    playermodel cm;
    if(ascii)
        cm = {"A", "%%", "g"};
    else
        cm = {"ඞ",  "🦴 ", "👻"};
    
    await(sockfd, playertag, positions, ghst, gamemap,wallmap, cm, triggers);
   
    // close the socket
    endwin();
    close(sockfd);
    return 0;
    
    
    
}



