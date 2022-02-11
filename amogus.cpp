#include "amogus.h"
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

bool readParameters ( int& argc, char** argv, std::string& ip, bool &a, bool &kb ) {
    bool errors = false;
    for ( int i = 1; i < argc - 1; i++ ) {
        if ( not strcmp ( argv[i], "-i" ) ) { //adres ip
            ip = argv[i + 1];

        } else if ( not strcmp ( argv[i], "-a" ) ) { //verbose, wypisuj dodatkowe informacje
            a = true;
        } else if ( not strcmp ( argv[i], "-k" ) ) { //verbose, wypisuj dodatkowe informacje
            kb = true;
        } else if ( not strcmp ( argv[i], "-h" ) ) { //pomoc
            errors = true;
        }
    }
    if ( not strcmp ( argv[argc - 1], "-a" ) ) a = true; //verbose, wypisuj dodatkowe informacje
    if ( not strcmp ( argv[argc - 1], "-k" ) ) kb = true; //verbose, wypisuj dodatkowe informacje
    if ( not strcmp ( argv[argc - 1], "-h" ) ) { //pomoc
        errors = true;
    };

    if ( ip == ""|| ( ip.size() ==2&&ip[0]=='-' ) ) {
        std::cerr << "Nie podano adresu ip"<<std::endl;
        errors = true;
    }

    return errors;
}
void printCenter(std::string text, int limX, int limY, int offX, int offY) {
    int halfText = text.length()/2;
    mvprintw(limY/2+offY, (limX/2)-halfText+offX, text.c_str());
}
void update_all(std::map<int, std::pair<int, int>>  &positions) {
    for(auto ch : positions) {
        attron(COLOR_PAIR(ch.first+1));
        mvprintw(ch.second.second, ch.second.first, "ඞ");
    }
    attron(COLOR_PAIR(20));

}
void drawMap(std::vector<std::string> &gmap, int x, int y) {

    for(int i = 0; i < FOVX; i++) {
        for(int k = 0; k < FOVY; k++) {
            if((k+y-(FOVY/2))>0&&(i+x-(FOVX/2))>0&&(k+y-(FOVY/2))<gmap.size()&&(i+x-(FOVX/2))<gmap[k].size()) {
                move(k+TOPOFFSET, i);
                switch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2))) {
                case '%':
                    printw("█");
                    break;
                case 'Y':
                case ':':
                    attron(COLOR_PAIR(4));//green
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attron(COLOR_PAIR(20));
                    break;
                case 'w':
                case '#':
                    attron(COLOR_PAIR(2));//cyan
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attron(COLOR_PAIR(20));
                    break;
                case '?':
                case 'M':
                    attron(COLOR_PAIR(3));//magenta
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attron(COLOR_PAIR(20));
                    break;
                case '@':
                case '(':
                case ')':
                    attron(COLOR_PAIR(1));//red
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attron(COLOR_PAIR(20));
                    break;

                case 'T':
                    attron(COLOR_PAIR(6));//blue
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attron(COLOR_PAIR(20));
                    break;
                case '}':
                case '{':
                case '!':
                    attron(COLOR_PAIR(5));//yellow
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attron(COLOR_PAIR(20));
                    break;
                default:
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    break;

                }

            }
        }
    }
    //mvprintw((FOVY/2), (FOVX/2), "ඞ");
    mvprintw(FOVY+2, 0, "%d, %d", x, y);
}
void displayRole(int status) {
    if(status>>1%2) { //impostor
        attron(COLOR_PAIR(1));
        mvprintw(5,0, impostor.c_str());
    } else { //crewmate
        attron(COLOR_PAIR(2));
        mvprintw(5,0,crewm8.c_str());
    }
    attron(COLOR_PAIR(20));
}

