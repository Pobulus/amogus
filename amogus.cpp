#include "amogus.h"

bool readParameters ( int& argc, char** argv, std::string& ip, bool &verb ) {
    bool errors = false;
    for ( int i = 1; i < argc - 1; i++ ) {
        if ( not strcmp ( argv[i], "-i" ) ) { //adres ip
            ip = argv[i + 1];
        
        } else if ( not strcmp ( argv[i], "-v" ) ) { //verbose, wypisuj dodatkowe informacje
            verb = true;
        } else if ( not strcmp ( argv[i], "-h" ) ) { //pomoc
            errors = true;
        }
    }
    if ( not strcmp ( argv[argc - 1], "-v" ) ) verb = true; //verbose, wypisuj dodatkowe informacje
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
void await(int sockfd, const int id, std::map<int, std::pair<int, int>>  &positions)
{
    
    char buff[MAX];
    int n;
    int a = 0;
    for (;;) {
        mvprintw(3, 0, "pressed: %d", a);
        refresh();
        a = getch();
        
        bzero(buff, sizeof(buff));
        switch(a){
            case KEY_UP:
                strcpy(buff, "m8\n");
                break;
            case KEY_LEFT:
               strcpy(buff, "m4\n");
                break;
            case KEY_RIGHT:
                strcpy(buff, "m6\n");
                break;
            case KEY_DOWN:
                strcpy(buff, "m2\n");

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
                    attron(COLOR_PAIR(cnt+1));
                    mvprintw(b, a, "ඞ");
                    attron(COLOR_PAIR(10));
                    cnt++;
                }
                
                 
                
                
        
        
    }
}
   
