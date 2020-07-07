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
CFLAGS = -g -Wall -Wextra -std=c11 -D_XOPEN_SOURCE=700 -fPIC
CPROD = -O3 -DCHESSLIB_PROD
CTEST = -g -O1

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
	find bin   -type f -name '*.a' -delete -o -name '*.so' -delete -o -name '*.dll' -delete -o -name '*Test' -delete ; \
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
	@mkdir build build/src build/src/prod build/src/test build/test
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

build/src/prod/algnot.c: src/algnot.flex
	$(LEX) $(LEXFLAGS) -o $@ $<
build/src/test/algnot.c: src/algnot.flex
	$(LEX) $(LEXFLAGS) -o $@ $<

# ------------------------
# >>>> PYTHON RECIPES <<<<
# ------------------------

test/perftTest.py: $(LIBSO)

test/memTest.py: $(LIBSO) $(UNIT_TESTS) $(SYSTEM_TESTS)

# ------------------------
# >>>> OBJECT RECIPES <<<<
# ------------------------

build/src/prod/algnot.o: build/src/prod/algnot.c include
	$(C) $(CFLAGS) $(CPROD) -I include -c -o $@ $<
build/src/test/algnot.o: build/src/test/algnot.c include
	$(C) $(CFLAGS) $(CTEST) -I include -c -o $@ $<

build/src/prod/parseutils.o: src/parseutils.c include
	$(C) $(CFLAGS) $(CPROD) -I include -c -o $@ $<
build/src/test/parseutils.o: src/parseutils.c include
	$(C) $(CFLAGS) $(CTEST) -I include -c -o $@ $<

build/src/prod/move.o: src/move.c include
	$(C) $(CFLAGS) $(CPROD) -I include -c -o $@ $<
build/src/test/move.o: src/move.c include
	$(C) $(CFLAGS) $(CTEST) -I include -c -o $@ $<

build/src/prod/board.o: src/board.c include
	$(C) $(CFLAGS) $(CPROD) -I include -c -o $@ $<
build/src/test/board.o: src/board.c include
	$(C) $(CFLAGS) $(CTEST) -I include -c -o $@ $<

build/src/prod/arraylist.o: src/arraylist.c include
	$(C) $(CFLAGS) $(CPROD) -I include -c -o $@ $<
build/src/test/arraylist.o: src/arraylist.c include
	$(C) $(CFLAGS) $(CTEST) -I include -c -o $@ $<

build/src/prod/movegen.o: src/movegen.c include
	$(C) $(CFLAGS) $(CPROD) -I include -c -o $@ $<
build/src/test/movegen.o: src/movegen.c include
	$(C) $(CFLAGS) $(CTEST) -I include -c -o $@ $<

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

bin/moveTest: build/src/test/parseutils.o build/src/test/move.o build/src/test/algnot.o build/test/moveTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/boardTest: build/src/test/parseutils.o build/src/test/arraylist.o build/src/test/move.o build/src/test/algnot.o build/src/test/board.o build/src/test/movegen.o build/test/boardTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/arraylistTest: build/src/test/arraylist.o build/test/arraylistTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/movegenTest: build/src/test/parseutils.o build/src/test/arraylist.o build/src/test/move.o build/src/test/algnot.o build/src/test/board.o build/src/test/movegen.o build/test/movegenTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/perftTest: build/src/test/parseutils.o build/src/test/arraylist.o build/src/test/move.o build/src/test/algnot.o build/src/test/board.o build/src/test/movegen.o build/test/perftTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/libchess.a: build/src/prod/parseutils.o build/src/prod/move.o build/src/prod/algnot.o build/src/prod/board.o build/src/prod/movegen.o
	$(AR) $(ARFLAGS) $@ $^

bin/libchess.so: build/src/prod/parseutils.o build/src/prod/arraylist.o build/src/prod/move.o build/src/prod/algnot.o build/src/prod/board.o build/src/prod/movegen.o
	$(C) $(CFLAGS) $^ -shared -o $@

bin/libchess.dll: build/src/prod/parseutils.o build/src/prod/arraylist.o build/src/prod/move.o build/src/prod/algnot.o build/src/prod/board.o build/src/prod/movegen.o
	$(C) $(CFLAGS) $^ -shared -o $@
