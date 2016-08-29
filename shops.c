/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 						      	Shops Module 					  		  	*
 * 																			*
 *  Contains functions used by mobshops. Buy, sell, list etc.				*
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

void save_vendor(ROOM_INDEX_DATA *room);

/*
 * Local functions
 */

#define	CD	CHAR_DATA
CD *	find_keeper	args( ( CHAR_DATA *ch ) );
CD *	find_fixer	args( ( CHAR_DATA *ch ) );
int	get_cost	args( ( CHAR_DATA *ch, CHAR_DATA *keeper,
		OBJ_DATA *obj, BOOL fBuy ) );
int 	get_repaircost  args( ( CHAR_DATA *keeper, OBJ_DATA *obj ) );
#undef CD

/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch )
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
		send_to_char( "You can't do that here.\n\r", ch );
		return NULL;
	}


	/*
	 * Shop hours.
	 */
	if ( time_info.hour < pShop->open_hour )
	{
		do_say( keeper, "Sorry, come back later." );
		return NULL;
	}

	if ( time_info.hour > pShop->close_hour )
	{
		do_say( keeper, "Sorry, come back tomorrow." );
		return NULL;
	}

	if ( !knows_language( keeper, ch->speaking, ch ) )
	{
		do_say( keeper, "I can't understand you." );
		return NULL;
	}

	return keeper;
}

/*
 * repair commands.
 */
CHAR_DATA *find_fixer( CHAR_DATA *ch )
{
	CHAR_DATA *keeper;
	REPAIR_DATA *rShop;

	rShop = NULL;
	for ( keeper = ch->in_room->first_person;
			keeper;
			keeper = keeper->next_in_room )
		if ( IS_NPC(keeper) && (rShop = keeper->pIndexData->rShop) != NULL )
			break;

	if ( !rShop )
	{
		send_to_char( "You can't do that here.\n\r", ch );
		return NULL;
	}


	/*
	 * Shop hours.
	 */
	if ( time_info.hour < rShop->open_hour )
	{
		do_say( keeper, "Sorry, come back later." );
		return NULL;
	}

	if ( time_info.hour > rShop->close_hour )
	{
		do_say( keeper, "Sorry, come back tomorrow." );
		return NULL;
	}


	if ( !knows_language( keeper, ch->speaking, ch ) )
	{
		do_say( keeper, "I can't understand you." );
		return NULL;
	}

	return keeper;
}

int get_cost( CHAR_DATA *ch, CHAR_DATA *keeper, OBJ_DATA *obj, BOOL fBuy )
{
	SHOP_DATA *pShop;
	int cost = 0;
	BOOL richcustomer;
	int profitmod;

	if ( !obj || ( pShop = keeper->pIndexData->pShop ) == NULL )
		return 0;

	/* If mob is a player shop or clan shop, return the straight cost */
	if(pShop->clanshop == TRUE || pShop->playershop == TRUE)
		return obj->cost;

	if ( ch->gold > (ch->top_level * ch->top_level * 1000) )
		richcustomer = TRUE;
	else
		richcustomer = FALSE;

	if ( fBuy )
	{
		cost = (int) (obj->cost * (80 + UMIN(ch->top_level, LEVEL_AVATAR))) / 100;

		profitmod = 13 - get_curr_cha(ch) + (richcustomer ? 15 : 0)
				  + ((URANGE(5,ch->top_level,LEVEL_AVATAR)-20)/2);
		cost = (int) (obj->cost
				* UMAX( (pShop->profit_sell+1), pShop->profit_buy+profitmod ) )
				/ 100;
	}
	else
	{
		OBJ_DATA *obj2;
		int itype;

		profitmod = get_curr_cha(ch) - 13 - (richcustomer ? 15 : 0);
		cost = 0;
		for ( itype = 0; itype < MAX_TRADE; itype++ )
		{
			if ( obj->item_type == pShop->buy_type[itype] )
			{
				cost = (int) (obj->cost
						* UMIN( (pShop->profit_buy-1),
								pShop->profit_sell+profitmod) ) / 100;
				break;
			}
		}
		for ( obj2 = keeper->first_carrying; obj2; obj2 = obj2->next_content )
		{
			if ( obj->pIndexData == obj2->pIndexData )
			{
				cost /= (obj2->count+1);
				break;
			}
		}

		cost = UMIN( cost , 2500 );

	}


	if ( obj->item_type == ITEM_DEVICE )
		cost = (int) (cost * obj->value[2] / obj->value[1]);

	return cost;
}

int get_repaircost( CHAR_DATA *keeper, OBJ_DATA *obj )
{
	REPAIR_DATA *rShop;
	int cost;
	int itype;
	BOOL found;

	if ( !obj || ( rShop = keeper->pIndexData->rShop ) == NULL )
		return 0;

	cost = 0;
	found = FALSE;
	for ( itype = 0; itype < MAX_FIX; itype++ )
	{
		if ( obj->item_type == rShop->fix_type[itype] )
		{
			cost = (int) (obj->cost * rShop->profit_fix / 1000);
			found = TRUE;
			break;
		}
	}

	if ( !found )
		cost = -1;

	if ( cost == 0 )
		cost = 1;

	if ( found && cost > 0 )
	{
		switch (obj->item_type)
		{
		case ITEM_ARMOR:
			if (obj->value[0] >= obj->value[1])
				cost = -2;
			else
				cost *= (obj->value[1] - obj->value[0]);
			break;
		case ITEM_WEAPON:
			if (INIT_WEAPON_CONDITION == obj->value[0])
				cost = -2;
			else
				cost *= (INIT_WEAPON_CONDITION - obj->value[0]);
			break;
		case ITEM_DEVICE:
			if (obj->value[2] >= obj->value[1])
				cost = -2;
			else
				cost *= (obj->value[1] - obj->value[2]);
		}
	}

	return cost;
}


