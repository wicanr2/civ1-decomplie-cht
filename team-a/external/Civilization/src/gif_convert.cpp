/*
 * Demostration source for various formats used in Civilization I for Windows.
 * Copyright (c) 2008, by Honza Havlicek
 * 
 * Contact: havlicek.honza@gmail.com
 *
 * Permission is given by the author to freely redistribute and include
 * this code in any program as long as this credit is given where due. 
 *
 * It also took ages to decode, so if you like this or use this, 
 * please send me a note.
 *
 * Release notes: This file contains a decoder and encoder routines for GIF files 
 * used by game Civilization 1. The GIF files are not the standard GIF files
 * although they use LZW compression with variable bitwidth of bitcode, 
 * reset of dictionary bitcode and done bitcode.
 *
 * The structure of file is not properly defined in structure, but it should 
 * be easy to recognize by researching gif2palette_image()
 */
 
 
 
#include "common.h"

#include <string.h>
#include <conio.h>
#include <math.h>

#define DEBUG 0

/* Level of debug output:
 *   4: Everything, even colors and palette
 *   3: Show chunks, bitcodes and DONE and RESET bitcodes, also show what each bitcode does
 *   2: Show chunks, bitcodes and DONE and RESET bitcodes
 *   1: Output chunks and basic info about image
 *   0: Only stderr
 */



#define BITCODE_WIDTH_MAX	11

/**
 * I guess even C# doesn't have bitreader like this
 * It reads bits from passed FILE f. f should point at start of bitstream.
 * It's a civ I bitstream = it consists from chunks.
 *
 * Structure of bitstream:
 * 1 Byte                     = length_of_chunk_1
 * [length_of_chunk_1] Bytes  = data that are used in bitstream
 * 1 Byte                     = length_of_chunk_2
 * [length_of_chunk_2] Bytes  = data for bitstream
 * ....
 * 1 Byte                     = length_of_chunk_2
 * [length_of_chunk_2] Bytes  = data for bitstream
 * 1 Byte                     = 0 -> End of bitstream
 *
 * DO NOT USE ANYTHING ELSE TO MANIPULATE f
 */
class BitReader {
private: 
	FILE * f;
	int data_chunk_length;			//< how many bytes can we read before end of chunk
	unsigned read_bits;				//< buffer of read bits
	unsigned number_of_read_bits;

	/**
	 * Read one byte from file
	 */
	unsigned read_byte()
	{
		unsigned char c;
		fread(&c,1,1,f);
		return (unsigned)c;
	}
public:
	BitReader(FILE *file) {
		f = file;
		data_chunk_length = 0;
		read_bits = 0;
		number_of_read_bits = 0;
	}


	/*
	 * Read number that is "bytes" long. 
	 */
	unsigned read_number(int bytes) {
		unsigned number = 0;
		unsigned char byte;

		for (int i=0;i<bytes;i++) {
			fread(&byte, 1, 1, f);
			number += byte << ((bytes - 1 - i)*8);
		}
		return number;
	}

	/**
	 * Get bitcode from the stream
	 */
	int get_bitcode(unsigned bitcode_width) 
	{
		while (number_of_read_bits < bitcode_width+1)
		{
			if (data_chunk_length == 0) 
			{
				data_chunk_length = read_byte();
#if DEBUG >= 1
				printf("!! new chunk of data, size 0x%02X\n", data_chunk_length);
#endif
				// end of compressed data is indicated by data_chunk_length = zero
				if (data_chunk_length == 0)
				{
#if DEBUG >= 1
					printf("******Double data_chunk zero at 0x%X\n",int(ftell(f) - 1L));
#endif
					return 0;
				}
			} 
			else 
			{
				unsigned byte = read_byte();
				read_bits = (byte << number_of_read_bits) | read_bits;
				number_of_read_bits += 8;
#if DEBUG >= 2
				printf("!! (0x%02X)read compressed byte 0x%02X\n", data_chunk_length, byte);
#endif
				data_chunk_length--;
			}
		}

		// get bitcode from read_bits buffer
		unsigned bitcode_mask = ((2 << bitcode_width)-1);
		unsigned bitcode = read_bits & bitcode_mask;
#if DEBUG >= 2
		printf("Bitcode %X (mask 0x%04X)\n", bitcode, bitcode_mask);
#endif

		// empty bits we have used in bitcode from red_bits buffer
		read_bits >>= bitcode_width + 1;
		number_of_read_bits -= bitcode_width + 1;
		
		return bitcode;
	}
	
};



