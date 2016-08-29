/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 							  Blueprints Module	 							*
 * 																			*
 * 	Allows player-built structures and ships. Players design blueprints		*
 *  much like builders build areas, but with a more rp-appropriate			*
 *  interface.																*
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
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

static int VNUMS_PER_PRINT = 100;

/*
 * Local functions
 */
void do_makeblueprint 			args((CHAR_DATA *ch, char *argument));
void do_editblueprint 			args((CHAR_DATA *ch, char *argument));	// TODO
int  find_blueprint_vnum_block  args((int num_needed));

void do_makeblueprint(CHAR_DATA *ch, char *argument)
{
	int chance;
	OBJ_DATA *obj = 0x0, *blueprint;

	char arg[MAX_STRING_LENGTH];
	char arg0[MAX_STRING_LENGTH];
	int print_type = 0;	// 1 = Ship; 2 = Structure

	BOOL checkdatapad = FALSE;

	for( obj = ch->last_carrying; obj; obj = obj->prev_content )
	{
		if (obj->item_type == ITEM_DATAPAD || obj->item_type == ITEM_PEN)
		{
			checkdatapad = TRUE;
		}
		if (obj->item_type == ITEM_BLUEPRINT)
		{
			if(!blueprint && obj->value[2] == 0)
				blueprint = obj;
		}
	}
	switch( ch->substate )
	{
	default:
		if(!blueprint)
		{
			send_to_char("&RYou'll need some blank blueprints first.\n\r", ch);
			return;
		}
		if(!checkdatapad)
		{
			send_to_char("&RYou'll need a datapad.\n\r", ch);
			return;
		}
		// TODO Make sure datapad is charged.
		char temp[MAX_STRING_LENGTH];
		strcpy(temp, argument);
		argument = one_argument(argument, arg0);
		if(arg0[0] == '\0' || argument[0] == '\0' || (str_cmp(arg0, "ship") && str_cmp(arg0, "structure")))
		{
			send_to_char("&RSyntax: makeblueprint ship&z/&Rstructure &z<&Rname&z>\n\r", ch);
			return;
		}

		if(!str_cmp(arg0, "ship"))
			print_type = 1;
		else if(!str_cmp(arg0, "structure"))
			print_type = 2;

		//		if(!is_name(argument, "print"))
		//		{
		//			send_to_char("&RThat isn't the name of a blueprint!\n\r", ch);
		//			return;
		//		}
		chance = IS_NPC(ch) ? ch->top_level
				: (int) (ch->pcdata->learned[gsn_makeblueprint]);
		if ( number_percent( ) < chance )
		{
			if(print_type == 1)
				send_to_char( "&GYou begin the process of drawing up a ship blueprint.\n\r", ch);
			else if(print_type == 2)
				send_to_char( "&GYou begin the process of drawing up a structural blueprint.\n\r", ch);

			act( AT_PLAIN, "$n takes out $s datapad and begins drawing up a blueprint.", ch,
					NULL, temp , TO_ROOM );
			add_timer ( ch , TIMER_DO_FUN , 30 , do_makeblueprint , 1 );
			ch->dest_buf   = str_dup(temp);
			return;
		}
		send_to_char("&RYour attempt at blueprinting leaves you hopelessly confused...\n\r",ch);
		learn_from_failure( ch, gsn_makeblueprint );
		return;

	case 1:
		if ( !ch->dest_buf )
			return;
		strcpy(arg, ch->dest_buf);
		DISPOSE( ch->dest_buf);
		break;

	case SUB_TIMER_DO_ABORT:
		DISPOSE( ch->dest_buf );
		ch->substate = SUB_NONE;
		send_to_char("&RYou are interrupted and fail to finish the blueprint.\n\r", ch);
		return;
	}

	ch->substate = SUB_NONE;

	int level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makeblueprint]);
	int vnum = 40;
	OBJ_INDEX_DATA *pObjIndex;

	if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
	{
		send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
		return;
	}

	chance = IS_NPC(ch) ? ch->top_level
			: (int) (ch->pcdata->learned[gsn_makeblueprint]) ;

	if ( number_percent( ) > chance*2  || ( !checkdatapad )  || ( !blueprint ))
	{
		send_to_char( "&RYou finish work on your blueprint, but discover that none of it makes any sense.\n\r", ch);
		learn_from_failure( ch, gsn_makeblueprint );
		return;
	}

	/* Assign blueprint vnums. */
	int startvnum = 0;
	startvnum = find_blueprint_vnum_block(VNUMS_PER_PRINT);
	if(startvnum <= 0)
	{
		send_to_char("&Y[&RERROR&Y]&W: &RCannot assign blueprint vnums - area full. Please notify Administration.\n\r", ch);
		return;
	}
	/* Make all rooms in blueprint range. */
	int location;
	ROOM_INDEX_DATA *room;

	AREA_DATA *tarea;
	for ( tarea = first_area; tarea; tarea = tarea->next )
		if ( !str_cmp( BLUEPRINT_AREA, tarea->filename ) )
			break;
	for(location = startvnum; location < startvnum + VNUMS_PER_PRINT; ++location)
	{
		room = make_room( location, tarea );
		if ( !room )
		{
			send_to_char("&W[&RERROR&W] &RMake_room failed. Please notify Administration.\n\r", ch);
			bug( "Makeblueprint: make_room failed.", 0 );
			return;
		}
	}
	fold_area(tarea, BLUEPRINT_AREA, TRUE);

	separate_obj( blueprint );
	obj_from_char( blueprint );
	extract_obj( blueprint );


	obj = create_object( pObjIndex, level );
	obj->item_type = ITEM_BLUEPRINT;
	SET_BIT( obj->wear_flags, ITEM_HOLD );
	SET_BIT( obj->wear_flags, ITEM_TAKE );
	obj->level = level;
	obj->weight = 1;
	STRFREE( obj->name );
	char buf[MAX_STRING_LENGTH];
	strcpy( arg, one_argument(arg, arg0));
	strcpy( buf, arg );
	strcat( buf, " ");
	strcat( buf, remand(buf));
	strcat( buf, "blueprints");
	obj->name = STRALLOC(buf);
	STRFREE( obj->short_descr );
	strcpy(buf, arg);
	obj->short_descr = STRALLOC( buf );
	STRFREE( obj->description );
	strcat(buf, " was left lying here.");
	obj->description = STRALLOC( buf );
	obj->value[0] = startvnum;
	obj->value[1] = startvnum + VNUMS_PER_PRINT - 1;
	if(!str_cmp(arg0, "ship"))
		obj->value[2] = 1;
	else if(!str_cmp(arg0, "structure"))
		obj->value[2] = 2;

	obj = obj_to_char( obj, ch );

	send_to_char( "&GYou finish drawing up a set of blueprints&w\n\r", ch);
	act( AT_PLAIN, "$n finishes drawing up a set of blueprints.", ch,
			NULL, argument , TO_ROOM );

	{
		long xpgain;

		xpgain = number_range(5000, 10000);
		gain_exp(ch, xpgain, ENGINEERING_ABILITY);
		ch_printf( ch , "You gain %d engineering experience.\n\r", xpgain );
	}
	learn_from_success( ch, gsn_makeblueprint );
	return;
}

