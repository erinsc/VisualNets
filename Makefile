SRC_DIR := src
OBJ_DIR := obj

CC := g++

CFLAGS :=  -std=c++17 -O2 -MMD -MP -W -Wall -Wextra -I $(SRC_DIR)
LFLAGS :=  -l raylib

TARGET := VisualNets
SRCS   := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS   := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DEPS   := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.d, $(SRCS))

.PHONY: run clean compile recompile

run: $(TARGET)
	clear
	./$(TARGET)

-include $(DEPS)

compile: $(TARGET)

recompile: clean compile

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)