int gif2tga(const char *gif_filename, const char *tga_filename) {
	FILE *gif, *tga;
    if ((gif = fopen(gif_filename,"rb")) == NULL) 
    {
		fprintf(stderr, "Specified file (%s) is not present.\n", gif_filename);	
		_getch();
		return 0x0;
    }


    if ((tga = fopen(tga_filename, "wb")) == NULL) 
    {
		fclose(gif);
		fprintf(stderr, "Specified file (%s) is not present.\n", tga_filename);	
		_getch();
		return 0x0;
    }

	int ret = gif2tga(gif, tga);
	fclose(gif);
	fclose(tga);

	return ret;
}


int gif2tga(FILE *gif, FILE *tga) {
	struct palette_image image;
	if (!gif2palette_image(gif, image)) {
		fputs("Unable to load gif image.\n", stderr);
		return 0;
	}

	if (!write_color_mapped_tga(tga, image)) {
		fputs("Unable to write loaded gif image to tga.\n", stderr);
		return 0;
	}
	delete[] image.palette;
	delete[] image.data;

	image.palette = 0;
	image.width = 0;
	image.height = 0;

	return 0x1;
}


int gif2palette_image(FILE *f, struct palette_image & image)
{    
	BitReader * bit_reader = new BitReader(f);


	// image header
	unsigned image_width  = bit_reader->read_number(2);
	unsigned image_height = bit_reader->read_number(2);
	unsigned orig_bitcode_width = bit_reader->read_number(1);
	unsigned palette_size = bit_reader->read_number(1)+1;
#if DEBUG >= 1
	printf("Image width is 0x%04X\nimage height is 0x%04X\ninitial bitcode width %d\npalette_size %d\n\n", image_width, image_height, orig_bitcode_width, palette_size);
#endif

	// read palette
#if DEBUG >= 4
	printf("\nPalette (0x%02X):\n", palette_size);
#endif
	struct color *palette = new struct color[256];
	for (unsigned i=0;i<palette_size;i++) 
	{
		palette[i].red   = bit_reader->read_number(1);
		palette[i].green = bit_reader->read_number(1); 
		palette[i].blue  = bit_reader->read_number(1);
#if DEBUG >= 4
		printf(" %03d - r %03d g %03d b %03d\n", i, palette[i].red, palette[i].green, palette[i].blue);
#endif
	}

	// now we are at compressed data and the fun can begin
	// first some inicialization
	const unsigned RESET_DICTIONARY_BITCODE = 1 << orig_bitcode_width;
	const unsigned DONE_BITCODE             = (1 << orig_bitcode_width) + 1;
	unsigned prev_bitcode = RESET_DICTIONARY_BITCODE;
	unsigned bitcode = 0;
	unsigned next_bitcode = DONE_BITCODE + 1;
	unsigned bitcode_width = orig_bitcode_width;
	unsigned bitcode_mask = ((2 << orig_bitcode_width)-1);
#if DEBUG >= 1
	printf("\nRESET_DICTIONARY_BITCODE 0x%02X\n", RESET_DICTIONARY_BITCODE);
	printf("DONE_BITCODE             0x%02X\n", DONE_BITCODE);

	printf("prev_bitcode 0x%02X\n", prev_bitcode);
	printf("bitcode      0x%02X\n", bitcode);
	printf("next_bitcode 0x%02X\n", next_bitcode);
#endif
	unsigned *picture = new unsigned[image_width * image_height];
	unsigned *picture_cur = picture;
	unsigned *pixel;
	// fill in dictionary
	struct dictionary_entry *dictionary = new struct dictionary_entry[0x2000];

	// fill table with initial bitcodes that coresponds to the palette colors
	for (bitcode=0; bitcode < unsigned(1 << orig_bitcode_width); bitcode++)
	{
		dictionary[bitcode].color_index    = bitcode;
		dictionary[bitcode].prefix_bitcode = -1;
		dictionary[bitcode].length         = 1;
	}

	/*
	 * Main loop, always get a bitcode and act upon it
	 */
	while ( (bitcode = bit_reader->get_bitcode(bitcode_width)) != DONE_BITCODE)
	{
#if DEBUG >= 3
	    printf("\n");
#endif
		// Read bitcode
		if (bitcode == RESET_DICTIONARY_BITCODE)
		{
#if DEBUG >= 2
			printf("Got RESET_DICTIONARY_BITCODE(%X)\n", bitcode);
#endif
			prev_bitcode  = RESET_DICTIONARY_BITCODE;
			next_bitcode  = DONE_BITCODE + 1;
			bitcode_width = orig_bitcode_width;
			bitcode_mask  = ((2 << orig_bitcode_width)-1);
			// it's not necessary to initialize dictionary
			continue;
		}

		//now for the real stuff: bitcode to pixels

		// first case: immediately after reset of dictionary, you cannot add a new bitcode, since prefix doesn't exist
		if (prev_bitcode == RESET_DICTIONARY_BITCODE)
		{
#if DEBUG >= 3
			printf(" 1: Previous bitcode was RESET_DICTIONARY_BITCODE\n *** length %d\n", dictionary[bitcode].length);
#endif
			// since dictionary was reseted, only original bitcodes are valid. 
			// The original bitcodes represents only themself
			prev_bitcode = bitcode;
			*picture_cur++ = dictionary[bitcode].color_index;
#if DEBUG >= 4
			printf("  1: Color %02X\n", dictionary[bitcode].prefix_bitcode);
#endif
			continue;	// alright, we are finished with this bitcode
		}
		dictionary[next_bitcode].length = dictionary[prev_bitcode].length + 1;
		dictionary[next_bitcode].prefix_bitcode = prev_bitcode;

		// second case: bitcode is already in the dictionary
		if (bitcode < next_bitcode)
		{
#if DEBUG >= 3
			printf(" 2: Bitcode is in dictionary\n *** length %d\n", dictionary[bitcode].length);
#endif
			picture_cur += dictionary[bitcode].length;
			pixel = picture_cur;
			
			prev_bitcode = bitcode;
			// fill in colors from bitcode dictionary word
			do
			{
				pixel--;
#if DEBUG >= 4
				printf("  1: Color %02X\n", dictionary[bitcode].color_index);
#endif
				*pixel  = dictionary[bitcode].color_index;
				bitcode = dictionary[bitcode].prefix_bitcode;
			} while(bitcode != -1);

		// third case: ugly catch with "C STRING C STRING C", where encoder encode C STRING to bitcode1, to dictionary
		//             adds C STRING C and it writes it immediately so we don't have C STRING C in dictionary yet
		} else if (bitcode == next_bitcode) 
		{
#if DEBUG >= 3
			printf(" 3: Bitcode is not in dictionary\n *** length %d + 1 = %d\n", dictionary[prev_bitcode].length, dictionary[prev_bitcode].length+ 1);
#endif

			picture_cur += dictionary[prev_bitcode].length + 1;
			pixel = picture_cur - 1;	// the last one will be same as the first color in dic[prev_bitcode]

			do
			{	
				pixel--;
				*pixel = dictionary[prev_bitcode].color_index;
#if DEBUG >= 4
				printf("  3: Color %02X\n", dictionary[prev_bitcode].color_index);
#endif
				prev_bitcode = dictionary[prev_bitcode].prefix_bitcode;
			} while (prev_bitcode != -1);

			*(picture_cur-1) = *pixel;
#if DEBUG >= 4
			printf("  3: LAST Color %02X\n", *pixel);
#endif

			prev_bitcode = bitcode;
		} else {
			fprintf(stderr, "stderr Error:Third case, bitcode 0x%04X is not equal to next_bitcode 0x%04X\n", bitcode, next_bitcode);
#if DEBUG >=1 
			fprintf(stdout, "stdout Error:Third case, bitcode 0x%04X is not equal to next_bitcode 0x%04X\n", bitcode, next_bitcode);
#endif
			break;
		}
		// assign to next_bitcode previous_sequence + current_seqence[0]
		dictionary[next_bitcode].color_index = *pixel;
#if DEBUG >= 3
		printf("Inserting to %d sequence(rev):", next_bitcode);
		unsigned index = next_bitcode;
		do {
			printf(" 0x%02X", dictionary[index].color_index);
			index = dictionary[index].prefix_bitcode;
		} while(index != -1);
		printf("\n");
#endif

		next_bitcode++;
		if (next_bitcode > bitcode_mask && bitcode_width < BITCODE_WIDTH_MAX) {
			bitcode_width++;
			bitcode_mask = (bitcode_mask << 1) | 0x1;
#if DEBUG >= 3
			printf("Resizing bitcode_width to %d\n", bitcode_width);
		} else if (next_bitcode > bitcode_mask) {
			printf("**** Can't resize bitcode width to 12, next_bitcode 0x%04X, bitcode_mask 0x%04X\n", next_bitcode, bitcode_mask);
#endif
		}

	}


	// Convert it all to structure
	
	image.width = image_width;
	image.height = image_height;
	image.palette_size = palette_size;
	image.palette = palette;
	image.data = picture;

	delete[] dictionary;
	delete bit_reader;

	return 0x1;
}






