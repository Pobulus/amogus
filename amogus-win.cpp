#include "amogus-win.h"
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
void printCenter(std::string text, const int limX, const int limY, const int offX, const int offY) {
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
void drawMap(std::vector<std::string> &gmap, int x, int y, bool camera) {

    for(int i = 0; i < FOVX; i++) {
        for(int k = 0; k < FOVY; k++) {
            if((k+y-(FOVY/2))>0&&(i+x-(FOVX/2))>0&&(k+y-(FOVY/2))<gmap.size()&&(i+x-(FOVX/2))<gmap[k].size()) {
                move(k+TOPOFFSET, i);
                switch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2))) {
                case '%':
                    attron(COLOR_PAIR(14));
                    printw(" ");
                    attron(COLOR_PAIR(20));
                    break;
                case '~':
                    if(camera) attron(COLOR_PAIR(21));
                    else attron(COLOR_PAIR(14));
                    attron(A_BOLD);
                    printw("~");
                    attroff(A_BOLD);
                    attron(COLOR_PAIR(20));
                case 'Y':
                case ':':
                    attron(COLOR_PAIR(4));//green
                    attron(A_BOLD);
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attroff(A_BOLD);
                    attron(COLOR_PAIR(20));
                    break;
                case 'w':
                case '#':
                    attron(COLOR_PAIR(2));//cyan
                    attron(A_BOLD);
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attroff(A_BOLD);
                    attron(COLOR_PAIR(20));
                    break;
                case '?':
                case 'M':
                    attron(COLOR_PAIR(3));//magenta
                    attron(A_BOLD);
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attroff(A_BOLD);
                    attron(COLOR_PAIR(20));
                    break;
                case '@':
                case '(':
                case ')':
                    attron(COLOR_PAIR(1));//red
                    attron(A_BOLD);
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attroff(A_BOLD);
                    attron(COLOR_PAIR(20));
                    break;

                case 'T':
                    attron(COLOR_PAIR(6));//blue
                    attron(A_BOLD);
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attroff(A_BOLD);
                    attron(COLOR_PAIR(20));
                    break;
                case '}':
                case '{':
                case '!':
                    
                    attron(COLOR_PAIR(5));//yellow
                    attron(A_BOLD);
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    attroff(A_BOLD);
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
            case '_':
                attron(COLOR_PAIR(colA));
                addch(character[y].at(x));
                break;
            case 'a':
                attron(COLOR_PAIR(colA));
                addrawch(L'▒');//░
                break;
            case 'A':
                attron(COLOR_PAIR(colA));
                addrawch(L'▓');
                break;
            case 'b':
                attron(COLOR_PAIR(colB));
                addrawch(L'▒');//░
                break;
            case 'B':
                attron(COLOR_PAIR(colB));
                addrawch(L'▓');
                break;
            case '#':
                attron(COLOR_PAIR(20));
                addrawch(L'█');
                break;
            case 'e':
                attron(COLOR_PAIR(2));
                addrawch(L'▓');
                break;
            case 'E':
                attron(COLOR_PAIR(2));
                addrawch(L'█');
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
void drawBox(int x1, int y1, int w, int h, std::string type){
    std::map<std::string, std::vector<std::string>> frames;
    frames["basic"] = {"─", "│", "┌", "┐", "└", "┘"};
    frames["bold"] = {"━", "┃", "┏", "┓", "┗", "┛"};
    frames["rounded"] = {"─", "│", "╭", "╮", "╰", "╯"};
    frames["double"] = {"═", "║", "╔", "╗", "╚", "╝"};
    move(y1, x1);
    for(int i = 0; i < w;i++){
        printw(frames[type][0].c_str());
    }
    move(y1+h, x1);
    for(int i = 0; i < w;i++){
        printw(frames[type][0].c_str());
    }
    for(int i = 0; i < h;i++){
        mvprintw(y1+i, x1, frames[type][1].c_str());
    }
    for(int i = 0; i < h;i++){
        mvprintw(y1+i, x1+w, frames[type][1].c_str());
    }
    mvprintw(y1, x1, frames[type][2].c_str());
    mvprintw(y1, x1+w, frames[type][3].c_str());
    mvprintw(y1+h, x1, frames[type][4].c_str());
    mvprintw(y1+h, x1+w, frames[type][5].c_str());
    
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
void loadKeyBinds ( std::string filename, keyBinds &temp ) {
    std::ifstream fileStream ( filename );
    if ( fileStream ) {
        fileStream >> temp.moveN >> temp.moveE >> temp.moveW >> temp.moveS;
        fileStream >> temp.moveNE >> temp.moveNW >> temp.moveSE >> temp.moveSW;
        fileStream >> temp.middle;
        fileStream >> temp.kill >> temp.use >> temp.report >> temp.ready;
        fileStream >> temp.quit;

        fileStream.close();
    } else {
        std::cerr << "Błąd w otwieraniu pliku "<< filename << std::endl;
    }
                   
}

int distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x1-x2, 2)+pow(y1-y2, 2));
}
void drawTaskbar(int percent){
    mvprintw(0,0,"[");
    int signs = (FOVX*percent)/100;
    attron(COLOR_PAIR(4));
    for(int i = 0; i < signs; i++){
        printw("#");
    }
    attron(COLOR_PAIR(20));
    for(int i = 0; i < FOVX-2-signs; i++){
        printw(" ");
    }
    mvprintw(0,FOVX,"]");
    
}
bool handleTask(taskStruct tasks, std::map<char,std::vector<std::string>> &triggers, const keyBinds kBinds, std::map<int, crewmate>  &positions, std::vector<std::string> &gamemap,std::vector<std::string> &wallmap, playermodel model, int sockfd, int id){
    int  cdown = 18;
    char taskName = tasks.current;
    if(triggers[taskName][2]=="upload"){
        clear();
        int taskload = (FOVX-6)*4;
        int ch;
        while(taskload){
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            }
            
            drawBox(2,2, FOVX-4, FOVY-4, "rounded");
            printCenter("Uploading from "+triggers[taskName][1], FOVX, FOVY/2);
            mvprintw((int)FOVY*0.75, 5, "[");
            attron(COLOR_PAIR(2));
            for(int i = 0; i<((FOVX-10)*4-taskload)/4; i++){
                addch('#');
            }
            attron(COLOR_PAIR(20));
            mvprintw((int)FOVY*0.75, FOVX-5, "]");
            taskload--;
            
        }
        while(cdown){
            getch();
            printCenter("     Task completed!     ", FOVX, FOVY/2);
            cdown--;
        }
        clear();
        return true;
    }else if(triggers[taskName][2]=="divert"){
        int knob = 0;
        std::map<int, int> levels;
        std::vector<char> rooms = {'t', 'z', 'C', 'K', 'N', 'R','T','Z'};
        std::vector<std::string> roomNames= {"Comms", "EN.Up", "EN.Dn", "Navig", " O2", "Secur","Shlds","Wpns"};
        for(int i = 0; i<rooms.size();i++){
            if(tasks.list[rooms[i]]){
                levels[i] = 3;
            }
        }
        bool taskloop = true;
        int ch;
        int d = FOVX/9;
        clear();
        while(taskloop){
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            }else if(ch == kBinds.moveW){
                if(knob>0)knob--;
                
            }else if(ch == kBinds.moveE){
                if(knob<7)knob++;
                
            }else if(ch == kBinds.moveN){
                if(levels[knob]>0)levels[knob]--;
                int cnt =0;
                for(auto x : levels)
                    if(x.second)cnt++;
                if(!cnt){
                    taskloop = 0;
                }
            }else if(ch == kBinds.moveS){
                if(levels[knob]<3)levels[knob]++;
            }
            
            attron(COLOR_PAIR(12));
            for(int i =0; i < 8; i++){
                for(int j =1;  j< FOVY/2+1; j++){
                if(j>levels[i]*2)attron(COLOR_PAIR(12));
                else attron(COLOR_PAIR(20));
                mvprintw(j, 4+d*i, "     ");
                attron(COLOR_PAIR(20));
                mvprintw(FOVY/2+j-1, d*i+4, "  #  ");
                }
                if(i==knob)attron(COLOR_PAIR(1));
                else attron(COLOR_PAIR(20));
                mvprintw(FOVY/2+1, d*i+4, roomNames[i].c_str());
                mvprintw(FOVY/2+levels[i]+3, d*i+4, "[===]");
            }
            attron(COLOR_PAIR(20));
            drawBox(0,0,FOVX, FOVY, "double");
             drawBox(1,(FOVY/2),FOVX-2, (FOVY/2)-1, "double");
            
        }
        while(cdown){
            getch();
            printCenter("     Task completed!     ", FOVX, FOVY/2);
            cdown--;
        }
        clear();
        return true;
    }else if(triggers[taskName][2]=="cams"){
        clear();
        int ch, cam=0;
        char buff[MAX];
        std::vector<int> x, y;
        x = {46, 90, 224, 152};
        y = {38, 16, 37, 42};
        while(true){
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            }
            else if(ch == kBinds.moveW){
                if(cam>0)cam--;
                else cam = 3;
            }
            else if(ch == kBinds.moveE){
                if(cam<3)cam++;
                else cam = 0;
            }
            bzero(buff, sizeof(buff));
            strcpy(buff, "u\n");
            send(sockfd, buff, sizeof(buff), 0);
            bzero(buff, sizeof(buff));
            recv(sockfd, buff, sizeof(buff), 0);
            mvprintw(FOVY+1,0,"From Server : %s   ", buff);
            std::stringstream decoder;
            decoder << buff;
            int a=0, b=0, c=0, cnt=0;
            char mode;
            int other;
            decoder >> mode >> other;
            move(FOVY+2, 0);
            while(decoder >> a >> b >> c) {
                positions[cnt] = {a, b, c};
                cnt++;
            }
            drawMap(gamemap, x[cam], y[cam], true);
            drawCharacters( x[cam], y[cam], positions, wallmap,model);
            drawBox(0,0, FOVX, FOVY, "bold");
            printCenter("Camera: "+std::to_string(cam+1), FOVX, 1);
        }
    }else if(triggers[taskName][2]=="accept"){
        clear();
        int taskload = 1;
        bool flipped = false;
        int ch;
        while(taskload){
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            }
            else if(ch == kBinds.use){
                flipped = true;
            }
            if(!flipped)cutscene(1, 6, "divertOff.txt");
            else{ cutscene(4, 2, "divertOn.txt");taskload=0;}
            printCenter("Divert power to "+triggers[taskName][1], FOVX, FOVY/2);
            
            
        }
        while(cdown){
            getch();
            printCenter("     Task completed!     ", FOVX, FOVY/2);
            cdown--;
        }
        clear();
        return true;
        
    
    }else if(triggers[taskName][2]=="scan"){
        int taskload =  (FOVX-6)*4;;
        std::vector<std::string> colors = {"RED", "CYAN", "MAGENTA", "GREEN", "YELLOW", "BLUE", "WHITE", "DARK RED", "DARK CYAN", "DARK MAGENTA", "DARK GREEN", "DARK YELLOW", "DARK BLUE", "GRAY"};
        char buff[MAX];
        int ch;
        while(taskload){
            ch = getch();
            if(ch==kBinds.quit){
                clear();
                return false;
            }
            bzero(buff, sizeof(buff));
            strcpy(buff, "u\n");
            send(sockfd, buff, sizeof(buff), 0);
            bzero(buff, sizeof(buff));
            recv(sockfd, buff, sizeof(buff), 0);
            //mvprintw(FOVY+1,0,"From Server : %s   ", buff);
            std::stringstream decoder;
            decoder << buff;
            int a=0, b=0, c=0, cnt=0;
            char mode;
            int other;
            decoder >> mode >> other;
            move(FOVY+2, 0);
            while(decoder >> a >> b >> c) {
                positions[cnt] = {a, b, c};
                cnt++;
            }
            drawMap(gamemap, 122, 37);
            
            drawCharacters( 122, 37, positions, wallmap,model);
            if(!positions[id].status){
                attron(COLOR_PAIR(id+1));
                mvprintw((FOVY/2)+TOPOFFSET, (FOVX/2), model.ghost.c_str());
                attron(COLOR_PAIR(20));
            }
            mvprintw((int)FOVY*0.75, 10, "[");
            attron(COLOR_PAIR(3));
            for(int i = 0; i<((FOVX-20)*4-taskload)/4; i++){
                addch('#');
            }
            attron(COLOR_PAIR(20));
            mvprintw((int)FOVY*0.75, FOVX-10, "]");
            printCenter("Scanning body...", FOVX/2, FOVY/3, FOVX/2);
            if(taskload<(FOVX-6)*3) printCenter("COLOR: "+colors[id], FOVX/2, FOVY/3, FOVX/2, 2);
            if(taskload<(FOVX-6)*2) printCenter("WIDTH: 2 chars", FOVX/2, FOVY/3, FOVX/2, 3);
            if(taskload<(FOVX-6)*1) printCenter("WEIGHT:  "+std::to_string(sizeof(positions[id]))+"bytes", FOVX/2, FOVY/3, FOVX/2, 4);
            if(taskload<4) printCenter("DONE.", FOVX/2, FOVY/3, FOVX/2, 5);
                
            taskload--;
        }
        while(cdown){
            getch();
            printCenter("     Task completed!     ", FOVX, FOVY/2);
            cdown--;
        }
        clear();
        return true;
        
    }else if(triggers[taskName][2]=="fix"){
        clear();
        bool taskloop = true;
        int ch, node=0;
        int curX=0, curY=0; 
        std::vector<int> spaces = {1, 2, 3, 4};
        std::vector<int> colors = {1, 6, 5, 3};
        std::map<char, int> tasked;
        int k = 0;
        while(spaces.size()){
            int r = rand()%spaces.size();
            tasked['A'+k] = spaces[r];
            spaces.erase(spaces.begin()+r);
            k++;
        }
         attron(COLOR_PAIR(20));
        drawBox(2,2, FOVX-4, FOVY-4, "double");
        while(taskloop){
            attron(COLOR_PAIR(colors[node]));
            ch = getch();
            if(ch == kBinds.moveN){
                if(curY>0)curY--;
            }else if(ch == kBinds.moveS){
                if(curY<FOVY-6)curY++;
            }else if(ch == kBinds.moveW){
                if(curX>0)curX--;
            }else if(ch == kBinds.moveE){
                if(curX<FOVX-14)curX++;
            } else if(ch == kBinds.moveNE){
                if(curY>0)curY--;
                 mvprintw((FOVY/5)+curY, 7+curX, "#");
                if(curX<FOVX-14)curX++;
            }else if(ch == kBinds.moveNW){
                if(curY>0)curY--;
                 mvprintw((FOVY/5)+curY, 7+curX, "#");
                 if(curX>0)curX--;
            }else if(ch == kBinds.moveSE){
                if(curY<FOVY-6)curY++;
                 mvprintw((FOVY/5)+curY, 7+curX, "#");
                if(curX<FOVX-14)curX++;
            }else if(ch == kBinds.moveSW){
                if(curY<FOVY-6)curY++;
                 mvprintw((FOVY/5)+curY, 7+curX, "#");
                if(curX>0)curX--;
            }
            else if(ch==kBinds.quit){
                clear();
                return false; 
            }
            
            
            mvprintw((FOVY/5)+curY, 7+curX, "#");
           
            attron(COLOR_PAIR(colors[0]));
            mvprintw((FOVY/5), 4, "[A]");
            mvprintw((FOVY/5)*tasked['A'], FOVX-6, "[A]");
            attron(COLOR_PAIR(colors[1]));
            mvprintw((FOVY/5)*2, 4, "[B]");
            mvprintw((FOVY/5)*tasked['B'], FOVX-6, "[B]");
            attron(COLOR_PAIR(colors[2]));
            mvprintw((FOVY/5)*3, 4, "[C]");
            mvprintw((FOVY/5)*tasked['C'], FOVX-6, "[C]");
            attron(COLOR_PAIR(colors[3]));
            mvprintw((FOVY/5)*4, 4, "[D]");
            mvprintw((FOVY/5)*tasked['D'], FOVX-6, "[D]");
            if(curX==FOVX-14){
                if(curY==(FOVY/5)*(tasked['A'+node]-1)){
                    node++;
                    curX = 0;
                    curY = (FOVY/5)*node;
                    if(node == 4){
                        taskloop = false;
                    }
                }
                
            }
            
            
            
            
        }
        while(cdown){
            getch();
            printCenter("     Task completed!     ", FOVX, FOVY/2);
            cdown--;
        }
        clear();
        return true;
    }
    
    
    return true;
    
}

