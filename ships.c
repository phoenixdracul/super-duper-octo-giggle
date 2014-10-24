/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 						     	Ship System 					    		*
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "mud.h"

BMARKET_DATA *first_market_ship;
BMARKET_DATA *last_market_ship;

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}


typedef enum
{
  SP_NOT_SET,SP_COCKPIT,SP_ENTRANCE,SP_HANGER1, SP_HANGER2, SP_HANGER3, SP_HANGER4,SP_ENGINEROOM,
  SP_PILOT,SP_COPILOT,SP_NAVIGATOR,SP_GUNNER,SP_TURRET1,SP_TURRET2,SP_TURRET3,SP_TURRET4,SP_TURRET5,
  SP_TURRET6,SP_TURRET7,SP_TURRET8,SP_TURRET9,SP_TURRET10,SP_WINDOW1,SP_WINDOW2,SP_WINDOW3,SP_WINDOW4,
  SP_WINDOW5,SP_WINDOW6,SP_OTHER
} ship_proto_room_types;

struct ship_prototypes_struct
{
    char       *name;
    char       *sname;
    char       *clan;
    int         num_rooms;
    int         cost;
    int         class;
    int		tractor;
    int		primaryType;
    int     secondaryType;
    int     tertiaryType;
	int		primaryCount;
	int		secondaryCount;
	int		tertiaryCount;
    int		range_weapons[3];
    int		hull;
    int		shields;
    int		energy;
    int		countermeasures;
    int		maxbombs;
    int		speed;
    int		hyperspeed;
    int		manuever;
    int		turrets;
    int		windows;
    int         maxcargo;
	int	    gwell;	// Added by Michael
	int         hangar1space;
	int	    hangar2space;	// Added by Michael
	int	    hangar3space;	// Added by Michael
	int	    hangar4space;	// Added by Michael
    int		mods;
    //int         plasma;
};

struct ship_prototypes_struct ship_prototypes[256];
int NUM_PROTOTYPES;
   

typedef struct prototype_room PROTO_ROOM;

struct prototype_room
{
    int                what_prototype;
    PROTO_ROOM         *next;
    PROTO_ROOM         *prev;
    char               *name;
    char               *desc;
    char               *flags;
    int                tunnel;
    int                exits[10];
    int                exflags[10];
    int                keys[10];
    int                room_num;
    int                room_type;
    char               *rprog[10];
    char               *reset[10];
};

PROTO_ROOM *first_prototype_room;
PROTO_ROOM *last_prototype_room;

void instaroom( AREA_DATA *pArea, ROOM_INDEX_DATA *pRoom, bool dodoors );
void shiplist(CHAR_DATA *ch);
char *primary_beam_name_proto(int shiptype);
char *secondary_beam_name_proto(int shiptype);
char *tertiary_beam_name_proto(int shiptype);

void do_buymobship(CHAR_DATA *ch, char *argument ) //Improved by Michael to allow naming ships and buying anything smaller than a capital
{
   int x, size, ship_type, vnum;
   SHIP_DATA *ship;
   SHIP_DATA *sship;
   char arg[MAX_STRING_LENGTH];
   char shipname[MAX_STRING_LENGTH];
   char buf[MAX_STRING_LENGTH];
   bool found_proto = FALSE;
   AREA_DATA *tarea;
   CLAN_DATA *clan;
   CLAN_DATA *mainclan;
   SPACE_DATA *system;
   PLANET_DATA *planet;
   bool fsys, fplan;

   argument = one_argument( argument, arg );

   if( (arg[0] != '\0') && (argument[0] == '\0') )
   {
      send_to_char( "Usage: buymobship <ship type> <ship name>  <starsystem to be sent to>\r\n", ch );
      return;
   }
   if( IS_NPC( ch ) || !ch->pcdata )
   {
      send_to_char( "&ROnly players can do that!\r\n", ch );
      return;
   }
   if( !ch->pcdata->clan )
   {
      send_to_char( "You do not belong to any organization.\r\n", ch );
      return;
   }
   clan = ch->pcdata->clan;
   mainclan = ch->pcdata->clan->mainclan ? ch->pcdata->clan->mainclan : clan;

   if( ( ch->pcdata->bestowments && is_name( "clanbuyship", ch->pcdata->bestowments ) )
       || !str_cmp( ch->name, clan->leader ) )
      ;
   else
   {
      send_to_char( "Your organization hasn't seen fit to bestow you with that ability.\r\n", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "\r\n&z+&W-----------------------------------------------------------------------------&z+\r\n", ch );
      send_to_char( "&W|&w                               Available ships                               &W|\r\n", ch );
      send_to_char( "&z+&W-----------------------------------------------------------------------------&z+\r\n", ch );

      for( x = 0; x < NUM_PROTOTYPES; x++ )
      {
         if( !ch->pcdata->clan && str_cmp( ship_prototypes[x].clan, "" ) )
            continue;
         if( ch->pcdata->clan && str_cmp( ship_prototypes[x].clan, ch->pcdata->clan->name ) )
            continue;
         if( ship_prototypes[x].class != 0 && ship_prototypes[x].class != 1 && ship_prototypes[x].class != 2 )
            continue;

         ch_printf( ch, "&W|&w %s%-39.39s&W Type:  &w%-13.13s &WCost:&w %8d &W|\r\n",
                    !str_cmp( ship_prototypes[x].clan, "The Republic" ) ? "&G" : "&w",
                    ship_prototypes[x].name,
		 ship_prototypes[x].class == 1 ? "Starfighter" :
		 ship_prototypes[x].class == 2 ? "Bomber" : 
		 ship_prototypes[x].class == 3 ? "Shuttle" : 
		 ship_prototypes[x].class == 4 ? "Freighter" : 
		 ship_prototypes[x].class == 5 ? "Frigate" :
		 ship_prototypes[x].class == 7 ? "Corvette" : "Spacecraft", ship_prototypes[x].cost );
      }

      send_to_char( "&z+&W-----------------------------------------------------------------------------&z+\r\n", ch );
      send_to_char( "&W|&w               Refer to 'shiplist' for a complete ship listing               &W|\r\n", ch );
      send_to_char( "&z+&W-----------------------------------------------------------------------------&z+\r\n", ch );
      return;
   }

   size = NUM_PROTOTYPES;
   for( x = 0; x < size; x++ )
   {
      if( nifty_is_name_prefix( arg, ship_prototypes[x].name ) )
      {
         found_proto = TRUE;
         break;
      }
   }
   if( !found_proto )
   {
      send_to_char( "\r\n&z+&W-----------------------------------------------------------------------------&z+\r\n", ch );
      send_to_char( "&W|&w                               Available ships                               &W|\r\n", ch );
      send_to_char( "&z+&W-----------------------------------------------------------------------------&z+\r\n", ch );

      for( x = 0; x < NUM_PROTOTYPES; x++ )
      {
         if( !ch->pcdata->clan && str_cmp( ship_prototypes[x].clan, "" ) )
            continue;
         if( ch->pcdata->clan && str_cmp( ship_prototypes[x].clan, ch->pcdata->clan->name ) )
            continue;
         if( ship_prototypes[x].class != 0 && ship_prototypes[x].class != 1 && ship_prototypes[x].class != 2 )
            continue;

         ch_printf( ch, "&W|&w %s%-39.39s&W Type:  &w%-13.13s &WCost:&w %8d &W|\r\n",
                    !str_cmp( ship_prototypes[x].clan, "The Republic" ) ? "&G" : "&w",
                    ship_prototypes[x].name,
                 ship_prototypes[x].class == 1 ? "Starfighter" :
                 ship_prototypes[x].class == 2 ? "Bomber" :
                 ship_prototypes[x].class == 3 ? "Shuttle" :
                 ship_prototypes[x].class == 4 ? "Freighter" :
                 ship_prototypes[x].class == 5 ? "Frigate" :
                 ship_prototypes[x].class == 7 ? "Corvette" : "Spacecraft", ship_prototypes[x].cost );
      }
      send_to_char( "&z+&W-----------------------------------------------------------------------------&z+\r\n", ch );
      send_to_char( "&W|&w               Refer to 'shiplist' for a complete ship listing               &W|\r\n", ch );
      send_to_char( "&z+&W-----------------------------------------------------------------------------&z+\r\n", ch );
      return;
   }
   ship_type = x;

   // Modify later. -||

   if( ( clan->clan_type == 0 ) && str_cmp( ship_prototypes[ship_type].clan, ch->pcdata->clan->name ) )
   {
      send_to_char( "Your organization may only purchase its own ship models.\r\n", ch );
      return;
   }
   if( ship_prototypes[x].class != 1 && ship_prototypes[x].class != 2 && ship_prototypes[x].class != 3 && ship_prototypes[x].class != 4 && ship_prototypes[x].class != 5  && ship_prototypes[x].class != 7 )
   {
      send_to_char( "You may only purchase fighters, bombers, or midtargets for mobile ships.\r\n", ch );
      return;
   }
   if(argument[0] == '\0' || argument == NULL || !argument)
   {
      send_to_char("What do you want to name the ship?\r\n",ch);
      return;
   }

   vnum = find_vnum_block( ship_prototypes[ship_type].num_rooms, FALSE );
   if( vnum == -1 )
   {
      send_to_char( "There was a problem with your ship: free vnums. Notify an administrator.\r\n", ch );
      bug( "Ship area is low on vnums." );
      return;
   }

        char arg2[MAX_STRING_LENGTH];
        argument = one_argument(argument, arg2);
        arg2[0] = UPPER(arg2[0]);
        sprintf(arg2,"%s (%s)",arg2, ship_prototypes[ship_type].sname);

        for ( ship = first_ship; ship; ship = ship->next )
        {
                if(!str_cmp(ship->name,arg2))
                {
                        send_to_char("&CThat ship name is already in use. Choose another.\r\n",ch);
                        return;
                }
        }

        if(!str_cmp(arg2, "&"))
        {
                send_to_char("No color codes in ship names.\n\r", ch);
                return;
        }

/*   switch ( ship_type )
   {
      default:
         sprintf( shipname, "Mobile Ship MS" );
         break;
         // NR
      case 0:
         sprintf( shipname, "X-Wing Snubfighter MXW" );
         break;
      case 1:
         sprintf( shipname, "A-Wing Scout MAW" );
         break;
      case 2:
         sprintf( shipname, "B-Wing Heavy Fighter MBW" );
         break;
      case 3:
         sprintf( shipname, "Y-Wing Bomber MYB" );
         break;
      case 4:
         sprintf( shipname, "K-Wing Heavy Bomber MKW" );
         break;
         // Imp
      case 6:
         sprintf( shipname, "TIE Fighter MTF" );
         break;
      case 7:
         sprintf( shipname, "TIE Bomber MTB" );
         break;
      case 8:
         sprintf( shipname, "TIE Defender MTD" );
         break;
      case 9:
         sprintf( shipname, "XM-1 Missileboat MXM" );
         break;
      case 10:
         sprintf( shipname, "XG-1 Assault Gunboat MXG" );
         break;
   }

   sprintf( shipname, "%s%d (%s)", shipname, number_range( 1111, 9999 ), ship_prototypes[ship_type].sname );*/

   if( ch->pcdata->clan->funds < ship_prototypes[ship_type].cost )
   {
      send_to_char( "Your organization cannot pay for that ship.\r\n", ch );
      return;
   }
   clan = ch->pcdata->clan;
   mainclan = ch->pcdata->clan->mainclan ? ch->pcdata->clan->mainclan : clan;

   if( ( ch->pcdata->bestowments && is_name( "clanbuyship", ch->pcdata->bestowments ) )
       || !str_cmp( ch->name, clan->leader ) )
      ;
   else
   {
      send_to_char( "Your organization hasn't seen fit to bestow you with that ability.\r\n", ch );
      return;
   }

   fsys = fplan = fcap = FALSE;
   for(system = first_starsystem; system; system = system->next)
	{
	   if(nifty_is_name(argument, system->name))
		{
		   fsys = TRUE;
		   break;
		}
	}
   if(!fsys)
	{
	   send_to_char("No such starsystem.\n\r", ch);
	   return;
	}
   for(planet = system->first_planet; planet; planet = planet->next_in_system)
	{
	   if(ch->pcdata->clan == planet->governed_by)
		   fplan = TRUE;
	}
   if(!fplan)
	{
	   send_to_char("Your organization does not control any planets in that starsystem.\n\r", ch);
	   return;
	}
   count = 0;
   for(sship = system->first_ship; sship; sship = sship->next_in_starsystem)
	{
	   if(sship->type == MOB_SHIP) count++;
	   if(sship->class >= SHIP_LFRIGATE) { fcap = TRUE; caps++; }
	}
   if(!fcap)
	{
	   send_to_char("There isn't a capital class ship in that starsystem.\n\r", ch);
	   return;
	}
	   if(count > 3*caps)
		{
		   send_to_char("You can only have 3 mobile ships per capital-class ship in a system.\n\r", ch);
		   return;
		}

    for ( tarea = first_area; tarea; tarea = tarea->next )
      if ( !str_cmp( SHIP_AREA, tarea->filename ) )
        break;
    if(make_prototype_rooms(ship_type,vnum,tarea,shipname) == -1)
    {
    	send_to_char("There was a problem with your ship: unable to create a room. Notify an administrator.\r\n",ch);
    	bug("Ship area unable to make_room.",0);
      return;
    }

   ch->pcdata->clan->funds -= ( int )( ship_prototypes[ship_type].cost * 1.3 );

   ch_printf( ch, "It costs %d to build the ship and %d to train a pilot.\r\n",
              ship_prototypes[ship_type].cost, ship_prototypes[ship_type].cost / 3 );
   ch_printf( ch, "%s is quickly dispatched to the %s system.\r\n", shipname, system->name );


   ship = make_prototype_ship( ship_type, vnum, ch, arg2 );
   ship->owner = STRALLOC( mainclan->name );
   extract_ship( ship );
   ship_to_starsystem( ship, system );
   ship->location = 0;
   ship->inship = NULL;
   ship->type = MOB_SHIP;
   if( ship->home )
      STRFREE( ship->home );
   ship->home = STRALLOC( system->name );
   ship->hx = ship->hy = ship->hz = 1;
   ship->vx = number_range( -3000, 3000 );
   ship->vy = number_range( -3000, 3000 );
   ship->vz = number_range( -3000, 3000 );
   ship->autopilot = TRUE;
   save_ship( ship );
   write_ship_list(  );
   sprintf( buf, "%s enters the starsystem at %.0f %.0f %.0f", ship->name, ship->vx, ship->vy, ship->vz );
   echo_to_system( AT_YELLOW, ship, buf, NULL );
   return;
}

