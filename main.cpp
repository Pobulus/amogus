#include <iostream>
#include "amogus.h"
#include "draw.h"


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
    bool ascii=false, kb=false;
     std::string ip;
       if(readParameters(argc, argv, ip, ascii, kb)){
        return 1;
    }
    setlocale(LC_ALL, "");
    
    taskStruct tasks;
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
        std::cout << "socket creation failed..."<<std::endl;
        exit(0);
    }
    else
        std::cout << "Socket successfully created..."<<std::endl;
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    servaddr.sin_port = htons(PORT);
   
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        std::cout << "connection with the server failed..."<<std::endl;
        getch();
        endwin();
        close(sockfd);
        return 1;
    }
    else
        std::cout << "connected to the server..."<<std::endl;
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
    init_pair(20, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    init_pair(21, COLOR_RED, COLOR_WHITE);
    init_pair(22, COLOR_CYAN, COLOR_WHITE);
    init_pair(23, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(24, COLOR_GREEN, COLOR_WHITE);
    init_pair(25, COLOR_YELLOW, COLOR_WHITE);
    init_pair(26, COLOR_BLUE, COLOR_WHITE);
    init_pair(8, COLOR_BLACK, COLOR_RED);
    init_pair(9, COLOR_BLACK, COLOR_CYAN );
    init_pair(10, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(11, COLOR_BLACK, COLOR_GREEN);
    init_pair(12, COLOR_BLACK, COLOR_YELLOW);
    init_pair(13, COLOR_BLACK, COLOR_BLUE);
    init_pair(14, COLOR_BLACK, COLOR_WHITE );
    
    init_pair(51, COLOR_RED, COLOR_CYAN);
    init_pair(52, COLOR_CYAN, COLOR_BLUE);
    init_pair(53, COLOR_MAGENTA, COLOR_CYAN);
    init_pair(54, COLOR_GREEN, COLOR_CYAN);
    init_pair(55, COLOR_YELLOW, COLOR_CYAN);
    init_pair(56, COLOR_BLUE, COLOR_CYAN);
    init_pair(57, COLOR_WHITE, COLOR_CYAN);
    init_pair(71, COLOR_RED, COLOR_CYAN);
    init_pair(72, COLOR_CYAN, COLOR_CYAN);
    init_pair(73, COLOR_MAGENTA, COLOR_CYAN);
    init_pair(74, COLOR_GREEN, COLOR_CYAN);
    init_pair(75, COLOR_YELLOW, COLOR_CYAN);
    init_pair(76, COLOR_BLUE, COLOR_CYAN);
    init_pair(58, COLOR_BLACK, COLOR_CYAN);
    init_pair(59, COLOR_BLACK, COLOR_CYAN );
    init_pair(60, COLOR_BLACK, COLOR_CYAN);
    init_pair(61, COLOR_BLACK, COLOR_CYAN);
    init_pair(62, COLOR_BLACK, COLOR_CYAN);
    init_pair(63, COLOR_BLACK, COLOR_CYAN);
    init_pair(64, COLOR_BLACK, COLOR_CYAN );

    mvprintw(0,0, banner.c_str());
         char buff[MAX];


        bzero(buff, sizeof(buff));
        int playertag = 0;
        read(sockfd, buff, sizeof(buff));
        {
        std::stringstream deco;
        deco << buff;
        deco >> playertag;
        }
    getch();
    
    timeout(100);
    clear();
    
    printCenter("Server is full.", FOVX, FOVY);
    refresh();
    // function for connections
    srand(time(NULL));
       
    crewmate ghst; //pozycja gracza kiedy jest duchem
    playermodel cm;
    if(ascii)
        cm = {"A", "%%", "g"};
    else
        cm = {"ඞ",  "🦴 ", "👻"};
    
    await(sockfd, playertag, positions, ghst, gamemap,wallmap, cm, triggers, kb, tasks);
   
    // close the socket
    endwin();
    close(sockfd);
    return 0;
    
    
    
}