int tga2gif(const char * in_name, const char * out_name)
{
	FILE *in, *out;
	
	if ((in = fopen(in_name, "rb")) == NULL) {
		fprintf(stderr, "%s : Unable to open file %s\n", __FUNCTION__, in_name);
		return 0;
	}
	
	if ((out = fopen(out_name, "wb")) == NULL) {
		fprintf(stderr, "%s : Unable to open file %s\n", __FUNCTION__, out_name);
		fclose(out);
	}
	
	struct palette_image tga_image;
	
	if (!load_tga_image(in, tga_image)) {
		fprintf(stderr, "Unable to read tga file\n");
		return 0;
	}

	int res = palette_image2gif(tga_image, out);
	
	delete[] tga_image.data;
	delete[] tga_image.palette;

	fclose(in);

	fflush(out);
	fclose(out);
	
	return res;
}

// fue to differences in upper part and this one, here is accurate. 
// In the upper part, this is 1<<biwidth-1, here it is real bitwidth
#undef BITCODE_WIDTH_MAX
#define BITCODE_WIDTH_MAX	12


/**
 * Antagonist of BitReader.
 * 
 */
class BitWriter {
private:
	FILE *f;
	unsigned bitcode_buffer[256];
	unsigned buffer_index;			// how many bytes are occupied by bitstream
	unsigned bits_in_last_byte;		// number of bits used in bitcode_buffer[buffer_index]

