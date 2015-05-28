// Probabilites.ino - for explaining conditional probabilities.
// TN-04
// For faster upload with ArduinoISP, add to boards.txt: arduinoisp.program.extra_params=-B1

// 2 states.  
// When master is pressed, set outputs to 0 and colour to OFF.  
// When master is released: 
//   if no inputs connected: pick a state
//   if inputs are connected then wait for them to change.
//   then pick between inputs and internal.  Then pick state.
//   Once changed, set colour + outputs simultaneously.

// Knob: no effect
// Pushbutton: lock on/off.  Flashes when locked.  (stops flashing when master pressed)
//   Toggle through unlocked and each of the colours.

// Only read inputs when master is released!

// No check for loops, but it just won't do anything!

#include <TN.h>

const int states = 2;
const double probabilities[] = {0.3, 0.7}; // SET PROBABILITIES HERE!
const int colours[states][3] = { {255,255,0}, {0,255,255} };
int state = 0;

TN Tn = TN(0.0,1.0*states);




void setup() {}


void loop() {
  if (Tn.masterSw() ) masterPressed();
  if (Tn.sw()) lock();
  delay(30);
}

int lock() {
  while (Tn.sw()) delay(30);
  int blinkMax = 30;
  int blinkCtr = 0;
  int prevSw = Tn.sw();
  int lockState = 0;
  for (int i=1; i<4; i++) {
    Tn.analogWrite(i,1.0 + (double)lockState);
  }
  while (1) {
    if (!Tn.sw() && prevSw) {
      lockState++;
      blinkCtr = 0;
      if (lockState == states) {
        Tn.colour(0,0,0);
        for(int i=1; i<4; i++) {
          Tn.analogWrite(i,0.0);
        }
        return 0;
      }
      for (int i=1; i<4; i++) {
        Tn.analogWrite(i,1.0 + (double)lockState);
      }
    }
    prevSw = Tn.sw();
    
    if (blinkCtr == 0) {
      Tn.colour(colours[lockState][0],colours[lockState][1],colours[lockState][2]);
    }
    if (blinkCtr == blinkMax/2) Tn.colour(0,0,0);
    blinkCtr++;
    blinkCtr %= blinkMax;
    delay(30);
    
  }
}
    
    


void masterPressed() { // called when the master switch is pressed
  Tn.colour(0,0,0);
  for (int i=1; i<4; i++) {
    Tn.analogWrite(i,0.0);
  }
  delay(50);
  while (Tn.masterSw()) delay(50);  
  for (int i=1; i<4; i++) {
    while (Tn.isConnected(i) && round(Tn.analogRead(i)) == 0) delay(50);
  }
  generateState();
  Tn.colour(colours[state][0],colours[state][1],colours[state][2]);

  delay(1000);
  for (int i=1; i<4; i++) {
    Tn.analogWrite(i,1.0 + (double)state);
  }
}


int generateState() {
  while (1) {
    long source = random(4);
    if (source == 0) {
      double val = (double)random(2147483647L)/(double)2147483647L;
      for (int i=0; i<states; i++) {
        if (val < probabilities[i]) {
          state = i;
          return 0;
        }
        val = val - probabilities[i];
      }
    }
    else { // read from one of the inputs
      if (Tn.isConnected(source)) {
        state = round(Tn.analogRead(source) - 1.0);
        return 0;
      }
    }
  }
}  
  




