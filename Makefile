# Macros para compilacao
CC = g++
CFLAGS = -std=c++11 -lpthread 
NCURSES_FLAGS = -lncurses
PORTAUDIO_FLAGS = -lportaudio -ljack -lrt -lasound -lm

# Diretorios
ASSETS_DIR = audio/assets
MAIN_DIR = server
API_DIR = src
SERIAL_DIR = serial
REMOTE_DIR = client

# Programas a serem gerados
TARGET = snake\ battle\ royale\ server
REMOTE_TARGET = snake\ battle\ royale\ client

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
SERVER_ERROR_FILE = server_error_log
CLIENT_ERROR_FILE = client_error_log

.PHONY: depend clean

all:$(TARGET) $(REMOTE_TARGET) sound

$(TARGET):$(MAIN_OBJ) $(API_OBJS) $(SERIAL_OBJS)
	$(CC) -o$(TARGET) $(MAIN_OBJ) $(API_OBJS) $(SERIAL_OBJS) $(CFLAGS) $(NCURSES_FLAGS) $(PORTAUDIO_FLAGS)

%.o:%.cpp
	$(CC) $(MAIN_SRC) $(API_SRCS) $(SERIAL_SRCS) $(CFLAGS) -c $< -o $@

server:$(TARGET)
	./$(TARGET) $(PORT) $(PLAYERS) 2>$(SERVER_ERROR_FILE)

sound:
	unzip -n $(ASSETS_ZIP) -d $(ASSETS_DIR)/

client: remote sound
	./$(REMOTE_TARGET) $(IP) $(PORT) 2>$(CLIENT_ERROR_FILE)

remote:$(REMOTE_TARGET)

$(REMOTE_TARGET):$(REMOTE_OBJS) $(API_OBJS) $(SERIAL_OBJS)
	$(CC) -o$(REMOTE_TARGET) $(REMOTE_OBJS) $(API_OBJS) $(SERIAL_OBJS) $(CFLAGS) $(NCURSES_FLAGS) $(PORTAUDIO_FLAGS)

%.o:%.cpp
	$(CC) $(REMOTE_SRCS) $(API_SRCS) $(SERIAL_SRCS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) ./$(TARGET) ./$(REMOTE_TARGET)
	$(RM) $(API_DIR)/*.o $(SERIAL_DIR)/*.o $(MAIN_DIR)/*.o $(REMOTE_DIR)/*.o
	$(RM) $(ASSETS_DIR)/*.dat
	$(RM) $(SERVER_ERROR_FILE) $(CLIENT_ERROR_FILE)