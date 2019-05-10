#include <Bounce.h>
#include "./audioSystem.h"
#include "Siren.h"

#define BIG_RED_BUTTON         0               // big red button should bridge pin 0 and ground.
#define BIG_RED_LED            1               // big red button's LED
#define FLASHER                2               // Flasher's relay should be attached to digital pin 2
#define MAX_MOVIE_SIGN_MS      15000           // Don't let it go more than 5s
#define MS_PER_LOOP            20              // Loop resolution is about 50 Hz. (1000/50 = 20)
#define MS_PER_FLASH           250             // Time for 1/2 cycle of the big red button's LED, about a half second per cycle

#define FLASHER_ON             0
#define FLASHER_OFF            1

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
elapsedMillis bigRedLedTimer;
int bigRedLedState = 0;

void setup() {
  // Set up serial, so there's debugging feedback
  Serial.begin(19200);
  AudioMemory(8);

  // Set up the pin for bigRedButton
  pinMode(BIG_RED_BUTTON, INPUT_PULLUP);

  // Set up the pin controlling the button's LED
  pinMode(BIG_RED_LED, OUTPUT);

  // Set up the pin controlling the flashy lamp relay
  pinMode(FLASHER, OUTPUT);

  // We're ready master (I'm not ready...)
  // Serial.println("Movie sign ready!");
  noMoreMovieSign();
  delay(1000);
  weveGotMovieSign();
}

// sets `state`, enables the flasher, starts playing
void weveGotMovieSign() {
  // Serial.println("WE'VE GOT MOVIE SIGN!");
  state = WEVE_GOT_MOVIE_SIGN;
  digitalWrite(BIG_RED_LED, 1);
  digitalWrite(FLASHER, FLASHER_ON);
  siren.play(Siren);
  bigRedLedTimer = 0;
  bigRedLedState = 0;
}

// resets `state`, disables the flasher, and stops playback
void noMoreMovieSign() {
  // Serial.println("Movie sign: offline");
  state = OFF;
  digitalWrite(BIG_RED_LED, 0);
  digitalWrite(FLASHER, FLASHER_OFF);
  if (siren.isPlaying()) {
    siren.stop();
  }
}



void loop() {
  // reset the timer for the loop
  loopCounter = 0;
  if (state != WEVE_GOT_MOVIE_SIGN) {
    // reset the blaring timeout.
    blaringFor = 0;
  }
  // update the state of the button
  bigRedButton.update();
  // If the button's _just_ been pressed...
  if (bigRedButton.fallingEdge()) {
    // Serial.println("Button pressed");
    // toggle the state
    if (state == OFF) {
      weveGotMovieSign();
    } else {
      noMoreMovieSign();
    }
  }
  // If the thing's been going for long enough
  if (state == WEVE_GOT_MOVIE_SIGN && blaringFor > MAX_MOVIE_SIGN_MS) {
    // Serial.println("Ok, that's long enough.");
    // shut it off
    noMoreMovieSign();
  }
  // If we're on
  if (state == WEVE_GOT_MOVIE_SIGN) {
    // if the siren stopped
    if (!siren.isPlaying()) {
      // Serial.println("Woop.  Play it again, Sam.");
      // loop that shit.
      siren.play(Siren);
    }
    // Flash the LED
    // "X >> 1" is a fast integer "X / 2"
    int halfMs = MS_PER_FLASH >> 1;
    if (bigRedLedState == 0 && bigRedLedTimer >= MS_PER_FLASH) {
      Serial.println("bigRedLed: OFF");
      // We're in the second half of the flash cycle
      digitalWrite(BIG_RED_LED, 0);
      bigRedLedState = 1;
      bigRedLedTimer = 0;
    } else if (bigRedLedState == 1 && bigRedLedTimer >= MS_PER_FLASH) {
      Serial.println("bigRedLed: ON");
      // We're at the end of the cycle
      digitalWrite(BIG_RED_LED, 1);
      // Reset the timer
      bigRedLedState = 0;
      bigRedLedTimer = 0;
    }
  }
  // Let the processor chill for the remaining alloted time for this loop.
  delay(MS_PER_LOOP - loopCounter);
}
