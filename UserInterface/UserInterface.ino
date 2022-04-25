#include "LCD_screen.hpp"
#include "Matrix_screen.hpp"
#include "inputs.hpp"
#include "stepper_communication.hpp"
#include "algorithm_communication.hpp"

// user interface
// output
LCD_screen lcd { };
Matrix_screen mtx { 13, 12, 11 };

// input
Buttons buttons { 7, 6 };

enum ChessGameState {
   // loop:
   USER_CHOOSING, WAIT_FOR_ALGORITHM
};

ChessGameState g_gameState = USER_CHOOSING;

void next_game_state()
{
   // switch
   g_gameState = (g_gameState == USER_CHOOSING) ? WAIT_FOR_ALGORITHM : USER_CHOOSING;
}

void set_first_row(byte b)
{
   for (int i = 0; i < 8; i++)
      mtx.pixels[i] = b;
   mtx.update();
   delay(2000);
}

const int key_select = 3;
const int key_left   = 4; 
const int key_right  = 6;
const int key_down   = 7;
const int key_up     = 5;

// communication to stepper motors
uart::Reciever    stepperRX { 2, 9600 };
uart::Transmitter stepperTX { A2, 9600 };

void setup()
{
   stepperRX.init();
   stepperTX.init();

   // initialize user interface
   lcd.init();
   mtx.init();
   buttons.init();

   lcd.clear_screen();
   lcd.set_str("Begin", 0);
   lcd.update();

   // intialize communication with chess-algorithm
   Serial.begin(9600);
   while (!Serial)
      ;

   // restart the game, when arduino is powered on
   send_restart_command();
   delay(500);

}

void loop_user_choosing();
void wait_for_algorithm();

void loop()
{
   buttons.poll();

   switch(g_gameState) {
      case USER_CHOOSING:
         loop_user_choosing(); break;
      case WAIT_FOR_ALGORITHM:
         wait_for_algorithm(); break;
   }

   lcd.append_char('0' + stepperRX.num_packets(), 1);

   update_user_interface();
}

// current cursor position
int cursorX = 3, cursorY = 3;
void set_cursor(int x, int y)
{
   cursorX = x;
   cursorY = y;
}

void move_cursor(int dx, int dy)
{
   cursorX += dx;
   cursorY += dy;

   if (cursorX < 0) cursorX = 7;
   if (cursorX > 7) cursorX = 0;
   if (cursorY < 0) cursorY = 7;
   if (cursorY > 7) cursorY = 0;
}

bool g_cursorBlinkState = false;
const unsigned long cursorBlinkTime = 200; 
unsigned long g_cursorLastBlink = 0;
/** Override matrix display with blink cursor at (cursorX, cursorY). */
void display_cursor()
{
   unsigned long ms = millis();
   if (ms - g_cursorLastBlink > cursorBlinkTime) {
      g_cursorBlinkState = !g_cursorBlinkState;

      g_cursorLastBlink = ms;
   }

   // override pixel on matrix screen to cursors blink state:
   // delete old pixel
   mtx.pixels[cursorY] &= ~(0b10000000 >> cursorX);

   // add on cursor-pixel
   mtx.pixels[cursorY] |= g_cursorBlinkState << (7 - cursorX);
}

byte g_chessBoard[8]; 
/** Get state of chess board by polling algorithm. */
void update_chess_board()
{
   get_all_pieces(g_chessBoard);
}

byte g_possibleMoves[8];
/** Update the state of g_possibleMoves based on algorithm response. */
void update_possible_moves(int fx, int fy)
{
   get_possible_moves(g_possibleMoves, fx, fy);
}

/* Returns whether a given position tx, ty is valid from the g_possibleMoves array */
bool move_is_possible(int tx, int ty)
{
   return bool(g_possibleMoves[ty] & (0b10000000 >> tx));
}

/** Update pixels on matrix display with the given array of bytes. */
void update_matrix_screen_with_arr(byte * arr)
{
   for (int i = 0; i < 8; i++) {
      byte row = arr[i];
      if (mtx.pixels[i] != row)
         mtx.pixels[i] = row;
   }
}

/** Display either the state of the chess board or possible 
 *  moves based on the g_displayPossibleMoves boolean. */
void update_chess_screen(bool displayPossibleMoves)
{
   // display either chess board or possible moves
   update_matrix_screen_with_arr(
      displayPossibleMoves ? g_possibleMoves : g_chessBoard);

   // override with cursor pixel
   display_cursor();
}

