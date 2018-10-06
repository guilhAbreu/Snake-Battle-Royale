# Macros para compilacao
CC = g++
CFLAGS = -std=c++11 -lpthread 
NCURSES_FLAGS = -lncurses
PORTAUDIO_FLAGS = -lportaudio -ljack -lrt -lasound -lm
ASSETS_ZIP = audio/assets/assets.zip
ASSETS_DIR = audio/assets
STAT_DIR = statistics
DIR = src
TARGET = snake\ single\ player
REMOTE_TARGET = remote-controler
REMOTE_DIR = player
REMOTE_SRCS = $(shell find $(REMOTE_DIR) -name '*.cpp')
REMOTE_OBJS = $(REMOTE_SRCS:.c=.o)
SRCS := $(shell find $(DIR) -name '*.cpp')
OBJS = $(SRCS:.c=.o)
ERROR_FILE = error_log

.PHONY: depend clean

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o$(TARGET) $(OBJS) $(CFLAGS) $(NCURSES_FLAGS) $(PORTAUDIO_FLAGS)

%.o:%.cpp
	$(CC) $(SRCS) $(CFLAGS) -c $< -o $@

server:all sound
	./$(TARGET) 2>$(ERROR_FILE)

sound:
	unzip -n $(ASSETS_ZIP) -d $(ASSETS_DIR)/

client: remote
	./$(REMOTE_TARGET) $(IP) $(PORT)

remote:$(REMOTE_TARGET)

$(REMOTE_TARGET):$(REMOTE_OBJS)
	$(CC) -o$(REMOTE_TARGET) $(REMOTE_OBJS) $(CFLAGS) $(NCURSES_FLAGS) $(PORTAUDIO_FLAGS)

%.o:%.cpp
	$(CC) $(REMOTE_SRCS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) ./$(TARGET) ./$(REMOTE_TARGET)
	$(RM) $(DIR)/*.o $(REMOTE_DIR)/*.o
	$(RM) $(ASSETS_DIR)/*.dat
	$(RM) $(STAT_DIR)/*.est
	>$(ERROR_FILE)