# ----------------------
# >>>> DEPENDENCIES <<<<
# ----------------------

GTEST_HDR = lib/googletest/include
GTEST_LIB = lib/googletest/lib
GTEST_LIBS = $(GTEST_LIB)/libgtest.a      \
             $(GTEST_LIB)/libgtest_main.a \
             $(GTEST_LIB)/libgmock.a      \
             $(GTEST_LIB)/libgmock_main.a

# ------------------------
# >>>> GCC PROPERTIES <<<<
# ------------------------

C = gcc
CFLAGS = -g -Wall -Wextra -std=c11 -D_XOPEN_SOURCE=700 -fPIC -O3  # -DCHESSLIB_PRODUCTION

CXX = g++
CPPFLAGS = -isystem $(GTEST_HDR)
CXXFLAGS = -g -Wall -Wextra -pthread -std=c++11 -O0

# --------------------------
# >>>> LEXER PROPERTIES <<<<
# --------------------------

LEX = flex
LEXFLAGS = --align --fast

# ---------------------------
# >>>> PYTHON PROPERTIES <<<<
# ---------------------------

PY = python3
PYTEST_FLAGS = -m unittest -v

# --------------------------
# >>>> GTEST PROPERTIES <<<<
# --------------------------

GTEST_FLAGS = --gtest_brief=1

# -----------------
# >>>> TARGETS <<<<
# -----------------

UNIT_TESTS =bin/moveTest        \
			bin/boardTest       \
			bin/arraylistTest   \
			bin/movegenTest

SYSTEM_TESTS =  bin/perftTest     \
				test/perftTest.py \
				test/memTest.py

LIBA =  bin/libchess.a
LIBSO = bin/libchess.so
LIBDLL = bin/libchess.dll

unittest: $(UNIT_TESTS)
	@echo `echo $^ | sed -r 's/\s/ $(GTEST_FLAGS) ; /g'` $(GTEST_FLAGS) | sh

systemtest: $(SYSTEM_TESTS)
	bin/perftTest ; $(PY) $(PYTEST_FLAGS) test.perftTest test.memTest

test: unittest systemtest

liba: $(LIBA)

libso: $(LIBSO)

libdll: $(LIBDLL)

all: libso unittest systemtest

.PHONY: clean

clean:
	find bin   -type f -name '*.a' -delete -o -name '*.so' -delete -o -name '*Test' -delete ; \
	find build -type f -name '*.c' -delete -o -name '*.o' -delete

init:
	@echo "making directories..."
	@if [ -d "bin" ]; then rm -Rf bin; fi
	@if [ -d "lib" ]; then rm -Rf lib; fi
	@if [ -d "build" ]; then rm -Rf build; fi
	@if [ -d "googletest" ]; then rm -Rf googletest; fi
	@if [ -d "log" ]; then rm -Rf log; fi
	@make clean
	@mkdir bin
	@mkdir lib
	@mkdir lib/googletest $(GTEST_HDR) $(GTEST_LIB)
	@mkdir build build/src build/test
	@mkdir log
	@echo "fetching dependencies..."
	@ROOTDIR=$(pwd)
	@git clone https://github.com/google/googletest.git
	@cd googletest && cmake CMakeLists.txt && make && cp lib/* $(GTEST_LIB)/ && cp -R googletest/include/* $(GTEST_HDR)/
	@cd $(ROOTDIR)
	@rm -rf googletest
	@echo "done"

# -------------------
# >>>> C RECIPES <<<<
# -------------------

build/src/algnot.c: src/algnot.flex
	$(LEX) $(LEXFLAGS) -o $@ $<

# ------------------------
# >>>> PYTHON RECIPES <<<<
# ------------------------

test/perftTest.py: $(LIBSO)

test/memTest.py: $(LIBSO) $(UNIT_TESTS) $(SYSTEM_TESTS)

# ------------------------
# >>>> OBJECT RECIPES <<<<
# ------------------------

build/src/algnot.o: build/src/algnot.c include
	$(C) $(CFLAGS) -I include -c -o $@ $<

build/src/parseutils.o: src/parseutils.c include
	$(C) $(CFLAGS) -I include -c -o $@ $<

build/src/move.o: src/move.c include
	$(C) $(CFLAGS) -I include -c -o $@ $<

build/src/board.o: src/board.c include
	$(C) $(CFLAGS) -I include -c -o $@ $<

build/src/arraylist.o: src/arraylist.c include
	$(C) $(CFLAGS) -I include -c -o $@ $<

build/src/movegen.o: src/movegen.c include
	$(C) $(CFLAGS) -I include -c -o $@ $<

build/test/boardTest.o: test/boardTest.cpp include
	$(CXX) $(CXXFLAGS) -I include -I $(GTEST_HDR) -c -o $@ $<

build/test/moveTest.o: test/moveTest.cpp include
	$(CXX) $(CXXFLAGS) -I include -I $(GTEST_HDR) -c -o $@ $<

build/test/arraylistTest.o: test/arraylistTest.cpp include
	$(CXX) $(CXXFLAGS) -I include -I $(GTEST_HDR) -c -o $@ $<

build/test/movegenTest.o: test/movegenTest.cpp include
	$(CXX) $(CXXFLAGS) -I include -I $(GTEST_HDR) -c -o $@ $<

build/test/perftTest.o: test/perftTest.cpp include
	$(CXX) $(CXXFLAGS) -I include -I $(GTEST_HDR) -c -o $@ $<

# ------------------------
# >>>> BINARY RECIPES <<<<
# ------------------------

bin/moveTest: build/src/parseutils.o build/src/move.o build/src/algnot.o build/test/moveTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/boardTest: build/src/parseutils.o build/src/arraylist.o build/src/move.o build/src/algnot.o build/src/board.o build/src/movegen.o build/test/boardTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/arraylistTest: build/src/arraylist.o build/test/arraylistTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/movegenTest: build/src/parseutils.o build/src/arraylist.o build/src/move.o build/src/algnot.o build/src/board.o build/src/movegen.o build/test/movegenTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/perftTest: build/src/parseutils.o build/src/arraylist.o build/src/move.o build/src/algnot.o build/src/board.o build/src/movegen.o build/test/perftTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/libchess.a: build/src/parseutils.o build/src/move.o build/src/algnot.o build/src/board.o build/src/movegen.o
	$(AR) $(ARFLAGS) $@ $^

bin/libchess.so: build/src/parseutils.o build/src/arraylist.o build/src/move.o build/src/algnot.o build/src/board.o build/src/movegen.o
	$(C) $(CFLAGS) $^ -shared -o $@

bin/libchess.dll: build/src/parseutils.o build/src/arraylist.o build/src/move.o build/src/algnot.o build/src/board.o build/src/movegen.o
	$(C) $(CFLAGS) $^ -shared -o $@
