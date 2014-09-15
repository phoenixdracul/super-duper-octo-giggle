/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 							    Color System 	 							*
 * 																			*
 * 	Handles the parsing of color characters '&c &z &O' etc.					*
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
 * AFKMud Copyright 1997-2003 by Roger Libiez (Samson),                     *
 * Levi Beckerson (Whir), Michael Ward (Tarl), Erik Wolfe (Dwip),           *
 * Cameron Carroll (Cam), Cyberfox, Karangi, Rathian, Raine, and Adjani.    *
 * All Rights Reserved.                                                     *
 * 																			*
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,		*
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,		*
 * Grishnakh, Fireblade, and Nivek.											*
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.						*
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,		*
 * Michael Seifert, and Sebastian Hammer.									*
 * 																			*
 +--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--*/

/*
 * The following instructions assume you know at least a little bit about
 * coding.  I firmly believe that if you can't code (at least a little bit),
 * you don't belong running a mud.  So, with that in mind, I don't hold your
 * hand through these instructions.
 *
 * You may use this code provided that:
 *
 *     1)  You understand that the authors _DO NOT_ support this code
 *         Any help you need must be obtained from other sources.  The
 *         authors will ignore any and all requests for help.
 *     2)  You will mention the authors if someone asks about the code.
 *         You will not take credit for the code, but you can take credit
 *         for any enhancements you make.
 *     3)  This message remains intact.
 *
 * If you would like to find out how to send the authors large sums of money,
 * you may e-mail the following address:
 *
 * Matthew Bafford & Christopher Wigginton
 * wiggy@mudservices.com
 */

/*
 * To add new color types:
 *
 * 1.  Edit color.h, and:
 *     1.  Add a new AT_ define.
 *     2.  Increment MAX_COLORS by however many AT_'s you added.
 * 2.  Edit color.c and:
 *     1.  Add the name(s) for your new color(s) to the end of the pc_displays array.
 *     2.  Add the default color(s) to the end of the default_set array.
 */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "mud.h"

char * const pc_displays[MAX_COLORS] =
{
		"black",      "dred",      "dgreen",      "orange",
		"dblue",      "purple",    "cyan",        "grey",
		"dgrey",      "red",       "green",       "yellow",
		"blue",       "pink",      "lblue",       "white",    /*0 - 15*/

		"blink",    "bdred",     "bdgreen",   "bdorange",
		"bdblue",   "bpurple",   "bcyan",     "bgrey",
		"bdgrey",   "bred",      "bgreen",    "byellow",
		"bblue",    "bpink",     "blblue",    "bwhite",


		"immortal",   "note",      "object",      "person",
		"rmdesc",     "rmname",    "ship",                    /*16 - 22*/

		"action",          "consider",    "exits",
		"gold",       "help",      "list",        "oldscore",
		"plain",      "quit",      "report",      "skill",
		"slist",                                              /*23 - 35*/

		"damage",     "fleeing",   "hit",         "hitme",
		"hurt",                                               /*36 - 40 */

		"dead",       "dying",     "falling",     "hungry",
		"poison",     "reset",     "sober",       "thirsty",
		"wearoff",                                            /*41 - 49*/

		"arena",      "auction",   "chat",        "clan",
		"gossip",     "gtell",     "holonet",     "ooc",
		"music",      "say",       "shiptalk",    "shout",
		"social",     "tell",      "wartalk",     "whisper",
		"yells",                                               /*50 - 66*/

		"avatar",     "build",     "comm",        "immtalk",
		"log",        "rflags1",   "rflags2",     "rvnum"     /*67 - 74*/
};

/* All defaults are set to Alsherok default scheme, if you don't 
like it, change it around to suite your own needs - Samson */
const sh_int default_set [MAX_COLORS] =
{
		AT_BLACK,     AT_BLOOD,  AT_DGREEN,     AT_ORANGE, /*  3 */
		AT_DBLUE,     AT_PURPLE, AT_CYAN,       AT_GREY,   /*  7 */
		AT_DGREY,     AT_RED,    AT_GREEN,      AT_YELLOW, /* 11 */
		AT_BLUE,      AT_PINK,   AT_LBLUE,      AT_WHITE,  /* 15 */

		AT_BLACK_BLINK, AT_BLOOD_BLINK, AT_DGREEN_BLINK, AT_ORANGE_BLINK, /* 19 */
		AT_DBLUE_BLINK, AT_PURPLE_BLINK, AT_CYAN_BLINK, AT_GREY_BLINK, /* 23 */
		AT_DGREY_BLINK, AT_RED_BLINK, AT_GREEN_BLINK, AT_YELLOW_BLINK, /* 27 */
		AT_BLUE_BLINK, AT_PINK_BLINK, AT_LBLUE_BLINK, AT_WHITE_BLINK, /* 31 */


		AT_WHITE,     AT_GREEN,   AT_GREEN,      AT_PINK,    /* 19 */
		AT_GREY,      AT_WHITE,   AT_LBLUE,                 /* 22 */

		AT_LBLUE,    AT_CYAN,    AT_GREY,    /* 26 */
		AT_YELLOW,    AT_GREY,   AT_GREY,      AT_LBLUE,    /* 30 */
		AT_GREY,      AT_DGREEN, AT_CYAN,      AT_GREY,    /* 34 */
		AT_GREY,                                           /* 35 */

		AT_RED,      AT_GREY,   AT_RED,      AT_BLOOD,    /* 39 */
		AT_DGREY,                                            /* 40 */

		AT_RED,       AT_BLOOD,   AT_PINK,     AT_ORANGE,    /* 44 */
		AT_WHITE,     AT_DGREY,  AT_CYAN,      AT_BLUE,    /* 48 */
		AT_ORANGE,                                         /* 49 */

		AT_BLUE,      AT_RED,    AT_GREY,      AT_GREY,    /* 53 */
		AT_GREEN,     AT_DGREEN, AT_WHITE,     AT_WHITE,  /* 57 */
		AT_BLUE,      AT_GREY,   AT_DGREY,     AT_YELLOW,  /* 61 */
		AT_GREEN,     AT_DGREY,  AT_RED,       AT_CYAN,    /* 65 */
		AT_YELLOW,                                         /* 66 */

		AT_PINK,      AT_PURPLE, AT_PURPLE,    AT_DGREY,  /* 70 */
		AT_PURPLE,    AT_GREY,   AT_DGREEN,    AT_CYAN   /* 74 */
};

