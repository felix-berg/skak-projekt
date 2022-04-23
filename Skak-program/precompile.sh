shopt -s globstar

cd chess_algorithm
g++ -c **/*.hpp
g++ -c **/*.cpp

cd ../chess_lib
g++ -c **/*.hpp
g++ -c **/*.cpp

cd ../PC_Chess_IO
g++ -c **/*.hpp
g++ -c **/*.cpp