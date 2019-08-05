# paths
BUILD_PATH		::= build
SRC_PATH		::= src
INSTALL_PATH	::= /usr/bin

# targets
TARGET_SDL		::= $(BUILD_PATH)/crayon_sdl
TARGET_QT		::= $(BUILD_PATH)/crayon_qt

# programs
COMPILER		::= g++ -Wall -Og -g -I$(SRC_PATH)
DEBUGGER		::= gdb

# flags
QT_FLAGS        ::= $(shell pkg-config --cflags --libs Qt5Widgets) -fPIC



all: sdl qt

sdl: $(TARGET_SDL)

qt: $(TARGET_QT)

$(TARGET_QT): $(BUILD_PATH)/qt.o $(BUILD_PATH)/canvas.o $(BUILD_PATH)/crayon.o
	$(COMPILER) $(QT_FLAGS) \
		$(BUILD_PATH)/qt.o $(BUILD_PATH)/canvas.o $(BUILD_PATH)/crayon.o -o $(TARGET_QT)

$(TARGET_SDL): $(BUILD_PATH)/sdl.o $(BUILD_PATH)/canvas.o $(BUILD_PATH)/crayon.o
	$(COMPILER) -lSDL2 \
		$(BUILD_PATH)/sdl.o $(BUILD_PATH)/canvas.o $(BUILD_PATH)/crayon.o -o $(TARGET_SDL)

$(BUILD_PATH)/qt.o: $(BUILD_PATH) $(SRC_PATH)/qt.cpp
	$(COMPILER) $(QT_FLAGS) -c $(SRC_PATH)/qt.cpp -o $(BUILD_PATH)/qt.o

$(BUILD_PATH)/sdl.o: $(BUILD_PATH) $(SRC_PATH)/sdl.cpp
	$(COMPILER) -c $(SRC_PATH)/sdl.cpp -o $(BUILD_PATH)/sdl.o

$(BUILD_PATH)/canvas.o: $(BUILD_PATH) $(SRC_PATH)/stack.h $(SRC_PATH)/canvas.h $(SRC_PATH)/canvas.cpp
	$(COMPILER) -c $(SRC_PATH)/canvas.cpp -o $(BUILD_PATH)/canvas.o

$(BUILD_PATH)/crayon.o: $(BUILD_PATH) $(SRC_PATH)/wax.h $(SRC_PATH)/vec.h $(SRC_PATH)/crayon.h $(SRC_PATH)/crayon.cpp
	$(COMPILER) -c $(SRC_PATH)/crayon.cpp -o $(BUILD_PATH)/crayon.o

$(BUILD_PATH):
	mkdir $(BUILD_PATH)

.PHONY:
clean: $(BUILD_PATH)
	rm -rf $(BUILD_PATH)

.PHONY:
run: $(TARGET_QT)
	./$(TARGET_QT)

.PHONY:
test: $(TARGET_SDL)
	./$(TARGET_SDL)

.PHONY:
debug: $(TARGET_SDL)
	$(DEBUGGER) $(TARGET_SDL)

.PHONY:
install: $(TARGET_QT) $(TARGET_SDL)
	cp $(TARGET_QT) $(INSTALL_PATH)
	cp $(TARGET_SDL) $(INSTALL_PATH)
