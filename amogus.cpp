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

void drawCircle(const int x, const int y, const int r, std::string characters){
    int x0 = (x-r);
    int y0 = (y-r);
    
    for(int yi = 0; yi <= 2*r; yi++){
        for(int xi = 0; xi <= 4*r; xi++){
            int d = r - distance(x, y, x0+(xi), y0+yi);
            
            if(d>=0 and y0+yi<FOVY){
                if(d>=characters.size())d=characters.size()-1;
                mvprintw(y0+yi,x0+2*xi, "%c",  characters.at(d));
            }
        }
    }
}
void drawAsteroid(const int x, const int y, const int r, std::string characters){
    int x0 = (x-r);
    int y0 = (y-r);
    
    for(int yi = 0; yi <= 2*r; yi++){
        for(int xi = 0; xi <= 4*r; xi++){
            int d = r - distance(x, y, x0+(xi), (y0+yi)*1.5);
            
            if(d>=0 and y0+yi<FOVY){
                if(d>=characters.size())d=characters.size()-1;
                mvprintw(y0+yi,x0+2*xi, "%c%c",  characters.at(d),characters.at(d));
            }
        }
    }
}
void drawExplosion(const int x, const int y){
    for(int r = 0; r < 8; r++){
        drawCircle(x-r,y,r," @o. ");
        wait(1);
    }
    
}