void do_buy( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	int maxgold;
	BOOL debit;
	OBJ_DATA *obj;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Buy what?\n\r", ch );
		return;
	}

	if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
	{
		char buf[MAX_STRING_LENGTH];
		CHAR_DATA *pet;
		ROOM_INDEX_DATA *pRoomIndexNext;
		ROOM_INDEX_DATA *in_room;

		if ( argument[0] == '\0' )
			debit = FALSE;
		else if ( !str_cmp( "atm", argument ) || !str_cmp( "debit", argument ) )
		{
			BOOL has_card = FALSE;

			for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
			{
				if ( obj->item_type == ITEM_DEBIT_CARD )
					has_card = TRUE;
			}

			if ( has_card == TRUE )
				debit = TRUE;
			else
			{
				send_to_char( "You don't even have your card with you!\n\r", ch );
				return;
			}
		}

		if ( IS_NPC(ch) )
			return;

		pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
		if ( !pRoomIndexNext )
		{
			bug( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
			send_to_char( "Sorry, you can't buy that here.\n\r", ch );
			return;
		}

		in_room     = ch->in_room;
		ch->in_room = pRoomIndexNext;
		pet         = get_char_room( ch, arg );
		ch->in_room = in_room;

		if ( pet == NULL || !IS_NPC( pet ) || !xIS_SET(pet->act, ACT_PET) )
		{
			send_to_char( "Sorry, you can't buy that here.\n\r", ch );
			return;
		}

		if (( ch->gold < 10 * pet->top_level * pet->top_level ) && debit == FALSE)
		{
			send_to_char( "You can't afford it.\n\r", ch );
			return;
		}
		else if ( (ch->pcdata->bank < 10 * pet->top_level * pet->top_level) && debit == TRUE )
		{
			send_to_char( "You dont have enough money in your bank account for it.\n\r", ch );
			return;
		}

		maxgold = 10 * pet->top_level * pet->top_level;
		if ( debit == FALSE )
			ch->gold	-= maxgold; /* this was already here, btw */
		else
			ch->pcdata->bank  -= maxgold;

		boost_economy( ch->in_room->area, maxgold );
		pet		= create_mobile( pet->pIndexData );
		xSET_BIT(pet->act, ACT_PET);
		SET_BIT(pet->affected_by, AFF_CHARM);

		argument = one_argument( argument, arg );
		if ( arg[0] != '\0' )
		{
			sprintf( buf, "%s %s", pet->name, arg );
			STRFREE( pet->name );
			pet->name = STRALLOC( buf );
		}

		sprintf( buf, "%sA neck tag says 'I belong to %s'.\n\r",
				pet->description, ch->name );
		STRFREE( pet->description );
		pet->description = STRALLOC( buf );

		char_to_room( pet, ch->in_room );
		add_follower( pet, ch );
		send_to_char( "Enjoy your pet.\n\r", ch );
		act( AT_ACTION, "$n bought $N as a pet.", ch, NULL, pet, TO_ROOM );
		return;
	}
	else
	{
		CHAR_DATA *keeper;
		int cost;
		int noi = 1;		/* Number of items */
		sh_int mnoi = 20;	/* Max number of items to be bought at once */

		if ( ( keeper = find_keeper( ch ) ) == NULL )
			return;

		maxgold = keeper->top_level * 10;

		if ( is_number( arg ) )
		{
			noi = atoi( arg );
			argument = one_argument( argument, arg );
			if ( noi > mnoi )
			{
				act( AT_TELL, "$n tells you 'I don't sell that many items at"
						" once.'", keeper, NULL, ch, TO_VICT );
				ch->reply = keeper;
				return;
			}
		}

		if ( argument[0] == '\0' )
			debit = FALSE;
		else if ( !str_cmp( "atm", argument ) || !str_cmp( "debit", argument ) )
		{
			BOOL has_card = FALSE;

			for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
			{
				if ( obj->item_type == ITEM_DEBIT_CARD )
					has_card = TRUE;
			}

			if ( has_card == TRUE )
				debit = TRUE;
			else
			{
				send_to_char( "You don't even have your card with you!\n\r", ch );
				return;
			}
		}

		obj  = get_obj_carry( keeper, arg );

		if ( !obj && arg[0] == '#' )
		{
			int onum, oref;
			BOOL ofound = FALSE;

			onum =0;
			oref = atoi(arg+1);
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
		if (keeper->home != NULL && obj->cost > 0)
			cost= obj->cost;
		cost = ( get_cost( ch, keeper, obj, TRUE ) * noi );

		/* Can't bargain with player shops. */
		if(!keeper->pIndexData->pShop->clanshop == TRUE
							&& !keeper->pIndexData->pShop->playershop == TRUE)
		{
			if( !IS_NPC(ch) && ch->pcdata->learned[gsn_bargain] > 0 && ch->pcdata->learned[gsn_bargain] > number_percent())
			{
				ch_printf(ch,"You are able to bargain from %d credits to %d credits!\n\r", cost, (cost/3)+(cost/2));
				cost = (cost/3) + (cost/2);
				if(number_percent() > 50)
					learn_from_success(ch, gsn_bargain);
			}
		}
		if ( cost <= 0 || !can_see_obj( ch, obj ) )
		{
			act( AT_TELL, "$n tells you 'I don't sell that -- try 'list'.'",
					keeper, NULL, ch, TO_VICT );
			ch->reply = keeper;
			return;
		}

		if ( !IS_OBJ_STAT( obj, ITEM_INVENTORY ) && ( noi > 1 ) )
		{
			interpret( keeper, "laugh" );
			act( AT_TELL, "$n tells you 'I don't have enough of those in stock"
					" to sell more than one at a time.'", keeper, NULL, ch, TO_VICT );
			ch->reply = keeper;
			return;
		}

		if ( ch->gold < cost && debit == FALSE)
		{
			act( AT_TELL, "$n tells you 'You can't afford to buy $p.'",
					keeper, obj, ch, TO_VICT );
			ch->reply = keeper;
			return;
		}

		if ( ch->pcdata->bank < cost && debit == TRUE)
		{
			send_to_char( "You are almost slide your card through, but you remember you don't have enough money!\n\r", ch );
			return;
		}

		if ( IS_SET(obj->extra_flags, ITEM_PROTOTYPE)
				&& get_trust( ch ) < LEVEL_IMMORTAL )
		{
			act( AT_TELL, "$n tells you 'This is a only a prototype!  I can't sell you that...'",
					keeper, NULL, ch, TO_VICT );
			ch->reply = keeper;
			return;
		}

		/* Check if item is unique and if player is holding one already - Boran */
		if(IS_SET(obj->extra_flags, ITEM_UNIQUE))
		{
			OBJ_DATA *pObj;
			for ( pObj = ch->first_carrying; pObj; pObj = pObj->next_content )
			{
				if ( can_see_obj(ch, pObj) && pObj->pIndexData->vnum == obj->pIndexData->vnum )
				{
					send_to_char("They can only carry one of those.\n\r", ch);
					return;
				}
			}
		}
		if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
		{
			send_to_char( "You can't carry that many items.\n\r", ch );
			return;
		}

		if ( ch->carry_weight + ( get_obj_weight( obj ) * noi )
				+ (noi > 1 ? 2 : 0) > can_carry_w( ch ) )
		{
			send_to_char( "You can't carry that much weight.\n\r", ch );
			return;
		}

		if ( noi == 1 )
		{
			if ( !IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
				separate_obj( obj );
			act( AT_ACTION, "$n buys $p.", ch, obj, NULL, TO_ROOM );
			act( AT_ACTION, "You buy $p.", ch, obj, NULL, TO_CHAR );
		}
		else
		{
			sprintf( arg, "$n buys %d $p%s.", noi,
					( obj->short_descr[strlen(obj->short_descr)-1] == 's'
							? "" : "s" ) );
			act( AT_ACTION, arg, ch, obj, NULL, TO_ROOM );
			sprintf( arg, "You buy %d $p%s.", noi,
					( obj->short_descr[strlen(obj->short_descr)-1] == 's'
							? "" : "s" ) );
			act( AT_ACTION, arg, ch, obj, NULL, TO_CHAR );
			act( AT_ACTION, "$N puts them into a bag and hands it to you.",
					ch, NULL, keeper, TO_CHAR );
		}

		if ( debit == FALSE )
			ch->gold     -= cost; /* this line was already here, btw */
		else if ( debit == TRUE )
			ch->pcdata->bank     -= cost;
		keeper->gold += cost;

		if ( keeper->gold > maxgold
				&& keeper->pIndexData->pShop->clanshop == FALSE
				&& keeper->pIndexData->pShop->playershop == FALSE)
		{
			boost_economy( keeper->in_room->area, keeper->gold - maxgold/2 );
			keeper->gold = maxgold/2;
			act( AT_ACTION, "$n puts some credits into a large safe.", keeper, NULL, NULL, TO_ROOM );
		}

		if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
		{
			OBJ_DATA *buy_obj, *bag;

			buy_obj = create_object( obj->pIndexData, obj->level );

			/*
			 * Due to grouped objects and carry limitations in SMAUG
			 * The shopkeeper gives you a bag with multiple-buy,
			 * and also, only one object needs be created with a count
			 * set to the number bought.		-Thoric
			 */
			if ( noi > 1 )
			{
				bag = create_object( get_obj_index( OBJ_VNUM_SHOPPING_BAG ), 1 );
				/* perfect size bag ;) */
				bag->value[0] = bag->weight + (buy_obj->weight * noi);
				buy_obj->count = noi;
				obj->pIndexData->count += (noi - 1);
				numobjsloaded += (noi - 1);
				obj_to_obj( buy_obj, bag );
				obj_to_char( bag, ch );
			}
			else
				obj_to_char( buy_obj, ch );
		}
		else
		{
			obj_from_char( obj );
			obj_to_char( obj, ch );
			if(keeper->pIndexData->pShop->clanshop == TRUE
					|| keeper->pIndexData->pShop->playershop == TRUE)
				save_vendor(keeper->in_room);
		}

		return;
	}
}

// TODO
void do_list( CHAR_DATA *ch, char *argument )
{
	if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
	{
		ROOM_INDEX_DATA *pRoomIndexNext;
		CHAR_DATA *pet;
		BOOL found;

		pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
		if ( !pRoomIndexNext )
		{
			bug( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
			send_to_char( "You can't do that here.\n\r", ch );
			return;
		}

		found = FALSE;
		for ( pet = pRoomIndexNext->first_person; pet; pet = pet->next_in_room )
		{
			if ( xIS_SET(pet->act, ACT_PET) && IS_NPC(pet) )
			{
				if ( !found )
				{
					found = TRUE;
					send_to_char( "Pets for sale:\n\r", ch );
				}
				ch_printf( ch, "[%2d] %8d - %s\n\r",
						pet->top_level,
						10 * pet->top_level * pet->top_level,
						pet->short_descr );
			}
		}
		if ( !found )
			send_to_char( "Sorry, we're out of pets right now.\n\r", ch );
		return;
	}
	else
	{
		char arg[MAX_INPUT_LENGTH];
		CHAR_DATA *keeper;
		OBJ_DATA *obj;
		int cost;
		int oref = 0;
		BOOL found;

		one_argument( argument, arg );

		if ( ( keeper = find_keeper( ch ) ) == NULL )
			return;

		found = FALSE;
		
		int item_count = -1;	// Used to recolor item price and ID after every 5 items. (For aesthetics)
		int item_color = 0;
		for ( obj = keeper->last_carrying; obj; obj = obj->prev_content )
		{
			if ( obj->wear_loc == WEAR_NONE
					&&   can_see_obj( ch, obj ) )
			{
				oref++;
				if ( ( cost = get_cost( ch, keeper, obj, TRUE ) ) > 0
						&& ( arg[0] == '\0' || nifty_is_name( arg, obj->name ) ) )
				{
					if (keeper->home != NULL)
						cost = obj->cost;
					if ( !found )
					{
						found = TRUE;
						ch_printf( ch, "&z[&WPrice &z] [&WType          &z] {&WID &z} &WItem\n\r" );
					}

					char typeStr[MAX_STRING_LENGTH];

					// Find type of object.
					switch(obj->item_type)
					{
					case ITEM_LIGHT: strcpy(typeStr, "light");
					break;
					case ITEM_WEAPON:
					{
						switch(obj->value[3])
						{
						case WEAPON_BLASTER: strcpy(typeStr, "blaster");
						break;
						case WEAPON_VIBRO_BLADE: strcpy(typeStr, "blade");
						break;
						case WEAPON_FORCE_PIKE: strcpy(typeStr, "forcepike");
						break;
						case WEAPON_LIGHTSABER: strcpy(typeStr, "lightsaber");
						break;
						case WEAPON_DUAL_LIGHTSABER: strcpy(typeStr, "d_lightsaber");
						break;
						case WEAPON_BOWCASTER: strcpy(typeStr, "bowcaster");
						break;
						case WEAPON_VIBRO_AXE: strcpy(typeStr, "axe");
						break;
						case WEAPON_WHIP: strcpy(typeStr, "whip");
						break;
						case WEAPON_CLAW: strcpy(typeStr, "claw");
						break;
						default: strcpy(typeStr, "weapon");
						}
					}
					break;
					case ITEM_MISSILE: strcpy(typeStr, "missile");
					break;
					case ITEM_TREASURE: strcpy(typeStr, "treasure");
					break;
					case ITEM_ARMOR: strcpy(typeStr, "armor");
					break;
					case ITEM_POTION: strcpy(typeStr, "potion");
					break;
					case ITEM_FURNITURE: strcpy(typeStr, "furniture");
					break;
					case ITEM_TRASH: strcpy(typeStr, "trash");
					break;
					case ITEM_CONTAINER:
					{
						if(IS_SET(obj->wear_flags, WEAR_HOLSTER_L) || IS_SET(obj->wear_flags, WEAR_HOLSTER_R))
							strcpy(typeStr, "holster");
						else
							strcpy(typeStr, "container");
					}
					break;
					case ITEM_NOTE: strcpy(typeStr, "message");
					break;
					case ITEM_DRINK_CON: strcpy(typeStr, "drinkcon");
					break;
					case ITEM_KEY: strcpy(typeStr, "key");
					break;
					case ITEM_FOOD: strcpy(typeStr, "food");
					break;
					case ITEM_PEN: strcpy(typeStr, "pen");
					break;
					case ITEM_CORPSE_NPC: strcpy(typeStr, "corpse");
					break;
					case ITEM_PILL: strcpy(typeStr, "pill");
					break;
					case ITEM_SCRAPS: strcpy(typeStr, "scraps");
					break;
					case ITEM_PIPE: strcpy(typeStr, "pipe");
					break;
					case ITEM_HERB_CON: strcpy(typeStr, "herbcon");
					break;
					case ITEM_HERB: strcpy(typeStr, "herb");
					break;
					case ITEM_INCENSE: strcpy(typeStr, "incense");
					break;
					case ITEM_BOOK: strcpy(typeStr, "book");
					break;
					case ITEM_BUTTON: strcpy(typeStr, "button");
					break;
					case ITEM_MAP: strcpy(typeStr, "map");
					break;
					case ITEM_LOCKPICK: strcpy(typeStr, "lockpick");
					break;
					case ITEM_OIL: strcpy(typeStr, "oil");
					break;
					case ITEM_FUEL: strcpy(typeStr, "fuel");
					break;
					case ITEM_AMMO: strcpy(typeStr, "ammo");
					break;
					case ITEM_SHOVEL: strcpy(typeStr, "shovel");
					break;
					case ITEM_RAWSPICE: strcpy(typeStr, "raw spice");
					break;
					case ITEM_LENS: strcpy(typeStr, "lens");
					break;
					case ITEM_CRYSTAL: strcpy(typeStr, "crystal");
					break;
					case ITEM_DURAPLAST: strcpy(typeStr, "duraplast");
					break;
					case ITEM_BATTERY: strcpy(typeStr, "battery");
					break;
					case ITEM_TOOLKIT: strcpy(typeStr, "toolkit");
					break;
					case ITEM_DURASTEEL: strcpy(typeStr, "durasteel");
					break;
					case ITEM_OVEN: strcpy(typeStr, "oven");
					break;
					case ITEM_MIRROR: strcpy(typeStr, "mirror");
					break;
					case ITEM_CIRCUIT: strcpy(typeStr, "circuit");
					break;
					case ITEM_SUPERCONDUCTOR: strcpy(typeStr, "superconductor");
					break;
					case ITEM_COMLINK: strcpy(typeStr, "comlink");
					break;
					case ITEM_MEDPAC: strcpy(typeStr, "medpac");
					break;
					case ITEM_FABRIC: strcpy(typeStr, "fabric");
					break;
					case ITEM_RARE_METAL: strcpy(typeStr, "rare metal");
					break;
					case ITEM_MAGNET: strcpy(typeStr, "magnet");
					break;
					case ITEM_THREAD: strcpy(typeStr, "thread");
					break;
					case ITEM_SPICE: strcpy(typeStr, "spice");
					break;
					case ITEM_DEVICE: strcpy(typeStr, "device");
					break;
					case ITEM_GRENADE: strcpy(typeStr, "grenade");
					break;
					case ITEM_LANDMINE: strcpy(typeStr, "landmine");
					break;
					case ITEM_BOLT: strcpy(typeStr, "bolt");
					break;
					case ITEM_CHEMICAL: strcpy(typeStr, "chemical");
					break;
					case ITEM_COMMSYSTEM: strcpy(typeStr, "commsystem");
					break;
					case ITEM_BINDERS: strcpy(typeStr, "binders");
					break;
					case ITEM_GOGGLES: strcpy(typeStr, "goggles");
					break;
					default: strcpy(typeStr, "");
					}

					if(item_count == 4)
					{
						if(item_color == 0)
							item_color = 1;
						else
							item_color = 0;
						item_count = 0;
					}
					else
						item_count++;
						
					if(item_color == 0)	// Paint it in dark cyan
						ch_printf( ch, "&z[&c%6d&z] [&G%-14s&z] {&c%3d&z}&w %s%s%s.\n\r",
								cost, typeStr, oref, capitalize( obj->short_descr ), color_str(AT_LIST, ch),
								IS_SET(obj->extra_flags, ITEM_HUTT_SIZE) ? " (hutt size)" :
										( IS_SET(obj->extra_flags, ITEM_LARGE_SIZE) ? " (large)" :
												( IS_SET(obj->extra_flags, ITEM_HUMAN_SIZE) ? " (medium)" :
														( IS_SET(obj->extra_flags, ITEM_SMALL_SIZE) ? " (small)" :
																"" ) ) ) );
					else	// Paint it in light cyan
						ch_printf( ch, "&z[&C%6d&z] [&G%-14s&z] {&C%3d&z}&w %s%s%s.\n\r",
								cost, typeStr, oref, capitalize( obj->short_descr ), color_str(AT_LIST, ch),
								IS_SET(obj->extra_flags, ITEM_HUTT_SIZE) ? " (hutt size)" :
										( IS_SET(obj->extra_flags, ITEM_LARGE_SIZE) ? " (large)" :
												( IS_SET(obj->extra_flags, ITEM_HUMAN_SIZE) ? " (medium)" :
														( IS_SET(obj->extra_flags, ITEM_SMALL_SIZE) ? " (small)" :
																"" ) ) ) );
				}
			}
		}

		if ( !found )
		{
			if ( arg[0] == '\0' )
				send_to_char( "You can't buy anything here.\n\r", ch );
			else
				send_to_char( "You can't buy that here.\n\r", ch );
		}
		return;
	}
}

// TODO
void do_sell( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Sell what?\n\r", ch );
		return;
	}

	if ( ( keeper = find_keeper( ch ) ) == NULL )
		return;

	if( keeper->pIndexData && keeper->pIndexData->pShop && keeper->pIndexData->pShop->playershop && keeper->pIndexData->pShop->owner
		&& str_cmp(keeper->pIndexData->pShop->owner, ch->name ) )
	{
		act( AT_TELL, "$n glances at the item, 'Nah.'", keeper, NULL, ch, TO_VICT );
		return;
	}
	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
		act( AT_TELL, "$n tells you 'You don't have that item.'", keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}
	/* Bug report and solution thanks to animal@netwin.co.nz */
	if ( !can_see_obj( keeper, obj) )
	{
		send_to_char("What are you trying to sell me? I don't buy thin air!\n\r", ch );
		return;
	}


	if ( !can_drop_obj( ch, obj ) )
	{
		send_to_char( "You can't let go of it!\n\r", ch );
		return;
	}

	if ( obj->timer > 0 )
	{
		act( AT_TELL, "$n tells you, '$p is depreciating in value too quickly...'", keeper, obj, ch, TO_VICT );
		return;
	}

	if ( ( cost = get_cost( ch, keeper, obj, FALSE )) <= 0 )
	{
		act( AT_ACTION, "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
		return;
	}

	if ( cost > keeper->gold )
	{
		act( AT_TELL, "$n makes a credit transaction.", keeper, obj, ch, TO_VICT );
		lower_economy( ch->in_room->area, cost-keeper->gold );
	}

	separate_obj( obj );
	act( AT_ACTION, "$n sells $p.", ch, obj, NULL, TO_ROOM );
	sprintf( buf, "You sell $p for %d credit%s.",
			cost, cost == 1 ? "" : "s" );
	act( AT_ACTION, buf, ch, obj, NULL, TO_CHAR );
	ch->gold     += cost;
	keeper->gold -= cost;
	if ( keeper->gold < 0 )
		keeper->gold = 0;

	if ( obj->item_type == ITEM_TRASH )
		extract_obj( obj );
	else  if ( IS_SET( obj->extra_flags , ITEM_CONTRABAND) )
	{
		long ch_exp;

		ch_exp = UMIN( obj->cost*10 , ( exp_level( ch->skill_level[SMUGGLING_ABILITY]+1) - exp_level( ch->skill_level[SMUGGLING_ABILITY])  ) / 10  );
		ch_printf( ch, "You receive %ld smuggling experience for unloading your contraband.\n\r " , ch_exp );
		gain_exp( ch, ch_exp , SMUGGLING_ABILITY );
		if ( obj->item_type == ITEM_SPICE || obj->item_type == ITEM_RAWSPICE )
			extract_obj( obj );
		else
		{
			REMOVE_BIT( obj->extra_flags , ITEM_CONTRABAND );
			obj_from_char( obj );
			obj_to_char( obj, keeper );
		}
	}
	else if ( obj->item_type == ITEM_SPICE || obj->item_type == ITEM_RAWSPICE )
		extract_obj( obj );
	else
	{
		obj_from_char( obj );
		obj_to_char( obj, keeper );
	}

	return;
}



void do_value( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost;

	if ( argument[0] == '\0' )
	{
		send_to_char( "Value what?\n\r", ch );
		return;
	}

	if ( ( keeper = find_keeper( ch ) ) == NULL )
		return;

	if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
	{
		act( AT_TELL, "$n tells you 'You don't have that item.'",
				keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
		send_to_char( "You can't let go of it!\n\r", ch );
		return;
	}

	if ( ( cost = get_cost( ch, keeper, obj, FALSE ) ) <= 0 )
	{
		act( AT_ACTION, "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
		return;
	}

	sprintf( buf, "$n tells you 'I'll give you %d credits for $p.'", cost );
	act( AT_TELL, buf, keeper, obj, ch, TO_VICT );
	ch->reply = keeper;

	return;
}

/*
 * Repair a single object. Used when handling "repair all" - Gorog
 */
void repair_one_obj( CHAR_DATA *ch, CHAR_DATA *keeper, OBJ_DATA *obj,
		char *arg, int maxgold, char *fixstr, char*fixstr2 )
{
	char buf[MAX_STRING_LENGTH];
	int cost;

	if ( !can_drop_obj( ch, obj ) )
		ch_printf( ch, "You can't let go of %s.\n\r", obj->name );
	else if ( ( cost = get_repaircost( keeper, obj ) ) < 0 )
	{
		if (cost != -2)
			act( AT_TELL, "$n tells you, 'Sorry, I can't do anything with $p.'",
					keeper, obj, ch, TO_VICT );
		else
			act( AT_TELL, "$n tells you, '$p looks fine to me!'", keeper, obj, ch, TO_VICT );
	}
	/* "repair all" gets a 10% surcharge - Gorog */

	else if ( (cost = strcmp("all",arg) ? cost : 11*cost/10) > ch->gold )
	{
		sprintf( buf,
				"$N tells you, 'It will cost %d credit%s to %s %s...'", cost,
				cost == 1 ? "" : "s", fixstr, obj->name );
		act( AT_TELL, buf, ch, NULL, keeper, TO_CHAR );
		act( AT_TELL, "$N tells you, 'Which I see you can't afford.'", ch,
				NULL, keeper, TO_CHAR );
	}
	else
	{
		sprintf( buf, "$n gives $p to $N, who quickly %s it.", fixstr2 );
		act( AT_ACTION, buf, ch, obj, keeper, TO_ROOM );
		sprintf( buf, "$N charges you %d credit%s to %s $p.",
				cost, cost == 1 ? "" : "s", fixstr );
		act( AT_ACTION, buf, ch, obj, keeper, TO_CHAR );
		ch->gold     -= cost;
		keeper->gold += cost;
		if ( keeper->gold < 0 )
			keeper->gold = 0;
		else
			if ( keeper->gold > maxgold )
			{
				boost_economy( keeper->in_room->area, keeper->gold - maxgold/2 );
				keeper->gold = maxgold/2;
				act( AT_ACTION, "$n puts some credits into a large safe.", keeper,
						NULL, NULL, TO_ROOM );
			}

		switch ( obj->item_type )
		{
		default:
			send_to_char( "For some reason, you think you got ripped off...\n\r", ch);
			break;
		case ITEM_ARMOR:
			obj->value[0] = obj->value[1];
			break;
		case ITEM_WEAPON:
			obj->value[0] = INIT_WEAPON_CONDITION;
			break;
		case ITEM_DEVICE:
			obj->value[2] = obj->value[1];
			break;
		}

		oprog_repair_trigger( ch, obj );
	}
}

void do_mobrepair( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	char *fixstr;
	char *fixstr2;
	int maxgold;

	if ( argument[0] == '\0' )
	{
		send_to_char( "Repair what?\n\r", ch );
		return;
	}

	if ( ( keeper = find_fixer( ch ) ) == NULL )
		return;

	maxgold = keeper->top_level * 10;
	switch( keeper->pIndexData->rShop->shop_type )
	{
	default:
	case SHOP_FIX:
		fixstr  = "repair";
		fixstr2 = "repairs";
		break;
	case SHOP_RECHARGE:
		fixstr  = "recharge";
		fixstr2 = "recharges";
		break;
	}

	if ( !strcmp( argument, "all" ) )
	{
		for ( obj = ch->first_carrying; obj ; obj = obj->next_content )
		{
			if ( obj->wear_loc  == WEAR_NONE
					&&   can_see_obj( ch, obj )
			&& ( obj->item_type == ITEM_ARMOR
					||   obj->item_type == ITEM_WEAPON
					||   obj->item_type == ITEM_DEVICE ) )
				repair_one_obj( ch, keeper, obj, argument, maxgold,
						fixstr, fixstr2);
		}
		return;
	}

	if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
	{
		act( AT_TELL, "$n tells you 'You don't have that item.'",
				keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}

	repair_one_obj( ch, keeper, obj, argument, maxgold, fixstr, fixstr2);
}

void appraise_all( CHAR_DATA *ch, CHAR_DATA *keeper, char *fixstr )
{
	OBJ_DATA *obj;
	char buf[MAX_STRING_LENGTH], *pbuf=buf;
	int cost, total=0;

	for ( obj = ch->first_carrying; obj != NULL ; obj = obj->next_content )
	{
		if ( obj->wear_loc  == WEAR_NONE
				&&   can_see_obj( ch, obj )
		&& ( obj->item_type == ITEM_ARMOR
				||   obj->item_type == ITEM_WEAPON
				||   obj->item_type == ITEM_DEVICE ) ){

			if ( !can_drop_obj( ch, obj ) )
				ch_printf( ch, "You can't let go of %s.\n\r", obj->name );
			else if ( ( cost = get_repaircost( keeper, obj ) ) < 0 )
			{
				if (cost != -2)
					act( AT_TELL,
							"$n tells you, 'Sorry, I can't do anything with $p.'",
							keeper, obj, ch, TO_VICT );
				else
					act( AT_TELL, "$n tells you, '$p looks fine to me!'",
							keeper, obj, ch, TO_VICT );
			}
			else
			{
				sprintf( buf,
						"$N tells you, 'It will cost %d credit%s to %s %s'",
						cost, cost == 1 ? "" : "s", fixstr, obj->name );
				act( AT_TELL, buf, ch, NULL, keeper, TO_CHAR );
				total += cost;
			}
		}
	}
	if ( total > 0 )
	{
		send_to_char ("\n\r", ch);
		sprintf( buf,
				"$N tells you, 'It will cost %d credit%s in total.'",
				total, cost == 1 ? "" : "s" );
		act( AT_TELL, buf, ch, NULL, keeper, TO_CHAR );
		strcpy( pbuf,
				"$N tells you, 'Remember there is a 10% surcharge for repair all.'");
		act( AT_TELL, buf, ch, NULL, keeper, TO_CHAR );
	}
}


void do_appraise( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost;
	char *fixstr;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Appraise what?\n\r", ch );
		return;
	}

	if ( ( keeper = find_fixer( ch ) ) == NULL )
		return;

	switch( keeper->pIndexData->rShop->shop_type )
	{
	default:
	case SHOP_FIX:
		fixstr  = "repair";
		break;
	case SHOP_RECHARGE:
		fixstr  = "recharge";
		break;
	}

	if ( !strcmp( arg, "all") )
	{
		appraise_all( ch, keeper, fixstr );
		return;
	}

	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
		act( AT_TELL, "$n tells you 'You don't have that item.'",
				keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
		send_to_char( "You can't let go of it.\n\r", ch );
		return;
	}

	if ( ( cost = get_repaircost( keeper, obj ) ) < 0 )
	{
		if (cost != -2)
			act( AT_TELL, "$n tells you, 'Sorry, I can't do anything with $p.'", keeper, obj, ch, TO_VICT );
		else
			act( AT_TELL, "$n tells you, '$p looks fine to me!'", keeper, obj, ch, TO_VICT );
		return;
	}

	sprintf( buf,
			"$N tells you, 'It will cost %d credit%s to %s that...'", cost,
			cost == 1 ? "" : "s", fixstr );
	act( AT_TELL, buf, ch, NULL, keeper, TO_CHAR );
	if ( cost > ch->gold )
		act( AT_TELL, "$N tells you, 'Which I see you can't afford.'", ch,
				NULL, keeper, TO_CHAR );

	return;
}


/* ------------------ Shop Building and Editing Section ----------------- */


void do_makeshop( CHAR_DATA *ch, char *argument )
{
	SHOP_DATA *shop;
	int vnum;
	MOB_INDEX_DATA *mob;

	if ( !argument || argument[0] == '\0' )
	{
		send_to_char( "Usage: makeshop <mobvnum>\n\r", ch );
		return;
	}

	vnum = atoi( argument );

	if ( (mob = get_mob_index(vnum)) == NULL )
	{
		send_to_char( "Mobile not found.\n\r", ch );
		return;
	}

	if ( !can_medit(ch, mob) )
		return;

	if ( mob->pShop )
	{
		send_to_char( "This mobile already has a shop.\n\r", ch );
		return;
	}

	CREATE( shop, SHOP_DATA, 1 );

	LINK( shop, first_shop, last_shop, next, prev );
	shop->keeper	= vnum;
	shop->profit_buy	= 120;
	shop->profit_sell	= 90;
	shop->open_hour	= 0;
	shop->close_hour	= 23;
	shop->clanshop = 0;
	shop->playershop = 0;
	STRFREE(shop->owner);
	shop->owner = STRALLOC("");
	mob->pShop		= shop;
	send_to_char( "Done.\n\r", ch );
	return;
}


void do_shopset( CHAR_DATA *ch, char *argument )
{
	SHOP_DATA *shop;
	MOB_INDEX_DATA *mob, *mob2;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int vnum;
	int value;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Usage: shopset <mob vnum> <field> value\n\r", ch );
		send_to_char( "\n\rField being one of:\n\r", ch );
		send_to_char( "  buy0 buy1 buy2 buy3 buy4 buy sell open close keeper\n\r", ch );
		return;
	}

	vnum = atoi( arg1 );

	if ( (mob = get_mob_index(vnum)) == NULL )
	{
		send_to_char( "Mobile not found.\n\r", ch );
		return;
	}

	if ( !can_medit(ch, mob) )
		return;

	if ( !mob->pShop )
	{
		send_to_char( "This mobile doesn't keep a shop.\n\r", ch );
		return;
	}
	shop = mob->pShop;
	value = atoi( argument );

	if ( !str_cmp( arg2, "buy0" ) )
	{
		if ( !is_number(argument) )
			value = get_otype(argument);
		if ( value < 0 || value > MAX_ITEM_TYPE )
		{
			send_to_char( "Invalid item type!\n\r", ch );
			return;
		}
		shop->buy_type[0] = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "buy1" ) )
	{
		if ( !is_number(argument) )
			value = get_otype(argument);
		if ( value < 0 || value > MAX_ITEM_TYPE )
		{
			send_to_char( "Invalid item type!\n\r", ch );
			return;
		}
		shop->buy_type[1] = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "buy2" ) )
	{
		if ( !is_number(argument) )
			value = get_otype(argument);
		if ( value < 0 || value > MAX_ITEM_TYPE )
		{
			send_to_char( "Invalid item type!\n\r", ch );
			return;
		}
		shop->buy_type[2] = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "buy3" ) )
	{
		if ( !is_number(argument) )
			value = get_otype(argument);
		if ( value < 0 || value > MAX_ITEM_TYPE )
		{
			send_to_char( "Invalid item type!\n\r", ch );
			return;
		}
		shop->buy_type[3] = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "buy4" ) )
	{
		if ( !is_number(argument) )
			value = get_otype(argument);
		if ( value < 0 || value > MAX_ITEM_TYPE )
		{
			send_to_char( "Invalid item type!\n\r", ch );
			return;
		}
		shop->buy_type[4] = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "buy" ) )
	{
		if ( value <= (shop->profit_sell+5) || value > 1000 )
		{
			send_to_char( "Out of range.\n\r", ch );
			return;
		}
		shop->profit_buy = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "sell" ) )
	{
		if ( value < 0 || value >= (shop->profit_buy-5) )
		{
			send_to_char( "Out of range.\n\r", ch );
			return;
		}
		shop->profit_sell = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "open" ) )
	{
		if ( value < 0 || value > 23 )
		{
			send_to_char( "Out of range.\n\r", ch );
			return;
		}
		shop->open_hour = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "close" ) )
	{
		if ( value < 0 || value > 23 )
		{
			send_to_char( "Out of range.\n\r", ch );
			return;
		}
		shop->close_hour = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "keeper" ) )
	{
		if ( (mob2 = get_mob_index(vnum)) == NULL )
		{
			send_to_char( "Mobile not found.\n\r", ch );
			return;
		}
		if ( !can_medit(ch, mob) )
			return;
		if ( mob2->pShop )
		{
			send_to_char( "That mobile already has a shop.\n\r", ch );
			return;
		}
		mob->pShop  = NULL;
		mob2->pShop = shop;
		shop->keeper = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	do_shopset( ch, "" );
	return;
}


