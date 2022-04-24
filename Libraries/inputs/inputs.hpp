class Buttons {
	public:
		static const int time_between_polls = 2000; // once every 2000 µs

		Buttons(int in, int cl) : input_pin { in }, clock_pin { cl } { };

		void init();
		byte poll();
		bool key_is_down(int i);
		bool key_just_pressed(int i);

	private:
		int input_pin = -1,
			 clock_pin = -1;

		byte m_poll_result = 0;
		byte m_recent = 0; // which buttons just changed?

		unsigned long m_last_poll_time = 0;

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

		if (i != 7) result <<= 1;		

   	long_clock();
	}

 	short_clock();
 
	return result;
}

bool Buttons::key_is_down(int i) {
	return (bool) (m_poll_result & (0b10000000 >> i));
}

bool Buttons::key_just_pressed(int i) {
	return (bool) (m_recent & (0b10000000 >> i));
}

/*
	To be called in the loop() arduino function. Reads a byte and returns it. The buttons state can be gotten with key_is_down().
*/
byte Buttons::poll() {
	unsigned long mcs = micros();
	if (mcs - m_last_poll_time > time_between_polls) {
		byte last = m_poll_result;
		m_poll_result = read_byte();

		m_recent = (last ^ m_poll_result) & m_poll_result;

		m_last_poll_time = mcs;
	}

	return m_poll_result;
}
