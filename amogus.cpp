#include "amogus.h"
#include "draw.h"
#include "tasks.h"
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


bool lookForObstacles(int xa, int ya, int xb, int yb, std::vector<std::string> &wallmap, char obs){
    int dx, dy;
    dx = xa-xb;//x distance
    dy = ya-yb;//y distance
    if(abs(dx)>abs(dy)) { //dx is bigger
        for(int i = 0; i < (abs(dx)-abs(dy))/2; i++) {
            xb+=sgn(dx);
            if(wallmap[yb].at(xb) == obs)return false;
        }
        while(yb != ya) {
            xb+=sgn(dx);
            if(wallmap[yb].at(xb) == obs)return false;
            yb+=sgn(dy);
            if(wallmap[yb].at(xb) == obs)return false;
        }
        while(xb != xa) {
            xb+=sgn(dx);
            if(wallmap[yb].at(xb) == obs)return false;
        }
    } else { //dy is bigger
        for(int i = 0; i < (abs(dy)-abs(dx))/2; i++) {
            yb+=sgn(dy);
            if(wallmap[yb].at(xb) == obs)return false;
        }
        while(xb != xa) {
            yb+=sgn(dy);
            xb+=sgn(dx);
            if(wallmap[yb].at(xb) == obs)return false;
        }
        while(yb != ya) {
            yb+=sgn(dy);
            if(wallmap[yb].at(xb) == obs)return false;
        }
    }
    return true;
}
bool LOS(int xa, int ya, int xb, int yb, std::vector<std::string> &wallmap, int sabbo) {
    //cannot see further than SIGHT_RADIUS
    if(sabbo != 9){
        if(distance(xa,  ya, xb, yb) > SIGHT_RADIUS)return false;
    }else{
        if(distance(xa,  ya, xb, yb) > SIGHT_RADIUS/3)return false;
    }
    if(lookForObstacles(xa, ya, xb, yb, wallmap, '%')){
       switch(sabbo){
           case 1:
               return lookForObstacles(xa, ya, xb, yb, wallmap, '$');
               break;
           case 2:
               return lookForObstacles(xa, ya, xb, yb, wallmap, '&');
               break;
           case 3:
               return lookForObstacles(xa, ya, xb, yb, wallmap, '#');
               break;
           case 4:
               return lookForObstacles(xa, ya, xb, yb, wallmap, '`');
               break;
           case 5:
               return lookForObstacles(xa, ya, xb, yb, wallmap, '>');
               break;
           case 6:
               return lookForObstacles(xa, ya, xb, yb, wallmap, '<');
               break;
           case 7:
               return lookForObstacles(xa, ya, xb, yb, wallmap, '^');
               break;
           default: 
               return true;//no doors
               break;
    } 
    }else return false;
    
}
void wait(int n){
    while(n){
    getch();
    n--; 
    }
    
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
std::string key(int ch){
    std::string temp;
    switch(ch){
        case ' ':
            temp = "Spc";
            break;
        case 27:
            temp = "Esc";
            break;
        default:
            temp = (char)ch;
            break;
    }
    return "["+temp+"]";
}
void loadKeyBinds ( std::string filename, keyBinds &temp ) {
    std::ifstream fileStream ( filename );
    if ( fileStream ) {
        fileStream >> temp.moveN >> temp.moveE >> temp.moveW >> temp.moveS;
        fileStream >> temp.moveNE >> temp.moveNW >> temp.moveSE >> temp.moveSW;
        fileStream >> temp.middle;
        fileStream >> temp.kill >> temp.use >> temp.report >> temp.sabbotage;
        fileStream >> temp.quit;

        fileStream.close();
    } else {
        std::cerr << "Błąd w otwieraniu pliku "<< filename << std::endl;
    }
}

int distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x1-x2, 2)+pow(y1-y2, 2));
}
int randInt( int low, int high){
    return low+rand()%(high-low);
}


