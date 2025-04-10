PROJECT_NAME = find_freq

BUILD_DIR = ./build
TARGET = $(BUILD_DIR)/$(PROJECT_NAME)

TARGET_FILES = src/main.c
TARGET_HEADERS =

CFLAGS = -Wall -Wextra -pedantic -std=c99
LDFLAGS = -lm

$(TARGET): $(TARGET_FILES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TARGET_FILES) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR):
	mkdir -pv $(BUILD_DIR)

%.o: %.c $(TARGET_HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(BUILD_DIR)
