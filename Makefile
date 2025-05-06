CC = mpic++
SRCS = ./src/*
INC = ./src/
OPTS = -std=c++17 -Wall -Werror

EXEC = nbody

# Make directory for target nbody executable
#$(shell mkdir -p bin)
# Make directories for outputs
#$(shell mkdir -p output)

all: clean compile

compile:
	$(CC) $(SRCS) -I $(INC) $(OPTS) -o $(EXEC)

clean:
	rm -f $(EXEC)