void do_orderclanship(CHAR_DATA *ch, char *argument )
{
  int x,size,ship_type,vnum;
  SHIP_DATA *ship;
  char arg[MAX_STRING_LENGTH];
  bool found_proto = FALSE;
  AREA_DATA *tarea;
  CLAN_DATA   *clan;
  CLAN_DATA   *mainclan;
       
   if ( IS_NPC(ch) || !ch->pcdata )
   {
   	send_to_char( "&ROnly players can do that!\r\n" ,ch );
   	return;
   }
   if ( !ch->pcdata->clan )
   {
   	send_to_char( "You do not belong to any organization.\r\n" ,ch );
   	return;
   }
   
   clan = ch->pcdata->clan;
   mainclan = ch->pcdata->clan->mainclan ? ch->pcdata->clan->mainclan : clan;
   
   if ( ( ch->pcdata->bestowments
    &&    is_name("clanbuyship", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, clan->leader  ))
	;
   else
   {
   	send_to_char( "Your organization hasn't seen fit to bestow you with that ability.\r\n" ,ch );
   	return;
   }
    if( !IS_SET(ch->in_room->room_flags2,ROOM_SHIPYARD) )
    {
    	send_to_char("You can only purchase a ship at a shipyard.\r\n",ch);
      return;
    }
    if(argument == NULL || !argument || argument[0] == '\0')
    {
        send_to_char("\n\r&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w                              :Available ships:                               &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w           :Ships Name:                       :Type:            :Cost:        &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);

        for(x=0;x<NUM_PROTOTYPES;x++)
		{
                  		  if(!ch->pcdata->clan && (!str_cmp(ship_prototypes[x].clan, "Neutral") || !str_cmp(ship_prototypes[x].clan, "The Empire")
								|| !str_cmp(ship_prototypes[x].clan, "The New Republic"))) continue;
		  if( ch->pcdata->clan && str_cmp( ship_prototypes[x].clan, ch->pcdata->clan->name )&& str_cmp( ship_prototypes[x].clan, "" ) )
		  continue;

                 ch_printf(ch,"&W|&w %s%-40.40s&w%-18.18s &w%8d      &W|\r\n",
                 !str_cmp(ship_prototypes[x].clan, "Neutral") ? "&C" :
		 !str_cmp( ship_prototypes[x].clan, "The Empire" ) ? "&R" :
		 !str_cmp( ship_prototypes[x].clan, "" ) ? "&B" :
                 !str_cmp(ship_prototypes[x].clan, "The New Republic") ? "&G" : "&w",
                 ship_prototypes[x].name,
		 ship_prototypes[x].class == SHIP_FIGHTER ? "Starfighter" :
		 ship_prototypes[x].class == SHIP_BOMBER  ? "Bomber" : 
		 ship_prototypes[x].class == SHIP_SHUTTLE ? "Shuttle" : 
		 ship_prototypes[x].class == SHIP_FREIGHTER ? "Freighter" : 
		 ship_prototypes[x].class == SHIP_FRIGATE ? "Frigate" : 
		 ship_prototypes[x].class == SHIP_TT ? "Troop Transport" : 
		 ship_prototypes[x].class == SHIP_CORVETTE ? "Corvette" : 
		 ship_prototypes[x].class == SHIP_LFRIGATE ? "Large Frigate" :
		 ship_prototypes[x].class == SHIP_CRUISER ? "Cruiser" : 
	         ship_prototypes[x].class == SHIP_DREADNAUGHT ? "Dreadnaught" : 
		 ship_prototypes[x].class == SHIP_DESTROYER ? "Star Destroyer" : 
		 ship_prototypes[x].class == SHIP_SPACE_STATION ? "Space Station" : 
		 ship_prototypes[x].class == LAND_VEHICLE ? "Land vehicle" : "Unknown",
            
            ship_prototypes[x].cost);
        }
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w               Refer to 'shiplist' for a complete ship listing                &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\n\r",ch); 

      return;
    }
    argument = one_argument(argument,arg);
    size = NUM_PROTOTYPES;
    for(x=0;x<size;x++) 
    {
    	if(nifty_is_name_prefix(arg,ship_prototypes[x].name)) {
            found_proto = TRUE;
    	  break;
    	}
    }
    if(!found_proto)
    {
        send_to_char("\n\r&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w                              :Available ships:                               &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w           :Ships Name:                       :Type:            :Cost:        &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);

        for(x=0;x<NUM_PROTOTYPES;x++)
		{
                  		  if(!ch->pcdata->clan && (!str_cmp(ship_prototypes[x].clan, "Neutral") || !str_cmp(ship_prototypes[x].clan, "The Empire")
								|| !str_cmp(ship_prototypes[x].clan, "The New Republic"))) continue;
		  if( ch->pcdata->clan && str_cmp( ship_prototypes[x].clan, ch->pcdata->clan->name )&& str_cmp( ship_prototypes[x].clan, "" ) )
		  continue;

                 ch_printf(ch,"&W|&w %s%-40.40s&w%-18.18s &w%8d      &W|\r\n",
                 !str_cmp(ship_prototypes[x].clan, "Neutral") ? "&C" :
		 !str_cmp( ship_prototypes[x].clan, "The Empire" ) ? "&R" :
		 !str_cmp( ship_prototypes[x].clan, "" ) ? "&B" :
                 !str_cmp(ship_prototypes[x].clan, "The New Republic") ? "&G" : "&w",
        
                 ship_prototypes[x].name,
		 ship_prototypes[x].class == SHIP_FIGHTER ? "Starfighter" :
		 ship_prototypes[x].class == SHIP_BOMBER  ? "Bomber" : 
		 ship_prototypes[x].class == SHIP_SHUTTLE ? "Shuttle" : 
		 ship_prototypes[x].class == SHIP_FREIGHTER ? "Freighter" : 
		 ship_prototypes[x].class == SHIP_FRIGATE ? "Frigate" : 
		 ship_prototypes[x].class == SHIP_TT ? "Troop Transport" : 
		 ship_prototypes[x].class == SHIP_CORVETTE ? "Corvette" : 
		 ship_prototypes[x].class == SHIP_LFRIGATE ? "Large Frigate" :
		 ship_prototypes[x].class == SHIP_CRUISER ? "Cruiser" : 
                 ship_prototypes[x].class == SHIP_DREADNAUGHT ? "Dreadnaught" : 
		 ship_prototypes[x].class == SHIP_DESTROYER ? "Star Destroyer" : 
		 ship_prototypes[x].class == SHIP_SPACE_STATION ? "Space Station" : 
		 ship_prototypes[x].class == LAND_VEHICLE ? "Land vehicle" : "Unknown",
            
            ship_prototypes[x].cost);
        }
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w               Refer to 'shiplist' for a complete ship listing                &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\n\r",ch);

      return;
    }
    ship_type = x;
    if(argument[0] == '\0' || argument == NULL || !argument)
    {
        send_to_char("What would you like to name your ship?\r\n",ch);
      return;
    }
    // Modify later. -||

    if( (clan->clan_type == 0) && 
	 str_cmp(ship_prototypes[ship_type].clan, ch->pcdata->clan->name))
    {
    	send_to_char("Your organization may only purchase its own ship models.\n\r", ch);
    	return;
    }
    vnum = find_vnum_block(ship_prototypes[ship_type].num_rooms);
    if(vnum == -1)
    {
    	send_to_char("There was a problem with your ship: free vnums. Notify an administrator.\r\n",ch);
    	bug("Ship area is low on vnums.",0);
      return;
    }
    argument[0] = UPPER(argument[0]);
    sprintf(argument,"%s (%s)",argument,ship_prototypes[ship_type].sname);
    for ( ship = first_ship; ship; ship = ship->next )
    {   
        if(!str_cmp(ship->name,argument))
        {
        send_to_char("That ship name is already in use. Choose another.\r\n",ch);
          return;
        }
    }
    if(ch->pcdata->clan->funds < ship_prototypes[ship_type].cost)
    {
        send_to_char("Your organization cannot pay for that ship.\r\n",ch);
      return;
    }
    for ( tarea = first_area; tarea; tarea = tarea->next )
      if ( !str_cmp( SHIP_AREA, tarea->filename ) )
        break;
    if(make_prototype_rooms(ship_type,vnum,tarea,argument) == -1)
    {
    	send_to_char("There was a problem with your ship: unable to create a room. Notify an administrator.\r\n",ch);
    	bug("Ship area unable to make_room.",0);
      return;
    }

    ch->pcdata->clan->funds -= ship_prototypes[ship_type].cost;

    send_to_char("&wA shipyard salesman enters some information into a datapad.\r\n",ch);
    send_to_char("&R&CThe salesman says:&R&W You're all set. Thanks.\n\r", ch);
    ship = make_prototype_ship(ship_type,vnum,ch,argument);
    ship->owner = ch->pcdata->clan->name;
    save_ship( ship );
    write_ship_list( );
  return;
}

void do_ordership(CHAR_DATA *ch, char *argument)
{
  int x,size,ship_type,vnum,count;
  SHIP_DATA *ship;
  char arg[MAX_STRING_LENGTH];
  bool found_proto = FALSE;
  AREA_DATA *tarea;
  BMARKET_DATA *marketship;
  char *bmshipname;
  int bmshipcost;

  count=0;
    if( !IS_SET(ch->in_room->room_flags2,ROOM_SHIPYARD) && !IS_SET(ch->in_room->room_flags2, ROOM_BLACKMARKET))
    {
    	send_to_char("You can only purchase ships at a shipyard.\r\n",ch);
      return;
    }
   if(!IS_SET(ch->in_room->room_flags2, ROOM_BLACKMARKET))
   {
    if(argument == NULL || !argument || argument[0] == '\0')
    {
        send_to_char("\n\r&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w                              :Available ships:                               &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w           :Ships Name:                       :Type:            :Cost:        &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        
        for( x = 0; x < NUM_PROTOTYPES; x++ )
		{
		  if(!ch->pcdata->clan && (!str_cmp(ship_prototypes[x].clan, "Neutral") || !str_cmp(ship_prototypes[x].clan, "The Empire")
								|| !str_cmp(ship_prototypes[x].clan, "The New Republic"))) continue;
		  if( ch->pcdata->clan && str_cmp( ship_prototypes[x].clan, ch->pcdata->clan->name )&& str_cmp( ship_prototypes[x].clan, "" ) )
		  continue;

                 ch_printf(ch,"&W|&w %s%-40.40s&w%-18.18s &w%8d      &W|\r\n",
                      !str_cmp(ship_prototypes[x].clan, "Neutral") ? "&C" :
		      !str_cmp( ship_prototypes[x].clan, "The Empire" ) ? "&R" :
		      !str_cmp( ship_prototypes[x].clan, "" ) ? "&B" :
                      !str_cmp(ship_prototypes[x].clan, "The New Republic") ? "&G" : "&w",
                        ship_prototypes[x].name,
		        ship_prototypes[x].class == SHIP_FIGHTER ? "Starfighter" :
		        ship_prototypes[x].class == SHIP_BOMBER  ? "Bomber" : 
		        ship_prototypes[x].class == SHIP_SHUTTLE ? "Shuttle" : 
		        ship_prototypes[x].class == SHIP_FREIGHTER ? "Freighter" : 
		        ship_prototypes[x].class == SHIP_FRIGATE ? "Frigate" : 
		        ship_prototypes[x].class == SHIP_TT ? "Troop Transport" : 
		        ship_prototypes[x].class == SHIP_CORVETTE ? "Corvette" : 
			ship_prototypes[x].class == SHIP_LFRIGATE ? "Large Frigate" :
		        ship_prototypes[x].class == SHIP_CRUISER ? "Cruiser" : 
			ship_prototypes[x].class == SHIP_DREADNAUGHT ? "Dreadnaught" : 
		        ship_prototypes[x].class == SHIP_DESTROYER ? "Star Destroyer" : 
		        ship_prototypes[x].class == SHIP_SPACE_STATION ? "Space Station" : 
		        ship_prototypes[x].class == LAND_VEHICLE ? "Land vehicle" : "Unknown",
            
            ship_prototypes[x].cost);
          
        } 
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w               Refer to 'shiplist' for a complete ship listing                &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\n\r",ch);
      return;
    }
   }
   else
   {
    if(argument == NULL || !argument || argument[0] == '\0')
    {
    ch_printf(ch,"&z+&W------------------------&z The Black Market &W--------------------------&z+\n\r");
   	for(marketship = first_market_ship; marketship; marketship = marketship->next)
   	{
         for(x=0;x<NUM_PROTOTYPES;x++)
   	 {
   	  if(!str_cmp(marketship->filename, ship_prototypes[x].sname))
   	  {
   	   bmshipname = ship_prototypes[x].name;
   	   bmshipcost = ship_prototypes[x].cost*3;
	   break;
   	  }
   	 }
   	 
   	 ch_printf(ch,"&W|&z %-35.35s&W |&z Quantity: %d&W |&z Cost: %-8d&W |\n\r", bmshipname,marketship->quantity,bmshipcost);
   	 count++;
   	}
   if(count==0)
    ch_printf(ch,"&W|&z                         No ships available.                        &W|\n\r");
    ch_printf(ch,"&z+&W--------------------------------------------------------------------&z+\n\r");
    return;
    } 
   }
   	    	 
    argument = one_argument(argument, arg);
    size = NUM_PROTOTYPES;
    for(x=0;x<size;x++) 
    {
       if(!IS_SET(ch->in_room->room_flags2, ROOM_BLACKMARKET))
       {
    	if(nifty_is_name_prefix(arg,ship_prototypes[x].name)) 
    	{
            found_proto = TRUE;
    	    break;
        }
       }
       else
       {
       	for(marketship = first_market_ship; marketship; marketship = marketship->next)
       	{
       	 if(nifty_is_name_prefix(arg, ship_prototypes[x].name) && !str_cmp(ship_prototypes[x].sname, marketship->filename))
	 {
       	   found_proto = 1;
	   break;
	 }
        }        send_to_char("\n\r&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w                                Available ships                               &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);

     if(found_proto == 1)
       break;
       }
    }

    if(!found_proto)
    {
     if(!IS_SET(ch->in_room->room_flags2, ROOM_BLACKMARKET))
     {
        send_to_char("\n\r&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w                                Available ships                               &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);

        for(x=0;x<NUM_PROTOTYPES;x++)
		{
		  if(!ch->pcdata->clan && (!str_cmp(ship_prototypes[x].clan, "Neutral") || !str_cmp(ship_prototypes[x].clan, "The Empire")
								|| !str_cmp(ship_prototypes[x].clan, "The New Republic"))) continue;
		  if(ch->pcdata->clan && str_cmp(ship_prototypes[x].clan, ch->pcdata->clan->name) && str_cmp(ship_prototypes[x].clan, "")) continue;

            ch_printf(ch,"&W|&w %s%-34.34s&W Type:  &w%-13.13s &WCost:&w %8d  &W|\r\n",
            !str_cmp(ship_prototypes[x].clan, "Neutral") ? "&C" :
            !str_cmp(ship_prototypes[x].clan, "The Empire") ? "&R" :
            !str_cmp(ship_prototypes[x].clan, "The New Republic") ? "&G" : "&w",
            ship_prototypes[x].name,
		        ship_prototypes[x].class == SHIP_FIGHTER ? "Starfighter" :
		        ship_prototypes[x].class == SHIP_BOMBER  ? "Bomber" : 
		        ship_prototypes[x].class == SHIP_SHUTTLE ? "Shuttle" : 
		        ship_prototypes[x].class == SHIP_FREIGHTER ? "Freighter" : 
		        ship_prototypes[x].class == SHIP_FRIGATE ? "Frigate" : 
		        ship_prototypes[x].class == SHIP_TT ? "Troop Transport" : 
		        ship_prototypes[x].class == SHIP_CORVETTE ? "Corvette" : 
                        ship_prototypes[x].class == SHIP_LFRIGATE ? "Large Frigate" :
		        ship_prototypes[x].class == SHIP_CRUISER ? "Cruiser" : 
			ship_prototypes[x].class == SHIP_DREADNAUGHT ? "Dreadnaught" : 
		        ship_prototypes[x].class == SHIP_DESTROYER ? "Star Destroyer" : 
		        ship_prototypes[x].class == SHIP_SPACE_STATION ? "Space Station" : 
		        ship_prototypes[x].class == LAND_VEHICLE ? "Land vehicle" : "Unknown",
            
            ship_prototypes[x].cost);
          
        }   
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w               Refer to 'shiplist' for a complete ship listing                &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\n\r",ch);
      return;
     }
     else
     {
      ch_printf(ch,"&z+&W------------------------&z The Black Market &W--------------------------&z+\n\r");
        for(marketship = first_market_ship; marketship; marketship = marketship->next)
    	{
          for(x=0;x<NUM_PROTOTYPES;x++)
   	  {
   	    if(!str_cmp(marketship->filename, ship_prototypes[x].sname))
   	    {
   	      bmshipname = ship_prototypes[x].name;
   	      if(marketship->quantity == 1)
   	       bmshipcost = ship_prototypes[x].cost*2.5;
   	      else if(marketship->quantity == 2)
   	       bmshipcost = ship_prototypes[x].cost*2;
   	      else
   	       bmshipcost = ship_prototypes[x].cost*1.5;
   	    }
   	  }
   	   ch_printf(ch,"&W|&z %-35.35s&W |&z Quantity: %d&W |&z Cost: %-8d&W |\n\r", bmshipname,marketship->quantity,bmshipcost);
   	   count++;
   	}
   if(count==0)
    ch_printf(ch,"&W|&z                         No ships available.                        &W|\n\r");
      ch_printf(ch,"&z+&W--------------------------------------------------------------------&z+\n\r");
      return;
      }
   }
   
    ship_type = x;
    if(argument[0] == '\0' || argument == NULL || !argument)
    {
        send_to_char("What do you want to name your ship?\r\n",ch);
      return;
    }
    
    /* Ask Techon what he was thinking here */
//    if( ship_prototypes[ship_type].class == 5 && !IS_IMMORTAL( ch ) )
//          {
//      send_to_char( "At the moment, Capital (Military) ships may only be purchased by clans.\r\n", ch );
//      return;
//   }
//   if( ship_prototypes[ship_type].class == 6 && !IS_IMMORTAL( ch ) )
//          {
//      send_to_char( "At the moment, Capital (Military) ships may only be purchased by clans.\r\n", ch );
//      return;
//   }
//   if( ship_prototypes[ship_type].class == 7 && !IS_IMMORTAL( ch ) )
//          {
//      send_to_char( "At the moment, Capital (Military) ships may only be purchased by clans.\r\n", ch );
//      return;
//   }
   if( ship_prototypes[ship_type].class == 8 && !IS_IMMORTAL( ch ) )
          {
      send_to_char( "At the moment, Capital (Military) ships may only be purchased by clans.\r\n", ch );
      return;
   }
   if( ship_prototypes[ship_type].class == 9 && !IS_IMMORTAL( ch ) )
          {
      send_to_char( "At the moment, Capital (Military) ships may only be purchased by clans.\r\n", ch );
      return;
   }
   if( ship_prototypes[ship_type].class == 10 && !IS_IMMORTAL( ch ) )
          {
      send_to_char( "At the moment, Capital (Military) ships may only be purchased by clans.\r\n", ch );
      return;
   }
   if( ship_prototypes[ship_type].class == 11 && !IS_IMMORTAL( ch ) )
          {
      send_to_char( "At the moment, Capital (Military) ships may only be purchased by clans.\r\n", ch );
      return;
   }
   
  if(!IS_SET(ch->in_room->room_flags2, ROOM_BLACKMARKET))
  {  
   if(!str_cmp(ship_prototypes[ship_type].clan, "Neutral") || !str_cmp(ship_prototypes[ship_type].clan, "The Empire") ||
      !str_cmp(ship_prototypes[ship_type].clan,"The New Republic"))
   {
     if(!ch->pcdata->clan)
     {
      send_to_char("You must find a black market to purchase clanned ships.\n\r", ch);
      return;
     }
     else if(str_cmp(ch->pcdata->clan->name, ship_prototypes[ship_type].clan))
     {
      send_to_char("You must find a black market to purchase other clan's ships.\n\r", ch);
      return;
     }
   }
  }

    vnum = find_vnum_block(ship_prototypes[ship_type].num_rooms);
    if(vnum == -1)
    {
    	send_to_char("There was a problem with your ship: free vnums. Notify an administrator.\r\n",ch);
    	bug("Ship Shop area is low on vnums.",0);
      return;
    }

    argument[0] = UPPER(argument[0]);

    sprintf(argument,"%s (%s)",argument, ship_prototypes[ship_type].sname);
    for ( ship = first_ship; ship; ship = ship->next )
    {   
        if(!str_cmp(ship->name,argument))
        {
        send_to_char("&CThat ship name is already in use. Choose another.\r\n",ch);
          return;
        }
    }
   if(!IS_SET(ch->in_room->room_flags2, ROOM_BLACKMARKET))
   {
    if(ch->gold < ship_prototypes[ship_type].cost)
    {
        send_to_char("You can't afford that ship.\r\n",ch);
      return;
    }
   }
   else
   {
    if(ch->gold < ship_prototypes[ship_type].cost*3)
    {
    	send_to_char("You can't afford that ship.\n\r", ch);
    	return;
    }
   }

	if(!str_cmp(argument, "&"))
    {
    	send_to_char("No color codes in ship names.\n\r", ch);
    	return;
    }
	
	for ( tarea = first_area; tarea; tarea = tarea->next )
      if ( !str_cmp( SHIP_AREA, tarea->filename ) )
        break;
    if(make_prototype_rooms(ship_type,vnum,tarea,argument) == -1)
    {
    	send_to_char("There was a problem with your ship: unable to create a room. Notify an administrator.\n\r",ch);
    	bug("Ship Shop unable to make_room.",0);
      return;
    }
    
   if(!IS_SET(ch->in_room->room_flags2, ROOM_BLACKMARKET))
   {
    ch->gold -= ship_prototypes[ship_type].cost; 
    send_to_char("A shipyard salesman enters some information into a datapad.\r\n",ch);
    send_to_char("&R&CA salesman says:&R&W You're all set. Thanks.\n\r", ch);
   }
   else
   {
    ch->gold -= ship_prototypes[ship_type].cost*3;
    
for(marketship = first_market_ship; marketship; marketship = marketship->next) 
{ 
  if(!str_cmp(ship_prototypes[ship_type].sname, marketship->filename)) { 
    remove_market_ship(marketship); 
    break; 
  } 
} 

    send_to_char("A smuggler gives a grin and shows you to the ship.\n\r", ch);
    send_to_char("&R&CA smuggler says:&R&W There ya go, she's all yours.\n\r", ch);
   } 
    ship = make_prototype_ship(ship_type,vnum,ch,argument);
  return;
}

void do_listmobships(CHAR_DATA *ch, char *argument)
{
	SHIP_DATA *ship;
	SHIP_DATA *ship2;
	char location[MAX_STRING_LENGTH];

	send_to_char("&WName                     Owner               Location\n\r", ch);
	send_to_char("&c-----------------------------------------------------------------\n\r", ch);

	for(ship = first_ship; ship; ship = ship ->next)
	{
		if(ship->type == MOB_SHIP)
		{
			bool room_found = FALSE;
			if(!ship->in_room && ship->shipstate != SHIP_HYPERSPACE)
			{
				room_found = TRUE;
				sprintf(location, "%s System", ship->starsystem->name);
			}
			if(ship->shipstate == SHIP_HYPERSPACE)
			{
				room_found = TRUE;
				sprintf(location, "In Hyperspace");
			}
			for(ship2 = first_ship; ship2; ship2 = ship2->next)
			{
				if(ship->in_room)
				{
					if(ship->in_room->vnum == ship2->hangar1)
					{
						sprintf(location, "%s:%s", ship->in_room->name, ship2->name);
						room_found = TRUE;
						break;
					}
					if(ship->in_room->vnum == ship2->hangar2)
					{
						sprintf(location, "%s:%s", ship->in_room->name, ship2->name);
						room_found = TRUE;
						break;
					}
					if(ship->in_room->vnum == ship2->hangar3)
					{
						sprintf(location, "%s:%s", ship->in_room->name, ship2->name);
						room_found = TRUE;
						break;
					}
					if(ship->in_room->vnum == ship2->hangar4)
					{
						sprintf(location, "%s:%s", ship->in_room->name, ship2->name);
						room_found = TRUE;
						break;
					}
				}
			}
			if(!room_found)
				sprintf(location, "(Nowhere)");

			ch_printf(ch, "%-2s%-24s %-19s %-20s\n\r", !str_cmp(ship->owner, "") ? "&w" : "&C",
					ship->name,
					!str_cmp(ship->owner, "") ? "(None)" : ship->owner,
					location);
		}
	}
}
void do_removemobship(CHAR_DATA *ch, char *argument)
{
	SHIP_DATA *ship;
	char buf[MAX_STRING_LENGTH];

	if( argument[0] == '\0' )
	{
		send_to_char( "Remove which ship?\n\r", ch);
		return;
	}

	ship = get_ship( argument );
	if ( !ship )
	{
		send_to_char( "No such ship.\n\r", ch );
		return;
	}
	if( !ship->type == MOB_SHIP)
	{
		send_to_char("That's not a mobship. Use 'removeship' instead.\n\r", ch);
		return;
	}
	if(IS_SET(ship->flags, SHIP_RESPAWN))
		REMOVE_BIT(ship->flags, SHIP_RESPAWN);
	if(IS_SET(ship->flags, SHIP_NODESTROY))
		REMOVE_BIT(ship->flags, SHIP_NODESTROY);

	sprintf(buf,"%s: removemobship %s",ch->name,argument);
	log_string(buf);
	destroy_ship(ship, ch, "removed");
	send_to_char("Ship removed.\r\n",ch);
	return;
}
void do_makemobship( CHAR_DATA *ch, char *argument ){

	int x,size,ship_type,vnum,count;
	SHIP_DATA *ship;
	char arg[MAX_STRING_LENGTH];
	bool found_proto = FALSE;
	AREA_DATA *tarea;
	SPACE_DATA *system;
	bool fsys = FALSE;

	count=0;

	if(argument == NULL || !argument || argument[0] == '\0')
	{
		send_to_char("\n\r&z+&W-----------------------------------------------------------------------------&z+\r\n",ch);
		send_to_char("&W|&w                               Ship Prototypes                               &W|\r\n",ch);
		send_to_char("&z+&W-----------------------------------------------------------------------------&z+\r\n",ch);

		for(x=0;x<NUM_PROTOTYPES;x++)
		{
			ch_printf(ch,"&W|&w %s%-39.39s&W Type:  &w%-13.13s &WCost:&w %8d &W|\r\n",
					!str_cmp(ship_prototypes[x].clan, "Neutral") ? "&C" :
					        !str_cmp(ship_prototypes[x].clan, "The Empire") ? "&R" :
							!str_cmp(ship_prototypes[x].clan, "The New Republic") ? "&G" : "&w",
									ship_prototypes[x].name,
									ship_prototypes[x].class == SHIP_FIGHTER ? "Starfighter" :
											ship_prototypes[x].class == SHIP_BOMBER  ? "Bomber" :
													ship_prototypes[x].class == SHIP_SHUTTLE ? "Shuttle" :
															ship_prototypes[x].class == SHIP_FREIGHTER ? "Freighter" :
																	ship_prototypes[x].class == SHIP_FRIGATE ? "Frigate" :
																			ship_prototypes[x].class == SHIP_TT ? "Troop Transport" :
																					ship_prototypes[x].class == SHIP_CORVETTE ? "Corvette" :
                                                                                                                                						        ship_prototypes[x].class == SHIP_LFRIGATE ? "Large Frigate" :
																									ship_prototypes[x].class == SHIP_CRUISER ? "Cruiser" :
																											ship_prototypes[x].class == SHIP_DREADNAUGHT ? "Dreadnaught" :
																													ship_prototypes[x].class == SHIP_DESTROYER ? "Star Destroyer" :
																															ship_prototypes[x].class == SHIP_SPACE_STATION ? "Space Station" :
																																	ship_prototypes[x].class == LAND_VEHICLE ? "Land vehicle" : "Unknown",

																																	ship_prototypes[x].cost);

		}
		send_to_char("&z+&W-----------------------------------------------------------------------------&z+\r\n",ch);
		send_to_char("&RSyntax: &Wmakemobship &R<&Wprototype&R> <&Wname&R> <&Whome system&R>&w\n\r", ch);
		return;
	}

	argument = one_argument(argument, arg);	// Get prototype argument.
	size = NUM_PROTOTYPES;
	for(x=0;x<size;x++)
	{
		if(nifty_is_name_prefix(arg,ship_prototypes[x].name))
		{
			found_proto = TRUE;
			break;
		}
	}

	if(!found_proto)
	{
		send_to_char("&RPrototype not found.&w\r\n",ch);
		return;
	}

	ship_type = x;
	if(argument[0] == '\0' || argument == NULL || !argument)
	{
		send_to_char("What do you want to name the ship?\r\n",ch);
		return;
	}

	vnum = find_vnum_block(ship_prototypes[ship_type].num_rooms);
	if(vnum == -1)
	{
		send_to_char("There was a problem with your ship: free vnums. Notify an administrator.\r\n",ch);
		bug("Ship Shop area is low on vnums.",0);
		return;
	}


	char arg2[MAX_STRING_LENGTH];
	argument = one_argument(argument, arg2);
	arg2[0] = UPPER(arg2[0]);
	sprintf(arg2,"%s (%s)",arg2, ship_prototypes[ship_type].sname);

	for ( ship = first_ship; ship; ship = ship->next )
	{
		if(!str_cmp(ship->name,arg2))
		{
			send_to_char("&CThat ship name is already in use. Choose another.\r\n",ch);
			return;
		}
	}

	if(!str_cmp(arg2, "&"))
	{
		send_to_char("No color codes in ship names.\n\r", ch);
		return;
	}

	for ( tarea = first_area; tarea; tarea = tarea->next )
		if ( !str_cmp( SHIP_AREA, tarea->filename ) )
			break;


	if(argument[0] == '\0' || argument == NULL || !argument)
	{
		send_to_char("&RWhat starsystem do you want to place the ship in?&w\r\n",ch);
		return;
	}

	for(system = first_starsystem; system; system = system->next)
	{
	   if(nifty_is_name(argument, system->name))
		{
		   fsys = TRUE;
		   break;
		}
	}
   if(!fsys)
	{
	   send_to_char("&RNo such starsystem.&w\n\r", ch);
	   return;
	}

	if(make_prototype_rooms(ship_type, vnum, tarea, arg2) == -1)
	{
		send_to_char("There was a problem with your ship: unable to create a room. Notify an administrator.\n\r",ch);
		bug("Ship Shop unable to make_room.",0);
		return;
	}

   ship = make_prototype_ship(ship_type,vnum,ch,arg2);
       ship->owner = STRALLOC("");
       save_ship( ship );
       write_ship_list();
   	extract_ship(ship);
   	ship_to_starsystem(ship, system);
   	ship->location = 0;
   	ship->inship = NULL;
   	ship->type = MOB_SHIP;
   	SET_BIT(ship->flags, SHIP_RESPAWN);
   	if(ship->home) STRFREE(ship->home);
   	ship->home = STRALLOC(system->name);
   	ship->hx = ship->hy = ship->hz = 1;
   	ship->vx = number_range(-3000, 3000);
   	ship->vy = number_range(-3000, 3000);
   	ship->vz = number_range(-3000, 3000);
   	ship->autopilot = TRUE;
   	char buf[MAX_STRING_LENGTH];
   	sprintf(buf, "%s enters the starsystem at %.0f %.0f %.0f", ship->name, ship->vx, ship->vy, ship->vz);
    echo_to_system(AT_YELLOW, ship, buf, NULL);

	return;
}

SHIP_DATA *make_prototype_ship(int ship_type,int vnum,CHAR_DATA *ch,char *ship_name)
{
  SHIP_DATA *ship;
  PROTO_ROOM *proom;
  ROOM_INDEX_DATA *room;
  char sp_filename[MAX_STRING_LENGTH];
    CREATE( ship, SHIP_DATA, 1 );
    LINK( ship, first_ship, last_ship, next, prev );
    ship->name		= STRALLOC( ship_name );
    ship->owner		= STRALLOC( "" ); 
    ship->protoname     = STRALLOC( ship_prototypes[ship_type].sname );

    if( !ship_prototypes[ship_type].clan || ( ship_prototypes[ship_type].clan[0] == '\0' ) )
       ship->clanowner = STRALLOC( "" );
    else
       ship->clanowner = STRALLOC( ship_prototypes[ship_type].clan );

    ship->description	= STRALLOC( "" );
    ship->copilot       = STRALLOC( "" );
    ship->pilot         = STRALLOC( "" );
 
    /* Added in check to make sure all things cruiser+ get put in 
       the right starsystem by Keberus, thanks Guido 
     */ 
    if(ch->in_room->area && ch->in_room->area->planet)
    {
	// This was causing a bug - even capital ships should stay on the group until they're launched -- Boran
        if( ship_prototypes[ship_type].class >= SHIP_LFRIGATE &&  ch->in_room->area->planet->starsystem )
           ship->home = STRALLOC( ch->in_room->area->planet->starsystem->name );
        else
           ship->home = STRALLOC( ch->in_room->area->planet->name );
    }
    else
       ship->home = STRALLOC( "" );

    ship->pbeacon       = STRALLOC( "" );
    ship->type          = PLAYER_SHIP;
    ship->class         = ship_prototypes[ship_type].class;
    ship->cost		= ship_prototypes[ship_type].cost;
    ship->starsystem    = NULL;
    ship->in_room       = NULL;
    ship->next_in_room  = NULL;
    ship->prev_in_room  = NULL;
    ship->currjump      = NULL;
    ship->target0       = NULL;
    ship->target1       = NULL;
    ship->target2       = NULL;
    ship->target3       = NULL;
    ship->target4       = NULL;
    ship->target5       = NULL;
    ship->target6       = NULL;
    ship->target7       = NULL;
    ship->target8       = NULL;
    ship->target9       = NULL;
    ship->target10      = NULL;
    ship->password      = number_range(1111,9999);;
    ship->maxmods	= ship_prototypes[ship_type].mods;
    sprintf(sp_filename,"ship_%d.sss",vnum);
    ship->filename      = str_dup( sp_filename );
	ship->pilotseat = vnum;
    	ship->coseat = vnum;
    	ship->navseat = vnum;
    	ship->gunseat = vnum;
    	ship->entrance = vnum;
    	ship->engineroom = vnum;
    for(proom = first_prototype_room;proom;proom = proom->next)
    {
        if(proom->what_prototype == ship_type)
        {
            if(proom->room_num == 1)
                ship->firstroom = vnum;
            if(proom->room_num == ship_prototypes[ship_type].num_rooms)
                ship->lastroom = proom->room_num+vnum-1;
        }
    }
    for(proom = first_prototype_room;proom;proom = proom->next)
    {
        if(proom->what_prototype == ship_type)
        {
            room = get_room_index(proom->room_num+vnum-1);
            switch(proom->room_type)
            {
              case SP_COCKPIT     : ship->cockpit    = proom->room_num+vnum-1; break;
              case SP_ENTRANCE    : ship->entrance   = proom->room_num+vnum-1; break;
              case SP_HANGER1     : ship->hangar1    = proom->room_num+vnum-1; break;
              case SP_HANGER2     : ship->hangar2    = proom->room_num+vnum-1; break;
              case SP_HANGER3     : ship->hangar3    = proom->room_num+vnum-1; break;
              case SP_HANGER4     : ship->hangar4    = proom->room_num+vnum-1; break;
              case SP_ENGINEROOM  : ship->engineroom = proom->room_num+vnum-1; break;
              case SP_PILOT       : ship->pilotseat  = proom->room_num+vnum-1; break;
              case SP_COPILOT     : ship->coseat     = proom->room_num+vnum-1; break;
              case SP_NAVIGATOR   : ship->navseat    = proom->room_num+vnum-1; break;
              case SP_GUNNER      : ship->gunseat    = proom->room_num+vnum-1; break;
              case SP_TURRET1     : ship->turret1    = proom->room_num+vnum-1; break;
              case SP_TURRET2     : ship->turret2    = proom->room_num+vnum-1; break;
              case SP_TURRET3     : ship->turret3    = proom->room_num+vnum-1; break;
              case SP_TURRET4     : ship->turret4    = proom->room_num+vnum-1; break;
              case SP_TURRET5     : ship->turret5    = proom->room_num+vnum-1; break;
              case SP_TURRET6     : ship->turret6    = proom->room_num+vnum-1; break;
              case SP_TURRET7     : ship->turret7    = proom->room_num+vnum-1; break;
              case SP_TURRET8     : ship->turret8    = proom->room_num+vnum-1; break;
              case SP_TURRET9     : ship->turret9    = proom->room_num+vnum-1; break;
              case SP_TURRET10    : ship->turret10   = proom->room_num+vnum-1; break;
	      case SP_WINDOW1	  : ship->window1    = proom->room_num+vnum-1; break;
              case SP_WINDOW2     : ship->window2    = proom->room_num+vnum-1; break;
              case SP_WINDOW3     : ship->window3    = proom->room_num+vnum-1; break;
              case SP_WINDOW4     : ship->window4    = proom->room_num+vnum-1; break;
              case SP_WINDOW5     : ship->window5    = proom->room_num+vnum-1; break;
              case SP_WINDOW6     : ship->window6    = proom->room_num+vnum-1; break;
              default             : break;
            }
        }
    }
    ship->tractorbeam = ship_prototypes[ship_type].tractor;
    ship->gravitywell = ship_prototypes[ship_type].gwell;
    ship->grav = FALSE;
    ship->primaryType = ship_prototypes[ship_type].primaryType;
    ship->secondaryType = ship_prototypes[ship_type].secondaryType;
    ship->tertiaryType = ship_prototypes[ship_type].tertiaryType;
	ship->primaryCount = ship_prototypes[ship_type].primaryCount;
	ship->secondaryCount = ship_prototypes[ship_type].secondaryCount;
	ship->tertiaryCount = ship_prototypes[ship_type].tertiaryCount;
    ship->missiles = ship_prototypes[ship_type].range_weapons[0];
    ship->torpedos = ship_prototypes[ship_type].range_weapons[1];
    ship->rockets = ship_prototypes[ship_type].range_weapons[2];
    ship->maxmissiles = ship_prototypes[ship_type].range_weapons[0];
    ship->maxtorpedos = ship_prototypes[ship_type].range_weapons[1];
    ship->maxrockets = ship_prototypes[ship_type].range_weapons[2];
    ship->maxhull = ship_prototypes[ship_type].hull;
    ship->hull = ship_prototypes[ship_type].hull;
    ship->shield = ship_prototypes[ship_type].shields;
    ship->maxshield = ship_prototypes[ship_type].shields;
    //ship->plasmashield = ship_prototypes[ship_type].plasma;
    //ship->maxplasmashield = ship_prototypes[ship_type].plasma;
    ship->energy = ship_prototypes[ship_type].energy;
    ship->maxenergy = ship_prototypes[ship_type].energy;
    ship->countermeasures = ship_prototypes[ship_type].countermeasures;
    ship->maxcountermeasures = ship_prototypes[ship_type].countermeasures;
    ship->bombs = ship_prototypes[ship_type].maxbombs;
    ship->maxbombs = ship_prototypes[ship_type].maxbombs;
    ship->realspeed = ship_prototypes[ship_type].speed;
    ship->hyperspeed = ship_prototypes[ship_type].hyperspeed;
#ifdef USECARGO
    ship->maxcargo = ship_prototypes[ship_type].maxcargo;
#endif
	ship->hangar1space = ship_prototypes[ship_type].hangar1space;
	ship->hangar2space = ship_prototypes[ship_type].hangar2space;
	ship->hangar3space = ship_prototypes[ship_type].hangar3space;
	ship->hangar4space = ship_prototypes[ship_type].hangar4space;
    ship->manuever = ship_prototypes[ship_type].manuever;
  //MARKER
        ship->shipyard = ch->in_room->vnum;
    resetship( ship );
    ship->owner 	= STRALLOC( ch->name );
    save_ship( ship );
    write_ship_list( );
  return ship;
}       	
    
int make_prototype_rooms(int ship_type, int vnum,AREA_DATA *tarea,char *Sname)
{
  PROTO_ROOM *proom;
  ROOM_INDEX_DATA *newroom,*rfrom,*rto;
  int value;
  char  *arg;
  char  arg2[MAX_STRING_LENGTH];
  EXIT_DATA *xit;
  int x,y;
  int *exits;
  int *keys;
  int *exflags;
  char buf[MAX_STRING_LENGTH];
  char newdesc[MAX_STRING_LENGTH];
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;
  CHAR_DATA *victim;
  MOB_INDEX_DATA *pMobIndex;
  int rvnum;
  char *rarg;
  char rtype[MAX_STRING_LENGTH];
  
    for(proom = first_prototype_room;proom;proom = proom->next)
    {
        if(proom->what_prototype == ship_type)
        {
            if( (newroom = get_room_index(vnum+proom->room_num-1)) == NULL)
            {
                newroom = make_room(vnum+proom->room_num-1, tarea);
                if(!newroom)
                  return -1;
            }
            newroom->area = tarea;
            if(proom->room_type == SP_NOT_SET)
            {
                if(newroom->name) 
                    STRFREE(newroom->name);
                sprintf(buf,"SPARE ROOM FOR: %d",vnum);
                newroom->name = STRALLOC( buf );
                if(newroom->description) 
                    STRFREE(newroom->description);
                newroom->description = STRALLOC( "" );
              continue;
            }
	    strcpy(newdesc,strlinwrp(proom->desc,60));
            if(newroom->name) STRFREE(newroom->name);
            newroom->name = STRALLOC( strrep(proom->name,"$SN$",Sname) );
            if(newroom->description) STRFREE(newroom->description);
            newroom->description = STRALLOC( strrep(newdesc,"$SN$",Sname) );
            arg = STRALLOC( proom->flags );
            newroom->tunnel = proom->tunnel;
            while ( arg[0] != '\0' )
	    {
	        arg = one_argument( arg, arg2 );
	        value = get_sp_rflag( arg2 );
  	        TOGGLE_BIT( newroom->room_flags, 1 << value );
	    }
	    for(y=0;y<10;y++)
	    {
	    	if(proom->reset[y] != NULL && proom->reset[y][0] != '\0')
	    	{
	    	    rarg = STRALLOC(proom->reset[y]);
	    	    rarg = one_argument(rarg,rtype);
	    	    rvnum = atoi(rarg);
	    	    switch(UPPER(rtype[0]))
	    	    {
	    	    	case 'O':
     	    	            if( ( pObjIndex = get_obj_index( rvnum ) ) == NULL)
     	    	              break;
     	    	            obj = create_object( pObjIndex, 60 );
     	    	            obj = obj_to_room( obj, newroom );
     	    	          break;
     	    	        case 'M':
     	    	            if( ( pMobIndex = get_mob_index( rvnum ) ) == NULL)
     	    	              break;
     	    	            victim = create_mobile( pMobIndex );
         	    	    char_to_room( victim, newroom );
         	    	  break;
                        default:
                          break;
         	    }
         	}
            }
            instaroom(tarea,newroom,TRUE);
        }
    }
    for(proom = first_prototype_room;proom;proom = proom->next)
    {
        if(proom->what_prototype == ship_type)
        {
            exits = proom->exits;
            keys = proom->keys;
            exflags = proom->exflags;
            for(x=0;x<10;x++)
            {
                if(exits[x] > 0)
                {
                    rfrom = get_room_index(vnum+proom->room_num-1);
                    rto = get_room_index(vnum+exits[x]-1);
                    xit = make_exit( rfrom, rto, x );
                    xit->keyword                = STRALLOC( "" );
                    xit->description    = STRALLOC( "" );
                    xit->key            = keys[x] == 0 ? -1 : vnum+keys[x]-1;
                    xit->exit_info              = exflags[x];
                }
            }
        }
    }
    make_rprogs(ship_type, vnum);
    fold_area( tarea, tarea->filename, FALSE );   
  return 1;
}



int find_vnum_block(int num_needed)
{
  bool counting = FALSE;
  int count = 0;
  AREA_DATA *tarea;
  int lrange;
  int trange;
  int vnum;
  int startvnum = -1;
  ROOM_INDEX_DATA *room;
  
    for ( tarea = first_area; tarea; tarea = tarea->next )
        if ( !str_cmp( SHIP_AREA, tarea->filename ) )
	  break;
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

int get_sp_rflag( char *flag )
{
    int x;
    
    for ( x = 0; x < 32; x++ )
      if ( !str_cmp( flag, r_flags[x] ) )
        return x;
    return -1;
}

void make_rprogs(int ship_type, int vnum)
{
  PROTO_ROOM *proom;
  char argument[MAX_STRING_LENGTH];
  char arg1 [MAX_INPUT_LENGTH];
  char arg2 [MAX_INPUT_LENGTH];
  MPROG_DATA *mprog, *mprg;
  ROOM_INDEX_DATA *room;
  int mptype, x, size;
    for(proom = first_prototype_room;proom;proom = proom->next)
    {
        if(proom->what_prototype == ship_type)
        {
            size = 10;
            for(x=0;x<size;x++)
            {
                strcpy(argument, proom->rprog[x]);
                if(argument[0] == '\0')
                    continue;
                strcpy(argument, one_argument( argument, arg1 ) );
                strcpy(argument, one_argument( argument, arg2 ) );
                sprintf(argument,"%s",parse_prog_string(argument,ship_type,vnum));
                room = get_room_index(proom->room_num+vnum-1);
                mprog = room->mudprogs;
	        mptype = get_mpflag( arg1 );
	        if ( mprog )
	          for ( ; mprog->next; mprog = mprog->next );
	            CREATE( mprg, MPROG_DATA, 1 );
	        if ( mprog )
	          mprog->next		= mprg;
	        else
	         room->mudprogs	= mprg;
	        room->progtypes |= ( 1 << mptype );


    	        if ( mptype != -1 )
	        {
	          mprg->type = 1 << mptype;
	          if ( mprg->arglist )
	            STRFREE( mprg->arglist );
	          mprg->arglist = STRALLOC( arg2 );
	        }
                mprg->comlist = STRALLOC( argument );
	        mprg->next = NULL;
            }
        }
    }
}

char *parse_prog_string(char *inp,int ship_type,int vnum)
{
  char sch[MAX_STRING_LENGTH];
  char rep[MAX_STRING_LENGTH];
  static char newinp[MAX_STRING_LENGTH];
  int x,size;
    strcpy(newinp,inp);
    size = ship_prototypes[ship_type].num_rooms;
    for(x=0;x<size;x++)
    {
        sprintf(sch,"$RNUM:%d$",x+1);
        sprintf(rep,"%d",x+vnum);
        sprintf(newinp,"%s",strrep(newinp,sch,rep));
    }
  return newinp;
}

void save_prototype( int prototype )
{
    PROTO_ROOM          *proom;
    FILE		*fpout;
    char                filename[MAX_STRING_LENGTH];
    int x;
    fclose( fpReserve );
    sprintf(filename,"%s%s.proto",SHIP_PROTOTYPE_DIR,ship_prototypes[prototype].sname);
    if ( ( fpout = fopen( filename, "w" ) ) == NULL )
    {
	perror( filename );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
    }
    fprintf( fpout, "#HEADER\n\n");
    fprintf( fpout, "Name           %s~\n", ship_prototypes[prototype].name );
    fprintf( fpout, "Sname          %s~\n", ship_prototypes[prototype].sname );
    fprintf( fpout, "NumRooms       %d\n",  ship_prototypes[prototype].num_rooms );
    fprintf( fpout, "Cost           %d\n",  ship_prototypes[prototype].cost );
    fprintf( fpout, "Class          %d\n",  ship_prototypes[prototype].class );
    fprintf( fpout, "Tractor        %d\n",  ship_prototypes[prototype].tractor );
    fprintf( fpout, "Gwells         %d\n", ship_prototypes[prototype].gwell );
    fprintf( fpout, "primaryType         %d\n",  ship_prototypes[prototype].primaryType );
	fprintf( fpout, "secondaryType		 %d\n",  ship_prototypes[prototype].secondaryType);
	fprintf( fpout, "tertiaryType		 %d\n",  ship_prototypes[prototype].secondaryType);
    fprintf( fpout, "primaryCount		 %d\n",  ship_prototypes[prototype].primaryCount);
	fprintf( fpout, "secondaryCount		 %d\n",  ship_prototypes[prototype].secondaryCount);
	fprintf( fpout, "tertiaryCount		 %d\n",  ship_prototypes[prototype].secondaryCount);
    fprintf( fpout, "RangeWeapons0  %d\n",  ship_prototypes[prototype].range_weapons[0] );
    fprintf( fpout, "RangeWeapons1  %d\n",  ship_prototypes[prototype].range_weapons[1] );
    fprintf( fpout, "RangeWeapons2  %d\n",  ship_prototypes[prototype].range_weapons[2] );
    fprintf( fpout, "Hull           %d\n",  ship_prototypes[prototype].hull );
    fprintf( fpout, "Shields        %d\n",  ship_prototypes[prototype].shields );
    fprintf( fpout, "Energy         %d\n",  ship_prototypes[prototype].energy );
    fprintf( fpout, "Countermeasures          %d\n",  ship_prototypes[prototype].countermeasures );
    fprintf( fpout, "MaxCargo       %d\n",  ship_prototypes[prototype].maxcargo );
	fprintf( fpout, "Hanger1Space   %d\n",  ship_prototypes[prototype].hangar1space );
	fprintf( fpout, "Hanger2Space   %d\n",  ship_prototypes[prototype].hangar2space );
	fprintf( fpout, "Hanger3Space   %d\n",  ship_prototypes[prototype].hangar3space );
	fprintf( fpout, "Hanger4Space   %d\n",  ship_prototypes[prototype].hangar4space );
    fprintf( fpout, "Maxbombs       %d\n",  ship_prototypes[prototype].maxbombs );
    fprintf( fpout, "Speed          %d\n",  ship_prototypes[prototype].speed );
    fprintf( fpout, "Hyperspeed     %d\n",  ship_prototypes[prototype].hyperspeed );
    fprintf( fpout, "Manuever       %d\n",  ship_prototypes[prototype].manuever );
    fprintf( fpout, "Turrets        %d\n",  ship_prototypes[prototype].turrets );
    fprintf( fpout, "Windows        %d\n",  ship_prototypes[prototype].windows );
    fprintf( fpout, "Mods	    %d\n",  ship_prototypes[prototype].mods );
    fprintf( fpout, "Clan           %s~\n", ship_prototypes[prototype].clan );
    //fprintf( fpout, "Plasma         %d\n",  ship_prototypes[prototype].plasma );
    fprintf( fpout, "End\n");
    fprintf( fpout, "\n#ROOMS\n\n" );
    for(proom = first_prototype_room;proom;proom = proom->next)
    {
      if(proom->what_prototype == prototype)
      {
        fprintf( fpout, "#ROOM\n\n");
        fprintf( fpout, "RoomNum           %d\n", proom->room_num );
        fprintf( fpout, "Name              %s~\n",proom->name  );
        fprintf( fpout, "Desc              %s~\n",proom->desc  );
        fprintf( fpout, "Flags             %s~\n",proom->flags );
        fprintf( fpout, "Tunnel            %d\n", proom->tunnel );
        for(x=0;x<10;x++)
            fprintf( fpout, "Exits%d       %d\n", x, proom->exits[x] );        
        for(x=0;x<10;x++)
            fprintf( fpout, "Exflags%d     %d\n", x, proom->exflags[x] );        
        for(x=0;x<10;x++)
            fprintf( fpout, "Keys%d        %d\n", x, proom->keys[x] );        
        fprintf( fpout, "RoomType          %d\n", proom->room_type );
        for(x=0;x<10;x++)
            fprintf( fpout, "Rprog%d        %s~\n", x, proom->rprog[x] );        
        for(x=0;x<10;x++)
            fprintf( fpout, "Reset%d        %s~\n", x, proom->reset[x] );        
        fprintf( fpout,"End\n\n");
      }
    }
    fprintf( fpout, "\n#ENDROOMS\n\n" );

    fprintf( fpout,"#END\n" );
    fclose( fpout );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void write_prototype_list()
{
    FILE		*fpout;
    char                filename[MAX_STRING_LENGTH];
    int x;
    fclose( fpReserve );
    sprintf(filename,"%sprototype.lst",SHIP_PROTOTYPE_DIR);
    if ( ( fpout = fopen( filename, "w" ) ) == NULL )
    {
	perror( filename );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
    }
    for(x=0;x<NUM_PROTOTYPES;x++)
        fprintf(fpout, "%s.proto\n",ship_prototypes[x].sname);
    fprintf(fpout,"$\n");
    fclose( fpout );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}
    
void write_all_prototypes()
{
  int x;
    for(x=0;x<NUM_PROTOTYPES;x++)
        save_prototype(x);
    write_prototype_list();
}

bool load_prototype_header(FILE *fp,int prototype)
{
  char buf[MAX_STRING_LENGTH];
  char *word;
  bool done = FALSE;
  bool fMatch;
    while(!done)
    {
      word   = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;
	switch ( UPPER(word[0]) )
	{
           case 'C':
               KEY( "Cost",           ship_prototypes[prototype].cost,             fread_number(fp));
               KEY( "Class",          ship_prototypes[prototype].class,            fread_number(fp));
               KEY( "Countermeasures",          ship_prototypes[prototype].countermeasures,            fread_number(fp));
               KEY( "Clan",	      ship_prototypes[prototype].clan,		   fread_string(fp));
           case 'E':
               KEY( "Energy",         ship_prototypes[prototype].energy,           fread_number(fp));
  	       if ( !str_cmp( word, "End" ) )
	       {
	       	    done = TRUE;
	       	    fMatch = TRUE;
	       	  break;
	       }
        	 case 'G':
           	   KEY( "Gwell", ship_prototypes[prototype].gwell, fread_number( fp ) );
           case 'H':
				KEY( "Hangar1Space",   ship_prototypes[prototype].hangar1space,     fread_number(fp));
				KEY( "Hangar2Space",   ship_prototypes[prototype].hangar2space,     fread_number(fp));
				KEY( "Hangar3Space",   ship_prototypes[prototype].hangar3space,     fread_number(fp));
				KEY( "Hangar4Space",   ship_prototypes[prototype].hangar4space,     fread_number(fp));
				// Included for legacy compatibility
				KEY( "Hanger1Space",   ship_prototypes[prototype].hangar1space,     fread_number(fp));
				KEY( "Hanger2Space",   ship_prototypes[prototype].hangar2space,     fread_number(fp));
				KEY( "Hanger3Space",   ship_prototypes[prototype].hangar3space,     fread_number(fp));
				KEY( "Hanger4Space",   ship_prototypes[prototype].hangar4space,     fread_number(fp));
               KEY( "Hull",           ship_prototypes[prototype].hull,             fread_number(fp));
               KEY( "Hyperspeed",     ship_prototypes[prototype].hyperspeed,       fread_number(fp));

           case 'M':
               KEY( "Maxbombs",       ship_prototypes[prototype].maxbombs,            fread_number(fp));
               KEY( "MaxCargo",       ship_prototypes[prototype].maxcargo,         fread_number(fp));
               KEY( "Manuever",       ship_prototypes[prototype].manuever,         fread_number(fp));
               KEY( "Mods",	      ship_prototypes[prototype].mods,		   fread_number(fp));
           case 'N':
               KEY( "Name",           ship_prototypes[prototype].name,             fread_string(fp));
               KEY( "NumRooms",       ship_prototypes[prototype].num_rooms,        fread_number(fp));
           case 'P':
               KEY( "primaryType",         ship_prototypes[prototype].primaryType,           fread_number(fp));
		       KEY( "primaryCount", ship_prototypes[prototype].primaryCount, fread_number(fp));
           case 'R':
               KEY( "RangeWeapons0",  ship_prototypes[prototype].range_weapons[0], fread_number(fp));
               KEY( "RangeWeapons1",  ship_prototypes[prototype].range_weapons[1], fread_number(fp));
               KEY( "RangeWeapons2",  ship_prototypes[prototype].range_weapons[2], fread_number(fp));
           case 'S':
			   KEY( "secondaryType",  ship_prototypes[prototype].secondaryType, fread_number(fp));
			   KEY( "secondaryCount", ship_prototypes[prototype].secondaryCount, fread_number(fp));
               KEY( "Sname",          ship_prototypes[prototype].sname,            fread_string(fp));
               KEY( "Shields",        ship_prototypes[prototype].shields,          fread_number(fp));
               KEY( "Speed",          ship_prototypes[prototype].speed,            fread_number(fp));
           case 'T':
			   KEY( "tertiaryType",  ship_prototypes[prototype].tertiaryType, fread_number(fp));
			   KEY( "tertiaryCount", ship_prototypes[prototype].tertiaryCount, fread_number(fp));
               KEY( "Tractor",        ship_prototypes[prototype].tractor,          fread_number(fp));
               KEY( "Turrets",        ship_prototypes[prototype].turrets,          fread_number(fp));
	   case 'W':
               KEY( "Windows",        ship_prototypes[prototype].windows,          fread_number(fp));

        }
	if ( !fMatch )
	{
	    sprintf( buf, "Load_prototype_header: no match: %s", word );
	    bug( buf, 0 );
	}
    }
  return TRUE;
}

bool fread_prototype_room( FILE *fp, int prototype)
{
  PROTO_ROOM *proom;
  char buf[MAX_STRING_LENGTH];
  char *word;
  bool done = FALSE;
  bool fMatch;
    CREATE(proom, PROTO_ROOM, 1);
    proom->what_prototype = prototype;
    while(!done)
    {
      word   = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;
	switch ( UPPER(word[0]) )
	{
           case 'D':
               KEY( "Desc", proom->desc, fread_string(fp));
           case 'E':
  	       if ( !str_cmp( word, "End" ) )
	       {
	       	    done = TRUE;
	       	    fMatch = TRUE;
	       	  break;
	       }
               KEY( "Exits0", proom->exits[0], fread_number(fp));               
               KEY( "Exits1", proom->exits[1], fread_number(fp));               
               KEY( "Exits2", proom->exits[2], fread_number(fp));               
               KEY( "Exits3", proom->exits[3], fread_number(fp));               
               KEY( "Exits4", proom->exits[4], fread_number(fp));               
               KEY( "Exits5", proom->exits[5], fread_number(fp));               
               KEY( "Exits6", proom->exits[6], fread_number(fp));               
               KEY( "Exits7", proom->exits[7], fread_number(fp));               
               KEY( "Exits8", proom->exits[8], fread_number(fp));               
               KEY( "Exits9", proom->exits[9], fread_number(fp));               
               KEY( "Exflags0", proom->exflags[0], fread_number(fp));
               KEY( "Exflags1", proom->exflags[1], fread_number(fp));
               KEY( "Exflags2", proom->exflags[2], fread_number(fp));
               KEY( "Exflags3", proom->exflags[3], fread_number(fp));
               KEY( "Exflags4", proom->exflags[4], fread_number(fp));
               KEY( "Exflags5", proom->exflags[5], fread_number(fp));
               KEY( "Exflags6", proom->exflags[6], fread_number(fp));
               KEY( "Exflags7", proom->exflags[7], fread_number(fp));
               KEY( "Exflags8", proom->exflags[8], fread_number(fp));
               KEY( "Exflags9", proom->exflags[9], fread_number(fp));
           case 'F':
               KEY( "Flags", proom->flags, fread_string(fp));
           case 'K':
               KEY( "Keys0", proom->keys[0], fread_number(fp));
               KEY( "Keys1", proom->keys[1], fread_number(fp));
               KEY( "Keys2", proom->keys[2], fread_number(fp));
               KEY( "Keys3", proom->keys[3], fread_number(fp));
               KEY( "Keys4", proom->keys[4], fread_number(fp));
               KEY( "Keys5", proom->keys[5], fread_number(fp));
               KEY( "Keys6", proom->keys[6], fread_number(fp));
               KEY( "Keys7", proom->keys[7], fread_number(fp));
               KEY( "Keys8", proom->keys[8], fread_number(fp));
               KEY( "Keys9", proom->keys[9], fread_number(fp));
           case 'N':
               KEY( "Name", proom->name, fread_string(fp));
           case 'R':
               KEY( "RoomType", proom->room_type, fread_number(fp));
               KEY( "RoomNum", proom->room_num, fread_number(fp));
               KEY( "Rprog0", proom->rprog[0], fread_string(fp));
               KEY( "Rprog1", proom->rprog[1], fread_string(fp));
               KEY( "Rprog2", proom->rprog[2], fread_string(fp));
               KEY( "Rprog3", proom->rprog[3], fread_string(fp));
               KEY( "Rprog4", proom->rprog[4], fread_string(fp));
               KEY( "Rprog5", proom->rprog[5], fread_string(fp));
               KEY( "Rprog6", proom->rprog[6], fread_string(fp));
               KEY( "Rprog7", proom->rprog[7], fread_string(fp));
               KEY( "Rprog8", proom->rprog[8], fread_string(fp));
               KEY( "Rprog9", proom->rprog[9], fread_string(fp));
               KEY( "Reset0", proom->reset[0], fread_string(fp));
               KEY( "Reset1", proom->reset[1], fread_string(fp));
               KEY( "Reset2", proom->reset[2], fread_string(fp));
               KEY( "Reset3", proom->reset[3], fread_string(fp));
               KEY( "Reset4", proom->reset[4], fread_string(fp));
               KEY( "Reset5", proom->reset[5], fread_string(fp));
               KEY( "Reset6", proom->reset[6], fread_string(fp));
               KEY( "Reset7", proom->reset[7], fread_string(fp));
               KEY( "Reset8", proom->reset[8], fread_string(fp));
               KEY( "Reset9", proom->reset[9], fread_string(fp));
           case 'T':
               KEY( "Tunnel", proom->tunnel, fread_number(fp));
       }
       if ( !fMatch )
       {
	    sprintf( buf, "Fread_prototype_room: no match: %s", word );
	    bug( buf, 0 );
       }
    }
    LINK(proom,first_prototype_room,last_prototype_room,next,prev);
  return TRUE;
}
    
bool load_prototype_rooms( FILE *fp, int prototype)
{
  char letter;
  char *word;
  bool done = FALSE;
    while(!done)
    {
    	letter = fread_letter( fp );
    	if( letter == '#')
    	{
    	    word = fread_word( fp );
            if(!strcmp("ROOM",word))
            {
            	if(!fread_prototype_room(fp,prototype))
                    return FALSE;
            }
            if(!strcmp("ENDROOMS",word))
            	done = TRUE;
        }
        else
        {
            bug("Load_prototype_rooms, unknown prefix: %s",letter);
          return FALSE;
        }
    }
  return TRUE;
}

int load_prototype( char *prototypefile, int prototype)
{
    char filename[256];
    FILE *fp;
    bool found = FALSE;
    int stage = -1;
    bool ok = TRUE;
    char letter;
    char *word;
    
    sprintf( filename, "%s%s", SHIP_PROTOTYPE_DIR, prototypefile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {
        found = TRUE;
        prototype++;
        while(ok)
        {
	    letter = fread_letter( fp );
	    if ( letter != '#' )
	    {
		bug( "Load_prototype: # not found.", 0 );
		break;
	    }
            stage++;
            word = fread_word( fp );
            if(strcmp(word,"END"))
            {
                switch(stage)
                {
                    case 0:
                        if(strcmp(word,"HEADER"))
                        {
                            bug( "Load_prototype: HEADER not found.", 0);
                          break;
                        }
                        if(!load_prototype_header(fp,prototype))
                        {
                           ok = FALSE;
                           prototype--;
                        }
                      break;
                    case 1:
                        if(strcmp(word,"ROOMS"))
                        {
                            bug( "Load_prototype: ROOMS not found.", 0);
                          break;
                        }
                        if(!load_prototype_rooms(fp,prototype))
                        {
                           ok = FALSE;
                           prototype--;
                        }
                      break;
                    default:
                        bug( "Load_prototype: Unknown stage: %d in %s",stage, filename);
                }
            }
            else
              ok = FALSE;                  
	}
	fclose( fp );
    }
    return prototype;
}

void load_ship_prototypes()
{
    FILE *fpList;
    char *filename;
    char prototypeslist[256];
    int prototype;
    first_prototype_room = NULL;
    last_prototype_room = NULL;

    log_string( "Loading ship prototypes..." );

    sprintf(prototypeslist,"%sprototype.lst",SHIP_PROTOTYPE_DIR);
    fclose( fpReserve );
    if ( ( fpList = fopen( prototypeslist, "r" ) ) == NULL )
    {
	perror( prototypeslist );
	exit( 1 );
    }

    for ( prototype = -1; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	if ( filename[0] == '$' )
	  break;
	  
       
	prototype = load_prototype( filename, prototype );
    }
    fclose( fpList );
    log_string(" Done ship prototypes " );
    NUM_PROTOTYPES = prototype+1;
    fpReserve = fopen( NULL_FILE, "r" );
   // write_all_prototypes();
  return;
}

void do_makeprototypeship(CHAR_DATA *ch, char *argument)
{
  SHIP_DATA *ship;
  int prototype;
  int cost;
  int count;
  char ship_name[MAX_STRING_LENGTH];
  char name[MAX_STRING_LENGTH];
  char sname[MAX_STRING_LENGTH];
  char scost[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int x,len;
  ROOM_INDEX_DATA *room;
  MPROG_DATA *mprg;
  EXIT_DATA *eroom;
  int room_count = 1;
  PROTO_ROOM *proom;

    argument = one_argument(argument,ship_name);
    argument = one_argument(argument,scost);
    argument = one_argument(argument,sname);
    strcpy(name,argument);
    if(ship_name[0] == '\0')
    {
        send_to_char("You must specify a valid ship name.\r\n",ch);
        send_to_char("USAGE: makeprototypeship <ship name> <cost> <short name> <long name>\r\n",ch);
      return;
    }
    cost = atoi(scost);
    if(cost <= 0)
    {
        send_to_char("The cost must be greater than 0.\r\n",ch);
        send_to_char("USAGE: makeprototypeship <ship name> <cost> <short name> <long name>\r\n",ch);
      return;
    }
    if(sname[0] == '\0')
    {
        send_to_char("You must specify a short name for the ship.\r\n",ch);
        send_to_char("USAGE: makeprototypeship <ship name> <cost> <short name> <long name>\r\n",ch);
      return;
    }
    len = strlen(sname);
    for(x=0;x<len;x++)
        sname[x] = UPPER(sname[x]);
    if(name[0] == '\0')
    {
        send_to_char("You must specify a long name for the ship.\r\n",ch);
        send_to_char("USAGE: makeprototypeship <ship name> <cost> <short name> <long name>\r\n",ch);
      return;
    }
    if(scost[0] == '\0')
    {
        send_to_char("You must specify a cost for the ship.\r\n",ch);
        send_to_char("USAGE: makeprototypeship <ship name> <cost> <short name> <long name>\r\n",ch);
      return;
    }
    if(( ship = get_ship(ship_name) ) == NULL)
    {
        send_to_char("You must specify a valid ship name.\r\n",ch);
        send_to_char("USAGE: makeprototypeship <ship name> <short name> <long name> <cost>\r\n",ch);
      return;
    }
    
    prototype = NUM_PROTOTYPES++;
 
    ship_prototypes[prototype].tractor = ship->tractorbeam;
    ship_prototypes[prototype].primaryType = ship->primaryType;
	ship_prototypes[prototype].secondaryType = ship->secondaryType;
	ship_prototypes[prototype].tertiaryType = ship->tertiaryType;
	ship_prototypes[prototype].primaryCount = ship->primaryCount;
    ship_prototypes[prototype].secondaryCount = ship->secondaryCount;
    ship_prototypes[prototype].tertiaryCount = ship->tertiaryCount;
    ship_prototypes[prototype].range_weapons[0] = ship->maxmissiles;
    ship_prototypes[prototype].range_weapons[1] = ship->maxtorpedos;
    ship_prototypes[prototype].range_weapons[2] = ship->maxrockets;
    ship_prototypes[prototype].hull = ship->maxhull;
    ship_prototypes[prototype].shields = ship->maxshield;
    //ship_prototypes[prototype].plasma = ship->maxplasmashield;
    ship_prototypes[prototype].energy = ship->maxenergy;
    ship_prototypes[prototype].countermeasures = ship->maxcountermeasures;
#ifdef USECARGO
    ship_prototypes[prototype].maxcargo = ship->maxcargo;
#endif
	ship_prototypes[prototype].gwell	= ship->gravitywell;
	ship_prototypes[prototype].hangar1space = ship->hangar1space;
	ship_prototypes[prototype].hangar2space = ship->hangar2space;
	ship_prototypes[prototype].hangar3space = ship->hangar3space;
	ship_prototypes[prototype].hangar4space = ship->hangar4space;
    ship_prototypes[prototype].maxbombs = ship->maxbombs;
    ship_prototypes[prototype].speed = ship->realspeed;
    ship_prototypes[prototype].hyperspeed = ship->hyperspeed;
    ship_prototypes[prototype].manuever = ship->manuever;
    ship_prototypes[prototype].cost = cost;
    sprintf(buf,"%s: '%s'",sname,name);
    ship_prototypes[prototype].name = STRALLOC( buf );
    ship_prototypes[prototype].sname = STRALLOC( sname );
    ship_prototypes[prototype].class = ship->class;
    ship_prototypes[prototype].num_rooms = ship->lastroom - ship->firstroom + 1;
    count=0;
    count=ship->turret1+ship->turret2+ship->turret3+ship->turret4+ship->turret5+ship->turret6+ship->turret7+ship->turret8+ship->turret9+ship->turret10;
   ship_prototypes[prototype].turrets = count;
    count=ship->window1+ship->window2+ship->window3+ship->window4+ship->window5+ship->window6;
   ship_prototypes[prototype].windows = count;

    for ( room = get_room_index(ship->firstroom) ; room ; room = room->next_in_ship )
    {
        CREATE(proom, PROTO_ROOM, 1);
	proom->room_num = room_count;
        proom->name = STRALLOC(room->name);
        proom->desc = STRALLOC(room->description);
        proom->flags = STRALLOC(flag_string(room->room_flags, r_flags));
        proom->tunnel = room->tunnel;

        for(x=0;x<10;x++)
	{
	    eroom = get_exit_num(room, x);
	    if(eroom)
	    {
	    proom->exits[x] = eroom->to_room->vnum;
	    proom->exflags[x] = eroom->exit_info;
	    proom->keys[x] = eroom->key;
	    }
	}

	if( ship->cockpit == room->vnum)
	    proom->room_type = 1;
	if( ship->entrance == room->vnum)
	    proom->room_type = 2;
	if( ship->hangar1 == room->vnum)
	    proom->room_type = 3;
	if( ship->hangar2 == room->vnum)
	    proom->room_type = 4;
	if( ship->hangar3 == room->vnum)
	    proom->room_type = 5;
	if( ship->hangar4 == room->vnum)
	    proom->room_type = 6;
	if( ship->engineroom == room->vnum)
	    proom->room_type = 7;
	if( ship->pilotseat == room->vnum)
	    proom->room_type = 8;
	if( ship->coseat == room->vnum)
	    proom->room_type = 9;
	if( ship->navseat == room->vnum)
	    proom->room_type = 10;
	if( ship->gunseat == room->vnum)
	    proom->room_type = 11;
	if( ship->turret1 == room->vnum)
	    proom->room_type = 12;
	if( ship->turret2 == room->vnum)
	    proom->room_type = 13;
	if( ship->turret3 == room->vnum)
	    proom->room_type = 14;
	if( ship->turret4 == room->vnum)
	    proom->room_type = 15;
	if( ship->turret5 == room->vnum)
	    proom->room_type = 16;
	if( ship->turret6 == room->vnum)
	    proom->room_type = 17;
	if( ship->turret7 == room->vnum)
	    proom->room_type = 18;
	if( ship->turret8 == room->vnum)
	    proom->room_type = 19;
	if( ship->turret9 == room->vnum)
	    proom->room_type = 20;
	if( ship->turret10 == room->vnum)
	    proom->room_type = 21;
	if( ship->window1 == room->vnum)
	    proom->room_type = 22;
        if( ship->window2 == room->vnum)
            proom->room_type = 23;
        if( ship->window3 == room->vnum)
            proom->room_type = 24;
        if( ship->window4 == room->vnum)
            proom->room_type = 25;
        if( ship->window5 == room->vnum)
            proom->room_type = 26;
        if( ship->window6 == room->vnum)
            proom->room_type = 27;
    x = 1;
    for ( mprg = room->mudprogs; mprg; mprg = mprg->next )
    {
	sprintf(buf, "%s %s %s ~", mprog_flags[mprg->type], mprg->arglist, mprg->comlist);
	proom->rprog[x] = STRALLOC(buf);
	x++;
    }

/* Remind me to add resets later        
	for(x=0;x<10;x++)
            fprintf( fpout, "Reset%d        %s~\n", x, proom->reset[x] );
        fprintf( fpout,"End\n\n");
*/

    LINK(proom,first_prototype_room,last_prototype_room,next,prev); 
    room_count++;
    }

    send_to_char("Done.\r\n",ch);
    save_prototype(prototype);
  return;
}

char pbname[MAX_STRING_LENGTH];

char *primary_beam_name_proto(int shiptype)
{
    if(ship_prototypes[shiptype].primaryCount != 0)
	{   
		sprintf(pbname, "%d %s", ship_prototypes[shiptype].primaryCount,
		(ship_prototypes[shiptype].primaryType == LIGHT_LASER_NR && ship_prototypes[shiptype].primaryCount == 1)? "Light Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == LIGHT_LASER_NR && ship_prototypes[shiptype].primaryCount != 1)? "Light Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == SINGLE_LASER_NR && ship_prototypes[shiptype].primaryCount == 1)? "Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == SINGLE_LASER_NR && ship_prototypes[shiptype].primaryCount != 1)? "Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == DUAL_LASER_NR && ship_prototypes[shiptype].primaryCount == 1)? "Dual Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == DUAL_LASER_NR && ship_prototypes[shiptype].primaryCount != 1)? "Dual Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == TRI_LASER_NR && ship_prototypes[shiptype].primaryCount == 1)? "Triple Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == TRI_LASER_NR && ship_prototypes[shiptype].primaryCount != 1)? "Triple Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == QUAD_LASER_NR && ship_prototypes[shiptype].primaryCount == 1)? "Quad Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == QUAD_LASER_NR && ship_prototypes[shiptype].primaryCount != 1)? "Quad Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == AUTOBLASTER_NR && ship_prototypes[shiptype].primaryCount == 1)? "Repeating Blaster Cannon" :
		(ship_prototypes[shiptype].primaryType == AUTOBLASTER_NR && ship_prototypes[shiptype].primaryCount != 1)? "Repeating Blaster Cannons" :
		(ship_prototypes[shiptype].primaryType == HEAVY_LASER_NR && ship_prototypes[shiptype].primaryCount == 1)? "Heavy Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == HEAVY_LASER_NR && ship_prototypes[shiptype].primaryCount != 1)? "Heavy Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == TURBOLASER_NR && ship_prototypes[shiptype].primaryCount == 1)? "Turbolaser Cannon" :
		(ship_prototypes[shiptype].primaryType == TURBOLASER_NR && ship_prototypes[shiptype].primaryCount != 1)? "Turbolaser Cannons" :
		(ship_prototypes[shiptype].primaryType == LIGHT_LASER_GE && ship_prototypes[shiptype].primaryCount == 1)? "Light Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == LIGHT_LASER_GE && ship_prototypes[shiptype].primaryCount != 1)? "Light Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == SINGLE_LASER_GE && ship_prototypes[shiptype].primaryCount == 1)? "Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == SINGLE_LASER_GE && ship_prototypes[shiptype].primaryCount != 1)? "Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == DUAL_LASER_GE && ship_prototypes[shiptype].primaryCount == 1)? "Dual Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == DUAL_LASER_GE && ship_prototypes[shiptype].primaryCount != 1)? "Dual Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == TRI_LASER_GE && ship_prototypes[shiptype].primaryCount == 1)? "Triple Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == TRI_LASER_GE && ship_prototypes[shiptype].primaryCount != 1)? "Triple Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == QUAD_LASER_GE && ship_prototypes[shiptype].primaryCount == 1)? "Quad Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == QUAD_LASER_GE && ship_prototypes[shiptype].primaryCount != 1)? "Quad Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == AUTOBLASTER_GE && ship_prototypes[shiptype].primaryCount == 1)? "Repeating Blaster Cannon" :
		(ship_prototypes[shiptype].primaryType == AUTOBLASTER_GE && ship_prototypes[shiptype].primaryCount != 1)? "Repeating Blaster Cannons" :
		(ship_prototypes[shiptype].primaryType == HEAVY_LASER_GE && ship_prototypes[shiptype].primaryCount == 1)? "Heavy Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == HEAVY_LASER_GE && ship_prototypes[shiptype].primaryCount != 1)? "Heavy Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == TURBOLASER_GE && ship_prototypes[shiptype].primaryCount == 1)? "Turbolaser Cannon" :
		(ship_prototypes[shiptype].primaryType == TURBOLASER_GE && ship_prototypes[shiptype].primaryCount != 1)? "Turbolaser Cannons" :
		(ship_prototypes[shiptype].primaryType == LIGHT_LASER_B && ship_prototypes[shiptype].primaryCount == 1)? "Light Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == LIGHT_LASER_B && ship_prototypes[shiptype].primaryCount != 1)? "Light Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == SINGLE_LASER_B && ship_prototypes[shiptype].primaryCount == 1)? "Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == SINGLE_LASER_B && ship_prototypes[shiptype].primaryCount != 1)? "Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == DUAL_LASER_B && ship_prototypes[shiptype].primaryCount == 1)? "Dual Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == DUAL_LASER_B && ship_prototypes[shiptype].primaryCount != 1)? "Dual Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == TRI_LASER_B && ship_prototypes[shiptype].primaryCount == 1)? "Triple Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == TRI_LASER_B && ship_prototypes[shiptype].primaryCount != 1)? "Triple Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == QUAD_LASER_B && ship_prototypes[shiptype].primaryCount == 1)? "Quad Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == QUAD_LASER_B && ship_prototypes[shiptype].primaryCount != 1)? "Quad Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == AUTOBLASTER_B && ship_prototypes[shiptype].primaryCount == 1)? "Repeating Blaster Cannon" :
		(ship_prototypes[shiptype].primaryType == AUTOBLASTER_B && ship_prototypes[shiptype].primaryCount != 1)? "Repeating Blaster Cannons" :
		(ship_prototypes[shiptype].primaryType == HEAVY_LASER_B && ship_prototypes[shiptype].primaryCount == 1)? "Heavy Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == HEAVY_LASER_B && ship_prototypes[shiptype].primaryCount != 1)? "Heavy Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == TURBOLASER_B && ship_prototypes[shiptype].primaryCount == 1)? "Turbolaser Cannon" :
		(ship_prototypes[shiptype].primaryType == TURBOLASER_B && ship_prototypes[shiptype].primaryCount != 1)? "Turbolaser Cannons" :
		(ship_prototypes[shiptype].primaryType == LIGHT_LASER_P && ship_prototypes[shiptype].primaryCount == 1)? "Light Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == LIGHT_LASER_P && ship_prototypes[shiptype].primaryCount != 1)? "Light Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == SINGLE_LASER_P && ship_prototypes[shiptype].primaryCount == 1)? "Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == SINGLE_LASER_P && ship_prototypes[shiptype].primaryCount != 1)? "Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == DUAL_LASER_P && ship_prototypes[shiptype].primaryCount == 1)? "Dual Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == DUAL_LASER_P && ship_prototypes[shiptype].primaryCount != 1)? "Dual Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == TRI_LASER_P && ship_prototypes[shiptype].primaryCount == 1)? "Triple Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == TRI_LASER_P && ship_prototypes[shiptype].primaryCount != 1)? "Triple Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == QUAD_LASER_P && ship_prototypes[shiptype].primaryCount == 1)? "Quad Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == QUAD_LASER_P && ship_prototypes[shiptype].primaryCount != 1)? "Quad Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == AUTOBLASTER_P && ship_prototypes[shiptype].primaryCount == 1)? "Repeating Blaster Cannon" :
		(ship_prototypes[shiptype].primaryType == AUTOBLASTER_P && ship_prototypes[shiptype].primaryCount != 1)? "Repeating Blaster Cannons" :
		(ship_prototypes[shiptype].primaryType == HEAVY_LASER_P && ship_prototypes[shiptype].primaryCount == 1)? "Heavy Laser Cannon" :
		(ship_prototypes[shiptype].primaryType == HEAVY_LASER_P && ship_prototypes[shiptype].primaryCount != 1)? "Heavy Laser Cannons" :
		(ship_prototypes[shiptype].primaryType == TURBOLASER_P && ship_prototypes[shiptype].primaryCount == 1)? "Turbolaser Cannon" :
		(ship_prototypes[shiptype].primaryType == TURBOLASER_P && ship_prototypes[shiptype].primaryCount != 1)? "Turbolaser Cannons" :
		(ship_prototypes[shiptype].primaryType == LIGHT_ION && ship_prototypes[shiptype].primaryCount == 1)? "Light Ion Cannon" :
		(ship_prototypes[shiptype].primaryType == LIGHT_ION && ship_prototypes[shiptype].primaryCount != 1)? "Light Ion Cannons" :
		(ship_prototypes[shiptype].primaryType == REPEATING_ION && ship_prototypes[shiptype].primaryCount == 1)? "Repeating Ion Cannon" :
		(ship_prototypes[shiptype].primaryType == REPEATING_ION && ship_prototypes[shiptype].primaryCount != 1)? "Repeating Ion Cannons" :
		(ship_prototypes[shiptype].primaryType == HEAVY_ION && ship_prototypes[shiptype].primaryCount == 1)? "Heavy Ion Cannon" :
		(ship_prototypes[shiptype].primaryType == HEAVY_ION && ship_prototypes[shiptype].primaryCount != 1)? "Heavy Ion Cannons" :
		(ship_prototypes[shiptype].primaryType == LIGHT_PLASMA && ship_prototypes[shiptype].primaryCount == 1)? "Light Plasma Cannon" :
		(ship_prototypes[shiptype].primaryType == LIGHT_PLASMA && ship_prototypes[shiptype].primaryCount != 1)? "Light Plasma Cannons" :
		(ship_prototypes[shiptype].primaryType == PLASMA && ship_prototypes[shiptype].primaryCount == 1)? "Plasma Cannon" :
		(ship_prototypes[shiptype].primaryType == PLASMA && ship_prototypes[shiptype].primaryCount != 1)? "Plasma Cannons" :
		(ship_prototypes[shiptype].primaryType == HEAVY_PLASMA && ship_prototypes[shiptype].primaryCount == 1)? "Heavy Plasma Cannon" :
		(ship_prototypes[shiptype].primaryType == HEAVY_PLASMA && ship_prototypes[shiptype].primaryCount != 1)? "Heavy Plasma Cannons" :
		(ship_prototypes[shiptype].primaryType == LIGHT_MASS && ship_prototypes[shiptype].primaryCount == 1)? "Light Mass Driver" :
		(ship_prototypes[shiptype].primaryType == LIGHT_MASS && ship_prototypes[shiptype].primaryCount != 1)? "Light Mass Drivers" :
		(ship_prototypes[shiptype].primaryType == MASS && ship_prototypes[shiptype].primaryCount == 1)? "Mass Driver" :
		(ship_prototypes[shiptype].primaryType == MASS && ship_prototypes[shiptype].primaryCount != 1)? "Mass Drivers" :
		(ship_prototypes[shiptype].primaryType == HEAVY_MASS && ship_prototypes[shiptype].primaryCount == 1)? "Heavy Mass Driver" :
		(ship_prototypes[shiptype].primaryType == HEAVY_MASS && ship_prototypes[shiptype].primaryCount != 1)? "Heavy Mass Drivers" :
                (ship_prototypes[shiptype].primaryType == C_MISSILE && ship_prototypes[shiptype].primaryCount == 1)? "Concussion Missile Launcher" :
                (ship_prototypes[shiptype].primaryType == C_MISSILE && ship_prototypes[shiptype].primaryCount != 1)? "Concussion Missile Launchers" :
                (ship_prototypes[shiptype].primaryType == P_TORPEDO && ship_prototypes[shiptype].primaryCount == 1)? "Proton Torpedo Launcher" :
                (ship_prototypes[shiptype].primaryType == P_TORPEDO && ship_prototypes[shiptype].primaryCount != 1)? "Proton Torpedo Launchers" :
                (ship_prototypes[shiptype].primaryType == H_ROCKET && ship_prototypes[shiptype].primaryCount == 1)? "Heavy Rocket Launcher" :
                (ship_prototypes[shiptype].primaryType == H_ROCKET && ship_prototypes[shiptype].primaryCount != 1)? "Heavy Rocket Launchers" :
			"unknown");

		return pbname;
	}
	else
		return "None.";
}
char *secondary_beam_name_proto(int shiptype)
{
    if(ship_prototypes[shiptype].secondaryCount != 0)
	{   
		sprintf(pbname, "%d %s", ship_prototypes[shiptype].secondaryCount,
		(ship_prototypes[shiptype].secondaryType == LIGHT_LASER_NR && ship_prototypes[shiptype].secondaryCount == 1)? "Light Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_LASER_NR && ship_prototypes[shiptype].secondaryCount != 1)? "Light Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == SINGLE_LASER_NR && ship_prototypes[shiptype].secondaryCount == 1)? "Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == SINGLE_LASER_NR && ship_prototypes[shiptype].secondaryCount != 1)? "Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == DUAL_LASER_NR && ship_prototypes[shiptype].secondaryCount == 1)? "Dual Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == DUAL_LASER_NR && ship_prototypes[shiptype].secondaryCount != 1)? "Dual Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == TRI_LASER_NR && ship_prototypes[shiptype].secondaryCount == 1)? "Triple Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == TRI_LASER_NR && ship_prototypes[shiptype].secondaryCount != 1)? "Triple Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == QUAD_LASER_NR && ship_prototypes[shiptype].secondaryCount == 1)? "Quad Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == QUAD_LASER_NR && ship_prototypes[shiptype].secondaryCount != 1)? "Quad Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == AUTOBLASTER_NR && ship_prototypes[shiptype].secondaryCount == 1)? "Repeating Blaster Cannon" :
		(ship_prototypes[shiptype].secondaryType == AUTOBLASTER_NR && ship_prototypes[shiptype].secondaryCount != 1)? "Repeating Blaster Cannons" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_LASER_NR && ship_prototypes[shiptype].secondaryCount == 1)? "Heavy Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_LASER_NR && ship_prototypes[shiptype].secondaryCount != 1)? "Heavy Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == TURBOLASER_NR && ship_prototypes[shiptype].secondaryCount == 1)? "Turbolaser Cannon" :
		(ship_prototypes[shiptype].secondaryType == TURBOLASER_NR && ship_prototypes[shiptype].secondaryCount != 1)? "Turbolaser Cannons" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_LASER_GE && ship_prototypes[shiptype].secondaryCount == 1)? "Light Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_LASER_GE && ship_prototypes[shiptype].secondaryCount != 1)? "Light Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == SINGLE_LASER_GE && ship_prototypes[shiptype].secondaryCount == 1)? "Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == SINGLE_LASER_GE && ship_prototypes[shiptype].secondaryCount != 1)? "Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == DUAL_LASER_GE && ship_prototypes[shiptype].secondaryCount == 1)? "Dual Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == DUAL_LASER_GE && ship_prototypes[shiptype].secondaryCount != 1)? "Dual Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == TRI_LASER_GE && ship_prototypes[shiptype].secondaryCount == 1)? "Triple Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == TRI_LASER_GE && ship_prototypes[shiptype].secondaryCount != 1)? "Triple Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == QUAD_LASER_GE && ship_prototypes[shiptype].secondaryCount == 1)? "Quad Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == QUAD_LASER_GE && ship_prototypes[shiptype].secondaryCount != 1)? "Quad Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == AUTOBLASTER_GE && ship_prototypes[shiptype].secondaryCount == 1)? "Repeating Blaster Cannon" :
		(ship_prototypes[shiptype].secondaryType == AUTOBLASTER_GE && ship_prototypes[shiptype].secondaryCount != 1)? "Repeating Blaster Cannons" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_LASER_GE && ship_prototypes[shiptype].secondaryCount == 1)? "Heavy Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_LASER_GE && ship_prototypes[shiptype].secondaryCount != 1)? "Heavy Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == TURBOLASER_GE && ship_prototypes[shiptype].secondaryCount == 1)? "Turbolaser Cannon" :
		(ship_prototypes[shiptype].secondaryType == TURBOLASER_GE && ship_prototypes[shiptype].secondaryCount != 1)? "Turbolaser Cannons" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_LASER_B && ship_prototypes[shiptype].secondaryCount == 1)? "Light Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_LASER_B && ship_prototypes[shiptype].secondaryCount != 1)? "Light Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == SINGLE_LASER_B && ship_prototypes[shiptype].secondaryCount == 1)? "Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == SINGLE_LASER_B && ship_prototypes[shiptype].secondaryCount != 1)? "Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == DUAL_LASER_B && ship_prototypes[shiptype].secondaryCount == 1)? "Dual Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == DUAL_LASER_B && ship_prototypes[shiptype].secondaryCount != 1)? "Dual Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == TRI_LASER_B && ship_prototypes[shiptype].secondaryCount == 1)? "Triple Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == TRI_LASER_B && ship_prototypes[shiptype].secondaryCount != 1)? "Triple Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == QUAD_LASER_B && ship_prototypes[shiptype].secondaryCount == 1)? "Quad Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == QUAD_LASER_B && ship_prototypes[shiptype].secondaryCount != 1)? "Quad Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == AUTOBLASTER_B && ship_prototypes[shiptype].secondaryCount == 1)? "Repeating Blaster Cannon" :
		(ship_prototypes[shiptype].secondaryType == AUTOBLASTER_B && ship_prototypes[shiptype].secondaryCount != 1)? "Repeating Blaster Cannons" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_LASER_B && ship_prototypes[shiptype].secondaryCount == 1)? "Heavy Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_LASER_B && ship_prototypes[shiptype].secondaryCount != 1)? "Heavy Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == TURBOLASER_B && ship_prototypes[shiptype].secondaryCount == 1)? "Turbolaser Cannon" :
		(ship_prototypes[shiptype].secondaryType == TURBOLASER_B && ship_prototypes[shiptype].secondaryCount != 1)? "Turbolaser Cannons" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_LASER_P && ship_prototypes[shiptype].secondaryCount == 1)? "Light Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_LASER_P && ship_prototypes[shiptype].secondaryCount != 1)? "Light Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == SINGLE_LASER_P && ship_prototypes[shiptype].secondaryCount == 1)? "Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == SINGLE_LASER_P && ship_prototypes[shiptype].secondaryCount != 1)? "Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == DUAL_LASER_P && ship_prototypes[shiptype].secondaryCount == 1)? "Dual Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == DUAL_LASER_P && ship_prototypes[shiptype].secondaryCount != 1)? "Dual Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == TRI_LASER_P && ship_prototypes[shiptype].secondaryCount == 1)? "Triple Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == TRI_LASER_P && ship_prototypes[shiptype].secondaryCount != 1)? "Triple Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == QUAD_LASER_P && ship_prototypes[shiptype].secondaryCount == 1)? "Quad Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == QUAD_LASER_P && ship_prototypes[shiptype].secondaryCount != 1)? "Quad Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == AUTOBLASTER_P && ship_prototypes[shiptype].secondaryCount == 1)? "Repeating Blaster Cannon" :
		(ship_prototypes[shiptype].secondaryType == AUTOBLASTER_P && ship_prototypes[shiptype].secondaryCount != 1)? "Repeating Blaster Cannons" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_LASER_P && ship_prototypes[shiptype].secondaryCount == 1)? "Heavy Laser Cannon" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_LASER_P && ship_prototypes[shiptype].secondaryCount != 1)? "Heavy Laser Cannons" :
		(ship_prototypes[shiptype].secondaryType == TURBOLASER_P && ship_prototypes[shiptype].secondaryCount == 1)? "Turbolaser Cannon" :
		(ship_prototypes[shiptype].secondaryType == TURBOLASER_P && ship_prototypes[shiptype].secondaryCount != 1)? "Turbolaser Cannons" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_ION && ship_prototypes[shiptype].secondaryCount == 1)? "Light Ion Cannon" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_ION && ship_prototypes[shiptype].secondaryCount != 1)? "Light Ion Cannons" :
		(ship_prototypes[shiptype].secondaryType == REPEATING_ION && ship_prototypes[shiptype].secondaryCount == 1)? "Repeating Ion Cannon" :
		(ship_prototypes[shiptype].secondaryType == REPEATING_ION && ship_prototypes[shiptype].secondaryCount != 1)? "Repeating Ion Cannons" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_ION && ship_prototypes[shiptype].secondaryCount == 1)? "Heavy Ion Cannon" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_ION && ship_prototypes[shiptype].secondaryCount != 1)? "Heavy Ion Cannons" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_PLASMA && ship_prototypes[shiptype].secondaryCount == 1)? "Light Plasma Cannon" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_PLASMA && ship_prototypes[shiptype].secondaryCount != 1)? "Light Plasma Cannons" :
		(ship_prototypes[shiptype].secondaryType == PLASMA && ship_prototypes[shiptype].secondaryCount == 1)? "Plasma Cannon" :
		(ship_prototypes[shiptype].secondaryType == PLASMA && ship_prototypes[shiptype].secondaryCount != 1)? "Plasma Cannons" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_PLASMA && ship_prototypes[shiptype].secondaryCount == 1)? "Heavy Plasma Cannon" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_PLASMA && ship_prototypes[shiptype].secondaryCount != 1)? "Heavy Plasma Cannons" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_MASS && ship_prototypes[shiptype].secondaryCount == 1)? "Light Mass Driver" :
		(ship_prototypes[shiptype].secondaryType == LIGHT_MASS && ship_prototypes[shiptype].secondaryCount != 1)? "Light Mass Drivers" :
		(ship_prototypes[shiptype].secondaryType == MASS && ship_prototypes[shiptype].secondaryCount == 1)? "Mass Driver" :
		(ship_prototypes[shiptype].secondaryType == MASS && ship_prototypes[shiptype].secondaryCount != 1)? "Mass Drivers" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_MASS && ship_prototypes[shiptype].secondaryCount == 1)? "Heavy Mass Driver" :
		(ship_prototypes[shiptype].secondaryType == HEAVY_MASS && ship_prototypes[shiptype].secondaryCount != 1)? "Heavy Mass Drivers" :
                (ship_prototypes[shiptype].secondaryType == C_MISSILE && ship_prototypes[shiptype].secondaryCount == 1)? "Concussion Missile Launcher" :
                (ship_prototypes[shiptype].secondaryType == C_MISSILE && ship_prototypes[shiptype].secondaryCount != 1)? "Concussion Missile Launchers" :
                (ship_prototypes[shiptype].secondaryType == P_TORPEDO && ship_prototypes[shiptype].secondaryCount == 1)? "Proton Torpedo Launcher" :
                (ship_prototypes[shiptype].secondaryType == P_TORPEDO && ship_prototypes[shiptype].secondaryCount != 1)? "Proton Torpedo Launchers" :
                (ship_prototypes[shiptype].secondaryType == H_ROCKET && ship_prototypes[shiptype].secondaryCount == 1)? "Heavy Rocket Launcher" :
                (ship_prototypes[shiptype].secondaryType == H_ROCKET && ship_prototypes[shiptype].secondaryCount != 1)? "Heavy Rocket Launchers" :
			"unknown");

		return pbname;
	}
	else
		return "None.";
}
char *tertiary_beam_name_proto(int shiptype)
{
    if(ship_prototypes[shiptype].tertiaryCount != 0)
	{   
		sprintf(pbname, "%d %s", ship_prototypes[shiptype].tertiaryCount,
		(ship_prototypes[shiptype].tertiaryType == LIGHT_LASER_NR && ship_prototypes[shiptype].tertiaryCount == 1)? "Light Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_LASER_NR && ship_prototypes[shiptype].tertiaryCount != 1)? "Light Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == SINGLE_LASER_NR && ship_prototypes[shiptype].tertiaryCount == 1)? "Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == SINGLE_LASER_NR && ship_prototypes[shiptype].tertiaryCount != 1)? "Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == DUAL_LASER_NR && ship_prototypes[shiptype].tertiaryCount == 1)? "Dual Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == DUAL_LASER_NR && ship_prototypes[shiptype].tertiaryCount != 1)? "Dual Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == TRI_LASER_NR && ship_prototypes[shiptype].tertiaryCount == 1)? "Triple Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == TRI_LASER_NR && ship_prototypes[shiptype].tertiaryCount != 1)? "Triple Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == QUAD_LASER_NR && ship_prototypes[shiptype].tertiaryCount == 1)? "Quad Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == QUAD_LASER_NR && ship_prototypes[shiptype].tertiaryCount != 1)? "Quad Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == AUTOBLASTER_NR && ship_prototypes[shiptype].tertiaryCount == 1)? "Repeating Blaster Cannon" :
		(ship_prototypes[shiptype].tertiaryType == AUTOBLASTER_NR && ship_prototypes[shiptype].tertiaryCount != 1)? "Repeating Blaster Cannons" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_LASER_NR && ship_prototypes[shiptype].tertiaryCount == 1)? "Heavy Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_LASER_NR && ship_prototypes[shiptype].tertiaryCount != 1)? "Heavy Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == TURBOLASER_NR && ship_prototypes[shiptype].tertiaryCount == 1)? "Turbolaser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == TURBOLASER_NR && ship_prototypes[shiptype].tertiaryCount != 1)? "Turbolaser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_LASER_GE && ship_prototypes[shiptype].tertiaryCount == 1)? "Light Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_LASER_GE && ship_prototypes[shiptype].tertiaryCount != 1)? "Light Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == SINGLE_LASER_GE && ship_prototypes[shiptype].tertiaryCount == 1)? "Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == SINGLE_LASER_GE && ship_prototypes[shiptype].tertiaryCount != 1)? "Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == DUAL_LASER_GE && ship_prototypes[shiptype].tertiaryCount == 1)? "Dual Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == DUAL_LASER_GE && ship_prototypes[shiptype].tertiaryCount != 1)? "Dual Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == TRI_LASER_GE && ship_prototypes[shiptype].tertiaryCount == 1)? "Triple Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == TRI_LASER_GE && ship_prototypes[shiptype].tertiaryCount != 1)? "Triple Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == QUAD_LASER_GE && ship_prototypes[shiptype].tertiaryCount == 1)? "Quad Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == QUAD_LASER_GE && ship_prototypes[shiptype].tertiaryCount != 1)? "Quad Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == AUTOBLASTER_GE && ship_prototypes[shiptype].tertiaryCount == 1)? "Repeating Blaster Cannon" :
		(ship_prototypes[shiptype].tertiaryType == AUTOBLASTER_GE && ship_prototypes[shiptype].tertiaryCount != 1)? "Repeating Blaster Cannons" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_LASER_GE && ship_prototypes[shiptype].tertiaryCount == 1)? "Heavy Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_LASER_GE && ship_prototypes[shiptype].tertiaryCount != 1)? "Heavy Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == TURBOLASER_GE && ship_prototypes[shiptype].tertiaryCount == 1)? "Turbolaser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == TURBOLASER_GE && ship_prototypes[shiptype].tertiaryCount != 1)? "Turbolaser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_LASER_B && ship_prototypes[shiptype].tertiaryCount == 1)? "Light Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_LASER_B && ship_prototypes[shiptype].tertiaryCount != 1)? "Light Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == SINGLE_LASER_B && ship_prototypes[shiptype].tertiaryCount == 1)? "Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == SINGLE_LASER_B && ship_prototypes[shiptype].tertiaryCount != 1)? "Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == DUAL_LASER_B && ship_prototypes[shiptype].tertiaryCount == 1)? "Dual Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == DUAL_LASER_B && ship_prototypes[shiptype].tertiaryCount != 1)? "Dual Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == TRI_LASER_B && ship_prototypes[shiptype].tertiaryCount == 1)? "Triple Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == TRI_LASER_B && ship_prototypes[shiptype].tertiaryCount != 1)? "Triple Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == QUAD_LASER_B && ship_prototypes[shiptype].tertiaryCount == 1)? "Quad Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == QUAD_LASER_B && ship_prototypes[shiptype].tertiaryCount != 1)? "Quad Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == AUTOBLASTER_B && ship_prototypes[shiptype].tertiaryCount == 1)? "Repeating Blaster Cannon" :
		(ship_prototypes[shiptype].tertiaryType == AUTOBLASTER_B && ship_prototypes[shiptype].tertiaryCount != 1)? "Repeating Blaster Cannons" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_LASER_B && ship_prototypes[shiptype].tertiaryCount == 1)? "Heavy Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_LASER_B && ship_prototypes[shiptype].tertiaryCount != 1)? "Heavy Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == TURBOLASER_B && ship_prototypes[shiptype].tertiaryCount == 1)? "Turbolaser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == TURBOLASER_B && ship_prototypes[shiptype].tertiaryCount != 1)? "Turbolaser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_LASER_P && ship_prototypes[shiptype].tertiaryCount == 1)? "Light Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_LASER_P && ship_prototypes[shiptype].tertiaryCount != 1)? "Light Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == SINGLE_LASER_P && ship_prototypes[shiptype].tertiaryCount == 1)? "Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == SINGLE_LASER_P && ship_prototypes[shiptype].tertiaryCount != 1)? "Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == DUAL_LASER_P && ship_prototypes[shiptype].tertiaryCount == 1)? "Dual Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == DUAL_LASER_P && ship_prototypes[shiptype].tertiaryCount != 1)? "Dual Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == TRI_LASER_P && ship_prototypes[shiptype].tertiaryCount == 1)? "Triple Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == TRI_LASER_P && ship_prototypes[shiptype].tertiaryCount != 1)? "Triple Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == QUAD_LASER_P && ship_prototypes[shiptype].tertiaryCount == 1)? "Quad Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == QUAD_LASER_P && ship_prototypes[shiptype].tertiaryCount != 1)? "Quad Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == AUTOBLASTER_P && ship_prototypes[shiptype].tertiaryCount == 1)? "Repeating Blaster Cannon" :
		(ship_prototypes[shiptype].tertiaryType == AUTOBLASTER_P && ship_prototypes[shiptype].tertiaryCount != 1)? "Repeating Blaster Cannons" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_LASER_P && ship_prototypes[shiptype].tertiaryCount == 1)? "Heavy Laser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_LASER_P && ship_prototypes[shiptype].tertiaryCount != 1)? "Heavy Laser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == TURBOLASER_P && ship_prototypes[shiptype].tertiaryCount == 1)? "Turbolaser Cannon" :
		(ship_prototypes[shiptype].tertiaryType == TURBOLASER_P && ship_prototypes[shiptype].tertiaryCount != 1)? "Turbolaser Cannons" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_ION && ship_prototypes[shiptype].tertiaryCount == 1)? "Light Ion Cannon" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_ION && ship_prototypes[shiptype].tertiaryCount != 1)? "Light Ion Cannons" :
		(ship_prototypes[shiptype].tertiaryType == REPEATING_ION && ship_prototypes[shiptype].tertiaryCount == 1)? "Repeating Ion Cannon" :
		(ship_prototypes[shiptype].tertiaryType == REPEATING_ION && ship_prototypes[shiptype].tertiaryCount != 1)? "Repeating Ion Cannons" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_ION && ship_prototypes[shiptype].tertiaryCount == 1)? "Heavy Ion Cannon" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_ION && ship_prototypes[shiptype].tertiaryCount != 1)? "Heavy Ion Cannons" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_PLASMA && ship_prototypes[shiptype].tertiaryCount == 1)? "Light Plasma Cannon" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_PLASMA && ship_prototypes[shiptype].tertiaryCount != 1)? "Light Plasma Cannons" :
		(ship_prototypes[shiptype].tertiaryType == PLASMA && ship_prototypes[shiptype].tertiaryCount == 1)? "Plasma Cannon" :
		(ship_prototypes[shiptype].tertiaryType == PLASMA && ship_prototypes[shiptype].tertiaryCount != 1)? "Plasma Cannons" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_PLASMA && ship_prototypes[shiptype].tertiaryCount == 1)? "Heavy Plasma Cannon" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_PLASMA && ship_prototypes[shiptype].tertiaryCount != 1)? "Heavy Plasma Cannons" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_MASS && ship_prototypes[shiptype].tertiaryCount == 1)? "Light Mass Driver" :
		(ship_prototypes[shiptype].tertiaryType == LIGHT_MASS && ship_prototypes[shiptype].tertiaryCount != 1)? "Light Mass Drivers" :
		(ship_prototypes[shiptype].tertiaryType == MASS && ship_prototypes[shiptype].tertiaryCount == 1)? "Mass Driver" :
		(ship_prototypes[shiptype].tertiaryType == MASS && ship_prototypes[shiptype].tertiaryCount != 1)? "Mass Drivers" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_MASS && ship_prototypes[shiptype].tertiaryCount == 1)? "Heavy Mass Driver" :
		(ship_prototypes[shiptype].tertiaryType == HEAVY_MASS && ship_prototypes[shiptype].tertiaryCount != 1)? "Heavy Mass Drivers" :
                (ship_prototypes[shiptype].tertiaryType == C_MISSILE && ship_prototypes[shiptype].tertiaryCount == 1)? "Concussion Missile Launcher" :
                (ship_prototypes[shiptype].tertiaryType == C_MISSILE && ship_prototypes[shiptype].tertiaryCount != 1)? "Concussion Missile Launchers" :
                (ship_prototypes[shiptype].tertiaryType == P_TORPEDO && ship_prototypes[shiptype].tertiaryCount == 1)? "Proton Torpedo Launcher" :
                (ship_prototypes[shiptype].tertiaryType == P_TORPEDO && ship_prototypes[shiptype].tertiaryCount != 1)? "Proton Torpedo Launchers" :
                (ship_prototypes[shiptype].tertiaryType == H_ROCKET && ship_prototypes[shiptype].tertiaryCount == 1)? "Heavy Rocket Launcher" :
                (ship_prototypes[shiptype].tertiaryType == H_ROCKET && ship_prototypes[shiptype].tertiaryCount != 1)? "Heavy Rocket Launchers" :
			"unknown");

		return pbname;
	}
	else
		return "None.";
}
void do_shipstat( CHAR_DATA *ch, char *argument )
{
 int shiptype;
 char buf1[MAX_STRING_LENGTH];
 char buf2[MAX_STRING_LENGTH];
 char buf3[MAX_STRING_LENGTH];
 char buf4[MAX_STRING_LENGTH];
 char buf5[MAX_STRING_LENGTH];
 char buf6[MAX_STRING_LENGTH];
 char buf7[MAX_STRING_LENGTH];
 char buf8[MAX_STRING_LENGTH];
 char buf9[MAX_STRING_LENGTH];
 char buf10[MAX_STRING_LENGTH];
 char buf11[MAX_STRING_LENGTH];
 char buf12[MAX_STRING_LENGTH];
 if ( IS_NPC( ch ) )
 {
  send_to_char( "Huh?\n\r", ch );
  return;
 }
 if ( argument[0] == '\0' )
 {
  send_to_char( "Usage: shipstat <ship number>\n\r", ch );
  return;
 }
 shiptype = atoi(argument)-1;
 if(shiptype < 0 || shiptype > NUM_PROTOTYPES-1)
 {
  send_to_char("Invalid number.\n\r", ch);
  return;
 }

 if(ship_prototypes[shiptype].primaryType > 0) sprintf(buf1, "%d", ship_prototypes[shiptype].primaryType);
  else sprintf(buf1, "&RNone.");
 if(ship_prototypes[shiptype].secondaryType > 0) sprintf(buf2, "%d", ship_prototypes[shiptype].secondaryType);
  else sprintf(buf2, "&RNone.");
 if(ship_prototypes[shiptype].tertiaryType > 0) sprintf(buf2, "%d", ship_prototypes[shiptype].tertiaryType);
  else sprintf(buf2, "&RNone.");
 if(ship_prototypes[shiptype].range_weapons[0] > 0) sprintf(buf3, "%d", ship_prototypes[shiptype].range_weapons[0]);
  else sprintf(buf3, "&RNone.");
 if(ship_prototypes[shiptype].shields > 0) sprintf(buf4, "%d", ship_prototypes[shiptype].shields);
  else sprintf(buf4, "&RNone.");
 if(ship_prototypes[shiptype].range_weapons[1] > 0) sprintf(buf5, "%d", ship_prototypes[shiptype].range_weapons[1]);
  else sprintf(buf5, "&RNone.");
 if(ship_prototypes[shiptype].hyperspeed > 0) sprintf(buf6, "Class %d", ship_prototypes[shiptype].hyperspeed);
  else sprintf(buf6, "&RNone.");
 if(ship_prototypes[shiptype].range_weapons[2] > 0) sprintf(buf7, "%d", ship_prototypes[shiptype].range_weapons[2]);
  else sprintf(buf7, "&RNone.");
 if(ship_prototypes[shiptype].maxbombs > 0) sprintf(buf8, "%d", ship_prototypes[shiptype].maxbombs);
  else sprintf(buf8, "&RNone.");
 if(ship_prototypes[shiptype].turrets > 0) sprintf(buf9, "%d", ship_prototypes[shiptype].turrets);
  else sprintf(buf9, "&RNone.");
 if(ship_prototypes[shiptype].countermeasures > 0) sprintf(buf10, "%d", ship_prototypes[shiptype].countermeasures);
  else sprintf(buf10, "&RNone.");
 if(ship_prototypes[shiptype].windows > 0) sprintf(buf9, "%d", ship_prototypes[shiptype].windows);
  else sprintf(buf11, "&RNone.");
   if( ship_prototypes[shiptype].gwell > 0 )         sprintf( buf12, "%d", ship_prototypes[shiptype].gwell );
    else
       sprintf( buf12, "&RNone." );
  
   ch_printf( ch, "&R&z+&W---------------------------------------------------------------&z+\r\n" );
   ch_printf( ch, "&z|&W  &cName&B: &w%-35.35s     &cCost&B: &w%8d &W|\r\n", ship_prototypes[shiptype].name,               ship_prototypes[shiptype].cost );
    ch_printf( ch, "&z+&W---------------------------------------------------------------&z+\r\n" );
    ch_printf( ch, "&z|&W  &cProfile&B:&W                                                     &z|\r\n" );
    ch_printf( ch, "&z|&W    &gHull:&w %-5d&W   &gShields:&w %-5s         &gEnergy:&w %-6d        &z|\r\n", ship_prototypes[shiptype].hull, buf4, ship_prototypes[shiptype].energy );
    ch_printf( ch, "&z|&W    &gSpeed:&w %-5d&W  &gManeuverability:&w %-5d &gHyperdrive:&w %-5s   &z|\r\n",  ship_prototypes[shiptype].speed, ship_prototypes[shiptype].manuever, buf6 );
    ch_printf( ch, "&z+&W---------------------------------------------------------------&z+\r\n" );    ch_printf( ch, "&z|&W  &cArmaments&B:&W                                                   &z|\r\n" );
    ch_printf( ch, "&z|&W    &gPrimary Weapon System:&w   %-30.30s    &z|\r\n", primary_beam_name_proto( shiptype ) );
    ch_printf( ch, "&z|&W    &gSecondary Weapon System:&w %-30.30s    &z|\r\n", secondary_beam_name_proto( shiptype ) );
    ch_printf( ch, "&z|&W    &gTertiary Weapon System:&w  %-30.30s    &z|\r\n", tertiary_beam_name_proto( shiptype ) );
    ch_printf( ch, "&z+&W---------------------------------------------------------------&z+\r\n" );
    ch_printf( ch, "&z|&W  &cOrdinance&B:&W                                                   &z|\r\n" );
    ch_printf( ch, "&z|&W    &gMissiles:&w %-5s&W   &gTorpedos:&w %-5s&W   &gRockets:&w %-5s         &z|\r\n", buf3, buf5, buf7 );
    ch_printf( ch, "&z|&W       &gPlanetary bombs:&w %-5s&W       &gCountermeasures:&w %-5s     &z|\r\n", buf8, buf10 );
    ch_printf( ch, "&z+&W---------------------------------------------------------------&z+\r\n" );
    ch_printf( ch, "&z|&W  &cMisc&B:&W                                                        &z|\r\n" );
    if( ship_prototypes[shiptype].turrets > 0 )    ch_printf( ch, "&z|&W    &gTurrets:&w %-5s                                             &z|\r\n", buf9  );
    if( ship_prototypes[shiptype].windows > 0 )    ch_printf( ch, "&z|&W    &gWindows:&w %-5s                                             &z|\r\n", buf11 );
    if( ship_prototypes[shiptype].hangar1space > 0 )    ch_printf( ch, "&z|&W    &gHangar 1 Capacity:&w %-5d  	                                &z|\n\r", ship_prototypes[shiptype].hangar1space);
    if( ship_prototypes[shiptype].hangar2space > 0 )    ch_printf( ch, "&z|&W    &gHangar 2 Capacity:&w %-5d  	                                &z|\n\r", ship_prototypes[shiptype].hangar2space);
    if( ship_prototypes[shiptype].hangar3space > 0 )    ch_printf( ch, "&z|&W    &gHangar 3 Capacity:&w %-5d  	                                &z|\n\r", ship_prototypes[shiptype].hangar3space);
    if( ship_prototypes[shiptype].hangar4space > 0 )    ch_printf( ch, "&z|&W    &gHangar 4 Capacity:&w %-5d  	                                &z|\n\r", ship_prototypes[shiptype].hangar4space);
    if( ship_prototypes[shiptype].gwell > 0 )    ch_printf( ch, "&z|&W    &gGravity Well Generators:&w %-5d                                 &z|\r\n", ship_prototypes[shiptype].gwell );
 #ifdef USECARGO
    if( ship_prototypes[shiptype].maxcargo > 0 )    ch_printf( ch, "&z|&W    &gMaxCargo:&w %-5d                                            &z|\n\r", ship_prototypes[shiptype].maxcargo );
 #endif
    ch_printf( ch, "&R&z+&W---------------------------------------------------------------&z+\r\n" );
    return;
 }

