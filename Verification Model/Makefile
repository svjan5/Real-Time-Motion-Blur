CC = g++
RM = /bin/rm -rf

CFLAGS = -O3 -Wall -std=c++11 

INCDIRS = -I. 
LIBDIRS = -L.
LIBS = -lGL -lGLEW -lglut -lm -lSOIL

BIN = main
SRCS = main.cpp

OBJS = main.o 

all: clean ${BIN}
	sudo ./main
# compile all '.o' files from their like named '.cpp' files and then link
#   them into a file name ${BIN}
${BIN} : ${OBJS}
	${CC} ${OBJS} ${LIBDIRS} ${LIBS} -o $@ 
.cpp.o :
	${CC} ${CFLAGS} ${INCDIRS} -c $< 


d:debug
	valgrind ./debug ./input.off input.crd

debug: clean dc
	${CC} ${OBJS} ${LIBDIRS} ${LIBS} -g -o $@
dc:
	${CC} ${CFLAGS} ${INCDIRS} -g -c *.cpp
# specify clobber and clean as phony so they still run even if files
#   exist with the same names
.PHONY : clean remake
clean :
	${RM} ${BIN}
	${RM} *.o

remake : clean ${BIN}

#make a list of dependencies using makedepend
depend:
	makedepend -- $(CFLAGS) -- -Y $(SRCS)