void do_editblueprint(CHAR_DATA *ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if(arg1[0] == '\0')
	{
		send_to_char("&RSyntax: editblueprint <print name>\n\r", ch);
		return;
	}
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
		send_to_char( "&RYou don't have a blueprint like that!\n\r", ch );
		return;
	}
	else if(obj->item_type != ITEM_BLUEPRINT)
	{
		send_to_char("&RThat's not a blueprint!\n\r", ch);
		return;
	}
	else if(obj->value[2] == 0)
	{
		send_to_char("&RThat blueprint is blank! You'll need to &rmakeblueprint &Rfirst.\n\r", ch);
		return;
	}
	BOOL checkdatapad = FALSE;
	for( obj = ch->last_carrying; obj; obj = obj->prev_content )
		if (obj->item_type == ITEM_DATAPAD || obj->item_type == ITEM_PEN)
			checkdatapad = TRUE;
	if(!checkdatapad)
	{
		send_to_char("&RYou need a datapad to edit blueprints!\n\r", ch);
		return;
	}

}
int find_blueprint_vnum_block(int num_needed)
{
	BOOL counting = FALSE;
	int count = 0;
	AREA_DATA *tarea;
	int lrange;
	int trange;
	int vnum;
	int startvnum = -1;
	ROOM_INDEX_DATA *room;

	for ( tarea = first_area; tarea; tarea = tarea->next )
		if ( !str_cmp( BLUEPRINT_AREA, tarea->filename ) )
			break;

	if ( str_cmp( BLUEPRINT_AREA, tarea->filename ) )
		return -1;

	lrange = tarea->low_r_vnum;
	trange = tarea->hi_r_vnum;
	for ( vnum = lrange; vnum <= trange; vnum++ )
	{
		//        if ( (room = get_room_index( vnum )) == NULL || !strcmp(room->name,"VNUM_AVAILABLE\0"))
		if ( (room = get_room_index( vnum )) == NULL )
		{
			if(!counting)
			{
				counting = TRUE;
				startvnum = vnum;
			}
			count++;
			if(count == num_needed+1)
				break;
		}
		else if(counting)
		{
			counting = FALSE;
			count = 0;
			startvnum = -1;
		}
	}
	return startvnum;
}
