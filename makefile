CFLAGS = -g

all: face_detection

face_detection: face_detection.cpp
	g++ face_detection.cpp $(CFLAGS) -o face_detection


clean:
	rm *.o face_detection
