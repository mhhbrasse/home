#ifndef TDisplay_h
#define TDisplay_h

#include <Windows.h>

typedef unsigned int uint32_t;

class TDisplay
{
public:
	TDisplay(int width, int height);
	~TDisplay();
	void display(COLORREF* colordata, int frameWidth, int frameHeight);	
	
private:
	COLORREF *displayBuffer; // reference to color buffer for display on device screen 
	int displayWidth;
	int displayHeight;
	bool valid;
};

#endif