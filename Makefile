# -----------------
# >>>> MODULES <<<<
# -----------------
CHESS_ROOT = ./chess
CHESS_BIN = $(CHESS_ROOT)/bin
CHESS_HDR = $(CHESS_ROOT)/include
CHESS_LIB = $(CHESS_ROOT)/lib
CHESS_OBJ = $(CHESS_ROOT)/out
CHESS_TST = $(CHESS_ROOT)/test

# ----------------------
# >>>> DEPENDENCIES <<<<
# ----------------------
GTEST_ROOT = $(CHESS_LIB)/googletest
GTEST_HDR = $(GTEST_ROOT)/include
GTEST_LIB = $(GTEST_ROOT)/lib
GTEST_LIBS = $(GTEST_LIB)/libgtest.a      \
             $(GTEST_LIB)/libgtest_main.a \
             $(GTEST_LIB)/libgmock.a      \
             $(GTEST_LIB)/libgmock_main.a

# -----------------------------
# >>>> COMPILER PROPERTIES <<<<
# -----------------------------
CXX = g++
CPPFLAGE = -isystem $(GTEST_HDR)
CXXFLAGS = -g -Wall -Wextra -pthread -std=c++11

# -------------------------
# >>>> TARGET BINARIES <<<<
# -------------------------
TESTS = $(CHESS_BIN)/moveTest        \
        $(CHESS_BIN)/boardTest       \
        $(CHESS_BIN)/movegenTest     \

all: $(TESTS)

.PHONY: clean

clean:
	rm -f bin/* $(CHESS_OBJ)/src/*.o $(CHESS_OBJ)/test/*.o

# -----------------
# >>>> RECIPES <<<<
# -----------------

$(CHESS_OBJ)/src/parseutils.o: $(CHESS_ROOT)/src/parseutils.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -c $(CHESS_ROOT)/src/parseutils.cpp -o $@

$(CHESS_OBJ)/src/move.o: $(CHESS_ROOT)/src/move.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -c $(CHESS_ROOT)/src/move.cpp -o $@

$(CHESS_OBJ)/src/board.o: $(CHESS_ROOT)/src/board.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -c $(CHESS_ROOT)/src/board.cpp -o $@

$(CHESS_OBJ)/src/movegen.o: $(CHESS_ROOT)/src/movegen.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -c $(CHESS_ROOT)/src/movegen.cpp -o $@

$(CHESS_OBJ)/test/boardTest.o: $(CHESS_ROOT)/test/boardTest.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -I $(GTEST_HDR) -c $(CHESS_ROOT)/test/boardTest.cpp -o $@

$(CHESS_OBJ)/test/moveTest.o: $(CHESS_ROOT)/test/moveTest.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -I $(GTEST_HDR) -c $(CHESS_ROOT)/test/moveTest.cpp -o $@

$(CHESS_OBJ)/test/movegenTest.o: $(CHESS_ROOT)/test/movegenTest.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -I $(GTEST_HDR) -c $(CHESS_ROOT)/test/movegenTest.cpp -o $@

$(CHESS_BIN)/moveTest: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/test/moveTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

$(CHESS_BIN)/boardTest: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/board.o $(CHESS_OBJ)/test/boardTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

$(CHESS_BIN)/movegenTest: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/board.o $(CHESS_OBJ)/src/movegen.o $(CHESS_OBJ)/test/movegenTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

