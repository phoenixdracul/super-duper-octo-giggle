/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 							   Accounts Module	 							*
 * 																			*
 * 	Player accounts which group together each player's characters, 			*
 * 	accumulates points, identifies the player on OOC chat etc.				*
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
#include "mccp.h"

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

/* account.c, this file was edited.
 */

/*
 * Global variables.
 */
ACCOUNT_DATA *first_account;
ACCOUNT_DATA *last_account;

void  account_dispose args( ( ACCOUNT_DATA *account ) );
ACCOUNT_CHARACTER_DATA *account_get_character( ACCOUNT_DATA *account, char *name );
void  account_nanny args( ( DESCRIPTOR_DATA *d, char *argument ) );
void  account_save args( ( ACCOUNT_DATA *account ) );
void  account_charater_fread args ( ( ACCOUNT_CHARACTER_DATA *ach, FILE *fp ) );
ACCOUNT_DATA *account_fread args((char *name));
bool account_add_character( ACCOUNT_DATA *account, char *name, bool pending, bool newcharacter );
void account_menu( DESCRIPTOR_DATA * d );
void account_update_character(CHAR_DATA *ch);
bool  check_reconnect( DESCRIPTOR_DATA * d, char *name, bool fConn );
void  show_title( DESCRIPTOR_DATA * d );
bool  check_playing args( ( DESCRIPTOR_DATA *d, char *name, bool kick ) );

const char account_echo_off_str[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const char account_echo_on_str[] = { IAC, WONT, TELOPT_ECHO, '\0' };


void do_account( CHAR_DATA *ch, char *argument)
{
	ACCOUNT_DATA *account;

	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);


	if(IS_NPC(ch))
	{
		send_to_char("&RMobs cannot use accounts! What are you trying to do?\n\r", ch);
		return;
	}
	if(!ch->desc->account && (arg1[0] == '\0' || str_cmp(arg1, "link")))
	{
		send_to_char("&wYour character is not yet linked to an account.\n"
				"Type 'account link <account name> <account password>' to link an account.\n\r", ch);
		return;
	}
	account_update_character(ch);

	if( !str_cmp(arg1, "link") )
	{
		if(ch->desc->account)
		{
			send_to_char("&RThis character is already linked to an account.\n\r", ch);
			return;
		}
		if(arg2[0] == '\0' || argument[0] == '\0')
		{
			send_to_char("&RSyntax: account link <account name> <account password>\n\r", ch);
			return;
		}
		if((account = account_fread( arg2 )) == NULL )
		{
			send_to_char("&RThat account does not exist.\n\r", ch);
			return;
		}
		else if(str_cmp(smaug_crypt(argument), account->password))
		{
			send_to_char("&RInvalid password.\n\r", ch);
			return;
		}

		if(!account_add_character( account, ch->name, FALSE, TRUE ))
		{
			send_to_char("&RError adding character to account. Please inform Administration.\n\r", ch);
			return;
		}
		++account->alts;
		ch->desc->account = account;
		ch->pcdata->account = account;
		if(ch->rppoints && account->points)
			account->points += ch->rppoints;
		else if(ch->rppoints && !account->points)
			account->points = ch->rppoints;
		ch->rppoints = 0;
		account_update_character(ch);
		account_save(account);
		save_char_obj(ch);
		ch_printf(ch, "&G%s is now linked to account: &W@%s\n\r", ch->name, ch->desc->account->name);
		return;
	}
	if(!str_cmp(arg1, "chatname"))
	{
		if(ch->desc->account->chatname)
			ch->desc->account->chatname = FALSE;
		else
			ch->desc->account->chatname = TRUE;
		ch_printf(ch, "&WYour %s name will now display in ooc chats.\n\r",
				ch->desc->account->chatname ? "account" : "character");
		account_save(ch->desc->account);
		return;
	}
	if(!str_cmp(arg1, "email") && ch->desc->account)
	{
		if(arg2[0] == '\0')
		{
			send_to_char("&RSet your account's email address to what?\n\r", ch);
			return;
		}
		if(ch->desc->account->email)
			STRFREE(ch->desc->account->email);
		ch->desc->account->email = STRALLOC(arg2);
		account_save(ch->desc->account);
		ch_printf(ch, "&GYour email address has been set to &W%s\n\r", ch->desc->account->email);
		return;
	}

	if(arg1[0] == '\0' || !str_cmp(arg1, "show"))
		if(ch->desc && ch->desc->account)
			account_menu( ch->desc );
	send_to_desc_color2("&WSyntax: account &Cshow&W/&Cemail&W/&Clink&W/&Cchatname&w\n\r", ch->desc);
}

