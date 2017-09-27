#---------------------------------------------------
#
# Makefile
#
# for simple openGl programs
# with GLUT-library for X-window and event handling
#
#---------------------------------------------------
CC          = g++
CCFLAGS     = -c
LIBS        = -L /usr/X11/lib
INCLUDES    =
LDFLAGS     = -lglut -lGL -lGLU -lm

MYOBJECTS   = main.o waveother.o waveinteraction.o wavedisplay.o vector.o glutwindow.o datasourcerawringbuffer.o datasourceraw.o datasource.o config.o colormapasc.o
BIN         = wave

$(BIN): $(MYOBJECTS) Makefile
	$(CC) $(MYOBJECTS) $(LIBS) $(LDFLAGS)  -o $(BIN)
#	./$(BIN)

.cpp.o:
	$(CC) $(INCLUDES) $(CCFLAGS) $<

clean:
	rm -f *.o $(BIN) *~