void do_shopstat( CHAR_DATA *ch, char *argument )
{
	SHOP_DATA *shop;
	MOB_INDEX_DATA *mob;
	int vnum;

	if ( argument[0] == '\0' )
	{
		send_to_char( "Usage: shopstat <keeper vnum>\n\r", ch );
		return;
	}

	vnum = atoi( argument );

	if ( (mob = get_mob_index(vnum)) == NULL )
	{
		send_to_char( "Mobile not found.\n\r", ch );
		return;
	}

	if ( !mob->pShop )
	{
		send_to_char( "This mobile doesn't keep a shop.\n\r", ch );
		return;
	}
	shop = mob->pShop;

	ch_printf( ch, "Keeper: %d  %s\n\r", shop->keeper, mob->short_descr );
	ch_printf( ch, "buy0 [%s]  buy1 [%s]  buy2 [%s]  buy3 [%s]  buy4 [%s]\n\r",
			o_types[shop->buy_type[0]],
			o_types[shop->buy_type[1]],
			o_types[shop->buy_type[2]],
			o_types[shop->buy_type[3]],
			o_types[shop->buy_type[4]] );
	ch_printf( ch, "Profit:  buy %3d%%  sell %3d%%\n\r",
			shop->profit_buy,
			shop->profit_sell );
	ch_printf( ch, "Hours:   open %2d  close %2d\n\r",
			shop->open_hour,
			shop->close_hour );

	// TODO - ADD THESE 3 FIELDS AND MAKE SURE THEY INITIALIZE
	ch_printf(ch, "Clan shop  : [%s]\n\r", shop->clanshop ? "X" : " ");
	ch_printf(ch, "Player shop: [%s]\n\r", shop->playershop ? "X" : " ");
	ch_printf(ch, "Owner: %s", (shop->owner && shop->owner[0] != '\0')
			? shop->owner : "-none-");
	return;
}


