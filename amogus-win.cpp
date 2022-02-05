#include "amogus-win.h"

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

bool readParameters ( int& argc, char** argv, std::string& ip, bool &a ) {
    bool errors = false;
    for ( int i = 1; i < argc - 1; i++ ) {
        if ( not strcmp ( argv[i], "-i" ) ) { //adres ip
            ip = argv[i + 1];
        
        } else if ( not strcmp ( argv[i], "-a" ) ) { //verbose, wypisuj dodatkowe informacje
            a = true;
        } else if ( not strcmp ( argv[i], "-h" ) ) { //pomoc
            errors = true;
        }
    }
    if ( not strcmp ( argv[argc - 1], "-a" ) ) a = true; //verbose, wypisuj dodatkowe informacje
    if ( not strcmp ( argv[argc - 1], "-h" ) ) { //pomoc
        errors = true;
    };

    if ( ip == ""|| ( ip.size() ==2&&ip[0]=='-' ) ) {
        std::cerr << "Nie podano adresu ip"<<std::endl;
        errors = true;
    }

    return errors;
}
void printCenter(std::string text, int limX, int limY, int offX, int offY){
    int halfText = text.length()/2;
    mvprintw(limY/2+offY, (limX/2)-halfText+offX, text.c_str());
}
void update_all(std::map<int, std::pair<int, int>>  &positions){
                for(auto ch : positions){
                    attron(COLOR_PAIR(ch.first+1));
                    mvprintw(ch.second.second, ch.second.first, "ඞ");
                }attron(COLOR_PAIR(10));
                
}
void drawMap(std::vector<std::string> &gmap, int x, int y){
    
    for(int i = 0; i < FOVX; i++){
        for(int k = 0; k < FOVY; k++){
            if((k+y-(FOVY/2))>0&&(i+x-(FOVX/2))>0&&(k+y-(FOVY/2))<gmap.size()&&(i+x-(FOVX/2))<gmap[k].size()){
            move(k+TOPOFFSET, i);
            switch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2))){
                case '%':
                    //printw("█");
                    addch((char)219);
                    break;
                case 'Y':
                case ':':
                    attron(COLOR_PAIR(4));//green
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attron(COLOR_PAIR(10));
                    break;
                case 'w':
                case '#':
                    attron(COLOR_PAIR(2));//cyan
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attron(COLOR_PAIR(10));
                    break;
                case '?':
                case 'M':
                    attron(COLOR_PAIR(3));//magenta
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attron(COLOR_PAIR(10));
                    break;
                case '@':
                case '(':
                case ')':
                    attron(COLOR_PAIR(1));//red
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attron(COLOR_PAIR(10));
                    break;
                
                case 'T':
                    attron(COLOR_PAIR(6));//blue
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attron(COLOR_PAIR(10));
                    break;
                case '}':
                case '{':
                case '!':
                    attron(COLOR_PAIR(5));//yellow
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attron(COLOR_PAIR(10));
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
void displayRole(int status){
    if(status>>1%2){//impostor
        attron(COLOR_PAIR(1));
        mvprintw(5,0, impostor.c_str());
    }else{//crewmate
        attron(COLOR_PAIR(2));
        mvprintw(5,0,crewm8.c_str());
    }
    attron(COLOR_PAIR(10));
}
bool LOS(int xa, int ya, int xb, int yb, std::vector<std::string> &wallmap){
    int dx, dy;
    
    dx = xa-xb;
    dy = ya-yb;
    if(distance(xa,  ya, xb, yb) > RADIUS)return false;
    if(abs(dx)>abs(dy)){//dx jest większe
        for(int i = 0; i < (abs(dx)-abs(dy))/2; i++){
            xb+=sgn(dx);
            if(wallmap[yb].at(xb) == '%')return false;
        }
        while(yb != ya){
            xb+=sgn(dx);
            if(wallmap[yb].at(xb) == '%')return false;
            yb+=sgn(dy);
            if(wallmap[yb].at(xb) == '%')return false;
        }
        while(xb != xa){
            xb+=sgn(dx);
            if(wallmap[yb].at(xb) == '%')return false;
        }
    }else {
        for(int i = 0; i < (abs(dy)-abs(dx))/2; i++){
            yb+=sgn(dy);
            if(wallmap[yb].at(xb) == '%')return false;
        }
        while(xb != xa){
            yb+=sgn(dy);
            //if(wallmap[yb].at(xb) == '%')return false;
            xb+=sgn(dx);
            if(wallmap[yb].at(xb) == '%')return false;
        }
        while(yb != ya){
            yb+=sgn(dy);
            if(wallmap[yb].at(xb) == '%')return false;
        }
    }
    return true;
}
void cutscene(int colA, int colB, std::string filename){
    
    std::vector<std::string> character;
    character = loadMap(filename);
    for(int y = 0; y < character.size(); y++){
        for(int x = 0; x < character[0].size(); x++){
            move(y,x);
            switch(character[y].at(x)){
                case 'O':
                case 'U':
                case 'n':
                    attron(COLOR_PAIR(colA));
                    addch(character[y].at(x));
                    break;
                case 'a':
                    attron(COLOR_PAIR(colA));
                    addch('@');
                case 'A':
                    attron(COLOR_PAIR(colA));
                    addch('#');
                    break;
                case 'b':
                    attron(COLOR_PAIR(colB));
                    addch('@');
                    break;
                case 'B':
                    attron(COLOR_PAIR(colB));
                    addch('#');
                    break;
                case '#':
                    attron(COLOR_PAIR(10));
                    addch((char)219);
                    break;
                case 'e':
                    attron(COLOR_PAIR(2));
                    addch('@');
                    break;
                case 'E':
                    attron(COLOR_PAIR(2));
                    addch('#');
                    break;
                    
                default:
                    attron(COLOR_PAIR(10));
                    addch(character[y].at(x));
                    break;
            }
        }
    
    }refresh();
}
std::vector<std::string> loadMap ( std::string mapname ) {
    std::vector<std::string> gamemap;

    std::ifstream mapStream ( mapname );
    if ( mapStream ) {
        std::string line;
        
        while ( std::getline( mapStream, line) ){
            gamemap.push_back(line);

        }
        mapStream.close();
    } else {
        std::cerr << "Błąd w otwieraniu pliku "<<mapname << std::endl;
    }
    return gamemap;
}
std::map<char,std::vector<std::string>> loadLabels ( std::string filename ) {
    std::map<char,std::vector<std::string>> gamemap;
    
    std::ifstream mapStream ( filename );
    if ( mapStream ) {
        std::string type, loc, attr;
        char c;
        while ( mapStream >> c >> type >> loc >> attr){
            gamemap[c] = {type, loc, attr};
        }
        mapStream.close();
    } else {
        std::cerr << "Błąd w otwieraniu pliku "<< filename << std::endl;
    }
    return gamemap;
}
int distance(int x1, int y1, int x2, int y2){
    return sqrt(pow(x1-x2, 2)+pow(y1-y2, 2));
}
void await(int sockfd, const int id, std::map<int, crewmate>  &positions, crewmate &ghost, std::vector<std::string> &gamemap, std::vector<std::string> &wallmap ,playermodel model, std::map<char,std::vector<std::string>> &triggers)
{
    bool game_started = false, murdered=false;
    int role_countdown = 0, killing_cutscene_countdown=0, murderer=-1, murder_cutscene_countdown=0;
    std::map<int, int> previous_status;
    drawMap(gamemap, positions[id].x, positions[id].y);
    refresh();
    char buff[MAX];
    int a = 0;
    for (;;) {
        
        
        refresh();
        a = getch();
        
        bzero(buff, sizeof(buff));
        if(role_countdown){
            strcpy(buff, "u\n");
        }else{
            switch(a){
                case KEY_UP:
                case '8':
                    strcpy(buff, "m8\n");
                    break;
                case KEY_LEFT:
                case '4':
                strcpy(buff, "m4\n");
                    break;
                case KEY_RIGHT:
                case '6':
                    strcpy(buff, "m6\n");
                    break;
                case KEY_DOWN:
                case '2':
                    strcpy(buff, "m2\n");
                    break;
                case '1':
                    strcpy(buff, "m1\n");
                    break;
                case '3':
                strcpy(buff, "m3\n");
                    break;
                case '7':
                    strcpy(buff, "m7\n");
                    break;
                case '9':
                    strcpy(buff, "m9\n");
                    break;
                case ERR:
                    strcpy(buff, "u\n");
                    //mvprintw(0,0,"Timeout ");
                    break;
                case 'x':
                    if(positions[id].status>>1 % 2){
                    strcpy(buff, "k\n");
                    killing_cutscene_countdown = 20;
                    for(auto el : positions){
                        previous_status[el.first] = el.second.status;
                    }
                    }else{
                    strcpy(buff, "u\n");    
                    }
                    break;
                case 'q':
                    endwin();
                    return;
                case 'r':
                    strcpy(buff, "r\n");
                    break;
                default:
                    buff[0] = 'u';
                    buff[1] = '\n';
                    //mvprintw(0, 0, "pressed: %d", a);
                    break;
            }
        }
//         clear();
        send(sockfd, buff, sizeof(buff), 0);
        bzero(buff, sizeof(buff));
        recv(sockfd, buff, sizeof(buff), 0);
        
        mvprintw(FOVY+1,0,"From Server : %s   ", buff);
        std::stringstream decoder;
        decoder << buff;
        int a=0, b=0, c=0, cnt=0;
        std::string numb = "";
        char mode;
        decoder >> mode;
        if (mode == 'p'){
            while(decoder >> a >> b >> c){
                mvprintw(positions[cnt].y, positions[cnt].x, " ");
                positions[cnt] = {a, b, c};
                cnt++;
            }
            if(!game_started){
                clear();
                role_countdown = 50;//How many ticks should you display the message for
                game_started = true;
            }
        }
        else if (mode == 'g'){
            if(!murdered){
                
                murdered=true;
                murder_cutscene_countdown = 20;
            }
            decoder >> ghost.x >> ghost.y >> murderer;
            while(decoder >> a >> b >> c){
                mvprintw(positions[cnt].y, positions[cnt].x, " ");
                positions[cnt] = {a, b, c};
                
                cnt++;
            }
        }else if (mode == 'w'){
            decoder >> a >> b;
            
            numb = std::to_string(a)+"/"+std::to_string(b);
            
            while(decoder >> a >> b >> c){
                
                positions[cnt] = {a, b, c};
                mvprintw(positions[cnt].y, positions[cnt].x, " ");
                cnt++;
            }
        }
        
        if(role_countdown){
            displayRole(positions[id].status);
            role_countdown--;
        }else if(killing_cutscene_countdown){
            killing_cutscene_countdown--;
            for(auto el : previous_status){
                if(el.second != positions[el.first].status){
                    cutscene(id+1, el.first+1, "murder.txt");
                    goto kill_done;//note to self: do something better with this
                }
            }
            killing_cutscene_countdown = 0;
            kill_done:
                ;
             if(killing_cutscene_countdown==0)clear();
        }else if(murder_cutscene_countdown){
            murder_cutscene_countdown--;
            cutscene(murderer+1 ,id+1, "murder.txt");
            if(murder_cutscene_countdown==0)clear();
        }else{
            
            if(positions[id].status%2){
                drawMap(gamemap, positions[id].x, positions[id].y);
                char trig = wallmap[positions[id].y][positions[id].x];
                if(trig != ' '){
                mvprintw(FOVY+3, 0, "current trigger type: %s    ", triggers[trig][0].c_str());
                mvprintw(FOVY+4, 0, "location: %s      ", triggers[trig][1].c_str());
                mvprintw(FOVY+5, 0, "argument: %s      ", triggers[trig][2].c_str());
                }
                int y = positions[id].y;
                int x = positions[id].x;
                for(auto ch : positions){
                    int posx = ch.second.x-x+(FOVX/2);
                    int posy = ch.second.y-y+(FOVY/2);

                    if(posx<FOVX&&posy<FOVY&&LOS(x, y, ch.second.x, ch.second.y, wallmap)){
                    attron(COLOR_PAIR(ch.first+1));
                        if(ch.second.status %2)
                            mvprintw(posy+TOPOFFSET, posx, model.alive.c_str());
                        else{ 
                                mvprintw(posy+TOPOFFSET, posx, model.body.c_str());
                        }
                        attron(COLOR_PAIR(10));
                    }
                }
                    
            }else {
                drawMap(gamemap, ghost.x, ghost.y);
                int y = ghost.y;
                int x = ghost.x;
                attron(COLOR_PAIR(id+1));
                mvprintw((FOVY/2)+TOPOFFSET, (FOVX/2), model.ghost.c_str());
                attron(COLOR_PAIR(10));
                for(auto ch : positions){
                    int posx = ch.second.x-x+(FOVX/2);
                    int posy = ch.second.y-y+(FOVY/2);

                    if(posx<FOVX&&posy<FOVY&&LOS(x, y, ch.second.x, ch.second.y, wallmap)){
                    attron(COLOR_PAIR(ch.first+1));
                        if(ch.second.status %2)
                            mvprintw(posy+TOPOFFSET, posx, model.alive.c_str());
                        else{ 
                                mvprintw(posy+TOPOFFSET, posx, model.body.c_str());
                        }
                        attron(COLOR_PAIR(10));
                    }
                }

            }
            if(numb!=""){
                    printCenter("Waiting for players.", FOVX, FOVY, 0, 0);
                    printCenter(numb, FOVX, FOVY, 0, 2);
            }

                
        }

    }
}
