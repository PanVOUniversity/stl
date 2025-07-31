CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -O2
TESTFLAGS = -std=c++20 -Wall -Wextra -pedantic -O2 -lgtest -lgtest_main -pthread
DOCFLAGS = -std=c++20 -Wall -Wextra -pedantic

# Директории
SRCDIR = src
INCDIR = include
TESTDIR = tests
BUILDDIR = build
DOCDIR = docs

# Исходные файлы
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
TEST_SOURCES = $(wildcard $(TESTDIR)/*.cpp)
TEST_OBJECTS = $(TEST_SOURCES:$(TESTDIR)/%.cpp=$(BUILDDIR)/%.o)

# Цели
TARGET = skip_list
TEST_TARGET = test_skip_list

# Основные цели
all: $(TARGET) tests docs

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# Тесты
tests: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CXX) $^ -o $@ $(TESTFLAGS)

$(BUILDDIR)/%.o: $(TESTDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(TESTFLAGS) -I$(INCDIR) -c $< -o $@

# Документация
docs: Doxyfile
	@mkdir -p $(DOCDIR)
	doxygen Doxyfile

# Очистка
clean:
	rm -rf $(BUILDDIR) $(TARGET) $(TEST_TARGET) $(DOCDIR)

# Установка зависимостей (для Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y libgtest-dev doxygen graphviz

# CI/CD
ci: install-deps all

# Форматирование кода
format:
	clang-format -i $(INCDIR)/* $(SRCDIR)/* $(TESTDIR)/*

.PHONY: all tests docs clean install-deps ci format 