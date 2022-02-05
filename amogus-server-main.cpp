//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include "amogus.h"
#include <stdio.h>
#include <string.h>   //strlen 
#include <string>   //strlen 
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <errno.h>
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <map>
#include <random>
#define TRUE   1
#define FALSE  0
#define PORT 8888
bool collisionCheck(int id, std::map<int, crewmate> &position, std::vector<std::string> &gamemap) {
    auto curpos = position[id];
    if(curpos.x <0 || curpos.y < 0)return true;
    //std::cout << "znak na "<<curpos.second <<":" << curpos.first << "to: "<< (int)gamemap[curpos.first].size()<<std::endl;
    
    for(auto ch : position) {
        if(ch.first != id) {
            if((ch.second.x == position[id].x)&&(ch.second.y == position[id].y))return true;
        }
    }
    if(gamemap[curpos.y].at(curpos.x)== ' ')return false;
    else if(gamemap[curpos.y].at(curpos.x)>'?'&&gamemap[curpos.y].at(curpos.x)<'[') return false;
    else if(gamemap[curpos.y].at(curpos.x)>'_'&&gamemap[curpos.y].at(curpos.x)<'{') return false;
    else if(gamemap[curpos.y].at(curpos.x)>'/'&&gamemap[curpos.y].at(curpos.x)<':') return false;
    else return true;
}
void startGame(bool &gameStatus, std::map<int, crewmate> &position) {
    position[rand()%position.size()].status += 2;//make a random player the impostor
    gameStatus = true;
    std::cout << "starting game" <<std::endl;
    return;
}
int main(int argc, char *argv[])
{
    bool game_in_progress = false;
    std::map<int, bool> ready;
    std::map<int, crewmate> position;
    std::map<int, crewmate> ghosts;
    std::vector<std::string> gamemap;
    std::map<int, unsigned int> killTimeout;
    gamemap = loadMap("mapwalls.txt");

    int opt = TRUE;
    int master_socket, addrlen, new_socket, client_socket[30],
        max_clients = 30, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;

    char buffer[1025];  //data buffer of 1K

    //set of socket descriptors
    fd_set readfds;

    //a message
    std::string message = "amogus server 0.6 \r\n";

    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    //create a master socket
    if( (master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_addr.s_addr =
    address.sin_port = htons( PORT );

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while(TRUE)
    {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd, &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select( max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }

        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs
                   (address.sin_port));

            //send new connection greeting message


            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    //NEW CONNECTION
                    client_socket[i] = new_socket;
                    if(!ready[i]) {
                        position[i] = {137+2*i, 7, 1};
                        killTimeout[i] = 0;
                        ready[i] = false;

                    }

                    // if(i == 0)position[i].status += 2;//make the first player an impostor (for debug purposes)
                    printf("Adding to list of sockets as %d\n", i);
                    if( send(new_socket, std::to_string(i).c_str(), std::to_string(i).size(), 0) != std::to_string(i).size() )
                    {
                        perror("send");
                    }
                    break;
                }
            }
        }

        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];

            if (FD_ISSET( sd, &readfds))
            {
                //Check if it was for closing , and also read the
                //incoming message
                if ((valread = read( sd, buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd, (struct sockaddr*)&address, \
                                (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                }

                else
                {

                    if(buffer[0] == 'u') {
                        //std::cout << "u" << i <<std::endl;
                        if(killTimeout[i])killTimeout[i]--;
                        std::string pos;
                        if(game_in_progress) {
                            if(position[i].status%2) {
                                pos ="p ";
                            } else {
                                pos ="g ";
                                pos += std::to_string(ghosts[i].x)+" "+std::to_string(ghosts[i].y)+" "+std::to_string(ghosts[i].status)+" ";
                            }
                        } else {
                            pos ="w ";
                            int count = 0;
                            for(auto el : ready) {
                                if(el.second)count++;
                            }
                            pos += std::to_string(count) + " ";
                            pos += std::to_string(ready.size())+ " ";
                            if(count == ready.size())startGame(game_in_progress, position);
                        }
                        for(auto el : position) {
                            pos += std::to_string(el.second.x) + " " + std::to_string(el.second.y) + " "+ std::to_string(el.second.status) + " " ;
                        }

                        send(sd, pos.c_str(), pos.size(), 0 );

                    } else if(buffer[0] == 'k'&& game_in_progress) {

                        if((position[i].status >> 1)%2) {


                            std::string pos ="p ";
                            for(auto &el : position) {
                                if(el.second.status%2&&(el.second.status>>1)%2!=1&&!killTimeout[i]) {
                                    if(distance(position[i].x, position[i].y, el.second.x, el.second.y)<KILL_RADIUS) {
                                        killTimeout[i] = KILL_TIMEOUT;
                                        el.second.status -= 1;
                                        ghosts[el.first].x = el.second.x;
                                        ghosts[el.first].y = el.second.y;
                                        ghosts[el.first].status = i;
                                    }
                                }
                                pos += std::to_string(el.second.x) + " " + std::to_string(el.second.y) + " "+ std::to_string(el.second.status) + " "    ;
                            }
                            send(sd, pos.c_str(), pos.size(), 0 );
                        }


                    } else if(buffer[0] == 'm') { //player moves

                        if(position[i].status%2) {
                            switch(buffer[1]) {
                            case '6':
                                position[i].x++;
                                if(collisionCheck(i, position, gamemap))
                                    position[i].x--;
                                break;
                            case '2':
                                position[i].y++;
                                if(collisionCheck(i, position, gamemap))
                                    position[i].y--;
                                break;
                            case '4':
                                position[i].x--;
                                if(collisionCheck(i, position, gamemap))
                                    position[i].x++;
                                break;
                            case '8':
                                position[i].y--;
                                if(collisionCheck(i, position, gamemap))
                                    position[i].y++;
                                break;
                            case '3':
                                position[i].x++;
                                position[i].y++;
                                if(collisionCheck(i, position, gamemap))
                                {
                                    position[i].x--;
                                    position[i].y--;
                                }
                                break;
                            case '1':
                                position[i].y++;
                                position[i].x--;
                                if(collisionCheck(i, position, gamemap))
                                {
                                    position[i].x++;
                                    position[i].y--;
                                }
                                break;
                            case '7':
                                position[i].x--;
                                position[i].y--;
                                if(collisionCheck(i, position, gamemap))
                                {
                                    position[i].x++;
                                    position[i].y++;
                                }
                                break;
                            case '9':
                                position[i].y--;
                                position[i].x++;
                                if(collisionCheck(i, position, gamemap))
                                {
                                    position[i].x--;
                                    position[i].y++;
                                }
                                break;

                            }
                            std::string pos;
                            if(game_in_progress) {
                                pos ="p ";
                            } else {
                                pos ="w ";
                                int count = 0;
                                for(auto el : ready) {
                                    if(el.second)count++;
                                }
                                pos += std::to_string(count) + " ";
                                pos += std::to_string(ready.size())+ " ";

                            }
                            for(auto el : position) {
                                pos += std::to_string(el.second.x) + " " + std::to_string(el.second.y) + " " + std::to_string(el.second.status) + " ";
                            }

                            send(sd, pos.c_str(), pos.size(), 0 );
                        } else {
                            switch(buffer[1]) {
                            case '6':
                                ghosts[i].x++;
                                break;
                            case '2':
                                ghosts[i].y++;
                                break;
                            case '4':
                                ghosts[i].x--;

                                break;
                            case '8':
                                ghosts[i].y--;

                                break;
                            case '3':
                                ghosts[i].x++;
                                ghosts[i].y++;

                                break;
                            case '1':
                                ghosts[i].y++;
                                ghosts[i].x--;

                                break;
                            case '7':
                                ghosts[i].x--;
                                ghosts[i].y--;

                                break;
                            case '9':
                                ghosts[i].y--;
                                ghosts[i].x++;
                                break;
                            }
                            std::string pos ="g ";
                            pos += std::to_string(ghosts[i].x)+" "+std::to_string(ghosts[i].y)+" "+ std::to_string(ghosts[i].status)+" ";
                            for(auto el : position) {
                                pos += std::to_string(el.second.x) + " " + std::to_string(el.second.y) + " " + std::to_string(el.second.status) + " ";
                            }
                            send(sd, pos.c_str(), pos.size(), 0 );
                        }
                    } else if(buffer[0] == 'w') { //player asks who they are
                        std::cout << "Who am I? "<<i<<std::endl;
                        std::string mess = std::to_string(i)+"\0";
                        send(sd, mess.c_str(), mess.size(), 0 );
                    } else if(buffer[0] == 'r') { //player ready
                        std::cout << "Player ready:  "<<i<<std::endl;
                        ready[i] = true;
                        std:: string pos ="w ";
                        int count = 0;
                        for(auto el : ready) {
                            if(el.second)count++;
                        }
                        pos += std::to_string(count) + " ";
                        pos += std::to_string(ready.size())+ " ";


                        for(auto el : position) {
                            pos += std::to_string(el.second.x) + " " + std::to_string(el.second.y) + " " + std::to_string(el.second.status) + " ";
                        }

                        send(sd, pos.c_str(), pos.size(), 0 );


                    } else {
                        //set the string terminating NULL byte on the end
                        //of the data read
                        buffer[valread] = '\0';
                        std::cout << buffer << std::endl;
                        send(sd, buffer, strlen(buffer), 0 );
                    }

                }
            }
        }
    }

    return 0;
}
