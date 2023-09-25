# Amogus ඞ

Among Us recreated in terminal. 

Warning: the code for this is **extremely bad**. No objects, no classes, it's just a big bundle of spaghetti that I bodged togheter to form a somewhat functioning game. This was probably a mistake.

*(that being said it is pretty fun to play with friends)*
## Features 
- 14 vents
- 39 task stations
- 11 sobbotage options
- working cameras
- emergency meeting
![murder](https://github.com/Pobulus/amogus/assets/32367046/0b09acc6-6dce-4540-9367-33141a2e4527)
## Compiling
You have to install libncurses first.
`cmake . && make all` should work. If it doesn't try these:


- client:
`g++ amogus.cpp amogus-server-main.cpp -o amogus -lncursesw`
- server:
`g++ amogus-server-main.cpp  amogus.cpp -o server -lncursesw`
- keybinder:
`g++ keybinder.cpp -o keybinder -lncursesw`

## Usage 
First start up the server:
`./server`<br>
Then connect your client like so:
`./amogus -i <ip address>`<br>
(it uses port 8888 but you don't have to specify it anywhere)<br>
If your terminal doesn't support unicode, pass the `-a` argument for ascii only.<br>
You can generate custom keybinds file with `keybinder`, then pass the `-k <filename>` argument to use it