/* Makes sure account character data matches the actual character */
void account_update_character(CHAR_DATA *ch)
{
	ACCOUNT_CHARACTER_DATA *ach;
	if(!ch->desc || !ch->desc->account || IS_NPC(ch))
		return;
	if((ach = account_get_character(ch->desc->account, ch->name)) == NULL)
		return;

	/* Clan */
	if(ch->pcdata->clan_name)
	{
		if(!ach->clan || str_cmp(ch->pcdata->clan_name, ach->clan))
		{
			if(ach->clan)
				STRFREE(ach->clan);
			ach->clan = STRALLOC(ch->pcdata->clan_name);
			account_save(ch->desc->account);
		}
	}

	/* Password */
	if(ch->pcdata->pwd)
	{
		if(!ach->password || str_cmp(ch->pcdata->pwd, ach->password))
		{
			if(ach->password)
				STRFREE(ach->password);
			ach->password = STRALLOC(ch->pcdata->pwd);
			account_save(ch->desc->account);
		}
	}

	/* Top level */
	if(ch->top_level != ach->top_level)
	{
		ach->top_level = ch->top_level;
		account_save(ch->desc->account);
	}

	/* Last changes */
	if(ch->pcdata)
	{
		if(ch->pcdata->last_changes > ch->pcdata->account->last_changes)
			ch->pcdata->account->last_changes = ch->pcdata->last_changes;
		else
			ch->pcdata->last_changes = ch->pcdata->account->last_changes;
	}
}
bool account_character_playing( char *name )
{
	CHAR_DATA *ch;

	if( !name )
		return FALSE;

	for( ch = first_char; ch; ch = ch->next )
		if( !str_cmp( ch->name, name ) && !IS_NPC( ch ) )
			return TRUE;
	return FALSE;
}

bool account_add_character( ACCOUNT_DATA *account, char *name, bool pending, bool newcharacter )
{
	ACCOUNT_CHARACTER_DATA *ach;
	FILE *fp;
	char filename[256];

	if( !account || !name )
		return FALSE;

	for(ach = account->first_account_character; ach; ach = ach->next)
	{
		if(ach->name && !str_cmp(ach->name, name))
			return TRUE;
	}
	ach = NULL;
	if( newcharacter )
	{
		CREATE( ach, ACCOUNT_CHARACTER_DATA, 1 );
		LINK( ach, account->first_account_character, account->last_account_character, next, prev );

		ach->name = STRALLOC( name );
		ach->password = STRALLOC( "" );
		ach->quit_location = STRALLOC( "New Character: Unknown Location" );
		ach->top_level = 1;
		return TRUE;
	}

	sprintf(filename, "%s%c/%s", PLAYER_DIR, tolower(name[0]), name);
	if((fp = fopen(filename, "r")) != NULL)
	{
		CREATE( ach, ACCOUNT_CHARACTER_DATA, 1);
		LINK( ach, account->first_account_character, account->last_account_character, next, prev );

		ach->name = STRALLOC( name );
		account_charater_fread( ach, fp );

		if( pending )
		{
			account->pending = ach;
		}
		fclose( fp );
		return TRUE;
	}
	else
		return FALSE;
}

ACCOUNT_CHARACTER_DATA *account_get_character( ACCOUNT_DATA *account, char *name )
{
	ACCOUNT_CHARACTER_DATA *ach;

	for( ach = account->first_account_character; ach; ach = ach->next )
		if( !str_cmp( ach->name, name ) )
			return ach;
	return NULL;
}

void account_dispose_character( ACCOUNT_DATA *account, ACCOUNT_CHARACTER_DATA *ach )
{
	if( !ach )
		return;

	if( ach->name )
		STRFREE( ach->name );

	if( ach->clan )
		STRFREE( ach->clan );

	if( ach->password )
		STRFREE( ach->password );

	if( ach->quit_location )
		STRFREE( ach->quit_location );

	UNLINK( ach, account->first_account_character, account->last_account_character, next, prev );
	DISPOSE( ach );
}

