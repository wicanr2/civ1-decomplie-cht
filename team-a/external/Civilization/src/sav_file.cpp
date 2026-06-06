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
 * Release notes:  This file contains loader and saver for SAV file from 
 * Civilization I for Windows. The file is basically completely undocumented yet.
 * The SAV file is using RLE compression, there is alway control byte and then data
 *     [ length ]  [ byte ] - put *[length]* bytes with value *[byte]* to output
 *     [ length | 0x80 ] [length bytes]  = copy [length bytes] from input file to output
 */

#define NDEBUG

#include <stdio.h> 
#include <conio.h> 
#include <assert.h> 

#include "common.h"
#include "sav_file.h"


 
int RLE_decode(const char *in, const char * out) 
{
	FILE *in_f, *out_f;
	
	if ((in_f = fopen(in, "rb")) == NULL) 
	{
		ERROR("Unable to open input file %s", in);
		return 0;
	}
	if ((out_f = fopen(out, "wb")) == NULL) 
	{
		ERROR("Unable to open output file %s", out);
		fclose (in_f);
		return 0;
	}
	
	RLE_decode(in_f, out_f);
	
	fclose(in_f);
	fclose(out_f);
	
	return 1;
} 

/**
 * Basically no error checking
 */
int RLE_decode(FILE *in, FILE*out) 
{
	int c, r;
	while ((c = getc(in)) != -1)
	{
		
		if (c < 0x80)	// repeat byte following byte "c" , "c" times to result
		{
			
			//printf("%d\n", c);
			r = getc(in);
			while (c-- > 0)
			{
				putc(r, out);
			}
		}
		else			// copy following "c - 0x80" bytes from encoded to result
		{
			c = c - 0x80;
			while (c-- > 0) 
			{
				r = getc(in);
				putc(r, out);
			}
		}
	}
	return 1;
}








int RLE_encode(const char *in, const char * out) 
{
	FILE *in_f, *out_f;
	
	if ((in_f = fopen(in, "rb")) == NULL) 
	{
		ERROR("Unable to open input file %s", in);
		return 0;
	}
	if ((out_f = fopen(out, "wb")) == NULL) 
	{
		ERROR("Unable to open output file %s", out);
		fclose (in_f);
		return 0;
	}
	
	RLE_encode(in_f, out_f);
	
	fclose(in_f);
	fclose(out_f);
	
	return 1;
} 

// TODO: unoptimalized and NOT a stream friendly... but works

/**
 * 
 */
static int find_run(FILE *f) 
{
	int c, p = -1, num;
	long cur_pos = ftell(f);
	
	if ((p = getc(f)) == -1) 
	{
		fseek(f, cur_pos, SEEK_SET);
		return 0;
	}

	num = 1;
	while ((c = getc(f)) != -1 && c == p && num < 127) 
	{
		num++;
	}
	fseek(f, cur_pos, SEEK_SET);
	return num;	
}

/**
 * Return number of same bytes from the current position
 */
static int find_block(FILE *f) 
{
	int c, p = -1, num;
	long cur_pos = ftell(f);
	
	if ((p = getc(f)) == -1) {
		fseek(f, cur_pos, SEEK_SET);
		return 0;
	}
	num = 1;
	
	while ((c = getc(f)) != -1 && c != p && num < 127) 
	{
		p = c;
		num++;
	}

	fseek(f, cur_pos, SEEK_SET);
	return num;	
}
/* RLE encoding:
 *    <count> <value>                      # 'run' of count+1 equal pixels
 *    <count | 0x80> <count+1 data bytes>  # count+1 non-equal pixels
 *
 * count can range between 0 and 127
 *
 * returns length of the rleline vector
 */
int RLE_encode(FILE *in, FILE *out) {
	int run, block;	
	
	int c;
	while ((run = find_run(in)) > 0)	
	{
		
		if (run <2) 
		{
			block = find_block(in);
			printf("Block size %X\n", block + 0x80);
			unsigned char c = (unsigned char) block;
			
			assert(c == block);
			assert(block < 128);
			
			c |= 0x80;
			fwrite (&c,1,1,out);
			
			for (int i = 0; i < block; i++)
			{
				fread(&c,1,1,in);
				fwrite(&c,1,1,out);
			}
		} 
		else
		{
			printf("Run size %d\n", run);
			unsigned char c = (unsigned char) run;
			assert(c == run);
			assert(run < 128);
			fwrite(&c,1,1,out);
			for (int i=0; i<run; i++)
				c = (unsigned char)getc(in);
			fwrite(&c,1,1,out);
		}
//		getch();		
	}
} 
 
/*int RLE_encode(FILE *in, FILE *out)
{
	int prev_byte, cur_byte, limit = 0, block[256], blocklen;

	prev_byte = -1;
	blocklen = 0;	
	runlen = 0;
	
	while ((byte = getc(in)) == -1) {
		
		if (!blocklen) {				// 
			isrun = 1;
			runlen = 1;
			runcolour = cur_byte;
		} else 
		
		if (isrun) {				// run
			if (cur_byte == prev_byte) {
				runlen++;
			} else {
				if (runlen > 1)	{	// run is at least 2 bytes long
					output(runlen); // XXX: runlen-1?
					output(runcolour);
					runlen = 1;
					runcolour = cur_byte;
				} else {
					isrun = 0;
					block[]
				}
			}
		} else 
		{					// not a run
			if (prev_byte == cur_byte) {
				
			}
				
		}
		
		
		// if block is too big, flush it			
	}
	
	// output last block

	return 0;
}

*/