void do_shops( CHAR_DATA *ch, char *argument )
{
	SHOP_DATA *shop;

	if ( !first_shop )
	{
		send_to_char( "There are no shops.\n\r", ch );
		return;
	}

	set_char_color( AT_NOTE, ch );
	for ( shop = first_shop; shop; shop = shop->next )
		ch_printf( ch, "Keeper: %5d Buy: %3d Sell: %3d Open: %2d Close: %2d Buy: %2d %2d %2d %2d %2d\n\r",
				shop->keeper,	   shop->profit_buy, shop->profit_sell,
				shop->open_hour,   shop->close_hour,
				shop->buy_type[0], shop->buy_type[1],
				shop->buy_type[2], shop->buy_type[3], shop->buy_type[4] );
	return;
}


/* -------------- Repair Shop Building and Editing Section -------------- */


void do_makerepair( CHAR_DATA *ch, char *argument )
{
	REPAIR_DATA *repair;
	int vnum;
	MOB_INDEX_DATA *mob;

	if ( !argument || argument[0] == '\0' )
	{
		send_to_char( "Usage: makerepair <mobvnum>\n\r", ch );
		return;
	}

	vnum = atoi( argument );

	if ( (mob = get_mob_index(vnum)) == NULL )
	{
		send_to_char( "Mobile not found.\n\r", ch );
		return;
	}

	if ( !can_medit(ch, mob) )
		return;

	if ( mob->rShop )
	{
		send_to_char( "This mobile already has a repair shop.\n\r", ch );
		return;
	}

	CREATE( repair, REPAIR_DATA, 1 );

	LINK( repair, first_repair, last_repair, next, prev );
	repair->keeper	= vnum;
	repair->profit_fix	= 100;
	repair->shop_type	= SHOP_FIX;
	repair->open_hour	= 0;
	repair->close_hour	= 23;
	mob->rShop		= repair;
	send_to_char( "Done.\n\r", ch );
	return;
}