void account_dispose( ACCOUNT_DATA *account )
{
	ACCOUNT_CHARACTER_DATA *ach, *next_ach;
	//	DESCRIPTOR_DATA *d;
	CHAR_DATA *c;
	int playing_count = 0;

	/* Check if an alt is still playing... */
	for(c = first_char; c; c = c->next)
	{
		if(c->pcdata && c->pcdata->account && !str_cmp(c->pcdata->account->name, account->name))
			++playing_count;
	}
	/*for(d = first_descriptor; d; d = d->next)
	{
		if(d->account && !str_cmp(d->account->name, account->name))
			++playing_count;
	}*/
	if( !account || playing_count > 0)
		return;

	for( ach = account->first_account_character; ach; ach = next_ach )
	{
		next_ach = ach->next;
		account_dispose_character( account, ach );
	}

	if( account->name )
		STRFREE( account->name );
	if( account->email )
		STRFREE( account->email );
	if( account->host )

		UNLINK( account, first_account, last_account, next, prev );
	DISPOSE( account );
}

ACCOUNT_DATA *account_create( char *name )
{
	ACCOUNT_DATA *account;

	if( !name )
		return NULL;

	CREATE(account, ACCOUNT_DATA, 1);
	LINK(account, first_account, last_account, next, prev );

	account->name = STRALLOC( name );
	return account;
}

ACCOUNT_DATA *account_fread( char *name )
{
	FILE *fp;
	ACCOUNT_DATA *account;
	char filename[256];
	char *word;
	bool fMatch = FALSE;

	if( !name )
		return NULL;

	/* First check if account is already open */
	for(account = first_account; account; account = account->next)
	{
		if(account->name && !str_cmp(account->name, name))
			return account;
	}
	account = 0x0;

	sprintf( filename, "%s%c/%s", ACCOUNT_DIR, tolower( name[0] ), capitalize( name ) );
	if( ( fp = fopen( filename, "r" ) ) != NULL )
	{
		CREATE( account, ACCOUNT_DATA, 1 );
		LINK( account, first_account, last_account, next, prev );

		account->alts = 0;
		account->chatname = FALSE;
		for(;;)
		{
			word = feof( fp ) ? "End" : fread_word( fp );
			fMatch = FALSE;

			switch( UPPER( word[0] ) )
			{
			case 'C':
				if( !str_cmp( word, "Character" ) )
				{
					char *string = fread_string( fp );
					account_add_character( account, string, FALSE, FALSE );
					++account->alts;

					if( string )
						STRFREE( string );

					fMatch = TRUE;
					break;
				}
				KEY( "Chatname", account->chatname, fread_number( fp ) );
				break;
			case 'E':
				if( !str_cmp( word, "End" ) )
					return account;
				KEY( "Email", account->email, fread_string( fp ));
				break;
			case 'H':
				KEY( "Host", account->host, fread_string( fp ) );
				break;
			case 'L':
				KEY( "Last_played", account->last_played, fread_string ( fp ) );
				KEY( "LastChanges", account->last_changes, fread_number(fp));
				break;
			case 'M':
				KEY( "Multiplay", account->multiplay, fread_number( fp ) );
				break;
			case 'N':
				KEY( "Name", account->name, fread_string( fp ) );
				break;
			case 'P':
				KEY( "Password", account->password, fread_string( fp ) ) ;
				KEY( "Points", account->points,	 fread_number( fp ) );
				break;
			case 'T':
				KEY( "Timer", account->timer, fread_number( fp ) );
				break;
			}

			/* Partially expected, so no need to report a bug */
			/*if( !fMatch )
				bug("Account_fread: No match '%s'", 0, word ); */
		}
		fclose( fp );
	}
	else
		return NULL;
	return account;
}

void account_save( ACCOUNT_DATA *account )
{
	FILE *fp;
	ACCOUNT_CHARACTER_DATA *ach;
	char filename[256];

	if( !account )
		return;

	sprintf( filename, "%s%c/%s", ACCOUNT_DIR, tolower( account->name[0] ), capitalize( account->name ) );
	if( ( fp = fopen( filename, "w" ) ) == NULL )
	{
		bug( "account_save: fopen", 0 );
		perror( filename );
	}
	else
	{
		fprintf( fp, "Name          %s~\n", account->name );
		fprintf( fp, "Password      %s~\n", account->password );
		for( ach = account->first_account_character; ach; ach = ach->next )
			fprintf( fp, "Character     %s~\n", ach->name );
		fprintf( fp, "Host          %s~\n", account->host );
		fprintf( fp, "Last_played   %s~\n", account->last_played );
		if(account->email)
			fprintf( fp, "Email		  %s~\n", account->email);
		fprintf(fp, "Points        %d~\n", account->points);
		fprintf(fp, "LastChanges    %ld~\n", account->last_changes);
		fprintf( fp, "Timer         %d~\n", (int) account->timer );
		fprintf( fp, "Multiplay     %d~\n", account->multiplay );
		fprintf( fp, "Chatname      %d~\n", account->chatname);
		fprintf( fp, "End\n");
		fclose( fp );
		fp = NULL;
	}
	return;
}

