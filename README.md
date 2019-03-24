# WE'VE GOT MOVIE SIGN!

This is a project between [Fordi](https://github.com/Fordi) and [cjksallan](https://github.com/cjksallan) to design and build a Big Red Button to slap, which will produce the effects of MST3K's MOVIE SIGN, e.g., a loud buzzer, a flashy light, and all around annoyance to everyone.

* [Parts list](https://www.amazon.com/hz/wishlist/ls/E0ANLW49C5FE)

## Design plan

The Teensy will have two states:

- State 0: off
    - [x] Lamp driver relay is switched off
    - [x] DAC is not playing
    - [x] Button lamp is off
    - [x] When the button is pressed, it will switch to state 1
- State 1: WE'VE GOT MOVIE SIGN
    - [x] Lamp driver relay is switched on
    - [x] DAC is playing MOVIESGN.WAV on a loop
    - [x] Button lamp is flashing
    - [x] When the button is pressed, it will switch to state 0
    - [x] It will play for no more than 5 seconds

## Block diagram

![Wiring diagram](https://raw.githubusercontent.com/hatfield-lansdale-collective/movie-sign/master/movie_sign.svg?sanitize=true)

    digraph MovieSign {
      node [shape="rect"]
      BTN [shape="circle", color="red"]
      PWR_12V -> AMP [label="12V"]
      PWR_12V -> BUCK_5V [label="12V"]
      PWR_12V -> RELAY [label="12V"]
      BUCK_5V -> TEENSY [label="5V"]
      BTN -> TEENSY [label="INPUT_PULLUP"]
      TEENSY -> AMP [label="DAC"]
      TEENSY -> RELAY [label="OUTPUT"]
      RELAY -> LIGHT [label="12V switched"]
      TEENSY -> BTN [label="OUTPUT"]
      AMP -> SPEAKER [label="analog"]
    }