void load_market_list()
{
    FILE *fpList;
    char *filename;
    char list[256];
    BMARKET_DATA *marketship;
    int	quantity;
     
    first_market_ship = NULL;
    last_market_ship = NULL;

    log_string( "Loading black market." );

    sprintf( list, "%s%s", SHIP_PROTOTYPE_DIR, "blackmarket.lst" );
    fclose( fpReserve );
    if ( ( fpList = fopen( list, "r" ) ) == NULL )
    {
	perror( list );
	exit( 1 );
    }

    for ( ; ; )
    {
        filename = feof( fpList ) ? "$" : fread_word( fpList );
        if ( filename[0] == '$' )
        break;                                  
	CREATE( marketship, BMARKET_DATA, 1 );
        LINK( marketship, first_market_ship, last_market_ship, next, prev );
	marketship->filename = STRALLOC(filename);
	quantity = fread_number( fpList );
	marketship->quantity = quantity;
    }
    fclose( fpList );
    log_string("Finished loading blackmarket." );
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}

void save_market_list()
{    
    BMARKET_DATA *marketship;
    FILE *fpout;
    char filename[256];
    
    sprintf( filename, "%s%s", SHIP_PROTOTYPE_DIR, "blackmarket.lst" );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
         bug( "FATAL: cannot open blackmarket.lst for writing.\n\r", 0 );
         return;
    }
    for ( marketship = first_market_ship; marketship; marketship= marketship->next )
    {
        fprintf( fpout, "%s\n", marketship->filename );
        fprintf( fpout, "%d\n", marketship->quantity );
    }
    fprintf( fpout, "$\n" );
    fclose( fpout );
}

