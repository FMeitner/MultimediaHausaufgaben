#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"

void manipulate(bitmap_pixel_rgb_t* pixels, int count, int l, int LoD) //integer l = change specified in main
{
	for (int i = 0; i < count; i++)
	{
		bitmap_pixel_rgb_t* pixel = &pixels[i];
		if(LoD == 0) //check for brightening/darkening
		{
		 if((int)(pixel->r)+l > 255 || (int)pixel->b+l > 255 || (int)pixel->g+l > 255) //check for overflow when brightening
		 {
			int ll = 42; //value for adjusting brightness to the maximum amount without overflowing
			if(pixel->r >= pixel->b && pixel->r >= pixel->g)
			{
				ll = 255 - pixel->r;
			} else if(pixel->b >= pixel->r && pixel->b >= pixel->g) {
				ll = 255 - pixel->b;
			} else {
				ll = 255 - pixel->g;
			}
			pixel->r += ll;
			pixel->b += ll;
			pixel->g += ll;
		 } else {
			 pixel->r += l;
			 pixel->g += l;
			 pixel->b += l;
		 }
		} else {
			
		 if((int)(pixel->r)-l < 0 || (int)pixel->b-l < 0 || (int)pixel->g-l < 0) //check for underflow when darkening
		 {
			int ll = 42; //value for adjusting brightness to the maximum amount without underflowing
			if(pixel->r <= pixel->b && pixel->r <= pixel->g)
			{
				ll = pixel->r;
			} else if(pixel->b <= pixel->r && pixel->b <= pixel->g) {
				ll = pixel->b;;
			} else {
				ll = pixel->g;
			}
			pixel->r -= ll;
			pixel->b -= ll;
			pixel->g -= ll;
		 } else {
			 pixel->r -= l;
			 pixel->g -= l;
			 pixel->b -= l;
		 }
		}
	}
}

int main(int args, char** argsv)
{
	// Read the bitmap pixels.
	bitmap_error_t error;    //int error
	int width, height;
	bitmap_pixel_rgb_t* pixels; //pointer auf struct aus 4 integers
	char* manipbitmap = argsv[1];
	error = bitmapReadPixels(
		manipbitmap,
		(bitmap_pixel_t**)&pixels, 
		&width,				//referenz auf int
		&height,
		BITMAP_COLOR_SPACE_RGB  //0
	);

	assert(error == BITMAP_ERROR_SUCCESS);
	printf("Bitmap dimensions: %d x %d\n", width, height);
	int tmp = atoi(argsv[3]);
	int LoD = 42;  //variable for brightening/darkening -> 0 = brighten | !0 = darke
	int change = 42;  // variable for amount of brightness change
	if(tmp > 0)
	{
		LoD = 0;
		change = tmp;
	}
	else 
	{
		LoD = 1;
		change = tmp * (-1);
	}
	
	// Manipulate the pixels.
	manipulate(pixels, width * height, change, LoD);

	// Write the pixels back.
	bitmap_parameters_t params =
	{
		// Is this bitmap bottom-up?
		.bottomUp = BITMAP_BOOL_TRUE,

		// Width in pixels:
		.widthPx = width,

		// Height in pixels:
		.heightPx = height,

		// Which color depth is used?
		.colorDepth = BITMAP_COLOR_DEPTH_24,

		// Which compression is used?
		.compression = BITMAP_COMPRESSION_NONE,

		// Which kind of DIB header is used?
		.dibHeaderFormat = BITMAP_DIB_HEADER_INFO,

		// The color space the user provides:
		.colorSpace = BITMAP_COLOR_SPACE_RGB
	};
	if(LoD == 0) //Check for brightening/darkening -> creating file with according name
	{
	error = bitmapWritePixels(
		"sails_lighter.bmp",
		BITMAP_BOOL_TRUE,
		&params,
		(bitmap_pixel_t*)pixels
	);
	} else {
		error = bitmapWritePixels(
		"sails_darker.bmp",
		BITMAP_BOOL_TRUE,
		&params,
		(bitmap_pixel_t*)pixels
	);
	}

	assert(error == BITMAP_ERROR_SUCCESS);

	// Free the memory that has been allocated by the bitmap library.
	free(pixels);
	return 0;
}