char * const valid_color[] =
{
		"black",	// 0
		"dred",		// 1
		"dgreen",	// 2
		"orange",
		"dblue",
		"purple",	// 5
		"cyan",
		"grey",
		"dgrey",
		"red",
		"green",	// 10
		"yellow",
		"blue",
		"pink",
		"lblue",
		"white",	// 15
		"16",
		"17",
		"18",
		"19",
		"20",
		"21",
		"22",
		"23",
		"24",
		"25",
		"26",
		"27",
		"28",
		"29",
		"30",
		"31",
		"32",
		"33",
		"34",
		"35",
		"36",
		"37",
		"38",
		"39",
		"40",
		"41",
		"42",
		"43",
		"44",
		"45",
		"46",
		"47",
		"48",
		"49",
                "50",
                "51",
                "52",
                "53",
                "54",
                "55",
                "56",
                "57",
                "58",
                "59",
                "60",
                "61",
                "62",
                "63",
                "64",
                "65",
                "66",
                "67",
                "68",
                "69",
                "70",
                "71",
                "72",
                "73",
                "74",
                "75",
                "76",
                "77",
                "78",
                "79",
                "80",
                "81",
                "82",
                "83",
                "84",
                "85",
                "86",
                "87",
                "88",
                "89",
                "90",
                "91",
                "92",
                "93",
                "94",
                "95",
                "96",
                "97",
                "98",
                "99",
                "100",
                "101",
                "102",
                "103",
                "104",
                "105",
                "106",
                "107",
                "108",
                "109",
                "110",
                "111",
                "112",
                "113",
                "114",
                "115",
                "116",
                "117",
                "118",
                "119",
                "120",
                "121",
                "122",
                "123",
                "124",
                "125",
                "126",
                "127",
                "128",
                "129",
                "130",
                "131",
                "132",
                "133",
                "134",
                "135",
                "136",
                "137",
                "138",
                "139",
                "140",
                "141",
                "142",
                "143",
                "144",
                "145",
                "146",
                "147",
                "148",
                "149",
                "150",
                "151",
                "152",
                "153",
                "154",
                "155",
                "156",
                "157",
                "158",
                "159",
                "160",
                "161",
                "162",
                "163",
                "164",
                "165",
                "166",
                "167",
                "168",
                "169",
                "170",
                "171",
                "172",
                "173",
                "174",
                "175",
                "176",
                "177",
                "178",
                "179",
                "180",
                "181",
                "182",
                "183",
                "184",
                "185",
                "186",
                "187",
                "188",
                "189",
                "190",
                "191",
                "192",
                "193",
                "194",
                "195",
                "196",
                "197",
                "198",
                "199",
                "200",
                "201",
                "202",
                "203",
                "204",
                "205",
                "206",
                "207",
                "208",
                "209",
                "210",
                "211",
                "212",
                "213",
                "214",
                "215",
                "216",
                "217",
                "218",
                "219",
                "220",
                "221",
                "222",
                "223",
                "224",
                "225",
                "226",
                "227",
                "228",
                "229",
                "230",
                "231",
                "232",
                "233",
                "234",
                "235",
                "236",
                "237",
                "238",
                "239",
                "240",
                "241",
                "242",
                "243",
                "244",
                "245",
                "246",
                "247",
                "248",
                "249",
                "250",
                "251",
                "252",
                "253",
                "254",
                "255",
		"\0"
};

/* Color align functions by Justice@Aaern */
int const_color_str_len( const char *argument )
{
	int  str, count = 0;
	bool IS_COLOR = FALSE;

	for ( str = 0; argument[str] != '\0'; str++ )
	{
		if ( argument[str] == '&' )
		{
			if ( IS_COLOR == TRUE )
			{
				count++;
				IS_COLOR = FALSE;
			}
			else
				IS_COLOR = TRUE;
		}
#ifdef OVERLANDCODE
else if ( argument[str] == '{' )
#else
	else if ( argument[str] == '^' )
#endif
	{
		if ( IS_COLOR == TRUE )
		{
			count++;
			IS_COLOR = FALSE;
		}
		else
			IS_COLOR = TRUE;
	}
	else
	{
		if ( IS_COLOR == FALSE ) count++;
		else IS_COLOR = FALSE;
	}
	}

	return count;
}

int const_color_strnlen( const char *argument, int maxlength )
{
	int str, count = 0;
	bool IS_COLOR = FALSE;

	for ( str = 0; argument[str] != '\0'; str++ )
	{
		if ( argument[str] == '&' )
		{
			if ( IS_COLOR == TRUE )
			{
				count++;
				IS_COLOR = FALSE;
			}
			else
				IS_COLOR = TRUE;
		}
#ifdef OVERLANDCODE
else if ( argument[str] == '{' )
#else
	else if ( argument[str] == '^' )
#endif
	{
		if ( IS_COLOR == TRUE )
		{
			count++;
			IS_COLOR = FALSE;
		}
		else
			IS_COLOR = TRUE;
	}
	else
	{
		if ( IS_COLOR == FALSE ) count++;
		else IS_COLOR = FALSE;
	}

		if ( count >= maxlength ) break;
	}
	if ( count < maxlength ) return ((str - count) + maxlength);

	str++;
	return str;
}