	// write content of buffer in friendly chunk fashion
	void flush_buffer() {
		if (buffer_index == 0)
			return;


		unsigned char c = buffer_index + (bits_in_last_byte>0 ? 1 : 0);
		fwrite(&c,1,1,f);	//chunk size
		
		for (unsigned i=0;i<buffer_index;i++) {
			c = bitcode_buffer[i];
			fwrite(&c,1,1,f);
		}
		// is something in last byte?
		if (bits_in_last_byte!=0) {
			c = bitcode_buffer[buffer_index];
			fwrite(&c,1,1,f);
		}

		buffer_index = 0;
		memset(bitcode_buffer, 0, sizeof(bitcode_buffer));
	}

public:
	BitWriter(FILE *file) {
		f = file;
		buffer_index = 0;
		bits_in_last_byte = 0;
		memset(bitcode_buffer, 0, sizeof(bitcode_buffer));
	}

#ifndef MIN
#define MIN( x, y ) ((x) < (y) ? (x) : (y))
#endif
	/*
	 * Write bitcode to bitstream. 
	 */
	void writeBitcode(unsigned bitcode, unsigned bitcode_width) {
#if DEBUG >= 2
		printf("Write bitcode 0x%02X with bitwidth %d\n", bitcode, bitcode_width);
#endif
		while (bitcode_width > 0)
		{
			if (buffer_index == 255) // is is index of buffer that will be used to store bits
			{
#if DEBUG >= 2
				printf("Flushing buffer bytes: %d bits: %d\n", buffer_index, bits_in_last_byte);
#endif
				flush_buffer(); //  can be used inly because bits_in_last_byte==0
			} 
			else 
			{
				// how many bits should we take from bitcode and write int this loop
				unsigned num_of_bits2write = MIN(bitcode_width, 8-bits_in_last_byte);
				
				// take "num_of_bits2write" bits from "bitcode"
				unsigned bits = bitcode & ((1 << num_of_bits2write)-1);
				bitcode >>= num_of_bits2write;
				bitcode_width-=num_of_bits2write;

				// write "num_of_bits2write" from "bitcode" to "bitcode_buffer"
				bitcode_buffer[buffer_index] |= bits << bits_in_last_byte;
				bits_in_last_byte += num_of_bits2write;
				buffer_index += bits_in_last_byte/8;
				bits_in_last_byte = bits_in_last_byte % 8;
			}
		}

	}

