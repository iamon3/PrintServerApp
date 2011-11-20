CC=gcc
CC_WARN=${CC} -Wall
CC_WARN_OBJ=${CC_WARN} -c
CC_WARN_DEBUG=${CC_WARN} -g
CC_WARN_DEBUG_OBJ=${CC_WARN_OBJ} -g

compile: server client printer

server: server.o communication.o searhingAndSorting.o logicalComponants.o initDataStruct.o helperFunctions.o DataStructurManupulation.o
	${CC_WARN} $^ -o $@

client: client.o
	${CC_WARN} $^ -o $@

printer: printer.o
	${CC_WARN} $^ -o $@ -lpthread -lncurses

server.o: server.c MainHeader.h dataStructures.h initDataStruct.h communication.h logicalComponants.h
	${CC_WARN_OBJ} server.c

communication.o: communication.c communication.h
	${CC_WARN_OBJ} communication.c

searhingAndSorting.o: searhingAndSorting.c searchingAndSorting.h MainHeader.h
	${CC_WARN_OBJ} searhingAndSorting.c

logicalComponants.o: logicalComponants.c logicalComponants.h manipulateDataStruct.h helperFunctions.h searchingAndSorting.h 
	${CC_WARN_OBJ} logicalComponants.c

initDataStruct.o: initDataStruct.h initDataStruct.c MainHeader.h searchingAndSorting.h
	${CC_WARN_OBJ} initDataStruct.c

helperFunctions.o: helperFunctions.h helperFunctions.c searchingAndSorting.h manipulateDataStruct.h communication.h
	${CC_WARN_OBJ} helperFunctions.c

DataStructurManupulation.o: manipulateDataStruct.h DataStructurManupulation.c initDataStruct.h searchingAndSorting.h
	${CC_WARN_OBJ} DataStructurManupulation.c

printer.o: printer.c
	${CC_WARN_OBJ} printer.c

client.o: client.c
	${CC_WARN_OBJ} client.c

clean:
	rm *~ *.o
