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
 * Release notes: This file contains functions for loading, saving and modifying 
 * the .RSC (resource) files used in Civilization I for windows. There few unnatural 
 * things that should be noted:
 *  - In rsc_file_section numEntries is not number of entries, but tha maximal offset 
 *    therefore, in loops you will see a lot of i=0; i<=numEntries. 
 *  - The same things happens in rsc_storage.number_of_sections
 *
 * Since it is maximal offset and not length that is stored in file, it is 
 * impossible to have rsc file without at least one section and it is impossible 
 * to have section without file.
 *
 * Some routines are not properly tested (add, delete), but it is not important since 
 * there is no reason to do anything but to replace some file.
 */

#define _CRTDBG_MAP_ALLOC

#include <stdio.h>
#include <assert.h>
#include <conio.h>
#include <string.h>

#include "rsc_manager.h"

#define DEBUG 4

static unsigned read_number(FILE*f, int bytes) {
	unsigned number = 0;
	unsigned char byte;

	for (int i=0;i<bytes;i++) {
		fread(&byte, 1, 1, f);
		number += byte << ((bytes - 1 - i)*8);
	}
	return number;
}

static int read_string(FILE *f, char *string) {
	unsigned len = read_number(f,1);
	fread(string,1,len,f);
	string[len] = '\0';
	return 0;
}

// write number in BIG endian 0x1234 is 0x12 0x34
static void write_number(unsigned number, int bytes, FILE *f) {
	for (int i=bytes-1;i>=0;i--) {
		unsigned char c = (number >> (i*8)) & 0xFF;
		fwrite(&c,1,1,f);
	}
}


/**
 * Create storage = read it and fill structures
 * If creation failed, return NULL
 */
struct rsc_storage * rsc_create_storage(const char*filename) {
	struct rsc_storage * storage = NULL;

	FILE *f;

	assert(filename != NULL);
	if ((f = fopen(filename, "rb")) == NULL) {
		return NULL;
	}
	storage = new rsc_storage();
	storage->f = f;

	// read header
	storage->header.header_size = read_number(f, 4);
	storage->header.desc_offset = read_number(f, 4);
	storage->header.data_size = read_number(f, 4);
	storage->header.desc_size = read_number(f, 4);

	if (storage->header.header_size != 0x100) {
		fprintf(stderr, "Error: Length of storage header is not 0x100, but 0x%04X\n", storage->header.header_size);
		return 0;
	}

	storage->header.garbage_length = storage->header.header_size - 16;
	storage->header.garbage = new unsigned char[storage->header.garbage_length];

	fread(storage->header.garbage, 1, storage->header.garbage_length , f);

#if DEBUG >=1
	fprintf(stderr, "Storage: %s\nHeader size 0x%04X\n",filename, storage->header.header_size);
	fprintf(stderr, "File table description offset 0x%04X\n", storage->header.desc_offset);
	fprintf(stderr, "Data size 0x%04X\n", storage->header.data_size);
	fprintf(stderr, "File table size 0x%04X\n", storage->header.desc_size);
	fprintf(stderr, "=========================================\n");
#endif

	// seek to file table
	fseek(f,storage->header.desc_offset,SEEK_SET);
	// read file table

	///
	storage->file_table.rsc_header_size = read_number(f,4);

	// check
	if (storage->file_table.rsc_header_size != 0x00000100) {
		fprintf(stderr, "Signature is not 0x00000100, but 0x%X\n", storage->header.header_size);
		rsc_destroy_storage(storage);
		return NULL;
	}
	
	storage->file_table.file_table_offset  = read_number(f,4);
	storage->file_table.data_size          = read_number(f,4);
	storage->file_table.file_table_size    = read_number(f,4);
	
	fread(storage->file_table.unkn,1,8,f);

	storage->file_table.unk1C              = read_number(f,2);
	storage->file_table.file_info_len      = read_number(f,2);
	