void await(int sockfd, const int id, std::map<int, crewmate>  &positions, crewmate &ghost, std::vector<std::string> &gamemap, std::vector<std::string> &wallmap,playermodel model, std::map<char,std::vector<std::string>> &triggers, bool kb, taskStruct &tasks)
{
    int sabbo = 0, sabbo_countdown=0;
    bool game_started = false, murdered=false, voting=false;
    int role_countdown = 0, killing_cutscene_countdown=0, murderer=-1, murder_cutscene_countdown=0, ejection_cutscene_countdown=0, winner_countdown=0;
    int loading=1;
    int sample_countdown = -1;
    std::map<int, int> previous_status, votes;
    int winner = 0, tasksPercent=0;
    //drawMap(gamemap, positions[id].x, positions[id].y);
    //refresh();
    char buff[MAX];
    tasks.current = 0;
    int got_ch = 0, voting_time = 0;
    int killing_timeout= 1;
    keyBinds kBinds;
    if(kb)loadKeyBinds("keybinds.txt", kBinds);
    else {//defaullt keyBinds
        kBinds.kill = 'x';
        kBinds.middle = 5;
        kBinds.moveE = KEY_RIGHT;
        kBinds.moveN = KEY_UP;
        kBinds.moveW = KEY_LEFT;
        kBinds.moveS = KEY_DOWN;
        kBinds.sabbotage = 'r';
        kBinds.report = 'c';
        kBinds.use = 'z';
        kBinds.quit = 'q';
    }
    for (;;) {


        refresh();
        got_ch = getch();
        if(sample_countdown>0)sample_countdown--;
        bzero(buff, sizeof(buff));
        if(role_countdown) {
            strcpy(buff, "u\n");

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
                else strcpy(buff, "e\n");
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
            } else if(got_ch == kBinds.sabbotage) {
                if(game_started&&positions[id].status>>1%2) pickSabbotage(buff, sabbo, sabbo_countdown, kBinds);
                else strcpy(buff, "u\n");
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
                    strcpy(buff, "e\n");
                }
            } else if(got_ch == kBinds.report) {
                if(positions[id].status % 2)strcpy(buff, "c\n");
                else strcpy(buff, "e\n");
            } else if(got_ch == kBinds.use) {
                if(game_started) strcpy(buff, "t\n");
                else strcpy(buff, "r\n");                
            } else if(got_ch == kBinds.quit) {
                endwin();
                close(sockfd);
                break;
            } else {
                strcpy(buff, "e\n");
            }

        }
