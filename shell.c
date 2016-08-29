/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 					Internal server shell command module					*
 * 																			*
 *  Allows high-level imms to interact with the shell from within the mud.	*
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

/* Comment out this define if the child processes throw segfaults */
#define USEGLOB   /* Samson 4-16-98 - For new shell command */

#include <unistd.h>
#include <sys/wait.h>   /* Samson 4-16-98 - For new shell command */
#include <fcntl.h>
#include <arpa/telnet.h>
#include <string.h>
#include <stdio.h>

#ifdef USEGLOB /* Samson 4-16-98 - For new command pipe */
#include <glob.h>
#endif

#include "mud.h"

/* Global variable to protect online compiler code - Samson */
BOOL compilelock = FALSE;  /* Reboot/shutdown commands locked during compiles */
BOOL bootlock = FALSE;  /* Protects compiler from being used during boot timers */

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

#ifndef USEGLOB
/* OLD command shell provided by Ferris - ferris@FootPrints.net Installed by Samson 4-6-98
 * For safety reasons, this is only available if the USEGLOB define is commented out.
 */
/*
 * Local functions.
 */
FILE *popen( const char *command, const char *type );
int pclose( FILE * stream );

char *fgetf( char *s, int n, register FILE * iop )
{
   register int c;
   register char *cs;

   c = '\0';
   cs = s;
   while( --n > 0 && ( c = getc( iop ) ) != EOF )
      if( ( *cs++ = c ) == '\0' )
         break;
   *cs = '\0';
   return ( ( c == EOF && cs == s ) ? NULL : s );
}

/* NOT recommended to be used as a conventional command! */
void command_pipe( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   FILE *fp;

   set_char_color( AT_RED, ch );

#ifdef MCCP
   compressEnd( ch->desc );
#endif

   fp = popen( argument, "r" );

   fgetf( buf, MAX_STRING_LENGTH, fp );

   send_to_pager( buf, ch );

   pclose( fp );

   return;
}

/* End OLD shell command code */
#endif

/* New command shell code by Thoric - Installed by Samson 4-16-98 */
void send_telcode( int desc, int ddww, int code )
{
   char buf[4];

   buf[0] = IAC;
   buf[1] = ddww;
   buf[2] = code;
   buf[3] = 0;
   write( desc, buf, 4 );
}

void do_mudexec( CHAR_DATA * ch, char *argument )
{
   int desc;
   int flags;
   pid_t pid;
   BOOL iafork = FALSE;

   if( !ch->desc )
      return;

   if( strncasecmp( argument, "ia ", 3 ) == 0 )
   {
      argument += 3;
      iafork = TRUE;
   }

   desc = ch->desc->descriptor;

   set_char_color( AT_PLAIN, ch );

#ifdef MCCP
   compressEnd( ch->desc );
#endif

   if( ( pid = fork(  ) ) == 0 )
   {
      char buf[1024];
      char *p = argument;
#ifdef USEGLOB
      glob_t g;
#else
      char **argv;
      int argc = 0;
#endif
#ifdef DEBUGGLOB
      int argc = 0;
#endif

      flags = fcntl( desc, F_GETFL, 0 );
      flags &= ~FNDELAY;
      fcntl( desc, F_SETFL, flags );
      if( iafork )
      {
         send_telcode( desc, WILL, TELOPT_SGA );
         /*
          * send_telcode( desc, DO, TELOPT_NAWS );   
          */
         send_telcode( desc, DO, TELOPT_LFLOW );
         send_telcode( desc, DONT, TELOPT_LINEMODE );
         send_telcode( desc, WILL, TELOPT_STATUS );
         send_telcode( desc, DO, TELOPT_ECHO );
         send_telcode( desc, WILL, TELOPT_ECHO );
         read( desc, buf, 1024 );   /* read replies */
      }
      dup2( desc, STDIN_FILENO );
      dup2( desc, STDOUT_FILENO );
      dup2( desc, STDERR_FILENO );
      setenv( "TERM", "vt100", 1 );
      setenv( "COLUMNS", "80", 1 );
      setenv( "LINES", "24", 1 );

#ifdef USEGLOB
      g.gl_offs = 1;
      strtok( argument, " " );

      if( ( p = strtok( NULL, " " ) ) != NULL )
         glob( p, GLOB_DOOFFS | GLOB_NOCHECK, NULL, &g );

      if( !g.gl_pathv[g.gl_pathc - 1] )
         g.gl_pathv[g.gl_pathc - 1] = p;

      while( ( p = strtok( NULL, " " ) ) != NULL )
      {
         glob( p, GLOB_DOOFFS | GLOB_NOCHECK | GLOB_APPEND, NULL, &g );
         if( !g.gl_pathv[g.gl_pathc - 1] )
            g.gl_pathv[g.gl_pathc - 1] = p;
      }
      g.gl_pathv[0] = argument;

#ifdef DEBUGGLOB
      for( argc = 0; argc < g.gl_pathc; argc++ )
         printf( "arg %d: %s\r\n", argc, g.gl_pathv[argc] );
      fflush( stdout );
#endif

      execvp( g.gl_pathv[0], g.gl_pathv );
#else
      while( *p )
      {
         while( isspace( *p ) )
            ++p;
         if( *p == '\0' )
            break;
         ++argc;
         while( !isspace( *p ) && *p )
            ++p;
      }
      p = argument;
      argv = calloc( argc + 1, sizeof( char * ) );

      argc = 0;
      argv[argc] = strtok( argument, " " );
      while( ( argv[++argc] = strtok( NULL, " " ) ) != NULL );

      execvp( argv[0], argv );
#endif

      fprintf( stderr, "Shell process: %s failed!\n", argument );
      perror( "mudexec" );
      exit( 0 );
   }
   else if( pid < 2 )
   {
      send_to_char( "Process fork failed.\r\n", ch );
      fprintf( stderr, "%s", "Shell process: fork failed!\n" );
      return;
   }
   else
   {
      ch->desc->process = pid;
      ch->desc->connected = iafork ? CON_IAFORKED : CON_FORKED;
   }
}

