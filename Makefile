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
CFLAGS = -g -Wall -Wextra -std=c11 -D_XOPEN_SOURCE=700 -DCHESSLIB_QWORD_MOVE -fPIC
CPROD = -O3 -DCHESSLIB_PROD
CTEST = -g -O1

CXX = g++
CPPFLAGS = -isystem $(GTEST_HDR)
CXXFLAGS = -g -Wall -Wextra -pthread -std=c++11 -O0 -DCHESSLIB_QWORD_MOVE

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

UNIT_TESTS =bin/test/moveTest        \
			bin/test/boardTest       \
			bin/test/arraylistTest   \
			bin/test/movegenTest

SYSTEM_TESTS =  bin/test/perftTest \
				test/perftTest.py  \
				test/memTest.py

LIBA =  bin/lib/libchess.a
LIBSO = bin/lib/libchess.so
LIBDLL = bin/lib/libchess.dll

unittest: $(UNIT_TESTS)
	@echo `echo $^ | sed -r 's/\s/ $(GTEST_FLAGS) ; /g'` $(GTEST_FLAGS) | sh

systemtest: $(SYSTEM_TESTS)
	bin/test/perftTest ; $(PY) $(PYTEST_FLAGS) test.perftTest test.memTest

test: unittest systemtest

liba: $(LIBA)

libso: $(LIBSO)

libdll: $(LIBDLL)

all: libso unittest systemtest

.PHONY: all test clean release-clean release

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
	@mkdir -p bin/lib bin/test
	@mkdir -p $(GTEST_HDR) $(GTEST_LIB)
	@mkdir -p build/src/prod build/src/test build/test
	@mkdir log
	@echo "fetching dependencies..."
	@ROOTDIR=$(pwd)
	@git clone https://github.com/google/googletest.git
	@cd googletest && cmake CMakeLists.txt && make && cp lib/* $(GTEST_LIB)/ && cp -R googletest/include/* $(GTEST_HDR)/
	@cd $(ROOTDIR)
	@rm -rf googletest
	@echo "done"

release-clean:
	@rm -rf release

release-win:
	@make clean libdll
	@mkdir -p release/lib
	@cp bin/lib/libchess.dll release/lib/libchess.dll

release-unix:
	@make clean liba libso
	@mkdir -p release/lib
	@cp bin/lib/libchess.a release/lib/libchess.a
	@cp bin/lib/libchess.so release/lib/libchess.so

release:
	@mkdir -p release/include
	@cp include/* release/include/
	@cp src/pychess.py release/
	@printf "[libchess]\nposix_path = lib/libchess.so\nnt_path = lib\libchess.dll\n" > release/pychess.ini
	@cp LICENSE release/
	@cp README.md release/

release-zip:
	@zip -r release.zip release

release-tar-gz:
	@tar -czf release.tar.gz release

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

bin/test/moveTest: build/src/test/parseutils.o build/src/test/move.o build/src/test/algnot.o build/test/moveTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/test/boardTest: build/src/test/parseutils.o build/src/test/arraylist.o build/src/test/move.o build/src/test/algnot.o build/src/test/board.o build/src/test/movegen.o build/test/boardTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/test/arraylistTest: build/src/test/arraylist.o build/test/arraylistTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/test/movegenTest: build/src/test/parseutils.o build/src/test/arraylist.o build/src/test/move.o build/src/test/algnot.o build/src/test/board.o build/src/test/movegen.o build/test/movegenTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/test/perftTest: build/src/prod/parseutils.o build/src/prod/arraylist.o build/src/prod/move.o build/src/prod/algnot.o build/src/prod/board.o build/src/prod/movegen.o build/test/perftTest.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L $(GTEST_LIB) -lgtest_main -lpthread $^ -o $@

bin/lib/libchess.a: build/src/prod/parseutils.o build/src/prod/move.o build/src/prod/algnot.o build/src/prod/board.o build/src/prod/movegen.o
	$(AR) $(ARFLAGS) $@ $^

bin/lib/libchess.so: build/src/prod/parseutils.o build/src/prod/arraylist.o build/src/prod/move.o build/src/prod/algnot.o build/src/prod/board.o build/src/prod/movegen.o
	$(C) $(CFLAGS) $^ -shared -o $@

bin/lib/libchess.dll: build/src/prod/parseutils.o build/src/prod/arraylist.o build/src/prod/move.o build/src/prod/algnot.o build/src/prod/board.o build/src/prod/movegen.o
	$(C) $(CFLAGS) $^ -shared -o $@
