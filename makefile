CFLAGS = -g

all: readImage writeImage image

readImage.o:	image.h readImage.cpp
	g++ -c $(CFLAGS) readImage.cpp

writeImage.o:	image.h writeImage.cpp
	g++ -c $(CFLAGS) writeImage.cpp

image.o:	image.h image.cpp
	g++ -c $(CFLAGS) image.cpp

clean:
	rm *.o readImage image writeImage
