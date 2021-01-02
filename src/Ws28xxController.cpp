#include "ws28xxcontroller.h"

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

Ws28xxController::Ws28xxController() {}

void Ws28xxController::init() {
#if DEBUG > 0
  Serial.println("Initializing Ws28xxController");
#endif
  pixels.begin(); // This initializes the NeoPixel library.
}

void Ws28xxController::setPixel(int pixel, byte red, byte green, byte blue) {
   pixels.setPixelColor(pixel, pixels.Color(red, green, blue));
}

void Ws28xxController::showStrip() {
   pixels.show();
}

void Ws28xxController::stop() {
#if DEBUG > 1
  Serial.println("stop");
#endif
  for(int i = 0; i < NUMPIXELS; i++ ) {
    this->setPixel(i, 0, 0, 0);
  }
  this->showStrip();
}

void Ws28xxController::forward(byte brightness) {
  for(int i = 0; i < NUMPIXELS/2; i++ ) {
    this->setPixel(i, brightness, brightness, brightness);
  }
  for(int i = NUMPIXELS/2; i < NUMPIXELS; i++) {
    this->setPixel(i, brightness, 0, 0);
  }  
  this->showStrip();
} 

void Ws28xxController::backward(byte brightness) {
  for(int i = 0; i < NUMPIXELS/2; i++ ) {
    this->setPixel(i, brightness, 0, 0);
  }
  for(int i = NUMPIXELS/2; i < NUMPIXELS; i++) {
    this->setPixel(i, brightness, brightness, brightness);
  }    
  this->showStrip();
} 

void Ws28xxController::fadeIn(boolean isForward) {
#if DEBUG > 1
  Serial.println("fadein " + String(isForward));
#endif
  for(int k = 0; k < MAX_BRIGHTNESS+1; k++) {
    isForward ? this->forward(k) : this->backward(k);
    delay(15);
  }
}

void Ws28xxController::fadeOut(boolean isForward){
#if DEBUG > 1
   Serial.println("fadeout " + String(isForward));
#endif
   for(int k = MAX_BRIGHTNESS; k >= 0; k--) {
      isForward ? this->forward(k) : this->backward(k);
      delay(15);
    }
}

void Ws28xxController::flash(boolean isForward) {
#if DEBUG > 1
  Serial.println("flash " + String(isForward));
#endif
  for(int j=0; j<10; j++) {
    for(int i = 0; i < NUMPIXELS/2; i++ ) {
      isForward ? this->setPixel(i, j%2 == 0 ? MAX_BRIGHTNESS_BRAKE : MAX_BRIGHTNESS, 0, 0) : this->setPixel(i, MAX_BRIGHTNESS, MAX_BRIGHTNESS, MAX_BRIGHTNESS);
    }
    for(int i = NUMPIXELS/2; i < NUMPIXELS; i++) {
      isForward ? this->setPixel(i, MAX_BRIGHTNESS, MAX_BRIGHTNESS, MAX_BRIGHTNESS): this->setPixel(i, j%2 == 0 ? MAX_BRIGHTNESS_BRAKE : MAX_BRIGHTNESS, 0, 0);
    }    
    this->showStrip();
    delay(80);
  }
}

void Ws28xxController::startSequence(byte red, byte green, byte blue, int speedDelay) {
#if DEBUG > 1
    Serial.println("Ws28xxController startSequence ");
#endif
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < NUMPIXELS; i=i+3) {
        this->setPixel(i+q, red, green, blue);    //turn every third pixel on
      }
      this->showStrip();
     
      delay(speedDelay);
     
      for (int i=0; i < NUMPIXELS; i=i+3) {
        this->setPixel(i+q, 0,0,0);        //turn every third pixel off
      }
    }
  }
}


void Ws28xxController::loop(int *new_forward, int *old_forward, int *new_backward, int *old_backward) {
   // is there a change detected
  if(old_forward != new_forward || old_backward != new_backward) { 
#if DEBUG > 1
    Serial.print("change detected: ");
    Serial.print(", forward is "  + String(*new_forward)  + " was " + String(*old_forward));
    Serial.println(", backward is " + String(*new_backward) + " was " + String(*old_backward));
#endif

  if(*new_forward == HIGH && *new_backward == LOW) {
    this->fadeOut(false);
    this->fadeIn(true);
  }

  if(*new_backward == HIGH && *new_forward == LOW) {
    this->fadeOut(true);
    this->fadeIn(false);
  }
      
  *old_forward = *new_forward;
  *old_backward = *new_backward;
  }
}