bool LOS(int xa, int ya, int xb, int yb, std::vector<std::string> &wallmap) {
    //cannot see further than SIGHT_RADIUS
    if(distance(xa,  ya, xb, yb) > SIGHT_RADIUS)return false;
    int dx, dy;
    dx = xa-xb;//x distance
    dy = ya-yb;//y distance
    if(abs(dx)>abs(dy)) { //dx is bigger
        for(int i = 0; i < (abs(dx)-abs(dy))/2; i++) {
            xb+=sgn(dx);
            if(wallmap[yb].at(xb) == '%')return false;
        }
        while(yb != ya) {
            xb+=sgn(dx);
            if(wallmap[yb].at(xb) == '%')return false;
            yb+=sgn(dy);
            if(wallmap[yb].at(xb) == '%')return false;
        }
        while(xb != xa) {
            xb+=sgn(dx);
            if(wallmap[yb].at(xb) == '%')return false;
        }
    } else { //dy is bigger
        for(int i = 0; i < (abs(dy)-abs(dx))/2; i++) {
            yb+=sgn(dy);
            if(wallmap[yb].at(xb) == '%')return false;
        }
        while(xb != xa) {
            yb+=sgn(dy);
            xb+=sgn(dx);
            if(wallmap[yb].at(xb) == '%')return false;
        }
        while(yb != ya) {
            yb+=sgn(dy);
            if(wallmap[yb].at(xb) == '%')return false;
        }
    }
    return true;
}
void cutscene(int colA, int colB, std::string filename) {

    std::vector<std::string> character;
    character = loadMap(filename);
    for(int y = 0; y < character.size(); y++) {
        for(int x = 0; x < character[0].size(); x++) {
            move(y,x);
            switch(character[y].at(x)) {
            case 'O':
            case 'U':
            case 'n':
                attron(COLOR_PAIR(colA));
                addch(character[y].at(x));
                break;
            case 'a':
                attron(COLOR_PAIR(colA));
                printw("▒");//░
                break;
            case 'A':
                attron(COLOR_PAIR(colA));
                printw("▓");
                break;
            case 'b':
                attron(COLOR_PAIR(colB));
                printw("▒");//░
                break;
            case 'B':
                attron(COLOR_PAIR(colB));
                printw("▓");
                break;
            case '#':
                attron(COLOR_PAIR(20));
                printw("█");
                break;
            case 'e':
                attron(COLOR_PAIR(2));
                printw("▓");
                break;
            case 'E':
                attron(COLOR_PAIR(2));
                printw("█");
                break;

            default:
                attron(COLOR_PAIR(20));
                addch(character[y].at(x));
                break;
            }
        }

    }
    refresh();
}

std::vector<std::string> loadMap ( std::string filename ) {
    std::vector<std::string> temp;

    std::ifstream mapStream ( filename );
    if ( mapStream ) {
        std::string line;

        while ( std::getline( mapStream, line) ) {
            temp.push_back(line);

        }
        mapStream.close();
    } else {
        std::cerr << "Błąd w otwieraniu pliku " << filename << std::endl;
    }
    return temp;
}

std::map<char,std::vector<std::string>> loadLabels ( std::string filename ) {
    std::map<char,std::vector<std::string>> temp;

    std::ifstream mapStream ( filename );
    if ( mapStream ) {
        std::string type, loc, attr;
        char c;
        while ( mapStream >> c >> type >> loc >> attr) {
            temp[c] = {type, loc, attr};
        }
        mapStream.close();
    } else {
        std::cerr << "Błąd w otwieraniu pliku " << filename << std::endl;
    }
    return temp;
}
keyBinds loadKeyBinds ( std::string filename ) {
    keyBinds temp;
    std::ifstream fileStream ( filename );
    if ( fileStream ) {
        fileStream >> temp.moveN >> temp.moveE >> temp.moveW >> temp.moveS;
        fileStream >> temp.moveNE >> temp.moveNW >> temp.moveSE >> temp.moveSW;
        fileStream >> temp.middle;
        fileStream >> temp.kill >> temp.use >> temp.report >> temp.ready;

        fileStream.close();
    } else {
        std::cerr << "Błąd w otwieraniu pliku "<< filename << std::endl;
    }
    return temp;
}

int distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x1-x2, 2)+pow(y1-y2, 2));
}

void await(int sockfd, const int id, std::map<int, crewmate>  &positions, crewmate &ghost, std::vector<std::string> &gamemap, std::vector<std::string> &wallmap,playermodel model, std::map<char,std::vector<std::string>> &triggers, bool kb)
{
    bool game_started = false, murdered=false, voting=false;
    int role_countdown = 0, killing_cutscene_countdown=0, murderer=-1, murder_cutscene_countdown=0, ejection_cutscene_countdown=0, winner_countdown=0;
    std::map<int, int> previous_status, votes;
    int winner = 0;
    drawMap(gamemap, positions[id].x, positions[id].y);
    refresh();
    char buff[MAX];
    int got_ch = 0, voting_time = 0;
    int killing_timeout= 1;
    const keyBinds kBinds = loadKeyBinds("keybinds.txt");
    for (;;) {


        refresh();
        got_ch = getch();

        bzero(buff, sizeof(buff));
        if(role_countdown) {
            strcpy(buff, "u\n");
        } else if(!kb) {
            switch(got_ch) {
            case KEY_UP:
            case '8':
                if(voting) strcpy(buff, "v8\n");
                else strcpy(buff, "m8\n");
                break;
            case KEY_LEFT:
            case '4':
                if(voting)strcpy(buff, "v4\n");
                else strcpy(buff, "m4\n");
                break;
            case KEY_RIGHT:
            case '6':
                if(voting)strcpy(buff, "v6\n");
                else strcpy(buff, "m6\n");
                break;
            case '5':
                if(voting)strcpy(buff, "v5\n");
                else strcpy(buff, "u\n");

                break;
            case KEY_DOWN:
            case '2':
                if(voting) strcpy(buff, "v2\n");
                else  strcpy(buff, "m2\n");
                break;
            case '1':
                if(voting) strcpy(buff, "v1\n");
                else strcpy(buff, "m1\n");
                break;
            case '3':
                if(voting) strcpy(buff, "v3\n");
                else strcpy(buff, "m3\n");
                break;
            case '7':
                if(voting)strcpy(buff, "v7\n");
                else strcpy(buff, "m7\n");
                break;
            case '9':
                if(voting)strcpy(buff, "v9\n");
                else strcpy(buff, "m9\n");
                break;
            case ERR:
                strcpy(buff, "u\n");
                //mvprintw(0,0,"Timeout ");
                break;
            case 'x':
                if(positions[id].status>>1 % 2 && !killing_timeout) {
                    strcpy(buff, "k\n");
                    killing_cutscene_countdown = 20;
                    for(auto el : positions) {
                        previous_status[el.first] = el.second.status;
                    }
                } else {
                    strcpy(buff, "u\n");
                }
                break;
            case 'c':
                if(positions[id].status % 2)strcpy(buff, "c\n");
                else strcpy(buff, "u\n");
                break;
            case 'z':
                if(game_started) strcpy(buff, "t\n");
                else strcpy(buff, "u\n");

                break;
            case 'r':
                strcpy(buff, "r\n");
                break;
            default:
                strcpy(buff, "u\n");
                //mvprintw(0, 0, "pressed: %d", a);
                break;
            }
        } else {
            if(got_ch == kBinds.moveN||got_ch == '8') {
                if(voting) strcpy(buff, "v8\n");
                else strcpy(buff, "m8\n");
            } else if(got_ch == kBinds.moveW||got_ch=='4') {
                if(voting)strcpy(buff, "v4\n");
                else strcpy(buff, "m4\n");
            } else if(got_ch == kBinds.moveE||got_ch=='6') {
                if(voting)strcpy(buff, "v6\n");
                else strcpy(buff, "m6\n");
            } else if(got_ch == kBinds.middle||got_ch=='5') {
                if(voting)strcpy(buff, "v5\n");
                else strcpy(buff, "u\n");
            } else if(got_ch == kBinds.moveS||got_ch=='2') {
                if(voting) strcpy(buff, "v2\n");
                else  strcpy(buff, "m2\n");
            } else if(got_ch == kBinds.moveSW||got_ch=='1') {
                if(voting) strcpy(buff, "v1\n");
                else strcpy(buff, "m1\n");
            } else if(got_ch == kBinds.moveSE||got_ch=='3') {
                if(voting) strcpy(buff, "v3\n");
                else strcpy(buff, "m3\n");
            } else if(got_ch == kBinds.moveNW||got_ch=='7') {
                if(voting)strcpy(buff, "v7\n");
                else strcpy(buff, "m7\n");
            } else if(got_ch == kBinds.moveNE||got_ch=='9') {
                if(voting)strcpy(buff, "v9\n");
                else strcpy(buff, "m9\n");
            } else if(got_ch == ERR) {
                strcpy(buff, "u\n");
            } else if(got_ch == kBinds.kill) {
                if(positions[id].status>>1 % 2 && !killing_timeout) {
                    strcpy(buff, "k\n");
                    killing_cutscene_countdown = 20;
                    for(auto el : positions) {
                        previous_status[el.first] = el.second.status;
                    }
                } else {
                    strcpy(buff, "u\n");
                }
            } else if(got_ch == kBinds.report) {
                if(positions[id].status % 2)strcpy(buff, "c\n");
                else strcpy(buff, "u\n");
            } else if(got_ch == kBinds.use) {
                if(game_started) strcpy(buff, "t\n");
                else strcpy(buff, "u\n");
            } else if(got_ch == kBinds.ready) {
                strcpy(buff, "r\n");
            } else {
                strcpy(buff, "u\n");
            }

        }
//         clear();
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));

        mvprintw(FOVY+1,0,"From Server : %s   ", buff);
        std::stringstream decoder;
        decoder << buff;
        int a=0, b=0, c=0, cnt=0;

        std::string numb = "";
        char mode;
        decoder >> mode;
        if (mode == 'p') {
            if(murdered) {
                murdered = false;
            }
            if(voting) {
                voting = false;
                ejection_cutscene_countdown = 40;
                clear();
            }
            decoder >> killing_timeout;
            while(decoder >> a >> b >> c) {
                mvprintw(positions[cnt].y, positions[cnt].x, " ");
                positions[cnt] = {a, b, c};
                cnt++;
            }
            if(!game_started) {
                clear();
                role_countdown = 40;//How many ticks should you display the message for
                game_started = true;
            }
        }
        if (mode == 'v') {

            if(!voting) {
                voting = true;
                for(auto el : positions) {
                    previous_status[el.first] = el.second.status;
                }
                clear();
            }
            decoder >> voting_time;
            while(decoder >> a) {

                votes[cnt] = a;
                cnt++;
            }

        }
        else if (mode == 'g') {

            if(!murdered) {

                murdered=true;
                murder_cutscene_countdown = 20;
            }
            if(voting) {
                voting = false;
                ejection_cutscene_countdown = 40;
                murder_cutscene_countdown = 0;
                clear();
            }
            decoder >> ghost.x >> ghost.y >> murderer;
            while(decoder >> a >> b >> c) {
                mvprintw(positions[cnt].y, positions[cnt].x, " ");
                positions[cnt] = {a, b, c};

                cnt++;
            }
        } else if (mode == 'w') {
            if(voting) {
                voting = false;
                clear();
            }
            decoder >> c >> a >> b;
            if(c>0&&!winner) {
                winner = c;
                winner_countdown = 40;
            }
            game_started = false;
            numb = std::to_string(a)+"/"+std::to_string(b);

            while(decoder >> a >> b >> c) {

                positions[cnt] = {a, b, c};
                mvprintw(positions[cnt].y, positions[cnt].x, " ");
                cnt++;
            }
        }

        if(role_countdown) {
            displayRole(positions[id].status);
            winner = 0;
            role_countdown--;
        } else if(winner_countdown) {
            winner_countdown--;
            if(winner==1)printCenter("Impostors win.", FOVX, FOVY);
            else printCenter("Crewmates win.", FOVX, FOVY);
            if(!winner_countdown)clear();
        } else if(ejection_cutscene_countdown) {
            ejection_cutscene_countdown--;
            int victim = -1;
            for(auto el : previous_status) {
                if(el.second != positions[el.first].status) {
                    victim = el.first;
                }
            }
            if(victim >= 0) {
                cutscene(1, victim+1, "ejected.txt");
                if(positions[victim].status>>1%2) {
                    printCenter("Player "+std::to_string(victim)+" was the impostor.", FOVX, 2);
                } else {
                    printCenter("Player "+std::to_string(victim)+" was not the impostor.", FOVX, 2);
                }
            } else {
                cutscene(1, 1, "empty.txt");
                printCenter("Nobody was ejected.", FOVX, 2);

            }
            if(ejection_cutscene_countdown==0)clear();
        } else if(killing_cutscene_countdown) {
            killing_cutscene_countdown--;
            for(auto el : previous_status) {
                if(el.second != positions[el.first].status) {

                    cutscene(id+1, el.first+1, "murder.txt");
                    goto kill_done;//note to self: do something better with this
                }
            }
            killing_cutscene_countdown = 0;
kill_done:
            ;
            if(killing_cutscene_countdown==0)clear();
        } else if(murder_cutscene_countdown) {
            murder_cutscene_countdown--;
            if(murderer == -1) {
                cutscene(murderer+1,id+1, "ejected.txt");
            }
            else {
                cutscene(murderer+1,id+1, "murder.txt");
            }
            if(murder_cutscene_countdown==0)clear();
        } else if(voting) {
            printCenter("VOTING TIME: "+std::to_string(voting_time/10*positions.size()), FOVX, 1);
            printCenter("Who is the impostor?", FOVX, 1, 0, 1);
            for(auto crewm8 : positions) {

                attron(COLOR_PAIR(crewm8.first+1));
                if(crewm8.second.status%2) {
                    mvprintw(3+(crewm8.first),3, model.alive.c_str());
                } else {
                    mvprintw(3+(crewm8.first),3, model.body.c_str());
                }
                mvprintw(3+(crewm8.first),5, "Player %d:               ", crewm8.first+1);
                move(3+(crewm8.first),16);
                for(auto accuser : votes) {
                    if(accuser.second-1 == crewm8.first) {
                        attron(COLOR_PAIR(accuser.first+1));
                        printw(model.alive.c_str());
                        printw(" ");
                    }
                }

            }
            attron(COLOR_PAIR(20));
        } else {

            if(positions[id].status%2) {
                drawMap(gamemap, positions[id].x, positions[id].y);
                char trig = wallmap[positions[id].y][positions[id].x];
                if(trig != ' ') {
                    mvprintw(FOVY+3, 0, "current trigger type: %s    ", triggers[trig][0].c_str());
                    mvprintw(FOVY+4, 0, "location: %s      ", triggers[trig][1].c_str());
                    mvprintw(FOVY+5, 0, "argument: %s      ", triggers[trig][2].c_str());
                }
                int y = positions[id].y;
                int x = positions[id].x;
                for(auto ch : positions) {
                    int posx = ch.second.x-x+(FOVX/2);
                    int posy = ch.second.y-y+(FOVY/2);

                    if(posx<FOVX&&posy<FOVY&&LOS(x, y, ch.second.x, ch.second.y, wallmap)) {
                        attron(COLOR_PAIR(ch.first+1));
                        if(ch.second.status %2)
                            mvprintw(posy+TOPOFFSET, posx, model.alive.c_str());
                        else {
                            mvprintw(posy+TOPOFFSET, posx, model.body.c_str());
                        }
                        attron(COLOR_PAIR(20));
                    }
                }

            } else {
                drawMap(gamemap, ghost.x, ghost.y);
                int y = ghost.y;
                int x = ghost.x;
                attron(COLOR_PAIR(id+1));
                mvprintw((FOVY/2)+TOPOFFSET, (FOVX/2), model.ghost.c_str());
                attron(COLOR_PAIR(20));
                for(auto ch : positions) {
                    int posx = ch.second.x-x+(FOVX/2);
                    int posy = ch.second.y-y+(FOVY/2);

                    if(posx<FOVX&&posy<FOVY&&LOS(x, y, ch.second.x, ch.second.y, wallmap)) {
                        attron(COLOR_PAIR(ch.first+1));
                        if(ch.second.status %2)
                            mvprintw(posy+TOPOFFSET, posx, model.alive.c_str());
                        else {
                            mvprintw(posy+TOPOFFSET, posx, model.body.c_str());
                        }
                        attron(COLOR_PAIR(20));
                    }
                }

            }
            if(numb!="") {
                printCenter("Waiting for players.", FOVX, FOVY, 0, 0);
                printCenter(numb, FOVX, FOVY, 0, 2);
            }


        }

    }
}


