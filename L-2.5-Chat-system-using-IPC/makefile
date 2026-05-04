#########################################
# Homework4 - Multi-Client Chat System
#########################################

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -pthread -Iinclude
LDFLAGS := -pthread -lrt          # -lrt required for message queues

SRC_DIR := src
BIN_DIR := bin

SERVER  := $(BIN_DIR)/chat_server
CLIENT  := $(BIN_DIR)/chat_client

SRC     := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC))

# Default IPC backend (can be overridden)
IPC ?= sockets

#########################################
# IPC Backend Selection
#########################################
ifeq ($(IPC), sockets)
    CFLAGS += -DIPC_UNIX_SOCKETS
else ifeq ($(IPC), mq)
    CFLAGS += -DIPC_MESSAGE_QUEUE
else
    $(error Invalid IPC backend: choose IPC=sockets or IPC=mq)
endif

#########################################
# Targets
#########################################

.PHONY: all clean run_server run_client print_ipc

all: print_ipc $(SERVER) $(CLIENT)

print_ipc:
	@echo "Building with IPC backend: $(IPC)"

#########################################
# Build Executables
#########################################

$(SERVER): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

$(CLIENT): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

#########################################
# Build Object Files
#########################################

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

#########################################
# Convenience Targets
#########################################

run_server: $(SERVER)
	$(SERVER)

run_client: $(CLIENT)
	$(CLIENT)

clean:
	rm -rf $(BIN_DIR)/*.o $(SERVER) $(CLIENT)

#########################################
# End
#########################################

