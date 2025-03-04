# CPSC-351 IPC File Transfer

## Section Information
- Section: 09 14061
- Name: Kenny Garcia
- Email: kennygarcia15@csu.fullerton.edu

## Programming Language
- Both the sender and receiver files are implemented in **C++**

## Directory Contents

```bash
p1-bebopkenny/
│── Makefile/                
│── msg.h/           
│── sender.cpp         
│── recv.cpp       
│── signaldemo.cpp 
│── keyfile.txt     
│── cat.jpg           
│── empty_file.txt  
│── lorem_ipsum.txt      
│── README.md           
```

## How to Build
```bash
cd p1-bebopkenny     # Go to the directory
make                 # This compiles sender and recv
```

## How to Run
1. Open two terminals in the p1-bebopkenny directory
2. Terminal 1 is the Receiver:
```bash
./recv
```
3. Terminal 2 is the Sender
```bash
./sender <FILE>
```
Replace the file with the one you want to program it with. 
For example:
```bash
./sender cat.jpg
```
The receiver will save the file and transfer it as ```<originalName>__recv```
For example: ```cat.jpg__recv```

## Sample Usage and Screenshot

**Image of 4 unit tests**
![Self Unit Tests](https://cdn.discordapp.com/attachments/1299155448959598595/1346368028853538817/Screenshot_2025-03-03_222526.png?ex=67c7ee70&is=67c69cf0&hm=839b92912beea5bcc24cd4ab4bcc86e9532fa623c3f7e70d9c99173e5298e1a8&)

- Terminal 1:
```bash
./recv
```
It will display on the terminal like:
```bash
The number of bytes received is: X
```
once completed.
- Terminal 2:
```bash
./sender cat.jpg
```
It will display:
```bash
The number of bytes sent is X
```

After testing, make sure to check the directory files and see if ```cat.jpg__recv``` matches the original ```cat.jpt``` by running:
```bash
diff cat.jpg cat.jpg__recv
```
If the terminal does not output anything, then the two files are the same.



## Extra Credit 
No implementation on the extra credit.

## Anything Special?
- In the unit test image that I provided above, I tested the program with text files, empty files, and binary images.
- In the terminal that the user pressed ```./recv```. If the user presses ```Ctrl+C``` it beings the queue by a signal handler and cleans up the shared memory.