void position_to_str(char * s, int x, int y)
{
   char letter = 'A' + x;
   char number = '1' + (7 - y);

   s[0] = letter;
   s[1] = number;
   s[2] = '\0';
}

bool g_chessBoardUpdated = false;
int fromX = -1,
    fromY = -1;
void loop_user_choosing()
{
   // if this is first time -> update the state of the chess board
   // by getting it from the algorithm
   if (!g_chessBoardUpdated) {
      lcd.set_str("Choose \"from\"", 0);
      update_chess_board();
      g_chessBoardUpdated = true;
   }
   
   // move cursor based on button input
   if (buttons.key_just_pressed(key_left))  move_cursor(-1,  0);
   if (buttons.key_just_pressed(key_right)) move_cursor( 1,  0);
   if (buttons.key_just_pressed(key_down))  move_cursor( 0,  1);
   if (buttons.key_just_pressed(key_up))    move_cursor( 0, -1);

   char str[3];
   position_to_str(str, cursorX, cursorY);
   lcd.set_str("Position: ", 1);
   lcd.append_str(str, 1);

   if (buttons.key_just_pressed(key_select)) {
      if (fromX == -1 || fromY == -1) {
         // from position not set -> set it
         fromX = cursorX; fromY = cursorY;

         // update the state of g_possibleMoves 
         // based on the selected "from" square
         update_possible_moves(fromX, fromY);

         lcd.set_str("Choose \"to\"", 0);
      } else {
         // selecting "to" position
         int toX = cursorX,
             toY = cursorY;

         // if same position chosen -> deselect "from" position
         if (toX == fromX && toY == fromY) {
            fromX = -1;
            fromY = -1;

            lcd.set_str("Choose \"from\"", 0);
         } else { // new position 
            // is the move possible?
            if (move_is_possible(toX, toY)) {
               // yes -> do move

               // send to algorithm
               send_move_to_algorithm(fromX, fromY, toX, toY);

               lcd.set_str("Moving piece...", 0);
               lcd.set_str("", 1);
               lcd.update();

               // send to stepper motor
               move_piece_from_to(stepperRX, stepperTX, fromX, fromY, toX, toY);

               // reset state of loop_user_choosing 
               // and go to next game stage
               fromX = -1; fromY = -1;
               g_chessBoardUpdated = false;
               next_game_state();

            } else 
               ;/* Invalid move -> do nothing*/
         }
      }
   }

   // display possible moves if fromX is defined (and fromY)
   bool displayPossibleMoves = fromX != -1;

   // display current chess board (or possible moves) and cursor on screen
   update_chess_screen(displayPossibleMoves);
}

void wait_for_algorithm()
{

   // at this point, stepper has moved user piece
   int fx, fy, tx, ty;
   get_ai_move_from_algorithm(&fx, &fy, &tx, &ty);

   lcd.set_str("Countermove: ", 0);
   lcd.set_str("F: ", 1);
   char s[3]; 
   position_to_str(s, fx, fy);
   lcd.append_str(s, 1);

   lcd.append_str(", T: ", 1);
   position_to_str(s, tx, ty);
   lcd.append_str(s, 1);
   lcd.update();

   move_piece_from_to(stepperRX, stepperTX, fx, fy, tx, ty);

   next_game_state();
}

byte g_waiting[] = {
   0b00010001,
   0b00100010,
   0b01000100,
   0b10001000,
   0b00010001,
   0b00100010,
   0b01000100,
   0b10001000,
};

void rotate_bits_r(byte * b)
{
   byte remaining = (*b & 1) << 7;
   *b <<= 1;
   *b |= remaining;
}

const int g_animationFrametime = 100; // in ms
unsigned long g_lastAnimationFrame = 0;
void display_waiting_animation()
{
   unsigned long ms = millis();
   if (ms - g_lastAnimationFrame > g_animationFrametime) {
      for (int i = 0; i < 8; i++)
         rotate_bits_r(g_waiting[i]);

      g_lastAnimationFrame = ms;
   }

   update_user_interface();
}

void update_user_interface()
{
   lcd.update();
   mtx.update();
   stepperTX.update();
}

void throw_error(const char * str)
{
   lcd.set_str("Error: ", 0);
   lcd.set_str(str, 1);
   lcd.update();
   delay(2000);
   exit(1);
}