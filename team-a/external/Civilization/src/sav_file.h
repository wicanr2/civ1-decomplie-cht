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
 */
#ifndef __SAV_FILE__
#define __SAV_FILE__

// declaration
int RLE_decode(FILE *in, FILE *out);
int RLE_decode(const char *in, const char *out);

int RLE_encode(FILE *in, FILE *out);
int RLE_encode(const char *in, const char *out);

#endif