void add_market_ship( SHIP_DATA *ship )
{
    BMARKET_DATA *marketship;
    bool found;

    if(!ship)  return;
    
    for ( marketship = first_market_ship; marketship; marketship = marketship->next )
    {
    	if ( !str_cmp( marketship->filename , ship->protoname ))
    	{
    		//Debugging  bug("Found, adding quantity", 0);
    		found = TRUE;
    		marketship->quantity++;
    		return;
    	}
    }        
    

    	//Debugging  bug("Not found, adding to .lst", 0);
        CREATE( marketship, BMARKET_DATA, 1 );
        LINK( marketship, first_market_ship, last_market_ship, next, prev );
    
        marketship->filename      = STRALLOC( ship->protoname );
        marketship->quantity      = 1;
        
    save_market_list();
}

void remove_market_ship( BMARKET_DATA *marketship )
{
    if(marketship->quantity > 1)
     marketship->quantity--;
    else
    {
	UNLINK( marketship, first_market_ship, last_market_ship, next, prev );
	STRFREE( marketship->filename );
	DISPOSE( marketship );
    }
    
    save_market_list();
}

void make_random_marketlist()
{
// BMARKET_DATA *marketship;
 BMARKET_DATA *nmarketship;
 FILE *fpout;
 char filename[256];
 int x,priority,count;
 
 // Clear previously loaded data *** Change of plans, delete file and rebuild from scratch
 // *** db.c does not load from file anymore
/* for( marketship = first_market_ship; marketship; marketship = marketship->next )
 {
	UNLINK( marketship, first_market_ship, last_market_ship, next, prev );
	STRFREE( marketship->filename );
	DISPOSE( marketship );
 }*/
 
 sprintf( filename, "%s%s", SHIP_PROTOTYPE_DIR, "blackmarket.lst" );
 fpout = fopen(filename, "w");
 fprintf( fpout, "$\n" );
 fclose( fpout );
 	
 // Make a new list
 count = 0;
while(1)
{
 for(x=0;x<NUM_PROTOTYPES;x++)
 {
  
  if(str_cmp(ship_prototypes[x].clan,"Neutral") && str_cmp(ship_prototypes[x].clan,"The Empire") && str_cmp(ship_prototypes[x].clan,"The New Republic"))
   continue;
  if(count == 1)
  {
   if(number_range(1, 10) < 2)
   {
   	save_market_list();
   	return;
   }
  }
  if(count == 2)
  {
   if(number_range(1, 10) < 3)
   {
   	save_market_list();
   	return;
   }
  }
  if(count == 3)
  {
   if(number_range(1, 10) < 4)
   {
  	save_market_list();
  	return;
   }
  }
  if(count == 4)
  {
   if(number_range(1, 10) < 5)
   {
   
        save_market_list();
  	return;
	
   }
  } 
  if(count == 5)
  {
       
        save_market_list();
  	return;
	
   }
  
    
  priority = 0;
  if(!str_cmp(ship_prototypes[x].sname,"JV-7-NRM"))	priority = 20;
  if(!str_cmp(ship_prototypes[x].sname,"CB-H17"))	priority = 1;
  if(!str_cmp(ship_prototypes[x].sname,"CF-101"))	priority = 2;
  if(!str_cmp(ship_prototypes[x].sname,"CI-V7"))	priority = 3;
  if(!str_cmp(ship_prototypes[x].sname,"CS-411"))	priority = 4;
  if(!str_cmp(ship_prototypes[x].sname,"JV-7-TEM"))	priority = 19;
  if(!str_cmp(ship_prototypes[x].sname,"T-4A-NRM"))	priority = 18;
  if(!str_cmp(ship_prototypes[x].sname,"T-4A-TEM"))	priority = 17;
  if(!str_cmp(ship_prototypes[x].sname,"T-6A-NRM"))	priority = 16;
  if(!str_cmp(ship_prototypes[x].sname,"T-6A-TEM"))	priority = 15;
  if(!str_cmp(ship_prototypes[x].sname,"TIE-I"))	priority = 14;
  if(!str_cmp(ship_prototypes[x].sname,"TIE-D"))	priority = 10;
  if(!str_cmp(ship_prototypes[x].sname,"TIE-SA"))	priority = 13;
  if(!str_cmp(ship_prototypes[x].sname,"XM-1"))		priority = 12;
  if(!str_cmp(ship_prototypes[x].sname,"XG-1"))		priority = 11;
  if(!str_cmp(ship_prototypes[x].sname,"X-Wing-XJ"))	priority = 5;
  if(!str_cmp(ship_prototypes[x].sname,"X-Wing-BR"))	priority = 6;
  if(!str_cmp(ship_prototypes[x].sname,"B-Wing"))	priority = 9;
  if(!str_cmp(ship_prototypes[x].sname,"A-Wing-Z3"))	priority = 7;
  if(!str_cmp(ship_prototypes[x].sname,"K-Wing"))	priority = 8;
  
  
  if(priority != 0)
  {
   if(priority == 1 || priority == 2)
   {
    if(number_range(1, 25) >= 24)
    {

   	 	CREATE(nmarketship, BMARKET_DATA, 1);
   		LINK(nmarketship, first_market_ship, last_market_ship, next, prev);
   	
   		nmarketship->filename = STRALLOC(ship_prototypes[x].sname);
   		nmarketship->quantity = 1;
   		count++;

    }
   }


   if(priority == 3 || priority == 4)
   {
    if(number_range(1, 25) >= 23)
    {

   	 	CREATE(nmarketship, BMARKET_DATA, 1);
   		LINK(nmarketship, first_market_ship, last_market_ship, next, prev);
   	
   		nmarketship->filename = STRALLOC(ship_prototypes[x].sname);
   		nmarketship->quantity = 1;
   		count++;

    }
   }

   if(priority == 5 || priority == 6)
   {
    if(number_range(1, 25) >= 22)
    {

   	 	CREATE(nmarketship, BMARKET_DATA, 1);
   		LINK(nmarketship, first_market_ship, last_market_ship, next, prev);
   	
   		nmarketship->filename = STRALLOC(ship_prototypes[x].sname);
   		nmarketship->quantity = 1;
   		count++;

    }
   }

   if(priority == 7 || priority == 8)
   {
    if(number_range(1, 25) >= 21)
    {

   	 	CREATE(nmarketship, BMARKET_DATA, 1);
   		LINK(nmarketship, first_market_ship, last_market_ship, next, prev);
   	
   		nmarketship->filename = STRALLOC(ship_prototypes[x].sname);
   		nmarketship->quantity = 1;
   		count++;

    }
   }

   if(priority == 9 || priority == 10)
   {
    if(number_range(1, 25) >= 20)
    {

   	 	CREATE(nmarketship, BMARKET_DATA, 1);
   		LINK(nmarketship, first_market_ship, last_market_ship, next, prev);
   	
   		nmarketship->filename = STRALLOC(ship_prototypes[x].sname);
   		nmarketship->quantity = 1;
   		count++;

    }
   }

   if(priority == 11 || priority == 12)
   {
    if(number_range(1, 25) >= 19)
    {

   	 	CREATE(nmarketship, BMARKET_DATA, 1);
   		LINK(nmarketship, first_market_ship, last_market_ship, next, prev);
   	
   		nmarketship->filename = STRALLOC(ship_prototypes[x].sname);
   		nmarketship->quantity = 1;
   		count++;

    }
   }

   if(priority == 13 || priority == 14)
   {
    if(number_range(1, 25) >= 18)
    {

   	 	CREATE(nmarketship, BMARKET_DATA, 1);
   		LINK(nmarketship, first_market_ship, last_market_ship, next, prev);
   	
   		nmarketship->filename = STRALLOC(ship_prototypes[x].sname);
   		nmarketship->quantity = 1;
   		count++;

    }
   }

   if(priority == 15 || priority == 16)
   {
    if(number_range(1, 25) >= 17)
    {

   	 	CREATE(nmarketship, BMARKET_DATA, 1);
   		LINK(nmarketship, first_market_ship, last_market_ship, next, prev);
   	
   		nmarketship->filename = STRALLOC(ship_prototypes[x].sname);
   		nmarketship->quantity = 1;
   		count++;

    }
   }

   if(priority == 17 || priority == 18)
   {
    if(number_range(1, 25) >= 16)
    {

   	 	CREATE(nmarketship, BMARKET_DATA, 1);
   		LINK(nmarketship, first_market_ship, last_market_ship, next, prev);
   	
   		nmarketship->filename = STRALLOC(ship_prototypes[x].sname);
   		nmarketship->quantity = 1;
   		count++;

    }
   }

   if(priority == 19 || priority == 20)
   {
    if(number_range(1, 25) >= 15)
    {

   	 	CREATE(nmarketship, BMARKET_DATA, 1);
   		LINK(nmarketship, first_market_ship, last_market_ship, next, prev);
   	
   		nmarketship->filename = STRALLOC(ship_prototypes[x].sname);
   		nmarketship->quantity = 1;
   		count++;

    }
   }
  }
 }
}
save_market_list();
}

