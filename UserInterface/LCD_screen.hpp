#ifndef LED_SCREEN_HPP
#define LED_SCREEN_HPP

#include "Wire.h"
#include "LiquidCrystal_I2C.h"

void print_error(const char * errorstr) {
  Serial.println(errorstr);
  delay(1000);
  exit(1);
}

class LCD_screen {
	public:
		static const int cols = 16;
		static const int rows = 2;

		LCD_screen();
    ~LCD_screen();

		void init();
		void update();

		void set_str(const char * str, int row);
		void append_str(const char * str, int row);
		void append_char(char c, int row);
		void clear_row(int row);
		void clear_screen();

	private:
		LiquidCrystal_I2C m_lcd;

		char m_screen_contents[rows][cols];
};

LCD_screen::LCD_screen() 
	: m_lcd { 0x27, cols, rows }
{ }

LCD_screen::~LCD_screen() {
  clear_screen();
  m_lcd.setBacklight(LOW);
}

void LCD_screen::init() {
	m_lcd.init();
	m_lcd.setBacklight(HIGH);
 
  clear_screen();
};

void LCD_screen::set_str(const char * str, int row) {	
	if (strlen(str) > cols) {
		print_error("Error: String to large to print.\n");
	}
	if (row >= rows) {
	  print_error("Error: Cannot set row %d.\n");
	}
	
	strcpy(m_screen_contents[row], str);
}

void LCD_screen::append_str(const char * str, int row) {
	if (strlen(str) + strlen(m_screen_contents[row]) > cols) {
		print_error("Error: appended string, that is too long.\n");
	}
	strcat(m_screen_contents[row], str);
}

void LCD_screen::append_char(char c, int row) {
	char str[2] = { c, '\0' };
	append_str(str, row);
}

void LCD_screen::clear_row(int row) {
	m_screen_contents[row][0] = '\0';
}

void LCD_screen::clear_screen() {
	for (int i = 0; i < rows; i++)
		clear_row(i);
}

void LCD_screen::update() {
	static char prev[rows][cols];

	bool has_changed = false;
	for (int i = 0; i < rows; i++) {
		if (strcmp(m_screen_contents[i], prev[i]) != 0)
			has_changed = true;
	}

	if (!has_changed)
		return;

	m_lcd.clear();



	for (int i = 0; i < rows; i++) {
		strcpy(prev[i], m_screen_contents[i]);
		m_lcd.setCursor(0, i);
		m_lcd.printstr(m_screen_contents[i]);
	}
};

#endif