void do_repairset( CHAR_DATA *ch, char *argument )
{
	REPAIR_DATA *repair;
	MOB_INDEX_DATA *mob, *mob2;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int vnum;
	int value;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Usage: repairset <mob vnum> <field> value\n\r", ch );
		send_to_char( "\n\rField being one of:\n\r", ch );
		send_to_char( "  fix0 fix1 fix2 profit type open close keeper\n\r", ch );
		return;
	}

	vnum = atoi( arg1 );

	if ( (mob = get_mob_index(vnum)) == NULL )
	{
		send_to_char( "Mobile not found.\n\r", ch );
		return;
	}

	if ( !can_medit(ch, mob) )
		return;

	if ( !mob->rShop )
	{
		send_to_char( "This mobile doesn't keep a repair shop.\n\r", ch );
		return;
	}
	repair = mob->rShop;
	value = atoi( argument );

	if ( !str_cmp( arg2, "fix0" ) )
	{
		if ( !is_number(argument) )
			value = get_otype(argument);
		if ( value < 0 || value > MAX_ITEM_TYPE )
		{
			send_to_char( "Invalid item type!\n\r", ch );
			return;
		}
		repair->fix_type[0] = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "fix1" ) )
	{
		if ( !is_number(argument) )
			value = get_otype(argument);
		if ( value < 0 || value > MAX_ITEM_TYPE )
		{
			send_to_char( "Invalid item type!\n\r", ch );
			return;
		}
		repair->fix_type[1] = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "fix2" ) )
	{
		if ( !is_number(argument) )
			value = get_otype(argument);
		if ( value < 0 || value > MAX_ITEM_TYPE )
		{
			send_to_char( "Invalid item type!\n\r", ch );
			return;
		}
		repair->fix_type[2] = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "profit" ) )
	{
		if ( value < 1 || value > 1000 )
		{
			send_to_char( "Out of range.\n\r", ch );
			return;
		}
		repair->profit_fix = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "type" ) )
	{
		if ( value < 1 || value > 2 )
		{
			send_to_char( "Out of range.\n\r", ch );
			return;
		}
		repair->shop_type = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "open" ) )
	{
		if ( value < 0 || value > 23 )
		{
			send_to_char( "Out of range.\n\r", ch );
			return;
		}
		repair->open_hour = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "close" ) )
	{
		if ( value < 0 || value > 23 )
		{
			send_to_char( "Out of range.\n\r", ch );
			return;
		}
		repair->close_hour = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "keeper" ) )
	{
		if ( (mob2 = get_mob_index(vnum)) == NULL )
		{
			send_to_char( "Mobile not found.\n\r", ch );
			return;
		}
		if ( !can_medit(ch, mob) )
			return;
		if ( mob2->rShop )
		{
			send_to_char( "That mobile already has a repair shop.\n\r", ch );
			return;
		}
		mob->rShop  = NULL;
		mob2->rShop = repair;
		repair->keeper = value;
		send_to_char( "Done.\n\r", ch );
		return;
	}

	do_repairset( ch, "" );
	return;
}


