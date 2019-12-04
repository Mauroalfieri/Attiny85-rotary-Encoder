#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define encA 3
#define encB 4
#define swtc 1
#define PIN  2
#define NUMPIXELS      24

volatile byte currentValue = 0;
volatile int  lastEncoded  = 0;
volatile byte mybrightness = 128;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    pinMode(encA, INPUT);
    pinMode(encB, INPUT);
    pinMode(swtc, INPUT);
    digitalWrite(encA, HIGH);
    digitalWrite(encB, HIGH);
   
    GIMSK = 0b00100000;       // Enable pin change interrupts
    PCMSK = 0b00011010;       // Enable pin change interrupt for PB3 and PB4 and PB1
    sei();                    // Turn on interrupts

    pixels.setBrightness(mybrightness);
    pixels.begin();
}
 
void loop() {
    byte numLed = (currentValue*NUMPIXELS/255);
    
    for(int i=0;i<numLed;i++) { pixels.setPixelColor(i, 38,114,184); }
    for(int i=numLed;i<NUMPIXELS;i++) { pixels.setPixelColor(i, 0,0,0); }
    pixels.show(); // This sends the updated pixel color to the hardware.

    if (digitalRead(swtc) == LOW ) { currentValue = setCurrBrightness();  }
    delay(20);
}

byte setCurrBrightness() {
  byte oldValue = currentValue;
  currentValue = mybrightness;
  
  while (digitalRead(swtc) == LOW) {
    mybrightness = currentValue;
    pixels.setBrightness(mybrightness);
    pixels.show();
  }
  return oldValue;
}

ISR(PCINT0_vect) {
  byte MSB = digitalRead(encA); //MSB = most significant bit
  byte LSB = digitalRead(encB); //LSB = least significant bit
  
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
 
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)  currentValue++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)  currentValue--;
  
  lastEncoded = encoded; //store this value for next time
}
