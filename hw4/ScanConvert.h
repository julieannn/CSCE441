#ifndef SCAN_CONVERT_H
#define SCAN_CONVERT_H

#define ImageW 400
#define ImageH 400

// Clears framebuffer to black
void clearFramebuffer();

// Sets pixel x,y to the color RGB
void setFramebuffer(int x, int y, float R, float G, float B);

//sets the zbuffer
void setZbuffer(int x, int y, float z);

//returns the z value at that pixel
float getZbuffer(int x, int y);

#endif
