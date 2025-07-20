CFLAGS :=  -std=c++17 -O2 -MMD -MP -W -Wall -Wextra -Wpedantic
LFLAGS :=  -l raylib

TARGET := VisualNets
SRCS   := $(wildcard src/*.cpp)
OBJS   := $(patsubst src/%.cpp,obj/%.o,$(SRCS))
DEPS   := $(patsubst src/%.cpp,obj/%.d,$(SRCS))

.PHONY: run clean compile recompile

run: $(TARGET)
	clear
	./$(TARGET)

-include $(DEPS)

compile: $(TARGET)

recompile: clean compile

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	g++ $(OBJS) -o $@ $(LFLAGS)

obj/%.o: src/%.cpp
	@mkdir -p obj
	g++ -c $< -o $@ $(CFLAGS)
