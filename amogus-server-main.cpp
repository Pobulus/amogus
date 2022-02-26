#include "amogus.h"
#include "tasks.h"
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
    game.cameras = 0;
    std::map<char, std::pair<int, int>> vent_positions= {
        {'a', {58, 12}},
        {'b', {10, 31}},
        {'c', {20, 44}},
        {'d', {58, 64}},
        {'e', {74, 45}},
        {'f', {88, 34}},
        {'g', {96, 47}},
        {'h', {210, 40}},
        {'i', {178, 24}},
        {'j', {178, 53}},
        {'k', {209, 8}},
        {'l', {250, 31}},
        {'m', {250, 43}},
        {'n', {212, 73}}
    };
    std::map<char, char> vent_connections = {
        {'a', 'b'},
        {'b', 'a'},
        {'c', 'd'},
        {'d', 'c'},
        {'e', 'f'},
        {'f', 'g'},
        {'g', 'e'},
        {'h', 'j'},
        {'i', 'h'},
        {'j', 'i'},
        {'k', 'l'},
        {'l', 'k'},
        {'m', 'n'},
        {'n', 'm'}
    };
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


        
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    //NEW CONNECTION
                    client_socket[i] = new_socket;
                    if(!game.ready[i]) {
                        game.tasks[i].current = 0;
                        game.tasks[i].done = 0;
                        game.tasks[i].received = false;
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
                    game.tasks[i].received = false;
                    if(game.tasks[i].current>'n'||game.tasks[i].current<'a')game.tasks[i].current= 0;
                    client_socket[i] = 0;
                }

                else
                {

                    if(buffer[0] == 'u') {//player asks for an update
                        //std::cout << "u" << i <<std::endl;
                        if(game.timer.button)game.timer.button--;
                        if(game.timer.sabbotage){
                            if(game.sabbo<8){
                                game.timer.sabbotage--;
                                if(game.timer.sabbotage==0)game.sabbo = 0;
                            }else if(game.sabbo>9){
                                game.timer.sabbotage--;
                                if(game.timer.sabbotage==0){
                                    game.sabbo = 0;
                                    std::cout << "impostors win"<<std::endl;
                                    game.winner = 1;
                                    std::cout <<"Reseting game"<<std::endl;
                                    
                                    for(auto cm : game.position) {
                                        game.position[cm.first] = {STARTING_POSITION_X+2*cm.first, STARTING_POSITION_Y, 1};
                                        game.timer.kill[cm.first] = INITIAL_KTIMEOUT;
                                        game.ready[cm.first] = false;
                                        game.in_progress = false;
                                        game.tasks[cm.first].list.clear();
                                        game.tasks[cm.first].current = 0;
                                    }
                                    
                                }
                            }
                            
                        }
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

                    } else if(buffer[0] == 's'&& game.in_progress) {//player wants to sabbotage
                        if(!game.sabbo&&!game.timer.sabbotage)
                        switch(buffer[1]){
                            case '1':
                                game.sabbo = 1;
                                game.timer.sabbotage = DOOR_TIMEOUT*game.position.size();
                                break;
                            case '2':
                                game.sabbo = 2;
                                game.timer.sabbotage = DOOR_TIMEOUT*game.position.size();
                                break;
                            case '3':
                                game.sabbo = 3;
                                game.timer.sabbotage = DOOR_TIMEOUT*game.position.size();
                                break;
                            case '4':
                                game.sabbo = 4;
                                game.timer.sabbotage = DOOR_TIMEOUT*game.position.size();
                                break;
                            case '5':
                                game.sabbo = 5;
                                game.timer.sabbotage = DOOR_TIMEOUT*game.position.size();
                                break;
                            case '6':
                                game.sabbo = 6;
                                game.timer.sabbotage = DOOR_TIMEOUT*game.position.size();
                                break;
                            case '7':
                                game.sabbo = 7;
                                game.timer.sabbotage = DOOR_TIMEOUT*game.position.size();
                                break;
                            case '8':
                                game.sabbo = 8;
                                for(auto  &player : game.tasks){
                                    player.second.list['5'] = 1;
//                                     player.second.to_do++;
                                    player.second.received = false;
                                }
                                game.timer.sabbotage = SABBOTAGE_TIMEOUT;
                                break;
                            case '9':
                                game.sabbo = 9;
                                for(int i = 0; i<5;i++){
                                    game.light_switches[i] = rand()%2;
                                }
                                
                                for(auto  &player : game.tasks){
                                    player.second.list['2'] = 1;
//                                     player.second.to_do++;
                                    player.second.received = false;
                                }
                                game.timer.sabbotage = SABBOTAGE_TIMEOUT;
                                break;
                            case 'a'://reactor
                                game.sabbo = 10;
                                game.hand_scanners = {0,0};
                                for(auto  &player : game.tasks){
                                    player.second.list['0'] = 1;
                                    player.second.list['1'] = 1;
//                                     player.second.to_do++;
                                    player.second.received = false;
                                }
                                game.timer.sabbotage = SABBOTAGE_TIMEOUT;
                                break;
                            case 'b'://oxygen
                                game.sabbo = 11;
                                for(auto  &player : game.tasks){
                                    player.second.list['3'] = 1;
                                    player.second.list['4'] = 1;
//                                     player.second.to_do++;
                                    player.second.received = false;
                                }
                                game.timer.sabbotage = SABBOTAGE_TIMEOUT;
                                break;
                        }
                        sendReply(sd, i, game);
                    } else if(buffer[0] == 't'&& game.in_progress) {//player wants to do a task
                        char trigger;
                        std::cout << "t"<<i;
                        if(game.position[i].status%2)
                            trigger = gamemap[game.position[i].y][game.position[i].x];
                        else
                            trigger = gamemap[game.ghosts[i].y][game.ghosts[i].x];

                       
                        if(trigger!= ' ') {
                             std::cout<<"Player "<<i<<":" << triggers[trigger][0]<<" "<<triggers[trigger][1]<<" "<<triggers[trigger][2]<<" "<<std::endl;
                            if(triggers[trigger][0]=="util") {
                                if(triggers[trigger][2]=="button"&&!game.timer.button) { //emeregency meeting
                                    game.timer.voting = game.position.size()*300;
                                    for(auto x : game.position) {
                                        if(x.second.status%2)game.votes[x.first] = -1;
                                        else game.votes[x.first] = -2;
                                    }

                                } else if(triggers[trigger][2]=="cams") { //security cameras
                                     if(game.position[i].status%2)game.tasks[i].current=trigger;
                                    game.cameras++;
                                } else { // admin door logs
                                     if(game.position[i].status%2)game.tasks[i].current=trigger;
                                }
                            } else if(triggers[trigger][0]=="task") {
                                if(game.position[i].status >>1%2==0&&game.tasks[i].list[trigger]){
                                    game.tasks[i].current=trigger;
                                    std::cout << "This task is on the list"<<std::endl;   
                                }
                                
                            } else if(triggers[trigger][0]=="sabbo") {
                                if(game.position[i].status%2==1&&game.tasks[i].list[trigger]){
                                    game.tasks[i].current=trigger;
                                    std::cout << "This task is on the list"<<std::endl;  
                                    if(trigger == '0'&&game.sabbo == 10){
                                        game.hand_scanners.first++;
                                        if(game.hand_scanners.second){
                                            //HAND SCAN SUCcessfULL
                                            game.tasks[i].current = 0;
                                            for(auto &player : game.tasks){
                                                player.second.list['0'] = 0;
                                                player.second.list['1'] = 0;
                                                player.second.received = false;
                                            }
                                            game.sabbo = 0;
                                            
                                        }
                                    }else if(trigger == '1'&&game.sabbo == 10){
                                        game.hand_scanners.second++;
                                        if(game.hand_scanners.first){
                                            //HAND SCAN SUCcessfULL
                                            game.tasks[i].current = 0;
                                            for(auto &player : game.tasks){
                                                player.second.list['0'] = 0;
                                                player.second.list['1'] = 0;
                                                player.second.received = false;
                                            }
                                            game.sabbo = 0;
                                        }
                                    }
                                }
                                
                            } else if(triggers[trigger][0]=="vent") {
                                if(game.position[i].status >>1%2){
                                   game.tasks[i].current=trigger; 
                                   game.position[i].x = 0;
                                   game.position[i].y = 0;
                                }
                            }
                        }
                        sendReply(sd, i, game);
                        
                    } else if(buffer[0] == 'l'&& game.in_progress) {//player flips a switch
                        switch(buffer[1]){
                            case '1':
                                game.light_switches[0] = !game.light_switches[0];
                                break;
                            case '2':
                                game.light_switches[1] = !game.light_switches[1];
                                break;
                            case '3':
                                game.light_switches[2] = !game.light_switches[2];
                                break;
                            case '4':
                                game.light_switches[3] = !game.light_switches[3];
                                break;
                            case '5':
                                game.light_switches[4] = !game.light_switches[4];
                                break;
                            
                        }
                        if(game.light_switches[0]&&game.light_switches[1]&&game.light_switches[2]&&game.light_switches[3]&&game.light_switches[4]){
                            game.tasks[i].current = 0;
                            for(auto &player : game.tasks){
                                player.second.list['2'] = 0;
                                player.second.received = false;
                            }
                            game.sabbo = 0;
                        }
                         
                        
                         sendReply(sd, i, game);
                    }else if(buffer[0] == 'e'){//empty message
                        sendReply(sd, i, game);
                    } else if(buffer[0] == 'd'&& game.in_progress) {//player has done a task
                        if(game.tasks[i].current>='a'&&game.tasks[i].current<'o'){//player moves to next vent
                           game.tasks[i].current = vent_connections[game.tasks[i].current];
                           std::cout << i <<": vent next"<<std::endl;
                        }else if(game.tasks[i].current=='5'&&game.sabbo == 8){//player fixed communications
                            game.tasks[i].current = 0;
                            for(auto &player : game.tasks){
                                player.second.list['5'] = 0;
                                player.second.received = false;
                            }
                            game.sabbo = 0;
                        
                        }else if(game.tasks[i].current=='4'&&game.sabbo == 11){//player fixed oxygen in o2
                            game.tasks[i].current = 0;
                            for(auto &player : game.tasks){
                                player.second.list['4'] = 0;
                                player.second.received = false;
                            }
                            if(game.tasks[i].list['3']==0)
                                game.sabbo = 0;
                        
                        }else if(game.tasks[i].current=='3'&&game.sabbo == 11){//player fixed oxygen in admin
                            game.tasks[i].current = 0;
                            for(auto &player : game.tasks){
                                player.second.list['3'] = 0;
                                player.second.received = false;
                            }
                            if(game.tasks[i].list['4']==0)
                                game.sabbo = 0;
                        
                        
                        }else if(game.tasks[i].current=='2'&&game.sabbo == 9){//player fixed lights
                            game.tasks[i].current = 0;
                            for(auto &player : game.tasks){
                                player.second.list['2'] = 0;
                                player.second.received = false;
                            }
                            game.sabbo = 0;
                         }else if(game.tasks[i].current == '0'&&game.sabbo == 10){
                            game.hand_scanners.first--;
                        }else if(game.tasks[i].current == '1'&&game.sabbo == 10){
                            game.hand_scanners.second--;
                        }
                        else{
                            game.tasks[i].done++;
                            game.tasks[i].list[game.tasks[i].current] = 0;
                            game.tasks[i].current = 0;
                        }
                        //sendReply(sd, i, game);
                    } else if(buffer[0] == 'f'&& game.in_progress) {//player has failed a task
                        if(game.tasks[i].current=='6')game.cameras--;
                        else if(game.tasks[i].current>='a'&&game.tasks[i].current<'o'){//player leaves a vent
                            game.position[i].x = vent_positions[game.tasks[i].current].first;
                            game.position[i].y = vent_positions[game.tasks[i].current].second;
                            std::cout << i <<": vent exit"<<std::endl;
                            
                        }else if(game.tasks[i].current == '0'){
                            game.hand_scanners.first--;
                        }else if(game.tasks[i].current == '1'){
                            game.hand_scanners.second--;
                        }
                        game.tasks[i].current = 0;
                        //sendReply(sd, i, game);
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
                            applyMovement(buffer[1], i, game.position, gamemap, game.sabbo, false, game.doorlog);

                            sendReply(sd, i, game);

                        } else {
                            applyMovement(buffer[1], i, game.ghosts, gamemap, game.sabbo, true, game.doorlog);
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
