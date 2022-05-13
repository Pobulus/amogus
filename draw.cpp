#include "draw.h"

void printCenter(std::string text, const int limX, const int limY, const int offX, const int offY) {
    int halfText = text.length()/2;
    mvprintw(limY/2+offY, (limX/2)-halfText+offX, text.c_str());
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
                case '&':
                    addch(' ');
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

