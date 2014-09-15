/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 							    Ideas System	 							*
 * 																			*
 * 	Manages the idealog and voting on ideas.						     	*
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


#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include "mud.h"

/*
 * Globals
 */
char * current_date   args( ( void ) );
int    num_changes    args( ( void ) );

/*
 * Local Functions
 */

int      maxIdeas;
int		 maxID;
#define  NULLSTR( str )  ( str == NULL || str[0] == '\0' )
IDEAS_DATA * ideas_table;

void load_ideas( )
{
	FILE *fp;
	int i;

	if ( !(fp = fopen( IDEA_FILE, "r")) )
	{
		bug( "Could not open Idea File for reading.", 0 );
		return;
	}

	fscanf( fp, "%d\n", &maxIdeas );
	fscanf( fp, "%d\n", &maxID );
	/* Use malloc so we can realloc later on */
	ideas_table = malloc( sizeof( IDEAS_DATA) * (maxIdeas+1) );
	for( i = 0; i < maxIdeas; i++ )
	{
		ideas_table[i].id = fread_number( fp );
		ideas_table[i].poster = fread_string( fp );
		ideas_table[i].description = fread_string( fp );
		ideas_table[i].players_yes = fread_string_nohash( fp );
		ideas_table[i].players_no = fread_string_nohash( fp );
		ideas_table[i].mudtime = fread_number( fp );
		ideas_table[i].votes = fread_number( fp );
	}
	ideas_table[maxChanges].poster = str_dup("");
	fclose(fp);
	return; /* just return */
}

void save_ideas(void)
{
	FILE *fp;
	int i;

	if ( !(fp = fopen( IDEA_FILE, "w")) )
	{
		perror( IDEA_FILE );
		return;
	}

	fprintf( fp, "%d\n", maxIdeas );
	fprintf( fp, "%d\n", maxID);
	for( i = 0; i < maxIdeas; i++ )
	{
		fprintf (fp, "%d\n", ideas_table[i].id);
		fprintf (fp, "%s~\n", ideas_table[i].poster);
		fprintf (fp, "%s~\n", ideas_table[i].description);
		fprintf (fp, "%s~\n", ideas_table[i].players_yes);
		fprintf (fp, "%s~\n", ideas_table[i].players_no);
		fprintf (fp, "%ld\n",  ideas_table[i].mudtime );
		fprintf (fp, "%d\n",   ideas_table[i].votes);
		fprintf ( fp, "\n" );
	}

	fclose(fp);
	return;
}

void add_idea( CHAR_DATA *ch, char *description )
{
	IDEAS_DATA * new_table;
	new_table = realloc(ideas_table, sizeof( IDEAS_DATA ) * (maxIdeas+2));
	
	if (!new_table) /* realloc failed */
	{
		send_to_char ("Memory allocation failed. Brace for impact.\n\r",ch);
		return;
	}
	ideas_table = new_table;
	
	ideas_table[maxIdeas].id = maxID + 1;
	ideas_table[maxIdeas].poster = STRALLOC(ch->name);
	ideas_table[maxIdeas].description = STRALLOC( description );
	ideas_table[maxIdeas].players_yes = str_dup("");
	ideas_table[maxIdeas].players_no = str_dup("");
	ideas_table[maxIdeas].mudtime = current_time;
	ideas_table[maxIdeas].votes = 0;
	
	maxIdeas += 1;
	maxID += 1;
	
	save_ideas();
	return;
}

void delete_idea(int iIdea)
{
	int i,j;
	IDEAS_DATA * new_table;

	new_table = malloc( sizeof( IDEAS_DATA ) * maxIdeas );

	if( !new_table )
	{
		return;
	}

	for ( i= 0, j = 0; i < maxIdeas + 1; i++)
	{
		if( i != (iIdea) )
		{
			new_table[j] = ideas_table[i];
			j++;
		}
		else if (ideas_table[i].id == maxID)
		{
			maxID -= 1;
		}
	}

	free( ideas_table );
	ideas_table = new_table;

	maxIdeas--;
	save_ideas();
	return;
}

void do_idea( CHAR_DATA *ch, char *argument )
{
	if( argument[0] == '\0' )
	{
		send_to_char("Syntax: idea <your idea>\n        idealog\n\r", ch);
		return;
	}

	add_idea(ch, argument);
	ch_printf( ch, "Thank you for your idea. We at %s are always looking for suggestions. If you have any more, feel free to offer them up. - The %s Staff", sysdata.mud_acronym, sysdata.mud_acronym);
	return;
}