	storage->number_of_sections = read_number(f,2);
	storage->sections = new rsc_file_section[storage->number_of_sections+1];

#if DEBUG >= 2
	fprintf(stderr, "File table offset 0x%04X\n", storage->file_table.file_table_offset);
	fprintf(stderr, "File table data size 0x%04X\n", storage->file_table.data_size);
	fprintf(stderr, "File table size 0x%04X\n", storage->file_table.file_table_size);
	fprintf(stderr, "Number of sections 0x%04X\n", storage->number_of_sections);
	fprintf(stderr, "\n");
#endif

	for (int i=0;i<=storage->number_of_sections;i++) {
		fread(storage->sections[i].name,1,4,f);
		storage->sections[i].name[4]    = '\0';
		storage->sections[i].numEntries = read_number(f,2);
		storage->sections[i].offset     = read_number(f,2); 
#if DEBUG >= 2
		fprintf(stderr, "Section %d: %s entries: 0x%04X offset: 0x%04X\n", i, storage->sections[i].name, storage->sections[i].numEntries, storage->sections[i].offset);
#endif
	}

	for (int sec=0; sec<=storage->number_of_sections; sec++)	{
		struct rsc_file_section * section = &storage->sections[sec];
		section->files = new rsc_file_info[section->numEntries + 1];

		int index = 0;
		// for each file in section
		for (unsigned i=0;i<=section->numEntries;i++) {
			char filename[256];
			struct rsc_file_info *file_info = &section->files[i];
			file_info->id          = read_number(f,2);
			file_info->name_offset = read_number(f,2);
			file_info->file_offset = read_number(f,4);
			file_info->zero        = read_number(f,4);
			file_info->name        = NULL;

			if (file_info->name_offset != 0xFFFF) {
				long current_pos = ftell(f);
				fseek(f, storage->header.desc_offset + storage->file_table.file_info_len + file_info->name_offset, SEEK_SET);
				read_string(f, filename);
				// return
				fseek(f, current_pos, SEEK_SET);

				file_info->name = new char[strlen(filename) + 1]; 
				strcpy(file_info->name, filename);
			} else {
				sprintf(filename, "CivRSC_%s_0x%X", section->name, file_info->id);
			}
#if DEBUG >= 3
			printf("Sec %d:0x%03X (%15s)- Id 0x%04X - name offset: 0x%04X file_offset 0x%08X Zero %08X\n", sec, index++, filename, (int)file_info->id, (int)file_info->name_offset, (unsigned)file_info->file_offset, file_info->zero);
#endif
			//write_file(f,filename, (desc.file_offset & 0x00FFFFFF)+0x100);
			// read file
			long cur_pos = ftell(f);
			fseek(f, (file_info->file_offset & 0x00FFFFFF) + 0x100, SEEK_SET);
			file_info->data_length = read_number(f, 4);
			fprintf(stderr, "Size of data: %d\n", file_info->data_length);
			file_info->data = new unsigned char[file_info->data_length];

			if (file_info->data != NULL) {
				fread(file_info->data, 1, file_info->data_length, f);
			} else {
				fprintf(stderr, "Unable to allocate memory for file from sec %d, id 0x%02X (0x%04X bytes)\n", sec, i, file_info->data_length);
				file_info->data_length = 0;
			}
			fseek(f, cur_pos, SEEK_SET);
		}
#if DEBUG >= 3
		printf("\n");
#endif
	}

	// initialize file table

	return storage;
}



/**
 * Get size of all files that are held in storage
 */
static unsigned get_all_files_size(struct rsc_storage *storage) {
	unsigned size = 0;
	for (int sec=0; sec<=storage->number_of_sections; sec++) {
		for (unsigned file=0; file<=storage->sections[sec].numEntries; file++) {
			size += storage->sections[sec].files[file].data_length + 4;
		}
	}
	return size;
}

/**
 * Return size of filenames table at the end of file description table
 */
static unsigned get_names_table_size(struct rsc_storage * storage) {
	unsigned size = 0;
	for (int i=0; i<=storage->number_of_sections; i++) {
		for (unsigned file=0; file<=storage->sections[i].numEntries; file++ ) {
			if (storage->sections[i].files[file].name != NULL)
				size += strlen(storage->sections[i].files[file].name) + 1;
		}
	}
	return size;
}

