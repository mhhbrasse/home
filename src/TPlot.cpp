#include <Windows.h>
#include "TPlot.h"

TPlot::TPlot(TRender& myRenderer)
{ 
	frameWidth = myRenderer.getFrameWidth();
	frameHeight = myRenderer.getFrameHeight();
	colorBuffer = myRenderer.getBuffer();
}

TPlot::~TPlot() {}

bool TPlot::plot(int x, int y, int r, int g, int b) 
{
	bool result = false;
	if (0<=x && x<frameWidth) 
	{
		if (0<=y && y<frameHeight) 
		{
			unsigned char ucolor_r = (unsigned char ) r;
			unsigned char ucolor_g = (unsigned char ) g;
			unsigned char ucolor_b = (unsigned char ) b;
			colorBuffer[(frameHeight-y-1)*frameWidth + x] = ucolor_r<<16 | ucolor_g<<8 | ucolor_b; 
			result = true;
		}
	}	
	return result;
}


// next method, see <>
void TPlot::drawLine(int xb, int yb, int xe, int ye, int r, int g, int b)
{
	int dx0;
	int dy0;

	// sort begin-point and end-point
	if (xb>xe) 
	{ 
	  int t;
	  t=xb; xb=xe; xe=t;
	  t=yb; yb=ye; ye=t;
	}
	dx0 = xe-xb; // dx0 >= 0
	dy0 = ye-yb;

	// special case (point)
	if (dx0==0 && dy0==0)
	{
		plot(xb, yb, r, g, b);
		return;
	}

	// special case (vertical line)
	if (dx0==0)
	{
		// vertical line
		if (yb<=ye)
		{
			for (int y=yb; y<=ye; y++) plot(xb, y, r, g, b);
		}
		else
		{
			for (int y=ye; y<=yb; y--) plot(xb, y, r, g, b);
		}
		return;
	}

	// regular case (line with dx0>0)
	int x1=0;
	int x2=xe-xb;
	int y1=0;
	int y2=ye-yb;
	int dx=x2-x1; // dx>=0
	int dy=y2-y1;
	// transform to the case dy>0 and dy<dx
	if (dy>=0 && abs(dy)>dx)      { int t; t=x2; x2=y2; y2=t;  } 
	else if (dy<0 && abs(dy)<=dx) { y2 = -y2; }
	else if (dy<0 && abs(dy)>dx)  { int t; t=x2; x2=-y2; y2=t; } 
	dx=x2-x1; 
	dy=y2-y1;
	//
	// draw line from (x1,y1) to (x2,y2), x2-x1 > 0
	//
	int x=x1; 
	int y=y1;
	int v = (x-x1)*dy - (y-y1)*dx;
	// Invariant I: v == (x-x1)*dy - (y-y1)*dx 
	// dy > 0 /\ dx > 0
	while (x<=x2)
	{
		int xp; // xplot
		int yp; // yplot
		// (y-y1) <= (x-x1)*dy/dx + 0.5 (S) /\ I /\ dx > 0 /\ dy > 0 
		while (2*v >= dx)
		{
			// 2 dx (y - y1 ) - 2 dy (x - x1) <= -dx /\ I => -2v <= -dx, 
			// so 2*v >= dx is a sufficient condition for (S)
			v = v - dx;
			y = y + 1;
			// (y-y1) <= (x-x1)*dy/dx + 0.5 (S) /\ I
		}
		//
		// 2*v < dx /\ I, so (x-x1)*dy/dx + 0.5 < (y-y1) + 1
		// and (y-y1) <= (x-x1)*dy/dx + 0.5, 
		// so
		// (y-y1) <= (x-x1)*dy/dx + 0.5 < (y-y1) + 1
		// where (x,y) is the valid plot data
		if      (dy0>=0 && abs(dy0)<=dx0) { xp = xb + x; yp = yb + y; }
		else if (dy0>=0 && abs(dy0)> dx0) { xp = xb + y; yp = yb + x; }
		else if (dy0<0  && abs(dy0)<=dx0) { xp = xb + x; yp = yb - y; }
		else if (dy0<0  && abs(dy0)> dx0) { xp = xb + y; yp = yb - x; }
		plot(xp, yp, r, g, b);
		//
		v = v + dy;
		x = x + 1;
		// Invariant I holds
	}
};


// next method, see <>
void TPlot::drawCircle( int x0,int y0, int radius,int r,int g, int b )
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

	if (radius<=0) return;

    plot(x0, y0 + radius, r, g, b);
    plot(x0, y0 - radius, r, g, b);
    plot(x0 + radius, y0, r, g, b);
    plot(x0 - radius, y0, r, g, b);
 
    while(x < y) 
    {
        if(f >= 0) 
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;    
        plot(x0 + x, y0 + y, r, g, b);
        plot(x0 - x, y0 + y, r, g, b);
        plot(x0 + x, y0 - y, r, g, b);
        plot(x0 - x, y0 - y, r, g, b);
        plot(x0 + y, y0 + x, r, g, b);
        plot(x0 - y, y0 + x, r, g, b);
        plot(x0 + y, y0 - x, r, g, b);
        plot(x0 - y, y0 - x, r, g, b);
    }
	

	if (radius==1)
	{
		   plot(x0, y0, r, g, b);
		   plot(x0 + 1, y0 + 1, r, g, b);
		   plot(x0 + 1, y0 - 1, r, g, b);
		   plot(x0 - 1, y0 + 1, r, g, b);
		   plot(x0 - 1, y0 - 1, r, g, b);
	}
}

// next method, see https://gist.github.com/bert/1085538
void TPlot::drawEllipse( int x0,int y0, int x1, int y1, int rr, int gg, int bb)
{
	int a = abs (x1 - x0);
   int b = abs (y1 - y0); 
   int b1 = b & 1; /* values of diameter */
   long dx = 4 * (1 - a) * b * b;
   long dy = 4 * (b1 + 1) * a * a; /* error increment */
   long err = dx + dy + b1 * a * a;
   long e2; /* error of 1.step */

   if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
   if (y0 > y1) y0 = y1; /* .. exchange them */
   y0 += (b + 1) / 2;
   y1 = y0-b1;   /* starting pixel */
   a *= 8 * a; b1 = 8 * b * b;
   do
   {
       plot (x1, y0, rr, gg, bb); /*   I. Quadrant */
       plot (x0, y0, rr, gg, bb); /*  II. Quadrant */
       plot (x0, y1, rr, gg, bb); /* III. Quadrant */
       plot (x1, y1, rr, gg, bb); /*  IV. Quadrant */
       e2 = 2 * err;
       if (e2 >= dx)
       {
          x0++;
          x1--;
          err += dx += b1;
       } /* x step */
       if (e2 <= dy)
       {
          y0++;
          y1--;
          err += dy += a;
       }  /* y step */ 
   } while (x0 <= x1);
   while (y0-y1 < b)
   {  /* too early stop of flat ellipses a=1 */
       plot (x0-1, y0, rr, gg, bb); /* -> finish tip of ellipse */
       plot (x1+1, y0, rr, gg, bb); y0++;
       plot (x0-1, y1, rr, gg, bb);
       plot (x1+1, y1, rr, gg, bb); y1--;
   }
}