void await(int sockfd, const int id, std::map<int, crewmate>  &positions, crewmate &ghost, std::vector<std::string> &gamemap, std::vector<std::string> &wallmap,playermodel model, std::map<char,std::vector<std::string>> &triggers, bool kb, taskStruct &tasks)
{
    
    bool game_started = false, murdered=false, voting=false;
    int role_countdown = 0, killing_cutscene_countdown=0, murderer=-1, murder_cutscene_countdown=0, ejection_cutscene_countdown=0, winner_countdown=0;
    int loading=1;
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
        kBinds.ready = 'r';
        kBinds.report = 'c';
        kBinds.use = 'z';
        kBinds.quit = 'q';
    }
    for (;;) {


        refresh();
        got_ch = getch();

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
            } else if(got_ch == kBinds.quit) {
                endwin();
                closesocket(sockfd);
                break;
            } else {
                strcpy(buff, "u\n");
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
            
            decoder >> tasksPercent >> killing_timeout >> cams;
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
            tasks.current = (char)a;
            int cnt = 0;
            
            
        } else if (mode == 'g') {
            
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
            decoder >> tasksPercent;
            drawTaskbar(tasksPercent);
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
            
        }
        if (loading){
            printCenter("Loading",FOVX, FOVY);
            loading--;
        }else if(role_countdown) {
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
        }else if(tasks.current){
            bool success = false;
            switch(tasks.current){
                
                case 't'://accept diverted power
                case 'z':
                case 'C':
                case 'K':
                case 'N':
                case 'R':
                case 'T':
                case 'Z':
                    if(!tasks.list['x'])success = handleTask(tasks, triggers, kBinds, positions, gamemap,wallmap, model,  sockfd, id);
                    break;
                case 'B':
                case 'E':
                    if(!tasks.list['U'])
                        success = true;
                    break;
                case 'U'://fuel storage
                    success = true;
                    break;
                case 'M':
                case 'r'://empty 
                    success = handleTask(tasks, triggers, kBinds, positions, gamemap, wallmap, model, sockfd, id);
                    break;
                case 'W':
                    if(!(tasks.list['M'])&&!(tasks.list['r']))
                        success = handleTask(tasks, triggers, kBinds, positions, gamemap, wallmap, model, sockfd, id);
                    break;
                default:
                    success = handleTask(tasks, triggers, kBinds, positions, gamemap, wallmap, model,  sockfd, id);
                    break;
            }
            
            if(success){
                tasks.list[tasks.current] = 0;
                strcpy(buff, "d\n");
            }else{
                strcpy(buff, "f\n");
            }
            send(sockfd, buff, sizeof(buff), 0);
            bzero(buff, sizeof(buff));
            tasks.current = 0;
            
        } else {
            //getch();
            printTasks(tasks, triggers, (positions[id].status>>1%2));
            if(positions[id].status%2) {
                drawMap(gamemap, positions[id].x, positions[id].y, cams);
                char trig = wallmap[positions[id].y][positions[id].x];
                if(trig != ' ') {
                    mvprintw(FOVY+3, 0, "current trigger type: %s    ", triggers[trig][0].c_str());
                    mvprintw(FOVY+4, 0, "location: %s      ", triggers[trig][1].c_str());
                    mvprintw(FOVY+5, 0, "argument: %s      ", triggers[trig][2].c_str());
                }
                int y = positions[id].y;
                int x = positions[id].x;
                drawCharacters( x, y, positions, wallmap,model);
                                                      
                                                      

                                                                                           
                                                   
                                               
                                                                                
                              
                                                                                   
                         
                                               
                     
                 
                    
            } else {
                drawMap(gamemap, ghost.x, ghost.y);
                int y = ghost.y;
                int x = ghost.x;
                attron(COLOR_PAIR(id+1));
                move((FOVY/2)+TOPOFFSET, (FOVX/2));
                addwchstr(u8"👻");
                attron(COLOR_PAIR(20));
                drawCharacters( x, y, positions, wallmap,model);
                                                      
                                                      

            }
            if(numb!="") {
                printCenter("Waiting for players.", FOVX, FOVY, 0, 0);
                printCenter(numb, FOVX, FOVY, 0, 2);
                              
                                                                                   
            }
            drawTaskbar(tasksPercent);
                     
                 


        }

    }
}
void drawCharacters(const int x, const int y, std::map<int, crewmate>  &positions, std::vector<std::string> &wallmap, playermodel model){
    for(auto ch : positions) {
        int posx = ch.second.x-x+(FOVX/2);
        int posy = ch.second.y-y+(FOVY/2);

        if(posx<FOVX&&posy<FOVY&&LOS(x, y, ch.second.x, ch.second.y, wallmap)) {
            attron(COLOR_PAIR(ch.first+1));
            if(ch.second.status %2){
                move(posy+TOPOFFSET, posx);
                addrawch(L'ඞ');
            }
            else {
                move(posy+TOPOFFSET, posx);
                addwchstr(u8"🦴");
            }
            attron(COLOR_PAIR(20));
        }
    }
}

