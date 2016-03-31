#include <LED_animations.h>
#include <Adafruit_NeoPixel.h>

// RGB of common colors
#define RED   0xff0000
#define GREEN 0x00ff00
#define BLUE  0x0000ff

//
// value = 256 * log10(i) / log10(256) where i is 1-256
// This is used to give a logarithmic response to the color wheel
static const uint8_t PROGMEM
log10lookup[256] = {
		0, 31, 50, 63, 74, 82, 89, 95, 101, 106, 110, 114, 118, 121, 125, 127, 130, 133, 135, 138, 140, 142, 144, 146, 
		148, 150, 152, 153, 155, 157, 158, 159, 161, 162, 164, 165, 166, 167, 169, 170, 171, 172, 173, 174, 175, 176, 
		177, 178, 179, 180, 181, 182, 183, 184, 185, 185, 186, 187, 188, 189, 189, 190, 191, 191, 192, 193, 194, 194, 
		195, 196, 196, 197, 198, 198, 199, 199, 200, 201, 201, 202, 202, 203, 204, 204, 205, 205, 206, 206, 207, 207, 
		208, 208, 209, 209, 210, 210, 211, 211, 212, 212, 213, 213, 213, 214, 214, 215, 215, 216, 216, 217, 217, 217, 
		218, 218, 219, 219, 219, 220, 220, 221, 221, 221, 222, 222, 222, 223, 223, 223, 224, 224, 225, 225, 225, 226, 
		226, 226, 227, 227, 227, 228, 228, 228, 229, 229, 229, 230, 230, 230, 231, 231, 231, 231, 232, 232, 232, 233, 
		233, 233, 234, 234, 234, 234, 235, 235, 235, 236, 236, 236, 236, 237, 237, 237, 237, 238, 238, 238, 238, 239, 
		239, 239, 239, 240, 240, 240, 241, 241, 241, 241, 241, 242, 242, 242, 242, 243, 243, 243, 243, 244, 244, 244, 
		244, 245, 245, 245, 245, 245, 246, 246, 246, 246, 247, 247, 247, 247, 247, 248, 248, 248, 248, 249, 249, 249, 
		249, 249, 250, 250, 250, 250, 250, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 253, 253, 253, 253, 253, 
		253, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255
};

// LED animations
LED_animations::LED_animations(Adafruit_NeoPixel &stripArg, int defWaitArg, int groupArg) : 
strip(stripArg), defWait(defWaitArg), group(groupArg) 
{
	numPixels = strip.numPixels() / groupArg;
}; 

void LED_animations::Rainbow (int spanFactor, int wait)
{
	static Color CurrentColor = 0;

	for(int i=0; i < numPixels; i++)
		setPixelColor(i, wheel((i*spanFactor / numPixels)+CurrentColor));
	show();
	CurrentColor = ++CurrentColor % 256;
	animateDelay(wait);
}

void LED_animations::Solid (Color color, int wait)
{
	for(int i=0; i < numPixels; i++)
		setPixelColor(i, color);
	show();
	animateDelay(wait);	
}

void LED_animations::ColorWipe (Color color, int wait)
{
	static int currentSpot = 0;
	for(int i=0; i < numPixels; i++)
		setPixelColor(i, (i < currentSpot) ? color : 0);
	show();
	currentSpot = ++currentSpot % (numPixels+1);
	animateDelay(wait);	
}

void LED_animations::TheaterChase (Color color, int wait)
{
	const int NUM_PHASE = 3;
	static int phase = 0;
	for(int i=0; i < numPixels; i++)
		setPixelColor(i, (i % NUM_PHASE == phase) ? color : 0);
	show();
	phase = ++phase % NUM_PHASE;
	animateDelay(wait);
}

void LED_animations::TheaterChaseRainbow (int wait)
{
	const int NUM_PHASE = 4;
	static int phase = 0;
	static Color CurrentColor = 0;
	for (int j = 0; j < 10; j++)
	{
		for(int i=0; i < numPixels; i++)
		{
			if (i % NUM_PHASE == phase)
				setPixelColor(i, blend (0, wheel(i+CurrentColor), j*25));
			else if ((i+1) % NUM_PHASE == phase)
				setPixelColor (i, blend (wheel(i+1+CurrentColor), 0, j*25));
			else
				setPixelColor (i, 0);
		}
		show();
		if (animateDelay(wait >= 0 ? wait/10 : defWait/10))
			return;
	}
	phase = ++phase % NUM_PHASE;	
	CurrentColor = (CurrentColor+10) % 256;
}

//
// Wheel function with a logarithmic response, good for converting
// numbers with a logarithmic response (like sound intensity) into
// colors.
Color LED_animations::logWheel(int WheelPos)
{
	const int WheelSize = 256;	
	WheelPos %= WheelSize;
	int logWheelPos = pgm_read_byte(&log10lookup[WheelPos]);
	return (wheel(logWheelPos));
}

//
// The famous color wheel: blue->green->red->blue
Color LED_animations::wheel(int WheelPos)
{
	const int WheelSize = 256;	
	WheelPos %= WheelSize;

	if(WheelPos < 85)
		return blend (BLUE, GREEN, WheelPos*3);
	else if(WheelPos < 170)
		return blend (GREEN, RED, (WheelPos-85)*3);
	else
		return blend (RED, BLUE, (WheelPos-170)*3);
}

//
// Blend two colors together based on the ratio. ratio of 0 will be 100% color a and
// ratio of 255 will be 100% color b.
Color LED_animations::blend (Color ina, Color inb, uint8_t ratio)
{
	  Color r = ((((ina >> 16) & 0xff) * (255-ratio)) + (((inb >> 16) & 0xff) * ratio)) / 255;
	  Color g = ((((ina >> 8) & 0xff) * (255-ratio)) + (((inb >> 8) & 0xff) * ratio)) / 255;
	  Color b = ((((ina >> 0) & 0xff) * (255-ratio)) + (((inb >> 0) & 0xff) * ratio)) / 255;
	  return ((r << 16) | (g << 8) | b);
}

void LED_animations::setPixelColor (int index, Color pixelColor)
{
	for (int i = 0; i < group; i++)
	{
		int pixel = index*group+i;
		strip.setPixelColor (pixel, pixelColor);
	}
}

void LED_animations::show()
{
	strip.show();
}

bool LED_animations::animateDelay(int wait)
{
	if (wait == -1)
		wait = defWait;
	const int maxWait = 50;
	int numWait = wait / maxWait;
	int remainderWait = wait - numWait * maxWait;

	for (int i = 0; i < numWait; i++)
	{
		if (interruptFlag)
		{
			interruptFlag = false;
			return true;
		}
		delay(maxWait);
	}
	if (interruptFlag)
	{
		interruptFlag = false;
		return true;
	}
	delay(remainderWait);
	return false;
}
