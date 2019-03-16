# Please tweak the following variable definitions as needed by your
# project, except GTEST_HEADERS, which you can use in your own targets
# but shouldn't modify.

# Points to the root of Google Test, relative to where this file is. (inner /googletest)
# Remember to tweak this if you move this file.
GTEST_DIR = ./googletest/googletest

# Points to the location of the Google Test libraries. (.a files)
GTEST_LIB_DIR = ./out/gtest

# Where to find user code. (root)
USER_DIR = .

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread -std=c++11

# Google Test libraries
GTEST_LIBS = out/gtest/libgtest.a out/gtest/libgtest_main.a

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = bin/testMove bin/testBoard bin/testMoveGen

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# House-keeping build targets.

all : $(GTEST_LIBS) $(TESTS)

clean :
	rm -f $(GTEST_LIBS) $(TESTS) bin/* out/game/*.o out/test/*.o out/gtest/*.o

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
out/gtest/gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc -o $@

out/gtest/gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc -o $@

out/gtest/libgtest.a : out/gtest/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

out/gtest/libgtest_main.a : out/gtest/gtest-all.o out/gtest/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.

USER_HEADERS = $(USER_DIR)/game/defs.h \
               $(USER_DIR)/game/parseutils.h \
               $(USER_DIR)/game/move.h \
               $(USER_DIR)/game/board.h

out/game/parseutils.o: $(USER_DIR)/game/parseutils.cpp $(USER_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/game/parseutils.cpp -o $@

out/game/move.o: $(USER_DIR)/game/move.cpp $(USER_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/game/move.cpp -o $@

out/game/board.o: $(USER_DIR)/game/board.cpp $(USER_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/game/board.cpp -o $@

out/game/movegen.o: $(USER_DIR)/game/movegen.cpp $(USER_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/game/movegen.cpp -o $@

out/test/testBoard.o: $(USER_DIR)/test/testBoard.cpp $(USER_HEADERS) $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/test/testBoard.cpp -o $@

out/test/testMove.o: $(USER_DIR)/test/testMove.cpp $(USER_HEADERS) $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/test/testMove.cpp -o $@

out/test/testMoveGen.o: $(USER_DIR)/test/testMoveGen.cpp $(USER_HEADERS) $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/test/testMoveGen.cpp -o $@

bin/testMove: out/game/parseutils.o out/game/move.o out/test/testMove.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L$(GTEST_LIB_DIR) -lgtest_main -lpthread $^ -o $@

bin/testBoard: out/game/parseutils.o out/game/move.o out/game/board.o out/test/testBoard.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L$(GTEST_LIB_DIR) -lgtest_main -lpthread $^ -o $@

bin/testMoveGen: out/game/parseutils.o out/game/move.o out/game/board.o out/game/movegen.o out/test/testMoveGen.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -L$(GTEST_LIB_DIR) -lgtest_main -lpthread $^ -o $@
