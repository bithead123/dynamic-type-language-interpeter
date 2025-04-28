BUILD_DIR = build
SRC_DIR = src
COMPILER = gcc
CFLAGS = -Wall
TARGET = main

# Список всех .c файлов в SRC_DIR и подпапках
SOURCES = $(shell find $(SRC_DIR) -name "*.c")
# Преобразуем пути к .c файлам в пути к .o файлам
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

# Правило для сборки основной цели
$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(COMPILER) -o $@ $^

# Правило для компиляции исходников в объектные файлы
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)  # Создаем директорию, если её нет
	$(COMPILER) $(CFLAGS) -c $< -o $@

run:
	$(BUILD_DIR)/$(TARGET)

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*.o build/foo