	/*
	 * Write number in big endian (0x1234 as 0x12 0x34)
	 */
	void write_number(unsigned number, int bytes) {
#if DEBUG >= 3
		printf("Write number 0x%04X in %d bytes\n", number, bytes);
#endif
		flush_buffer();
		// write number
		for (int i=bytes-1;i>=0;i--) {
			unsigned char c = (number >> (i*8)) & 0xFF;
			fwrite(&c,1,1,f);
		}
	}

	/*
	 * Write last bitcode to bitstream and write end chunk mark.
	 */
	void finishBitcode(unsigned bitcode, unsigned bitwidth) {
		writeBitcode(bitcode, bitwidth);
		flush_buffer();
		// end of chunks
		write_number(0,1);
		fflush(f);
	}
};


/**
 * TODO: This should be done with something better, like hash table
 */
struct dic_array_int {
	int bitcode;			// bitcode of this entry
	int prefix_index;		// index of array - 1
	unsigned byte;			// last byte in array
};


/**
 * Fill table with initial bitcodes that coresponds to the palette colors
 */
static void init_dictionary(struct dic_array_int * dictionary, int length, int number) {
	for (int index=0; index < length; index++) {
		dictionary[index].prefix_index = -1;
		dictionary[index].bitcode = -1;
	}
	for (int index=0; index < number; index++) {
		dictionary[index].bitcode = index;
		dictionary[index].byte    = index;
	}
}

// insert bitcode to dictionary
static int insert2dic(struct dic_array_int * dictionary, int dic_length, int bitcode, int prev_index, unsigned cur_byte) {

	for (int index=0; index<dic_length;index++) {
		if (dictionary[index].bitcode == -1) {
			dictionary[index].prefix_index = prev_index;
			dictionary[index].bitcode      = bitcode;
			dictionary[index].byte         = cur_byte;
#if DEBUG >=3
			printf("Found free index %d\n", index);
			printf("Insert to %d dictionary(rev): 0x%02X", bitcode, cur_byte);
#endif
#if DEBUG >=4
			for (int i = prev_index; i != -1; i = dictionary[i].prefix_index) {
				printf(" 0x%02X", dictionary[i].byte);
			}
#endif
#if DEBUG >=3
			printf("\n");
#endif


			return index;
		}
	}
	return -1;
}

/**
 *
 */
int find_seq_index(struct dic_array_int *dictionary, int dic_length, int prev_index, unsigned cur_byte) {
	for (int index = 0; index <dic_length; index++) {
		if (dictionary[index].prefix_index == prev_index && dictionary[index].byte == cur_byte) {
			return index;
		}
	}
//	fprintf(stderr, "Unable to find sequence with prev_index: %d and cur_byte: %d\n", prev_index, cur_byte);
	return -1;
}

/*
 * Determine minimal bitwidth. 
 */
static int get_initial_bitwidth(int num_of_colours) {
	int bits = 0;
	while (num_of_colours != 0) {
		num_of_colours >>= 1;
		bits++;
	}
	return bits;
}