int color_str_len( char *argument )
{
	int str, count = 0;
	bool IS_COLOR = FALSE;

	for ( str = 0; argument[str] != '\0'; str++ )
	{
		if ( argument[str] == '&' )
		{
			if ( IS_COLOR == TRUE )
			{
				count++;
				IS_COLOR = FALSE;
			}
			else
				IS_COLOR = TRUE;
		}
#ifdef OVERLANDCODE
else if ( argument[str] == '{' )
#else
	else if ( argument[str] == '^' )
#endif
	{
		if ( IS_COLOR == TRUE )
		{
			count++;
			IS_COLOR = FALSE;
		}
		else
			IS_COLOR = TRUE;
	}
	else
	{
		if ( IS_COLOR == FALSE ) count++;
		else IS_COLOR = FALSE;
	}
	}

	return count;
}

int color_strnlen( char *argument, int maxlength )
{
	int str, count = 0;
	bool IS_COLOR = FALSE;

	for ( str = 0; argument[str] != '\0'; str++ )
	{
		if ( argument[str] == '&' )
		{
			if ( IS_COLOR == TRUE )
			{
				count++;
				IS_COLOR = FALSE;
			}
			else
				IS_COLOR = TRUE;
		}
#ifdef OVERLANDCODE
else if ( argument[str] == '{' )
#else
	else if ( argument[str] == '^' )
#endif
	{
		if ( IS_COLOR == TRUE )
		{
			count++;
			IS_COLOR = FALSE;
		}
		else
			IS_COLOR = TRUE;
	}
	else
	{
		if ( IS_COLOR == FALSE ) count++;
		else IS_COLOR = FALSE;
	}

		if ( count >= maxlength ) break;
	}
	if ( count < maxlength ) return ((str - count) + maxlength );

	str++;
	return str;
}

const char *const_color_align( const char *argument, int size, int align )
{
	int space = ( size - const_color_str_len( argument ) );
	static char buf[MAX_STRING_LENGTH];

	if( align == ALIGN_RIGHT || const_color_str_len( argument ) >= size )
		snprintf( buf, MAX_STRING_LENGTH, "%*.*s", const_color_strnlen( argument, size ),
				const_color_strnlen( argument, size ), argument );
	else if( align == ALIGN_CENTER )
		snprintf( buf, MAX_STRING_LENGTH, "%*s%s%*s", ( space/2 ),"",argument,
				( (space/2) * 2 ) == space ? (space/2) : ( (space/2) + 1 ), "" );
	else
		snprintf( buf, MAX_STRING_LENGTH, "%s%*s", argument, space, "" );

	return buf;
}

char *color_align( char *argument, int size, int align )
{
	int space = ( size - color_str_len( argument ) );
	static char buf[MAX_STRING_LENGTH];

	if( align == ALIGN_RIGHT || color_str_len( argument ) >= size )
		snprintf( buf, MAX_STRING_LENGTH, "%*.*s", color_strnlen( argument, size ), color_strnlen( argument, size ), argument );
	else if( align == ALIGN_CENTER )
		snprintf( buf, MAX_STRING_LENGTH, "%*s%s%*s", ( space/2 ), "", argument,
				( (space/2) * 2 ) == space ? (space/2) : ( (space/2) + 1 ), "" );
	else if( align == ALIGN_LEFT )
		snprintf( buf, MAX_STRING_LENGTH, "%s%*s", argument, space, "" );

	return buf;
}