/**
 * Return size of file description table including 
 */
static unsigned get_file_table_size(struct rsc_storage *storage) {
	unsigned size = 0; 
	size += 4; // rsc_header_size;    /* 4 probably size of header, nonetheless always 0x00000100 */
	size += 4; // file_table_offset;  /* 4 offset from the beginning of file*/
	size += 4; // data_size;          /* 4 size of data in whole file, probably file_table_offset - 0x100 (rsc header size) */
	size += 4; // file_table_size;    /* 4 length of whole description section from rsc_header_size to the end of file*/
	size += 8; // unkn[8];            /* 8 unknown for now, OK even if zero */
	size += 2; // unk1C;              /* 2 always 0x001C, something with memory, if changes, GPF*/
	size += 2; // file_info_len;      /* 2 size of file table without names table (from rsc_header_size to the name table*/
	size += 2; // number_of_sections; /* 2 number of sections minus one: 0 for one section (CvPc normally), 4 for five sections */

	// add size of sections
	size += (storage->number_of_sections + 1) * (4 + 2 + 2); // 4 for name, 

	// add size of file_info for each file
	for (int i=0; i<=storage->number_of_sections; i++) {
		/*2 for Id, 2 for name offset, 4 for file offset, 4 for zero */
		size += (storage->sections[i].numEntries + 1) * (2 + 2 + 4 + 4); 
	}

	size += get_names_table_size(storage);
	return size;
}

/**
 * Update offsets and size so they are accurate (like data size has changed after deletion of file)
 * TODO: take care about offsets 0x200... text files
 */
static int rsc_validate_storage(struct rsc_storage*storage) {
	unsigned all_files_size = get_all_files_size(storage);
	unsigned file_table_size = get_file_table_size(storage);
	unsigned names_table_size = get_names_table_size(storage);

	// update header
	storage->header.desc_offset = all_files_size + storage->header.header_size;
	storage->header.data_size = all_files_size;
	storage->header.desc_size = file_table_size;

	// update file table header
	storage->file_table.file_table_offset = storage->header.desc_offset;
	storage->file_table.data_size         = storage->header.data_size;
	storage->file_table.file_table_size   = storage->header.desc_size;
	storage->file_table.file_info_len     = file_table_size - names_table_size;

	// update sections 
	for (int sec=0; sec<=storage->number_of_sections; sec++) {
		/* 2 [num_of_sections] + num_of_sections * size_of_section */
		unsigned offset = 2 + (storage->number_of_sections + 1) * (4 + 2 + 2);

		/* add size of all file entries in sections before this one */
		for (int i=0; i<sec; i++)
			offset += (storage->sections[i].numEntries + 1) * (2 + 2 + 4 + 4); 

		storage->sections[sec].offset = offset;
	}

	// update name offsets of files an offset in file
	unsigned file_ofs = 0; 
	unsigned name_ofs = 0;

	for (int sec=0; sec<=storage->number_of_sections; sec++) {
		for (unsigned file_idx=0; file_idx<=storage->sections[sec].numEntries; file_idx++) {
			struct rsc_file_info * file = &storage->sections[sec].files[file_idx];
			file->file_offset = file_ofs;
			file_ofs += 4 + file->data_length;

			if (file->name != NULL) {
				file->name_offset = name_ofs;
				name_ofs += strlen(file->name) + 1;
			} else
				file->name_offset = 0xFFFF;
		}
	}
	
	return 1;
}


/**
 * Write resources to the file.
 */
