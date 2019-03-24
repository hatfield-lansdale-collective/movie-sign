#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

#define BIG_RED_BUTTON         0               // big red button should bridge pin 0 and ground.
#define FLASHER                1               // Flasher should be attached to digital pin 1
#define SIREN                  "MOVIESGN.WAV"  // Play this file off the SD card
#define MAX_MOVIE_SIGN_MS      5000            // Don't let it go more than 5s
#define MS_PER_LOOP            40              // Loop resolution is about 25 Hz.

// Define audio components
AudioPlaySdWav           siren;
AudioOutputAnalogStereo  dacs;

// Connect mixDown output to DAC
// (fromDevice, fromChannel, toDevice, toChannel)
AudioConnection          patchLeft(siren, 0, dacs, 0);
AudioConnection          patchRight(siren, 1, dacs, 1);

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

void setup() {
  // Set up the pin for bigRedButton
  pinMode(BIG_RED_BUTTON, INPUT_PULLUP);

  // Set up the pin controlling the flashy lamp relay
  pinMode(FLASHER, OUTPUT);

  // Set up serial, so there's debugging feedback
  Serial.begin(19200);
  Serial.println("Movie sign ready!");
}

// sets `state`, enables the flasher, starts playing, and resets the counter
void weveGotMovieSign() {
  Serial.println("WE'VE GOT MOVIE SIGN!");
  state = WEVE_GOT_MOVIE_SIGN;
  digitalWrite(FLASHER, 1);
  siren.play(SIREN);
  blaringFor = 0;
}

// resets `state`, disables the flasher, and stops playback
void noMoreMovieSign() {
  Serial.println("Movie sign: offline");
  state = OFF;
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
    siren.play(SIREN);
  }
  // Let the processor chill for the remaining alloted time for this loop.
  delay(MS_PER_LOOP - loopCounter);
}
