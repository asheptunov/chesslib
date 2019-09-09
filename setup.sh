# --------------------------------
# >>>> CLEAN UP RESIDUAL DIRS <<<<
# --------------------------------
echo "getting ready..."

make clean

# object files (.o)
if [ -d chess/out ]; then
rm -rf chess/out
fi

# headers and libraries (.h, .a)
if [ -d chess/lib ]; then
rm -rf chess/lib
fi

# executables (.exe)
if [ -d chess/bin ]; then
rm -rf chess/bin
fi

# libraries
if [ -d googletest ]; then
rm -rf googletest
fi

# -----------------------------------------
# >>>> SET UP COMPILE-TIME DIRECTORIES <<<<
# -----------------------------------------

echo "setting up directories..."

# object files
mkdir chess/out
mkdir chess/out/src
mkdir chess/out/test

# headers and libraries
mkdir chess/lib
mkdir chess/lib/googletest
mkdir chess/lib/googletest/include
mkdir chess/lib/googletest/lib

# executables
mkdir chess/bin

# --------------------------------
# >>>> SET UP DEPENDENCY LIBS <<<<
# --------------------------------

echo "Setting up dependencies..."

# gtest
git clone https://github.com/google/googletest.git
cd googletest && cmake CMakeLists.txt && make && cd ..
cp googletest/lib/* chess/lib/googletest/lib/
cp -R googletest/googletest/include/* chess/lib/googletest/include/
rm -rf googletest

# ------------------------------------
# >>>> BUILD BINARIES FROM SOURCE <<<<
# ------------------------------------

echo "building..."

make all

echo "binaries in chess/bin\n"
