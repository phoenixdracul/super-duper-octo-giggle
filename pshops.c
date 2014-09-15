/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 						      Player Shop System 					    	*
 * 																			*
 *  Player shop system. Players can buyvendor, placevendor, pricevendor,	*
 *  sellvendor and collectcredits.											*
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

/*
 * Local functions
 */
void do_buyvendor(CHAR_DATA *ch, char *argument);		/* Buy a vendor agreement */
void do_placevendor(CHAR_DATA *ch, char *argument);		/* Use your vendor agreement to place vendor in current room */
void do_pricevendor(CHAR_DATA *ch, char *argument);		/* Set the price of an item which your vendor is selling	*/
void do_sellvendor(CHAR_DATA *ch, char *argument);		/* Sell your shop - Gives < 200000 credits		*/
void delete_vendor( CHAR_DATA *mob, CHAR_DATA *ch);
void do_collectcredits(CHAR_DATA *ch, char *argument);	/* Collect the credits that vendor has made by trading (Charge 5%)	*/
void do_setshop(CHAR_DATA *ch, char *argument);			/* Set the parameters of a pshop - Name, desc, etc.		*/
void do_closeshop(CHAR_DATA *ch, char *argument);		/* Allows the government of a planet to close pshops on planet	*/

/*
 * External functions
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch );
void 	   instaroom( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoom, bool dodoors );
void	   save_vendor(ROOM_INDEX_DATA *room);
void       delete_reset(RESET_DATA * pReset);

void do_buyvendor(CHAR_DATA *ch, char *argument)
{
	OBJ_DATA *agreement;
	OBJ_INDEX_DATA *obj_index;

	/* Make sure that there is a keeper in the current room */
	if(!find_keeper(ch))
	{
		send_to_char("&RThere is no vendor in the current room.\n\r", ch);
		return;
	}

	/* Check player's gold */
	if(ch->gold < 200000)
	{
		send_to_char("&RYou cannot afford to purchase a vendor.\n\r", ch);
		return;
	}

	/* Invoke the vendors agreement */
	obj_index = get_obj_index( OBJ_VNUM_VENDOR_AGREEMENT );
	if(!obj_index)
	{
		send_to_char("&W[&RERROR&W] &RObject cannot be created. Notify an Administrator.\n\r", ch);
		return;
	}

	agreement = create_object( obj_index, 0);

	/* Make it non-prototype. */
	if(IS_SET(agreement->extra_flags, ITEM_PROTOTYPE))
		REMOVE_BIT(agreement->extra_flags, ITEM_PROTOTYPE);

	agreement = obj_to_char( agreement, ch);
	ch->gold -= 200000;
	send_to_char("&GA vendor has agreed to maintain your shop.\n\r&YYou pay 200,000 credits.\n\r&wHe signs and hands over an agreement document.\n\r", ch);
	return;
}

