CFLAGS = -g -lstdc++

all: readImage readImageHeader writeImage image pa03

readImage.o: image.h readImage.cpp
	g++ -c $(CFLAGS) readImage.cpp

readImageHeader.o: image.h readImageHeader.cpp
	g++ -c $(CFLAGS) readImageHeader.cpp

writeImage.o: image.h writeImage.cpp
	g++ -c $(CFLAGS) writeImage.cpp

image.o: image.h image.cpp
	g++ -c $(CFLAGS) image.cpp

pa03: image.h image.o readImage.o readImageHeader.o writeImage.o main.cpp
	g++ -o pa03 $(CFLAGS) image.o readImage.o readImageHeader.o writeImage.o main.cpp

clean:
	rm *.o readImage image writeImage readImageHeader
