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


#include "common.h"

#include <string.h>

static void read_tga_header(FILE *f,  tga_header & header);

/* write number in little endian*/
static void tga_write(FILE * f, unsigned number, int bytes)
{
	unsigned char c;
	for (int i=0;i<bytes;i++) {
		c = (number >> (i*8)) & 0xFF;
		fwrite(&c,1,1,f);
	}
}



static unsigned read_number(FILE*f, int bytes) {
	unsigned number = 0;
	unsigned char byte;

	for (int i=0;i<bytes;i++) {
		fread(&byte, 1, 1, f);
		number += byte << (i*8);
	}
	return number;
}

/**
 * can load 8bpp coloured image stored in TGA format. Basically what I can write, I can read.
 */
int load_tga_image(FILE *f, palette_image & image)
{
	tga_header header;
	read_tga_header(f, header);

	if (header.bitsperpixel != 8) {
		ERROR("Loader requires 8 bits per pixel in tga image.\n");
		return 0x0;
	}		
	if (header.colourmaptype != 1) {
		ERROR("TGA image does not contain palette.\n");
		return 0x0;
	}		
	if (header.datatypecode != TGA_TYPE_MAPPED) {
		ERROR("TGA image is not paletted image.\n");
		return 0x0;
	}		
	if (header.idlength != 0) {
		ERROR("TGA identity text length is not zero.\n");
		return 0x0;
	}		
	if (header.colourmapdepth != 24) {
		ERROR("Palette of TGA image is not 24 bits per item (1 byte for red, 1 for green, 1 for blue).\n");
		return 0x0;
	}
		
	// load palette
	image.palette_size = header.colourmaplength;	
	image.palette = new color[image.palette_size];
	
	memset(image.palette, 0, sizeof(color) * image.palette_size);
	
	for (unsigned i=0;i<image.palette_size;i++) {
		image.palette[i].blue = read_number(f, 1);
		image.palette[i].green = read_number(f, 1); 
		image.palette[i].red = read_number(f, 1);
	}
	
	image.width = header.width;
	image.height = header.height;

	int length = image.width * image.height;
	
	image.data = new unsigned[length];

	if (header.imagedescriptor & 0x20) {
		unsigned * data = image.data;
		for (int i=0;i<length;i++) {
			data[i] = read_number(f, 1);
		}
	} else {
		unsigned * data = image.data;
		for (int line=image.height-1; line>=0; line--)
			for (int pixel=0; pixel<image.width; pixel++)
				data[line*image.width + pixel] = read_number(f, 1);
	}
	//check maximum colour
/*
	int usage[256];
	memset(usage, 0, 256 * sizeof(int));
	
	int max_colour = 0;
	for (int i=0; i<length; i++) {
		
		usage[image.data[i]]++;
		if (image.data[i] > max_colour)
			max_colour = image.data[i];
	}
	
	for (int i=0; i<256; i++)
		printf("Color index %3d usage %4d\n", i, usage[i]);
	
*/	
	
	image.palette_size = 0x80;
	
	return 0x1;
}



static void write_tga_header(FILE *f, tga_header & header) 
{
	tga_write(f, header.idlength, 1);
	tga_write(f, header.colourmaptype, 1);
	tga_write(f, header.datatypecode, 1);
	tga_write(f, header.colourmaporigin, 2);
	tga_write(f, header.colourmaplength, 2);
	tga_write(f, header.colourmapdepth, 1);
	tga_write(f, header.x_origin, 2);
	tga_write(f, header.y_origin, 2);
	tga_write(f, header.width, 2);
	tga_write(f, header.height, 2);
	tga_write(f, header.bitsperpixel, 1);
	tga_write(f, header.imagedescriptor, 1);
}

static void read_tga_header(FILE *f, tga_header & header) 
{
	header.idlength = read_number(f, 1);
	header.colourmaptype = read_number(f, 1);
	header.datatypecode = read_number(f, 1);
	header.colourmaporigin = read_number(f, 2);
	header.colourmaplength = read_number(f, 2);
	header.colourmapdepth = read_number(f, 1);
	header.x_origin = read_number(f, 2);
	header.y_origin = read_number(f, 2);
	header.width = read_number(f, 2);
	header.height = read_number(f, 2);
	header.bitsperpixel = read_number(f, 1);
	header.imagedescriptor = read_number(f, 1);
}


int write_color_mapped_tga(FILE *f, palette_image &image) 
{
	// this will be uncompressed
	tga_header header;

	header.idlength = 0;
	header.colourmaptype = 1;
	header.datatypecode = TGA_TYPE_MAPPED ;
	header.colourmaporigin = 0;
	header.colourmaplength = image.palette_size;
	header.colourmapdepth = 24;
	header.x_origin = 0;
	header.y_origin = 0;
	header.width = image.width;
	header.height = image.height;
	header.bitsperpixel = 8;
	header.imagedescriptor = TGA_IMGDEC_ORIGIN_UPPER_LEFT ; 

	write_tga_header(f, header);
	
	// palette in BGR format
	for (unsigned i=0;i<image.palette_size;i++) 
	{
		tga_write(f,image.palette[i].blue, 1);
		tga_write(f,image.palette[i].green, 1);
		tga_write(f,image.palette[i].red, 1);
	}

	// now actual data
	for (unsigned i=0;i<image.width*image.height; i++)
		tga_write(f, image.data[i], 1);

	return 1;
}


/**
 * Write uncompressed RGB image.
 */
void write_uncompressed_tga(FILE *f, palette_image &image)
{
	// this will be uncompressed
	tga_header header;
	
	memset(&header, 0, sizeof(tga_header));
	header.datatypecode = TGA_TYPE_COLOR;
	header.width = image.width;
	header.height = image.height;
	header.bitsperpixel = 24;

	write_tga_header(f, header);
	
	for (unsigned i=0;i<image.width*image.height; i++)
	{
		tga_write(f,image.palette[image.data[i]].blue, 1);
		tga_write(f,image.palette[image.data[i]].green, 1);
		tga_write(f,image.palette[image.data[i]].red, 1);
	}
}