void do_repairstat( CHAR_DATA *ch, char *argument )
{
	REPAIR_DATA *repair;
	MOB_INDEX_DATA *mob;
	int vnum;

	if ( argument[0] == '\0' )
	{
		send_to_char( "Usage: repairstat <keeper vnum>\n\r", ch );
		return;
	}

	vnum = atoi( argument );

	if ( (mob = get_mob_index(vnum)) == NULL )
	{
		send_to_char( "Mobile not found.\n\r", ch );
		return;
	}

	if ( !mob->rShop )
	{
		send_to_char( "This mobile doesn't keep a repair shop.\n\r", ch );
		return;
	}
	repair = mob->rShop;

	ch_printf( ch, "Keeper: %d  %s\n\r", repair->keeper, mob->short_descr );
	ch_printf( ch, "fix0 [%s]  fix1 [%s]  fix2 [%s]\n\r",
			o_types[repair->fix_type[0]],
			o_types[repair->fix_type[1]],
			o_types[repair->fix_type[2]] );
	ch_printf( ch, "Profit: %3d%%  Type: %d\n\r",
			repair->profit_fix,
			repair->shop_type );
	ch_printf( ch, "Hours:   open %2d  close %2d\n\r",
			repair->open_hour,
			repair->close_hour );
	return;
}


void do_repairshops( CHAR_DATA *ch, char *argument )
{
	REPAIR_DATA *repair;

	if ( !first_repair )
	{
		send_to_char( "There are no repair shops.\n\r", ch );
		return;
	}

	set_char_color( AT_NOTE, ch );
	for ( repair = first_repair; repair; repair = repair->next )
		ch_printf( ch, "Keeper: %5d Profit: %3d Type: %d Open: %2d Close: %2d Fix: %2d %2d %2d\n\r",
				repair->keeper,	     repair->profit_fix, repair->shop_type,
				repair->open_hour,   repair->close_hour,
				repair->fix_type[0], repair->fix_type[1], repair->fix_type[2] );
	return;
}