/* End NEW shell command code */

/* This function verifies filenames during copy operations - Samson 4-7-98 */
int copy_file( CHAR_DATA * ch, char *filename )
{
   //~ FILE *fp;

   //~ if( !( fp = fopen( filename, "r" ) ) )
   //~ {
      //~ set_char_color( AT_RED, ch );
      //~ ch_printf( ch, "The file %s does not exist, or cannot be opened. Check your spelling.\r\n", filename );
      //~ return 1;
   //~ }
   //~ fclose( fp );
   //~ fp = NULL;
   return 0;
}

/* The guts of the compiler code, make any changes to the compiler options here - Samson 4-8-98 */
void compile_code( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   if( !str_cmp( argument, "clean" ) )
   {
      strncpy( buf, "make -C ../src clean", MAX_STRING_LENGTH );
      do_mudexec( ch, buf );
      return;
   }

   if( !str_cmp( argument, "dns" ) )
   {
	   strncpy( buf, "make -C ../src dns", MAX_STRING_LENGTH );
      do_mudexec( ch, buf );
      return;
   }

   strncpy( buf, "make -C ../src", MAX_STRING_LENGTH );
   do_mudexec( ch, buf );
   return;
}

/* This command compiles the code on the mud, works only on code port - Samson 4-8-98 */
/*												^^ Not anymore ;) - Boran			  */
void do_compile( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   /*if( port != CODEPORT )
   {
	   send_to_char( "&RThe compiler can only be run on the code port.\r\n", ch );
      return;
   } */

   if( bootlock )
   {
      send_to_char( "&RThe reboot timer is running, the compiler cannot be used at this time.\r\n", ch );
      return;
   }

   if( compilelock )
   {
      send_to_char( "&RThe compiler is in use, please wait for the compilation to finish.\r\n", ch );
      return;
   }

   compilelock = TRUE;
   set_char_color( AT_RED, ch );
   snprintf( buf, MAX_STRING_LENGTH, "Compiler operation initiated by %s. Reboot and shutdown commands are locked.", ch->name );
   to_channel(buf, CHANNEL_LOG, "Log", LEVEL_IMMORTAL);
   compile_code( ch, argument );
   return;
}

/* This command catches the shortcut "copy" - Samson 4-8-98 */
void do_copy( CHAR_DATA * ch, char *argument )
{
   //~ set_char_color( AT_YELLOW, ch );
   //~ send_to_char( "To use a copy command, you have to spell it out!\r\n", ch );
   //~ return;
}

