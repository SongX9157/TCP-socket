CS 452 Lab 2 sendfile

Song Xu (xus9157)

This assignment is about to send a secure file between a client and a server using TCP.

class Operation contains multiple functions:
    public:
	void server(): use to start as server;
	void client(): use to start as client;
	int recieveFile(const char *path, int client): use to receive file from socket, decrypt and write in to a new file;
	int sendFile(const char *path, int packetsize, int sock): use to send the specific file after encrypting it by using XOR;
    
    int main(int argc, char** argv): use argv to choose whether start as client or server;

How to run:
        compile:
	make
	./run client (start as client to send file)
	./run server (start as server to receive file)
	or
	g++ -o run sendfile.cpp

        run:
	start server first and input the port (between 9000-9999), 
	then start client and input the ip of server(which i use system() to print when server start, you can check at server) and the same 	port as server.
	if server and client connect, both of them will print "Send file:" and "Save file to:", then choose file to send and file to save.
	after that enter packet size (in kb) at client and packet size and file size will be automatically send to server.
	in both client and server input the encryption key.
	then file start to send and receive
	in the end i use system() again to print the md5sum result for the send and receive file, they should be same.

	Also, i have tried the large file such as 1G in both my unbuntu and phoenix, they seemed to work well. Large file will take more 	time and i hidden the send infomation, so just let it run until the success printed.

 Task complishment:
	All the requirements are finished in this assignment.
	
	

