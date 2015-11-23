CC            = gcc
CFLAGS        = -std=gnu99 -pipe -Wcomment -g -Wall

OBJECTS       = main.o performConnection.o connector.o config.o thinker.o
TARGET	      = muehle

all: $(OBJECTS)
		$(CC) $(OBJECTS) -o $(TARGET)

####### Compile

main.o: main.c constants.h \
		config.h \
		connector.h \
		thinker.h
	$(CC) -c $(CFLAGS) -o main.o main.c

performConnection.o: performConnection.c performConnection.h \
		connector.h \
		config.h \
		constants.h
	$(CC) -c $(CFLAGS) -o performConnection.o performConnection.c

connector.o: connector.c connector.h \
		config.h \
		constants.h \
		performConnection.h
	$(CC) -c $(CFLAGS) -o connector.o connector.c

config.o: config.c config.h \
		constants.h
	$(CC) -c $(CFLAGS) -o config.o config.c

thinker.o: thinker.c thinker.h \
		connector.h \
		config.h \
		constants.h
	$(CC) -c $(CFLAGS) -o thinker.o thinker.c

####### Clean

clean:
		rm -f $(OBJECTS) 
