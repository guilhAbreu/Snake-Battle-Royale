# Macros para compilacao
CC = g++
CFLAGS = -std=c++11 -lpthread 
NCURSES_FLAGS = -lncurses
PORTAUDIO_FLAGS = -lportaudio -ljack -lrt -lasound -lm

# Diretorios
ASSETS_DIR = audio/assets
STAT_DIR = statistics
MAIN_DIR = src/main
API_DIR = src/api
SERIAL_DIR = serial
REMOTE_DIR = player

# Programas a serem gerados
TARGET = snake\ single\ player
REMOTE_TARGET = remote-controler

# Arquivos Fontes
MAIN_SRC := $(shell find $(MAIN_DIR) -name '*.cpp')
API_SRCS := $(shell find $(API_DIR) -name '*.cpp')
SERIAL_SRCS := $(shell find $(SERIAL_DIR) -name '*.cpp')
REMOTE_SRCS = $(shell find $(REMOTE_DIR) -name '*.cpp')

# Objetos gerados durante fase de compilacao
REMOTE_OBJS = $(REMOTE_SRCS:.c=.o)
SERIAL_OBJS := $(SERIAL_SRCS:.c=.o)
MAIN_OBJ := $(MAIN_SRC:.c=.o)
API_OBJS = $(API_SRCS:.c=.o)

# Arquivos necessarios para executar os programas
ASSETS_ZIP = audio/assets/assets.zip
ERROR_FILE = error_log

.PHONY: depend clean

all:$(TARGET) $(REMOTE_TARGET) sound

$(TARGET):$(MAIN_OBJ) $(API_OBJS) $(SERIAL_OBJS)
	$(CC) -o$(TARGET) $(MAIN_OBJ) $(API_OBJS) $(SERIAL_OBJS) $(CFLAGS) $(NCURSES_FLAGS) $(PORTAUDIO_FLAGS)

%.o:%.cpp
	$(CC) $(MAIN_SRC) $(API_SRCS) $(SERIAL_SRCS) $(CFLAGS) -c $< -o $@

server:$(TARGET) sound
	./$(TARGET) $(PORT) 2>$(ERROR_FILE)

sound:
	unzip -n $(ASSETS_ZIP) -d $(ASSETS_DIR)/

client: remote
	./$(REMOTE_TARGET) $(IP) $(PORT)

remote:$(REMOTE_TARGET)

$(REMOTE_TARGET):$(REMOTE_OBJS) $(API_OBJS) $(SERIAL_OBJS)
	$(CC) -o$(REMOTE_TARGET) $(REMOTE_OBJS) $(API_OBJS) $(SERIAL_OBJS) $(CFLAGS) $(NCURSES_FLAGS) $(PORTAUDIO_FLAGS)

%.o:%.cpp
	$(CC) $(REMOTE_SRCS) $(API_SRCS) $(SERIAL_SRCS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) ./$(TARGET) ./$(REMOTE_TARGET)
	$(RM) $(API_DIR)/*.o $(SERIAL_DIR)/*.o $(MAIN_DIR)/*.o $(REMOTE_DIR)/*.o
	$(RM) $(ASSETS_DIR)/*.dat
	$(RM) $(STAT_DIR)/*.est
	>$(ERROR_FILE)