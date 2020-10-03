#pragma warning(disable:4996)

#include <stdio.h>
#include "TDisplay.h"

HWND window; 
HDC dc;


/*
 * Calculates the average of two rgb32 pixels.
 */
inline static uint32_t avg(uint32_t a, uint32_t b)
{
    return (((a^b) & 0xfefefefeUL) >> 1) + (a&b);
}

/*
 * Calculates the average of two rgb32 pixels.
 */
inline static uint32_t avg4(const uint32_t* a, const uint32_t* b)
{
    return avg(avg(a[0], a[1]), avg(b[0], b[1]));
}


TDisplay::TDisplay(int width, int height)
{
	
	displayWidth = width;
	displayHeight = height;
	displayBuffer = (COLORREF*) calloc(displayWidth*displayHeight, sizeof(COLORREF));
	valid = true;
}

TDisplay::~TDisplay()
{

}

//next function is taken from internet (stackoverflow)
void TDisplay::display(COLORREF* colordata, int frameWidth, int frameHeight)
{
	COLORREF* screenBuffer;

	/* assert(displayBuffer); */
	if (frameWidth == displayWidth && frameHeight == displayHeight)
	{
		valid = true;
		screenBuffer = colordata;
	}
	else if (frameWidth == 2*displayWidth && frameHeight == 2*displayHeight)
	{
		uint32_t* src_row0;
		uint32_t* src_row1;

		valid = true;
		// FSAA: Full Screen Anti Aliasing (4x)
		for (int j=0 ; j<displayHeight ; j++)
		{
			src_row0 = (uint32_t *) &colordata[j*2*frameWidth];
			src_row1 = (uint32_t *) &colordata[(j*2+1)*frameWidth];
			for (int i=0 ; i<displayWidth ; i++, src_row0 +=2, src_row1 += 2)
			{
				//uint32_t v0 = avg( *src_row0, *(src_row0+1) );
				//uint32_t v1 = avg( *src_row1, *(src_row1+1) );
				uint32_t v = avg4( src_row0, src_row1 );
				displayBuffer[displayWidth*j+i]= (COLORREF) v;
				/*
				for (int u=0;u<2;u++)
					for (int v=0;v<2;v++)
					{
						COLORREF v0 = colordata[j*2*frameWidth+2*i];
						COLORREF v1 = colordata[j*2*frameWidth+2*i+u];
						COLORREF v2 = colordata[(2*j+v)*frameWidth+2*i];
						COLORREF v3 = colordata[(2*j+v)*frameWidth+2*i+u];
						unsigned int v_r = ((unsigned int) GetRValue(v0) + (unsigned int) GetRValue(v1) + (unsigned int) GetRValue(v2) + (unsigned int) GetRValue(v3)) >> 2;
						unsigned int v_g = ((unsigned int) GetGValue(v0) + (unsigned int) GetGValue(v1) + (unsigned int) GetGValue(v2) + (unsigned int) GetGValue(v3)) >> 2;
						unsigned int v_b = ((unsigned int) GetBValue(v0) + (unsigned int) GetBValue(v1) + (unsigned int) GetBValue(v2) + (unsigned int) GetBValue(v3)) >> 2;
						BYTE b_r = (BYTE) v_r;
						BYTE b_g = (BYTE) v_g;
						BYTE b_b = (BYTE) v_b;				
						displayBuffer[displayWidth*j+i]= RGB(b_r,b_g,b_b);
					}
					*/
			}
		}
		screenBuffer = displayBuffer;
	}
	else if (valid)
	{
		printf("Display [%d x %d] cannot handle frameWidth (=%d) x frameHeight (=%d)\n", displayWidth, displayHeight, frameWidth, frameHeight);
		valid = false;
		return;
	}
	else
	{
		// no Display; printf warning was generated in a previous iteration (and it will not be repeated). 
		return;
	}

	window = GetActiveWindow(); 
	dc = GetDC(window);
	// Creating temp bitmap
	HBITMAP map = CreateBitmap(displayWidth, // width
							   displayHeight, // height
							   1,   // 
							   32, // bitdepth
							   (void*) screenBuffer); // pointer to array of COLORREF
	// Temp HDC to copy picture
	HDC src = CreateCompatibleDC(dc); // hdc - Device context for window, I've got earlier with GetDC(hWnd) or GetDC(NULL);
	SelectObject(src, map); // Inserting picture into our temp HDC
	// Copy image from temp HDC to window
	BitBlt(dc, // Destination
		   0,  // x and
		   0,  // y - upper-left corner of place, where we'd like to copy
		   displayWidth, // width of the region
		   displayHeight, // height
		   src, // source
		   0,   // x and
		   0,   // y of upper left corner  of part of the source, from where we'd like to copy
		   SRCCOPY); // Defined DWORD to juct copy pixels. Watch more on msdn;
	DeleteObject(map);
	DeleteDC(src); // Deleting temp HDC
}



/*
 * Calculates the average of two rows of rgb32 pixels.
 */
/*
void average2Rows(const uint32_t* src_row1, const uint32_t* src_row2, uint32_t* dst_row, int w)
{
#if !defined(__SSE)
        for (int x = w; x; --x, dst_row++, src_row1 += 2, src_row2 += 2)
            * dst_row = avg(src_row1, src_row2);
#else
        for (int x = w; x; x-=4, dst_row+=4, src_row1 += 8, src_row2 += 8)
        {
            __m128i left  = _mm_avg_epu8(_mm_load_si128((__m128i const*)src_row1), _mm_load_si128((__m128i const*)src_row2));
            __m128i right = _mm_avg_epu8(_mm_load_si128((__m128i const*)(src_row1+4)), _mm_load_si128((__m128i const*)(src_row2+4)));
			__m128i t0 = _mm_unpacklo_epi32( left, right ); // right.m128i_u32[1] left.m128i_u32[1] right.m128i_u32[0] left.m128i_u32[0]
			__m128i t1 = _mm_unpackhi_epi32( left, right ); // right.m128i_u32[3] left.m128i_u32[3] right.m128i_u32[2] left.m128i_u32[2]
			__m128i shuffle1 = _mm_unpacklo_epi32( t0, t1 );    // right.m128i_u32[2] right.m128i_u32[0] left.m128i_u32[2] left.m128i_u32[0]
			__m128i shuffle2 = _mm_unpackhi_epi32( t0, t1 );    // right.m128i_u32[3] right.m128i_u32[1] left.m128i_u32[3] left.m128i_u32[1]
            _mm_store_si128((__m128i *)dst_row, _mm_avg_epu8(shuffle1, shuffle2));
        }
#endif
}
*/


/*
 * Calculates the average of four rgb32 pixels.
 */

/*
inline static uint32_t avg(const uint32_t a[2], const uint32_t b[2])
{
    return avg(avg(a[0], a[1]), avg(b[0], b[1]));
}
*/