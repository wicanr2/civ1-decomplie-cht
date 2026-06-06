/*
 * Demostration convertor for GIF files used in Civilization I for Windows.
 * Copyright (c) 2008, by Honza Havlicek
 * 
 * Contact: havlicek.honza@gmail.com
 *
 * Permission is given by the author to freely redistribute and include
 * this code in any program as long as this credit is given where due. 
 *
 * Release notes: This file contains a decoder routine for GIF files 
 * used by game Civilization 1. The GIF files are not the standard GIF files
 * although they use LZW compression with variable bitwidth of bitcode, 
 * reset of dictionary bitcode and done bitcode.
 *
 * It also took quite a long time to create, so if you like this or use this, 
 * please send me a note.
 */


#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "common.h"
#include "rsc_manager.h"
#include "sav_file.h"




static int rsc_replace(int argc, char *argv[]) 
{
	if (argc < 5) {
		fprintf(stderr, "Usage: RSC_replace.exe rsc_filestorage section id replacing_file\n\nsection and id are decimal numbers\nFor the list of possible sections and files use RSC_extract\n");
		return 1;
	}
	errno = 0;
	int section = strtoimax(argv[2], NULL, 10);
	
	if (errno) {
		ERROR("Unable to convert '%s' to decimal number (errno: %d, %s).\n", argv[2], errno, strerror(errno));
		return 2;
	}
	fprintf(stderr, "Sec is %d\n", section);
	
	errno = 0;
	int id = strtoimax(argv[3], NULL, 10);
	if (errno) {
		ERROR("Unable to convert '%s' to decimal number (errno: %d, %s).\n", argv[3], errno, strerror(errno));
		return 3;
	}
	fprintf(stderr, "id is %d\n", id);

	fprintf(stderr, "Loading storage...\n");
	struct rsc_storage * storage = rsc_create_storage(argv[1]);
	if (storage == NULL) {
		ERROR("Unable to load storage from file '%s'\n", argv[1]) ;
		return 4;	
	}
	
	fprintf(stderr, "Replacing file...\n");
	if (!rsc_replace_file(storage, section, id, argv[4])) {
		ERROR("Unable to replace file '%s' in storage '%s'\n", argv[4], argv[1]);
		rsc_destroy_storage(storage);
		return 0;
	}
	
	fprintf(stderr, "Writing storage...\n");

	rsc_write_storage(storage, argv[1]);
	if (!rsc_destroy_storage(storage)) {
		ERROR("Unable to write storage '%s'");
	}
	
	return 0;	
}


static int main_tga2gif(int argc, char *argv[]) 
{
	if (argc!= 3)
	{
		fprintf(stderr, "Usage: %s tga_file output_gif_file\n", argv[0]);
		return 0;
	}
	fprintf(stderr, "This will take a while, even few minutes in case of a big image.\n");
	return tga2gif(argv[1], argv[2]);
}

