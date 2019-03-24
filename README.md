# WE'VE GOT MOVIE SIGN!

* [Parts list](https://www.amazon.com/hz/wishlist/ls/E0ANLW49C5FE)

## Design plan

The Teensy will have two states:

- State 0: off
    - [x] Lamp driver relay is switched off
    - [x] DAC is not playing
    - [ ] Button lamp is off
    - [x] When the button is pressed, it will switch to state 1
- State 1: WE'VE GOT MOVIE SIGN
    - [x] Lamp driver relay is switched on
    - [x] DAC is playing MOVIESGN.WAV on a loop
    - [ ] Button lamp is flashing
    - [x] When the button is pressed, it will switch to state 0
    - [x] It will play for no more than 5 seconds

## Block diagram

  

    digraph MovieSign {
      node [shape="rect"]
      BTN [shape="circle", color="red"]
      PWR_12V -> AMP [label="12V"]
      PWR_12V -> BUCK_5V [label="12V"]
      PWR_12V -> RELAY [label="12V"]
      BUCK_5V -> TEENSY [label="5V"]
      BTN -> TEENSY [label="3.3V signal"]
      TEENSY -> AMP [label="3.3V analog signal"]
      TEENSY -> RELAY [label="3.3V signal"]
      RELAY -> LIGHT [label="12V switched power"]
      TEENSY -> BTN [label="3.3V light control signal"]
      AMP -> SPEAKER [label="12V 10W analog signal"]
    }