int palette_image2gif(struct palette_image &image, FILE *out)
{
	int orig_bitcode_width = get_initial_bitwidth(image.palette_size);
	int bitcode_width = orig_bitcode_width + 1;
#if DEBUG >=1
	printf("orig_bitcode_width is %d\n", orig_bitcode_width);
#endif

	const int RESET_DICTIONARY_BITCODE = 1 << orig_bitcode_width;
	const int DONE_BITCODE             = (1 << orig_bitcode_width) + 1;
	
	int next_bitcode = DONE_BITCODE + 1;
	unsigned length = image.width * image.height;

	int dic_length = 0x4000;
	struct dic_array_int * dictionary = new struct dic_array_int[dic_length]; // should be more than enough
	BitWriter writer(out);

	// index to dictionary for the last 
	int prev_index = RESET_DICTIONARY_BITCODE; // first 0..RESET_DICTIONARY_BITCODE indexes are are same as bitcodes

	// write header
	writer.write_number(image.width, 2);
	writer.write_number(image.height, 2);
	writer.write_number(orig_bitcode_width, 1);
	writer.write_number(image.palette_size - 1, 1);

	// write palette
	for (int i = 0; i < image.palette_size; i++) {
		writer.write_number(image.palette[i].red, 1);
		writer.write_number(image.palette[i].green, 1);
		writer.write_number(image.palette[i].blue, 1);
	}

	init_dictionary(dictionary, dic_length, DONE_BITCODE + 1);
	
	for (unsigned offset = 0; offset <length; offset++) {
#if DEBUG >= 2
		printf("\n");
#endif

		unsigned cur_byte = image.data[offset];
		int cur_seq_index = -1;

#if DEBUG >= 4
		printf("Prev_seq:");
		for (int index = prev_index; index != -1; index = dictionary[index].prefix_index) {
			printf(" 0x%02X", dictionary[index].byte);
		}
		printf("\n");
#endif
#if DEBUG >= 3
		printf("Read byte: 0x%02X\n", cur_byte);
#endif

		/*
			Prev_seq: 0x00
			Read byte: 0x00
			Index for sequnce found: 0x0081
		 */
		// try to find index of cur_seq (prev_seq + cur_byte)
		cur_seq_index = find_seq_index(dictionary, dic_length, prev_index, cur_byte);
/*		for (int index = 0; index <dic_length; index++) {
			if (dictionary[index].prefix_index == prev_index && dictionary[index].byte == cur_byte) {
				cur_seq_index = index;
				break;
			}
		}			
*/
#if DEBUG >= 3
		if (cur_seq_index != -1) {
			printf("Index for sequnce found: 0x%04X\n", cur_seq_index);
		} else {
			printf("Index for sequnce NOT found\n");
		}
#endif

		if (prev_index == RESET_DICTIONARY_BITCODE) {
			writer.writeBitcode(dictionary[prev_index].bitcode, bitcode_width);
			prev_index = cur_byte;
#if DEBUG >= 3
			printf("Previous was RESET, set prev_index to 0x%02X\n", prev_index);
#endif
			continue;
			// if prev_seq + cur_char is in dictionary, continue
		} if (cur_seq_index != -1) {
			prev_index = cur_seq_index;
#if DEBUG >= 3
			printf("This sequence is already in the dictionary bitcode:0x%02X\n", dictionary[cur_seq_index].bitcode);
#endif
			continue;
		} else {
			// if prev_seq is in dictionary, but prev_seq+cur_byte is not
			writer.writeBitcode(dictionary[prev_index].bitcode, bitcode_width);

			insert2dic(dictionary, dic_length, next_bitcode, prev_index, cur_byte);
			
			prev_index = cur_byte;

			if (next_bitcode > ((1 << bitcode_width) - 1 )) {
				if (bitcode_width < BITCODE_WIDTH_MAX) {
					bitcode_width++;
#if DEBUG >= 3
					printf("Resizing bitcode width to %d\n", bitcode_width);
#endif
				} else {
					writer.writeBitcode(RESET_DICTIONARY_BITCODE, bitcode_width);
#if DEBUG >= 3
					printf("Want to resize bitcode width, but RESET necessary\n");
#endif
					// reset dictionary
					prev_index = RESET_DICTIONARY_BITCODE;
					offset--;
					init_dictionary(dictionary, dic_length, DONE_BITCODE + 1);
					next_bitcode = DONE_BITCODE;	// it ++ after
					bitcode_width = orig_bitcode_width+1;
					// TODO prev_index
				}
			}
			next_bitcode++; //  TODO according to image files, it should be here. Can it cause trouble? No
		}
	}
	// output bitcode for prev_seq
	writer.writeBitcode( dictionary[prev_index].bitcode, bitcode_width);
	// Done, finishing touch
	writer.finishBitcode( DONE_BITCODE, bitcode_width);

	// I have no ideal what these numbers are and I haven't seen any change
	// no matter what they were filled with.
#ifdef NORMAL
	writer.write_number(0x3b80, 2);	
	writer.write_number(0x0000, 2);	
	writer.write_number(0x0000, 2);	
	writer.write_number(0x52A8, 2);	

	writer.write_number(0x0000, 2);	
	writer.write_number(0x0000, 2);	
	writer.write_number(0x0082, 2);	
	writer.write_number(0x0C, 1);	
#else
	writer.write_number(0xFFFF, 2);	
	writer.write_number(0xEEEE, 2);	
	writer.write_number(0xDDDD, 2);	
	writer.write_number(0xCCCC, 2);	

	writer.write_number(0xBBBB, 2);	
	writer.write_number(0xAAAA, 2);	
	writer.write_number(0x9999, 2);	
	writer.write_number(0x88, 1);	
#endif
	delete[] dictionary;
	return 1;
}