/* This command copies class files from build port to the others - Samson 9-17-98 */
void do_copyclass( CHAR_DATA * ch, char *argument )
{
   //~ char buf[MAX_STRING_LENGTH];
   //~ int valid = 0;
   //~ char *fname, *fname2 = NULL;
   //~ char *fnamecheck = NULL;

   //~ if( IS_NPC( ch ) )
   //~ {
      //~ send_to_char( "Mobs cannot use the copyclass command!\r\n", ch );
      //~ return;
   //~ }


   //~ if( argument[0] == '\0' )
   //~ {
      //~ set_char_color( AT_DGREY, ch );
      //~ send_to_char( "You must specify a file to copy.\r\n", ch );
      //~ return;
   //~ }

   //~ if( !str_cmp( argument, "all" ) )
   //~ {
      //~ fname = "*.class";
      //~ fname2 = "skills.dat";
   //~ }
   //~ else if( !str_cmp( argument, "skills" ) )
      //~ fname = "skills.dat";
   //~ else
   //~ {
      //~ fname = argument;
      //~ strncpy( buf, BUILDCLASSDIR, MAX_STRING_LENGTH );
      //~ strncat( buf, fname, MAX_STRING_LENGTH );
      //~ fnamecheck = buf;
      //~ valid = copy_file( ch, fnamecheck );
   //~ }

   //~ if( valid != 0 )
   //~ {
      //~ bug( "do_copyclass: Error opening file for copy - %s!", fnamecheck );
      //~ return;
   //~ }

   //~ if( !str_cmp( argument, "all" ) )
   //~ {
      //~ set_char_color( AT_GREEN, ch );

         //~ send_to_char( "&RClass and skill files updated to main port.\r\n", ch );
         //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDCLASSDIR, fname, MAINCLASSDIR );
//~ #ifdef USEGLOB
         //~ do_mudexec( ch, buf );
//~ #else
         //~ command_pipe( ch, buf );
//~ #endif

         //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDSYSTEMDIR, fname2, MAINSYSTEMDIR );
         //~ do_mudexec( ch, buf );

      //~ send_to_char( "&GClass and skill files updated to code port.\r\n", ch );
      //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDCLASSDIR, fname, CODECLASSDIR );
//~ #ifdef USEGLOB
      //~ do_mudexec( ch, buf );
//~ #else
      //~ command_pipe( ch, buf );
//~ #endif

      //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDSYSTEMDIR, fname2, CODESYSTEMDIR );
      //~ do_mudexec( ch, buf );

      //~ return;
   //~ }

   //~ if( !str_cmp( argument, "skills" ) )
   //~ {
      //~ set_char_color( AT_GREEN, ch );

      //~ if( !sysdata.TESTINGMODE )
      //~ {
         //~ send_to_char( "&RSkill file updated to main port.\r\n", ch );
         //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDSYSTEMDIR, fname, MAINSYSTEMDIR );
         //~ do_mudexec( ch, buf );
      //~ }

      //~ send_to_char( "&GSkill file updated to code port.\r\n", ch );
      //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDSYSTEMDIR, fname, CODESYSTEMDIR );
      //~ do_mudexec( ch, buf );

      //~ return;
   //~ }

   //~ set_char_color( AT_GREEN, ch );

      //~ ch_printf( ch, "&R%s: file updated to main port.\r\n", argument );
      //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDCLASSDIR, fname, MAINCLASSDIR );
      //~ do_mudexec( ch, buf );

   //~ ch_printf( ch, "&G%s: file updated to code port.\r\n", argument );
   //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDCLASSDIR, fname, CODECLASSDIR );
   //~ do_mudexec( ch, buf );

   //~ return;
}

