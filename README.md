# Ȃ̶̢̯̩͕͎͇̼̅̈́͒̓̀̉̿̂͘̕M̸̢̡̳̥̯̮̯̭̤̬̯͎͓̄̐̔Ơ̶̡̫̩͙̥͍͇̩̪̬̅̔̿́́̎̈́͐̇̏̆͘Ģ̸͉̝̜̻̪͙̬̠̮̰̩̻̻̈́̀̓ͅŬ̷͎̬͍̠̠̹͇͍̦͓̺Ş̷̨̡̨̧͖̙̠̤̼̦͇͈͓́̉̽͘͜

## Compiling
You have to install libncurses first.
`cmake . && make all` should work. If it doesn't try these:<br>
- client:
`g++ amogus.cpp amogus-server-main.cpp -o amogus -lncursesw`
- server:
`g++ amogus-server-main.cpp  amogus.cpp -o server -lncursesw`
- server:
`g++ amogus-server-main.cpp  amogus.cpp -o server -lncursesw`



## Usage 
First start up the server:
`./server`<br>
Then connect your client like so:
`./amogus -i <ip address>`<br>
(it uses port 8888 but you don't have to specify it anywhere)<br>
If your terminal doesn't support unicode, pass the `-a` argument for ascii only.<br>

