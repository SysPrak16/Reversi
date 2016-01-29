CC 		= gcc
CFLAGS 		= -std=gnu99 -pipe -g
OBJECTS 	= main.o performConnection.o connector.o thinker.o fileIOHandler.o
TARGET 		= reversi
GAME_ID     = -g 1234567890

all: $(OBJECTS)
		$(CC) $(OBJECTS) -o $(TARGET)

play: all
	./$(TARGET) $(GAME_ID)

####### Compile

main.o: main.c fileIOHandler.h connector.h
	$(CC) -c $(CFLAGS) -o main.o main.c

performConnection.o: performConnection.c performConnection.h connector.h global.h
	$(CC) -c $(CFLAGS) -o performConnection.o performConnection.c

connector.o: connector.c connector.h performConnection.h
	$(CC) -c $(CFLAGS) -o connector.o connector.c

fileIOHandler.o: fileIOHandler.c fileIOHandler.h global.h
	$(CC) -c $(CFLAGS) -o fileIOHandler.o fileIOHandler.c

thinker.o: thinker.c thinker.h 
	$(CC) -c $(CFLAGS) -o thinker.o thinker.c

####### Clean

clean:
		rm -f $(OBJECTS) 
