/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 						    Copyover/Hotboot Module	 					    *
 * 																			*
 *  Allows you to reboot the server onto the most recent executable			*
 *  without having to kick all your descriptors.							*
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
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,		*
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,		*
 * Grishnakh, Fireblade, and Nivek.											*
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.						*
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,		*
 * Michael Seifert, and Sebastian Hammer.									*
 * 																			*
 +--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--*/

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include "mud.h"
/*
 * Socket and TCP/IP stuff.
 */
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <netdb.h>
/*
 * OS-dependent local functions.
 */
int	init_socket		args( ( int port ) );
void	new_descriptor		args( ( int new_desc ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
void    init_descriptor		args( ( DESCRIPTOR_DATA *dnew, int desc) );

/*  Warm reboot stuff, gotta make sure to thank Erwin for this :) */
int                 port;               /* Port number to be used       */
int		    control;		/* Controlling descriptor	*/
int		    control2;		/* Controlling descriptor #2	*/
int		    conclient;		/* MUDClient controlling desc	*/
int		    conjava;		/* JavaMUD controlling desc	*/

void  account_save args( ( ACCOUNT_DATA *account ) );
ACCOUNT_DATA *account_fread( char *name );

extern bool copyover_set;
extern int copytimer_seconds;
extern CHAR_DATA *copyover_ch;

void do_copyover (CHAR_DATA *ch, char * argument)
{
	ACCOUNT_DATA *account;
	FILE *fp;
	DESCRIPTOR_DATA *d, *d_next;
	char buf [100], buf2[100], buf3[100], buf4[100], buf5[100];
	char buffer[MAX_INPUT_LENGTH];

	if(!str_cmp(argument, "abort") || !str_cmp(argument, "stop"))
	{
		if(!copyover_set)
			send_to_char("&RThere is no copyover scheduled.", ch);
		else
		{
			copyover_set = FALSE;
			//sprintf(buffer, "&W--- &YCopyover Aborted. Calm down... &W---");
			sprintf(buffer, "&z--- &RCopyover Aborted. Calm down... &z---");
			echo_to_all(AT_YELLOW, buffer, ECHOTAR_ALL);
		}
		return;
	}

        if( !str_cmp( argument, "warn" ) )
        {
           echo_to_all( AT_RED, "Copyover Warning", ECHOTAR_ALL );
           return;
        }

	if( isdigit( argument[0] ) && atoi(argument) != -1)
	{
		if(copyover_set)
		{
			ch_printf(ch, "&WA copyover is scheduled for &Y%i &Wseconds. Type &RCOPYOVER ABORT &Wto stop it.\n\r", copytimer_seconds);
			return;
		}
		copyover_set = TRUE;
		copytimer_seconds = atoi(argument);
		copyover_ch = ch;
		sprintf(buffer, "&z--- &RCopyover in &w%i &Rseconds. &z---", copytimer_seconds);
		echo_to_all(AT_YELLOW, buffer, ECHOTAR_ALL);
		return;
	}
	fp = fopen (COPYOVER_FILE, "w");

	if (!fp)
	{
		send_to_char ("Copyover file not writeable, aborted.\n\r",ch);
		log_printf ("Could not write to copyover file: %s", COPYOVER_FILE);
		perror ("do_copyover:fopen");
		return;
	}

	/* Consider changing all saved areas here, if you use OLC */
	for( account = first_account; account; account = account->next )
		account_save( account );

	/* do_asave (NULL, ""); - autosave changed areas */

	snprintf(buf, MSL, "\n\r%s[%sCopyover%s] %sby %s%s%s - %sPlease remain seated!%s\n\r",
			ANSI_DGREY, ANSI_RED, ANSI_DGREY, ANSI_RED, ANSI_GREY, ch->name, ANSI_DGREY, ANSI_RED, ANSI_GREY);
	copyover_set = FALSE;	// Disable the copyover timer.
	//	snprintf(buf, MSL, "\n\r%sYou feel a great disturbance in the Force.%s\n\r", ANSI_YELLOW, ANSI_GREY);
	/* For each playing descriptor, save its state */
	for (d = first_descriptor; d ; d = d_next)
	{
		CHAR_DATA * och = CH(d);
		d_next = d->next; /* We delete from the list , so need to save this */
		if (!d->character || d->connected != CON_PLAYING) /* drop those logging on */
		{
			write_to_descriptor (d->descriptor, "\n\rSorry, we are rebooting."
					" Come back in a few minutes.\n\r", 0);
			close_socket (d, FALSE); /* throw'em out */
		}
		else
		{
			fprintf (fp, "%d %s %s %s\n", d->descriptor, och->name, d->account ? d->account->name : "None", d->host);
			if (och->top_level == 1)
			{
				write_to_descriptor (d->descriptor, "Since you are level one,"
						"and level one characters do not save, you gain a free level!\n\r",
						0);
				/*      advance_level (och, ability); */
				och->top_level++; /* Advance_level doesn't do that */
			}
			save_char_obj (och);
			write_to_descriptor (d->descriptor, buf, 0);
		}
	}
	fprintf (fp, "-1\n");
	fclose (fp);

	/* Close reserve and other always-open files and release other resources */
	fclose (fpReserve);
	fclose (fpLOG);

	/* exec - descriptors are inherited */

	sprintf (buf, "%d", port);
	sprintf (buf2, "%d", control);
	sprintf (buf3, "%d", control2);
	sprintf (buf4, "%d", conclient);
	sprintf (buf5, "%d", conjava);

	execl (EXE_FILE, "swreality", buf, "copyover", buf2, buf3,
			buf4, buf5, (char *) NULL);

	/* Failed - sucessful exec will not return */

	perror ("do_copyover: execl");
	send_to_char ("Copyover FAILED!\n\r",ch);

	/* Here you might want to reopen fpReserve */
	/* Since I'm a neophyte type guy, I'll assume this is
           a good idea and cut and past from main()  */

	if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
	{
		perror( NULL_FILE );
		exit( 1 );
	}
	if ( ( fpLOG = fopen( NULL_FILE, "r" ) ) == NULL )
	{
		perror( NULL_FILE );
		exit( 1 );
	}

}

/* Recover from a copyover - load players */
void copyover_recover ()
{
	DESCRIPTOR_DATA *d;
	FILE *fp;
	char name [100];
	char host[MAX_STRING_LENGTH];
	char account[MAX_STRING_LENGTH];
	int desc;
	bool fOld;
	/*  CHAR_DATA *ch; */ /* Uncomment This Line if putting functions in comm.c -Iczer */
	log_string ("Copyover recovery initiated");

	fp = fopen (COPYOVER_FILE, "r");

	if (!fp) /* there are some descriptors open which will hang forever then ? */
	{
		perror ("copyover_recover:fopen");
		log_string("Copyover file not found. Exitting.\n\r");
		exit (1);
	}

	unlink (COPYOVER_FILE); /* In case something crashes
                              - doesn't prevent reading */
	for (;;)
	{
		fscanf (fp, "%d %s %s %s\n", &desc, name, account, host);
		if (desc == -1)
			break;

		/* Write something, and check if it goes error-free */
		if (!write_to_descriptor (desc, "\n\rThe ether swirls in chaos.\n\r", 0))
		{
			close (desc); /* nope */
			continue;
		}

		CREATE(d, DESCRIPTOR_DATA, 1);
		init_descriptor (d, desc); /* set up various stuff */

		d->host = STRALLOC( host );
		if(str_cmp(account, "None"))
			d->account = account_fread(account);

		LINK( d, first_descriptor, last_descriptor, next, prev );
		//		d->connected = CON_COPYOVER_RECOVER; /* negative so close_socket
		//                                              will cut them off */

		d->connected = CON_GET_ACCOUNT;

		/* Now, find the pfile */

		fOld = load_char_obj (d, name, FALSE, TRUE);

		if (!fOld) /* Player file not found?! */
		{
			write_to_descriptor (desc, "\n\rSomehow, your character was lost in the copyover. Notify an Administrator.\n\r", 0);
			close_socket (d, FALSE);
		}
		else /* ok! */
		{
			act( AT_MAGIC, "You appear in a swirl of the Force!\n\r", d->character, NULL, NULL, TO_CHAR );
			act( AT_MAGIC, "$n appears in a swirl of the Force!\n\r", d->character, NULL, NULL, TO_ROOM );

			/* Load up builders' areas - Boran */
			if ( d->character->pcdata->area )
			{
				do_loadarea (d->character , "" );
				assign_area( d->character );
			}
			/* Just In Case,  Someone said this isn't necassary, but _why_
              do we want to dump someone in limbo? */
			if (!d->character->in_room)
				d->character->in_room = get_room_index (ROOM_VNUM_TEMPLE);

			/* Insert in the char_list */
			LINK( d->character, first_char, last_char, next, prev );

			char_to_room (d->character, d->character->in_room);
			//			do_look (d->character, "auto noprog");
			load_home(d->character);
			d->connected = CON_PLAYING;
		}

	}
	fclose (fp);
}