/* This command copies zones from build port to the others - Samson 4-7-98 */
void do_copyzone( CHAR_DATA * ch, char *argument )
{
   //~ char buf[MAX_STRING_LENGTH];
   //~ int valid = 0;
   //~ char *fname, *fname2 = NULL;

   //~ if( IS_NPC( ch ) )
   //~ {
      //~ send_to_char( "Mobs cannot use the copyzone command!\r\n", ch );
      //~ return;
   //~ }

   //~ if( port != BUILDPORT )
   //~ {
      //~ send_to_char( "&RThe copyzone command may only be used from the Builders' port.\r\n", ch );
      //~ return;
   //~ }

   //~ if( argument[0] == '\0' )
   //~ {
      //~ set_char_color( AT_DGREY, ch );
      //~ send_to_char( "You must specify a file to copy.\r\n", ch );
      //~ return;
   //~ }

   //~ if( !str_cmp( argument, "all" ) )
      //~ fname = "*.are";
   //~ else
   //~ {
      //~ fname = argument;

      //~ if( !str_cmp( argument, "help.are" ) )
         //~ fname2 = "help.are";
      //~ if( !str_cmp( argument, "gods.are" ) )
         //~ fname2 = "gods.are";
      //~ if( !str_cmp( argument, "newbie.are" ) )
         //~ fname2 = "newbie.are";
      //~ if( !str_cmp( argument, "limbo.are" ) )
         //~ fname2 = "limbo.are";

      //~ valid = copy_file( ch, fname );
   //~ }

   //~ if( valid != 0 )
   //~ {
      //~ bug( "do_copyzone: Error opening file for copy - %s!", fname );
      //~ return;
   //~ }

   //~ set_char_color( AT_GREEN, ch );

      //~ send_to_char( "&RArea file(s) updated to main port.\r\n", ch );
      //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDZONEDIR, fname, MAINZONEDIR );
//~ #ifdef USEGLOB
      //~ do_mudexec( ch, buf );
//~ #else
      //~ command_pipe( ch, buf );
//~ #endif
   //~ if( fname2 == "help.are" || fname2 == "gods.are" || fname2 == "limbo.are" || fname2 == "newbie.are" )
   //~ {
      //~ send_to_char( "&GArea file(s) updated to code port.\r\n", ch );
      //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDZONEDIR, fname2, CODEZONEDIR );
      //~ do_mudexec( ch, buf );
   //~ }
   //~ return;
}

#ifdef OVERLANDCODE
/* This command copies maps from build port to the others - Samson 8-2-99 */
void do_copymap( CHAR_DATA * ch, char *argument )
{
   //~ char buf[MAX_STRING_LENGTH];
   //~ int valid = 0;
   //~ char *fname;
   //~ char *fnamecheck = NULL;

   //~ if( IS_NPC( ch ) )
   //~ {
      //~ send_to_char( "Mobs cannot use the copymap command!\r\n", ch );
      //~ return;
   //~ }

   //~ if( port != BUILDPORT )
   //~ {
      //~ send_to_char( "&RThe copymap command may only be used from the Builders' port.\r\n", ch );
      //~ return;
   //~ }

   //~ if( argument[0] == '\0' )
   //~ {
      //~ set_char_color( AT_DGREY, ch );
      //~ send_to_char( "You must specify a file to copy.\r\n", ch );
      //~ return;
   //~ }

   //~ if( !str_cmp( argument, "all" ) )
      //~ fname = "*.raw";
   //~ else
   //~ {
      //~ fname = argument;
      //~ strncpy( buf, BUILDMAPDIR, MAX_STRING_LENGTH );
      //~ strncat( buf, fname, MAX_STRING_LENGTH );
      //~ fnamecheck = buf;
      //~ valid = copy_file( ch, fnamecheck );
   //~ }

   //~ if( valid != 0 )
   //~ {
      //~ bug( "do_copymap: Error opening file for copy - %s!", fnamecheck );
      //~ return;
   //~ }

   //~ set_char_color( AT_GREEN, ch );

   //~ if( !sysdata.TESTINGMODE )
   //~ {
      //~ send_to_char( "&RMap file(s) updated to main port.\r\n", ch );
      //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDMAPDIR, fname, MAINMAPDIR );
//~ #ifdef USEGLOB
      //~ do_mudexec( ch, buf );
//~ #else
      //~ command_pipe( ch, buf );
//~ #endif

      //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s*.dat %s", BUILDMAPDIR, MAINMAPDIR );
//~ #ifdef USEGLOB
      //~ do_mudexec( ch, buf );
//~ #else
      //~ command_pipe( ch, buf );
//~ #endif
   //~ }

   //~ send_to_char( "&GMap file(s) updated to code port.\r\n", ch );
   //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDMAPDIR, fname, CODEMAPDIR );
//~ #ifdef USEGLOB
   //~ do_mudexec( ch, buf );
//~ #else
   //~ command_pipe( ch, buf );
//~ #endif

   //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s*.dat %s", BUILDMAPDIR, CODEMAPDIR );
//~ #ifdef USEGLOB
   //~ do_mudexec( ch, buf );
//~ #else
   //~ command_pipe( ch, buf );
//~ #endif

   //~ return;
}
#endif