void do_generate_market(CHAR_DATA *ch, char *argument)
{
 BMARKET_DATA *marketship;


 if(IS_NPC(ch))  return;

 if(!first_market_ship)
 {
   bug("Error 0.");
   return;
 }

 marketship = first_market_ship;
 if(!(marketship))
 {
   bug("Error 2.");
   return;
 }


 // Clear list
 for(marketship = first_market_ship; marketship; marketship = marketship->next)  
 {
 	UNLINK( marketship, first_market_ship, last_market_ship, next, prev );
	STRFREE( marketship->filename );
// Pretty sure you don't need this, especially after its been unlinked.
//	DISPOSE( marketship );
 }
 // Generate new
 make_random_marketlist();
 return;
}



void do_installmodule( CHAR_DATA *ch, char *argument )
{
  SHIP_DATA *ship;
  MODULE_DATA *mod;
  int i=0, maxmod;
  bool checktool,checkmod;
  OBJ_DATA *obj;
  OBJ_DATA *modobj;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  int x;
  int  chance;
//Max slots per mod.
  int maxslot = 2;
//Damned Slots.
  int primary=0;
  int secondary=0;
  int missile=0;
  int rocket=0;
  int torpedo=0;
  int hull=0;
  int shields=0;
  int speed=0;
  int hyperspeed=0;
  int energy=0;
  int manuever=0;
  int alarm=0;
  int countermeasures=0;
  int slave=0;
  int tractor=0;
  int tertiary=0;
  int bombs=0;
  int maxcargo=0;

  strcpy( arg , argument );
  checktool = FALSE;
  checkmod = FALSE;
  switch( ch->substate )
   {
    default:
    if ( (ship = ship_from_engine(ch->in_room->vnum)) != NULL )
      {
      	ship = ship_from_engine(ch->in_room->vnum);
      	strcpy(arg, ship->name);
      }

     if (!ship)
      {
    	send_to_char("You need to be in the ships engine room.\n\r", ch);
    	return;
      }

     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
      	if (obj->item_type == ITEM_TOOLKIT)
      	  checktool = TRUE;
      	if (obj->item_type == ITEM_MODULE)
	{
      	  checkmod = TRUE;
	  modobj = obj;
	}
      }

     if ( checktool == FALSE )
      {
      	send_to_char("You need a toolkit to install a module.\n\r", ch);
      	return;
      }
     if ( checkmod == FALSE )
      {
      	send_to_char("You need a module to install!\n\r", ch);
      	return;
      }

  for(i=0,mod=ship->first_module;mod;mod=mod->next)
  {
      ++i;
      if(mod->affect == AFFECT_PRIMARY)
        ++primary;
      if(mod->affect == AFFECT_SECONDARY)
        ++secondary;
      if(mod->affect == AFFECT_MISSILE)
        ++missile;      
      if(mod->affect == AFFECT_ROCKET)
        ++rocket;
      if(mod->affect == AFFECT_TORPEDO)
        ++torpedo;
      if(mod->affect == AFFECT_HULL)
        ++hull;
      if(mod->affect == AFFECT_SHIELD)
        ++shields;
      if(mod->affect == AFFECT_SPEED)
        ++speed;
      if(mod->affect == AFFECT_HYPER)
        ++hyperspeed;
      if(mod->affect == AFFECT_ENERGY)
        ++energy;
      if(mod->affect == AFFECT_MANUEVER)
        ++manuever;
      if(mod->affect == AFFECT_ALARM)
        ++alarm;
      if(mod->affect == AFFECT_COUNTERMEASURES)
        ++countermeasures;
      if(mod->affect == AFFECT_SLAVE)
	++slave;
      if(mod->affect == AFFECT_TRACTOR)
        ++tractor;
      if(mod->affect == AFFECT_TERTIARY)
        ++tertiary;
      if(mod->affect == AFFECT_BOMB)
        ++bombs;
      if(mod->affect == AFFECT_CARGO)
        ++maxcargo;
  }

//Holy ifchecks batman!
  if (modobj->value[0] == AFFECT_ALARM && (ship->alarm == 1))
 {
    send_to_char("This ship already has an alarm system!\n\r", ch);
    return;
 }
  if (modobj->value[0] == AFFECT_HYPER && (ship->hyperspeed == 1))
 {
    send_to_char("This ship already has a first class hyperspeed drive.\n\r", ch);
    return;
 }
 if (modobj->value[0] == AFFECT_SLAVE && (ship->slave > 0))
 {
    send_to_char("This ship already has an slave circuit!\n\r", ch);
    return;
 }
 if (modobj->value[0] == AFFECT_TRACTOR && (ship->tractorbeam > 0))
 {
    send_to_char("This ship already has a tractor beam!\n\r", ch);
    return;
 }
  if((modobj->value[0] == AFFECT_PRIMARY && primary >= maxslot) ||
      (modobj->value[0] == AFFECT_SECONDARY && secondary >= maxslot) ||
      (modobj->value[0] == AFFECT_MISSILE && missile >= maxslot) ||
      (modobj->value[0] == AFFECT_ROCKET && rocket >= maxslot) ||
      (modobj->value[0] == AFFECT_TORPEDO && torpedo >= maxslot) ||
      (modobj->value[0] == AFFECT_HULL && hull >= maxslot) ||
      (modobj->value[0] == AFFECT_SHIELD && shields >= maxslot) ||
      (modobj->value[0] == AFFECT_SPEED && speed >= maxslot) ||
      (modobj->value[0] == AFFECT_HYPER && hyperspeed >= maxslot) ||
      (modobj->value[0] == AFFECT_ENERGY && energy >= maxslot) ||
      (modobj->value[0] == AFFECT_MANUEVER && manuever >= maxslot) ||
      (modobj->value[0] == AFFECT_ALARM && alarm >= maxslot) ||
      (modobj->value[0] == AFFECT_SLAVE && slave >= maxslot) ||
      (modobj->value[0] == AFFECT_TRACTOR && tractor >= maxslot) ||
      (modobj->value[0] == AFFECT_COUNTERMEASURES && countermeasures >= maxslot) ||
      (modobj->value[0] == AFFECT_TERTIARY && tertiary >= maxslot) ||
      (modobj->value[0] == AFFECT_BOMB && bombs >= maxslot) ||
      (modobj->value[0] == AFFECT_CARGO && maxcargo >= maxslot))
  {
    send_to_char("&RYou've already filled that slot to its maximum.\n\r", ch);
    return;
  }
  
      for(x=0;x<NUM_PROTOTYPES;x++)  
       {  
         if(!str_cmp(ship->protoname, ship_prototypes[x].sname) )  
           {  
              maxmod = ship_prototypes[x].mods;  
              break;  
           }  
       }

     if (i >= maxmod)
      {
	send_to_char("This ship is already at it's module limit!\n\r", ch);
	return;
      }

     if( IS_SET(ship->flags, SHIP_SIMULATOR))
      {
	send_to_char("Simulators can't have modules, fool!\n\r", ch);
	return;
      }

     chance = IS_NPC(ch) ? ch->top_level
        : (int) (ch->pcdata->learned[gsn_installmodule]);
     if ( number_percent( ) < chance )
      {
  	ch->dest_buf   = str_dup(arg);
       	send_to_char( "&GYou begin the long process of installing a new module.\n\r", ch);
    	sprintf(buf, "$n takes out $s toolkit and a module and begins to work.\n\r");
     	act( AT_PLAIN, buf, ch, NULL, argument , TO_ROOM );
     	add_timer ( ch , TIMER_DO_FUN , 5 , do_installmodule , 1 );
    	return;
      }

      send_to_char("&RYou are unable to figure out what to do.\n\r",ch);
      learn_from_failure( ch, gsn_installmodule );
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
    	  send_to_char("&RYou are interupted and fail to finish.\n\r", ch);
    	  return;
     }
     ch->substate = SUB_NONE;

     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
         if (obj->item_type == ITEM_TOOLKIT)
           checktool = TRUE;
         if (obj->item_type == ITEM_MODULE){
           checkmod = TRUE;
           modobj = obj;
         }
           
      }
    chance = IS_NPC(ch) ? ch->top_level
               : (int) (ch->pcdata->learned[gsn_installmodule]);

    ship = ship_from_engine(ch->in_room->vnum);
    if (!ship){
      send_to_char("Error: Something went wrong. Contact an Admin!\n\r", ch);
      return;
    }

    if ( number_percent( ) > chance*2 )
     {
       send_to_char("&RYou finish installing the new module and everything's looking good...\n\r", ch);
       send_to_char("&RThen it turns bright red and melts!\n\r", ch);
       learn_from_failure( ch, gsn_installmodule );
       return;
     }

      CREATE(mod, MODULE_DATA, 1);
      LINK(mod, ship->first_module, ship->last_module, next, prev);
      mod->affect = modobj->value[0];
      mod->ammount = modobj->value[1];
      if(mod->affect == AFFECT_PRIMARY)
        ship->primaryCount+=mod->ammount;
      if(mod->affect == AFFECT_SECONDARY)
        ship->secondaryCount+=mod->ammount;
      if(mod->affect == AFFECT_MISSILE)
        {
          ship->maxmissiles+=mod->ammount;
          ship->missiles=ship->maxmissiles;
        }
      if(mod->affect == AFFECT_ROCKET)
        {
          ship->maxrockets+=mod->ammount;
          ship->rockets=ship->maxrockets;
        }
      if(mod->affect == AFFECT_TORPEDO)
        {
          ship->maxtorpedos+=mod->ammount;
          ship->torpedos=ship->maxtorpedos;
        }
      if(mod->affect == AFFECT_HULL)
        ship->maxhull+=mod->ammount;
      if(mod->affect == AFFECT_SHIELD)
        ship->maxshield+=mod->ammount;
      if(mod->affect == AFFECT_SPEED)
        ship->realspeed+=mod->ammount;
      if(mod->affect == AFFECT_HYPER)
	ship->hyperspeed-=mod->ammount; 
      if(mod->affect == AFFECT_ENERGY)
        ship->maxenergy+=mod->ammount;
      if(mod->affect == AFFECT_MANUEVER)
        ship->manuever+=mod->ammount;
      if(mod->affect == AFFECT_ALARM)
        ship->alarm+=mod->ammount;
      if(mod->affect == AFFECT_COUNTERMEASURES)
        ship->maxcountermeasures+=mod->ammount;
      if(mod->affect == AFFECT_SLAVE)
        ship->slave+=mod->ammount;
      if(mod->affect == AFFECT_TRACTOR)
        ship->tractorbeam+=mod->ammount;
      if(mod->affect == AFFECT_TERTIARY)
        ship->tertiaryCount+=mod->ammount;
      if(mod->affect == AFFECT_BOMB)
        {
          ship->maxbombs+=mod->ammount;
          ship->bombs=ship->maxbombs;
        }
      if(mod->affect == AFFECT_CARGO)
        ship->maxcargo+=mod->ammount;
      save_ship(ship);        
      separate_obj( modobj );
      obj_from_char( modobj );
      extract_obj(modobj);
      send_to_char("You finish installing your new module.\n\r", ch);
      
     {
      long xpgain;
      xpgain = ((ch->skill_level[TECHNICIAN_ABILITY]+1) * 300);
      gain_exp(ch, xpgain, TECHNICIAN_ABILITY);
      ch_printf( ch , " You gain %d experience for being a Technician.\n\r", xpgain );
      learn_from_success( ch, gsn_installmodule ) ;
     }
      return;
}

