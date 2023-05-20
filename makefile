SRC := src
OBJ := obj
CC := sdcc
CCFLAGS := -c -mmcs51 --model-large --opt-code-size
LDFLAGS := -mmcs51 --model-large --stack-size 128 --xram-size 1024 --code-size 16384

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.rel, $(SOURCES))

all: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o keyer.ihx

clean:
	rm -f keyer.*
	rm -f obj/* 
	rm -f src/*.gch
	rm -f src/*.o
	rm -f src/*.rel
	rm -f src/*.lst
	rm -f src/*.asm
	rm -f src/*.sym

$(OBJ)/%.rel: $(SRC)/%.c
	$(CC) -I$(SRC) $(CCFLAGS) $< -o $@