void do_placevendor(CHAR_DATA *ch, char *argument)
{
	SHOP_DATA *shop;
	bool place_clan = FALSE;

	if(argument[0] != '\0' && !str_cmp(argument, "clan"))
		place_clan = TRUE;

	if(place_clan && !ch->pcdata->clan)
	{
		send_to_char("&RYou are not in a clan!\n\r", ch);
		return;
	}
	if(place_clan && ch->pcdata->bestowments
			&& !is_name("clanshops", ch->pcdata->bestowments))
	{
		send_to_char("&RYou are not bestowed with the ability to handle clan shops.\n\r", ch);
		return;
	}

	/* Make sure room is a player shop */
	if(!IS_SET(ch->in_room->room_flags2, ROOM_PLR_SHOP))
	{
		send_to_char("&RYou cannot place a vendor here.\n\r", ch);
		return;
	}

	/* See if there is another vendor in the room */
	CHAR_DATA *keeper;
	SHOP_DATA *pShop;

	pShop = NULL;
	for ( keeper = ch->in_room->first_person;
			keeper;
			keeper = keeper->next_in_room )
		if ( IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL )
			break;

	if ( pShop )
	{
		send_to_char( "&RThere is already a vendor in this shop.\n\r", ch );
		return;
	}

	/* Find a vendors' agreement in the player's inventory */
	OBJ_DATA *object, *agreement = 0x0;
	if(ch->first_carrying)
		for(object = ch->first_carrying; object; object = object->next)
		{
			if(object->pIndexData && object->pIndexData->vnum == OBJ_VNUM_VENDOR_AGREEMENT)
			{
				agreement = object;
				break;
			}
		}

	if(!agreement)
	{
		send_to_char("&RYou must have a vendor first (&WHELP PSHOPS&R).\n\r", ch);
		return;
	}

	/* Remove vendor agreement from player's inventory */
	separate_obj( agreement );
	obj_from_char( agreement );
	extract_obj( agreement );

	CHAR_DATA *vendor = 0x0;
	MOB_INDEX_DATA *mob_index = 0x0;

	/* Find an available vnum for the vendor */
	int index = 0;
	int m_vnum;
	for(index = ch->in_room->area->low_m_vnum; index < ch->in_room->area->hi_m_vnum; ++index)
	{
		if(get_mob_index(index) == NULL)
		{
			m_vnum = index;
			break;
		}
	}
	if(m_vnum != 0)
		mob_index = make_mobile(m_vnum, MOB_VNUM_VENDOR, "vendor");

	if(!mob_index)
	{
		send_to_char("&R[&WERROR&R] &WCan't create the vendor mob. Please notify an Administrator.\n\r", ch);
		return;
	}

	vendor = create_mobile(mob_index);

	if(xIS_SET(vendor->act, ACT_PROTOTYPE))
		xREMOVE_BIT(vendor->act, ACT_PROTOTYPE);
	CREATE( shop, SHOP_DATA, 1 );
	LINK( shop, first_shop, last_shop, next, prev );

	shop->keeper	= m_vnum;
	shop->open_hour	= 0;
	shop->close_hour	= 23;

	if(!place_clan)
	{
		shop->playershop	= TRUE;
		shop->clanshop		= FALSE;
		STRFREE(shop->owner);
		shop->owner = STRALLOC(ch->name);
	}
	else
	{
		shop->playershop	= FALSE;
		shop->clanshop		= TRUE;
		STRFREE(shop->owner);
		shop->owner = STRALLOC(ch->pcdata->clan_name);
	}

	vendor->pIndexData->pShop		= shop;
	char_to_room( vendor, ch->in_room );

	act( AT_PLAIN, "&GA vendor walks into the shop and sets himself up.\n\r", ch, 0, NULL, TO_CHAR );
	act( AT_PLAIN, "&GA vendor walks into the shop and sets himself up.\n\r", ch, 0, NULL, TO_ROOM );


	instaroom( ch, ch->in_room, FALSE );
	fold_area(ch->in_room->area, ch->in_room->area->filename, TRUE);
	reset_area(ch->in_room->area);
	return;
}

void do_pricevendor(CHAR_DATA *ch, char *argument)
{
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(arg1[0] == '\0')
	{
		send_to_char("&RSyntax: pricevendor &W<&Rname/num&W> <&Rnew price&W>&w\n\r", ch);
		return;
	}

	/* Check if the keeper in the room belongs to the player */
	CHAR_DATA *keeper;
	SHOP_DATA *pShop;

	pShop = NULL;
	for ( keeper = ch->in_room->first_person;
			keeper;
			keeper = keeper->next_in_room )
		if ( IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL )
			break;

	if ( !pShop )
	{
		send_to_char( "&RYou can't do that here!\n\r", ch );
		return;
	}
	else if( !pShop->owner || str_cmp(pShop->owner, ch->name ))
	{
		send_to_char("&RThat keeper isn't employed by you.\n\r", ch);
		return;
	}

	/* Get the item */
	OBJ_DATA *obj;

	obj  = get_obj_carry( keeper, arg1 );

	if ( !obj && arg1[0] == '#' )
	{
		int onum, oref;
		bool ofound = FALSE;

		onum =0;
		oref = atoi(arg1+1);
		for ( obj = keeper->last_carrying; obj; obj = obj->prev_content )
		{
			if ( obj->wear_loc == WEAR_NONE
					&&   can_see_obj( ch, obj ) )
				onum++;
			if ( onum == oref )
			{
				ofound = TRUE;
				break;
			}
			else if ( onum > oref )
				break;
		}
		if (!ofound)
			obj = NULL;
	}
	if(!obj)
	{
		send_to_char("&RNo item like that exists.\n\r", ch);
		return;
	}

	if(arg2[0] == '\0')
	{
		send_to_char("&RWhat price do you want to set it at?\n\r", ch);
		return;
	}

	/* Make sure price is valid */
	int price;
	price = atoi(arg2);

	if(price <= 0 || price > 5000000)
	{
		send_to_char("&RInvalid price.\n\r", ch);
		return;
	}

	send_to_char("&GPrice set.\n\r", ch);
	obj->cost = price;
	save_vendor(keeper->in_room);
	return;
}

void do_sellvendor(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *keeper;
	SHOP_DATA *pShop;

	pShop = NULL;
	for ( keeper = ch->in_room->first_person;
			keeper;
			keeper = keeper->next_in_room )
		if ( IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL )
			break;

	if ( !pShop )
	{
		send_to_char( "&RThere is no vendor here.\n\r", ch );
		return;
	}

	if((ch->pcdata->clan_name && pShop->clanshop && !str_cmp(pShop->owner, ch->pcdata->clan_name))
			|| (pShop->playershop && !str_cmp(pShop->owner, ch->name)))
	{
		send_to_char("&YYou get 150,000 credits.\n", ch);
		send_to_char("&GThe vendor packs up shop and leaves.\n\r", ch);
		delete_vendor( keeper, ch );
	}
	else
	{
		send_to_char("&RThis vendor is not yours to sell!\n\r", ch);
		return;
	}
}

