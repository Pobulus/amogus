#include <iostream>
#include "amogus-win.h"
#include <windows.h>



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
    //SetConsoleOutputCP(CP_UTF8);
    
    std::string ip;
    std::vector<std::string> gamemap;
    std::vector<std::string> wallmap;
    std::map<char,std::vector<std::string>> triggers;
    gamemap = loadMap("map.txt");
    wallmap = loadMap("mapwalls.txt");
    triggers = loadLabels("cha_list.txt");
    triggers[' '] = {"", ""};
    std::map<int,  crewmate> positions;
    bool ascii;
    if(readParameters(argc, argv, ip, ascii)){
        return 1;
    }
    setlocale(LC_ALL, "");

    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
   
    // socket create and varification
    int iResult;
    WSADATA wsaData;
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0){
        std::cout << "WSAStartup failed:" << iResult << std::endl;
        return 10;
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n %d", WSAGetLastError());
        return 1;
    }
    else
        printf("Socket successfully created..\n");
    std::cout << "Attempting connection to "<<ip << "..."<<std::endl;
    bzero(&servaddr, sizeof(servaddr));
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    servaddr.sin_port = htons(PORT);
   
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        closesocket(sockfd);
        return 1;
    }
    else
        printf("connected to the server..\n");
    
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
    std::cout << "colors" <<std::endl;
    init_pair(10, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_BLACK, COLOR_BLACK);
    mvprintw(0,0, banner.c_str());
    mvprintw(8,0, banner2.c_str());
    getch();
    timeout(100);
    clear();
    // function for connections
     char buff[MAX];


        bzero(buff, sizeof(buff));
        recv(sockfd, buff, sizeof(buff), 0);
        int playertag = buff[0]-48;
        mvprintw(10,0,"playertag: %s\n", buff);
    crewmate ghst; //pozycja gracza kiedy jest duchem
    playermodel cm;
    if(ascii)
        cm = {"A", "%%", "g"};
    else
        cm = {"ඞ",  "🦴", "👻"};
    
    await(sockfd, playertag, positions, ghst, gamemap,wallmap, cm, triggers);
   
    // close the socket
    endwin();
    closesocket(sockfd);
    return 0;
    
    
    
}



