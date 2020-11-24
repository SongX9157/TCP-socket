run:sendfile.o
	g++ -g -o run sendfile.o
sendfile.o: sendfile.cpp
	g++ -g -c sendfile.cpp
clean:
	rm sendfile.o run