void show_colors( CHAR_DATA *ch )
{
	sh_int count;
	sh_int columns = 0;
	sh_int limit;

	send_to_pager_color( "&BSyntax: color [color type] [color] | default\n\r", ch );
	send_to_pager_color( "&BSyntax: color blink (to see blinking colors)\n\r", ch );
	send_to_pager_color( "&BSyntax: color _reset_ (Resets all colors to default set)\n\r", ch );
	send_to_pager_color( "&BSyntax: color _all_ [color] (Sets all color types to [color])\n\r\n\r", ch );

	send_to_pager_color( "&W********************************[ COLORS ]*********************************\n\r", ch );

	for ( count = 0; count < 16; ++count )
	{
		if ( (count % 8) == 0 && count != 0 )
		{
			send_to_pager( "\n\r",ch );
		}
		pager_printf( ch, "%s%-10s%s", color_str( count, ch ), pc_displays[count], ANSI_RESET);
	}

	send_to_pager( "\n\r\n\r&W******************************[ COLOR TYPES ]******************************\n\r", ch );

	send_to_pager( "&YCharacters, Objects, or Rooms:\n\r",ch);
	for ( count = 32; count < 39; ++count )
	{
		if ( (columns % 8) == 0 && (columns != 0) )
		{
			send_to_pager( "\n\r",ch );
		}
		pager_printf( ch, "%s%-10s%s", color_str( count, ch ), pc_displays[count], ANSI_RESET );
		columns++;
	}

	columns = 0;
	send_to_pager( "\n\r\n\r&YInformational:\n\r",ch);
	for ( count = 40; count < 51; ++count )
	{
		if ( (columns % 8) == 0 && columns != 0)
		{
			send_to_pager( "\n\r",ch );
		}
		pager_printf( ch, "%s%-10s%s", color_str( count, ch ), pc_displays[count], ANSI_RESET );
		columns++;
	}

	columns = 0;
	send_to_pager( "\n\r\n\r&YCombat Messages:\n\r",ch);
	for ( count = 51; count < 56; ++count )
	{
		if ( (columns % 8) == 0 && columns != 0)
		{
			send_to_pager( "\n\r",ch );
		}
		pager_printf( ch, "%s%-10s%s", color_str( count, ch ), pc_displays[count], ANSI_RESET );
		columns++;
	}

	columns = 0;
	send_to_pager( "\n\r\n\r&YCharacter Health Messages:\n\r",ch);
	for ( count = 56; count < 65; ++count )
	{
		if ( (columns % 8) == 0 && columns != 0)
		{
			send_to_pager( "\n\r",ch );
		}
		pager_printf( ch, "%s%-10s%s", color_str( count, ch ), pc_displays[count], ANSI_RESET );
		columns++;
	}

	columns =0;
	send_to_pager( "\n\r\n\r&YChannels:\n\r",ch);
	for ( count = 65; count < 82; ++count )
	{
		if ( (columns % 8) == 0 && columns != 0)
		{
			send_to_pager( "\n\r",ch );
		}
		pager_printf( ch, "%s%-10s%s", color_str( count, ch ), pc_displays[count], ANSI_RESET );
		columns++;
	}

	if( IS_IMMORTAL(ch) )
	{
		columns =0;
		send_to_pager( "\n\r\n\r&YImmortal Colors:\n\r",ch);
		for ( count = 82; count < MAX_COLORS; ++count )
		{
			if ( (columns % 8) == 0 && columns != 0)
			{
				send_to_pager( "\n\r",ch );
			}
			pager_printf( ch, "%s%-10s%s", color_str( count, ch ), pc_displays[count], ANSI_RESET );
			columns++;
		}
	}


	send_to_pager( "\n\r\n\r", ch );
	send_to_pager( "&YAvailable colors are:&w\n\r", ch );

	limit = IS_SET(ch->pcdata->flags, PCFLAG_256COL) ? 256 : 16;
	for ( count = 0; count < limit; ++count)
	{
		if ( (count%8) == 0 && count != 0 )
			send_to_pager( "\n\r", ch );

		pager_printf( ch, "%-10s&w", valid_color[count] );
	}
	send_to_pager( "\n\r", ch );
	return;
}

