// LED animations library
// Pete Reiter
// The idea for most of these was cribbed from other sources.
// I've encapsulated them into a convenient library. I also
// made each one non-blocking, so that you can switch between them
// conveniently with an interrupt. You can even bail out of delays
// early by calling SetInterruptFlag()
#include <stdint.h>
class Adafruit_NeoPixel;
typedef uint32_t Color;

class LED_animations {
public:	
	LED_animations (Adafruit_NeoPixel &stripArg,      // Reference to neopixel class 
			int defWaitArg = -1,                      // default delay between iterations of animations
			int groupArg=1);                          // You can use this to group pixels. Grouped pixels
	                                                  // will all display the same color.
	
	// utility functions
	Color wheel(int WheelPos);                         // color wheel (0-255)
	Color logWheel(int WheelPos);                      // color wheel with a log response
	Color blend (Color ina, Color inb, uint8_t ratio); // mix two colors. ratio of 0 is all color a
                                                       // ratio 255 is all color b
	// animations
	void Rainbow (int spanFactor = 10, int wait = -1); // spanFactor is how much of the spectrum
	                                                   // you want showing at once. 0=all pixels same
	                                                   // color, 255= whole spectrum is shown across pixels
	void Solid (Color color, int wait = -1);           // set all pixels the same color
	void ColorWipe (Color color, int wait = -1);
	void TheaterChase (Color color, int wait = -1);
	void TheaterChaseRainbow (int wait = -1);
	
	void SetInterruptFlag() { interruptFlag = true; }  // bail out of an animation early, call from ISR
	void setPixelColor (int index, Color pixelColor);  // wrapper for strip.setPixelColor
	void show();                                       // wrapper for show
	
private:
	Adafruit_NeoPixel &strip;
	int defWait;                                       // default delay
	int group;                                         // led grouping
	int numPixels;                                     // cached value of the number of pixels
	volatile bool interruptFlag;
	
	bool animateDelay(int wait = -1);                  // wrapper for delay
};