bool collisionCheck(int id, std::map<int, crewmate> &pos, std::vector<std::string> &gamemap, bool ghost) {
    auto curpos = pos[id];
    if(curpos.x <0 || curpos.y < 0)return true;
    //std::cout << "znak na "<<curpos.second <<":" << curpos.first << "to: "<< (int)gamemap[curpos.first].size()<<std::endl;
    if(!ghost) {
        for(auto ch : pos) {
            if(ch.first != id) {
                if((ch.second.x == pos[id].x)&&(ch.second.y == pos[id].y))return true;
            }
        }
        if(gamemap[curpos.y].at(curpos.x)== ' ')return false;
        else if(gamemap[curpos.y].at(curpos.x)>'?'&&gamemap[curpos.y].at(curpos.x)<'[') return false;
        else if(gamemap[curpos.y].at(curpos.x)>'_'&&gamemap[curpos.y].at(curpos.x)<'{') return false;
        else if(gamemap[curpos.y].at(curpos.x)>'/'&&gamemap[curpos.y].at(curpos.x)<':') return false;
        else return true;
    } else return false;
}
void startGame(status &game) {
    game.position[rand()%game.position.size()].status += 2;//make a random player the impostor
    game.in_progress = true;
    game.winner = -1;
    std::cout << "starting game" <<std::endl;
    return;
}
/**
 * This function puts all dead crewmates into the corner of the map so no one can report them again.
 * @param[out] game.position map of crewmates' game.positions
 */