/* This command copies the social file from build port to the other ports - Samson 5-2-98 */
void do_copysocial( CHAR_DATA * ch, char *argument )
{
   //~ char buf[MAX_STRING_LENGTH];
   //~ int valid = 0;
   //~ char *fnamecheck;

   //~ if( IS_NPC( ch ) )
   //~ {
      //~ send_to_char( "Mobs cannot use the copysocial command!", ch );
      //~ return;
   //~ }

   //~ if( port != BUILDPORT )
   //~ {
      //~ send_to_char( "&RThe copysocial command may only be used from the Builders' port.\r\n", ch );
      //~ return;
   //~ }

   //~ snprintf( buf, MAX_STRING_LENGTH, "%ssocials.dat", BUILDSYSTEMDIR );
   //~ fnamecheck = buf;
   //~ valid = copy_file( ch, fnamecheck );

   //~ if( valid != 0 )
   //~ {
      //~ bug( "do_copysocial: Error opening file for copy - %s!", fnamecheck );
      //~ return;
   //~ }

   //~ set_char_color( AT_GREEN, ch );

   //~ if( !sysdata.TESTINGMODE )
   //~ {
      //~ /*
       //~ * Build port to Main port 
       //~ */
      //~ send_to_char( "&RSocial file updated to main port.\r\n", ch );
      //~ snprintf( buf, MAX_STRING_LENGTH, "cp %ssocials.dat %s", BUILDSYSTEMDIR, MAINSYSTEMDIR );
      //~ do_mudexec( ch, buf );
   //~ }

   //~ /*
    //~ * Build port to Code port 
    //~ */
   //~ send_to_char( "&GSocial file updated to code port.\r\n", ch );
   //~ snprintf( buf, MAX_STRING_LENGTH, "cp %ssocials.dat %s", BUILDSYSTEMDIR, CODESYSTEMDIR );
   //~ do_mudexec( ch, buf );
   //~ return;
}

/* This command copies the morphs file from build port to the other ports - Samson 5-2-98 */
void do_copymorph( CHAR_DATA * ch, char *argument )
{
   //~ char buf[MAX_STRING_LENGTH];
   //~ int valid = 0;
   //~ char *fnamecheck;

   //~ if( IS_NPC( ch ) )
   //~ {
      //~ send_to_char( "Mobs cannot use the copymorph command!", ch );
      //~ return;
   //~ }

   //~ if( port != BUILDPORT )
   //~ {
      //~ send_to_char( "&RThe copymorph command may only be used from the Builders' port.\r\n", ch );
      //~ return;
   //~ }

   //~ snprintf( buf, MAX_STRING_LENGTH, "%smorph.dat", BUILDSYSTEMDIR );
   //~ fnamecheck = buf;
   //~ valid = copy_file( ch, fnamecheck );

   //~ if( valid != 0 )
   //~ {
      //~ bug( "do_copymorph: Error opening file for copy - %s!", fnamecheck );
      //~ return;
   //~ }

   //~ set_char_color( AT_GREEN, ch );

   //~ if( !sysdata.TESTINGMODE )
   //~ {
      //~ /*
       //~ * Build port to Main port 
       //~ */
      //~ send_to_char( "&RPolymorph file updated to main port.\r\n", ch );
      //~ snprintf( buf, MAX_STRING_LENGTH, "cp %smorph.dat %s", BUILDSYSTEMDIR, MAINSYSTEMDIR );
      //~ do_mudexec( ch, buf );
   //~ }

   //~ /*
    //~ * Build port to Code port 
    //~ */
   //~ send_to_char( "&GPolymorph file updated to code port.\r\n", ch );
   //~ snprintf( buf, MAX_STRING_LENGTH, "cp %smorph.dat %s", BUILDSYSTEMDIR, CODESYSTEMDIR );
   //~ do_mudexec( ch, buf );
   //~ return;
}

