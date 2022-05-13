#include "tasks.h"
#include "draw.h"

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
            
            move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
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
            move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
        }
        attron(COLOR_PAIR(4));
        printCenter("Power restored!", FOVX, FOVY/3);
        wait(20);
        clear();
    }else if(triggers[taskName][2]=="logs"){
        clear();
        drawBox(0,0,FOVX,FOVY, "rounded");
        mvprintw(1,1, "door.log:");
        mvprintw(2,1, "WARNING: SYSTEM UNRELIABLE");
        bool taskloop = true;
        int ch;
        char buff[MAX];
        int guy, door;
        std::vector<std::string> colors = {"RED", "CYAN", "MAGENTA", "GREEN", "YELLOW", "BLUE", "WHITE", "DARK RED", "DARK CYAN", "DARK MAGENTA", "DARK GREEN", "DARK YELLOW", "DARK BLUE", "GRAY"};
        while(taskloop){
            ch = getch();
            bzero(buff, sizeof(buff));
            if(ch == kBinds.quit){
                clear();
                return false;
            }else{
                strcpy(buff, "u\n");
            }
            
           
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            //mvprintw(0,0,buff);
            std::stringstream decoder;
            decoder << buff;
            char mode;
            int a, b, sab;
            decoder >> mode >> a >> sab;
            
            if(mode == 'q'){
				if(sab!=8){
					int line = 0;
	//                 decoder >> b;
					while(decoder >> guy >> door) {
						attron(COLOR_PAIR(guy+1));
						if(door < 0){
							mvprintw(3+line, 4, ">%s exits %s                  ", colors[guy].c_str(), triggers[(char)-door][1].c_str());
						}else{
							mvprintw(3+line, 4, ">%s enters %s                  ", colors[guy].c_str(), triggers[(char)door][1].c_str());
						}
						line++;
					}
					attron(COLOR_PAIR(20));
				}else{
					mvprintw(3, 4, "ERROR LOADING FILE");
					mvprintw(4, 4, "DISCONNECTED FROM SERVER");
					
				}
            }
            if(mode == 't'){
                tasks.list.clear();
                while(decoder >> a >> b) {
                    tasks.list[(char)a] = b;
                }
                tasks.current = 0;
                tasks.list.erase('8');
                taskloop = false;
            } 
            move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
        }
        tasks.list.erase('8');
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
            move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
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
            move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
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
            move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
            
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
            move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
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
            move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
        }
        attron(COLOR_PAIR(20));
        printCenter("Task completed!", FOVX, FOVY/2);
        wait(20);
        clear();
        return true;
    }else if(triggers[taskName][2]=="calibrate"){
        int ch; 
        int delay = 1;
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
            drawBox(0,0,FOVX, FOVY, "double");
            
            
            drawRing(2, 1, a, 5);
            drawRing(22, 1, b, 6);
            drawRing(42, 1, c, 2);
            attron(COLOR_PAIR(20));
            mvprintw(10, 8, "^^^");
            mvprintw(10, 28, "^^^");
            mvprintw(10, 48, "^^^");
            wait(delay);
            drawBox(2, 12, 14, 3, "basic");
            drawBox(22, 12, 14, 3, "basic");
            drawBox(42, 12, 14, 3, "basic");
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
                            drawSquare(3, 13, 13, 2, " ");
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
                            drawSquare(23, 13, 13, 2, " ");
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
                            drawSquare(43, 13, 13, 2, " ");
                            taskloop = false;
                        }
                    }
                break;
            }
            attron(COLOR_PAIR(20));
            move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
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
            drawBox(0,0,FOVX, FOVY, "double");
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
            move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
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
             move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
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
            move(FOVY+1, 0);
            attron(COLOR_PAIR(14));
            printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
            attron(COLOR_PAIR(20));
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
            move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
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
        move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
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
        move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
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
        move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
            
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
        move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));    
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
        move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
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
        move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));    
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
         move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));    
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
         move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));   
            
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
        move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
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
            
        move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
            
            
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
        move(FOVY+1, 0);
        attron(COLOR_PAIR(14));
        printw(getControls(tasks.current, true, positions[id].status, kBinds).c_str());
        attron(COLOR_PAIR(20));
    }

    return result;
}
