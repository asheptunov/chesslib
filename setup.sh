echo "building directories..."
make clean
rm -rf bin
rm -rf out
mkdir bin
mkdir out
mkdir out/gtest
mkdir out/game
mkdir out/test
echo "done."
echo "building..."
make
echo "done."
echo -e "runners at\n"
echo -e "\t./bin/testMove"
echo -e "\t./bin/testBoard"
echo -e "\t./bin/testMoveGen\n"