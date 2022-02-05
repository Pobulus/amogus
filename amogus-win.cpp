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
            move(k, i);
            switch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2))){
                case '%':
                    addch((char)219);
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
int distance(int x1, int y1, int x2, int y2){
    return sqrt(pow(x1-x2, 2)+pow(y1-y2, 2));
}
void await(int sockfd, const int id, std::map<int, crewmate>  &positions, crewmate &ghost, std::vector<std::string> &gamemap, std::vector<std::string> &wallmap ,playermodel model)
{
    
    drawMap(gamemap, positions[id].x, positions[id].y);
    refresh();
    char buff[MAX];
    int a = 0;
    mvprintw(0,0,"Starting loop");
    while(true) {
        
        
        refresh();
        
        a = getch();
        mvprintw(0,0,"Starting loop %d", a);
        bzero(buff, sizeof(buff));
        switch(a){
            case 'q':
                endwin();
                closesocket(sockfd);
                exit(0);
                
                break;
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
                mvprintw(0,0,"Timeout ");
                break;
            case 'x':
                strcpy(buff, "k\n");
                break;
            default:
                buff[0] = 'u';
                buff[1] = '\n';
                //mvprintw(0, 0, "pressed: %d", a);
                break;
        }
//         clear();
        mvprintw(FOVY,0,"To Server : %s   ", buff);
        send(sockfd, buff, sizeof(buff), 0);
        bzero(buff, sizeof(buff));
        recv(sockfd, buff, sizeof(buff), 0);
        
        mvprintw(FOVY+1,0,"From Server : %s   ", buff);
                std::stringstream decoder;
                decoder << buff;
                int a=0, b=0, c=0, cnt=0;
                char mode;
                decoder >> mode;
                if (mode == 'p'){
                    while(decoder >> a >> b >> c){
                        mvprintw(positions[cnt].y, positions[cnt].x, " ");
                        positions[cnt] = {a, b, c};
    
                        cnt++;
                    }
                }
                if (mode == 'g'){
                    decoder >> ghost.x >> ghost.y;
                    while(decoder >> a >> b >> c){
                        mvprintw(positions[cnt].y, positions[cnt].x, " ");
                        positions[cnt] = {a, b, c};
                        
                        cnt++;
                    }
                }
                if(positions[id].status%2){
                    drawMap(gamemap, positions[id].x, positions[id].y);
                    int y = positions[id].y;
                    int x = positions[id].x;
                    for(auto ch : positions){
                        int posx = ch.second.x-x+(FOVX/2);
                        int posy = ch.second.y-y+(FOVY/2);

                        if(posx<FOVX&&posy<FOVY&&LOS(x, y, ch.second.x, ch.second.y, wallmap)){
                        attron(COLOR_PAIR(ch.first+1));
                            if(ch.second.status %2)
                                mvprintw(posy, posx, model.alive.c_str());
                            else{ 
                                    mvprintw(posy, posx, model.body.c_str());
                            }
                            attron(COLOR_PAIR(10));
                        }
                    }
                        
                }else {
                    drawMap(gamemap, ghost.x, ghost.y);
                    int y = ghost.y;
                    int x = ghost.x;
                    attron(COLOR_PAIR(id+1));
                    mvprintw((FOVY/2), (FOVX/2), model.ghost.c_str());
                    attron(COLOR_PAIR(10));
                    for(auto ch : positions){
                        int posx = ch.second.x-x+(FOVX/2);
                        int posy = ch.second.y-y+(FOVY/2);

                        if(posx<FOVX&&posy<FOVY&&LOS(x, y, ch.second.x, ch.second.y, wallmap)){
                        attron(COLOR_PAIR(ch.first+1));
                            if(ch.second.status %2)
                                mvprintw(posy, posx, model.alive.c_str());
                            else{ 
                                    mvprintw(posy, posx, model.body.c_str());
                            }
                            attron(COLOR_PAIR(10));
                        }
                    }
                }
                
        
        
    }
}
   
