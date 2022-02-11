#include "amogus.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <map>
#include <random>
#define TRUE   1
#define FALSE  0

#define PORT 8888


int main(int argc, char *argv[])
{
    srand((unsigned int)time(NULL));
    status game;
    game.in_progress = false;
    game.timer.voting = 0;
    game.winner = 0;
    std::vector<std::string> gamemap;
    std::map<char,std::vector<std::string>> triggers;
    gamemap = loadMap("mapwalls.txt");
    triggers = loadLabels("cha_list.txt");

    int opt = TRUE;
    int master_socket, addrlen, new_socket, client_socket[30],
        max_clients = 14, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;

    char buffer[1025];  //data buffer of 1K

    //set of socket descriptors
    fd_set readfds;

    //a message
    std::string message = "amogus server 0.9 \r\n";

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
                    if(!game.ready[i]) {
                        game.position[i] = {137+2*i, 7, 1};
                        game.timer.kill[i] = INITIAL_KTIMEOUT;
                        game.ready[i] = false;

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

                    if(buffer[0] == 'u') {//player asks for an update
                        //std::cout << "u" << i <<std::endl;
                        if(game.timer.kill[i])game.timer.kill[i]--;
                        if(game.timer.voting) {
                            game.timer.voting--;
                            if(!game.timer.voting) {
                                votesResult(game);
                                cleanDeadBodies(game.position);



                            }
                        }

                        sendReply(sd, i, game);

                    } else if(buffer[0] == 'k'&& game.in_progress && !game.timer.voting) {//player wants to kill

                        if((game.position[i].status >> 1)%2) {
                            for(auto &el : game.position) {
                                if(el.second.status%2&&(el.second.status>>1)%2!=1&&!game.timer.kill[i]) {
                                    if(distance(game.position[i].x, game.position[i].y, el.second.x, el.second.y)<KILL_RADIUS) {
                                        game.timer.kill[i] = KILL_TIMEOUT;
                                        el.second.status--;
                                        game.ghosts[el.first].x = el.second.x;
                                        game.ghosts[el.first].y = el.second.y;
                                        game.ghosts[el.first].status = i;
                                    }
                                }

                            }
                            sendReply(sd, i, game);
                        }
                    } else if(buffer[0] == 'c'&& game.in_progress) {//player reports a body

                        for(auto &el : game.position) {

                            if(el.second.status%2==0&&distance(game.position[i].x, game.position[i].y, el.second.x, el.second.y)<KILL_RADIUS) {

                                game.timer.voting = game.position.size()*300;
                                for(auto x : game.position) {
                                    if(x.second.status%2) game.votes[x.first] = -1;
                                    else game.votes[x.first] = -2;
                                }
                                break;
                            }

                        }
                        sendReply(sd, i, game);

                    } else if(buffer[0] == 't'&& game.in_progress) {//player wants to do a task
                        char trigger;

                        if(game.position[i].status%2)
                            trigger = gamemap[game.position[i].y][game.position[i].x];
                        else
                            trigger = gamemap[game.ghosts[i].y][game.ghosts[i].x];


                        if(trigger!= ' ') {
                            if(triggers[trigger][0]=="util") {
                                if(triggers[trigger][2]=="button") { //emeregency meeting
                                    game.timer.voting = game.position.size()*300;
                                    for(auto x : game.position) {
                                        if(x.second.status%2)game.votes[x.first] = -1;
                                        else game.votes[x.first] = -2;
                                    }

                                }
                            }
                            std::cout<<"Player "<<i<<":" << triggers[trigger][0]<<" "<<triggers[trigger][1]<<" "<<triggers[trigger][2]<<" "<<std::endl;
                        }
                        sendReply(sd, i, game);


                    } else if(buffer[0] == 'v') { //player votes
                        int attr = buffer[1]-'0';
                        if(attr<= game.votes.size()) {
                            if(game.votes[i]==-1&&game.position[i].status%2&&game.position[attr-1].status%2) {
                                game.votes[i] = attr;

                            }
                        }
                        sendReply(sd, i, game);

                    } else if(buffer[0] == 'm') { //player moves

                        if(game.position[i].status%2) {
                            applyMovement(buffer[1], i, game.position, gamemap, false);

                            sendReply(sd, i, game);

                        } else {
                            applyMovement(buffer[1], i, game.ghosts, gamemap, true);
                            sendReply(sd, i, game);

                        }
                    } else if(buffer[0] == 'w') { //player asks who they are
                        std::cout << "Who am I? "<<i<<std::endl;
                        std::string mess = std::to_string(i)+"\0";
                        send(sd, mess.c_str(), mess.size(), 0 );
                    } else if(buffer[0] == 'r') { //player ready
                        std::cout << "Player ready:  "<<i<<std::endl;
                        game.ready[i] = true;
                        sendReply(sd, i, game);
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