int main(int argc, char * argv[])
{
/*//	return rsc_replace(argc, argv);
	return !main_tga2gif(argc, argv);
	
	RLE_decode("x:\\FRE700A.SAV", "x:\\FRE700A.SAV.d");
	RLE_encode("x:\\FRE700A.SAV.d", "x:\\FRE700A.SAV.dc");
	RLE_decode("x:\\FRE700A.SAV.dc", "x:\\FRE700A.SAV.dcd");
 
	printf("END");
	getch();
	return 0;*/
	// memory allocations take place here
	fprintf(stderr, "Converting the file...\n");
	if (!tga2gif("x:\\SPR32X32.64.waves_ocean.mapwater8.tga", "x:\\SPR32X32.gif")) {
		fprintf(stderr, "Unable to change formats");
		_getch();
		return 1;
	}


	fprintf(stderr, "Creating storage...\n");
	struct rsc_storage * storage = rsc_create_storage("x:\\civdata4.rsc");

	if (storage != NULL) {
		fprintf(stderr, "Replacing file...\n");

		rsc_replace_file(storage, "SPR32X32.GIF", "x:\\SPR32X32.gif");

		fprintf(stderr, "Writing storage...\n");

		rsc_write_storage(storage, "x:\\civdata40.rsc");
		rsc_destroy_storage(storage);
	} else
		fprintf(stderr, "Cannot open the storage file\n");
	_getch();
	return 0;

	if (argc < 3) 
    {
        fprintf(stderr, "   Usage: program gif_file_from_civ1 converted_tga_file\n\nGIF files are not standard gif files, but the graphic files stored in rsc files of game Civilization I Win.");
		_getch();
        return 0x1;
    }
     
	fprintf(stderr, "[Build %s %s]: Convert %s -> %s\n",__DATE__, __TIME__, argv[1], argv[2]);
#if DEBUG >= 1
	printf("%s\n==================================================\n\n", argv[1]);
#endif

	if (tga2gif(argv[1], "x:\\tmp.gif")) {
		gif2tga("x:\\tmp.gif", argv[2]);
	} else
		_getch();
	return 0;
	
	
	/* getopt */
/*	int commands_number = 0; // number of commands used. 
	int command = 'h';
	char *in = NULL;
	char *out = NULL;
	char *sec = NULL;
	char *file_index = NULL;
	int index = -1;
	while ((c = getopt(argc, argv, "gtxradpi:o:s:f:h")) != -1) {
		case 'g': // convert tga to gif
		case 't': // convert gif to tga
		case 'x': // extract file from rcs storage
		case 'r': // replace file in rsc storage
		case 'd': // delete file from 
		case 'a': // add file to rsc storage
		case 'p': // print all files in storage
			commands_number++;
			command = c;
			break;
		case 'i': // input file
			break;
		case 'o': // output file
			break;
		case 's': // section name
			break;
		case 'f': // file index
			break;
		case 'h': // help
			print_help();
			return 2;
		default:
	}
	
	if (commands_number != 1) {
		fprintf(stderr, "Only one command allowed.\nUse -h switch for help");
		return 1;
	}

	// check for input and output files
	if (command == 't' || command == 'g') {
		if (in == NULL) {
			fprintf(stderr, "It is necessary to specify an input file with switch \"-i file\".");
			return 1;
		}
		if (out == NULL) {
			fprintf(stderr, "It is necessary to specify an output file with switch \"-o file\".");
			return 1;
		}
	}
	
	// check for section and file id
	if (command == 'x' || command == 'r' || command == 'd' || command == 'a') {
		if (sec == NULL) {
			fprintf(stderr, "It is necessary to specify an section of rsc storage with switch \"-s name_of_section\".\nUse \"-p\" to list all files and sections.\n");
			return 1;
		}
		if (file_index == NULL) {
			fprintf(stderr, "It is necessary to specify an index of file in rsc storage with switch \"-f decimal_number\".\nUse \"-p\" to list all files and sections.\n");
			return 1;
		}
		errno = 0;
		index = strtol(file_index, null, 10);
		if (errno != 0) {
			fprintf(stderr, "Unable to convert string \"%s\" to decimal number. It is necessary to specify an index of file in rsc storage with switch \"-f decimal_number\".\nUse \"-p\" to list all files and sections.\n", file_index);
			return 1;
		}
	}
	struct rsc_storage * storage;
	switch (command) {
		case 'g': // convert tga to gif
			tga2gif(input, output);
			break;
		case 't': // convert gif to tga
			gif2tga(input, output);
			break;
		case 'x': // extract file from rcs storage
			storage = rsc_create_storage(in);
			rsc
			break;
		case 'r': // replace file in rsc storage
		case 'a': // add file to rsc storage
		case 'd': // delete file from 
		case 'p': // print all files in storage
			commands_number++;
			command = c;
			break;
		case 'i': // input file
			break;
		case 'o': // output file
			break;
		case 'h': // help
			print_help();
			return 2;
		default:
			fprintf(stderr, "Unknown switch\n");
	}
*/
	return 1;
	
}

















