// FROM PONG EXAMPLE https://github.com/dimok789/pong_port

#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include "draw.h"
#include "dynamic_libs/os_functions.h"

void flipBuffers()
{
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int buf1_size = OSScreenGetBufferSizeEx(1);
	//Flush the cache
	DCFlushRange((void *)0xF4000000 + buf0_size, buf1_size);
	DCFlushRange((void *)0xF4000000, buf0_size);
	//Flip the buffer
	OSScreenFlipBuffersEx(0);
	OSScreenFlipBuffersEx(1);
}

void drawString(int x, int y, char * string)
{
	// Bad hacky way of scaling display 0 (tv) to the correct size
	OSScreenPutFontEx(0, x * 1.6f, y * 1.6f, string);
	OSScreenPutFontEx(1, x, y, string);
}

void fillScreen(char r, char g, char b, char a)
{
	uint32_t num = (r << 24) | (g << 16) | (b << 8) | a;
	OSScreenClearBufferEx(0, num);
	OSScreenClearBufferEx(1, num);
}

//Rendering in
void drawPixel(int x, int y, char r, char g, char b, char a)
{
	uint32_t num = (r << 24) | (g << 16) | (b << 8) | a;
	// OSScreenPutPixelEx(0,x*2,y*2,num);
	// OSScreenPutPixelEx(1,x*2,y*2,num);
	// OSScreenPutPixelEx(0,x*2+1,y*2,num);
	// OSScreenPutPixelEx(1,x*2+1,y*2,num);
	// OSScreenPutPixelEx(0,x*2,y*2+1,num);
	// OSScreenPutPixelEx(1,x*2,y*2+1,num);
	// OSScreenPutPixelEx(0,x*2+1,y*2+1,num);
	// OSScreenPutPixelEx(1,x*2+1,y*2+1,num);

	OSScreenPutPixelEx(0, x * 1.6f, y * 1.6f, num);
	OSScreenPutPixelEx(1, x, y, num);
	
	//Code to write to framebuffer directly. For some reason this is only writing to one of the framebuffers when calling flipBuffers. Should provide speedup but needs investigation.
	/*
	unsigned int coreinit_handle;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
	unsigned int(*OSScreenGetBufferSizeEx)(unsigned int bufferNum);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenGetBufferSizeEx", &OSScreenGetBufferSizeEx);

	void(*memcpy)(void* dest, void* src, int length);
	OSDynLoad_FindExport(coreinit_handle, 0, "memcpy", &memcpy);
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int height = 1024;
	int width = 1280;
	char *screen = (void *)0xF4000000;
	uint32_t v=(x + y*width)*4;
	screen[v]=r;
	screen[v+1]=g;
	screen[v+2]=b;
	screen[v+3]=a;

	height = 480;
	width = 896;
	char *screen2 = (void *)0xF4000000 + buf0_size;
	v=(x + y*width)*4;
	screen2[v]=r;
	screen2[v+1]=g;
	screen2[v+2]=b;
	screen2[v+3]=a;
	*/
}

// From https://www.freecodecamp.org/news/how-to-code-your-first-algorithm-draw-a-line-ca121f9a1395/
void drawLine(int x1, int y1, int x2, int y2, char r, char g, char b, char a)
{
	int x = 0;
	int y = 0; 
	int xe = 0;
	int ye = 0;

	int dx = x2 - x1;
	int dy = y2 - y1;

	int adx = std::abs(dx);
	int ady = std::abs(dy);

	// Calculate error intervals for both axes
    int px = 2 * ady - adx;
    int py = 2 * adx - ady;

	if(ady <= adx) {
		// Line is drawn left to right
		if (dx >= 0) {
            x = x1; y = y1; xe = x2;
        } else { // Line is drawn right to left (swap ends)
            x = x2; y = y2; xe = x1;
        }

		// Draw first pixel
		drawPixel(x, y, r, g, b, a); 
        
        // Rasterize the line
        for (int i = 0; x < xe; i++) {
            x = x + 1;
            
            // Deal with octants...
            if (px < 0) {
                px = px + 2 * ady;
            } else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    y = y + 1;
                } else {
                    y = y - 1;
                }
                px = px + 2 * (ady - adx);
            }
            
            // Draw pixel from line span at currently rasterized position
            drawPixel(x, y, r, g, b, a);
        }
	} else { // The line is Y-axis dominant
        
        // Line is drawn bottom to top
        if (dy >= 0) {
            x = x1; y = y1; ye = y2;
        } else { // Line is drawn top to bottom
            x = x2; y = y2; ye = y1;
        }
        
		// Draw first pixel
        drawPixel(x, y, r, g, b, a);
        
        // Rasterize the line
        for (int i = 0; y < ye; i++) {
            y = y + 1;
            
            // Deal with octants...
            if (py <= 0) {
                py = py + 2 * adx;
            } else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    x = x + 1;
                } else {
                    x = x - 1;
                }
                py = py + 2 * (adx - ady);
            }
            
            // Draw pixel from line span at currently rasterized position
            drawPixel(x, y, r, g, b, a);
        }
    }
}

void drawRect(int x1, int y1, int x2, int y2, char r, char g, char b, char a)
{
	drawLine(x1, y1, x2, y1, r, g, b, a);
	drawLine(x2, y1, x2, y2, r, g, b, a);
	drawLine(x1, y2, x2, y2, r, g, b, a);
	drawLine(x1, y1, x1, y2, r, g, b, a);
}

