/*
 * Demostration source for various formats used in Civilization I for Windows.
 * Copyright (c) 2008, by Honza Havlicek
 * 
 * Contact: havlicek.honza@gmail.com
 *
 * Permission is given by the author to freely redistribute and include
 * this code in any program as long as this credit is given where due. 
 *
 * It also took a very long time to program this, so if you like it or use it, 
 * please send me a note.
 *
 * Release notes: This file contains a routines for TGA file loading and saving.
 * Only TGA with palette (8 bits per pixel, 24 bits per colour in palette) are accepted.
 */
#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdio.h>


#define ERROR(fmt, args...) fprintf(stderr, "%s:%d - "fmt, __func__, __LINE__, ##args);

struct color {
	unsigned char red, green, blue;
};

struct dictionary_entry {
	int prefix_bitcode;
	unsigned color_index;
	int length;
};


/**
 * Structure for holding paletted image, maximum o
 *
 */
struct palette_image {
	unsigned width;
	unsigned height;
	unsigned palette_size;
	struct color *palette;
	unsigned *data;
};
	

struct tga_header {
   char  idlength;
   char  colourmaptype;
   char  datatypecode;
   short int colourmaporigin;
   short int colourmaplength;
   char  colourmapdepth;
   short int x_origin;
   short int y_origin;
   short width;
   short height;
   char  bitsperpixel;
   char  imagedescriptor;
};

// types in datatypecode
#define TGA_TYPE_MAPPED      1
#define TGA_TYPE_COLOR       2
#define TGA_TYPE_GRAY        3
#define TGA_TYPE_MAPPED_RLE  9
#define TGA_TYPE_COLOR_RLE  10
#define TGA_TYPE_GRAY_RLE   11


// flags for imagedescriptor
/* if zero, origin in lower left corner*/
#define TGA_IMGDEC_ORIGIN_UPPER_LEFT (1 << 5)	



void write_uncompressed_tga(FILE *f, struct palette_image & image);
int write_color_mapped_tga(FILE *f, struct palette_image & image);

int load_tga_image(FILE *f, struct palette_image & image);

int tga2gif(const char * in_name, const char * out_name);
int palette_image2gif(struct palette_image &image, FILE *out);


int gif2tga(FILE *gif, FILE *tga);
int gif2tga(const char *gif, const char *tga);
int gif2palette_image(FILE *f, struct palette_image & image);

#endif
