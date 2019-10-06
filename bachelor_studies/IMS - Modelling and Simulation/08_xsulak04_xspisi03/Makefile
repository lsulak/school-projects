CC =g++
CFLAGS = -std=c++11 -Wall -Wextra -pedantic -c
NAME = dalnice
ZIPPED = Makefile dokumentace.pdf $(NAME).cpp
LIBS = -lsimlib -lm
SRC_FILES = $(wildcard *.cpp)
OBJ_FILES = $(patsubst %.cpp, %.o, $(SRC_FILES))

CFLAGS_RELEASE = -O3 -s
CFLAGS_DEBUG = -O0 -g -DDEBUG

# Targets
release: CFLAGS += $(CFLAGS_RELEASE)
release: build

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: build

build: $(OBJ_FILES)
	$(CC) $^ -o $(NAME) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f dalnice *.zip *.o

zip: clean
	zip 08_xsulak04_xspisi03.zip $(ZIPPED)

run:
	./$(NAME)
