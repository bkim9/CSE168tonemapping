
#Use tpp7 for Pentium 4
#icc intel compiler -tpp7 for Pentium 4
#CC      = icc 
#CFLAGS  = -O3 -tpp7 -static 
CC     = g++
CFLAGS = -O3 -funroll-loops 
INCLUDE = 
# do not forget to comment LIBS to use intel compiler
LIBS    = -lm

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

all:	logmap

logmap:	tone_main.o logmapping.o color_convert.o rgbe.o
	$(CC) $(CFLAGS) $^ $(INCLUDE) -o $@ $(LIBS)

clean:
	\rm *.o logmap core


