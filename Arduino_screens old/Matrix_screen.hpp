#ifndef MATRIX_SCREEN_HPP
#define MATRIX_SCREEN_HPP

class Matrix_screen {
	public:
		// static const is used instead of define
		static const int width = 8;
		static const int blinks_per_second = 100; // make as small as possible
		static const int latch_micros = 20;			   // make as large as possible

		/** Constructor for screen handler, an 8x8 matrix
		 @param lc Pin for latch to shift register on the arduino
		 @param dt Pin for data output to shift register
		 @param ck Pin for clock signal to shift register
		*/
		Matrix_screen(int lc, int dt, int ck) : latch_pin { lc }, data_pin { dt }, clock_pin { ck } { };
 
		byte pixels[width];

		byte get_row  (int row) const { return pixels[row]; };
		bool get_pixel(int x, int y) const; 

		// interface -> what will be mostly used
		void init();
		void update();

		void clear_pixels();
		
	private:
		int latch_pin = -1, 
			 data_pin  = -1, 
			 clock_pin = -1;

		unsigned long last_update = 0; // timing for updating the entire display
		unsigned long last_latch  = 0; // timing for updating each row.
		int m_current_row = 0;			 // index for the update iteration

		void store();
		void shiftRegs();
		void increment_row();
		const int get_current_row() const { return m_current_row; };
};

bool Matrix_screen::get_pixel(int x, int y) const {
	byte mask = 0b00000001;
	int shift = Matrix_screen::width - x - 1;
	mask = mask << shift;

	return (bool) (pixels[y] & mask);
};

void Matrix_screen::increment_row() {
	// increment row and avoid overflow
	m_current_row = (m_current_row >= width - 1) ? 0 : m_current_row + 1;
}

/*
	Run this in the arduino loop function.
	Is asyncronous, and uses micros() to only update the matrix when needed.
	Will therefore not spend time on delay().
*/
void Matrix_screen::update() {
	unsigned long mcs = micros();

	// wait until appropriate time has passed: entire screen update
	if (get_current_row() == 0)
		if (mcs - last_update < 1e6 / (float) blinks_per_second)
			return;
		else
			last_update = mcs;
	
	// wait until appropriate time has passed: single row
	if (mcs - last_latch < latch_micros)
		return;
	
	last_latch = mcs;
	last_update = mcs;

	// set last latch to low
	digitalWrite(latch_pin, LOW);

	// states for LEDS on given row, inverted, since GND means LED is on.
	byte not_row = ~get_row(get_current_row());

	shiftOut(data_pin, clock_pin, LSBFIRST, not_row);
	// turn on appropriate row, by shifting one bit along with the count of the row.
	shiftOut(data_pin, clock_pin, LSBFIRST, 0b10000000 >> get_current_row());

	// latch data
	digitalWrite(latch_pin, HIGH);

	increment_row();
}

/*
	Run this in the arduino setup() function.
*/
void Matrix_screen::init() {
	pinMode(latch_pin, OUTPUT);
	pinMode(data_pin, OUTPUT);
	pinMode(clock_pin, OUTPUT);
}

void Matrix_screen::clear_pixels() {
	for (int row = 0; row < width; row++)	
		pixels[row] = 0b00000000;
}

#endif