void cleanDeadBodies(std::map<int, crewmate> &position) {

    for(auto &cm8:position) {

        if(cm8.second.status%2==0) { //dead bodies go in the corner

            cm8.second.x = 0;
            cm8.second.y = 0;
        } else { //crewmates go back to cafeteria
            cm8.second.x = 137+2*cm8.first;
            cm8.second.y = 7;
        }
    }
}
void votesResult(status &game) {
    std::map<int, int> votecount;
    for(auto crewm8 : game.votes) {
        votecount[crewm8.second]++;
    }
    bool tie=false;
    std::pair<int, int>highest = {-1, 0};
    for(auto vote : votecount) {
        if(vote.first >= 0) {
            if(vote.second > highest.second) {
                highest = vote;
                tie = false;
            } else if(vote.second==highest.second) {
                tie = true;
            }
        }
    }
    std::cout << "Voting results: tie?"<<tie<<" voted off: "<<highest.first<<std::endl;
    if(!tie && highest.first >0 ) {
        game.position[highest.first-1].status--;
        game.ghosts[highest.first-1] =game.position[highest.first-1];
        game.ghosts[highest.first-1].status = -1;
        int survivors = 0;
        int impostors = 0;
        for(auto cm : game.position) {
            if(cm.second.status==1)survivors++;
            else if(cm.second.status==3) {
                survivors++;
                impostors++;
            }
        }
        if(!impostors) {
            std::cout << "crewmates win"<<std::endl;
            game.winner = 2;
            std::cout <<"Reseting game"<<std::endl;
            for(auto cm : game.position) {
                game.position[cm.first] = {137+2*cm.first, 7, 1};
                game.timer.kill[cm.first] = INITIAL_KTIMEOUT;
                game.ready[cm.first] = false;
                game.in_progress = false;
            }
        }
        else if(survivors<3) { //reset the game to waiting state
            std::cout << "impostors win"<<std::endl;
            game.winner = 1;
            std::cout <<"Reseting game"<<std::endl;
            for(auto cm : game.position) {
                game.position[cm.first] = {137+2*cm.first, 7, 1};
                game.timer.kill[cm.first] = INITIAL_KTIMEOUT;
                game.ready[cm.first] = false;
                game.in_progress = false;
            }
        }

    }
}
void applyMovement(const char ch,  const int i, std::map<int, crewmate>&p, std::vector<std::string> &gamemap, const bool ghost) {
    switch(ch) {
    case '6':
        p[i].x++;
        if(collisionCheck(i, p, gamemap, ghost))
            p[i].x--;
        break;
    case '2':
        p[i].y++;
        if(collisionCheck(i, p, gamemap, ghost))
            p[i].y--;
        break;
    case '4':
        p[i].x--;
        if(collisionCheck(i, p, gamemap, ghost))
            p[i].x++;
        break;
    case '8':
        p[i].y--;
        if(collisionCheck(i, p, gamemap, ghost))
            p[i].y++;
        break;
    case '3':
        p[i].x++;
        p[i].y++;
        if(collisionCheck(i, p, gamemap, ghost))
        {
            p[i].x--;
            p[i].y--;
        }
        break;
    case '1':

        p[i].y++;
        p[i].x--;
        if(collisionCheck(i, p, gamemap, ghost))
        {
            p[i].x++;
            p[i].y--;
        }
        break;
    case '7':
        p[i].x--;
        p[i].y--;
        if(collisionCheck(i, p, gamemap, ghost))
        {
            p[i].x++;
            p[i].y++;
        }
        break;
    case '9':
        p[i].y--;
        p[i].x++;
        if(collisionCheck(i, p, gamemap, ghost))
        {
            p[i].x--;
            p[i].y++;
        }
        break;

    }

}
void sendReply(int sd,int i,  status &game) {
    std::string msg;
    if(game.timer.voting) {
        msg = "v ";
        msg += std::to_string(game.timer.voting) + " ";
        bool empty_votes = false;
        for(auto mate : game.votes) {
            if(mate.second==-1) {
                empty_votes = true;

            }
            msg += std::to_string(mate.second) + " ";
        }
        if(!empty_votes) {
            game.timer.voting = 0;
            votesResult(game);
            cleanDeadBodies(game.position);

        }
    }
    else {
        if(game.in_progress) {

            if(game.position[i].status%2) {
                msg ="p "+std::to_string(game.timer.kill[i])+" ";

            } else {
                msg ="g ";
                msg += std::to_string(game.ghosts[i].x)+" "+std::to_string(game.ghosts[i].y)+" "+std::to_string(game.ghosts[i].status)+" ";
            }
        } else {
            msg ="w "+std::to_string(game.winner)+ " ";
            int count = 0;
            for(auto el : game.ready) {
                if(el.second)count++;
            }
            msg += std::to_string(count) + " ";
            msg += std::to_string(game.ready.size())+ " ";
            if(count == game.ready.size())startGame(game);
        }
        for(auto el : game.position) {
            msg += std::to_string(el.second.x) + " " + std::to_string(el.second.y) + " "+ std::to_string(el.second.status) + " " ;
        }
    }
    send(sd, msg.c_str(), msg.size(), 0 );
}