int rsc_write_storage(struct rsc_storage * storage, const char *filename) {
	FILE *f;

	assert(filename != NULL);

	if (!rsc_validate_storage(storage)) {
		fputs("Unable to validate storage.\n", stderr);
		return 0;
	}

	if ((f = fopen(filename, "wb")) == NULL) {
		fprintf(stderr, "Unable to open file \"%s\" for writing.\n", filename);
		return 0;
	}

	// Write header
	write_number(storage->header.header_size, 4, f);
	write_number(storage->header.desc_offset, 4, f); // 
	write_number(storage->header.data_size, 4, f);
	write_number(storage->header.desc_size, 4, f);

	for (unsigned i=0; i < storage->header.garbage_length; i++) {
		write_number(storage->header.garbage[i], 1, f);
	}
	// Write files
	for (int sec=0; sec<=storage->number_of_sections; sec++) {
		for (unsigned index=0; index<=storage->sections[sec].numEntries; index++) {
			struct rsc_file_info *file = &storage->sections[sec].files[index];
			write_number(file->data_length, 4, f);
			fwrite(file->data, 1, file->data_length, f);
		}
	}

	// write filetable
	write_number(storage->file_table.rsc_header_size, 4, f);
	write_number(storage->file_table.file_table_offset, 4, f);
	write_number(storage->file_table.data_size, 4, f);
	write_number(storage->file_table.file_table_size, 4, f);
	fwrite(storage->file_table.unkn,1,8,f);
	write_number(storage->file_table.unk1C, 2 , f);
	write_number(storage->file_table.file_info_len, 2, f);
	
	// sections
	write_number(storage->number_of_sections, 2, f);

	for (int sec=0; sec<=storage->number_of_sections; sec++) {
		fwrite(storage->sections[sec].name, 1, 4, f);
		write_number(storage->sections[sec].numEntries, 2, f);
		write_number(storage->sections[sec].offset, 2, f);
	}

	// write file table
	for (int sec=0; sec<=storage->number_of_sections; sec++) {
		for (unsigned file_idx=0; file_idx<=storage->sections[sec].numEntries; file_idx++) {
			struct rsc_file_info * file = &storage->sections[sec].files[file_idx];
			write_number(file->id, 2, f);
			write_number(file->name_offset, 2, f);
			write_number(file->file_offset, 4, f);
			write_number(file->zero, 4, f);

			printf("id 0x%04X name offset 0x%04X file offset 0x%08X zero 0x08X\n", file->id, file->name_offset, file->file_offset, file->zero );
		}
	}

	// write names
	for (int sec=0; sec<=storage->number_of_sections; sec++) {
		for (unsigned file_idx=0; file_idx<=storage->sections[sec].numEntries; file_idx++) {
			struct rsc_file_info * file = &storage->sections[sec].files[file_idx];
			if (file->name != NULL) {
				write_number(strlen(file->name), 1, f);
				fwrite(file->name, 1, strlen(file->name), f);
			}
		}
	}

	fclose(f);
	return 1;
}






/**
 * Destory the storage, close files, free memory and so on.
 */
int rsc_destroy_storage(struct rsc_storage * storage) {

	if (storage != NULL) {
		fclose(storage->f);
//		fprintf(stderr, "Closing file\n");
		if (storage->sections != NULL) {
			for (int i=0; i<= storage->number_of_sections; i++) {
				if (storage->sections[i].files != NULL) {
					for (unsigned file=0; file<=storage->sections[i].numEntries;file++) {
						if (storage->sections[i].files[file].data != NULL) {
//							fprintf(stderr, "Releasing storage->sections[%d].files[%d].data\n", i, file);
							///delete[] storage->sections[i].files[file].data;
						}
						if (storage->sections[i].files[file].name != NULL)  {
//							fprintf(stderr, "Releasing storage->sections[%d].files[%d].name\n", i, file);
							///delete[] storage->sections[i].files[file].name;
						}
//						
					}
//					fprintf(stderr, "Releasing storage->sections[%d].files\n", i);
					// causes corruption sometimes
					//delete[] storage->sections[i].files;
				}
			}
//			fprintf(stderr, "Releasing storage->sections\n");
			///delete[] storage->sections;
		}
		fprintf(stderr, "Releasing storage\n");
		///delete storage;
	}
	return 1;
}








/* Write file from offset and return
 */
