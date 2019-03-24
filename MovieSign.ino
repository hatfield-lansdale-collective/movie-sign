#include <Bounce.h>
#include "./audioSystem.h"
#include "Siren.h"

#define BIG_RED_BUTTON         0               // big red button should bridge pin 0 and ground.
#define BIG_RED_LED            1               // big red button's LED
#define FLASHER                2               // Flasher should be attached to digital pin 2
#define SIREN                  "MOVIESGN.WAV"  // Play this file off the SD card
#define MAX_MOVIE_SIGN_MS      5000            // Don't let it go more than 5s
#define MS_PER_LOOP            40              // Loop resolution is about 25 Hz.
#define MS_PER_FLASH           500             // Time for 1 cycle of the flasher, about a half second


// Listen for changes to the big red button
// (pinNumber, sampleWindowInMS)
Bounce bigRedButton = Bounce(BIG_RED_BUTTON, 5);

// States of this machine
#define OFF 0
#define WEVE_GOT_MOVIE_SIGN 1

// Somewhere to keep the current state
int state = 0;

// elapsedMillis' automatically increment once per millisecond

// Gets reset when user presses the bigRedButton
elapsedMillis blaringFor;

// Gets reset at the start of loop().  We use this to limit
// how often the loop runs so we can get a consistent test rate on the
// big red button.
elapsedMillis loopCounter;

// Timer for the flasher
elapsedMillis bigRedLedState;

void setup() {
  // Set up the pin for bigRedButton
  pinMode(BIG_RED_BUTTON, INPUT_PULLUP);

  // Set up the pin controlling the button's LED
  pinMode(BIG_RED_LED, OUTPUT);

  // Set up the pin controlling the flashy lamp relay
  pinMode(FLASHER, OUTPUT);

  // Set up serial, so there's debugging feedback
  Serial.begin(19200);
  Serial.println("Movie sign ready!");
}

// sets `state`, enables the flashers, starts playing, and resets the counter
void weveGotMovieSign() {
  Serial.println("WE'VE GOT MOVIE SIGN!");
  state = WEVE_GOT_MOVIE_SIGN;
  digitalWrite(BIG_RED_LED, 1);
  digitalWrite(FLASHER, 1);
  siren.play(Siren);
  blaringFor = 0;
  bigRedLedState = 0;
}

// resets `state`, disables the flasher, and stops playback
void noMoreMovieSign() {
  Serial.println("Movie sign: offline");
  state = OFF;
  digitalWrite(BIG_RED_LED, 0);
  digitalWrite(FLASHER, 0);
  if (siren.isPlaying()) {
    siren.stop();
  }
}



void loop() {
  // reset the timer for the loop
  loopCounter = 0;
  // update the state of the button
  bigRedButton.update();
  // If the button's _just_ been pressed...
  if (bigRedButton.fallingEdge()) {
    Serial.println("Button pressed");
    // toggle the state
    if (state == OFF) {
      weveGotMovieSign();
    } else {
      noMoreMovieSign();
    }
  }
  // If the thing's been going for long enough
  if (state == WEVE_GOT_MOVIE_SIGN && blaringFor > MAX_MOVIE_SIGN_MS) {
    Serial.println("Ok, that's long enough.");
    // shut it off
    noMoreMovieSign();
  }
  // If we're still on, and the siren stopped
  if (state == WEVE_GOT_MOVIE_SIGN && !siren.isPlaying()) {
    Serial.println("Woop.  Play it again, Sam.");
    // loop that shit.
    siren.play(Siren);
  }
  if (WEVE_GOT_MOVIE_SIGN) {
    // Flash the LED
    // "X >> 1" is a fast integer "X / 2"
    if (bigRedLedState > (MS_PER_FLASH >> 1)) {
      // We're in the second half of the flash cycle
      digitalWrite(BIG_RED_LED, 0);
    } else if (bigRedLedState > MS_PER_FLASH) {
      // We're at the end of the cycle
      digitalWrite(BIG_RED_LED, 1);
      // Reset the timer
      bigRedLedState = 0;
    }
  }
  // Let the processor chill for the remaining alloted time for this loop.
  delay(MS_PER_LOOP - loopCounter);
}
