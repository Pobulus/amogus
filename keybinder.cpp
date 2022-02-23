
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <ncurses.h>

struct keyBinds {
    int moveN;
    int moveE;
    int moveW;
    int moveS;
    int moveNE;
    int moveNW;
    int moveSE;
    int moveSW;
    int middle;
    int kill;
    int use;
    int report;
    int sabbotage;
    int quit;
};


int main() {
    keyBinds kb;
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    curs_set(0);
    int c;
    mvprintw(0,0, "Type the key you want to set as following actions\n");
    printw("move up: ");
    c = getch();
    kb.moveN = c;
    printw("%d [%c]\n", c, (char)c);
    printw("move left: ");
    c = getch();
    kb.moveW = c;
    printw("%d [%c]\n", c, (char)c);
    printw("move right: ");
    c = getch();
    kb.moveE = c;
    printw("%d [%c]\n", c, (char)c);
    printw("move down: ");
    c = getch();
    kb.moveS = c;
    printw("%d [%c]\n", c, (char)c);
    printw("move up-left: ");
    c = getch();
    kb.moveNW = c;
    printw("%d [%c]\n", c, (char)c);
    printw("move up-right: ");
    c = getch();
    kb.moveNE = c;
    printw("%d [%c]\n", c, (char)c);
    printw("move down-left: ");
    c = getch();
    kb.moveSW = c;
    printw("%d [%c]\n", c, (char)c);
    printw("move down-right: ");
    c = getch();
    kb.moveSE = c;
    printw("%d [%c]\n", c, (char)c);
    printw("middle[5]: ");
    c = getch();
    kb.middle = c;
    printw("%d [%c]\n", c, (char)c);
    printw("kill: ");
    c = getch();
    kb.kill = c;
    printw("%d [%c]\n", c, (char)c);
    printw("use: ");
    c = getch();
    kb.use = c;
    printw("%d [%c]\n", c, (char)c);
    printw("report body: ");
    c = getch();
    kb.report = c;
    printw("%d [%c]\n", c, (char)c);
    printw("open sabbotage menu: ");
    c = getch();
    kb.sabbotage= c;
    printw("%d [%c]\n", c, (char)c);
    printw("Quit: ");
    c = getch();
    kb.quit= c;
    printw("%d [%c]\n", c, (char)c);
    printw("Is this correct?");
    c = getch();
    if(c == 'y'||c=='Y') {
        printw("Y\nSaving file...\n");
        std::ofstream fileStream ( "keybinds.txt" );
        if(fileStream) {
            fileStream << kb.moveN << " " << kb.moveE << " " << kb.moveW << " " << kb.moveS;
            fileStream << " " << kb.moveNE << " " << kb.moveNW << " " << kb.moveSE << " " << kb.moveSW;
            fileStream << " " << kb.middle;
            fileStream << " " << kb.kill << " " << kb.use << " " << kb.report << " " << kb.sabbotage << " "<< kb.quit;

            fileStream.close();
             std::cout << " " <<"File exported as keybinds.txt."<<std::endl;
        }
        else {
            printw("Error opening file.");
            endwin();
            return 1;
        }
    }
    endwin();
   
    return 0;
}


