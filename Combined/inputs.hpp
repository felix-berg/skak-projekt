class Buttons {
	public:
		Buttons(int in, int cl) : input_pin { in }, clock_pin { cl } { };

		void init();
		byte poll();
		bool key_is_pressed(int i);

	private:
		int input_pin = -1,
			 clock_pin = -1;

		byte m_poll_result = 0;

		void long_clock();
		void short_clock();

		byte read_byte();
};

/*
  Called in the arduino setup() function.
*/
void Buttons::init() {
	pinMode(input_pin, INPUT);
	pinMode(clock_pin, OUTPUT);
}

void Buttons::long_clock() {
  digitalWrite(clock_pin, HIGH);
  delayMicroseconds(4);
  digitalWrite(clock_pin, LOW);
  delayMicroseconds(4);
}

void Buttons::short_clock() {
  digitalWrite(clock_pin, HIGH);
  delayMicroseconds(1);
  digitalWrite(clock_pin, LOW);
  delayMicroseconds(1);
}

byte Buttons::read_byte() {
	byte result = 0;
  
	for (int i = 0; i < 8; i++) {
		result = result | digitalRead(input_pin);
		result <<= 1;		

    long_clock();
	}

  short_clock();
 
	return result;
}

bool Buttons::key_is_pressed(int i) {
	return (bool) (m_poll_result & (0b00000001 << i));
}

/*
	To be called in the loop() arduino function. Reads a byte and returns it. The buttons state can be gotten with key_is_pressed().
*/
byte Buttons::poll() {
	m_poll_result = read_byte();
	return m_poll_result;
}