void account_who( DESCRIPTOR_DATA *d )
{
	char buf[MSL];
	int count = 0;
	DESCRIPTOR_DATA *d2;

	/*
	 * #define WT_IMM    0;
	 * #define WT_MORTAL 1;
	 */

	WHO_DATA *cur_who = NULL;
	WHO_DATA *next_who = NULL;
	WHO_DATA *first_mortal = NULL;
	WHO_DATA *first_imm = NULL;


	for( d2 = last_descriptor; d2; d2 = d2->prev )
	{
		if( d2->connected != CON_PLAYING && d2->connected != CON_EDITING ) continue;
		if( IS_IMMORTAL( d2->character ) && xIS_SET( d2->character->act, PLR_WIZINVIS ) ) continue;

		CREATE( cur_who, WHO_DATA, 1 );
		cur_who->text = d2->character->pcdata->title;

		if( IS_IMMORTAL( d2->character ) )
			cur_who->type = WT_IMM;
		else
			cur_who->type = WT_MORTAL;

		switch ( cur_who->type )
		{
		case WT_MORTAL:
			cur_who->next = first_mortal;
			first_mortal = cur_who;
			break;
		case WT_IMM:
			cur_who->next = first_imm;
			first_imm = cur_who;
			break;
		}
	}

	if( first_mortal )
		sprintf( buf, "&z(  &rPlayers&z  )&R-_-&r^^-_-^^-&R_-&r^^-_-^^-_-^^-&R_-^^-&r_-^^-_-&R^^&z(=====+ &rAdvanced Universe &z+====&z=)&w\n\n" );

	for( cur_who = first_mortal; cur_who; cur_who = next_who )
	{
		send_to_desc_color2( buf, d );
		send_to_desc_color2( cur_who->text, d );
		next_who = cur_who->next;
		count = count + 1;
		//DISPOSE( cur_who->text ); Will this cause a memory leak?
		//DISPOSE( cur_who );
	}

	if( first_imm )
		sprintf( buf, "&z( &rImmortals&z )&R-_&r-^^-_-&R^&r-_-^^-_-^^&R-_-^^&r-_-^^-_-^^-&R_-^^&z(=====+ &rAdvanced Universe &z+====&z=)&w\n\n" );

	for( cur_who = first_imm; cur_who; cur_who = next_who )
	{
		send_to_desc_color2( buf, d );
		send_to_desc_color2( cur_who->text, d );
		next_who = cur_who->next;
		count = count + 1;
		//DISPOSE( cur_who->text ); Will this cause a memory leak?
		//DISPOSE( cur_who );
	}

	sprintf( buf, "\n\r\r\n&z( &R%d &rtotal&z player%s. )&R-_&r-^^-_-&R^^&r-_-^^&R-_-^^&r-_-^^-_-^^-&R_-^^&r-_-^^-_-&R^^-_-^^&r-_-^^-_-^^-&R_-^^&r-_-^^-\r\n", count, count == 1 ? "" : "s" );
	send_to_desc_color2( buf, d );

	return;
}

int get_account_top_level( ACCOUNT_DATA *account )
{
	ACCOUNT_CHARACTER_DATA *ach;
	int top_level = 0;
	if(!account)
		return 0;
	for(ach = account->first_account_character; ach; ach = ach->next)
		if(ach->top_level > top_level)
			top_level = ach->top_level;
	return top_level;
}
void account_menu( DESCRIPTOR_DATA * d )
{
	char buf[MAX_STRING_LENGTH];
	int count = 1;
	ACCOUNT_CHARACTER_DATA *ach;

	if(!d)
		return;
	sprintf(buf, "\n\r&W&G+&z------------------------------------------------------------------------------&G+&W\n\r"
			"&z| &CA&zccount name: &c%-62s &z|\n\r"
			"&z| &CT&zop level: &c%-27d &CP&zoints: &c%-29d &z|\n\r"
			"&z| &CE&zmail: &c%-69s %s&z|\n\r"
			"&z|%-78s|\n\r",
			d->account->name,
			get_account_top_level(d->account), d->account->points,
			d->account->email ? d->account->email : "&w(none)",
					d->account->email ? "" : "  ",
							"");

	if(d->character)
		send_to_char(buf, d->character);
	else
		send_to_desc_color2(buf, d);

	for( ach = d->account->first_account_character; ach; ach = ach->next )
	{
		sprintf( buf, "&W&z| #&C%-1.1d    &CN&zame: &w%-12.12s     &CL&zevel: &c%-3.3d    &CC&zlan: &w%-27.15s &z|\r\n",
				count, ach->name, ach->top_level, ach->clan && ach->clan[0] != '\0' ? ach->clan : "None" );
		count++;
		send_to_desc_color2(buf, d);
	}
	if(count == 1)
		send_to_desc_color2("&z| &wYou don't have any active characters. Type '&Cnew&w' to make one.                &z|\n\r", d);
	send_to_desc_color2("&G+&z------------------------------------------------------------------------------&G+\n\r", d);
	if( d->account->multiplay)
		send_to_desc_color2( "&wThis account may multiplay.\n\r", d );
	if(d->account->chatname)
		send_to_desc_color2( "&wAccount name will be displayed on ooc.\n\r", d);
	else
		send_to_desc_color2( "&wCharacter name will be displayed on ooc.\n\r", d);
	if(get_account_top_level(d->account) >= LEVEL_AVATAR)
		send_to_desc_color2( "&wThis account has immortal status.\n\r", d );
	return;
}