void do_color( CHAR_DATA *ch, char *argument ) 
{
	bool dMatch, cMatch;
	sh_int count = 0, y = 0;
	char arg[MIL];
	char arg2[MIL];
	char arg3[MIL];
	char buf[MAX_STRING_LENGTH];

	dMatch = FALSE;
	cMatch = FALSE;

	if (IS_NPC(ch))
	{
		send_to_pager( "Only PC's can change colors.\n\r", ch );
		return;
	}

	if ( !argument || argument[0] == '\0' )
	{
		show_colors( ch );
		return;
	}

	argument = one_argument( argument, arg );

	if( !str_cmp( arg, "ansitest" ) )
	{
		sprintf( log_buf, "%sBlack\n\r", ANSI_BLACK );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sDark Red\n\r", ANSI_DRED );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sDark Green\n\r", ANSI_DGREEN );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sOrange/Brown\n\r", ANSI_ORANGE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sDark Blue\n\r", ANSI_DBLUE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sPurple\n\r", ANSI_PURPLE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sCyan\n\r", ANSI_CYAN );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sGrey\n\r", ANSI_GREY );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sDark Grey\n\r", ANSI_DGREY );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sRed\n\r", ANSI_RED );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sGreen\n\r", ANSI_GREEN );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sYellow\n\r", ANSI_YELLOW );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sBlue\n\r", ANSI_BLUE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sPink\n\r", ANSI_PINK );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sLight Blue\n\r", ANSI_LBLUE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sWhite\n\r", ANSI_WHITE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sBlack\n\r", BLINK_BLACK );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sDark Red\n\r", BLINK_DRED );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sDark Green\n\r", BLINK_DGREEN );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sOrange/Brown\n\r", BLINK_ORANGE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sDark Blue\n\r", BLINK_DBLUE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sPurple\n\r", BLINK_PURPLE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sCyan\n\r", BLINK_CYAN );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sGrey\n\r", BLINK_GREY );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sDark Grey\n\r", BLINK_DGREY );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sRed\n\r", BLINK_RED );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sGreen\n\r", BLINK_GREEN );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sYellow\n\r", BLINK_YELLOW );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sBlue\n\r", BLINK_BLUE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sPink\n\r", BLINK_PINK );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sLight Blue\n\r", BLINK_LBLUE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%sWhite\n\r", BLINK_WHITE );
		write_to_buffer( ch->desc, log_buf, 0 );
		write_to_buffer( ch->desc, ANSI_RESET, 0 );
		sprintf( log_buf, "%s%sBlack\n\r", ANSI_WHITE, BACK_BLACK );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%s%sDark Red\n\r", ANSI_BLACK, BACK_DRED );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%s%sDark Green\n\r", ANSI_BLACK, BACK_DGREEN );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%s%sOrange/Brown\n\r", ANSI_BLACK, BACK_ORANGE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%s%sDark Blue\n\r", ANSI_BLACK, BACK_DBLUE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%s%sPurple\n\r", ANSI_BLACK, BACK_PURPLE );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%s%sCyan\n\r", ANSI_BLACK, BACK_CYAN );
		write_to_buffer( ch->desc, log_buf, 0 );
		sprintf( log_buf, "%s%sGrey\n\r", ANSI_BLACK, BACK_GREY );
		write_to_buffer( ch->desc, log_buf, 0 );
		write_to_buffer( ch->desc, ANSI_RESET, 0 );

		return;
	}

	if ( !str_prefix(arg, "_reset_" ) )
	{
		reset_colors(ch);
		send_to_pager( "All color types reset to default colors.\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg2 );

	if ( arg[0] == '\0' )
	{
		send_to_char("Change which color type?\r\n", ch);
		return;
	}

	argument = one_argument( argument, arg3 );

	if ( !str_prefix(arg, "_all_") )
	{
		dMatch = TRUE;
		count = -1;

		/* search for a valid color setting*/
		for ( y = 0; y < 16 ; y++ )
		{
			if (!str_cmp(arg2, valid_color[y]))
			{
				cMatch = TRUE;
				break;
			}
		}
	}
	else if ( arg2[0] == '\0' )
	{
		cMatch = FALSE;
	}
	else
	{
		/* search for the display type and str_cmp*/
		for ( count = 0; count < MAX_COLORS ; count++ )
		{
			if (!str_prefix (arg, pc_displays[count]))
			{
				dMatch = TRUE;
				break;
			}
		}

		if(!dMatch)
		{
			ch_printf( ch, "%s is an invalid color type.\n\r", arg );
			send_to_char( "Type color with no arguments to see available options.\n\r", ch );
			return;
		}

		if (!str_cmp (arg2, "default"))
		{
			ch->colors[count] = default_set[count];
			sprintf(buf, "Display %s set back to default.\n\r", pc_displays[count] );
			send_to_pager( buf, ch );
			return;
		}

		/* search for a valid color setting*/
		for ( y = 0; y < 16 ; y++ )
		{
			if (!str_cmp(arg2, valid_color[y]))
			{
				cMatch = TRUE;
				break;
			}
		}
	}

	if(!cMatch)
	{
		if ( arg[0] )
		{
			ch_printf(ch, "Invalid color for type %s.\n", arg);
		}
		else
		{
			send_to_pager( "Invalid color.\n\r", ch );
		}

		send_to_pager( "Choices are:\n\r", ch );

		for ( count = 0; count < 16; count++ )
		{
			if ( count % 5 == 0 && count != 0 )
				send_to_pager("\r\n", ch);

			pager_printf(ch, "%-10s", valid_color[count]);
		}

		pager_printf(ch, "%-10s\r\n", "default");
		return;
	}
	else
	{
		sprintf(buf, "Color type %s set to color %s.\n\r",count==-1?"_all_":pc_displays[count], valid_color[y] );
	}

	if (!str_cmp (arg3, "blink"))
	{
		y += AT_BLINK;
	}

	if ( count == -1 )
	{
		int ccount;

		for ( ccount = 0; ccount < MAX_COLORS; ++ccount )
		{
			ch->colors[ccount] = y;
		}

		set_pager_color(y, ch);

		sprintf(buf, "All color types set to color %s%s.%s\n\r",
				valid_color[y>AT_BLINK?y-AT_BLINK:y], y>AT_BLINK ? " [BLINKING]" : "", ANSI_RESET );

		send_to_pager(buf, ch);
	}
	else
	{
		ch->colors[count] = y;

		set_pager_color( count , ch);

		if (!str_cmp (arg3, "blink"))
			sprintf(buf, "Display %s set to color %s [BLINKING]%s\n\r", pc_displays[count], valid_color[y-AT_BLINK], ANSI_RESET );
		else
			sprintf(buf, "Display %s set to color %s.\n\r", pc_displays[count], valid_color[y] );

		send_to_pager( buf, ch );
	}
	set_pager_color( AT_PLAIN , ch);

	return;
}

void reset_colors( CHAR_DATA *ch )
{
	memcpy( &ch->colors, &default_set, sizeof(default_set) );
}

char *color_str( sh_int AType, CHAR_DATA *ch )
{
	if( !ch )
	{
		bug( "%s", "color_str: NULL ch!" );
		return( "" );
	}

	if( IS_NPC(ch) || !xIS_SET( ch->act, PLR_ANSI ) )
		return( "" );

	if ( IS_AFFECTED( ch, AFF_INFRARED) )
		return( ANSI_RED);


	switch( ch->colors[AType] )
	{
	case 0:  return( ANSI_BLACK );
	case 1:  return( ANSI_DRED );
	case 2:  return( ANSI_DGREEN );
	case 3:  return( ANSI_ORANGE );
	case 4:  return( ANSI_DBLUE );
	case 5:  return( ANSI_PURPLE );
	case 6:  return( ANSI_CYAN );
	case 7:  return( ANSI_GREY );
	case 8:  return( ANSI_DGREY );
	case 9:  return( ANSI_RED );
	case 10: return( ANSI_GREEN );
	case 11: return( ANSI_YELLOW );
	case 12: return( ANSI_BLUE );
	case 13: return( ANSI_PINK );
	case 14: return( ANSI_LBLUE );
	case 15: return( ANSI_WHITE );

	/* 16 thru 31 are for blinking colors */
	case 16: return( BLINK_BLACK );
	case 17: return( BLINK_DRED );
	case 18: return( BLINK_DGREEN );
	case 19: return( BLINK_ORANGE );
	case 20: return( BLINK_DBLUE );
	case 21: return( BLINK_PURPLE );
	case 22: return( BLINK_CYAN );
	case 23: return( BLINK_GREY );
	case 24: return( BLINK_DGREY );
	case 25: return( BLINK_RED );
	case 26: return( BLINK_GREEN );
	case 27: return( BLINK_YELLOW );
	case 28: return( BLINK_BLUE );
	case 29: return( BLINK_PINK );
	case 30: return( BLINK_LBLUE );
	case 31: return( BLINK_WHITE );

	default: return( ANSI_RESET );
	}
}

char * color256f(int code)
{
	char * buf;
	char tmp[MAX_STRING_LENGTH];

	sprintf(tmp, "\e[38;5;%dm", code);
	buf = STRALLOC(tmp);
	return buf;
}

char * color256b(int code)
{
        char * buf;
        char tmp[MAX_STRING_LENGTH];

        sprintf(tmp, "\e[48;5;%dm", code);
        buf = STRALLOC(tmp);
        return buf;
}

int colorcode( const char *col, char *code, CHAR_DATA *ch )
{
	const char *ctype = col;
	int ln = 0;
	bool ansi;
	int i;
	char tmp[4];

	if( !ch )
		ansi = TRUE;
	else
		ansi = ( ((!IS_NPC(ch)) || ch->desc) && xIS_SET( ch->act, PLR_ANSI ) );

	col++;

	if( !*col )
		ln = -1;
#ifdef OVERLANDCODE
	else if ( *ctype != '&'  )
	{
		bug( "colorcode: command '%c' not '&' ", *ctype );
		ln = -1;
	}
#else
	else if ( *ctype != '&' && *ctype != '^'  )
	{
		bug( "colorcode: command '%c' not '&', '^'", *ctype );
		ln = -1;
	}
#endif
	else if( *col == *ctype )
	{
		code[0] = *col;
		code[1] = '\0';
		ln = 1;
	}
	else if( !ansi )
		ln = 0;
	else
	{
		//   if ( IS_AFFECTED( ch, AFF_INFRARED) )
		//   {
		//    strcpy( code, ANSI_RED);
		//}
		//else
		//	     {

		/* Background color */
		if( *ctype == '^' )
		{
			ln++;
			if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
				strncpy( code, BACK_BLACK, 20 );
			else
			switch( *col )
			{
			default:
				code[0] = *ctype;
				code[1] = *col;
				code[2] = '\0';
				return 2;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				tmp[0] = *col;
				ln++;
				tmp[1] = tmp[2] = tmp[3] = '\0';
				col++;
				if (*col >= '0' && *col <= '9') {
					tmp[1] = *col; ln++; }
				col++;
				if (*col >= '0' && *col <= '9') {
					tmp[2] = *col; ln++; }
				i = atoi(tmp);
				if (IS_SET(ch->pcdata->flags, PCFLAG_256COL))
				{
					char * buf = COLOR256B(i);
					strncpy( code, buf, 20 );
					STRFREE(buf);
				}
				else
				{
					code[0] = '\0';
//					strncpy( code, BACK_BLACK, 20 );
				}
				return ln;
			case 'x': /* Black */
				strncpy( code, BACK_BLACK, 20 );
				break;
			case 'r': /* Dark Red */
				strncpy( code, BACK_DRED, 20 );
				break;
			case 'g': /* Dark Green */
				strncpy( code, BACK_DGREEN, 20 );
				break;
			case 'O': /* Orange/Brown */
				strncpy( code, BACK_ORANGE, 20 );
				break;
			case 'b': /* Dark Blue */
				strncpy( code, BACK_DBLUE, 20 );
				break;
			case 'p': /* Purple */
				strncpy( code, BACK_PURPLE, 20 );
				break;
			case 'c': /* Cyan */
				strncpy( code, BACK_CYAN, 20 );
				break;
			case 'w': /* Grey */
				strncpy( code, BACK_GREY, 20 );
				break;
			}
			ln++;
		}

		/* Foreground text - non-blinking */
		if( *ctype == '&' )
		{
			ln++;
			switch( *col )
			{

			default:
				code[0] = *ctype;
				code[1] = *col;
				code[2] = '\0';
				return 2;
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                                tmp[0] = *col;
				ln++;
                                tmp[1] = tmp[2] = tmp[3] = '\0';
                                col++;
                                if (*col >= '0' && *col <= '9') {
                                        tmp[1] = *col; ln++; }
                                col++;
                                if (*col >= '0' && *col <= '9') {
                                        tmp[2] = *col; ln++; }
                                i = atoi(tmp);
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
				else if (IS_SET(ch->pcdata->flags, PCFLAG_256COL) && !IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
				{
					char * buf = COLOR256F(i);
	                                strncpy( code, buf, 20 );
					STRFREE(buf);
				}
				else
				{
					code[0] = '\0';
//					strncpy( code, ANSI_GREY, 20 );
				}
                                return ln;
			case 'i': /* Italic text */
			case 'I':
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );

				else
					strncpy( code, ANSI_ITALIC, 20 );

				break;

			case 'v': /* Reverse colors */
			case 'V':
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );

				else
					strncpy( code, ANSI_REVERSE, 20 );

				break;

			case 'u': /* Underline */
			case 'U':
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );

				else
					strncpy( code, ANSI_UNDERLINE, 20 );

				break;

			case 's': /* Strikeover */
			case 'S':
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );

				else
					strncpy( code, ANSI_STRIKEOUT, 20 );

				break;

			case 'd': /* Player's client default color */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );

				else
					strncpy( code, ANSI_RESET, 20 );

				break;

			case 'D': /* Reset to custom color for whatever is being displayed */
				strncpy( code, ANSI_RESET, 20 ); /* Yes, this reset here is quite necessary to cancel out other things */
				strncat( code, color_str( ch->desc->pagecolor, ch ), 20 );
				break;

			case 'x': /* Black */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );

				else
					strncpy( code, ANSI_BLACK, 20 );

				break;

			case 'O': /* Orange/Brown */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
				else if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
					strncpy( code, ANSI_DGREY, 20 );
				else
					strncpy( code, ANSI_ORANGE, 20 );

				break;

			case 'c': /* Cyan */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
                                else if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
                                        strncpy( code, ANSI_GREY, 20 );
				else
					strncpy( code, ANSI_CYAN, 20 );

				break;

			case 'z': /* Dark Grey */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );

				else
					strncpy( code, ANSI_DGREY, 20 );

				break;

			case 'g': /* Dark Green */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
                                else if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
                                        strncpy( code, ANSI_GREY, 20 );
				else
					strncpy( code, ANSI_DGREEN, 20 );

				break;

			case 'G': /* Light Green */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
                                else if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
                                        strncpy( code, ANSI_WHITE, 20 );
				else
					strncpy( code, ANSI_GREEN, 20 );

				break;

			case 'P': /* Pink/Light Purple */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
                                else if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
                                        strncpy( code, ANSI_WHITE, 20 );
				else
					strncpy( code, ANSI_PINK, 20 );

				break;

			case 'r': /* Dark Red */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
                                else if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
                                        strncpy( code, ANSI_DGREY, 20 );
				else
					strncpy( code, ANSI_DRED, 20 );

				break;

			case 'b': /* Dark Blue */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
                                else if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
                                        strncpy( code, ANSI_DGREY, 20 );
				else
					strncpy( code, ANSI_DBLUE, 20 );

				break;

			case 'w': /* Grey */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );

				else
					strncpy( code, ANSI_GREY, 20 );

				break;

			case 'Y': /* Yellow */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
                                else if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
                                        strncpy( code, ANSI_WHITE, 20 );
				else
					strncpy( code, ANSI_YELLOW, 20 );

				break;

			case 'C': /* Light Blue */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
                                else if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
                                        strncpy( code, ANSI_WHITE, 20 );
				else
					strncpy( code, ANSI_LBLUE, 20 );

				break;

			case 'p': /* Purple */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
                                else if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
                                        strncpy( code, ANSI_DGREY, 20 );
				else
					strncpy( code, ANSI_PURPLE, 20 );

				break;

			case 'R': /* Red */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
                                else if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
                                        strncpy( code, ANSI_GREY, 20 );
				else
					strncpy( code, ANSI_RED, 20 );

				break;

			case 'B': /* Blue */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );
                                else if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
                                        strncpy( code, ANSI_GREY, 20 );
				else
					strncpy( code, ANSI_BLUE, 20 );

				break;

			case 'W': /* White */
				if ( IS_AFFECTED( ch, AFF_INFRARED) )
					strncpy( code, ANSI_RED, 20 );

				else
					strncpy( code, ANSI_WHITE, 20 );

				break;
				//}
			}
			ln++;
		}