/* This command copies the mud binary file from code port to main port and build port - Samson 4-7-98 */
void do_copycode( CHAR_DATA * ch, char *argument )
{
   //~ char buf[MAX_STRING_LENGTH];
   //~ int valid = 0;

   //~ if( IS_NPC( ch ) )
   //~ {
      //~ send_to_char( "Mobs cannot use the copycode command!\r\n", ch );
      //~ return;
   //~ }

   //~ if( port != CODEPORT )
   //~ {
      //~ send_to_char( "&RThe copycode command may only be used from the Code Port.\r\n", ch );
      //~ return;
   //~ }

   //~ snprintf( buf, MAX_STRING_LENGTH, "%s%s", TESTCODEDIR, BINARYFILE );
   //~ valid = copy_file( ch, buf );

   //~ if( valid != 0 )
   //~ {
      //~ bug( "do_copycode: Error opening file for copy - %s!", buf );
      //~ return;
   //~ }

   //~ set_char_color( AT_GREEN, ch );

   //~ /*
    //~ * Code port to Builders' port 
    //~ */
   //~ send_to_char( "&GBinary file updated to builder port.\r\n", ch );
   //~ snprintf( buf, MAX_STRING_LENGTH, "cp -f %s%s %s%s", TESTCODEDIR, BINARYFILE, BUILDCODEDIR, BINARYFILE );
   //~ do_mudexec( ch, buf );

   //~ if( !sysdata.TESTINGMODE )
   //~ {
      //~ send_to_char( "&RBinary file updated to main port.\r\n", ch );
      //~ /*
       //~ * Code port to Main port 
       //~ */
      //~ snprintf( buf, MAX_STRING_LENGTH, "cp -f %s%s %s%s", TESTCODEDIR, BINARYFILE, MAINCODEDIR, BINARYFILE );
      //~ do_mudexec( ch, buf );
   //~ }

//~ #ifdef DNS_FILE
   //~ /*
    //~ * Code port to Builders' port 
    //~ */
   //~ send_to_char( "&GDNS Resolver file updated to builder port.\r\n", ch );
   //~ snprintf( buf, MAX_STRING_LENGTH, "cp -f %sresolver %sresolver", TESTCODEDIR, BUILDCODEDIR );
   //~ do_mudexec( ch, buf );

   //~ if( !sysdata.TESTINGMODE )
   //~ {
      //~ send_to_char( "&RDNS Resolver file updated to main port.\r\n", ch );
      //~ /*
       //~ * Code port to Main port 
       //~ */
      //~ snprintf( buf, MAX_STRING_LENGTH, "cp -f %sresolver %sresolver", TESTCODEDIR, MAINCODEDIR );
      //~ do_mudexec( ch, buf );
   //~ }
//~ #endif
   //~ return;
}

/* This command copies race files from build port to main port and code port - Samson 10-13-98 */
void do_copyrace( CHAR_DATA * ch, char *argument )
{
   //~ char buf[MAX_STRING_LENGTH];
   //~ int valid = 0;
   //~ char *fname;
   //~ char *fnamecheck = NULL;

   //~ if( IS_NPC( ch ) )
   //~ {
      //~ send_to_char( "Mobs cannot use the copyrace command!\r\n", ch );
      //~ return;
   //~ }

   //~ if( port != BUILDPORT )
   //~ {
      //~ send_to_char( "&RThe copyrace command may only be used from the Builders' Port.\r\n", ch );
      //~ return;
   //~ }

   //~ if( argument[0] == '\0' )
   //~ {
      //~ set_char_color( AT_DGREY, ch );
      //~ send_to_char( "You must specify a file to copy.\r\n", ch );
      //~ return;
   //~ }

   //~ if( !str_cmp( argument, "all" ) )
   //~ {
      //~ fname = "*.race";
   //~ }
   //~ else
   //~ {
      //~ fname = argument;
      //~ strncpy( buf, BUILDRACEDIR, MAX_STRING_LENGTH );
      //~ strncat( buf, fname, MAX_STRING_LENGTH );
      //~ fnamecheck = buf;
      //~ valid = copy_file( ch, fnamecheck );
   //~ }

   //~ if( valid != 0 )
   //~ {
      //~ bug( "do_copyrace: Error opening file for copy - %s!", fnamecheck );
      //~ return;
   //~ }

   //~ set_char_color( AT_GREEN, ch );

   //~ /*
    //~ * Builders' port to Code port 
    //~ */
   //~ send_to_char( "&GRace file(s) updated to code port.\r\n", ch );
   //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDRACEDIR, fname, CODERACEDIR );
//~ #ifdef USEGLOB
   //~ do_mudexec( ch, buf );
//~ #else
   //~ command_pipe( ch, buf );
//~ #endif

   //~ if( !sysdata.TESTINGMODE )
   //~ {
      //~ /*
       //~ * Builders' port to Main port 
       //~ */
      //~ send_to_char( "&RRace file(s) updated to main port.\r\n", ch );
      //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDRACEDIR, fname, MAINRACEDIR );
//~ #ifdef USEGLOB
      //~ do_mudexec( ch, buf );
//~ #else
      //~ command_pipe( ch, buf );
//~ #endif
   //~ }
   //~ return;
}