void do_accountstat( CHAR_DATA *ch, char *argument )
{
	char arg[MSL];
	CHAR_DATA *victim;
	DESCRIPTOR_DATA *d;
	ACCOUNT_CHARACTER_DATA *ach;
	int count = 1;

	set_char_color( AT_PLAIN, ch );

	one_argument( argument, arg );

	if( arg[0] == '\0' )
	{
		send_to_char( "Accountstat whom?\r\n", ch );
		return;
	}

	if( arg[0] != '\'' && arg[0] != '"' && strlen( argument ) > strlen( arg ) )
		strcpy( arg, argument );

	if( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\r\n", ch );
		return;
	}

	if( IS_NPC( ch ) )
	{
		send_to_char( "Why would a mob need to accountstat something?\r\n", ch );
		return;
	}

	if( IS_NPC( victim ) )
	{
		send_to_char( "Mobs don't have accounts!\r\n", ch );
		return;
	}

	if(!victim->pcdata->account)
	{
		send_to_char("That character isn't linked to an account!\n\r", ch);
		return;
	}
	if( get_trust( ch ) < get_trust( victim ))
	{
		set_char_color( AT_IMMORT, ch );
		send_to_char( "Their godly glow prevents you from getting a good look.\r\n", ch );
		return;
	}

	account_update_character(victim);

	ch_printf( ch, "&W&GAccount Data for %s\r\n", victim->name );
	ch_printf( ch, "&W&z+------------------------------------------------------------------------------+\r\n" );
	ch_printf( ch, "&W&z| &GAccount Name:&W: %-12.12s      &GCurrent Character Name&W: %-13.13s       &z|\r\n", victim->pcdata->account ? victim->pcdata->account->name : "None", victim ? victim->name : "None");
	ch_printf( ch, "&W&z| &GTop Level&W: %-3.3d                   &GAccount Points&W: %-27d &z|\n\r", get_account_top_level(victim->pcdata->account), victim->pcdata->account->points);
	ch_printf( ch, "&W&z| &GEmail:&W %-69s &z|\n\r", victim->pcdata->account->email ? victim->pcdata->account->email : "(none)");

	for( ach = victim->pcdata->account->first_account_character; ach; ach = ach->next )
	{
		ch_printf( ch, "&W&z| &GCharacter #%-1.1d    Name: %-12.12s     Level: %-3.3d    Clan: %-17.17s &z|\r\n", count, ach->name, ach->top_level, ach->clan ? ach->clan : "None" );
		count++;
	}

	if( victim->pcdata->account->multiplay )
		ch_printf( ch, "&W&z| &GNote: This account can multiplay. &z|\r\n");
	if(victim->pcdata->account->chatname)
		ch_printf( ch, "&W&z| &wAccount name will be displayed on ooc.                                       &z|\n\r");
	else
		ch_printf( ch, "&W&z| &wCharacter name will be displayed on ooc.                                     &z|\n\r");
	if( get_account_top_level(victim->pcdata->account)  >= LEVEL_AVATAR)
		ch_printf( ch, "&W&z| &GNote: This account has immortal status.                                      &z|\r\n");
	ch_printf( ch, "&W&z+------------------------------------------------------------------------------+\r\n" );
	return;
}

void do_setaccount(CHAR_DATA *ch, char *argument)	// TODO
{

}

