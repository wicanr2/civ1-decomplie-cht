=========================================================
Demostration source and tools for various formats used in 
              Civilization I for Windows.
=========================================================
Copyright (c) 2008, by Honza Havlicek
 
Contact: havlicek.honza@gmail.com

Permission is given by the author to freely redistribute and include 
this code in any program as long as this credit is given where due. 

Note:	This readme is same for program and source code, so some parts may not be applicable.

[Content]
  Release notes
  How to use GIF_convert.exe
  How to use RSC extractor
  How to use RSC_replace
  How to use TGA2GIF
  How to extract and convert all images from Civ I for Win
  How to compile the source
  Something is wrong
  Examples

  Format information
  
  
 Release notes: 
================
	This archive contains several utilities, some of them were already made 
	public some time ago. All of them are from were created from the same source
	that was improved to make new tool available. 
	
	These tools are not exactly user friendly, but if I consider possible userbase
	I don't see any reason to make them more friendly. The usuall way to do somehing
	with this is to install dev-c++ (http://www.bloodshed.net/dev/devcpp.html), 
	load project, change main function, compile	and run the program.

	All of the tools presented here were created by me when I was researching 
	the Civ I for Win (CivWin). The files were not documented, so it took some
	time, skills and a lot of effort.
	
	---------------------------------------------
	THIS SOFTWARE IS PROVIDED AS-IS WITHOUT ANY WARRANTY.
	If it eats youd harddisk, kill your dog, scratch
	your DVD collection and delude girlfriend, 
	I AM IN NO WAY RESPONSIBLE.
	---------------------------------------------
	
    Utilities:
	 * RSC extractor - extract all files from .rsc file
	 * GIF convertor - converts Civ I GIF to paletted TGA image. The GIF file IS NOT 
	                   commonly used GIF file, it is a lzw compressed image.
	 * RSC replace   - replace one file in rsc archive
	 * TGA2GIF       - convert palleted TGA image to GIF file

 How to use GIF_convert.exe:
=============================
	Run "cmd.exe" by
		*] Press Win+R (or Start->Run) and type "cmd.exe"
	Change directory to the one that contains GIF_convert.exe
	Run GIF_convert.exe path_to_GIF_file path_to_converted_TGA_file
		
	The program (GIF_convert.exe) has to have two parameters:
		*] path to GIF file
		*] path to the file, where will be converted GIF image in TGA format.
	
	Examples:
		GIF_convert.exe x:\documents\civilization\all_img\ADVFLGHT.GIF x:\documents\civilization\all_img\ADVFLGHT.TGA
		GIF_convert.exe BIRTH01.gif c:\games\unsorted\intro01.tga
		GIF_convert.exe SPR32X32.GIF  terrain_sprites.tga


 How to use RSC extractor
==========================
	Tool for extraction of rsc (resource) files from windows version of Civilization I game. 
	My game is from Civilization Chronicles, but i should work with others. When you extract 
	the .rsc, it will extract all files from rsc to current directory.
	RSC files contains images for intro, civilopedia, units, tileset as well text for dialogs,
	discoveries, diplomatic screens, behaviour of leaders...
	It doesnt contain discovery tree nor the military units stats.

	How to extract files from rsc: RSC_extract.exe path_to_rsc_file

	When you run the program, you will see an output like:
		Section 0: CvPc entries: 0x000D offset: 0x000A
		Sec 0:0x000 (    BIRTH01.gif)- Id 0x0080 - name offset: 0x0000 file_offset 0x00000000 Zero 00000000
		Sec 0:0x001 (     BIRTH2.gif)- Id 0x0081 - name offset: 0x000C file_offset 0x0001A779 Zero 00000000
		...
	
	That is just some minor debugging left. The important part is Id. Id is used by Civ I to distinguise between 
	files. It is something like a handle, you can exchange Ids and different images will be used (e.g. reverse order of intro,
	first image will be the one with spaceshuttle and the last one with magma)
	

 How to use RSC_replace
========================
	Replace one file in rsc file with something else. If you are changing image, it has to be in GIF format.
	
	RSC_replace.exe rsc_storage section_number id_number replacing_file
	
	Example:
	RSC_replace.exe civdata4.org.rsc 0 200 SPR32X32.64.waves_red.GIF
	
	To find out, which section and id should you use, try output of RSC_extract.exe.
	
	
 How to use TGA2GIF
====================
	TGA2GIF accepts only TGA images with palette, not RLE compressed! 
	Usage: TGA2GIF tga_image_file output_gif_file

		
 How to extract and convert all images from Civ I for Win:
===========================================================
	Create a new directory
	Copy civdata0.rsc, civdata1.rsc, civdata2.rsc, civdata3.rsc and civdata4.rsc to the directory
	Copy conver.bat, RSC_extract.exe and GIF_convertor.exe  to the directory
	Execute convert.bat
	Done, in directory images should be now all images from Civ I for Win, they are not very different from DOS version though. They are often worse in quality department.
  		
		
 How to compile the source:
============================
	Get a Dev-C++ (http://www.bloodshed.net/dev/devcpp.html)
	Open project "GIF convertor.dev" 
	In menu "Execute" select "Rebuild all" or press Ctrl+F11
	a] Everything compiled OK =>
		In "release" directory should be file GIF_convert.exe
	b] There is some error
	
 Something is wrong:
=====================
	I]  Send me an incriminated file along with description what were you trying to accomplish
	    and the results.
	II] Attempt to update the source to accept the file. 
	DEBUG macro can be changed to increase debug output.

	Level of debug output:
		4: Everything, even colors and palette
		3: Show chunks, bitcodes and DONE and RESET bitcodes, also show what each bitcode does
		2: Show chunks, bitcodes and DONE and RESET bitcodes
		1: Output chunks and basic info about image
		0: Only stderr
 
 
 
  Examples:
 ===========
	How to change Tileset:
	0] copy civdata4.rsc to civdata4.org.rsc in release directory
	X:\Development\Projects\Civilization\release>RSC_extract.exe civdata4.org.rsc
	Section 0: CvPc entries: 0x0000 offset: 0x000A
	Sec 0:0x000 (SPR32X32.GIF)- Id 0x00C8 - name offset: 0x0000 file_offset 0x00000000 Zero 00000000
	Writing file SPR32X32.GIF 0x00000100


	X:\Development\Projects\Civilization\release>GIF_convertor.exe SPR32X32.GIF SPR32X32.tga
	[Build Mar  4 2008 02:57:40]: Convert SPR32X32.GIF -> SPR32X32.tga

 
	1] Change  tileset (image SPR32x32.tga), new tileset is in  
	
	2] Exchange it

	X:\Development\Projects\Civilization\release>TGA2GIF.exe SPR32X32.64.waves_red.tga SPR32X32.64.waves_red.GIF
	This will take a while, even few minutes in case of a big image.

	// USE ID and SECTION from RSC_extract (Sec 0:0x000 (SPR32X32.GIF)- Id 0x00C8) -> sec is 0, id 0xC8=200
	
	X:\Development\Projects\Civilization\release>RSC_replace.exe civdata4.org.rsc 0 200 SPR32X32.64.waves_red.GIF
	Sec is 0
	id is 200
	Loading storage...
	Storage: civdata4.org.rsc
	Header size 0x0100
	File table description offset 0x127F2
	Data size 0x126F2
	File table size 0x004C
	=========================================
	File table offset 0x127F2
	File table data size 0x126F2
	File table size 0x004C
	Number of sections 0x0000

	Section 0: CvPc entries: 0x0000 offset: 0x000A
	Sec 0:0x000 (SPR32X32.64.waves_red.GIF)- Id 0x00C8 - name offset: 0x0000 file_offset 0x00000000 Zero 00000000
	Size of data: 75502

	Replacing file...
	4Name of file SPR32X32.64.waves_red.GIF
	5Name of file SPR32X32.64.waves_red.GIF
	6Name of file SPR32X32.64.waves_red.GIF
	7Name of file SPR32X32.64.waves_red.GIF
	8Name of file SPR32X32.64.waves_red.GIF
	9Name of file SPR32X32.64.waves_red.GIF
	Writing storage...
	id 0x00C8 name offset 0x0000 file offset 0x00000000 zero 0x08X
	Releasing storage 
 
	3] Copy civdata.org.rsc to civdata4.rsc in your civilization directory
 
  File formats notes:
 =======================

 SPR32x32
 This is basic and the most important file of all. It contain tileset. In left bottom you can see a few pixels. 
 Pixels meaning from left to right
 
	0 - background color do not draw
	1 - inside of units color (in SPR32x32) that will be replaced with national main colour in game
	2 - border of units color (in SPR32x32) that will be replaced with national background colour in game
	3 - Main color of Barbarians
	4 - Background colour of Barbarians

	for (i=5; i<19; i+=2)
		colour[i]   - Main colour of Civilization i-5
		colour[i+1] - Backround colour of Civilization i-5

	19 - Color of city screen foreground
	20 - Colour of city screen background 
 
 Palette in SCR32x32 consists from 64 colours that are basic palette and the 8 colours that are used as cyclic palette animation.
 Dont use any colours above 72, they are black in 256 colour mode.
 
 Yes, only 64 colours for 1472 x 400 pixels image. + animation.
 
 GDAT and KDAT: King and Government data. Probably "personality" of kings and improvements of Government
 
 
 
 
 
 
 
 
 