void drawFillRect(int x1, int y1, int x2, int y2, char r, char g, char b, char a)
{
	int X1, X2, Y1, Y2, i, j;

	if (x1 < x2){
		X1 = x1;
		X2 = x2;
	}
	else {
		X1 = x2;
		X2 = x1;
	}

	if (y1 < y2){
		Y1 = y1;
		Y2 = y2;
	}
	else {
		Y1 = y2;
		Y2 = y1;
	}
	for (i = X1; i <= X2; i++){
		for (j = Y1; j <= Y2; j++){
			drawPixel(i, j, r, g, b, a);
		}
	}
}

void drawTri(int x1, int y1, int x2, int y2, int x3, int y3, char r, char g, char b, char a) {
	// int ax[] = { x1, x2, x3 };
	// int ay[] = { y1, y2, y3 };
	// std::sort(ax, ax + 3);
	// std::sort(ay, ay + 3);
	// drawLine(ax[0], ay[0], ax[1], ay[1], r, g, b, a);
	// drawLine(ax[1], ay[1], ax[2], ay[2], r, g, b, a);
	// drawLine(ax[2], ay[2], ax[0], ay[0], r, g, b, a);

	drawLine(x1, y1, x2, y2, r, g, b, a);
	drawLine(x2, y2, x3, y3, r, g, b, a);
	drawLine(x3, y3, x1, y1, r, g, b, a);
}

void drawFillTri(int x1, int y1, int x2, int y2, int x3, int y3, char r, char g, char b, char a) {
	int ax[] = { x1, x2, x3 };
	int ay[] = { y1, y2, y3 };
	std::sort(ax, ax + 3);
	std::sort(ay, ay + 3);

	for(int i = ax[0]; i < ax[2]; i++) {
		for(int j = ay[0]; j < ay[2]; j++) {
			if(isInside(i, j, x1, y1, x2, y2, x3, y3)) {
				drawPixel(i, j, r, g, b, a);
			}
		}
	}
}

void drawCircle(int xCen, int yCen, int radius, char r, char g, char b, char a)
{
	int x = 0;
	int y = radius;
	int p = (5 - radius * 4) / 4;
	drawCircleCircum(xCen, yCen, x, y, r, g, b, a);
	while (x < y){
		x++;
		if (p < 0){
			p += 2 * x + 1;
		}
		else{
			y--;
			p += 2 * (x - y) + 1;
		}
		drawCircleCircum(xCen, yCen, x, y, r, g, b, a);
	}
}

void drawFillCircle(int xCen, int yCen, int radius, char r, char g, char b, char a)
{
	drawCircle(xCen, yCen, radius, r, g, b, a);
	int x, y;
	for (y = -radius; y <= radius; y++){
		for (x = -radius; x <= radius; x++)
			if (x*x + y*y <= radius*radius + radius * .8f)
				drawPixel(xCen + x, yCen + y, r, g, b, a);
	}
}

void drawCircleCircum(int cx, int cy, int x, int y, char r, char g, char b, char a)
{

	if (x == 0){
		drawPixel(cx, cy + y, r, g, b, a);
		drawPixel(cx, cy - y, r, g, b, a);
		drawPixel(cx + y, cy, r, g, b, a);
		drawPixel(cx - y, cy, r, g, b, a);
	}
	if (x == y){
		drawPixel(cx + x, cy + y, r, g, b, a);
		drawPixel(cx - x, cy + y, r, g, b, a);
		drawPixel(cx + x, cy - y, r, g, b, a);
		drawPixel(cx - x, cy - y, r, g, b, a);
	}
	if (x < y){
		drawPixel(cx + x, cy + y, r, g, b, a);
		drawPixel(cx - x, cy + y, r, g, b, a);
		drawPixel(cx + x, cy - y, r, g, b, a);
		drawPixel(cx - x, cy - y, r, g, b, a);
		drawPixel(cx + y, cy + x, r, g, b, a);
		drawPixel(cx - y, cy + x, r, g, b, a);
		drawPixel(cx + y, cy - x, r, g, b, a);
		drawPixel(cx - y, cy - x, r, g, b, a);
	}
}

// Gets the area of a triangle formed by points (x1,y1), (x2,y2) and (x3,y3)
float area(int x1, int y1, int x2, int y2, int x3, int y3)
{ 
   return abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0f);
} 
  
// A function to check whether point P(x, y) lies inside the triangle formed by A(x1, y1), B(x2, y2) and C(x3, y3)
bool isInside(int pX, int pY, int x1, int y1, int x2, int y2, int x3, int y3)
{    
   // Calculate area of triangle ABC
   float A = area (x1, y1, x2, y2, x3, y3); 
  
   // Calculate area of triangle PBC
   float A1 = area (pX, pY, x2, y2, x3, y3); 
  
   // Calculate area of triangle PAC
   float A2 = area (x1, y1, pX, pY, x3, y3); 
  
   // Calculate area of triangle PAB
   float A3 = area (x1, y1, x2, y2, pX, pY); 
    
   // Check if sum of A1, A2 and A3 is same as A
   return (A == A1 + A2 + A3); 
} 