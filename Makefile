# paths
BUILD_PATH		::= build
SRC_PATH		::= src
INSTALL_PATH	::= /usr/bin

# targets
TARGET			::= $(BUILD_PATH)/crayon

# programs
COMPILER		::= g++ -Wall -Og -I$(SRC_PATH)
DEBUGGER		::= gdb



all: $(TARGET)

$(TARGET): $(BUILD_PATH)/main.o
	$(COMPILER) -lsfml-graphics -lsfml-window -lsfml-system $(BUILD_PATH)/main.o -o $(TARGET)

$(BUILD_PATH)/main.o: $(BUILD_PATH) $(SRC_PATH)/main.cpp
	$(COMPILER) -c $(SRC_PATH)/main.cpp -o $(BUILD_PATH)/main.o

$(BUILD_PATH):
	mkdir $(BUILD_PATH)

.PHONY:
clean: $(BUILD_PATH)
	rm -rf $(BUILD_PATH)

.PHONY:
run: $(TARGET)
	./$(TARGET)

.PHONY:
debug: $(TARGET)
	$(DEBUGGER) $(TARGET)
