#ifndef TPLOT_H
#define TPLOT_H

#include "TRender.h"

class TPlot
{  

public:  
		TPlot(TRender* renderer);
		~TPlot();

		void plot(int x, int y, int r, int g, int b);
		void drawLine(int xb, int yb, int xe, int ye, int r, int g, int b);
		void drawCircle( int x0,int y0, int radius,int r,int g, int b );
		void drawEllipse( int x0,int y0, int x1, int y1, int r, int g, int b);

private:
		TRender* mRenderer;
};

#endif