bool check_multi_account(DESCRIPTOR_DATA *d)	// TODO
{
	return FALSE;
}

void account_charater_fread( ACCOUNT_CHARACTER_DATA *ach, FILE *fp )
{
	ROOM_INDEX_DATA *room;
	char *word;
	bool fMatch = FALSE;
	int room_num = 0;

	for(;;)
	{
		word = feof( fp ) ? "End" : fread_word( fp );
		fMatch = FALSE;

		switch( UPPER( word[0] ) )
		{
		case '*':
			fMatch = TRUE;
			fread_to_eol( fp );
			break;
		case 'C':
			KEY( "Clan", ach->clan, fread_string( fp ) );
			break;
		case 'E':
			if( !str_cmp( word, "End") )
			{
				if( ( room = get_room_index( room_num ) ) != NULL )
					ach->quit_location = STRALLOC( room->name );
				return;
			}
			break;
		case 'P':
			KEY( "Password", ach->password, fread_string( fp ) );
			break;
		case 'R':
			KEY( "Room", room_num, fread_number( fp ) );
			break;
		case 'T':
			KEY( "Toplevel", ach->top_level, fread_number( fp ));
			break;
		}

		if( !fMatch )
			fread_to_eol( fp );
	}
}
void account_prompt( DESCRIPTOR_DATA *d)
{
	send_to_desc_color2( "\r\n&CE&znter a command (&CNew&z/&CName&z/&C#&z/&CEmail&z/&CWho&z/&CQuit&z): &W ", d );
}

