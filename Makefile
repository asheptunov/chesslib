# -----------------
# >>>> MODULES <<<<
# -----------------
CHESS_ROOT = ./chess
CHESS_BIN = $(CHESS_ROOT)/bin
CHESS_HDR = $(CHESS_ROOT)/include
CHESS_SRC = $(CHESS_ROOT)/src
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
CPPFLAGS = -isystem $(GTEST_HDR)
CXXFLAGS = -g -Wall -Wextra -pthread -std=c++11 -O3

# --------------------------
# >>>> LEXER PROPERTIES <<<<
# --------------------------
LEX = flex
LEXFLAGS = --align --fast --verbose  # --debug

# -----------------------------
# >>>> ARCHIVER PROPERTIES <<<<
# -----------------------------

AR = ar
ARFLAGS = -rv

# -------------------------
# >>>> TARGET BINARIES <<<<
# -------------------------
TESTS = $(CHESS_BIN)/moveTest        \
        $(CHESS_BIN)/boardTest       \
        $(CHESS_BIN)/movegenTest     \

LIBS = $(CHESS_BIN)/libchess.a

all: $(TESTS) $(LIBS)

.PHONY: clean

clean:
	rm -f bin/* $(CHESS_OBJ)/src/*.o $(CHESS_OBJ)/src/*.c $(CHESS_OBJ)/test/*.o

# -------------------
# >>>> C RECIPES <<<<
# -------------------

$(CHESS_OBJ)/src/algnot.c: $(CHESS_SRC)/algnot.flex
	$(LEX) $(LEXFLAGS) -o $@ $<

# ------------------------
# >>>> OBJECT RECIPES <<<<
# ------------------------

$(CHESS_OBJ)/src/algnot.o: $(CHESS_OBJ)/src/algnot.c $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -c -o $@ $<

$(CHESS_OBJ)/src/parseutils.o: $(CHESS_SRC)/parseutils.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -c -o $@ $<

$(CHESS_OBJ)/src/move.o: $(CHESS_SRC)/move.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -c -o $@ $<

$(CHESS_OBJ)/src/board.o: $(CHESS_SRC)/board.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -c -o $@ $<

$(CHESS_OBJ)/src/movegen.o: $(CHESS_SRC)/movegen.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -c -o $@ $<

$(CHESS_OBJ)/test/boardTest.o: $(CHESS_TST)/boardTest.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -I $(GTEST_HDR) -c -o $@ $<

$(CHESS_OBJ)/test/moveTest.o: $(CHESS_TST)/moveTest.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -I $(GTEST_HDR) -c -o $@ $<

$(CHESS_OBJ)/test/movegenTest.o: $(CHESS_TST)/movegenTest.cpp $(CHESS_HDR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(CHESS_HDR) -I $(GTEST_HDR) -c -o $@ $<

# ------------------------
# >>>> BINARY RECIPES <<<<
# ------------------------

$(CHESS_BIN)/moveTest: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/algnot.o $(CHESS_OBJ)/test/moveTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

$(CHESS_BIN)/boardTest: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/algnot.o $(CHESS_OBJ)/src/board.o $(CHESS_OBJ)/test/boardTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

$(CHESS_BIN)/movegenTest: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/algnot.o $(CHESS_OBJ)/src/board.o $(CHESS_OBJ)/src/movegen.o $(CHESS_OBJ)/test/movegenTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

$(CHESS_BIN)/libchess.a: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/algnot.o $(CHESS_OBJ)/src/board.o $(CHESS_OBJ)/src/movegen.o
	$(AR) $(ARFLAGS) $@ $^
