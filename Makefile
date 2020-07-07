# -----------------
# >>>> MODULES <<<<
# -----------------
CHESS_ROOT = $(strip $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST)))))
CHESS_BIN = $(CHESS_ROOT)/bin
CHESS_HDR = $(CHESS_ROOT)/include
CHESS_SRC = $(CHESS_ROOT)/src
CHESS_LIB = $(CHESS_ROOT)/lib
CHESS_OBJ = $(CHESS_ROOT)/build
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

# ------------------------
# >>>> GCC PROPERTIES <<<<
# ------------------------
C = gcc
CFLAGS = -g -Wall -Wextra -std=c99 -D_XOPEN_SOURCE=700 -fPIC -O3

CXX = g++
CPPFLAGS = -isystem $(GTEST_HDR)
CXXFLAGS = -g -Wall -Wextra -pthread -std=c++11 -O0

# --------------------------
# >>>> LEXER PROPERTIES <<<<
# --------------------------
LEX = flex
LEXFLAGS = --align --fast # --verbose --debug

# ---------------------------
# >>>> PYTHON PROPERTIES <<<<
# ---------------------------
PY = python3
PYTEST_FLAGS = -m unittest -v

# --------------------------
# >>>> GTEST PROPERTIES <<<<
# --------------------------
GTEST_FLAGS = --gtest_brief=1

# -------------------------
# >>>> TARGET BINARIES <<<<
# -------------------------
UNIT_TESTS =$(CHESS_BIN)/moveTest        \
			$(CHESS_BIN)/boardTest       \
			$(CHESS_BIN)/arraylistTest   \
			$(CHESS_BIN)/movegenTest

SYSTEM_TESTS =  $(CHESS_BIN)/perftTest \
				$(CHESS_TST)/perftTest.py \
				$(CHESS_TST)/memTest.py

LIBA =  $(CHESS_BIN)/libchess.a
LIBSO = $(CHESS_BIN)/libchess.so
LIBDLL = $(CHESS_BIN)/libchess.dll

