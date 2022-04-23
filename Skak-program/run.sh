rm a.out
c++ *.cpp ./chess_lib/*.o ./PC_Chess_IO/*.o ./chess_algorithm/*.o -lwiringPi
./a.out