//         clear();
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));

       // mvprintw(FOVY+1,0,"From Server : %s   ", buff);
        std::stringstream decoder;
        decoder << buff;
        int a=0, b=0, c=0, cnt=0;
        int cams = 0;
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
            
            decoder >> tasksPercent >> killing_timeout >> cams >> sabbo >> sabbo_countdown;
            while(decoder >> a >> b >> c) {
                mvprintw(positions[cnt].y, positions[cnt].x, " ");
                positions[cnt] = {a, b, c};
                cnt++;
            }
            if(!game_started) {
                clear();
                role_countdown = 30;//How many ticks should you display the message for
                game_started = true;
            }
        }else if (mode == 'v') {

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
        } else if (mode == 'q'){
            decoder >> a;
            game_started = true;
            tasks.current = (char)a;
            
            
            
        } else if (mode == 'g') {
            int cnt = 0;
            if(!murdered) {

                murdered=true;
                murder_cutscene_countdown = 20;
            }
            if(!game_started) {
                game_started = true;
            }
            if(voting) {
                voting = false;
                ejection_cutscene_countdown = 40;
                murder_cutscene_countdown = 0;
                clear();
            }
            decoder >> tasksPercent;
            drawTaskbar(tasksPercent);
            decoder >> ghost.x >> ghost.y >> murderer >> sabbo >> sabbo_countdown;
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
            sabbo = 0;
            sabbo_countdown = 0;
            if(tasks.list.size()){
                tasks.list.clear();
                tasksPercent = 0;
            }
            sample_countdown = -1;
            decoder >> c >> a >> b;
            if(c>0&&!winner) {
                winner = c;
                winner_countdown = 20;
            }
            game_started = false;
            numb = std::to_string(a)+"/"+std::to_string(b);

            while(decoder >> a >> b >> c) {

                positions[cnt] = {a, b, c};
                mvprintw(positions[cnt].y, positions[cnt].x, " ");
                cnt++;
            }
        } else if (mode == 't') {
            loading = 1;
            tasks.list.clear();
            while(decoder >> a >> b) {
                tasks.list[(char)a] = b;
            }
            tasks.current = 0;
            tasks.done = 0;
            
//             if(tasks.list['0']==0){
//                 tasks.list.erase('0');
//             }
//             if(tasks.list['1']==0){
//                 tasks.list.erase('1');
//             }
//             if(tasks.list['2']==0){
//                 tasks.list.erase('1');
//             }
//             if(tasks.list['3']==0){
//                 tasks.list.erase('3');
//             }
//             if(tasks.list['4']==0){
//                 tasks.list.erase('4');
//             }
//             if(tasks.list['5']==0){
//                 tasks.list.erase('5');
//             }
            
        }
        if (loading){
            printCenter("       Loading       ",FOVX, FOVY);
            loading--;
        }else if(role_countdown) {
            displayRole(positions[id].status);
            winner = 0;
            role_countdown--;
            if(role_countdown==0)
                clear();
        } else if(winner_countdown) {
            winner_countdown--;
            clear();
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
        }else if(tasks.current){
            bool success = false;
            switch(tasks.current){
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n': //vent
                    success = handleVent(tasks, triggers, kBinds, positions, gamemap,wallmap, model,  sockfd, id, sabbo);
                    break;
                case 't'://accept diverted power
                case 'z':
                case 'C':
                case 'K':
                case 'N':
                case 'R':
                case 'T':
                case 'Z':
                    if(!tasks.list['x'])success = handleTask(tasks, triggers, kBinds, positions, gamemap,wallmap, model,  sockfd, id, sample_countdown, sabbo);
                    break;
                case 'B':
                case 'E':
                    if(!tasks.list['U'])
                        success = true;
                    break;
                case 'U'://fuel storage
                    success = true;
                    break;
                case 'W':
                    if(!(tasks.list['M'])&&!(tasks.list['r']))
                        success = handleTask(tasks, triggers, kBinds, positions, gamemap, wallmap, model, sockfd, id, sample_countdown, sabbo);
                    break;
                default:
                    success = handleTask(tasks, triggers, kBinds, positions, gamemap, wallmap, model,  sockfd, id, sample_countdown, sabbo);
                    break;
            }
            
            if(success){
                tasks.list[tasks.current] = 0;
                strcpy(buff, "d\n");
            }else{
                
                strcpy(buff, "f\n");
            }
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            tasks.current = 0;
            
        } else {
            //getch();
            printTasks(tasks, triggers, positions[id].status, sample_countdown, sabbo);
            if(positions[id].status%2) {
                drawMap(gamemap, wallmap, positions[id].x, positions[id].y, sabbo, cams);
                char trig = wallmap[positions[id].y][positions[id].x];
                std::string controls;
                controls += key(kBinds.moveN);
                controls += key(kBinds.moveW);
                controls += key(kBinds.moveE);
                controls += key(kBinds.moveS);
                controls += "move ";
                controls += key(kBinds.moveNW);
                controls += key(kBinds.moveNE);
                controls += key(kBinds.moveSW);
                controls += key(kBinds.moveSE);
                controls += "diagonal ";
                if(game_started){
                controls += key(kBinds.use);
                controls += "use ";
                controls += key(kBinds.report);
                controls += "report ";
                }else{
                controls += key(kBinds.use);
                controls += "ready ";
                }
                controls += key(kBinds.quit);
                controls += "quit ";
                
                if(positions[id].status == 3){
                    controls += key(kBinds.kill);
                    controls += "kill ";
                    controls += key(kBinds.sabbotage);
                    controls += "sabbotage ";
                    attron(COLOR_PAIR(8));
                    move(FOVY+2, 0);
                    if(killing_timeout)printw("Killing timeout: %d ", killing_timeout/10);
                    if(sabbo_countdown)printw(" Sabbotage timeout %d  ", sabbo_countdown/10);
                   attron(COLOR_PAIR(20));
                 if(!killing_timeout&&!sabbo_countdown)printw("                                          ");
                   
                    
                }
                move(FOVY+1, 0);
                attron(COLOR_PAIR(14));
                printw(controls.c_str());
                 attron(COLOR_PAIR(20));
                if(trig != ' ') {
                    mvprintw(FOVY+3, 0, "current trigger type: %s    ", triggers[trig][0].c_str());
                    mvprintw(FOVY+4, 0, "location: %s      ", triggers[trig][1].c_str());
                    mvprintw(FOVY+5, 0, "argument: %s      ", triggers[trig][2].c_str());
                }
                int y = positions[id].y;
                int x = positions[id].x;
                drawCharacters( x, y, positions, wallmap,model, sabbo);

            } else {
                drawMap(gamemap, wallmap, ghost.x, ghost.y, sabbo);
                char trig = wallmap[ghost.y][ghost.x];
                int y = ghost.y;
                int x = ghost.x;
                attron(COLOR_PAIR(id+1));
                mvprintw((FOVY/2)+TOPOFFSET, (FOVX/2), model.ghost.c_str());
                attron(COLOR_PAIR(20));
                drawCharacters( x, y, positions, wallmap,model, sabbo);
                if(trig != ' ') {
                    mvprintw(FOVY+3, 0, "current trigger type: %s    ", triggers[trig][0].c_str());
                    mvprintw(FOVY+4, 0, "location: %s      ", triggers[trig][1].c_str());
                    mvprintw(FOVY+5, 0, "argument: %s      ", triggers[trig][2].c_str());
                }

            }
            if(numb!="") {
                printCenter("Waiting for players.", FOVX, FOVY, 0, 0);
                printCenter(numb, FOVX, FOVY, 0, 2);
            }
            drawTaskbar(tasksPercent);

            if(sabbo == 9){
                attron(COLOR_PAIR(1));
                printCenter("Lights out!", FOVX, FOVY/2);
                attron(COLOR_PAIR(20));
            }else if(sabbo == 10){
                if(sabbo_countdown%40>20)attron(COLOR_PAIR(1));
                else attron(COLOR_PAIR(8));
                printCenter(" Reactor overload! ", FOVX, FOVY/2);
                printCenter(" Core meltdown in: "+std::to_string(sabbo_countdown/(10*positions.size()))+" ", FOVX, FOVY/2, 0, 1);
                attron(COLOR_PAIR(20));
            }else if(sabbo == 11){
                if(sabbo_countdown%40>20)attron(COLOR_PAIR(1));
                else attron(COLOR_PAIR(8));
                printCenter(" Oxygen depleted! ", FOVX, FOVY/2);
                printCenter(" Reserves last for: "+std::to_string(sabbo_countdown/(10*positions.size()))+" ", FOVX, FOVY/2, 0, 1);
                attron(COLOR_PAIR(20));
            }
        }

    }
}
void drawCharacters(const int x, const int y, std::map<int, crewmate>  &positions, std::vector<std::string> &wallmap, playermodel model, int sabbo){
    for(auto ch : positions) {
        int posx = ch.second.x-x+(FOVX/2);
        int posy = ch.second.y-y+(FOVY/2);

        if(posx<FOVX&&posy<FOVY&&LOS(x, y, ch.second.x, ch.second.y, wallmap, sabbo)) {
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
void printLine(std::string label, std::string desc, int &line, int color){
    
    mvprintw(line, FOVX, "|[");
    if(label=="Y"||label=="::")attron(COLOR_PAIR(4));
    if(label=="M"||label=="?")attron(COLOR_PAIR(3));
    if(label=="T")attron(COLOR_PAIR(6));
    if(label=="#"||label=="w")attron(COLOR_PAIR(2));
    if(label=="-()"||label=="<()"||label=="(( ))"||label=="@")attron(COLOR_PAIR(1));
    if(label=="}{"||label=="!"||label=="[!]")attron(COLOR_PAIR(5));
    attron(A_BOLD);
    printw(label.c_str());
    attron(COLOR_PAIR(20));
    attroff(A_BOLD);
    printw("]");
    attron(COLOR_PAIR(color));
    printw(desc.c_str());
    line++;
    attron(COLOR_PAIR(20));
    printw("             ");
}
void printTasks(taskStruct &tasks, std::map<char,std::vector<std::string>> &triggers, int status, int sample_countdown, int sabbo){
    int line = 0;
    if(!(status%2)){
            attron(COLOR_PAIR(1));
            mvprintw(line, FOVX, "|You are dead. Finish your tasks.       ");
            line++;
            attron(COLOR_PAIR(20));
    }
    if(sabbo == 8){
        attron(COLOR_PAIR(20));
        mvprintw(line, FOVX, "|");
        attron(COLOR_PAIR(1));
        printw("Communications system sabbotaged!             ");line++;
        attron(COLOR_PAIR(20));
        mvprintw(line, FOVX, "|");
         attron(COLOR_PAIR(1));
        printw("Unable to display tasks             ");line++;
        attron(COLOR_PAIR(20));
        printLine("[==]","Restore connection in communications", line, 20-16*!tasks.list['5']);
        while(line<FOVY){
            mvprintw(line, FOVX, "|                              ");
            line++;
        }
        
        attron(COLOR_PAIR(20)); 
        return;
    }
    if(status>>1%2){
        
        attron(COLOR_PAIR(1));
        mvprintw(line, FOVX, "|Sabbotage and kill                 ");
        attron(COLOR_PAIR(20));
        line++;
        mvprintw(line, FOVX, "|Fake tasks:              ");
        line++;
        
    }else{
        
        
        mvprintw(line, FOVX, "|Your tasks:             ");
        line++;
    }
    std::string desc;
        
    std::set<char> on_list;
    for(auto t : tasks.list){
        on_list.insert(t.first);
    }
    if(sabbo == 10 && on_list.count('0')) printLine("w", "Scan hand in reactor top(simultaneously)", line, 20-16*!tasks.list['0']);
    if(sabbo == 10 && on_list.count('1')) printLine("w", "Scan hand in reactor bottom(simultaneously)", line, 20-16*!tasks.list['1']);
    if(sabbo == 9 && on_list.count('2')) printLine("[?]", "Restore power in electical", line, 20-16*!tasks.list['2']);
    if(sabbo == 11 && on_list.count('3')) printLine("::", "Initiate oxygen resupply in admin", line, 20-16*!tasks.list['3']);
    if(sabbo == 11 && on_list.count('4')) printLine("::", "Initiate oxygen resupply in O2",line, 20-16*!tasks.list['4']);
    if(on_list.count('U')){//fuel
        if(tasks.list['U']){
            printLine("@","Collect fuel from storage", line, 20-16*!tasks.list['U']);
        }else{
            printLine("<()","Fill top engine", line, 20-16*!tasks.list['B']);
            printLine("<()","Fill bottom engine", line, 20-16*!tasks.list['E']);
        }
    }
    if(on_list.count('x')){//diverting
        if(tasks.list['x']){
            printLine("}{","Divert power from electrical", line, 20-16*!tasks.list['x']);
        }else{
            if(on_list.count('t'))printLine("}{","Accept diverted power in communications", line, 20-16*!tasks.list['t']);
            if(on_list.count('z'))printLine("}{","Accept diverted power in upper engine", line, 20-16*!tasks.list['z']);
            if(on_list.count('C'))printLine("}{","Accept diverted power in lower engine", line, 20-16*!tasks.list['C']);
            if(on_list.count('K'))printLine("}{","Accept diverted power in navigations", line, 20-16*!tasks.list['K']);
            if(on_list.count('N')) printLine("}{","Accept diverted power in O2", line, 20-16*!tasks.list['N']);
            if(on_list.count('R')) printLine("}{","Accept diverted power in security", line, 20-16*!tasks.list['R']);
            if(on_list.count('T')) printLine("}{","Accept diverted power in shields", line, 20-16*!tasks.list['T']);
            if(on_list.count('Z')) printLine("}{","Accept diverted power in weapons", line, 20-16*!tasks.list['Z']);
        }
    }
    if(on_list.count('W')){//empty
        if(tasks.list['r']||tasks.list['M']){
            printLine("T","Empty trash in cafeteria", line, 20-16*!tasks.list['r']);
            printLine("T","Empty trash in O2", line, 20-16*!tasks.list['M']);
        }else{
            printLine("T","Empty chute in storage", line, 20-16*!tasks.list['W']);
        }
    }
    //UPLOADS
    if(on_list.count('@')) printLine("Y", "Upload data from admin", line, 20-16*!tasks.list['@']);
    if(on_list.count('q')) printLine("Y","Upload data from cafeteria", line, 20-16*!tasks.list['q']);
    if(on_list.count('u')) printLine("Y","Upload data from communications", line, 20-16*!tasks.list['u']);
    if(on_list.count('v')) printLine("Y","Upload data from electrical", line, 20-16*!tasks.list['v']);
    if(on_list.count('J')) printLine("Y","Upload data from navigations", line, 20-16*!tasks.list['J']);
    if(on_list.count('Y')) printLine("Y","Upload data from weapons", line, 20-16*!tasks.list['Y']);
    //FIXES
    if(on_list.count('p')) printLine("?","Fix wires in admin", line, 20-16*!tasks.list['p']);
    if(on_list.count('s')) printLine("?","Fix wires in cafeteria", line, 20-16*!tasks.list['s']);
    if(on_list.count('y')) printLine("?","Fix wires in electrical", line, 20-16*!tasks.list['y']);
    if(on_list.count('L')) printLine("?","Fix wires in navigation", line, 20-16*!tasks.list['L']);
    if(on_list.count('V')) printLine("?","Fix wires in storage", line, 20-16*!tasks.list['V']);
    //ALIGN
    if(on_list.count('A')) printLine("[]","Align upper engine output", line, 20-16*!tasks.list['A']);
    if(on_list.count('D')) printLine("[]","Align lower engine output", line, 20-16*!tasks.list['D']);
    
    //SINGLES
    if(on_list.count('o')) printLine(" ","Swipe card in admin", line, 20-16*!tasks.list['o']);
    if(on_list.count('w')) printLine("[!]","Calibrate output in electical", line, 20-16*!tasks.list['w']);
    if(on_list.count('F')) printLine("(( ))","Medical scan in medbay", line, 20-16*!tasks.list['F']);
    if(on_list.count('G')){
        if(sample_countdown>-1)printLine("[+]","Analyze samples in medbay("+std::to_string(sample_countdown/10)+")", line, 20-16*!tasks.list['G']);
        else printLine("[+]","Analyze samples in medbay", line, 20-16*!tasks.list['G']);
    }
    if(on_list.count('H')) printLine("L","Chart course in navigation", line, 20-16*!tasks.list['H']);
    if(on_list.count('I')) printLine("L","Stablize course in navigation", line, 20-16*!tasks.list['I']);
    if(on_list.count('O')) printLine("#","Clean airvent in O2", line, 20-16*!tasks.list['O']);
    if(on_list.count('P')) printLine("::","Start up reactor", line, 20-16*!tasks.list['P']);
    if(on_list.count('Q')) printLine("M","Unlock reactor manifolds", line, 20-16*!tasks.list['Q']);
    if(on_list.count('S')) printLine("-()","Prime shields", line, 20-16*!tasks.list['S']);
    if(on_list.count('X')) printLine("L\"","Clear asteroid with weapons", line, 20-16*!tasks.list['X']);
    attron(COLOR_PAIR(20));
}
void giveOutTasks(taskStruct &l){
   
    std::string shortTasks = "wAPQSHIFGXo";
    std::string uploadTasks = "YJvuq@";
    std::string fixTasks = "VLysp";
    std::string divertTasks = "txzCKNRTZ";
    l.received = false;
    l.done = 0;
    
    l.current = 0;
    l.list.clear();
   // l.list['F']= 1;//debug purposes, remove later!
   // l.list['I']= 1;//debug purposes, remove later!
    while(l.list.size()<3){
        l.list[shortTasks.at(rand()%shortTasks.size())] = 1;
    }
    if(rand()%2){
        l.list['B'] = 1;
        l.list['E'] = 1;
        l.list['U'] = 1;
    }else {
        l.list['r'] = 1;
        l.list['M'] = 1;
        l.list['W'] = 1;
    }
    
    if(rand()%2){
        while(l.list.size()<10){
            l.list[fixTasks.at(rand()%fixTasks.size())] = 1;
        }
    }else{
        while(l.list.size()<10){
            l.list[uploadTasks.at(rand()%uploadTasks.size())] = 1;
        }
    }
    l.list['x'] = 1;
        while(l.list.size()<13){
            l.list[divertTasks.at(rand()%divertTasks.size())] = 1;
        }   
    
    l.to_do = 13;
    return;
}

bool collisionCheck(int id, std::map<int, crewmate> &pos, std::vector<std::string> &gamemap, int sabbo, bool ghost) {
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
        

        if(gamemap[curpos.y].at(curpos.x)== '$'&&sabbo!=1)return false;
        if(gamemap[curpos.y].at(curpos.x)== '&'&&sabbo!=2)return false;
        if(gamemap[curpos.y].at(curpos.x)== '#'&&sabbo!=3)return false;
        if(gamemap[curpos.y].at(curpos.x)== '\"'&&sabbo!=4)return false;
        if(gamemap[curpos.y].at(curpos.x)== '>'&&sabbo!=5)return false;
        if(gamemap[curpos.y].at(curpos.x)== '<'&&sabbo!=6)return false;
        if(gamemap[curpos.y].at(curpos.x)== '^'&&sabbo!=7)return false;
        else return true;
    
    }
    else return false;
}
void startGame(status &game) {
    int impostorCount = (game.position.size()/5)+1;
    while(impostorCount){
        int a = rand()%game.position.size();
        if(game.position[a].status<2){
            game.position[a].status += 2;//make a random player the impostor
            impostorCount--;
        }
    }
    for(auto &player : game.tasks){
        giveOutTasks(player.second);
        
    }
    game.sabbo = 0;
    game.timer.sabbotage = 100;
    game.in_progress = true;
    game.winner = -1;
    game.cameras = 0;
    game.timer.button = BUTTON_TIMEOUT;
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
            cm8.second.x = STARTING_POSITION_X+2*cm8.first;
            cm8.second.y = STARTING_POSITION_Y;
        }
    }
}
void votesResult(status &game) {
    game.timer.button = BUTTON_TIMEOUT;
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
                game.position[cm.first] = {STARTING_POSITION_X+2*cm.first, STARTING_POSITION_Y, 1};
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
                game.position[cm.first] = {STARTING_POSITION_X+2*cm.first, STARTING_POSITION_Y, 1};
                game.timer.kill[cm.first] = INITIAL_KTIMEOUT;
                game.ready[cm.first] = false;
                game.in_progress = false;
            }
        }

    }
}
void applyMovement(const char ch,  const int i, std::map<int, crewmate>&p, std::vector<std::string> &gamemap,int sabbo, const bool ghost, std::deque<std::pair<int, char>> &doorlog) {
    std::string doors = "$&#\"<>^";
    char previous = gamemap[p[i].y].at(p[i].x);
    switch(ch) {
    case '6':
        p[i].x++;
        if(collisionCheck(i, p, gamemap, sabbo, ghost))
            p[i].x--;
        break;
    case '2':
        p[i].y++;
        if(collisionCheck(i, p, gamemap, sabbo, ghost))
            p[i].y--;
        break;
    case '4':
        p[i].x--;
        if(collisionCheck(i, p, gamemap, sabbo, ghost))
            p[i].x++;
        break;
    case '8':
        p[i].y--;
        if(collisionCheck(i, p, gamemap, sabbo, ghost))
            p[i].y++;
        break;
    case '3':
        p[i].x++;
        p[i].y++;
        if(collisionCheck(i, p, gamemap, sabbo, ghost))
        {
            p[i].x--;
            p[i].y--;
        }
        break;
    case '1':

        p[i].y++;
        p[i].x--;
        if(collisionCheck(i, p, gamemap, sabbo, ghost))
        {
            p[i].x++;
            p[i].y--;
        }
        break;
    case '7':
        p[i].x--;
        p[i].y--;
        if(collisionCheck(i, p, gamemap, sabbo, ghost))
        {
            p[i].x++;
            p[i].y++;
        }
        break;
    case '9':
        p[i].y--;
        p[i].x++;
        if(collisionCheck(i, p, gamemap, sabbo, ghost))
        {
            p[i].x--;
            p[i].y++;
        }
        break;

    }
    if(previous==' ' && doors.find(gamemap[p[i].y].at(p[i].x))!=std::string::npos){
        doorlog.push_back({i, gamemap[p[i].y].at(p[i].x)});
        if(doorlog.size()> 10){
            doorlog.pop_front();
        }
    }

}
int tasksCount(status &game){
    int totalDone=0, totalToDo=0;
    for(int i = 0; i  < game.position.size(); i++){
        if(game.position[i].status>>1%2==0){
            totalDone+= game.tasks[i].done;
            totalToDo += 13;
         
        }
    }
    if(totalDone == totalToDo){
        std::cout << "crewmates win"<<std::endl;
        game.winner = 2;
        std::cout <<"Reseting game"<<std::endl;
        for(auto cm : game.position) {
            game.position[cm.first] = {STARTING_POSITION_X+2*cm.first, STARTING_POSITION_Y, 1};
            game.timer.kill[cm.first] = INITIAL_KTIMEOUT;
            game.ready[cm.first] = false;
            game.in_progress = false;
        }
        clear();
    }
    if(totalToDo) return totalDone*100/totalToDo;
    else return 0;
}
void sendReply(int sd, int i, status &game) {
    std::string msg;
    if(!game.tasks[i].current){
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
                if(!game.tasks[i].received){
                    msg ="t ";
                    for(auto t : game.tasks[i].list){
                        msg += std::to_string(t.first)+ " "+std::to_string(t.second)+ " ";
                    }
                    std::cout << msg<<std::endl;
                    game.tasks[i].received = true;
                    goto send_already;
                }else{
                    int tcount = tasksCount(game);
                    if(game.position[i].status%2) {
                        
                        msg ="p "+std::to_string(tcount)+" "+std::to_string(game.timer.kill[i])+" "+std::to_string(game.cameras)+" "+std::to_string(game.sabbo)+" "+std::to_string(game.timer.sabbotage)+" ";

                    } else {
                        msg ="g "+std::to_string(tcount)+" ";
                        msg += std::to_string(game.ghosts[i].x)+" "+std::to_string(game.ghosts[i].y)+" "+std::to_string(game.ghosts[i].status)+" "+std::to_string(game.sabbo)+" "+std::to_string(game.timer.sabbotage)+" ";
                    }

                }
            } else {
                
                int count = 0;
                for(auto el : game.ready) {
                    if(el.second)count++;
                    
                    
                }
                if(count == game.ready.size()){
                    startGame(game);
                    
                }
                msg ="w "+std::to_string(game.winner)+ " ";
                msg += std::to_string(count) + " ";
                msg += std::to_string(game.ready.size())+ " ";
                
            }
            for(auto el : game.position) {
                msg += std::to_string(el.second.x) + " " + std::to_string(el.second.y) + " "+ std::to_string(el.second.status) + " " ;
            }
        }
    }else{
        if(!game.tasks[i].received){
                    msg ="t ";
                    for(auto t : game.tasks[i].list){
                        msg += std::to_string(t.first)+ " "+std::to_string(t.second)+ " ";
                    }
                    std::cout << msg<<std::endl;
                    game.tasks[i].received = true;
        }else{
            
            msg = "q "+ std::to_string(game.tasks[i].current)+" ";
            if(game.tasks[i].current=='8'){//admin logs
                for(auto log : game.doorlog){
                    msg += std::to_string(log.first)+" "+ std::to_string(log.second)+" ";
                }
            }else{
                if(game.tasks[i].current=='2'){//light switches;
                    for(int i = 0; i<5;i++){
                        msg += std::to_string(game.light_switches[i])+" ";
                    }
                }
                
                for(auto el : game.position) {
                    msg += std::to_string(el.second.x) + " " + std::to_string(el.second.y) + " "+ std::to_string(el.second.status) + " " ;
                }
            }
        }
    }
    send_already:    
    send(sd, msg.c_str(), msg.size(), 0 );
}