void delete_vendor( CHAR_DATA *mob, CHAR_DATA *ch)
{
	RESET_DATA *reset;
	AREA_DATA *area;

	area = mob->in_room->area;
	if((reset = mob->in_room->first_reset) != NULL)
	{
		for(;reset;reset = reset->next)
		{
			if(reset->command && reset->command == 'M'
				&& get_mob_index(reset->arg1) == mob->pIndexData)
			{
				UNLINK( reset, mob->in_room->first_reset, mob->in_room->last_reset, next, prev );
				delete_reset( reset );
				delete_mob( mob->pIndexData );
				fold_area(area, area->filename, TRUE);
				return;
			}
		}
	}
	return;
}

void do_setshop(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *location;

	argument = one_argument(argument, arg);

	CHAR_DATA *keeper;
	SHOP_DATA *pShop;

	switch ( ch->substate )
   {
	  default:
		 break;
	  case SUB_ROOM_DESC:
		 location = ch->dest_buf;
		 if( !location )
		 {
			bug( "setshop: sub_room_desc: NULL ch->dest_buf", 0 );
			location = ch->in_room;
		 }
		 STRFREE( location->description );
		 location->description = copy_buffer( ch );
		 stop_editing( ch );
		 ch->substate = ch->tempnum;
		 fold_area(ch->in_room->area, ch->in_room->area->filename, FALSE);
		 return;
   }

	pShop = NULL;
	for ( keeper = ch->in_room->first_person;
			keeper;
			keeper = keeper->next_in_room )
		if ( IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL )
			break;

	if ( !pShop )
	{
		send_to_char( "&wYou can't do that here!\n\r", ch );
		return;
	}

	if(!((ch->pcdata->clan_name && pShop->clanshop && !str_cmp(pShop->owner, ch->pcdata->clan_name))
			|| (pShop->playershop && !str_cmp(pShop->owner, ch->name))))
	{
		send_to_char("&RThis isn't your shop.\n\r", ch);
		return;
	}
	if( arg[0] == '\0' || (str_cmp( arg, "desc" ) && str_cmp(arg, "name")))
	{
		send_to_char( "Syntax: setshop <name/desc> value\r\n", ch );
	    return;
	}

	location = keeper->in_room;

	if( !str_cmp( arg, "name" ) )
   {
	  if( argument[0] == '\0' )
	  {
		 send_to_char( "Set the room name.  A very brief single line room description.\r\n", ch );
		 send_to_char( "Usage: setshop name <Room summary>\r\n", ch );
		 return;
	  }
	  STRFREE( location->name );
	  location->name = STRALLOC( argument );
	  fold_area(keeper->in_room->area, keeper->in_room->area->filename, FALSE);
	  return;
   }

	if( !str_cmp( arg, "desc" ) )
   {
	  if( ch->substate == SUB_REPEATCMD )
		 ch->tempnum = SUB_REPEATCMD;
	  else
		 ch->tempnum = SUB_NONE;
	  ch->substate = SUB_ROOM_DESC;
	  ch->dest_buf = location;
	  start_editing( ch, location->description );
	  editor_desc_printf( ch, "Description of room vnum %d (%s).", location->vnum, location->name );
	  return;
   }
}

void do_collectcredits(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *keeper;
	SHOP_DATA *pShop;

	pShop = NULL;
	for ( keeper = ch->in_room->first_person;
			keeper;
			keeper = keeper->next_in_room )
		if ( IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL )
			break;

	if ( !pShop )
	{
		send_to_char( "&RThere is no vendor here.\n\r", ch );
		return;
	}

	if((ch->pcdata->clan_name && pShop->clanshop && !str_cmp(pShop->owner, ch->pcdata->clan_name))
			|| (pShop->playershop && !str_cmp(pShop->owner, ch->name)))
	{
		if(keeper->gold <= 0)
		{
			act(AT_PLAIN, "$t shakes his head: \"&WI haven't made any credits. Come back later.&C\"", ch, keeper->short_descr, NULL, TO_CHAR);
			return;
		}
		int amount = keeper->gold;
		keeper->gold -= amount;
		boost_economy(keeper->in_room->area, (int)(amount * 5 / 100));
		ch->gold += (int)(amount * 95 / 100);
		save_vendor(keeper->in_room);
		ch_printf(ch, "&GThe shopkeeper takes 5%% commission and hands over &W%d &Gcredits.\n\r", (int)(amount * 95 / 100));
	}
	else
	{
		send_to_char("&RYou can't claim credits from a vendor you don't own.\n\r", ch);
		return;
	}
}

// TODO Possible 'transvendor <clan/[player]>'
// TODO Closevendor  For governments...
