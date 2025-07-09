# Compiler settings
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c11
CXXFLAGS = -Wall -Wextra -std=c++17

# Linker flags
LDFLAGS = -lprotobuf-c
TEST_LDFLAGS =

# Coverage flags
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
COVERAGE_LDFLAGS = -fprofile-arcs -ftest-coverage

# Source files
SRC = config.c main.c receiver.c sender.c filechunk.pb-c.c
OBJ = $(patsubst %.c,obj/%.o,$(SRC))

TEST_SRC = $(wildcard tests/*.cpp)
TEST_EXEC = $(patsubst tests/%.cpp,exec/%_test,$(TEST_SRC))
TEST_OBJ = $(patsubst tests/%.cpp,obj/tests/%.o,$(TEST_SRC))

# Coverage source and executables
COV_OBJ = $(patsubst %.c,obj_cov/%.o,$(SRC))
COV_TEST_OBJ = $(patsubst tests/%.cpp,obj_cov/tests/%.o,$(TEST_SRC))
COV_TEST_EXEC = $(patsubst tests/%.cpp,exec_cov/%_test,$(TEST_SRC))

# Directories
EXEC_DIR = exec
EXEC = $(EXEC_DIR)/echo-ft
OBJ_DIR = obj
TEST_OBJ_DIR = obj/tests
COV_OBJ_DIR = obj_cov
COV_TEST_OBJ_DIR = obj_cov/tests
EXEC_COV_DIR = exec_cov

.PHONY: all clean test coverage coverage_build coverage_run coverage_report prepare

all: $(EXEC) test

# Directory creation
prepare:
	mkdir -p $(EXEC_DIR) $(OBJ_DIR) $(TEST_OBJ_DIR) $(COV_OBJ_DIR) $(COV_TEST_OBJ_DIR) $(EXEC_COV_DIR)

# Normal build rules

# Main executable
$(EXEC): $(OBJ) | prepare
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile C sources to obj/
obj/%.o: %.c | prepare
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test cpp sources to obj/tests/
obj/tests/%.o: tests/%.cpp | prepare
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build each test executable (link with config.o)
exec/%_test: obj/tests/%.o obj/config.o | prepare
	$(CXX) $(CXXFLAGS) -o $@ $^ $(TEST_LDFLAGS)

# Run all tests
test: $(TEST_EXEC)
	@for test_exec in $(TEST_EXEC); do \
		echo "Running $$test_exec..."; \
		./$$test_exec || exit 1; \
	done

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(EXEC_DIR) $(COV_OBJ_DIR) $(EXEC_COV_DIR) *.gcda *.gcno *.gcov coverage.info coverage_html

################################################################################
# Coverage build rules
################################################################################

# Compile with coverage flags
obj_cov/%.o: %.c | prepare
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -c $< -o $@

obj_cov/tests/%.o: tests/%.cpp | prepare
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c $< -o $@

# Link coverage-enabled test executables
exec_cov/%_test: obj_cov/tests/%.o obj_cov/config.o | prepare
	$(CXX) $(CXXFLAGS) $(COVERAGE_LDFLAGS) -o $@ $^ $(TEST_LDFLAGS) -lgcov

# Coverage build - compile all sources with coverage
coverage_build: $(COV_OBJ) $(COV_TEST_OBJ) $(COV_TEST_EXEC)

# Run tests compiled with coverage
coverage_run: coverage_build
	@for test_exec in $(COV_TEST_EXEC); do \
		echo "Running $$test_exec for coverage..."; \
		./$$test_exec || exit 1; \
	done

# Generate coverage report with lcov & genhtml
coverage_report: coverage_run
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' '*/libs/catch.hpp' --output-file coverage.info
	genhtml coverage.info --output-directory coverage_html
	@echo "Coverage report generated at ./coverage_html/index.html"

# Shortcut to build, run tests, and generate coverage report
coverage: coverage_report

