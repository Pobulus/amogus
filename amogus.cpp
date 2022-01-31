#include "amogus.h"

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
                    printw("█");
                    break;
                default:
                    addch(gmap[k+y-(FOVY/2)].at(i+x-(FOVX/2)));
                    break;
                
            }
            
            }
        }  
    }
    //mvprintw((FOVY/2), (FOVX/2), "ඞ");
    mvprintw(0, 30, "%d, %d", x, y);
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
void await(int sockfd, const int id, std::map<int, std::pair<int, int>>  &positions, std::vector<std::string> &gamemap, std::string crewmate)
{
    
    drawMap(gamemap, positions[id].first, positions[id].second);
    refresh();
    char buff[MAX];
    int a = 0;
    for (;;) {
        
        mvprintw(3, 0, "pressed: %d", a);
        refresh();
        a = getch();
        
        bzero(buff, sizeof(buff));
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
                mvprintw(0,0,"Timeout ");
                break;
            default:
                buff[0] = 'u';
                buff[1] = '\n';
                
                break;
        }
//         clear();
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        
        mvprintw(0,0,"From Server : %s   ", buff);
                std::stringstream decoder;
                decoder << buff;
                int a=0, b=0, cnt=0;
                
                while(decoder >> a >> b){
                    mvprintw(positions[cnt].second, positions[cnt].first, " ");
                    positions[cnt] = {a, b};
 
                    cnt++;
                }
                drawMap(gamemap, positions[id].first, positions[id].second);
                int y = positions[id].second;
                int x = positions[id].first;
                for(auto ch : positions){
                    int posx = ch.second.first-x+(FOVX/2);
                    int posy = ch.second.second-y+(FOVY/2);
                    if(posx < FOVX && posy < FOVY){
                        attron(COLOR_PAIR(ch.first+1));
                        mvprintw(posy, posx, crewmate.c_str());
                        attron(COLOR_PAIR(10));
                    }
                }
                
        
        
    }
}
   
