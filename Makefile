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
SRCS := $(shell find $(DIR) -name '*.cpp')
OBJS = $(SRCS:.c=.o)

.PHONY: depend clean

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o$(TARGET) $(OBJS) $(CFLAGS) $(NCURSES_FLAGS) $(PORTAUDIO_FLAGS)

%.o:%.cpp
	$(CC) $(SRCS) $(CFLAGS) -c $< -o $@

run:all sound
	./$(TARGET) 2>/dev/null

sound:
	unzip -n $(ASSETS_ZIP) -d $(ASSETS_DIR)/

clean:
	$(RM) ./$(TARGET)
	$(RM) $(DIR)/*.o
	$(RM) $(ASSETS_DIR)/*.dat
	$(RM) $(STAT_DIR)/*.est