void do_shiplist(CHAR_DATA *ch, char *argument)
{
	int x;

        send_to_char("\n\r&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w                            Complete ship listing                      &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w           :Ships Name:                         :Type:                 &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
	
        for(x=0;x<NUM_PROTOTYPES;x++)  
	     ch_printf(ch,"&W|&w &w(&W%2d&w) %s%-36.36s &W %s %-16.16s  %s&W|\r\n", x+1,
            !str_cmp(ship_prototypes[x].clan, "Neutral") ? "&C" :
            !str_cmp(ship_prototypes[x].clan, "The Empire") ? "&R" :
            !str_cmp(ship_prototypes[x].clan, "") ? "&B" :
            !str_cmp(ship_prototypes[x].clan, "The New Republic") ? "&G" : 
			!str_cmp(ship_prototypes[x].clan, "The Jedi Order") ? "&C" : "&w",
            ship_prototypes[x].name,
			!str_cmp(ship_prototypes[x].clan, "Neutral") ? "&CUnknown?" :
		        !str_cmp(ship_prototypes[x].clan, "The Empire") ? "&RImperial" :
		        !str_cmp(ship_prototypes[x].clan, "") ? "&BCivilian" :
			!str_cmp(ship_prototypes[x].clan, "The New Republic") ? "&GRepublic" :
			!str_cmp(ship_prototypes[x].clan, "The Jedi Order") ? "&CJedi" :
			"&w",
		        ship_prototypes[x].class == SHIP_FIGHTER ? "Starfighter" :
		        ship_prototypes[x].class == SHIP_BOMBER  ? "Bomber" : 
		        ship_prototypes[x].class == SHIP_SHUTTLE ? "Shuttle" : 
		        ship_prototypes[x].class == SHIP_FREIGHTER ? "Freighter" : 
		        ship_prototypes[x].class == SHIP_FRIGATE ? "Frigate" : 
		        ship_prototypes[x].class == SHIP_TT ? "Troop Transport" : 
		        ship_prototypes[x].class == SHIP_CORVETTE ? "Corvette" : 
			ship_prototypes[x].class == SHIP_LFRIGATE ? "Large Frigate" :
		        ship_prototypes[x].class == SHIP_CRUISER ? "Cruiser" : 
			    ship_prototypes[x].class == SHIP_DREADNAUGHT ? "Dreadnaught" : 
		        ship_prototypes[x].class == SHIP_DESTROYER ? "Star Destroyer" : 
		        ship_prototypes[x].class == SHIP_SPACE_STATION ? "Space Station" : 
		        ship_prototypes[x].class == LAND_VEHICLE ? "Land vehicle" : "Unknown",
				!str_cmp(ship_prototypes[x].clan, "The Jedi Order") ? "    " : "");
          
            
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\r\n",ch);
        send_to_char("&W|&w               Use 'shipstat (number)' to get ship information         &W|\r\n",ch);
        send_to_char("&z+&W-----------------------------------------------------------------------&z+\n\r",ch);
      return;
}

/*free up ship data by Keberus */ 
void free_ship( SHIP_DATA *ship ) 
{ 
  MODULE_DATA *mod, *mod_next; 
  TURRET_DATA *turret, *tnext; 

    STRFREE( ship->clanowner ); 
    STRFREE( ship->copilot ); 
    STRFREE( ship->description ); 
    STRFREE( ship->home ); 
    STRFREE( ship->name ); 
    STRFREE( ship->owner ); 
    STRFREE( ship->pilot ); 
    DISPOSE( ship->filename ); 
    STRFREE( ship->protoname ); 
    STRFREE( ship->pbeacon ); 
    STRFREE( ship->dest ); 

  /*dispose of ship module data*/ 
  for( mod = ship->first_module; mod; mod = mod_next ) 
  { 
     mod_next = mod->next; 
     UNLINK( mod, ship->first_module, ship->last_module, next, prev ); 
     DISPOSE(mod); 
  } 

  /*dispose of ship turret data*/ 
  for( turret = ship->first_turret; turret; turret = tnext ) 
  { 
     tnext = turret->next; 
     UNLINK( turret, ship->first_turret, ship->last_turret, next, prev ); 
     DISPOSE(turret); 
  } 

  DISPOSE( ship ); 
  return; 
} 