//		ln = strlen( code );
	}
	if ( ln <= 0 )
		*code = '\0';
	return ln;
}


/* Moved from comm.c */
void set_char_color( sh_int AType, CHAR_DATA *ch )
{
	if ( !ch || !ch->desc )
		return;

	if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
		AType = AT_GREY;

	write_to_buffer( ch->desc, color_str( AType, ch ), 0 );

	if( !ch->desc )
	{
		bug( "set_char_color: NULL descriptor after WTB! CH: %s", ch->name ? ch->name : "Unknown?!?" );
		return;
	}

	ch->desc->pagecolor = ch->colors[AType];
}

void set_pager_color( sh_int AType, CHAR_DATA *ch )
{
	if ( !ch || !ch->desc )
		return;

	if (IS_SET(ch->pcdata->flags, PCFLAG_NOIR))
		AType = AT_GREY;

	if ( IS_AFFECTED( ch, AFF_INFRARED) )
		write_to_pager( ch->desc, color_str( 9, ch), 0 );
	else
		write_to_pager( ch->desc, color_str( AType, ch ), 0 );
	if( !ch->desc )
	{
		bug( "set_pager_color: NULL descriptor after WTP! CH: %s", ch->name ? ch->name : "Unknown?!?" );
		return;
	}
	ch->desc->pagecolor = ch->colors[AType];
}

