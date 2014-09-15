/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 						Internal server shell headers						*
 * 																			*
 +--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 * AU v1.0 coding by Jared Murphy											*
 * 																			*
 * SWFotE copyright (c) 2002 was created by									*
 * Chris 'Tawnos' Dary (cadary@uwm.edu),									*
 * Korey 'Eleven' King (no email),											*
 * Matt 'Trillen' White (mwhite17@ureach.com),								*
 * Daniel 'Danimal' Berrill (danimal924@yahoo.com),							*
 * Richard 'Bambua' Berrill (email unknown),								*
 * Stuart 'Ackbar' Unknown (email unknown)									*
 * 																			*
 * SWR 1.0 copyright (c) 1997, 1998 was created by Sean Cooper				*
 * based on a concept and ideas from the original SWR immortals: 			*
 * Himself (Durga), Mark Matt (Merth), Jp Coldarone (Exar), 				*
 * Greg Baily (Thrawn), Ackbar, Satin, Streen and Bib as well as much 		*
 * input from our other builders and players.								*
 * 																			*
 * AFKMud Copyright 1997-2002 Alsherok. Contributors: Samson, Dwip, Whir,   *
 * Cyberfox, Karangi, Rathian, Cam, Raine, and Tarl.                        *
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,		*
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,		*
 * Grishnakh, Fireblade, and Nivek.											*
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.						*
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,		*
 * Michael Seifert, and Sebastian Hammer.									*
 * 																			*
 +--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--*/

/* Change this line to the home directory for the server - Samson */
#define HOST_DIR 	"/home/ul/"

/* Change this line to the name of your compiled binary - Samson */
#define BINARYFILE "swr"

/* Change each of these to reflect your directory structure - Samson */

//#define CODEZONEDIR	HOST_DIR "dist3/area/"  /* Used in do_copyzone - Samson 8-22-98 */
//#define BUILDZONEDIR	HOST_DIR "dist2/area/"  /* Used in do_copyzone - Samson 4-7-98 */
#define MAINZONEDIR	HOST_DIR "auv1/area/"   /* Used in do_copyzone - Samson 4-7-98 */
//#define TESTCODEDIR     HOST_DIR "dist3/src/"   /* Used in do_copycode - Samson 4-7-98 */
//#define BUILDCODEDIR    HOST_DIR "dist2/src/"   /* Used in do_copycode - Samson 8-22-98 */
#define MAINCODEDIR	HOST_DIR "auv1/src/" /* Used in do_copycode - Samson 4-7-98 */
//#define CODESYSTEMDIR   HOST_DIR "dist3/system/"   /* Used in do_copysocial - Samson 5-2-98 */
//#define BUILDSYSTEMDIR  HOST_DIR "dist2/system/"   /* Used in do_copysocial - Samson 5-2-98 */
#define MAINSYSTEMDIR   HOST_DIR "auv1/system/" /* Used in do_copysocial - Samson 5-2-98 */
//#define CODECLASSDIR	HOST_DIR "dist3/classes/"  /* Used in do_copyclass - Samson 9-17-98 */
//#define BUILDCLASSDIR	HOST_DIR "dist2/classes/"  /* Used in do_copyclass - Samson 9-17-98 */
//#define MAINCLASSDIR	HOST_DIR "auv1/classes/"   /* Used in do_copyclass - Samson 9-17-98 */
//#define CODERACEDIR	HOST_DIR "dist3/races/" /* Used in do_copyrace - Samson 10-13-98 */
//#define BUILDRACEDIR	HOST_DIR "dist2/races/" /* Used in do_copyrace - Samson 10-13-98 */
//#define MAINRACEDIR	HOST_DIR "dist/races/"  /* Used in do_copyrace - Samson 10-13-98 */
//#define CODEDEITYDIR	HOST_DIR "dist3/deity/" /* Used in do_copydeity - Samson 10-13-98 */
//#define BUILDDEITYDIR	HOST_DIR "dist2/deity/" /* Used in do_copydeity - Samson 10-13-98 */
#define MAINDEITYDIR	HOST_DIR "auv1/gods/"  /* Used in do_copydeity - Samson 10-13-98 */

#ifdef OVERLANDCODE
#define MAINMAPDIR	HOST_DIR "dist/maps/"   /* Used in do_copymap - Samson 8-2-99 */
#define BUILDMAPDIR	HOST_DIR "dist2/maps/"  /* Used in do_copymap - Samson 8-2-99 */
#define CODEMAPDIR	HOST_DIR "dist3/maps/"  /* Used in do_copymap - Samson 8-2-99 */
#endif

DECLARE_DO_FUN( do_compile ); /* Compile command - Samson 4-8-98 */
//DECLARE_DO_FUN( do_copy ); /* Shortcut catcher for copy commands - Samson 4-8-98 */
//DECLARE_DO_FUN( do_copyclass );  /* Copy command for class & skill files - Samson 9-18-98 */
//DECLARE_DO_FUN( do_copycode );   /* Copy command for code files - Samson 4-7-98 */
//DECLARE_DO_FUN( do_copydeity );  /* Copy command for deity files - Samson 10-13-98 */
//DECLARE_DO_FUN( do_copyrace );   /* Copy command for race files - Samson 10-13-98 */
//DECLARE_DO_FUN( do_copysocial ); /* Copy command for socials file - Samson 5-2-98 */
//DECLARE_DO_FUN( do_copymorph );  /* Copy command for polymorph file - Samson 6-13-99 */
//DECLARE_DO_FUN( do_copyzone );   /* Copy command for zonefiles - Samson 4-7-98 */

#ifdef OVERLANDCODE
DECLARE_DO_FUN( do_copymap ); /* Copy command for overland maps - Samson 8-2-99 */
#endif

DECLARE_DO_FUN( do_mudexec ); /* New shell command piper - Samson 4-16-98 */
DECLARE_DO_FUN( do_grep ); /* Noplex's in-game grep interface - Samson 12-19-01 */

void send_telcode( int desc, int ddww, int code );
bool check_forks( DESCRIPTOR_DATA * d, char *cmdline );

extern bool compilelock;
extern bool bootlock;