init:
	@echo "making directories"
	@if [ -d "$(CHESS_BIN)" ]; then rm -Rf $(CHESS_BIN); fi
	@if [ -d "$(CHESS_LIB)" ]; then rm -Rf $(CHESS_LIB); fi
	@if [ -d "$(CHESS_OBJ)" ]; then rm -Rf $(CHESS_OBJ); fi
	@if [ -d "googletest" ]; then rm -Rf googletest; fi
	@if [ -d "log" ]; then rm -Rf log; fi
	@make clean ; mkdir $(CHESS_BIN) ; mkdir $(CHESS_LIB) $(GTEST_ROOT) $(GTEST_HDR) $(GTEST_LIB) ; mkdir $(CHESS_OBJ) $(CHESS_OBJ)/src $(CHESS_OBJ)/test ; mkdir log
	@echo "fetching dependencies"
	@ROOTDIR=$(pwd)
	@git clone https://github.com/google/googletest.git
	@cd googletest && cmake CMakeLists.txt && make && cp lib/* $(GTEST_LIB)/ && cp -R googletest/include/* $(GTEST_HDR)/
	@cd $(ROOTDIR)
	@rm -rf googletest
	@echo "building"
	@make all
	@echo "done"

unittest: $(UNIT_TESTS)
	@echo `echo $^ | sed -r 's/\s/ $(GTEST_FLAGS) ; /g'` $(GTEST_FLAGS) | sh

systemtest: $(SYSTEM_TESTS)
	$(CHESS_BIN)/perftTest ; $(PY) $(PYTEST_FLAGS) test.perftTest test.memTest

test: unittest systemtest

liba: $(LIBA)

libso: $(LIBSO)

libdll: $(LIBDLL)

all: $(LIBSO) $(UNIT_TESTS) $(SYSTEM_TESTS)

.PHONY: clean

clean:
	find $(CHESS_BIN) -type f -name '*.a' -delete -o -name '*.so' -delete -o -name '*Test' -delete ; \
	find $(CHESS_OBJ) -type f -name '*.c' -delete -o -name '*.o' -delete

# -------------------
# >>>> C RECIPES <<<<
# -------------------
$(CHESS_OBJ)/src/algnot.c: $(CHESS_SRC)/algnot.flex
	$(LEX) $(LEXFLAGS) -o $@ $<

# ------------------------
# >>>> PYTHON RECIPES <<<<
# ------------------------

$(CHESS_TST)/perftTest.py: $(LIBSO)

$(CHESS_TST)/memTest.py: $(LIBSO) $(UNIT_TESTS) $(SYSTEM_TESTS)

# ------------------------
# >>>> OBJECT RECIPES <<<<
# ------------------------
$(CHESS_OBJ)/src/algnot.o: $(CHESS_OBJ)/src/algnot.c $(CHESS_HDR)
	$(C) $(CFLAGS) -I $(CHESS_HDR) -c -o $@ $<

$(CHESS_OBJ)/src/parseutils.o: $(CHESS_SRC)/parseutils.c $(CHESS_HDR)
	$(C) $(CFLAGS) -I $(CHESS_HDR) -c -o $@ $<

$(CHESS_OBJ)/src/move.o: $(CHESS_SRC)/move.c $(CHESS_HDR)
	$(C) $(CFLAGS) -I $(CHESS_HDR) -c -o $@ $<

$(CHESS_OBJ)/src/board.o: $(CHESS_SRC)/board.c $(CHESS_HDR)
	$(C) $(CFLAGS) -I $(CHESS_HDR) -c -o $@ $<

$(CHESS_OBJ)/src/arraylist.o: $(CHESS_SRC)/arraylist.c $(CHESS_HDR)
	$(C) $(CFLAGS) -I $(CHESS_HDR) -c -o $@ $<

$(CHESS_OBJ)/src/movegen.o: $(CHESS_SRC)/movegen.c $(CHESS_HDR)
	$(C) $(CFLAGS) -I $(CHESS_HDR) -c -o $@ $<

$(CHESS_OBJ)/test/boardTest.o: $(CHESS_TST)/boardTest.cpp $(CHESS_HDR)
	$(CXX) $(CXXFLAGS) -I $(CHESS_HDR) -I $(GTEST_HDR) -c -o $@ $<

$(CHESS_OBJ)/test/moveTest.o: $(CHESS_TST)/moveTest.cpp $(CHESS_HDR)
	$(CXX) $(CXXFLAGS) -I $(CHESS_HDR) -I $(GTEST_HDR) -c -o $@ $<

$(CHESS_OBJ)/test/arraylistTest.o: $(CHESS_TST)/arraylistTest.cpp $(CHESS_HDR)
	$(CXX) $(CXXFLAGS) -I $(CHESS_HDR) -I $(GTEST_HDR) -c -o $@ $<

$(CHESS_OBJ)/test/movegenTest.o: $(CHESS_TST)/movegenTest.cpp $(CHESS_HDR)
	$(CXX) $(CXXFLAGS) -I $(CHESS_HDR) -I $(GTEST_HDR) -c -o $@ $<

$(CHESS_OBJ)/test/perftTest.o: $(CHESS_TST)/perftTest.cpp $(CHESS_HDR)
	$(CXX) $(CXXFLAGS) -I $(CHESS_HDR) -I $(GTEST_HDR) -c -o $@ $<

# ------------------------
# >>>> BINARY RECIPES <<<<
# ------------------------
$(CHESS_BIN)/moveTest: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/algnot.o $(CHESS_OBJ)/test/moveTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

$(CHESS_BIN)/boardTest: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/arraylist.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/algnot.o $(CHESS_OBJ)/src/board.o $(CHESS_OBJ)/src/movegen.o $(CHESS_OBJ)/test/boardTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

$(CHESS_BIN)/arraylistTest: $(CHESS_OBJ)/src/arraylist.o $(CHESS_OBJ)/test/arraylistTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

$(CHESS_BIN)/movegenTest: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/arraylist.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/algnot.o $(CHESS_OBJ)/src/board.o $(CHESS_OBJ)/src/movegen.o $(CHESS_OBJ)/test/movegenTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

$(CHESS_BIN)/perftTest: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/arraylist.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/algnot.o $(CHESS_OBJ)/src/board.o $(CHESS_OBJ)/src/movegen.o $(CHESS_OBJ)/test/perftTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

$(CHESS_BIN)/libchess.a: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/algnot.o $(CHESS_OBJ)/src/board.o $(CHESS_OBJ)/src/movegen.o
	$(AR) $(ARFLAGS) $@ $^

$(CHESS_BIN)/libchess.so: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/arraylist.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/algnot.o $(CHESS_OBJ)/src/board.o $(CHESS_OBJ)/src/movegen.o
	$(C) $(CFLAGS) $^ -shared -o $@

$(CHESS_BIN)/libchess.dll: $(CHESS_OBJ)/src/parseutils.o $(CHESS_OBJ)/src/arraylist.o $(CHESS_OBJ)/src/move.o $(CHESS_OBJ)/src/algnot.o $(CHESS_OBJ)/src/board.o $(CHESS_OBJ)/src/movegen.o
	$(C) $(CFLAGS) $^ -shared -o $@