void write_to_pager( DESCRIPTOR_DATA *d, const char *txt, int length )
{
	int pageroffset;	/* Pager fix by thoric */

	if( length <= 0 )
		length = strlen( txt );

	if( length == 0 )
		return;

	if( !d->pagebuf )
	{
		d->pagesize = MAX_STRING_LENGTH;
		CREATE( d->pagebuf, char, d->pagesize );
	}
	if( !d->pagepoint )
	{
		d->pagepoint = d->pagebuf;
		d->pagetop = 0;
		d->pagecmd = '\0';
	}
	if( d->pagetop == 0 && !d->fcommand )
	{
		d->pagebuf[0] = '\n';
		d->pagebuf[1] = '\r';
		d->pagetop = 2;
	}
	pageroffset = d->pagepoint - d->pagebuf;	/* pager fix (goofup fixed 08/21/97) */
	while( d->pagetop + length >= d->pagesize )
	{
		if( d->pagesize > MAX_STRING_LENGTH*16 )
		{
			bug( "%s", "Pager overflow.  Ignoring.\n\r" );
			d->pagetop = 0;
			d->pagepoint = NULL;
			DISPOSE( d->pagebuf );
			d->pagesize = MAX_STRING_LENGTH;
			return;
		}
		d->pagesize *= 2;
		RECREATE( d->pagebuf, char, d->pagesize );
	}
	d->pagepoint = d->pagebuf + pageroffset;	/* pager fix (goofup fixed 08/21/97) */
	strncpy( d->pagebuf + d->pagetop, txt, length );
	d->pagetop += length;
	d->pagebuf[d->pagetop] = '\0';
	return;
}

