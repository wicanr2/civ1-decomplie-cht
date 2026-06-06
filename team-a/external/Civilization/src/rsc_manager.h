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

#ifndef __RSC_MANAGER__
#define __RSC_MANAGER__

/*structure of header of .RSC file */
struct rsc_header {
	unsigned header_size; /* 4 size of file header probably always 0x100 */
	unsigned desc_offset; /* 4 offset to description table from SEEK_SET. */
	unsigned data_size;   /* 4 size of data part of file = file_size - header_size, probably. For now always desc_offset - 0x100*/
	unsigned desc_size;   /* 4 size of file description table including names, rsc_header size word */
	/*after this, there are some bytes to fill header_size. Useless with no used information. */
	unsigned char *garbage; /* header_size - 16 bytes of garbage*/
	unsigned garbage_length;
};


/* description of one file in file table */
struct rsc_file_info {
	unsigned id;             /* 2 unique id used by civ to distinguise the files, not sure if used in files too*/
	unsigned name_offset;    /* 2 offset in bytes fromthe start of name list 
							  *   if 0xFFFF then there is no name ~ -1
 							  */
	unsigned file_offset;    /* 4 offset of file after header 
							  *   (offset is 0x15A24, header is 0x0100 -> file is at 0x015B24 from the begining of the file)
							  *   first thing at the offset is the filesize
							  *   BEWARE: sometimes (0x2000 0000 | offset), probably means TEXT file
							  */
	unsigned zero;           /* 4 seems to be zero about half of the time of the time */

	unsigned data_length;
	unsigned char *data;

	char *name;
};


/* one section of file description table */
struct rsc_file_section {
	char name[5];        /*4 name of section: CvPc, STR#, TEXT, GDAT, KDAT, stored w/o ending zero */
	unsigned numEntries; /*2 number of entries for this section*/
	unsigned offset;     /*2 offset to the section from the numDescSec (so seek from BEFORE numDescSec offset)*/

	struct rsc_file_info *files;
};




struct rsc_file_table {
	unsigned rsc_header_size;    /* 4 probably size of header, nonetheless always 0x00000100 */
	unsigned file_table_offset;  /* 4 offset from the beginning of file*/
	unsigned data_size;          /* 4 size of data in whole file, probably file_table_offset - 0x100 (rsc header size) */
	unsigned file_table_size;    /* 4 length of whole description section from rsc_header_size to the end of file*/
	unsigned char unkn[8];       /* 8 unknown for now, OK even if zero */
	unsigned unk1C;              /* 2 always 0x001C, something with memory, if changes, GPF*/
	unsigned file_info_len;      /* 2 size of file table without names table (from rsc_header_size to the name table*/
	//unsigned number_of_sections; /* 2 number of sections minus one: 0 for one section (CvPc normally), 4 for five sections */

	//struct file_section *sections; /* number_of_sections sections */ 

	/* there are names of files after this but I don't know a way to compute it easily */
};


/**
 * Storage for storing 
 */
struct rsc_storage {
	FILE *f;
	struct rsc_header header;
	struct rsc_file_table file_table;

	int number_of_sections; // copy of file_table.number_of_sectins
	struct rsc_file_section * sections;
	//struct *
};


int rsc_destroy_storage(struct rsc_storage * storage);
int rsc_write_storage(struct rsc_storage * storage, const char *filename);
struct rsc_storage * rsc_create_storage(const char*filename);

/**
 * Add file to storage section "section" with id "id"
 * If first file in section, create the section.
 */
int rsc_add_file(struct rsc_storage * storage, int section, int id, const char * filename);

/**
 * Replace file in storage
 */
int rsc_replace_file(struct rsc_storage * storage, const char * rsc_filename, const char *filename);
int rsc_replace_file(struct rsc_storage * storage, int sec, int id, const char * filename);

/**
 * Delete file from storage, remove section, if it was last file in section
 */
int rsc_delete_file(struct rsc_storage * storage, const char * filename);
int rsc_delete_file(struct rsc_storage * storage, int section, int id);

/**
 * Find and return rsc_file_info for specified id and section.
 * Use ONLY in case you know, what you want to do. If you change something and don't know 
 * what will it do, don't blame me.
 * Return NULL, if nothing found
 */
struct rsc_file_info *rsc_find_file(struct rsc_storage * storage, int sec, unsigned id);
struct rsc_file_info *rsc_find_file(struct rsc_storage * storage, const char * filename);

/**
 * Use this to get section number
 * BEWARE: After deletion, section can be deleted! Therefore, numbering can change
 * return -1 if section not found
 */
int rsc_get_section_number(struct rsc_storage * storage, const char*sec_name);

#endif