void drawMap(std::vector<std::string> &gmap, std::vector<std::string> &wallmap, int x, int y, int sabbo, bool camera) {

    for(int i = 0; i < FOVX; i++) {
        for(int k = 0; k < FOVY; k++) {
            if((k+y-(FOVY/2))>0&&(i+x-(FOVX/2))>0&&(k+y-(FOVY/2))<gmap.size()&&(i+x-(FOVX/2))<gmap[k].size()) {
                move(k+TOPOFFSET, i);
                switch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2))) {
                case '%':
                    if(sabbo == 9){
                        printw("░");
                    }else{
                    attron(COLOR_PAIR(14));
                    printw(" ");
                    attron(COLOR_PAIR(20));
                    }
                    break;
                case '$':
                    
                    if(wallmap[k+y-(FOVY/2)].at(i+x-(FOVX/2))=='$')
                    {
                       if(sabbo == 1) 
                            printw("▓");
                       else
                            printw(" ");
                    }
                    else if(wallmap[k+y-(FOVY/2)].at(i+x-(FOVX/2))=='&')
                   {
                       if(sabbo == 2) 
                            printw("▓");
                       else
                            printw(" ");
                    }
                    
                    else if(wallmap[k+y-(FOVY/2)].at(i+x-(FOVX/2))=='#')
                   {
                       if(sabbo == 3) 
                            printw("▓");
                       else
                            printw(" ");
                    }
                    else if(wallmap[k+y-(FOVY/2)].at(i+x-(FOVX/2))=='\"')
                    {
                       if(sabbo == 4) 
                            printw("▓");
                       else
                            printw(" ");
                    }
                    else if(wallmap[k+y-(FOVY/2)].at(i+x-(FOVX/2))=='>')
                    {
                       if(sabbo == 5) 
                            printw("▓");
                       else
                            printw(" ");
                    }
                    else if(wallmap[k+y-(FOVY/2)].at(i+x-(FOVX/2))=='<')
                    {
                       if(sabbo == 6) 
                            printw("▓");
                       else
                            printw(" ");
                    }
                    else if(wallmap[k+y-(FOVY/2)].at(i+x-(FOVX/2))=='^')
                    {
                       if(sabbo == 7) 
                            printw("▓");
                       else
                            printw(" ");
                    }
                    break; 
                    
                case '~':
                    if(sabbo == 9){
                        printw("░");
                    }else{
                        if(camera) attron(COLOR_PAIR(21));
                        else attron(COLOR_PAIR(14));
                        attron(A_BOLD);
                        printw("~");
                        attroff(A_BOLD);
                        attron(COLOR_PAIR(20));
                    }
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
void drawSquare(int x1, int y1, int w, int h, std::string character){
    
    
    for(int j = 0; j < h;j++){
        move(y1+j, x1);
        for(int i = 0; i < w;i++){
            printw(character.c_str());
        }
    }
    
    
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
void drawHexagon(int x, int y){
    mvprintw(y, x+2,   "_____");
    mvprintw(y+1, x+1,"╱");
    mvprintw(y+2, x, "╱");
    mvprintw(y+1, x+7,        "╲");
    mvprintw(y+2, x+8,         "╲");
    mvprintw(y+3, x+8,         "╱");
    mvprintw(y+3, x,  "╲");
    mvprintw(y+4, x+1, "╲_____╱");
    
    
}
void drawTesttube(int x, int y){
    for(int i = 0; i<6;i++){
        mvprintw(y+i, x, "│");
        mvprintw(y+i, x+6, "│");
    }
//     mvprintw(y+6, x, "╲");
    mvprintw(y+6, x, "╰─────╯");
//     mvprintw(y+6, x+5, "╱");
    
}
void drawPipet(int x, int y){
    for(int i = 0; i<4;i++){
        mvprintw(y+i, x, "│");
        mvprintw(y+i, x+2, "│");
    }
//     mvprintw(y+6, x, "╲");
    mvprintw(y+4, x, "\\ /");
    mvprintw(y+5, x, " v");
//     mvprintw(y+6, x+5, "╱");
    
}
void drawHand(int x, int y){
    int line = 0;
    mvprintw(y+line, x, "             ____           ");line++; 
    mvprintw(y+line, x, "        ___ /    \\ ___      ");line++; 
    mvprintw(y+line, x, "       /   \\|    |/   \\ ___ ");line++; 
    mvprintw(y+line, x, "       |    |    |    |/   \\");line++; 
    mvprintw(y+line, x, " ___   |    |    |    |    |");line++; 
    mvprintw(y+line, x, "/   \\  |    |    |    |    |");line++; 
    mvprintw(y+line, x, "|    \\ |    |    |    |    |");line++; 
    mvprintw(y+line, x, "|     \\|    |         |    |");line++; 
    mvprintw(y+line, x, "\\                          |");line++; 
    mvprintw(y+line, x, " \\                         |");line++; 
    mvprintw(y+line, x, "  \\                        |");line++; 
    mvprintw(y+line, x, "   \\                       /");line++; 
    mvprintw(y+line, x, "    \\                     / ");line++; 
    mvprintw(y+line, x, "     \\___________________/  ");line++; 
}

bool handleVent(taskStruct tasks, std::map<char,std::vector<std::string>> &triggers, const keyBinds kBinds, std::map<int, crewmate>  &positions, std::vector<std::string> &gamemap,std::vector<std::string> &wallmap, playermodel model, int sockfd, int id, int &sabbo){
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
        {'n', {212, 73}},
    };
    int ch;
    bool ventloop = true;
    char buff[MAX];
    bool result = false;
    clear();    
    while(ventloop){
        ch = getch();
        if(ch == kBinds.quit){
            ventloop = false;
            result = false;
        } else if(ch == kBinds.use){
            ventloop = false;
            result = true;
        }
        bzero(buff, sizeof(buff));
        strcpy(buff, "u\n");
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
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
        drawMap(gamemap, wallmap, vent_positions[tasks.current].first, vent_positions[tasks.current].second, sabbo);
        
       drawCharacters( vent_positions[tasks.current].first, vent_positions[tasks.current].second, positions, wallmap,model, sabbo);
        if(!positions[id].status){
            attron(COLOR_PAIR(id+1));
            mvprintw((FOVY/2)+TOPOFFSET, (FOVX/2), model.ghost.c_str());
            attron(COLOR_PAIR(20));
        }
    }

    return result;
}
void drawSine(double f, double A, int t, int x){
    for(int i = 0; i < x;i++){
        mvprintw(6+(int)A*sin((2*PI/f)*i+t), i+1, "*");
    }
}
void drawRing(const int x, const int y, const int angle, const int color){
   attron(COLOR_PAIR(color));
    mvaddch(y, x+4, '*');
    mvaddch(y, x+5, '*');
    mvaddch(y, x+6, '*');
    mvaddch(y, x+7, '*');
    mvaddch(y, x+8, '*');
    mvaddch(y, x+9, '*');
    mvaddch(y, x+10, '*');
    mvaddch(y+1, x+11, '*');
    mvaddch(y+1, x+12, '*');
    mvaddch(y+2, x+13, '*');
    mvaddch(y+3, x+14, '*');
    mvaddch(y+4, x+14, '*');
    mvaddch(y+5, x+14, '*');
    mvaddch(y+6, x+13, '*');
    mvaddch(y+7, x+12, '*');
    mvaddch(y+7, x+11, '*');
    mvaddch(y+8, x+10, '*');
    mvaddch(y+8, x+9, '*');
    mvaddch(y+8, x+8, '*');
    mvaddch(y+8, x+7, '*');
    mvaddch(y+8, x+6, '*');
    mvaddch(y+8, x+5, '*');
    mvaddch(y+8, x+4, '*');
    mvaddch(y+7, x+3, '*');
    mvaddch(y+7, x+2, '*');
    mvaddch(y+6, x+1, '*');
    mvaddch(y+5, x, '*');
    mvaddch(y+4, x, '*');
    mvaddch(y+3, x, '*');
    mvaddch(y+2, x+1, '*');
    mvaddch(y+1, x+2, '*');
    mvaddch(y+1, x+3, '*');
    attron(COLOR_PAIR(1));
   switch(angle%32){
        case 0:
            mvaddch(y, x+4, '*');
        break;
        case 1:
            mvaddch(y, x+5, '*');
        break;
        case 2:
            mvaddch(y, x+6, '*');
        break;
        case 3:
        mvaddch(y, x+7, '*');
        break;
        case 4:
        mvaddch(y, x+8, '*');
        break;
        case 5:
        mvaddch(y, x+9, '*');
        break;
        case 6:
        mvaddch(y, x+10, '*');
        break;
        case 7:        
        mvaddch(y+1, x+11, '*');
        break;
        case 8:        
        mvaddch(y+1, x+12, '*');
        break;
        case 9:        
        mvaddch(y+2, x+13, '*');
        break;
        case 10:        
        mvaddch(y+3, x+14, '*');
        break;
        case 11:        
        mvaddch(y+4, x+14, '*');
        break;
        case 12:        
        mvaddch(y+5, x+14, '*');
        break;
        case 13:        
        mvaddch(y+6, x+13, '*');
        break;
        case 14:        
        mvaddch(y+7, x+12, '*');
        break;
        case 15:        
        mvaddch(y+7, x+11, '*');
        break;
        case 16:        
        mvaddch(y+8, x+10, '*');
        break;
        case 17:        
        mvaddch(y+8, x+9, '*');
        break;
        case 18:        
        mvaddch(y+8, x+8, '*');
        break;
        case 19:        
        mvaddch(y+8, x+7, '*');
        break;
        case 20:        
        mvaddch(y+8, x+6, '*');
        break;
        case 21:        
        mvaddch(y+8, x+5, '*');
        break;
        case 22:        
        mvaddch(y+8, x+4, '*');
        break;
        case 23:        
        mvaddch(y+7, x+3, '*');
        break;
        case 24:        
        mvaddch(y+7, x+2, '*');
        break;
        case 25:        
        mvaddch(y+6, x+1, '*');
        break;
        case 26:        
        mvaddch(y+5, x, '*');
        break;
        case 27:        
        mvaddch(y+4, x, '*');
        break;
        case 28:        
        mvaddch(y+3, x, '*');
        break;
        case 29:        
        mvaddch(y+2, x+1, '*');
        break;
        case 30:        
        mvaddch(y+1, x+2, '*');
        break;
        case 31:        
        mvaddch(y+1, x+3, '*');
        break;
   }
}
int randInt( int low, int high){
    return low+rand()%(high-low);
}
std::map<int, int> checkLevels(const taskStruct tasks){
    std::vector<char> rooms = {'t', 'z', 'C', 'K', 'N', 'R','T','Z'};
    std::map<int, int> levels;
    std::set<char> on_list;
    for(auto t : tasks.list){
        on_list.insert(t.first);
    }
    for(int i = 0; i<rooms.size();i++){
        if(on_list.count(rooms[i])){
            levels[i] = 3;
        }
    }
    return levels;
}
bool handleTask(taskStruct &tasks, std::map<char,std::vector<std::string>> &triggers, const keyBinds kBinds, std::map<int, crewmate>  &positions, std::vector<std::string> &gamemap,std::vector<std::string> &wallmap, playermodel model, int sockfd, int id, int &sample_countdown, int &sabbo){
    int  cdown = 18;
    char taskName = tasks.current;
    if(triggers[taskName][2]=="oxygen"){
         clear();
        bool taskloop = true;
        int ch;
        char buff[MAX];
        bzero(buff, sizeof(buff));
        int xOffset = 31;
        std::vector<int> to_choose = {0,1,2,3,4,5,6,7,8};
        std::vector<int> combination;
        srand(time(NULL));
        for(int i=0;i<4;i++){
            int r = rand()%to_choose.size();
            combination.push_back(to_choose[r]);
            to_choose.erase(to_choose.begin()+r);
        }
        drawBox(xOffset, 0, 30, 16, "basic");
        
        for(int y =0; y<3;y++){
            for(int x = 0; x<3;x++){
                attron(COLOR_PAIR(6));
                drawBox(xOffset+1+10*x, 1+5*y, 8,4,"basic");
                attron(COLOR_PAIR(20));
                mvprintw(3+5*y, xOffset+5+10*x, "%d",y*3+x+1);
            }
        }
        int chosen = -1;
        
        int current = 0;
        
        bool pressed = false;
        attron(COLOR_PAIR(1));
        drawBox(2,2,13, 2, "double");
        mvprintw(3, 3, "[ ][ ][ ][ ]");
        attron(COLOR_PAIR(12));
        
        drawSquare(4, 10, 11, 4, " ");
        mvprintw(11, 5, "The code:");
        for(int i = 0; i < combination.size(); i++){
            mvprintw(13, 6+i*2, "%d", combination[i]+1);
        }
        attron(COLOR_PAIR(2));
        while(taskloop){
            ch = getch();
            if(ch==kBinds.quit){
                clear();
                return false;
            }else if(ch == kBinds.moveNW){
                chosen = 0;
            }else if(ch == kBinds.moveN){
                chosen = 1;
            }else if(ch == kBinds.moveNE){
                chosen = 2;
            }else if(ch == kBinds.moveW){
                chosen = 3;
            }else if(ch == kBinds.middle){
                chosen = 4;
            }else if(ch == kBinds.moveE){
                chosen = 5;
            }else if(ch == kBinds.moveSW){
                chosen = 6;
            }else if(ch == kBinds.moveS){
                chosen = 7;
            }else if(ch == kBinds.moveSE){
                chosen = 8;
            }else if(ch == ERR){
                chosen = -1;
               
            }
            if(chosen>-1){
                pressed = true;
                attron(COLOR_PAIR(2));
                drawBox(xOffset+1+10*(chosen%3), 1+5*((int)chosen/3), 8,4,"bold");
                if(combination[current] == chosen){
                    attron(COLOR_PAIR(1));
                    mvprintw(3,4+current*3, "%d", chosen+1);
                    current++;
                    if(current == combination.size()){
                        taskloop = false;
                    }
                }else{
                    if(pressed){
                        current = 0;
                        attron(COLOR_PAIR(1));
                        mvprintw(3, 3, "[ ][ ][ ][ ]");
                        }
                }
            
            }else{
                 pressed = false;
                for(int y =0; y<3;y++){
                    for(int x = 0; x<3;x++){
                        attron(COLOR_PAIR(6));
                        drawBox(xOffset+1+10*x, 1+5*y, 8,4,"basic");
                    }
                }
            }
            bzero(buff, sizeof(buff));  
            strcpy(buff, "u\n");
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            
            std::stringstream decoder;
            decoder << buff;
            char mode;
            int a, b;
            decoder >> mode;
            if(mode == 't'){
                tasks.list.clear();
                while(decoder >> a >> b) {
                    tasks.list[(char)a] = b;
                }
                tasks.current = 0;
                tasks.list.erase('3');
                taskloop = false;
            }else{
                decoder.clear();
            }
        }
        
        
        attron(COLOR_PAIR(4));
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
    }else if(triggers[taskName][2]=="lights"){
         clear();
        bool taskloop = true;
        int ch;
        bool lights_s[5];
        char buff[MAX];
        bzero(buff, sizeof(buff));
        int space = FOVX/6;
        strcpy(buff, "u\n");
        write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            mvprintw(FOVY+1, 0, buff);
            std::stringstream decoder;
            decoder << buff;
            char mode;
            int a;
            
            decoder >> mode;
            if(mode == 'q'){
                for(int i = 0; i< 5;i++){
                    decoder >> a;
                    lights_s[i] = (bool)a;
                }
            }
        while(taskloop){
            ch = getch();
            bzero(buff, sizeof(buff));
            if(ch == kBinds.quit){
                clear();
                return false;
            }
            else if(ch == '1'){
                strcpy(buff, "l1\n");
            }
            else if(ch == '2'){
                strcpy(buff, "l2\n");
            }
            else if(ch == '3'){
                strcpy(buff, "l3\n");
            }
            else if(ch == '4'){
                strcpy(buff, "l4\n");
            }
            else if(ch == '5'){
                strcpy(buff, "l5\n");
            }else{
                strcpy(buff, "u\n");
            }
            
           
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            
            std::stringstream decoder;
            decoder << buff;
            char mode;
            int a, b;
            decoder >> mode;
            if(mode == 'q'){
                drawBox(0, 0, FOVX, FOVY/3, "bold");
                drawBox(0, FOVY/3+1, FOVX, 2*FOVY/3, "double");
                decoder >> b;
                for(int i = 0; i< 5;i++){
                    decoder >> a;
                    lights_s[i] = (bool)a;
                    attron(COLOR_PAIR(20));
                    mvprintw(FOVY/3+3, 1+space+(space*i), "  #  ");
                    mvprintw(FOVY/3+4, 1+space+(space*i), "  #  ");
                    mvprintw(FOVY/3+5, 1+space+(space*i), "  #  ");
                    mvprintw(FOVY/3+6, 1+space+(space*i), "  %d  ", i+1);
                    mvprintw(FOVY/3+7, 1+space+(space*i), "  #  ");
                    mvprintw(FOVY/3+8, 1+space+(space*i), "  #  ");
                    mvprintw(FOVY/3+9, 1+space+(space*i), "  #  ");
                    attron(COLOR_PAIR(4));
                    if(a)mvprintw(FOVY/3+3, 1+space+(space*i), "[===]");
                    else mvprintw(FOVY/3+9, 1+space+(space*i), "[===]");
                    
                }
            }
            if(mode == 't'){
                tasks.list.clear();
                while(decoder >> a >> b) {
                    tasks.list[(char)a] = b;
                }
                tasks.current = 0;
                tasks.list.erase('2');
                for(int i = 0; i< 5;i++){
                    attron(COLOR_PAIR(20));
                    mvprintw(FOVY/3+9, 1+space+(space*i), "  #  ");
                    attron(COLOR_PAIR(4));
                    mvprintw(FOVY/3+3, 1+space+(space*i), "[===]");
                }
                taskloop = false;
            } 
        }
        attron(COLOR_PAIR(4));
        printCenter("Power restored!", FOVX, FOVY/3);
        wait(20);
        clear();
    }else if(triggers[taskName][2]=="jam"){
         clear();
        bool taskloop = true;
        int ch;
        double f0=15+(randInt(-5, 5)/2);
        double f=32;
        int A0=4+randInt(-2, 2);
        int t = 0, A=2;
        while(taskloop){
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            }
            else if(ch == kBinds.moveW){
                if(f<40)f++;
            }
            else if(ch == kBinds.moveE){
                if(f>1)f--;
            }
            else if(ch == kBinds.moveN){
                if(A<5)A++;
            }
            else if(ch == kBinds.moveS){
                if(A>0)A--;
            }
//             else if(ch == kBinds.moveNE){
//                 if(d<5)d++;
//             }
//             else if(ch == kBinds.moveNW){
//                 if(d>-5)d--;
//             }
            clear();
            
            attron(COLOR_PAIR(2));
            
            drawSine(f0,A0,t,FOVX-2);
            attron(COLOR_PAIR(1));
            if(f==f0&&A==A0){
                attron(COLOR_PAIR(4));
                taskloop = false;
            }
            drawSine(f,A,t,FOVX-2);
//             mvprintw(FOVY+1,0,"%f, %f, %f, %f, %d", f, f0, A, A0, d);
            attron(COLOR_PAIR(20));
            drawBox(0,0,FOVX, 11, "bold");
            drawBox(0,12,FOVX, FOVY-12, "double");
            printCenter("Amplitude: "+std::to_string(A)+" ", FOVX,  FOVY-12, 0, 11);
            printCenter("Frequency: "+std::to_string(1000/f)+"kHz  " , FOVX,  FOVY-12, 0, 13);
            t++;
        }
        
        int cdown = 20;
        while(cdown){
            clear();
            attron(COLOR_PAIR(4));
            drawSine(f0,A0,t,FOVX-2);
            printCenter("Communications restored!", FOVX, FOVY/2, 0, -2);
            attron(COLOR_PAIR(20));
            drawBox(0,0,FOVX, 11, "bold");
            drawBox(0,12,FOVX, FOVY-12, "double");
            printCenter("Amplitude: "+std::to_string(A)+" ", FOVX,  FOVY-12, 0, 11);
            printCenter("Frequency: "+std::to_string(1000/f)+"kHz  " , FOVX,  FOVY-12, 0, 13);
            getch();
            cdown--;
            t++;
        }
        clear();
        return true;
    }else if(triggers[taskName][2]=="hand"){
        clear();
        bool taskloop = true;
        int ch;
        int y=0;
        bool reverse = false;
        char buff[MAX];
        while(taskloop){
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            }
            if(reverse){
                if(y>1)y--;
                if(y==1)reverse=false;
            }else{
                if(y<FOVY-1)y++;
                if(y==FOVY-1)reverse=true;
            }
            attron(COLOR_PAIR(id+1));
            drawHand(2,1);
            attron(COLOR_PAIR(2));
            drawSquare(0, y, FOVY*2, 1, "#");
            drawSquare(0, y-1, FOVY*2, 1, " ");
            drawSquare(0, y+1, FOVY*2, 1, " ");
            attron(COLOR_PAIR(20));
            drawBox(0,0,FOVY*2,FOVY,"bold");
            bzero(buff, sizeof(buff));
            strcpy(buff, "u\n");
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            mvprintw(FOVY+1, 0, buff);
            std::stringstream decoder;
            decoder << buff;
            char mode;
            int a, b;
            decoder >> mode;
//             if(mode == 'w'){
//                 tasks.current = 0;
//                 tasks.list.erase('1');
//                 tasks.list.erase('0');
//                 taskloop = false;
//             }
            if(mode == 't'){
                tasks.list.clear();
                while(decoder >> a >> b) {
                    tasks.list[(char)a] = b;
                }
                tasks.current = 0;
                tasks.list.erase('1');
                tasks.list.erase('0');
                taskloop = false;
            }   
        }
        clear();
        return false; 
    }else if (triggers[taskName][2]=="upload"){
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
        attron(COLOR_PAIR(4));
        printCenter("          Task completed!          ", FOVX, FOVY/2);
        wait(20);
        clear();
//         return true;
    }else if(triggers[taskName][2]=="clear"){
        clear();
        bool taskloop = true, shoot=false;
        int curX = FOVX/2;
        int curY = FOVY/2;
        int ch;
        crewmate a,b,c;
        a = {randInt(2, FOVX/4), randInt(2, FOVY/2), randInt(4, 6)};
        b = {randInt(1+FOVX/4, 2*(FOVX/4)), randInt(FOVY/2, FOVY-2), randInt(4, 6)};
        c = {randInt(1+2*(FOVX/4), 3*FOVX/4), randInt(2, FOVY/2), randInt(4, 6)};
        int aimed;
        
        while(taskloop){
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            }else if(ch == kBinds.moveN){
                if(curY>1)curY--;
            }else if(ch == kBinds.moveS){
                if(curY<FOVY)curY++;
            }else if(ch == kBinds.moveW){
                if(curX>1)curX--;
            }else if(ch == kBinds.moveE){
                if(curX<FOVX-1)curX++;
            } else if(ch == kBinds.moveNE){
                if(curY>1)curY--;
                if(curX<FOVX-1)curX++;
            }else if(ch == kBinds.moveNW){
                if(curY>1)curY--;
                 if(curX>1)curX--;
            }else if(ch == kBinds.moveSE){
                if(curY<FOVY-1)curY++;
                if(curX<FOVX-1)curX++;
            }else if(ch == kBinds.moveSW){
            
                if(curY<FOVY-1)curY++;
                if(curX>1)curX--;
            }else if(ch == kBinds.use){
                shoot = true;
            }
            if(ch!=ERR)clear();
            drawAsteroid(a.x, a.y, a.status, "@");
            drawAsteroid(b.x, b.y, b.status, "*");
            drawAsteroid(c.x, c.y, c.status, "#");
            for(int x = 0; x<FOVX; x++){
                mvaddch(curY, x, '-');
            }
            for(int y = 0; y<FOVY; y++){
                mvaddch(y, curX, '|');
            }
            mvprintw(curY, curX, "+");
            mvprintw(FOVY+1, 0, "%d", aimed);
            //mvprintw(FOVY+2, 0, "%d %d %d %d", mvinch(curY-1,curX-1)&A_CHARTEXT,mvinch(curY+1,curX-1)&A_CHARTEXT,mvinch(curY-1,curX+1)&A_CHARTEXT,mvinch(curY+1,curX+1)&A_CHARTEXT);
            drawBox(0,0,FOVX, FOVY, "bold");
            if(shoot){
                shoot = false;
                /*
                 * Basically what we do here is checking corner cells around the crosshair, if all of them are the same character, 
                 * we remove the asteroid made with this character
                 */
                aimed = (mvinch(curY-1,curX-1)&A_CHARTEXT)+(mvinch(curY+1,curX-1)&A_CHARTEXT)+(mvinch(curY-1,curX+1)&A_CHARTEXT)+(mvinch(curY+1,curX+1)&A_CHARTEXT);
                
                if(aimed/4=='@'){
                    a.status = 0;
                    attron(COLOR_PAIR(5));
                    drawExplosion(curX, curY);
                    attron(COLOR_PAIR(20));
                    clear();
                }else if(aimed/4=='*'){
                    b.status = 0;
                    attron(COLOR_PAIR(5));
                    drawExplosion(curX, curY);
                    attron(COLOR_PAIR(20));
                    clear();
                }else if(aimed/4=='#'){
                    c.status = 0;
                    attron(COLOR_PAIR(5));
                    drawExplosion(curX, curY);
                    attron(COLOR_PAIR(20));
                    clear();
                }
               if(a.status+b.status+c.status == 0){
                   taskloop = false;
                } 
            }
        }
        attron(COLOR_PAIR(20));
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
    }else if(triggers[taskName][2]=="clean"){
        bool taskloop = true;
        clear();
        int ch;
        int y=FOVY/2;
        std::vector<std::pair<int, int>> gusts;
        std::map<int, int> leaves;
        for(int i = 1; i < FOVY; i++){
            leaves[i] = randInt(0, FOVX-2);
        }
        while(taskloop){
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            } else if(ch == kBinds.moveN){
                if(y>1)y--;
            } else if(ch == kBinds.moveS){
                if(y<FOVY-1)y++;
            } else if(ch == kBinds.use){
                gusts.push_back({FOVX-2, y});
            }
            
            for(int i = 0;i< gusts.size(); i++){
                
                gusts[i].first--;
                if(leaves[gusts[i].second]>0){
                    if(leaves[gusts[i].second] == gusts[i].first){
                        leaves[gusts[i].second]--;
                    }
                }
                attron(COLOR_PAIR(2));
                mvprintw(gusts[i].second, gusts[i].first, "( " );
                if(gusts[i].first<0){
                    gusts.erase(gusts.begin()+i);
                }
                
            }
            int over = 0;
            for(auto leaf: leaves){
                attron(COLOR_PAIR(4));
                if(leaf.second>0){
                    over++;
                    mvprintw(leaf.first, leaf.second, "$");
                }
            }
            
            attron(COLOR_PAIR(5));
            
            mvprintw(y, FOVX-1, "{");
            mvprintw(y+1, FOVX-1, " ");
            mvprintw(y-1, FOVX-1, " ");
            attron(COLOR_PAIR(20));
            drawBox(0,0, FOVX, FOVY,"double");
            for(int i = 1; i<FOVY; i++){
                mvaddch(i, 0, '#');
            }
                
            if(!over){
                taskloop = false;
            }
        }
        attron(COLOR_PAIR(20));
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
    }else if(triggers[taskName][2]=="calibrate"){
        int ch; 
        int delay = 3;
        clear();
        int stage = 0;
        int a=0, b=0, c=0;
        bool taskloop = true;
        int chosen = 0;
        bool pressed = false;
        while(taskloop){
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            } else if(ch == kBinds.use){
                chosen = 1;
                
            }else{
                chosen = 0;
                pressed = false;
            }
            drawBox(0,0,FOVX, 11, "double");
            drawBox(0,12,FOVX, 6, "basic");
            
            drawRing(2, 1, a, 5);
            drawRing(22, 1, b, 6);
            drawRing(42, 1, c, 2);
            mvaddch(10, 9, '^');
            mvaddch(10, 29, '^');
            mvaddch(10, 49, '^');
            wait(delay);
            drawBox(2, 13, 14, 3, "basic");
            drawBox(22, 13, 14, 3, "basic");
            drawBox(42, 13, 14, 3, "basic");
            switch(stage){
                case 0:
                    a++;
                    attron(COLOR_PAIR(11));
                    drawSquare(3, 17, 13, 1, " ");
                    if(chosen&&!pressed){
                        pressed = true;
                        if(a%32>17&&a%32<21){
                            stage=1;
                            attron(COLOR_PAIR(12));
                            drawSquare(3, 14, 13, 2, " ");
                            attron(COLOR_PAIR(20));
                            drawSquare(3, 17, 13, 1, " ");
                        }
                    }
                break;
                case 1:
                    b++;
                    attron(COLOR_PAIR(11));
                    drawSquare(23, 17, 13, 1, " ");
                    if(chosen&&!pressed){
                        pressed = true;
                        if(b%32>17&&b%32<21){
                            stage=2;
                            attron(COLOR_PAIR(13));
                            drawSquare(23, 14, 13, 2, " ");
                            attron(COLOR_PAIR(20));
                            drawSquare(23, 17, 13, 1, " ");
                        }
                    }
                break;
                case 2:
                    c++;
                    attron(COLOR_PAIR(11));
                    drawSquare(43, 17, 13, 1, " ");
                    if(chosen&&!pressed){
                        pressed = true;
                        
                        if(c%32>17&&c%32<21){
                            attron(COLOR_PAIR(9));
                            drawSquare(43, 14, 13, 2, " ");
                            taskloop = false;
                        }
                    }
                break;
            }
            attron(COLOR_PAIR(20));
            
        }
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
    }else if(triggers[taskName][2]=="prime"){
        int ch;
        bool taskloop = true;
        clear();
        std::vector<int> to_choose = {1,2,3,5,7,8,9};
        std::map<int, crewmate> hexagons;
        hexagons[8] = {29, 1, 0};
        hexagons[2] = {29, 9, 0};
        hexagons[5] = {29, 5, 0};
        hexagons[7] = {22, 3, 0};
        hexagons[9] = {36, 3, 0};
        hexagons[1] = {22, 7, 0};
        hexagons[3] = {36, 7, 0};
        
//         drawHexagon(29, 1);
        for(auto &h : hexagons){
            drawHexagon(h.second.x, h.second.y);
            mvprintw(h.second.y+2, h.second.x+4, "%d", h.first);
        }
        int cnt = 0;
        attron(COLOR_PAIR(1));
        for(int i = 0; i<3;i++){
            int r = rand()%to_choose.size();
            int R = to_choose[r];
            hexagons[R].status = 1;
            drawHexagon(hexagons[R].x, hexagons[R].y);
            to_choose.erase(to_choose.begin()+r);
            cnt++;
        }
        attron(COLOR_PAIR(20));
        int c = 0;
        while(taskloop){
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            }else if(ch == kBinds.moveNW){
                c = 7;
            }else if(ch == kBinds.moveN){
                c = 8;
            }else if(ch == kBinds.moveNE){
                c = 9;
            }else if(ch == kBinds.middle){
                c = 5;
            }else if(ch == kBinds.moveSW){
                c = 1;
            }else if(ch == kBinds.moveS){
                c = 2;
            }else if(ch == kBinds.moveSE){
                c = 3;
                
            }else{
                c = ch-'0';
            }
            if(hexagons[c].status){
                hexagons[c].status = 0;
                drawHexagon(hexagons[c].x, hexagons[c].y);
                attron(COLOR_PAIR(1));
                for(auto &h : hexagons){
                    if(h.second.status)
                        drawHexagon(h.second.x, h.second.y);
                    
                }
                attron(COLOR_PAIR(20));
                cnt--;
                if(!cnt)taskloop = false;
            }
        }
        attron(COLOR_PAIR(20));
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
        
    }else if(triggers[taskName][2]=="empty"){
        int ch, y=0, t = 0;
        int r = 2+rand()%5;
        int x = 9+rand()%18;
        int rb = 2+rand()%5;
        int xb = 9+rand()%18;
        bool taskloop = true;
        clear();
        while(taskloop){
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            }else if(ch == kBinds.moveN||ch == ERR){
                if(t>0)t--;
            }else if(ch == kBinds.moveS){
                if(t<FOVY)t++;
                if(t==FOVY)taskloop = false;
            }
            
            attron(COLOR_PAIR(4));
            drawCircle(x, FOVY-1-r, r, "@0Oo.");
            attron(COLOR_PAIR(2));
            drawCircle(xb, FOVY-1-rb, rb, ".oO0@");
            attron(COLOR_PAIR(20));
            drawBox(1, 1, 3*(FOVX/4), FOVY-1, "basic");
            for(int i = 0; i<=FOVY; i++){
                 mvprintw(i, 3*(FOVX/4)+6, "  #  ");
            }
             attron(COLOR_PAIR(1));
             mvprintw(t, 3*(FOVX/4)+6, "[===]");
             attron(COLOR_PAIR(20));
        }
        
        while(cdown){
            getch();
            
            clear();
            attron(COLOR_PAIR(4));
            drawCircle(x, FOVY-1-r+y, r, "@0Oo. ");
            attron(COLOR_PAIR(2));
            drawCircle(xb, FOVY-1-rb+y, rb, ".oO0@");
            printCenter("     Task completed!     ", FOVX, FOVY/2);
            attron(COLOR_PAIR(20));
            drawBox(1, 1, 3*(FOVX/4), FOVY-1, "basic");
            for(int i = 0; i<=FOVY; i++){
                mvprintw(i, 3*(FOVX/4)+6, "  #  ");
            }
             attron(COLOR_PAIR(1));
             mvprintw(FOVY, 3*(FOVX/4)+6, "[===]");
             attron(COLOR_PAIR(20));
            cdown--;y++;
        }
        clear();
        
    }else if(triggers[taskName][2]=="stabilize"){
        
        int curX, curY;
        srand(time(NULL));
        curX = FOVX/4+rand()%(FOVX/2);
        curY = 2+rand()%(FOVY-4);
        bool taskloop = true;
        int ch;
        while(taskloop){
            clear();
            drawCircle(FOVX/2-(FOVY/2)+1, FOVY/2, FOVY/2-1, "*    ");
            
            for(int x = 0; x<FOVX; x++){
                mvaddch(curY, x, '-');
            }
            for(int y = 0; y<FOVY; y++){
                mvaddch(y, curX, '|');
            }
            mvprintw(curY, curX, "+");
            mvprintw(FOVY/2, FOVX/2, "+");
            if(curX==(int)FOVX/2&&curY==(int)FOVY/2)
                taskloop = false;
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            }else if(ch == kBinds.moveN){
                if(curY>0)curY--;
            }else if(ch == kBinds.moveS){
                if(curY<FOVY)curY++;
            }else if(ch == kBinds.moveW){
                if(curX>0)curX--;
            }else if(ch == kBinds.moveE){
                if(curX<FOVX)curX++;
            } else if(ch == kBinds.moveNE){
                if(curY>0)curY--;
                if(curX<FOVX)curX++;
            }else if(ch == kBinds.moveNW){
                if(curY>0)curY--;
                 if(curX>0)curX--;
            }else if(ch == kBinds.moveSE){
                if(curY<FOVY)curY++;
                if(curX<FOVX)curX++;
            }else if(ch == kBinds.moveSW){
                if(curY<FOVY)curY++;
                if(curX>0)curX--;
            }
        }
        attron(COLOR_PAIR(4));
        for(int x = 0; x<FOVX; x++){
            mvaddch(curY, x, '-');
        }
        for(int y = 0; y<FOVY; y++){
            mvaddch(y, curX, '|');
        }
        mvprintw(curY, curX, "+");
        attron(COLOR_PAIR(20));
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
        
        
    }else if(triggers[taskName][2]=="start"){
        
        bool taskloop =true;
        int ch;
        clear();
        int xOffset = 0;
        int xOffset2 = 31;
        std::vector<int> to_choose = {0,1,2,3,4,5,6,7,8};
        std::vector<int> combination;
        srand(time(NULL));
        for(int i=0;i<5;i++){
            int r = rand()%to_choose.size();
            combination.push_back(to_choose[r]);
        }
        drawBox(xOffset, 1, 29, 15, "double");
        
        mvprintw(0,0, " [ ]   [ ]   [ ]   [ ]   [ ]");
        for(int y =0; y<3;y++){
            for(int x = 0; x<3;x++){
                attron(COLOR_PAIR(13));
                drawSquare(xOffset+1+10*x, 2+5*y, 8,4," ");
                attron(COLOR_PAIR(20));
            }
        }
        drawBox(xOffset2, 0, 30, 16, "rounded");
        
        for(int y =0; y<3;y++){
            for(int x = 0; x<3;x++){
                attron(COLOR_PAIR(6));
                drawBox(xOffset2+1+10*x, 1+5*y, 8,4,"basic");
                attron(COLOR_PAIR(20));
                mvprintw(3+5*y, xOffset2+5+10*x, "%d",y*3+x+1);
            }
        }
        int chosen = -1;
        int stage = 1;
        int current = 0;
        bool show=true;
        bool pressed = false;
        while(taskloop){
            if(show){
                wait(5);
                for(int y =0; y<3;y++){
                    for(int x = 0; x<3;x++){
                        attron(COLOR_PAIR(6));
                        drawBox(xOffset2+1+10*x, 1+5*y, 8,4,"basic");
                    }
                }
                
                
                for(int i = 0; i<stage; i++ ){
                    wait(2);
                    attron(COLOR_PAIR(21));
                    drawSquare(xOffset+1+10*(combination[i]%3), 2+5*((int)combination[i]/3), 8,4," ");
                    wait(10);
                    attron(COLOR_PAIR(13));
                    drawSquare(xOffset+1+10*(combination[i]%3), 2+5*((int)combination[i]/3), 8,4," ");
                }
                show = false;
            }
            ch = getch();
            if(ch==kBinds.quit){
                clear();
                return false;
            }else if(ch == kBinds.moveNW){
                chosen = 0;
            }else if(ch == kBinds.moveN){
                chosen = 1;
            }else if(ch == kBinds.moveNE){
                chosen = 2;
            }else if(ch == kBinds.moveW){
                chosen = 3;
            }else if(ch == kBinds.middle){
                chosen = 4;
            }else if(ch == kBinds.moveE){
                chosen = 5;
            }else if(ch == kBinds.moveSW){
                chosen = 6;
            }else if(ch == kBinds.moveS){
                chosen = 7;
            }else if(ch == kBinds.moveSE){
                chosen = 8;
            }else if(ch == ERR){
                chosen = -1;
               
            }
            if(chosen>-1){
                pressed = true;
                attron(COLOR_PAIR(2));
                drawBox(xOffset2+1+10*(chosen%3), 1+5*((int)chosen/3), 8,4,"bold");
                if(combination[current] == chosen){
                    
                    current++;
                    if(current == stage){
                        
                        attron(COLOR_PAIR(4));
                        mvaddch(0, 2+6*(stage-1), '*');
                        stage++;
                        current = 0;
                        show = true;
                        if(stage > combination.size()){
                            taskloop = false;
                        }
                    }
                }else{
                    if(pressed){
                        current = 0;
                        stage = 1;
                        show = 1;
                        attron(COLOR_PAIR(20));
                        mvprintw(0,0, " [ ]   [ ]   [ ]   [ ]   [ ]");
                    }
                }
            
            }else{
                 pressed = false;
                for(int y =0; y<3;y++){
                    for(int x = 0; x<3;x++){
                        attron(COLOR_PAIR(6));
                        drawBox(xOffset2+1+10*x, 1+5*y, 8,4,"basic");
                    }
                }
            }
        }
        attron(COLOR_PAIR(20));
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
        
    }else if(triggers[taskName][2]=="unlock"){
        clear();
        int xOffset = 16;
        int ch;
        std::map<int, int>pressed;
        std::vector<int> to_choose = {1,2,3,4,5,6,7,8,9};
        std::vector<int> combination;
        while(to_choose.size()){
            int r = rand()%to_choose.size();
            combination.push_back(to_choose[r]);
            to_choose.erase(to_choose.begin()+r);
        }
        
        drawBox(xOffset, 0, 30, 16, "rounded");
        
        for(int y =0; y<3;y++){
            for(int x = 0; x<3;x++){
                attron(COLOR_PAIR(6));
                drawBox(xOffset+1+10*x, 1+5*y, 8,4,"basic");
                attron(COLOR_PAIR(20));
                mvprintw(3+5*y, xOffset+5+10*x, "%d",combination[y*3+x]);
            }
        }
        int chosen = -1;
        int next = 1;
        bool taskloop = true;
        while(taskloop){
            ch = getch();
            if(ch == kBinds.quit){
                clear();
                return false;
            }else if(ch == kBinds.moveNW){
                chosen = 0;
            }else if(ch == kBinds.moveN){
                chosen = 1;
            }else if(ch == kBinds.moveNE){
                chosen = 2;
            }else if(ch == kBinds.moveW){
                chosen = 3;
            }else if(ch == kBinds.middle){
                chosen = 4;
            }else if(ch == kBinds.moveE){
                chosen = 5;
            }else if(ch == kBinds.moveSW){
                chosen = 6;
            }else if(ch == kBinds.moveS){
                chosen = 7;
            }else if(ch == kBinds.moveSE){
                chosen = 8;
            }
            if(!pressed[chosen]){
                if(combination[chosen]== next){
                    next++;
                    pressed[chosen] = 1;
                    attron(COLOR_PAIR(2));
                    drawBox(xOffset+1+10*(chosen%3), 1+5*((int)chosen/3), 8,4,"bold");
                    
                    if(next == 10)taskloop = false;
                }else{
                    next = 1;
                    pressed.clear();
                    for(int y =0; y<3;y++){
                        for(int x = 0; x<3;x++){
                            attron(COLOR_PAIR(6));
                            drawBox(xOffset+1+10*x, 1+5*y, 8,4,"basic");
                        }
                    }
                }
            }
        }
        attron(COLOR_PAIR(20));
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
    }else if(triggers[taskName][2]=="divert"){
        int knob = 0;
        std::map<int, int> levels = checkLevels(tasks);
        std::vector<std::string> roomNames= {"Comms", "EN.Up", "EN.Dn", "Navig", " O2", "Secur","Shlds","Wpns"};
        
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
        attron(COLOR_PAIR(20));
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
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
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
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
            drawMap(gamemap, wallmap, x[cam], y[cam], sabbo, true);
            drawCharacters( x[cam], y[cam], positions, wallmap,model, sabbo);
            drawBox(0,0, FOVX, FOVY, "bold");
            printCenter("Camera: "+std::to_string(cam+1), FOVX, 1);
        }
    }else if(triggers[taskName][2]=="card"){
        bool taskloop = true;
        int ch, x = 0;
        int w = 25;
        clear();
        while(taskloop){
            ch = getch();
            if(ch==kBinds.moveE){
                if(x<FOVX-w)x++;
            }else if(ch==kBinds.moveW){
                if(x>0)x--;
            }else if(ch==kBinds.quit){
                clear();
                return false; 
            }
            attron(COLOR_PAIR(20));
            drawSquare(1+x, FOVY/3, w+3, FOVY/2+1, " ");
            drawBox(3+x, FOVY/3+1, 6, 3, "basic");
            
            mvprintw(FOVY/3+1, 10+x, "CREWMATE ID");
            drawBox(2+x, FOVY/3, w, FOVY/2, "rounded");
            attron(COLOR_PAIR(id+1));
            mvprintw(FOVY/3+2, 4+x, "▄███▖");
            mvprintw(FOVY/3+3, 4+x, "   █▌");
            attron(COLOR_PAIR(id+51));
            mvprintw(FOVY/3+3, 4+x, "▙▄▟");
            attron(COLOR_PAIR(21));
            drawSquare(3+x, FOVY/2+3, w-1, 2, " ");
            attron(COLOR_PAIR(13));
            drawSquare(2*(FOVX/5)+4, 2*(FOVY/3), FOVX/5, FOVY/3, " ");
            
            mvprintw(2*(FOVY/3), 2*(FOVX/5)+5, "CardScaner");
            if(x==FOVX-w){
                taskloop = false;
            }
            
        }
        attron(COLOR_PAIR(20));
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
    }else if(triggers[taskName][2]=="chart"){
        bool taskloop = true;
        std::pair<int, int> A = {2+rand()%(FOVX/3), 2+rand()%(FOVY/2)};
        std::pair<int, int> B = {(FOVX/3)+rand()%(FOVX/3), (FOVY/2)+rand()%(FOVY/2-2)};
        std::pair<int, int> C = {(FOVX/3*2)+rand()%(FOVX/3), 2+rand()%(FOVY/2)};
        int ch, curY=A.second, curX=A.first;
        clear();
        attron(COLOR_PAIR(5));
        for(int i = 0; i <10;i++){
            mvprintw(2+rand()%(FOVY-4), 3+(rand()%(FOVX/10))*i, "*" );
        }
        attron(COLOR_PAIR(20));
        while(taskloop){
            ch = getch();
            if(curX==A.first){
                if(ch == kBinds.moveN){
                    if(curY>A.second)
                        curY--;
                }else if(ch == kBinds.moveS){
                    if(curY<B.second)
                        curY++;
                }
            }
            if(curX==B.first){
                if(ch == kBinds.moveN){
                    if(curY>C.second)
                        curY--;
                }else if(ch == kBinds.moveS){
                    if(curY<B.second)
                        curY++;
                }                
            }
            if(curY==B.second){
                if(ch == kBinds.moveW){
                    if(curX>A.first)curX--;
                }else if(ch == kBinds.moveE){
                    if(curX<B.first)curX++;
                }
            }
            if(curY==C.second){
                if(ch == kBinds.moveW){
                    if(curX>B.first)curX--;
                }else if(ch == kBinds.moveE){
                    if(curX<C.first)curX++;
                }
            }
            if(ch==kBinds.quit){
                clear();
                return false; 
            }
            
            for(int y = A.second; y<B.second;y++){
                 mvprintw(y, A.first, "│");
                
            }
            for(int x = A.first; x<B.first;x++){
                 mvprintw(B.second,x, "─");
            }
            for(int y = B.second; y>C.second;y--){
                 mvprintw(y, B.first, "│");
            }
            for(int x = B.first; x<C.first;x++){
                 mvprintw(C.second,x, "─");
            }
            mvprintw(B.second, A.first, "└");
            mvprintw(C.second, B.first, "┌");
            mvprintw(A.second, A.first, "O");
            mvprintw(B.second, B.first, "O");
            mvprintw(C.second, C.first, "X");
            drawBox(0,0,FOVX, FOVY, "bold");
            mvprintw(curY, curX, "D");
            if(curX == C.first&&curY==C.second){
                taskloop = false;
            }
        }
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
    }else if(triggers[taskName][2]=="sample"){
        clear();
        bool taskloop = 1;
        int ch;
        int r = -1;
        if(sample_countdown<0){
            
            for(int i=0;i<5; i++){
                drawTesttube(8+(FOVX/6)*i,FOVY/2-1);
                mvprintw(FOVY/2+6, 9+(FOVX/6)*i,  "%d[ ] ", i+1);
            }
            for(int i=0;i<5; i++){
                attron(COLOR_PAIR(20));
                drawSquare(10+(FOVX/6)*(i-1), 1, 3, 7, " ");
                drawPipet(10+(FOVX/6)*i,1);
                drawBox(0,1,FOVX,FOVY-2, "double");
                attron(COLOR_PAIR(6));
                
                drawSquare(9+(FOVX/6)*i,FOVY/2, 5, 5, "@");
                wait(5);
            }
            
            attron(COLOR_PAIR(20));
            drawSquare(10+(FOVX/6)*(4), 1, 3, 7, " ");
            drawBox(0,1,FOVX,FOVY-2, "double");
            sample_countdown = 600;
            
            
        }
        int choice = -1;
        while(taskloop){
            
            ch = getch();
            if(ch==kBinds.quit){
                clear();
                return false;
            }else if(ch=='1'){
                choice = 0;
            }else if(ch=='2'){
                choice = 1;
            }else if(ch=='3'){
                choice = 2;
            }else if(ch=='4'){
                choice = 3;
            }else if(ch=='5'){
                choice = 4;
            }
            if(sample_countdown>0){
                sample_countdown--;
                choice = -1;
            }
            drawBox(0,1,FOVX,FOVY-2, "double");
            printCenter("Waiting for samples: "+std::to_string(sample_countdown/10)+"  ", FOVX, 1 );
            printCenter("(You don't have to wait here)", FOVX, FOVY/3, 0, 1);
            if(sample_countdown == 0){
                 printCenter("       Choose the anomaly       ", FOVX, FOVY/3, 0, 1);
                if(r <0)r=rand()%5;
                if(choice == r){
                    taskloop = false;
                }
            }
            for(int i=0;i<5; i++){
               
                drawTesttube(8+(FOVX/6)*i,FOVY/2-1);
                mvprintw(FOVY/2+6, 9+(FOVX/6)*i,  "%d[ ] ", i+1);
                if(sample_countdown==0){
                    attron(COLOR_PAIR(4));
                    if(i==choice)attron(COLOR_PAIR(1));
                    mvprintw(FOVY/2+6, 11+(FOVX/6)*i,  "*");
                }
                attron(COLOR_PAIR(6));
                if(i==r)attron(COLOR_PAIR(1));
                drawSquare(9+(FOVX/6)*i,FOVY/2, 5, 5, "@");
                attron(COLOR_PAIR(20));
                
            }
            
        }
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
        
    }else if(triggers[taskName][2]=="align"){
        clear();
        int y = FOVY/2;
        while(y==FOVY/2){
            y = 1+rand()%(FOVY-2);
        }
        std::map<int, int> indents = {
            {0, 2},
            {1, 2},
            {2, 1},
            {3, 1},
            {4, 1},
            {5, 0},
            {6, 0},
            {7, 0},
            {8, 0},
            {9, 0},
            {10, 1},
            {11, 1},
            {12, 1},
            {13, 2},
            {14, 2},
            
        };
        bool taskloop = true; 
        int ch;
        while(taskloop){
            ch = getch();
            if(ch==kBinds.moveN){
                if(y>1)y--;
            }else if(ch==kBinds.moveS){
                if(y<FOVY-1)y++;
            }else if(ch==kBinds.quit){
                clear();
                return false;
            }
            
            for(int i = 1;i<FOVY; i++){
                mvprintw(i, FOVX*0.75+4+indents[i-1], "  #  ");
                
            }
            mvprintw(y, FOVX*0.75+4+indents[y-1], "<=<=<");
            for(int x = 1; x<FOVX*0.75-1;x++){
                attron(COLOR_PAIR(1));
                mvprintw(y, x, "-");
                mvprintw(y-1, x, " ");
                mvprintw(y+1, x, " ");
                attron(COLOR_PAIR(20));
                if(y==FOVY/2){
                    attron(COLOR_PAIR(4));
                    taskloop = false;
                }
                mvprintw(FOVY/2, x, "-");
            }
            mvprintw(y, FOVX*0.75-2, "D");
            drawBox(0,0,FOVX*0.75, FOVY, "bold");
             
        }
        attron(COLOR_PAIR(20));
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
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
        attron(COLOR_PAIR(20));
        printCenter("          Task completed!           ", FOVX, FOVY/2);
        wait(20);
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
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
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
            drawMap(gamemap, wallmap, 122, 37, sabbo);
            
            drawCharacters( 122, 37, positions, wallmap,model, sabbo);
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
            drawSquare(FOVX/2+6, FOVY/6-1, FOVX/3-3, FOVY/2-1, " ");
            drawBox(FOVX/2+6, FOVY/6-1, FOVX/3-3, FOVY/2-1, "basic");
            printCenter("Scanning body...", FOVX/2, FOVY/3, FOVX/2);
            if(taskload<(FOVX-6)*3) printCenter("COLOR: "+colors[id], FOVX/2, FOVY/3, FOVX/2, 2);
            if(taskload<(FOVX-6)*2) printCenter("WIDTH: 2 chars", FOVX/2, FOVY/3, FOVX/2, 3);
            if(taskload<(FOVX-6)*1) printCenter("WEIGHT:  "+std::to_string(sizeof(positions[id]))+"bytes", FOVX/2, FOVY/3, FOVX/2, 4);
            if(taskload<4) printCenter("DONE.", FOVX/2, FOVY/3, FOVX/2, 5);
                
            taskload--;
        }
        attron(COLOR_PAIR(20));
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
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
        attron(COLOR_PAIR(20));
        printCenter("          Task completed!          ", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
    }
    
    
    return true;
    
}
void pickSabbotage(char buff[MAX], int sabbo, int sabbo_countdown, keyBinds kBinds){
    if(sabbo > 0){
        strcpy(buff, "u\n");
    }else{
        bool pickloop = true;
        int ch;
        std::string message = "s";
        clear();
        while(pickloop){
            ch = getch();
            if(ch == kBinds.quit||ch==kBinds.sabbotage){
                strcpy(buff, "u\n");
                pickloop = false;
            }else if(ch>'0'&&ch<='9'){
                message += std::to_string(ch-'0')+"\n";
                strcpy(buff, message.c_str());
                pickloop = false;
            }else if(ch == kBinds.kill){
                strcpy(buff, "sa\n");
                pickloop = false;
            }else if(ch == kBinds.use){
                strcpy(buff, "sb\n");
                pickloop = false;
            }
            printCenter("Sabbotage!", FOVX, 1);
            if(sabbo_countdown){
                attron(COLOR_PAIR(1));
                printCenter("You cannot sabbotage yet.", FOVX, 1, 0, 1);
            }
            mvprintw(2, 5, (key('1')+" Shut doors in cafeteria ").c_str());
            mvprintw(3, 5, (key('2')+" Shut doors in medbay").c_str());
            mvprintw(4, 5, (key('3')+" Shut doors in electical ").c_str());
            mvprintw(5, 5, (key('4')+" Shut doors in security ").c_str());
            mvprintw(6, 5, (key('5')+" Shut doors in upper engine").c_str());
            mvprintw(7, 5, (key('6')+" Shut doors in lower engine").c_str());
            mvprintw(8, 5, (key('7')+" Shut doors in storage").c_str());
            mvprintw(9, 5, (key('8')+" Sabbotage communications").c_str());
            mvprintw(10, 5, (key('9')+" Power down electrical").c_str());
            mvprintw(11, 5, (key(kBinds.kill)+" Reactor core meltdown").c_str());
            mvprintw(12, 5, (key(kBinds.use)+" Deplete oxygen reserves").c_str());
            attron(COLOR_PAIR(20));
        }
    }
    clear();
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
                case 'M':
                case 'r'://empty 
                    success = handleTask(tasks, triggers, kBinds, positions, gamemap, wallmap, model, sockfd, id, sample_countdown, sabbo);
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
void applyMovement(const char ch,  const int i, std::map<int, crewmate>&p, std::vector<std::string> &gamemap,int sabbo, const bool ghost) {
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
            if(game.tasks[i].current=='2'){
                for(int i = 0; i<5;i++){
                    msg += std::to_string(game.light_switches[i])+" ";
                }
            }
            for(auto el : game.position) {
                msg += std::to_string(el.second.x) + " " + std::to_string(el.second.y) + " "+ std::to_string(el.second.status) + " " ;
            }
        }
    }
    send_already:    
    send(sd, msg.c_str(), msg.size(), 0 );
}