/* Writes to a descriptor, usually best used when there's no character to send to ( like logins ) */
void send_to_desc_color( const char *txt, DESCRIPTOR_DATA *d )
{
	char *colstr;
	const char *prevstr = txt;
	char colbuf[20];
	int ln;

	if ( !d )
	{
		bug( "%s", "send_to_desc_color: NULL *d" );
		return;
	}

	if ( !txt || !d->descriptor )
		return;

#ifdef OVERLANDCODE
	while( ( colstr = strpbrk( prevstr, "&" ) ) != NULL )
#else
		while( ( colstr = strpbrk( prevstr, "&^" ) ) != NULL )
#endif
		{
			if( colstr > prevstr )
				write_to_buffer( d, prevstr, ( colstr-prevstr ) );

			ln = colorcode( colstr, colbuf, d->character );
			if ( ln < 0 )
			{
				prevstr = colstr+1;
				break;
			}

			else if ( ln > 0 )
				write_to_buffer( d, colbuf, 0 );
			if (ln > 0)
				prevstr = colstr+ln;
			else
				prevstr = colstr+2;
		}
	if ( *prevstr )
		write_to_buffer( d, prevstr, 0 );

	return;
}

/*
 * Write to one char. Convert color into ANSI sequences.
 */
void send_to_char_color( const char *txt, CHAR_DATA *ch )
{
	char *colstr;
	const char *prevstr = txt;
	char colbuf[20];
	int ln;

	if( !ch )
	{
		bug( "%s", "send_to_char_color: NULL ch!" );
		return;
	}

	if( txt && ch->desc )
	{
#ifdef OVERLANDCODE
		while( ( colstr = strpbrk( prevstr, "&" ) ) != NULL )
#else
			while( ( colstr = strpbrk( prevstr, "&^" ) ) != NULL )
#endif
			{
				if( colstr > prevstr )
					write_to_buffer( ch->desc, prevstr, ( colstr-prevstr ) );
				ln = colorcode( colstr, colbuf, ch );
				if ( ln < 0 )
				{
					prevstr = colstr+1;
					break;
				}
				else if( ln > 0 )
					write_to_buffer( ch->desc, colbuf, 0 );
	                        if (ln > 0)
	                                prevstr = colstr+ln;
	                        else
	                                prevstr = colstr+2;
			}
		if ( *prevstr )
			write_to_buffer( ch->desc, prevstr, 0 );
	}
	return;
}

void send_to_pager_color( const char *txt, CHAR_DATA *ch )
{
	char *colstr;
	const char *prevstr = txt;
	char colbuf[20];
	int ln;

	if( !ch )
	{
		bug( "%s", "send_to_pager_color: NULL ch!" );
		return;
	}

	if ( txt && ch->desc )
	{
		DESCRIPTOR_DATA *d = ch->desc;

		ch = d->original ? d->original : d->character;
		if ( IS_NPC(ch) || !IS_SET( ch->pcdata->flags, PCFLAG_PAGERON ) )
		{
			send_to_char_color( txt, d->character );
			return;
		}
#ifdef OVERLANDCODE
		while( ( colstr = strpbrk( prevstr, "&" ) ) != NULL )
#else
			while( ( colstr = strpbrk( prevstr, "&^" ) ) != NULL )
#endif
			{
				if( colstr > prevstr )
					write_to_pager( ch->desc, prevstr, ( colstr-prevstr ) );
				ln = colorcode( colstr, colbuf, ch );
				if ( ln < 0 )
				{
					prevstr = colstr+1;
					break;
				}
				else if( ln > 0 )
					write_to_pager( ch->desc, colbuf, 0 );
	                        if (ln > 0)
	                                prevstr = colstr+ln;
	                        else
	                                prevstr = colstr+2;
			}
		if ( *prevstr )
			write_to_pager( ch->desc, prevstr, 0 );
	}
	return;
}

void send_to_char( const char *txt, CHAR_DATA *ch )
{
	send_to_char_color( txt, ch );
	return;
}

void send_to_pager( const char *txt, CHAR_DATA *ch )
{
	send_to_pager_color( txt, ch );
	return;
}
//newchprintf
void ch_printf( CHAR_DATA *ch, char *fmt, ... )
{
	char buf[MAX_STRING_LENGTH*2];
	va_list args;

	va_start( args, fmt );
	vsprintf( buf, fmt, args );
	va_end( args );

	send_to_char_color( buf, ch );
}

void pager_printf( CHAR_DATA *ch, char *fmt, ... )
{
	char buf[MAX_STRING_LENGTH*2];
	va_list args;

	va_start( args, fmt );
	vsprintf( buf, fmt, args );
	va_end( args );

	send_to_pager_color( buf, ch );
}

/*
 * The primary output interface for formatted output.
 */
/* Major overhaul. -- Alty */
void ch_printf_color( CHAR_DATA *ch, char *fmt, ... )
{
	char buf[MAX_STRING_LENGTH*2];
	va_list args;

	va_start( args, fmt );
	vsnprintf( buf, MAX_STRING_LENGTH*2, fmt, args );
	va_end( args );

	send_to_char( buf, ch );
}

void pager_printf_color( CHAR_DATA *ch, char *fmt, ... )
{
	char buf[MSL*2];
	va_list args;

	va_start( args, fmt );
	vsnprintf( buf, MAX_STRING_LENGTH*2, fmt, args );
	va_end( args );

	send_to_pager( buf, ch );
}