/* This command copies deity files from build port to main port and code port - Samson 10-13-98 */
void do_copydeity( CHAR_DATA * ch, char *argument )
{
   //~ char buf[MAX_STRING_LENGTH];
   //~ int valid = 0;
   //~ char *fname;
   //~ char *fnamecheck = NULL;

   //~ if( IS_NPC( ch ) )
   //~ {
      //~ send_to_char( "Mobs cannot use the copydeity command!\r\n", ch );
      //~ return;
   //~ }

   //~ if( port != BUILDPORT )
   //~ {
      //~ send_to_char( "&RThe copydeity command may only be used from the Builders' Port.\r\n", ch );
      //~ return;
   //~ }

   //~ if( argument[0] == '\0' )
   //~ {
      //~ set_char_color( AT_DGREY, ch );
      //~ send_to_char( "You must specify a file to copy.\r\n", ch );
      //~ return;
   //~ }

   //~ if( !str_cmp( argument, "all" ) )
   //~ {
      //~ fname = "*";
   //~ }
   //~ else
   //~ {
      //~ fname = argument;
      //~ strncpy( buf, BUILDDEITYDIR, MAX_STRING_LENGTH );
      //~ strncat( buf, fname, MAX_STRING_LENGTH );
      //~ fnamecheck = buf;
      //~ valid = copy_file( ch, fnamecheck );
   //~ }

   //~ if( valid != 0 )
   //~ {
      //~ bug( "do_copydeity: Error opening file for copy - %s!", fnamecheck );
      //~ return;
   //~ }

   //~ set_char_color( AT_GREEN, ch );

   //~ /*
    //~ * Builders' port to Code port 
    //~ */
   //~ send_to_char( "&GDeity file(s) updated to code port.\r\n", ch );
   //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDDEITYDIR, fname, CODEDEITYDIR );
//~ #ifdef USEGLOB
   //~ do_mudexec( ch, buf );
//~ #else
   //~ command_pipe( ch, buf );
//~ #endif

   //~ if( !sysdata.TESTINGMODE )
   //~ {
      //~ /*
       //~ * Builders' port to Main port 
       //~ */
      //~ send_to_char( "&RDeity file(s) updated to main port.\r\n", ch );
      //~ snprintf( buf, MAX_STRING_LENGTH, "cp %s%s %s", BUILDDEITYDIR, fname, MAINDEITYDIR );
//~ #ifdef USEGLOB
      //~ do_mudexec( ch, buf );
//~ #else
      //~ command_pipe( ch, buf );
//~ #endif
   //~ }
   //~ return;
}

/*
====================
GREP In-Game command	-Nopey
====================
*/
/* Modified by Samson to be a bit less restrictive. So one can grep anywhere the account will allow. */
void do_grep( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   set_char_color( AT_PLAIN, ch );

   if( !argument || argument[0] == '\0' )
      strncpy( buf, "grep --help", MAX_STRING_LENGTH );  /* Will cause it to forward grep's help options to you */
   else
      snprintf( buf, MAX_STRING_LENGTH, "grep -n %s", argument ); /* Line numbers are somewhat important */

#ifdef USEGLOB
   do_mudexec( ch, buf );
#else
   command_pipe( ch, buf );
#endif
   return;
}
