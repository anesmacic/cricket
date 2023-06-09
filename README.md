# Cricket

Cricket is a microcontroller that gets serial input to produce square waves on two pins (PB1 and PD6) on an atMega328p.
Serial command must be formatted as <code>SNNNMMMM</code> where <code>NNN</code> and <code>MMM</code> are the number of cycles for each of the high time of square wave pulse trains.
For instance <code>S150150L</code> produces two square wave pulse trains with a frequency of about 53kHz (with a 16MHz external oscillator).
<code>S000000L</code> is used to stop all triggers. When stopped, both pins are set to high impedance.