void do_idealog( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int i;
	
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );
	
	if(IS_NPC(ch))
	{
		send_to_char("NPC's can't do that.\n\r", ch);
		return;
	}
	
	if ( arg[0] == '\0' || !str_cmp(arg, "list") || !str_cmp(arg, "show"))
	{
		send_to_char("&YID    Poster        Votes   Idea&d\n\r",ch );
		send_to_char("&b---------------------------------------------------------------------------\n\r", ch );
		for (i = 0; i < maxIdeas; i++)
		{
			sprintf( buf,"&R[&Y%2d&R]&W %-14s %s%-6d &z%-55s&d\n\r",
					ideas_table[i].id,
					ideas_table[i].poster ,
					ideas_table[i].votes == 0 ? "&C*" : ideas_table[i].votes > 0 ? "&G+" : "&R",
					ideas_table[i].votes,
					ideas_table[i].description );
			//change_justify( changes_table[i].change, 55 ));
			send_to_char( buf, ch );
		}
		return;
	}

	if( !str_cmp( arg, "add"))
	{
		/* Rebuild the original arguments. */
		sprintf(buf, "%s %s", arg2, argument);
		do_idea(ch, buf);
		return;
	}
	if ( !str_cmp( arg, "vote") )
	{
		int id = atoi(arg2);
		int index = -1;
		
		int i;
		for (i = 0; i < maxIdeas; i++)
		{
			if(ideas_table[i].id == id)
			{
				index = i;
				break;
			}
		}
		if(index == -1)
		{
			send_to_char("&RThat idea doesn't exist.\n\r", ch);
			return;
		}
		
		if(argument[0] == '\0' || !str_cmp(argument, "yes"))
		{
			if(is_name(ch->name, ideas_table[index].players_yes))
			{
				send_to_char("&RYou have already voted 'yes' on that idea.\n\r", ch);
				return;
			}
			else if(is_name(ch->name, ideas_table[index].players_no))
			{
				send_to_char("&GYour vote has been changed to 'yes'.\n\r", ch);
				
				/* Remove the player's no vote */
				char new_players_no[MAX_STRING_LENGTH];
				char name[MAX_INPUT_LENGTH];
				
				char *namelist;
				namelist = str_dup(ideas_table[index].players_no);
				new_players_no[0] = '\0';
				int count = 0;
				for ( ; ; )
				{
					namelist = one_argument( namelist, name );
					if ( name[0] == '\0' )
						break;
					if ( !str_cmp( ch->name, name ) )
						continue;
					else if(count > 0)
						sprintf(new_players_no, "%s %s", new_players_no, name);
					else
						sprintf(new_players_no, "%s", name);
					count += 1;
				}
				DISPOSE( ideas_table[index].players_no );
				ideas_table[index].players_no = str_dup( new_players_no );
				ideas_table[index].votes = ideas_table[index].votes + 1;
			}
			else
			{
				send_to_char("&GVote accepted. (yes)\n\r", ch);
			}
			ideas_table[index].votes = ideas_table[index].votes + 1;
			
			sprintf( buf, "%s%s%s", ideas_table[index].players_yes, 
					ideas_table[index].players_yes[0] == '\0' ? "" : " ", ch->name );
			DISPOSE( ideas_table[index].players_yes );
			ideas_table[index].players_yes = str_dup( buf );
		}
		else if(!str_cmp(argument, "no"))
		{
			if(is_name(ch->name, ideas_table[index].players_no))
			{
				send_to_char("&RYou have already voted 'no' on that idea.\n\r", ch);
				return;
			}
			else if(is_name(ch->name, ideas_table[index].players_yes))
			{
				send_to_char("&GYour vote has been changed to 'no'.\n\r", ch);
				
				/* Remove the player's yes vote */
				char new_players_yes[MAX_STRING_LENGTH];
				char name[MAX_INPUT_LENGTH];
				
				char *namelist;
				namelist = str_dup(ideas_table[index].players_yes);
				new_players_yes[0] = '\0';
				int count = 0;
				for ( ; ; )
				{
					namelist = one_argument( namelist, name );
					if ( name[0] == '\0' )
						break;
					if ( !str_cmp( ch->name, name ) )
						continue;
					else if(count > 0)
						sprintf(new_players_yes, "%s %s", new_players_yes, name);
					else
						sprintf(new_players_yes, "%s", name);
					count += 1;
				}
				DISPOSE( ideas_table[index].players_yes );
				ideas_table[index].players_yes = str_dup( new_players_yes );
				ideas_table[index].votes = ideas_table[index].votes - 1;
			}
			else
			{
				send_to_char("&GVote accepted. (&Rno&G)\n\r", ch);
			}
			ideas_table[index].votes = ideas_table[index].votes - 1;
			
			sprintf( buf, "%s%s%s", ideas_table[index].players_no,
					ideas_table[index].players_no[0] == '\0' ? "" : " ", ch->name );
			DISPOSE( ideas_table[index].players_no );
			ideas_table[index].players_no = str_dup( buf );
		}
		save_ideas();
		return;
	}
	
	if( !str_cmp(arg, "delete") || !str_cmp(arg, "remove"))
	{
		if( ch->top_level < LEVEL_IMMORTAL)
		{
			send_to_char("&RI don't think so.\n\r", ch);
			return;
		}
		int id = atoi(arg2);
		int index = -1;
		
		int i;
		for (i = 0; i < maxIdeas; i++)
		{
			if(ideas_table[i].id == id)
			{
				index = i;
				break;
			}
		}
		if(index == -1)
		{
			send_to_char("&RThat idea doesn't exist.\n\r", ch);
			return;
		}
		delete_idea(index);
		send_to_char("&GIdea removed.\n\r", ch);
		return;
	}
	if ( !str_cmp( arg, "clear" ) )
	{
		if( ch->top_level < LEVEL_IMPLEMENTOR)
		{
			send_to_char("&RI don't think so.\n\r", ch);
			return;
		}
		if( !str_cmp(arg2, "now") )
		{
			sprintf( buf, "%sideas.txt", SYSTEM_DIR );
			sprintf( buf2, "%sideas.bak", SYSTEM_DIR );
			rename( buf, buf2 );
			send_to_char( "Idealog removed, saved as ideas.bak\n\r", ch);
			
			int i;
			for (i = maxIdeas - 1; i >= 0; i--)
			{
				delete_idea(i);
			}
		}
		else
			send_to_char( "&wType &Ridealog clear now &wto confirm.\n", ch );
		return;
	}
	else
	{
		send_to_char("&RInvalid command. Type &WHELP IDEALOG&R for help.\n\r", ch );
	}
	return;
}