void account_nanny( DESCRIPTOR_DATA *d, char *argument )
{
	char ar[MAX_STRING_LENGTH];
	ACCOUNT_CHARACTER_DATA *ach, *account_character;
	DESCRIPTOR_DATA *desc;
	bool chk = FALSE;

	switch( d->connected )
	{
	/*
		default:
			bug( "Account_Nanny: bad d->connected %d.", d->connected );
			close_socket( d, TRUE );
			return;
	 */
	case CON_GET_ACCOUNT:
		if( argument[0] == '\0' )
		{
			write_to_buffer( d, "Illegal name, try another.\r\nName: ", 0 );
			d->connected = CON_GET_NAME;
			return;
		}

		if(argument[0] == '@')
		{
			account_nanny(d, argument + 1);
			return;
		}
		argument[0] = UPPER( argument[0] );
		if( !check_parse_name( argument ) )
		{

			write_to_buffer( d, "Illegal name, try another.\r\nName: ", 0 );
			d->connected = CON_GET_NAME;
			return;
		}

		if( ( d->account = account_fread( argument ) ) != NULL )
		{
			/*if( !d->account->multiplay )
			{
				for( desc = first_descriptor; desc; desc = desc->next )
				{
					if( d != desc && desc->account && !str_cmp( d->host, desc->host ) && str_cmp( argument, desc->account->name)
							&& !str_cmp(desc->host, d->host ) )
					{
						write_to_buffer( d, "Sorry, multiplaying is not allowed...have your other character quit first.\r\n", 0 );
						close_socket( d, FALSE );
					}
				}
			}*/

			write_to_buffer( d, "Enter account password: ", 0 );
			d->connected = CON_GET_OLD_ACCOUNT_PASSWORD;
		}
		else
		{
			if( !check_parse_name( argument ) )
			{
				write_to_buffer( d, "Name is unacceptable. Please choose another\r\nName: ", 0 );
				return;
			}

			for( desc = first_descriptor; desc; desc = desc->next )
			{
				if( d != desc && desc->account && !str_cmp( d->host, desc->host ) && str_cmp( argument, desc->account->name )
						&& !str_cmp( desc->host, d->host ) )
				{
					write_to_buffer( d, "Sorry, only one account allowed per person...\r\n", 0 );
					close_socket( d, FALSE );
				}
			}

			write_to_buffer( d, "Account not found, creating account for name provided.\nDo you accept this name(Y/N): ", 0 );
			d->account = account_create( argument );
			d->account->last_changes = current_time;
			d->connected = CON_COMFIRM_NEW_ACCOUNT;
			return;
		}
		break;

	case CON_GET_OLD_ACCOUNT_PASSWORD:
		if( !strcmp( smaug_crypt( argument ), d->account->password ) )
		{
			if( d->account->host )
			{
				write_to_buffer( d, "\r\nLast logged in from: ", 0 );
				write_to_buffer( d, d->account->host, 0 );
			}

			if( d->account->last_played )
			{
				write_to_buffer( d, "\r\nLast Character played: ", 0 );
				write_to_buffer( d, d->account->last_played, 0 );
			}

			sprintf( log_buf, "Loading Account: %s", d->account->name );
			if( get_account_top_level(d->account) < LEVEL_DEMI )
				log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
			else
				log_string_plus( log_buf, LOG_COMM, get_account_top_level(d->account) );

			account_menu( d );
			account_prompt(d);

			if( d->account->host )
				STRFREE( d->account->host );
			d->account->host = STRALLOC( d->host );
			write_to_buffer( d, account_echo_on_str, 0 );
			d->connected = CON_ACCOUNT_PENDING;
		}
		else
		{
			write_to_buffer( d, "\r\nInvalid password, access denied.\r\n", 0 );
			close_socket( d, FALSE );
			return;
		}
		break;

	case CON_COMFIRM_NEW_ACCOUNT:
		switch( *argument )
		{
		case 'y' : case 'Y':
			sprintf(log_buf, "Creating a new account: %s", d->account->name );
			if( get_account_top_level(d->account) < LEVEL_DEMI )
				log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
			else
				log_string_plus( log_buf, LOG_COMM, get_account_top_level(d->account) );
			write_to_buffer( d, "Enter a password: ", 0 );
			write_to_buffer( d, account_echo_off_str, 0);
			d->connected = CON_GET_NEW_ACCOUNT_PASSWORD;
			break;

		case 'n' : case 'N':
			write_to_buffer( d, "Ok, what is it then? ", 0 );
			//					d->account->desc = NULL;
			account_fread( argument );
			d->account = NULL;
			d->connected = CON_GET_ACCOUNT;
			break;

		default:
			write_to_buffer( d, "Do you accept this name, yes or no? ", 0 );
			break;
		}
		break;

		case CON_GET_NEW_ACCOUNT_PASSWORD:
			if( strlen( argument ) < 5 )
			{
				write_to_buffer( d, "Password must be at least five characters long.\r\nPassword: ", 0 );
				return;
			}

			if( argument[0] == '!' )
			{
				write_to_buffer( d, "Password cannot begin with the '!' character.\r\nPassword: ", 0 );
				return;
			}

			d->account->password = STRALLOC( smaug_crypt( argument ) );
			write_to_buffer( d, "\r\nEnter password again: ", 0 );
			d->connected = CON_COMFIRM_ACCOUNT_PASSWORD;
			break;

		case CON_COMFIRM_ACCOUNT_PASSWORD:
			if( strcmp( smaug_crypt( argument ) , d->account->password ) )
			{
				write_to_buffer( d, "\r\nPasswords do not match.\r\nEnter a password: ", 0 );
				d->connected = CON_GET_NEW_ACCOUNT_PASSWORD;
				STRFREE( d->account->password );
				return;
			}
			write_to_buffer( d, account_echo_on_str, 0 );
			sprintf( log_buf, "Loading Account: %s", d->account->name );
			if(get_account_top_level(d->account) < LEVEL_DEMI )
				log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
			else
				log_string_plus( log_buf, LOG_COMM, get_account_top_level(d->account) );
			account_menu( d );
			account_prompt( d );
			d->connected = CON_ACCOUNT_PENDING;
			break;

		case CON_ACCOUNT_PENDING:

			/* Checks for a negative number */
			one_argument2( argument, ar);
			if( argument[0] == ' ' || argument[0] == '0' || ar[0] == '\0')
			{
				send_to_desc_color2("Huh?\n\r", d);
				account_prompt( d );
				return;
			}
			if(nifty_is_name_prefix( argument, "create" ) || nifty_is_name_prefix(argument, "new"))
			{
				if(d->account->alts >= MAX_ALTS)
				{
					char buf[MSL];
					sprintf( buf, "You may only have '%d' characters. Sorry.\n\n&W ", MAX_ALTS );
					send_to_desc_color2( buf, d );
					return;
				}
				send_to_desc_color2( "Enter desired name for character: ", d );
				d->newstate = 1;
				d->connected = CON_GET_NAME;
				return;
			}

			else if( nifty_is_name_prefix( argument, "who" ) )
			{
				account_who( d );
				account_prompt( d );
				return;
			}

			else if( nifty_is_name_prefix( argument, "quit" )
					|| nifty_is_name_prefix( argument, "exit" ) )
			{
				sprintf(log_buf, "Closing account: %s", d->account->name);
				if( get_account_top_level(d->account) < LEVEL_DEMI )
					log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
				else
					log_string_plus( log_buf, LOG_COMM, get_account_top_level(d->account) );
				send_to_desc_color2( "Goodbye...\n", d );
				account_save( d->account );
				close_socket(d, FALSE);
				return;
			}
			if( !argument )
			{
				account_prompt(d);
				return;
			}

			if( ( ach = account_get_character( d->account, argument ) ) == NULL )
			{
				int i = 0, count = 0;
				i = atoi(argument);
				if(i > 0)
				{
					for(ach = d->account->first_account_character; ach; ach = ach->next)
					{
						if(count >= i - 1)
							break;
						++count;
					}
					if(count != i - 1 || !ach)
					{
						send_to_desc_color2( "\n\rNo such character.\n\n", d );
						account_prompt(d);
						return;
					}
				}
				else
				{
					char arg[MAX_STRING_LENGTH];
					argument = one_argument(argument, arg);
					if(nifty_is_name_prefix(arg, "email"))
					{
						if(argument[0] == '\0')
						{
							send_to_desc_color2("Set your email to what?\n\r", d);
							account_prompt( d );
							return;
						}
						if(d->account->email)
							STRFREE(d->account->email);
						d->account->email = STRALLOC(argument);
						account_save(d->account);

						char buf[MAX_STRING_LENGTH];
						sprintf(buf, "&wYour email address has been set to &W%s\n\r", d->account->email);
						send_to_desc_color2(buf, d);
						account_prompt( d );
						return;
					}
					send_to_desc_color2( "\n\rNo such character.\n\n", d );
					account_prompt(d);
					return;
				}
			}

			if( !d->account->multiplay )
			{
				for( account_character = d->account->first_account_character; account_character; account_character = account_character->next )
				{
					if( !str_cmp( account_character->name, ach->name ) )
						continue;

					/* Put account multiplay checks here. */
					/*if( account_character_playing( account_character->name ) )
						{
							write_to_buffer( d, account_character->name, 0 );
							send_to_desc_color2( " is already playing. No multiplaying allowed.\n\n", d );
							account_prompt(d);
							return;
						}*/
				}
			}

			if( check_playing( d, ach->name, FALSE ) == BERR )
			{
				write_to_buffer( d, "Name: ", 0 );
				return;
			}

			// We aren't using this, but it could be useful sometime.
			/*if( d->account->timer > current_time && d->account->last_played
					&& !d->account->multiplay && str_cmp( ach->name, d->account->last_played ) )
			{
				char buf[MSL];
				int rtime = ( int ) difftime( d->account->timer, current_time );
				int minute = rtime / 60;
				int second = rtime % 60;

				sprintf(buf, "You must wait %d minute%s and %d second%s before playing that character.\n\n",
						minute, minute != 1 ? "s" : "", second, second != 1 ? "s" : "");
				send_to_desc_color2( buf, d );
				account_prompt(d);
				return;
			}*/

			if( load_char_obj( d, ach->name, TRUE, FALSE ) )
			{
				char buf[MSL];
				chk = check_reconnect( d, ach->name, FALSE );

				if( chk == BERR )
					return;

				if( check_playing( d, ach->name, TRUE ) )
					return;

				chk = check_reconnect( d, ach->name, TRUE );
				if( chk == BERR )
				{
					close_socket( d, FALSE );
					return;
				}

				if( chk == TRUE )
					return;

				sprintf(buf, "%s", d->character->name);
				d->character->desc = NULL;
				free_char(d->character);
				load_char_obj(d, buf, FALSE, FALSE);

				if(d->account->last_played)
					STRFREE(d->account->last_played);

				if( d->character->force_identified == 1 && d->character->force_level_status == FORCE_MASTER )
				{
					int ft;
					FORCE_SKILL *skill;
					if( d->character->force_skill[FORCE_SKILL_PARRY] < 50 )
						d->character->force_skill[FORCE_SKILL_PARRY] = 50;
					ft = d->character->force_type;
					for( skill = first_force_skill; skill; skill = skill->next )
						if( ( skill->type == ft || skill->type == FORCE_GENERAL ) && d->character->force_skill[skill->index] < 50
								&& ( strcmp( skill->name, "master" ) && strcmp( skill->name, "student" )
										&& strcmp( skill->name, "promote" ) && strcmp( skill->name, "instruct" ) ) )
							d->character->force_skill[skill->index] = 50;
				}

				d->account->last_played = STRALLOC( ach->name );
				account_save( d->account );
				show_title( d );
			}
			else
			{
				send_to_desc_color2( "Character not found.\n\n", d );
				account_prompt( d );
			}
			return;
			break;
	}
}
