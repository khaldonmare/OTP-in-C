//
// Example of a heart_beat code for use in any sketch where a visual indication is
// required that the sketch is operating.
//
// Ron D Bentley, Stafford, UK, July 2021
//
// This example and code is in the public domain and
// may be used without restriction and without warranty.
//
// Note that
// 1. this example uses the LED_BUILTIN digital pin, normally digital pin 13,
//    so no wiring or external LED is needed to implement this code.
//    However, if an external LED is wished, then simply decide which digital pin
//    is to be used, change to definition 'hear_beat_pin' to be this pin mumber
//    and wire in the external LED.
//

#define heart_beat_pin   LED_BUILTIN  // digital pin for heart beat LED

long unsigned heart_beat_freq = 1000; // time(milliseconds) of heart beat frequency

long unsigned heart_beat_on_off_time; // the time the LED is on and off - 1/2 frequency
long unsigned last_heart_beat_time;   // time in milliseconds of last heart beat status change
bool heart_beat_status = HIGH;        // current status of heart beat, start high

//
// Function handles the heart beat cycle.
// May be called from anywhere, but at least every main loop cycle.
//
void heart_beat() {
  if (millis() - last_heart_beat_time >= heart_beat_on_off_time) {
    // time to swap status of the heart beat LED and update it
    last_heart_beat_time = millis();
    heart_beat_status = !heart_beat_status;           // invert current heart beat status value
    digitalWrite(heart_beat_pin, heart_beat_status);  // update LED with new status
  }
}

void setup() {
  // setup heart beat
  pinMode(heart_beat_pin, OUTPUT);
  heart_beat_on_off_time = heart_beat_freq / 2; // LED is on and off at 1/2 frequency time
  // end of heart beat setup
}

void loop() {
  heart_beat();
  // add your other code here...


}