void printTasks(taskStruct &tasks, std::map<char,std::vector<std::string>> &triggers, bool impostor){
    if(impostor){
        mvprintw(0, FOVX+1, "|  Fake tasks:");
        int line = 1;
        std::string desc;
        for(auto t : tasks.list){
            desc  = "| "+triggers[t.first][2]+" in "+triggers[t.first][1];
            
            mvprintw(line, FOVX+1, desc.c_str());
            line++;
        }
    }else{
        mvprintw(0, FOVX+1, "|  Your tasks:");
        int line = 1;
        std::string desc;
        for(auto t : tasks.list){
            desc  = triggers[t.first][2]+" in "+triggers[t.first][1];
            attron(COLOR_PAIR(20));
            mvprintw(line, FOVX+1, "| ");
            if(t.second)attron(COLOR_PAIR(1));
            
            else attron(COLOR_PAIR(4));
            
            printw(desc.c_str());
            line++;
    
            
        }
    }
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
    
    return;
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
                        
                        msg ="p "+std::to_string(tcount)+" "+std::to_string(game.timer.kill[i])+" "+std::to_string(game.cameras)+" ";

                    } else {
                        msg ="g "+std::to_string(tcount)+" ";
                        msg += std::to_string(game.ghosts[i].x)+" "+std::to_string(game.ghosts[i].y)+" "+std::to_string(game.ghosts[i].status)+" ";
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
        msg = "q "+ std::to_string(game.tasks[i].current)+" ";
        for(auto el : game.position) {
            msg += std::to_string(el.second.x) + " " + std::to_string(el.second.y) + " "+ std::to_string(el.second.status) + " " ;
        }
    }
    send_already:    
    send(sd, msg.c_str(), msg.size(), 0 );
}