static int write_file(FILE *f, char * filename, long offset) {
	fprintf(stderr, "Writing file %s 0x%08X\n", filename, offset);
	long cur_pos = ftell(f);
	fseek(f, offset, SEEK_SET);
	unsigned len = read_number(f,4);
	FILE * file;
	
	if ((file = fopen(filename, "wb")) == NULL) {
		fprintf(stderr, "File %s cannot be opened for writing.", filename);
		return 0;
	}

	unsigned char *memory = new unsigned char[len];
	if (memory == NULL) {
		fprintf(stderr, "Memory is null\n");
		fclose(file);
		fseek(f, cur_pos, SEEK_SET);
		return 0;
	}
	fread(memory,1,len,f);
	fwrite(memory,1,len,file);
	
	fclose(file);
	fseek(f, cur_pos, SEEK_SET);

	///delete[] memory;
	return 0;
}

/**
 * Read file and return the read data and size of file. If error, return NULL
 */
static unsigned char * read_file(const char * filename, unsigned * size) {
	FILE *f;
	if ((f = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "%s: Unable to open file %s.\n", __FUNCTION__, filename);
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char *data = new unsigned char[*size];
	if (data == NULL) {
		fprintf(stderr, "%s: Unable to allocate %d bytes.\n", __FUNCTION__, *size);
		fclose(f);
		return NULL;
	}
	fread(data, 1, *size, f);
	fclose(f);
	return data;
}


/**
 * Removes empty section = section with no files in it
 */
static int rsc_delete_empty_section(struct rsc_storage * storage, int sec) {
	assert(sec>=0 && sec <= storage->number_of_sections);
	assert(storage->number_of_sections > 0);		//there has to be at least one section

	for (int i=sec+1; i<=storage->number_of_sections; i++) {
		storage->sections[i-1] = storage->sections[i];	// XXX: does this copy whole structure?
	}
	storage->number_of_sections--;
	return 1;
}


struct rsc_file_info *rsc_find_file(struct rsc_storage * storage, int param_sec, unsigned param_id) {
	struct rsc_file_info *file = NULL;

	for (int sec=0; sec<=storage->number_of_sections; sec++) {
		for (unsigned id=0; id<=storage->sections[sec].numEntries; id++) {
			if (param_sec == sec && storage->sections[sec].files[id].id == param_id)
				return &storage->sections[sec].files[id];
		}
	}
	return NULL;
}


struct rsc_file_info *rsc_find_file(struct rsc_storage * storage, const char * filename) {
	assert(filename != NULL);
	for (int sec=0; sec<=storage->number_of_sections; sec++) {
		for (unsigned id=0; id<=storage->sections[sec].numEntries; id++) {
			struct rsc_file_info * file = &storage->sections[sec].files[id];
			if (file->name != NULL && !strcmp(filename, file->name))
				return file;
		}
	}
	return NULL;
}


int rsc_delete_file(struct rsc_storage * storage, const char * filename) {
	// find section and id
	for (int sec=0; sec<=storage->number_of_sections; sec++) {
		for (unsigned id=0; id<=storage->sections[sec].numEntries; id++) {
			struct rsc_file_info * file = &storage->sections[sec].files[id];
			if (file->name != NULL && !strcmp(filename, file->name)) {
				return rsc_delete_file(storage, sec, id);
			}
		}
	}
	return 0;
}


int rsc_delete_file(struct rsc_storage * storage, int sec, int id) {
	assert(storage!=NULL);
	struct rsc_file_info *file;

	if (!(file = rsc_find_file(storage, sec, id)))
		return 0;

	if (file->data != NULL) {
		///delete[] file->data;
		file->data = NULL;
	}
	if (file->name != NULL) {
		///delete[] file->name;
		file->name = NULL;
	}

	int num_of_files = storage->sections[sec].numEntries;
	struct rsc_file_section * section = &storage->sections[sec];

	for (int i=id+1; i<=num_of_files; i++) {
		// move file from files[i] to files[i-1]
		section->files[i-1] = section->files[i];
	}

	// if this was last file in section, remove section
	if (num_of_files == 0) {
		return rsc_delete_empty_section(storage, sec);
	} else {
		storage->sections[sec].numEntries--;
	}
	return 1;
}



int rsc_get_section_number(struct rsc_storage * storage, const char*sec_name) {
	assert(storage != NULL);
	assert(sec_name != NULL);
	for (int sec=0; sec<=storage->number_of_sections; sec++) {
		if (!strcmp(sec_name, storage->sections[sec].name))
			return sec;
	}
	return -1;
}


int rsc_add_file(struct rsc_storage * storage, int sec, int id, const char * filename, unsigned zero) {
	assert(storage != NULL);
	assert(filename != NULL);
	assert(sec>=0 && sec<=storage->number_of_sections);

	if (rsc_find_file(storage, sec, id) == NULL) {
		fprintf(stderr, "There is already file with specified sectiond and id.\n");
		return 0;
	}
	
	unsigned char * file_data;
	unsigned file_length;
	if ((file_data = read_file(filename, &file_length)) == NULL) {
		return 0;
	}

	// allocate space for another rsc_file_info
	struct rsc_file_section * section = &storage->sections[sec];
	struct rsc_file_info * new_file_list;

	int num_of_files = section->numEntries + 1;

	new_file_list = new rsc_file_info[num_of_files+1];
	if (!new_file_list) {
		fprintf(stderr, "%s: Unable to allocate new_file_list", __FUNCTION__);
		///delete[] file_data;
		return 0;
	}
	// copy original num_of_files entres to the new array
	memcpy(new_file_list, section->files, sizeof(struct rsc_file_info)*(num_of_files));

	// set up new file
	

	new_file_list[num_of_files].name        = new char[strlen(filename)+1];
	if (new_file_list[num_of_files].name == NULL) {
		fprintf(stderr, "%s: Memory allocation error (want to allocate %d bytes).\n", __FUNCTION__, strlen(filename)+1);
		///delete[] file_data;
		///delete[] new_file_list;
		return 0;
	}
	strcpy(new_file_list[num_of_files].name, filename);
	new_file_list[num_of_files].id          = id;
	new_file_list[num_of_files].data        = file_data;
	new_file_list[num_of_files].data_length = file_length;
	new_file_list[num_of_files].zero        = zero;
	new_file_list[num_of_files].file_offset = 0xFFFFFFFE; // should be OK after validation
	new_file_list[num_of_files].name_offset = 0xFFFFE;    // should be OK after validation
	

	///delete[] section->files;
	section->files = new_file_list;
	section->numEntries++;

	return 1;
}




static int rsc_replace_file(struct rsc_storage * storage, struct rsc_file_info *file, const char *filename) {
	char *name1 = new char[255];//[strlen(filename)+1];
	memset(name1, 0, 255);
	if (name1 == NULL) {
		fprintf(stderr, "%s: Unable to allocate %d bytes.\n", __FUNCTION__, strlen(filename)+1);
		return 0;
	}

	unsigned data_size;
	unsigned char *data = read_file(filename, &data_size);

	if (data == NULL) {
		fprintf(stderr, "%s: Unable to read file %s.\n", __FUNCTION__, filename);
		delete[] name1;
		return 0;
	}


	strcpy(name1, filename);
	delete[] (file->name);
	delete[] (file->data);

	file->name = name1;
	file->data = data;
	file->data_length = data_size;

	return 1;
}

int rsc_replace_file(struct rsc_storage * storage, int sec, int id, const char * filename) {
	struct rsc_file_info *file;
	if ((file = rsc_find_file(storage, sec, id)) == NULL) {
		fprintf(stderr, "%s: File from section %d, id %d is not in resource storage.\n", __FUNCTION__, sec, id);
		return 0;
	}
	return rsc_replace_file(storage, file, filename);
}


int rsc_replace_file(struct rsc_storage * storage, const char * rsc_filename, const char *filename) {
	struct rsc_file_info *file;
	if ((file = rsc_find_file(storage, rsc_filename)) == NULL) {
		fprintf(stderr, "%s: File %s is not in resource storage.\n", __FUNCTION__, rsc_filename);
		return 0;
	}
	return rsc_replace_file(storage, file, filename);
}



