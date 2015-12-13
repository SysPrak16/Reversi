CC 				= gcc
CFLAGS 		= -Wall -st=c99 -g

OBJECTS 	= main.o performConnection.o connector.o thinker.o appParamHandler.o errorHandler.o fileIOHandler.o
TARGET 		= reversi

all: $(OBJECTS)
		$(CC) $(OBJECTS) -o $(TARGET)
		
####### Compile

main.o: main.c appParamHandler.h \
		fileIOHandler.h \
		connector.h \
	$(CC) -c $(CFLAGS) -o main.o main.c
	
appParamHandler.o: appParamHandler.o appParamHandler.h\
	$(CC) -c $(CFLAGS) -o appParamHandler.o appParamHandler.c	

performConnection.o: performConnection.c performConnection.h \
		connector.h \
		global.h \
	$(CC) -c $(CFLAGS) -o performConnection.o performConnection.c

connector.o: connector.c connector.h \
		performConnection.h
	$(CC) -c $(CFLAGS) -o connector.o connector.c
	
errorHandler.o: errorHandler.c errorHandler.h\
	$(CC) -c $(CFLAGS) -o errorHandler.o errorHandler.c

fileIOHandler.o: fileIOHandler.c fileIOHandler.h\
		errorHandler.h\
		global.h
	$(CC) -c $(CFLAGS) -o fileIOHandler.o fileIOHandler.c

thinker.o: thinker.c thinker.h \
	$(CC) -c $(CFLAGS) -o thinker.o thinker.c

####### Clean

clean:
		rm -f $(OBJECTS) 
