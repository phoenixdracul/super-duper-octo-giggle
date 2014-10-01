/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 						Miscellaneous Player Functions 					    *
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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

char * num_punct(int foo);

char * min_max_gauge(float min, float max, int size);
char * min_max_gauge_256(float min, float max, int size);

/*
 *  Locals
 */
char *tiny_affect_loc_name(int location);
char *exp_bar( CHAR_DATA*, int );

void do_gold(CHAR_DATA * ch, char *argument)
{
   set_char_color( AT_GOLD, ch );
   ch_printf( ch,  "You have %s credits.\n\r", num_punct(ch->gold) );
   return;
}


char *drawlife(int min, int max){
   static char buf[MAX_STRING_LENGTH];
   int per;
   per = max > 0 ? ((min * 100) / max) : 100;
   if (per < 0) per = 0;
   if (per > 100) per = 100;
                   if (per >= 100)  sprintf(buf, "&G&W[&R|||||&Y||||&G||||&W]");
   else if (per >= 90 && per < 100) sprintf(buf, "&G&W[&R|||||&Y||||&G|||&G-&W]");
   else if (per >= 80 && per < 90)  sprintf(buf, "&G&W[&R|||||&Y||||&G||&G--&W]");
   else if (per >= 70 && per < 80)  sprintf(buf, "&G&W[&R|||||&Y||||&G|&G---&W]");
   else if (per >= 60 && per < 70)  sprintf(buf, "&G&W[&R|||||&Y|||&G&G-----&W]");
   else if (per >= 50 && per < 60)  sprintf(buf, "&G&W[&R|||||&Y||&G&G------&W]");
   else if (per >= 40 && per < 50)  sprintf(buf, "&G&W[&R|||||&Y|&G&G-------&W]");
   else if (per >= 30 && per < 40)  sprintf(buf, "&G&W[&R|||||&Y&G&G--------&W]");
   else if (per >= 30 && per < 40)  sprintf(buf, "&G&W[&R||||&Y&G&G---------&W]");
   else if (per >= 20 && per < 40)  sprintf(buf, "&G&W[&R|||&Y&G&G----------&W]");
   else if (per >= 10 && per < 40)  sprintf(buf, "&G&W[&R||&Y&G&G-----------&W]");
   else if (per >= 0 &&  per < 10)  sprintf(buf, "&G&W[&R&Y&G&G-------------&W]");
   return buf;
}

char *drawmove(int min, int max){
   static char buf[MAX_STRING_LENGTH];
   int per;
   per = max > 0 ? ((min * 100) / max) : 100;
                   if (per >= 100)  sprintf(buf, "&G&W[&R|||&Y||&G||&W]");
   else if (per >= 90 && per < 100) sprintf(buf, "&G&W[&R|||&Y||&G|&G-&W]");
   else if (per >= 80 && per < 90)  sprintf(buf, "&G&W[&R|||&Y||&G&G--&W]");
   else if (per >= 70 && per < 80)  sprintf(buf, "&G&W[&R|||&Y|&G&G---&W]");
   else if (per >= 60 && per < 70)  sprintf(buf, "&G&W[&R|||&Y&G&G----&W]");
   else if (per >= 50 && per < 60)  sprintf(buf, "&G&W[&R||&Y&G&G-----&W]");
   else if (per >= 40 && per < 50)  sprintf(buf, "&G&W[&R|&Y&G&G------&W]");
   else if (per >= 30 && per < 40)  sprintf(buf, "&G&W[&R|&G&Y&G------&W]");
   else if (per >= 20 && per < 40)  sprintf(buf, "&G&W[&R|&G&Y&G------&W]");
   else if (per >= 10 && per < 40)  sprintf(buf, "&G&W[&R|&G&Y&G------&W]");
   else if (per >= 0 &&  per < 10)  sprintf(buf, "&G&W[&R&G&Y&G-------&W]");
   return buf;
}

char *drawalign(int align){
  static char buf[MAX_STRING_LENGTH];
  if (align >= 100) sprintf(buf, "&W[&C============&W|&W]");
  else if (align >= 90 && align < 100) sprintf(buf, "&W[&C===========&W|&C=&W]");
  else if (align >= 60 && align < 90) sprintf(buf, "&W[&C==========&W|&C==&W]");
  else if (align >= 40 && align < 60) sprintf(buf, "&W[&C=========&W|&C===&W]");
  else if (align >= 20 && align < 40) sprintf(buf, "&W[&C========&W|&C====&W]");
  else if (align >= 10 && align < 20) sprintf(buf, "&W[&C=======&W|&C=====&W]");
  else if (align >= 0 && align < 10) sprintf(buf, "&W[&C======&W|&C======&W]");
  else if (align <= -1 && align > -20) sprintf(buf, "&W[&C=====&W|&C=======&W]");
  else if (align <= -20 && align > -40) sprintf(buf, "&W[&C====&W|&C========&W]");
  else if (align <= -60 && align > -80) sprintf(buf, "&W[&C===&W|&C=========&W]");
  else if (align <= -80 && align > -100) sprintf(buf, "&W[&C==&W|&C==========&W]");
  else if (align <= -100) sprintf(buf, "&W[&W|&C============&W]");

  return buf;
}
/*
  New Score by Calo [Daniel Donaldson]
*/
void do_score( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  int ability;
    if(!argument || argument[0] == '\0')
        victim = ch;
    else if(IS_IMMORTAL(ch))
    {
        if((victim = get_char_world( ch, argument )) == NULL)
        {
            send_to_char("Victim not found.\r\n",ch);
          return;
        }
        if(xIS_SET(victim->act, PLR_WIZINVIS) && victim->pcdata->wizinvis > ch->top_level)
        {
            send_to_char("Victim not found.\r\n",ch);
          return;
        }
    }
    else
        victim = ch;
    if (IS_NPC(victim))
    {
	do_oldscore(ch, argument);
	return;
    }

  send_to_char("&r+==========================================================+&W\n\r", ch);
  send_to_char("&r|  &WAdvanced Universe                          &r|    &WScore   &r|&W\n\r", ch);
  send_to_char("&r+---------------------------------------------+------------+&W\n\r", ch);
  ch_printf(ch, "&r|  &CData File: 19049-%-4.4s-3940305                           &r|&W\n\r", sysdata.mud_acronym);
  ch_printf(ch, "&r|  &CName:  &W%-25.25s          &CStrength: &W%-2d  &r|\n\r", victim->name, get_curr_str(victim));
  if (victim->pcdata->clan)
    ch_printf(ch, "&r|  &CClan:  &W%-25.25s         &CDexterity: &W%-2d  &r|\n\r", victim->pcdata->clan->name, get_curr_dex(victim));
  else
  ch_printf(ch, "&r|  &CClan:  &W%-25.25s         &CDexterity: &W%-2d  &r|\n\r", "None", get_curr_dex(victim));
  ch_printf(ch, "&r|  &CRace:  &W%-25.25s      &CConstitution: &W%-2d  &r|\n\r", capitalize(get_race(victim)), get_curr_con(victim));
  ch_printf(ch, "&r|  &CArmor: &W%-25d      &CIntelligence: &W%-2d  &r|\n\r", GET_AC(victim), get_curr_int(victim));
  ch_printf(ch, "&r|  &CHP:    %s  &CMove: %s     &CWisdom: &W%-2d  &r|\n\r", drawlife(victim->hit, victim->max_hit), drawmove(victim->move, victim->max_move), get_curr_wis(victim));
  ch_printf(ch, "&r|  &CAlign: %s                    &CCharisma: &W%-2d  &r|\n\r", drawalign(victim->alignment), get_curr_cha(victim));
  send_to_char("&r+==========================================================+&W\n\r", ch);
  ch_printf(ch, "&r|  &CCredits: &W%-10s", num_punct(victim->gold));
  ch_printf(ch, "             &CSavings: &W%-10s     &r|&W\n\r", num_punct(victim->pcdata->bank));
  if(ch->plr_home && ch->plr_home->area->planet)
	ch_printf(ch, "&r|  &CHome on Planet: &W%-10s                              &r|\n\r", ch->plr_home->area->planet->name);
//  ch_printf(ch, "&r|  &CWeight:  %s        &CItems: %s  &r|&W\n\r", drawlife(victim->carry_weight, can_carry_w(victim)), drawlife(victim->carry_number, can_carry_n(victim)));
  ch_printf(ch, "&r|  &CWeight:  &W%5.5d &R(&Wmax %6.6d&R)     &CItems: &W%5.5d &R(&Wmax %5.5d&R)&r|&W\n\r",
  	ch->carry_weight, can_carry_w(ch) , ch->carry_number, can_carry_n(ch));

  send_to_char("&r+==========================================================+&W\n\r", ch);
  for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
  {
    if ( ability != FORCE_ABILITY )
    {
    	if((victim->skill_level[ability] > 2 || max_level(victim, ability) <= 2 || IS_IMMORTAL(ch)))
		  {
    		ch_printf( ch, "&r| &W%s&C%-15s   Level: &W%-3d   &Cof  &W%-3d   &CExp: &W%-10ld&r|\n\r",
    							victim->main_ability == ability ? "+": victim->secondary_ability == ability? "-": " ", ability_name[ability], victim->skill_level[ability], max_level(victim, ability), victim->experience[ability]);

		  }
		  else	// Put a '?' for Max if player hasn't reached level 3 in the class.
		  {
			  ch_printf( ch, "&r| &W%s&C%-15s   Level: &W%-3d   &Cof  &W%-3s   &CExp: &W%-10ld&r|\n\r",
					victim->main_ability == ability ? "+": victim->secondary_ability == ability? "-": " ", ability_name[ability], victim->skill_level[ability], "?", victim->experience[ability]);
		  }

    }
  }
  send_to_char("&r+==========================================================+&W\n\r", ch);
  send_to_char("&r|  &W+&C = Primary Ability, &W-&C = Secondary Ability            &r  |\n\r", ch);
  send_to_char("&r+==========================================================+&W\n\r", ch);
  ch_printf(ch, "&r|  &CSkill Points: &W%-4d  &CFeat Points: &W%-4d                 &r  |\n\r", ch->pcdata->skill_points, ch->pcdata->feat_points);
  send_to_char("&r+==========================================================+&W\n\r", ch);
  ch_printf(ch, "&R&r| &CCurrent Comm Frequency: &W%-9s     &CGood RP Points: &W%d  &r|&W\n\r", victim->comfreq, victim->rppoints ? victim->rppoints : 0 );
  if(ch->pcdata->lottery_number > 0)
	  ch_printf(ch, "&R&r| &CYour lottery number is: &W%3d                              &r|&W\n\r", ch->pcdata->lottery_number);
  ch_printf(ch,  "&r| &CCurrent Board: &W%-41s &r|\n\r", ch->pcdata->board ? ch->pcdata->board->short_name : "(none)");
  send_to_char("&r+==========================================================+&W\n\r", ch);
  send_to_char("&r|  &CFor more Information see lang, aff, armor, group        &r|\n\r", ch);
  send_to_char("&r+==========================================================+&W\n\r", ch);

  if(IS_IMMORTAL(ch))
  {
	  ch_printf(ch, "&r| &CIMMORTAL DATA&z:&C  Wizinvis [&R%-1s&C]  Wizlevel (&W%-2d&C)%-14s&r|\n\r",
  		xIS_SET(ch->act, PLR_WIZINVIS) ? "X" : " ", ch->pcdata->wizinvis, "" );

	  char defaultBamfin[MAX_STRING_LENGTH];
	  strcpy(defaultBamfin, ch->name);
	  strcat(defaultBamfin, " appears in a swirling mist.");
	  char defaultBamfout[MAX_STRING_LENGTH];
	  strcpy(defaultBamfout, ch->name);
	  strcat(defaultBamfout, " leaves in a swirling mist.");

          ch_printf(ch, "&r| &CMentor/Mentee:&C %-30s&r            |\n\r", ch->pcdata->mentor);

          ch_printf(ch, "&r| &CVnum room # [ %5d ]&r                                    |\n\r", victim->in_room->vnum);

	  ch_printf(ch, "&r| &CBamfin&z:&C  %-48s&r|\n\r", (ch->pcdata->bamfin && ch->pcdata->bamfin[0] != '\0')
  		? ch->pcdata->bamfin : defaultBamfin);
	  ch_printf(ch, "&r| &CBamfout&z:&C %-48s&r|\n\r", (ch->pcdata->bamfout && ch->pcdata->bamfout[0] != '\0')
  		? ch->pcdata->bamfout : defaultBamfout);

	  if (ch->pcdata->area)
	  {
  	    ch_printf(ch, "&r| &CVnums&z:&C Room &z(&W%-5.5d &C-&W %-5.5d&z)&C Object &z(&W%-5.5d &z- &W%-5.5d&z)&C       &r|\n\r",
  		ch->pcdata->area->low_r_vnum, ch->pcdata->area->hi_r_vnum,
  		ch->pcdata->area->low_o_vnum, ch->pcdata->area->hi_o_vnum);
  	    ch_printf(ch, "&r|        &CMob  &z(&W%-5.5d &C- &W%-5.5d&z) &CArea Loaded &r[%s]  &r            |\n\r",
  	    		ch->pcdata->area->low_m_vnum, ch->pcdata->area->hi_m_vnum,
  	    		(IS_SET (ch->pcdata->area->status, AREA_LOADED)) ? "&RX&r" : " ");
	  }
	  send_to_char("&r+----------------------------------------------------------+&W\n\r", ch);
  }

}

void do_armor(CHAR_DATA * ch, char * argument)
{
	float min = 0.0, max = 0.0;
	int i;

//					   |----+----+----+----+----+----+----+----+----+----+|
	send_to_char("&G.------------------.--------------------------------------------------.\n\r", ch);
	send_to_char("&G|Resistance        |&R0%                     &Y50%                    &G100%|\n\r", ch);
	send_to_char("&G|------------------+--------------------------------------------------|\n\r", ch);

	for (i = 0; i < RES_MAX; ++i)
	{
		min = calc_res_min(ch, i);
		max = calc_res(ch, i);
		if (IS_NPC(ch) || !IS_SET(ch->pcdata->flags, PCFLAG_256COL))
			ch_printf(ch, "&G|&W%-18s&G|%s&G|\n\r", tiny_affect_loc_name(i + APPLY_RES_1), min_max_gauge(min, max, 50));
		else
			ch_printf(ch, "&G|&W%-18s&G|%s&G|\n\r", tiny_affect_loc_name(i + APPLY_RES_1), min_max_gauge_256(min, max, 50));
	}

	send_to_char("&G'------------------'--------------------------------------------------'\n\r", ch);
}

/*
 * New score command by Haus
 */
/*
void do_score(CHAR_DATA * ch, char *argument)
{
    char            buf[MAX_STRING_LENGTH];
    AFFECT_DATA    *paf;
    int iLang, drug;

    if (IS_NPC(ch))
    {
	do_oldscore(ch, argument);
	return;
    }
    set_char_color(AT_OLDSCORE, ch);
    send_to_char(drawlife(ch->hit, ch->max_hit), ch);
    ch_printf(ch, "\n\rScore for %s.\n\r", ch->pcdata->title);
    set_char_color(AT_OLDSCORE, ch);
    if ( get_trust( ch ) != ch->top_level )
	ch_printf( ch, "You are trusted at level %d.\n\r", get_trust( ch ) );

    send_to_char("----------------------------------------------------------------------------\n\r", ch);

    ch_printf(ch,   "Race: %3d year old %-10.10s                Log In:  %s\r",
	get_age(ch), capitalize(get_race(ch)), ctime(&(ch->logon)) );
	
    ch_printf(ch,   "Hitroll: %-2.2d  Damroll: %-2.2d   Armor: %-4d        Saved:  %s\r",
		GET_HITROLL(ch), GET_DAMROLL(ch), GET_AC(ch),
		ch->save_time ? ctime(&(ch->save_time)) : "no\n" );

    ch_printf(ch,   "Align: %-5d    Wimpy: %-3d                    Time:   %s\r",
		ch->alignment, ch->wimpy  , ctime(&current_time) );

    if ( ch->skill_level[FORCE_ABILITY] > 1
    || IS_IMMORTAL(ch) )
      	ch_printf(ch, "Hit Points: %d of %d     Move: %d of %d     Force: %d of %d\n\r",
            ch->hit, ch->max_hit, ch->move, ch->max_move, ch->mana, ch->max_mana );
    else
      	ch_printf(ch, "Hit Points: %d of %d     Move: %d of %d\n\r",
            ch->hit, ch->max_hit, ch->move, ch->max_move);

    ch_printf(ch, "Str: %2d  Dex: %2d  Con: %2d  Int: %2d  Wis: %2d  Cha: %2d  Lck: ??  Frc: ??\n\r",
	get_curr_str(ch), get_curr_dex(ch),get_curr_con(ch),get_curr_int(ch),get_curr_wis(ch),get_curr_cha(ch));

    
    send_to_char("----------------------------------------------------------------------------\n\r", ch);

    { 
       int ability;
    
       for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
          if ( ability != FORCE_ABILITY || ch->skill_level[FORCE_ABILITY] > 1 )
            ch_printf( ch, "%-15s   Level: %-3d   Max: %-3d   Exp: %-10ld   Next: %-10ld\n\r",
            ability_name[ability], ch->skill_level[ability], max_level(ch, ability), ch->experience[ability],
            exp_level( ch->skill_level[ability]+1 ) );
          else
            ch_printf( ch, "%-15s   Level: %-3d   Max: ???   Exp: ???          Next: ???\n\r",
            ability_name[ability], ch->skill_level[ability], ch->experience[ability]);
    }

    send_to_char("----------------------------------------------------------------------------\n\r", ch);

    
    
    ch_printf(ch, "CREDITS: %-10d   BANK: %-10d    Pkills: %-5.5d   Mkills: %-5.5d\n\r",
	ch->gold, ch->pcdata->bank, ch->pcdata->pkills, ch->pcdata->mkills);
 
    ch_printf(ch, "Weight: %5.5d (max %7.7d)    Items: %5.5d (max %5.5d)\n\r",
	ch->carry_weight, can_carry_w(ch) , ch->carry_number, can_carry_n(ch));
 
    ch_printf(ch, "Pager: (%c) %3d   AutoExit(%c)  AutoLoot(%c)  Autosac(%c)\n\r", 
	IS_SET(ch->pcdata->flags, PCFLAG_PAGERON) ? 'X' : ' ',
	ch->pcdata->pagerlen, IS_SET(ch->act, PLR_AUTOEXIT) ? 'X' : ' ', 
	IS_SET(ch->act, PLR_AUTOLOOT) ? 'X' : ' ', IS_SET(ch->act, PLR_AUTOSAC) ? 'X' : ' ');
    
    switch (ch->position)
    {
	case POS_DEAD:
		sprintf(buf, "You are slowly decomposing. ");
		break;
	case POS_MORTAL:
		sprintf(buf, "You are mortally wounded. ");
		break;
	case POS_INCAP:
		sprintf(buf, "You are incapacitated. ");
		break;
	case POS_STUNNED:
		sprintf(buf, "You are stunned. ");
		break;
	case POS_SLEEPING:
		sprintf(buf, "You are sleeping. ");
		break;
	case POS_RESTING:
		sprintf(buf, "You are resting. ");
		break;
	case POS_STANDING:
		sprintf(buf, "You are standing. ");
		break;
	case POS_FIGHTING:
		sprintf(buf, "You are fighting. " );
		break;
	case POS_MOUNTED:
		sprintf(buf, "You are mounted. ");
		break;
        case POS_SITTING:
		sprintf(buf, "You are sitting. ");
		break;
    }
    
    send_to_char( buf, ch );
    
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	send_to_char("You are drunk.\n\r", ch);

    if ( ch->position != POS_SLEEPING )
	switch( ch->mental_state / 10 )
	{
	    default:   send_to_char( "You're completely messed up!\n\r", ch );	break;
	    case -10:  send_to_char( "You're barely conscious.\n\r", ch );	break;
	    case  -9:  send_to_char( "You can barely keep your eyes open.\n\r", ch );	break;
	    case  -8:  send_to_char( "You're extremely drowsy.\n\r", ch );	break;
	    case  -7:  send_to_char( "You feel very unmotivated.\n\r", ch );	break;
	    case  -6:  send_to_char( "You feel sedated.\n\r", ch );		break;
	    case  -5:  send_to_char( "You feel sleepy.\n\r", ch );		break;
	    case  -4:  send_to_char( "You feel tired.\n\r", ch );		break;
	    case  -3:  send_to_char( "You could use a rest.\n\r", ch );		break;
	    case  -2:  send_to_char( "You feel a little under the weather.\n\r", ch );	break;
	    case  -1:  send_to_char( "You feel fine.\n\r", ch );		break;
	    case   0:  send_to_char( "You feel great.\n\r", ch );		break;
	    case   1:  send_to_char( "You feel energetic.\n\r", ch );	break;
	    case   2:  send_to_char( "Your mind is racing.\n\r", ch );	break;
	    case   3:  send_to_char( "You can't think straight.\n\r", ch );	break;
	    case   4:  send_to_char( "Your mind is going 100 miles an hour.\n\r", ch );	break;
	    case   5:  send_to_char( "You're high as a kite.\n\r", ch );	break;
	    case   6:  send_to_char( "Your mind and body are slipping apart.\n\r", ch );	break;
	    case   7:  send_to_char( "Reality is slipping away.\n\r", ch );	break;
	    case   8:  send_to_char( "You have no idea what is real, and what is not.\n\r", ch );	break;
	    case   9:  send_to_char( "You feel immortal.\n\r", ch );	break;
	    case  10:  send_to_char( "You are a Supreme Entity.\n\r", ch );	break;
	}
    else
    if ( ch->mental_state >45 )
	send_to_char( "Your sleep is filled with strange and vivid dreams.\n\r", ch );
    else
    if ( ch->mental_state >25 )
	send_to_char( "Your sleep is uneasy.\n\r", ch );
    else
    if ( ch->mental_state <-35 )
	send_to_char( "You are deep in a much needed sleep.\n\r", ch );
    else
    if ( ch->mental_state <-25 )
	send_to_char( "You are in deep slumber.\n\r", ch );
    send_to_char("SPICE Level/Addiction: ", ch );
    for ( drug = 0; drug <= 9; drug++ )
	if ( ch->pcdata->drug_level[drug] > 0 || ch->pcdata->drug_level[drug] > 0 )
	{
	    ch_printf( ch, "%s(%d/%d) ", spice_table[drug],
	                                 ch->pcdata->drug_level[drug],
	                                 ch->pcdata->addiction[drug] );
	}
    send_to_char("\n\rLanguages: ", ch );
    for ( iLang = 0; lang_array[iLang] != LANG_UNKNOWN; iLang++ )
	if ( knows_language( ch, lang_array[iLang], ch )
	||  (IS_NPC(ch) && ch->speaks == 0) )
	{
	    if ( lang_array[iLang] & ch->speaking
	    ||  (IS_NPC(ch) && !ch->speaking) )
		set_char_color( AT_RED, ch );
	    send_to_char( lang_names[iLang], ch );
	    send_to_char( " ", ch );
	    set_char_color( AT_OLDSCORE, ch );
	}
    
    send_to_char( "\n\r", ch );
    ch_printf( ch, "WANTED ON: %s\n\r",
             flag_string(ch->pcdata->wanted_flags, planet_flags) );
                            
    if ( ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0' )
	ch_printf( ch, "You are bestowed with the command(s): %s.\n\r", 
		ch->pcdata->bestowments );

    if ( ch->pcdata->clan )
    {
      send_to_char( "----------------------------------------------------------------------------\n\r", ch);
      ch_printf(ch, "ORGANIZATION: %-35s Pkills/Deaths: %3.3d/%3.3d",
		ch->pcdata->clan->name, ch->pcdata->clan->pkills, ch->pcdata->clan->pdeaths) ;
      send_to_char( "\n\r", ch );
    }  
    if (IS_IMMORTAL(ch))
    {
	send_to_char( "----------------------------------------------------------------------------\n\r", ch);

	ch_printf(ch, "IMMORTAL DATA:  Wizinvis [%s]  Wizlevel (%d)\n\r",
		IS_SET(ch->act, PLR_WIZINVIS) ? "X" : " ", ch->pcdata->wizinvis );

	ch_printf(ch, "Bamfin:  %s\n\r", (ch->pcdata->bamfin[0] != '\0')
		? ch->pcdata->bamfin : "%s appears in a swirling mist.", ch->name);
	ch_printf(ch, "Bamfout: %s\n\r", (ch->pcdata->bamfout[0] != '\0')
		? ch->pcdata->bamfout : "%s leaves in a swirling mist.", ch->name);


	if (ch->pcdata->area)
	{
	    ch_printf(ch, "Vnums:   Room (%-5.5d - %-5.5d)   Object (%-5.5d - %-5.5d)   Mob (%-5.5d - %-5.5d)\n\r",
		ch->pcdata->area->low_r_vnum, ch->pcdata->area->hi_r_vnum,
		ch->pcdata->area->low_o_vnum, ch->pcdata->area->hi_o_vnum,
		ch->pcdata->area->low_m_vnum, ch->pcdata->area->hi_m_vnum);
	    ch_printf(ch, "Area Loaded [%s]\n\r", (IS_SET (ch->pcdata->area->status, AREA_LOADED)) ? "yes" : "no");
	}
    }
    if (ch->first_affect)
    {
	int i;
	SKILLTYPE *sktmp;

	i = 0;
	send_to_char( "----------------------------------------------------------------------------\n\r", ch);
	send_to_char("AFFECT DATA:                            ", ch);
	for (paf = ch->first_affect; paf; paf = paf->next)
	{
	    if ( (sktmp=get_skilltype(paf->type)) == NULL )
		continue;
	    if (ch->top_level < 20)
	    {
		ch_printf(ch, "[%-34.34s]    ", sktmp->name);
		if (i == 0)
		   i = 2;
		if ((++i % 3) == 0)
		   send_to_char("\n\r", ch);
	     }
	     else
	     {
		if (paf->modifier == 0)
		    ch_printf(ch, "[%-24.24s;%5d rds]    ",
			sktmp->name,
			paf->duration);
		else
		if (paf->modifier > 999)
		    ch_printf(ch, "[%-15.15s; %7.7s;%5d rds]    ",
			sktmp->name,
			tiny_affect_loc_name(paf->location),
			paf->duration);
		else
		    ch_printf(ch, "[%-11.11s;%+-3.3d %7.7s;%5d rds]    ",
			sktmp->name,
			paf->modifier,
			tiny_affect_loc_name(paf->location),
			paf->duration);
		if (i == 0)
		    i = 1;
		if ((++i % 2) == 0)
		    send_to_char("\n\r", ch);
	    }
	}
    }
    send_to_char("\n\r", ch);
    return;
}
*/
/*
 * Return ascii name of an affect location.
 */
char           *
tiny_affect_loc_name(int location)
{
	switch (location) {
	case APPLY_NONE:		return "NIL";
	case APPLY_STR:			return " STR  ";
	case APPLY_DEX:			return " DEX  ";
	case APPLY_INT:			return " INT  ";
	case APPLY_WIS:			return " WIS  ";
	case APPLY_CON:			return " CON  ";
	case APPLY_CHA:			return " CHA  ";
	case APPLY_LCK:			return " LCK  ";
	case APPLY_SEX:			return " SEX  ";
	case APPLY_LEVEL:		return " LVL  ";
	case APPLY_AGE:			return " AGE  ";
	case APPLY_MANA:		return " MANA ";
	case APPLY_HIT:			return " HV   ";
	case APPLY_MOVE:		return " MOVE ";
	case APPLY_GOLD:		return " GOLD ";
	case APPLY_EXP:			return " EXP  ";
	case APPLY_AC:			return " AC   ";
	case APPLY_HITROLL:		return " HITRL";
	case APPLY_DAMROLL:		return " DAMRL";
	case APPLY_SAVING_POISON:	return "SV POI";
	case APPLY_SAVING_ROD:		return "SV ROD";
	case APPLY_SAVING_PARA:		return "SV PARA";
	case APPLY_SAVING_BREATH:	return "SV BRTH";
	case APPLY_SAVING_SPELL:	return "SV SPLL";
	case APPLY_HEIGHT:		return "HEIGHT";
	case APPLY_WEIGHT:		return "WEIGHT";
	case APPLY_AFFECT:		return "AFF BY";
	case APPLY_RESISTANT:		return "RESIST";
	case APPLY_IMMUNE:		return "IMMUNE";
	case APPLY_SUSCEPTIBLE:		return "SUSCEPT";
	case APPLY_WEAPONSPELL:		return " WEAPON";
	case APPLY_BACKSTAB:		return "BACKSTB";
	case APPLY_PICK:		return " PICK  ";
	case APPLY_TRACK:		return " TRACK ";
	case APPLY_STEAL:		return " STEAL ";
	case APPLY_SNEAK:		return " SNEAK ";
	case APPLY_HIDE:		return " HIDE  ";
	case APPLY_PALM:		return " PALM  ";
	case APPLY_DETRAP:		return " DETRAP";
	case APPLY_DODGE:		return " DODGE ";
	case APPLY_PEEK:		return " PEEK  ";
	case APPLY_SCAN:		return " SCAN  ";
	case APPLY_GOUGE:		return " GOUGE ";
	case APPLY_SEARCH:		return " SEARCH";
	case APPLY_MOUNT:		return " MOUNT ";
	case APPLY_DISARM:		return " DISARM";
	case APPLY_KICK:		return " KICK  ";
	case APPLY_PARRY:		return " PARRY ";
	case APPLY_BASH:		return " BASH  ";
	case APPLY_STUN:		return " STUN  ";
	case APPLY_PUNCH:		return " PUNCH ";
	case APPLY_CLIMB:		return " CLIMB ";
	case APPLY_GRIP:		return " GRIP  ";
	case APPLY_SCRIBE:		return " SCRIBE";
	case APPLY_COVER_TRAIL:		return " COVER ";
	case APPLY_WEARSPELL:		return " WEAR  ";
	case APPLY_REMOVESPELL:		return " REMOVE";
	case APPLY_EMOTION:		return "EMOTION";
	case APPLY_MENTALSTATE:		return " MENTAL";
	case APPLY_STRIPSN:		return " DISPEL";
	case APPLY_REMOVE:		return " REMOVE";
	case APPLY_DIG:			return " DIG   ";
	case APPLY_FULL:		return " HUNGER";
	case APPLY_THIRST:		return " THIRST";
	case APPLY_DRUNK:		return " DRUNK ";
	case APPLY_BLOOD:		return " BLOOD ";
	case APPLY_RES_1:           	return "Plasma";
	case APPLY_RES_2:           	return "Electric";
	case APPLY_RES_3:           	return "Lightsaber";
	case APPLY_RES_4:           	return "Fire";
	case APPLY_RES_5:           	return "Cold";
	case APPLY_RES_6:           	return "Blunt";
	case APPLY_RES_7:           	return "Piercing";
	case APPLY_RES_8:           	return "Slashing";
	case APPLY_RES_9:           	return "Explosive";
	case APPLY_RES_10:          	return "Acid";
	case APPLY_RES_11:          	return "Poison";
	case APPLY_RES_12:          	return "Force";
	case APPLY_RES_13:          	return "Disruptor";
	case APPLY_EXTRA_ATTACK:	return "Extra Attacks";
	case APPLY_ARMOR_PEN:		return "Armor Pen";
	case APPLY_SHIELD_DEFLECT:	return "Shield Deflect";
	}

	bug("Affect_location_name: unknown location %d.", location);
	return "(? ? ?)";
}

const char * get_race( CHAR_DATA *ch)
{
    if ( ch->race < MAX_NPC_RACE && ch->race >= 0)
	return ( npc_race[ch->race] );
    return ("Unknown");
}

void do_oldscore( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *paf;
    SKILLTYPE   *skill;

    if ( IS_AFFECTED(ch, AFF_POSSESS) )
    {   
       send_to_char("You can't do that in your current state of mind!\n\r", ch);
       return;
    }

    set_char_color( AT_OLDSCORE, ch );
    ch_printf( ch,
	"You are %s%s&w&W%s, level %d, %d years old (%d hours).\n\r",
	ch->name,
	IS_NPC(ch) ? "" : ch->pcdata->title,color_str(AT_OLDSCORE, ch),
	ch->top_level,
	get_age(ch),
	(get_age(ch) - 17) );

    if ( get_trust( ch ) != ch->top_level )
	ch_printf( ch, "You are trusted at level %d.\n\r",
	    get_trust( ch ) );

    if ( xIS_SET(ch->act, ACT_MOBINVIS) )
      ch_printf( ch, "You are mobinvis at level %d.\n\r",
            ch->mobinvis);

    
      ch_printf( ch,
	"You have %d/%d hit, %d/%d movement.\n\r",
	ch->hit,  ch->max_hit,
	ch->move, ch->max_move);

    ch_printf( ch,
	"You are carrying %d/%d items with weight %d/%d kg.\n\r",
	ch->carry_number, can_carry_n(ch),
	ch->carry_weight, can_carry_w(ch) );

    ch_printf( ch,
	"Str: %d  Int: %d  Wis: %d  Dex: %d  Con: %d  Cha: %d  Lck: ??  Frc: ??\n\r",
	get_curr_str(ch),
	get_curr_int(ch),
	get_curr_wis(ch),
	get_curr_dex(ch),
	get_curr_con(ch),
	get_curr_cha(ch) );

    ch_printf( ch,
	"You have have %d credits.\n\r" , ch->gold );

    if ( !IS_NPC(ch) )
    ch_printf( ch,
	"You have achieved %d glory during your life, and currently have %d.\n\r",
	ch->pcdata->quest_accum, ch->pcdata->quest_curr );

    ch_printf( ch,
	"Autoexit: %s   Autoloot: %s   Autosac: %s   Autocred: %s\n\r",
	(!IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOEXIT)) ? "yes" : "no",
	(!IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOLOOT)) ? "yes" : "no",
	(!IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOSAC) ) ? "yes" : "no",
  	(!IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOGOLD)) ? "yes" : "no" );

    ch_printf( ch, "Wimpy set to %d hit points.\n\r", ch->wimpy );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10 )
	send_to_char( "You are drunk.\n\r",   ch );

    switch( ch->mental_state / 10 )
    {
        default:   send_to_char( "You're completely messed up!\n\r", ch ); break;
        case -10:  send_to_char( "You're barely conscious.\n\r", ch ); break;
        case  -9:  send_to_char( "You can barely keep your eyes open.\n\r", ch ); break;
        case  -8:  send_to_char( "You're extremely drowsy.\n\r", ch ); break;
        case  -7:  send_to_char( "You feel very unmotivated.\n\r", ch ); break;
        case  -6:  send_to_char( "You feel sedated.\n\r", ch ); break;
        case  -5:  send_to_char( "You feel sleepy.\n\r", ch ); break;
        case  -4:  send_to_char( "You feel tired.\n\r", ch ); break;
        case  -3:  send_to_char( "You could use a rest.\n\r", ch ); break;
        case  -2:  send_to_char( "You feel a little under the weather.\n\r", ch ); break;
        case  -1:  send_to_char( "You feel fine.\n\r", ch ); break;
        case   0:  send_to_char( "You feel great.\n\r", ch ); break;
        case   1:  send_to_char( "You feel energetic.\n\r", ch ); break;
        case   2:  send_to_char( "Your mind is racing.\n\r", ch ); break;
        case   3:  send_to_char( "You can't think straight.\n\r", ch ); break;
        case   4:  send_to_char( "Your mind is going 100 miles an hour.\n\r", ch ); break;
        case   5:  send_to_char( "You're high as a kite.\n\r", ch ); break;
        case   6:  send_to_char( "Your mind and body are slipping appart.\n\r", ch ); break;
        case   7:  send_to_char( "Reality is slipping away.\n\r", ch ); break;
        case   8:  send_to_char( "You have no idea what is real, and what is not.\n\r", ch ); break;
        case   9:  send_to_char( "You feel immortal.\n\r", ch ); break;
        case  10:  send_to_char( "You are a Supreme Entity.\n\r", ch ); break;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_char( "You are DEAD!!\n\r",		ch );
	break;
    case POS_MORTAL:
	send_to_char( "You are mortally wounded.\n\r",	ch );
	break;
    case POS_INCAP:
	send_to_char( "You are incapacitated.\n\r",	ch );
	break;
    case POS_STUNNED:
	send_to_char( "You are stunned.\n\r",		ch );
	break;
    case POS_SLEEPING:
	send_to_char( "You are sleeping.\n\r",		ch );
	break;
    case POS_RESTING:
	send_to_char( "You are resting.\n\r",		ch );
	break;
    case POS_STANDING:
	send_to_char( "You are standing.\n\r",		ch );
	break;
    case POS_FIGHTING:
	send_to_char( "You are fighting.\n\r",		ch );
	break;
    case POS_MOUNTED:
	send_to_char( "Mounted.\n\r",			ch );
	break;
    case POS_SHOVE:
	send_to_char( "Being shoved.\n\r",		ch );
	break;
    case POS_DRAG:
	send_to_char( "Being dragged.\n\r",		ch );
	break;
    }

    if ( ch->top_level >= 25 )
	ch_printf( ch, "AC: %d.  ", GET_AC(ch) );

    send_to_char( "You are ", ch );
	 if ( GET_AC(ch) >=  101 ) send_to_char( "WORSE than naked!\n\r", ch );
    else if ( GET_AC(ch) >=   80 ) send_to_char( "naked.\n\r",            ch );
    else if ( GET_AC(ch) >=   60 ) send_to_char( "wearing clothes.\n\r",  ch );
    else if ( GET_AC(ch) >=   40 ) send_to_char( "slightly armored.\n\r", ch );
    else if ( GET_AC(ch) >=   20 ) send_to_char( "somewhat armored.\n\r", ch );
    else if ( GET_AC(ch) >=    0 ) send_to_char( "armored.\n\r",          ch );
    else if ( GET_AC(ch) >= - 20 ) send_to_char( "well armored.\n\r",     ch );
    else if ( GET_AC(ch) >= - 40 ) send_to_char( "strongly armored.\n\r", ch );
    else if ( GET_AC(ch) >= - 60 ) send_to_char( "heavily armored.\n\r",  ch );
    else if ( GET_AC(ch) >= - 80 ) send_to_char( "superbly armored.\n\r", ch );
    else if ( GET_AC(ch) >= -100 ) send_to_char( "divinely armored.\n\r", ch );
    else                           send_to_char( "invincible!\n\r",       ch );

    if ( ch->top_level >= 15 )
	ch_printf( ch, "Hitroll: %d  Damroll: %d.\n\r",
	    GET_HITROLL(ch), GET_DAMROLL(ch) );

    if ( ch->top_level >= 10 )
	ch_printf( ch, "Alignment: %d.  ", ch->alignment );

    send_to_char( "You are ", ch );
	 if ( ch->alignment >  900 ) send_to_char( "angelic.\n\r", ch );
    else if ( ch->alignment >  700 ) send_to_char( "saintly.\n\r", ch );
    else if ( ch->alignment >  350 ) send_to_char( "good.\n\r",    ch );
    else if ( ch->alignment >  100 ) send_to_char( "kind.\n\r",    ch );
    else if ( ch->alignment > -100 ) send_to_char( "neutral.\n\r", ch );
    else if ( ch->alignment > -350 ) send_to_char( "mean.\n\r",    ch );
    else if ( ch->alignment > -700 ) send_to_char( "evil.\n\r",    ch );
    else if ( ch->alignment > -900 ) send_to_char( "demonic.\n\r", ch );
    else                             send_to_char( "satanic.\n\r", ch );

    if ( ch->first_affect )
    {
	send_to_char( "You are affected by:\n\r", ch );
	for ( paf = ch->first_affect; paf; paf = paf->next )
	    if ( (skill=get_skilltype(paf->type)) != NULL )
	{
	    ch_printf( ch, "Spell: '%s'", skill->name );

	    if ( ch->top_level >= 20 )
		ch_printf( ch,
		    " modifies %s by %d for %d rounds",
		    affect_loc_name( paf->location ),
		    paf->modifier,
		    paf->duration );

	    send_to_char( ".\n\r", ch );
	}
    }

    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
    {
	ch_printf( ch, "WizInvis level: %d   WizInvis is %s\n\r",
			ch->pcdata->wizinvis,
			xIS_SET( ch->act, PLR_WIZINVIS ) ? "ON" : "OFF" );
	if ( ch->pcdata->r_range_lo && ch->pcdata->r_range_hi )
	  ch_printf( ch, "Room Range: %d - %d\n\r", ch->pcdata->r_range_lo,
					 	   ch->pcdata->r_range_hi	);
	if ( ch->pcdata->o_range_lo && ch->pcdata->o_range_hi )
	  ch_printf( ch, "Obj Range : %d - %d\n\r", ch->pcdata->o_range_lo,
	  					   ch->pcdata->o_range_hi	);
	if ( ch->pcdata->m_range_lo && ch->pcdata->m_range_hi )
	  ch_printf( ch, "Mob Range : %d - %d\n\r", ch->pcdata->m_range_lo,
	  					   ch->pcdata->m_range_hi	);
    }

    return;
}

/*								-Thoric
 * Display your current exp, level, and surrounding level exp requirements
 */
void do_level( CHAR_DATA *ch, char *argument )
{ 
       int ability;
    
       for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
          if ( ability != FORCE_ABILITY )
	  {
		  if(ch->skill_level[ability] > 2 || max_level(ch, ability) <= 2 || IS_IMMORTAL(ch))
		  {
			ch_printf( ch, "&w%-15s &CL&cevel&W: &W%-3d    &CM&cax&W: &w%-3d    &CE&cxp&W: %s\n\r", ability_name[ability], ch->skill_level[ability], max_level(ch, ability),
					exp_bar(ch, ability) );
		  }
		  else	// Put a '?' for Max if player hasn't reached level 3 in the class.
		  {
			  ch_printf( ch, "&w%-15s &CL&cevel&W: &W%-3d    &CM&cax&W: &w%-3s    &CE&cxp&W: %s\n\r", ability_name[ability], ch->skill_level[ability], "?",
					exp_bar(ch, ability) );
		  }
	  }
}

void do_oochistory(CHAR_DATA *ch, char *argument)
{
	int i;
	if(ooc_history.size == 0)
		send_to_char("&RThere are no ooc messages in the history to show.\n\r", ch);
	else
		ch_printf(ch, "&GShowing last &W%d &Gooc chat%s&Y:&W\n\r\n\r", ooc_history.size, ooc_history.size == 1 ? "" : "s");
	for(i = 0; i < ooc_history.size; ++i)
	{
		send_to_char(ooc_history.messages[i], ch);
		send_to_char("\n", ch);
	}
	ch_printf(ch, "\n\r");
	return;
}

void do_locker( CHAR_DATA * ch, char *argument )
{
   FILE *fp = NULL;
   char strsave[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   int locker_cost = 1000;
   OBJ_DATA *locker;
   bool foot = FALSE;

   if( IS_NPC( ch ) )
      return;

   argument = one_argument( argument, arg );

   if( IS_SET( ch->in_room->room_flags, ROOM_PLR_HOME ) )
      foot = TRUE;

   if( arg[0] == '\0' )
   {
      if( !foot )
      {
         send_to_char( "\n\rSyntax: LOCKER <purchase | open | close>\r\n", ch );
         ch_printf( ch, " Lockers may be purchased for %d credit(s).\n\r", locker_cost );
      }
      else
      {
         send_to_char( "\n\rSyntax: LOCKER <open | close>\r\n", ch );
      }
      return;
   }

   if( !foot && !IS_SET( ch->in_room->room_flags2, ROOM_LOCKER ) )
   {
      send_to_char( "You may only access lockers from locker rooms.\r\n", ch );
      return;
   }

   if( !foot && !str_cmp( arg, "purchase" ) )
   {
      for( locker = ch->in_room->first_content; locker; locker = locker->next_content )
      {
         if( locker->name == ch->name )
            break;
      }

      if( locker )
      {
         send_to_char( "&RYou already own a locker here.\n\r", ch );
         return;
      }

      sprintf( strsave, "%s%s.%d", LOCKER_DIR, capitalize( ch->name ), ch->in_room->vnum );

      if( ( fp = fopen( strsave, "r" ) ) != NULL )
      {
         send_to_char( "&RYou already own a locker here!\n\r", ch );
         fclose( fp );
         return;
      }
      else
      {
         if( ch->gold < locker_cost )
         {
            send_to_char( "You cannot afford to purchase a locker right now!\n\r", ch );
            return;
         }

         if( get_obj_index( OBJ_VNUM_LOCKER ) == NULL )
         {
            bug( "[ALERT]: Unable to locate OBJ_VNUM_LOCKER. Cannot create locker." );
            send_to_char( "All the lockers here seem to be owned!\n\r", ch );
            return;
         }

         ch->gold -= locker_cost;

         locker = create_object( get_obj_index( OBJ_VNUM_LOCKER ), 0 );
         sprintf( buf, locker->name, ch->name );
         STRFREE( locker->name );
         locker->name = STRALLOC( buf );
         sprintf( buf, locker->short_descr, ch->name );
         STRFREE( locker->short_descr );
         locker->short_descr = STRALLOC( buf );
         sprintf( buf, locker->description, ch->name );
         STRFREE( locker->description );
         locker->description = STRALLOC( buf );
         obj_to_room( locker, ch->in_room );
      }
      act( AT_ACTION, "$n opens $s new locker.", ch, NULL, NULL, TO_ROOM );
      act( AT_ACTION, "You open your new locker.", ch, NULL, NULL, TO_CHAR );
   }
   else if( !str_cmp( arg, "open" ) )
   {
      sprintf( buf, "locker %d", ch->in_room->vnum );
      for( locker = ch->in_room->first_content; locker; locker = locker->next_content )
      {
         if( !foot && locker->name == ch->name )
            break;
         if( foot && !str_cmp( locker->name, buf ) )
            break;
      }

      if( locker )
      {
         if( foot )
            send_to_char( "The locker is already open.\n\r", ch );
         if( !foot )
            send_to_char( "Your locker is already open.\n\r", ch );
         return;
      }

      if( foot )
         sprintf( strsave, "%shome.%d", LOCKER_DIR, ch->in_room->vnum );
      if( !foot )
         sprintf( strsave, "%s%s.%d", LOCKER_DIR, capitalize( ch->name ), ch->in_room->vnum );

      if( ( fp = fopen( strsave, "r" ) ) != NULL )
      {
         for( ;; )
         {
            char letter;
            char *word;

            letter = fread_letter( fp );
            if( letter == '#' )
            {
               word = fread_word( fp );

               if( !strcmp( word, "END" ) )
                  break;

               if( !strcmp( word, "OBJECT" ) )
               {
                  fread_obj( ch, fp, OS_LOCKER );
               }
            }
         }
         fclose( fp );
      }
      else
      {
         if( !foot )
         {
            send_to_char( "You don't currently own a locker here!\n\r", ch );
            ch_printf( ch, "If you wish, you can purchase one for %d credit(s).\n\r", locker_cost );
         }
         else
         {
            sprintf( strsave, "%shome.%d", LOCKER_DIR, ch->in_room->vnum );

            if( ( fp = fopen( strsave, "r" ) ) != NULL )
            {
               fclose( fp );
               return;
            }
            else
            {
               if( get_obj_index( OBJ_VNUM_LOCKER ) == NULL )
               {
                  bug( "[ALERT]: Unable to locate OBJ_VNUM_LOCKER. Cannot create locker." );
                  send_to_char( "All the lockers here seem to be owned!\n\r", ch );
                  return;
               }

               locker = create_object( get_obj_index( OBJ_VNUM_LOCKER ), 0 );
               sprintf( buf, "locker %d", ch->in_room->vnum );
               STRFREE( locker->name );
               locker->name = STRALLOC( buf );
               sprintf( buf, locker->short_descr, "Someone" );
               STRFREE( locker->short_descr );
               locker->short_descr = STRALLOC( buf );
               sprintf( buf, locker->description, "Someone" );
               STRFREE( locker->description );
               locker->description = STRALLOC( buf );
               obj_to_room( locker, ch->in_room );
            }
            act( AT_ACTION, "$n opens the locker.", ch, NULL, NULL, TO_ROOM );
            act( AT_ACTION, "You open the locker.", ch, NULL, NULL, TO_CHAR );
         }
         return;
      }
      act( AT_ACTION, "$n opens $s locker.", ch, NULL, NULL, TO_ROOM );
      act( AT_ACTION, "You open your locker.", ch, NULL, NULL, TO_CHAR );
   }
   else if( !str_cmp( arg, "close" ) )
   {
      sprintf( buf, "locker %d", ch->in_room->vnum );
      for( locker = ch->in_room->first_content; locker; locker = locker->next_content )
      {
         if( !foot && locker->name == ch->name )
            break;
         if( foot && !str_cmp( locker->name, buf ) )
            break;
      }

      if( !locker )
      {
         send_to_char( "That locker is not open.\n\r", ch );
         return;
      }
      else
      {
         act( AT_ACTION, "$n closes the locker.", ch, NULL, NULL, TO_ROOM );
         act( AT_ACTION, "You close the locker.", ch, NULL, NULL, TO_CHAR );
         fwrite_locker( ch, locker, foot );
         extract_obj( locker );
      }
   }
   return;
}

void fwrite_locker( CHAR_DATA * ch, OBJ_DATA * locker, bool foot )
{
   /*
    * Variables
    */
   FILE *fp = NULL;
   char strsave[MAX_INPUT_LENGTH];

   if( !locker )
   {
      bug( "Fwrite_locker: NULL object.", 0 );
      bug( ch->name, 0 );
      return;
   }

   if( foot )
      sprintf( strsave, "%shome.%d", LOCKER_DIR, ch->in_room->vnum );
   if( !foot )
      sprintf( strsave, "%s%s.%d", LOCKER_DIR, capitalize( ch->name ), ch->in_room->vnum );

   if( ( fp = fopen( strsave, "w" ) ) != NULL )
   {
      fwrite_obj( ch, locker, fp, 0, OS_LOCKER, FALSE );
      fprintf( fp, "#END \n\r" );
      fclose( fp );
   }
   return;
}

char *exp_bar( CHAR_DATA * ch, int num )
{
   static char buf[MAX_STRING_LENGTH];
   int ability = num;
   int level = ch->skill_level[ability];

   int max_exp = ( level ) * ( level ) * ( 500 );
   int bexp[19];
   int cur = ch->experience[ability];

   bexp[0] = ( level ) * ( level ) * ( 25 );
   bexp[1] = ( level ) * ( level ) * ( 50 );
   bexp[2] = ( level ) * ( level ) * ( 75 );
   bexp[3] = ( level ) * ( level ) * ( 100 );
   bexp[4] = ( level ) * ( level ) * ( 125 );
   bexp[5] = ( level ) * ( level ) * ( 150 );
   bexp[6] = ( level ) * ( level ) * ( 175 );
   bexp[7] = ( level ) * ( level ) * ( 200 );
   bexp[8] = ( level ) * ( level ) * ( 225 );
   bexp[9] = ( level ) * ( level ) * ( 250 );
   bexp[10] = ( level ) * ( level ) * ( 275 );
   bexp[11] = ( level ) * ( level ) * ( 300 );
   bexp[12] = ( level ) * ( level ) * ( 325 );
   bexp[13] = ( level ) * ( level ) * ( 350 );
   bexp[14] = ( level ) * ( level ) * ( 375 );
   bexp[15] = ( level ) * ( level ) * ( 400 );
   bexp[16] = ( level ) * ( level ) * ( 425 );
   bexp[17] = ( level ) * ( level ) * ( 450 );
   bexp[18] = ( level ) * ( level ) * ( 475 );
   bexp[19] = ( level ) * ( level ) * ( 499 );


   if( cur < bexp[0] )
      sprintf( buf, "&c[&w-------------------&c]" );
   if( cur >= bexp[0] && cur < bexp[1] )
      sprintf( buf, "&c[&r|&w------------------&c]" );
   if( cur >= bexp[1] && cur < bexp[2] )
      sprintf( buf, "&c[&r||&w-----------------&c]" );
   if( cur >= bexp[2] && cur < bexp[3] )
      sprintf( buf, "&c[&r|||&w----------------&c]" );
   if( cur >= bexp[3] && cur < bexp[4] )
      sprintf( buf, "&c[&R||||&w---------------&c]" );
   if( cur >= bexp[4] && cur < bexp[5] )
      sprintf( buf, "&c[&R|||||&w--------------&c]" );
   if( cur >= bexp[5] && cur < bexp[6] )
      sprintf( buf, "&c[&R||||||&w-------------&c]" );
   if( cur >= bexp[6] && cur < bexp[7] )
      sprintf( buf, "&c[&O|||||||&w------------&c]" );
   if( cur >= bexp[7] && cur < bexp[8] )
      sprintf( buf, "&c[&O||||||||&w-----------&c]" );
   if( cur >= bexp[8] && cur < bexp[9] )
      sprintf( buf, "&c[&O|||||||||&w----------&c]" );
   if( cur >= bexp[9] && cur < bexp[10] )
      sprintf( buf, "&c[&Y||||||||||&w---------&c]" );
   if( cur >= bexp[10] && cur < bexp[11] )
      sprintf( buf, "&c[&Y|||||||||||&w--------&c]" );
   if( cur >= bexp[11] && cur < bexp[12] )
      sprintf( buf, "&c[&Y||||||||||||&w-------&c]" );
   if( cur >= bexp[12] && cur < bexp[13] )
      sprintf( buf, "&c[&g|||||||||||||&w------&c]" );
   if( cur >= bexp[13] && cur < bexp[14] )
      sprintf( buf, "&c[&g||||||||||||||&w-----&c]" );
   if( cur >= bexp[14] && cur < bexp[15] )
      sprintf( buf, "&c[&g|||||||||||||||&w----&c]" );
   if( cur >= bexp[15] && cur < bexp[16] )
      sprintf( buf, "&c[&G||||||||||||||||&w---&c]" );
   if( cur >= bexp[16] && cur < bexp[17] )
      sprintf( buf, "&c[&G|||||||||||||||||&w--&c]" );
   if( cur >= bexp[17] && cur < bexp[18] )
      sprintf( buf, "&c[&G||||||||||||||||||&w-&c]" );
   if( (cur >= bexp[18] && cur < max_exp) || (ch->skill_level[ability] >= max_level(ch, ability)))
      strcpy( buf, "&c[&W|||||||||||||||||||&c]" );


   return buf;
}

void do_affected ( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    SKILLTYPE *skill;
 
    if ( IS_NPC(ch) )
        return;

    argument = one_argument( argument, arg );

    if ( !str_cmp( arg, "by" ) )
    {
        set_char_color( AT_BLUE, ch );
        send_to_char( "\n\rImbued with:\n\r", ch );
	set_char_color( AT_OLDSCORE, ch );
	ch_printf( ch, "%s\n\r", affect_bit_name( ch->affected_by ) );
        if ( ch->top_level >= 20 )
        {
            send_to_char( "\n\r", ch );
            if ( ch->resistant > 0 )
	    {
		set_char_color ( AT_BLUE, ch );
                send_to_char( "Resistances:  ", ch );
                set_char_color( AT_OLDSCORE, ch );
                ch_printf( ch, "%s\n\r", flag_string(ch->resistant, ris_flags) );
	    }
            if ( ch->immune > 0 )
	    {
                set_char_color( AT_BLUE, ch );
                send_to_char( "Immunities:   ", ch);
                set_char_color( AT_OLDSCORE, ch );
                ch_printf( ch, "%s\n\r", flag_string(ch->immune, ris_flags) );
	    }
            if ( ch->susceptible > 0 )
	    {
                set_char_color( AT_BLUE, ch );
                send_to_char( "Suscepts:     ", ch );
		set_char_color( AT_OLDSCORE, ch );
                ch_printf( ch, "%s\n\r", flag_string(ch->susceptible, ris_flags) );
	    }
        }
	return;      
    }

    if ( !ch->first_affect )
    {
        set_char_color( AT_OLDSCORE, ch );
        send_to_char( "\n\rNo cantrip or skill affects you.\n\r", ch );
    }
    else
    {
		send_to_char( "\n\r", ch );
        for (paf = ch->first_affect; paf; paf = paf->next)
        {
			set_char_color( AT_BLUE, ch );
			send_to_char( "Affected:  ", ch );
			set_char_color( AT_OLDSCORE, ch );
			if ( ch->top_level >= 20 )
			{
				if (paf->duration < 25 ) set_char_color( AT_WHITE, ch );
				if (paf->duration < 6  ) set_char_color( AT_WHITE + AT_BLINK, ch );
				ch_printf( ch, "(%5d)   ", paf->duration );
			}
			ch_printf( ch, "%-18s\n\r", (skill=get_skilltype(paf->type)) != NULL ? skill->name: "The Force");
        }
    }

    if(IS_IMMORTAL(ch) && ch->wait_state)
     ch_printf(ch, "Your wait_state: %d", ch->wait_state);
    ch_printf(ch, "See also: aff by\n\r");

    return;
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_RED, ch );
    send_to_char( "You are carrying:\n\r", ch );
    show_list_to_char( ch->first_carrying, ch, TRUE, TRUE );
    return;
}


void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear, dam;
    bool found;
    char buf[MAX_STRING_LENGTH];
    
    set_char_color( AT_RED, ch );
    send_to_char( "You are using:\n\r", ch );
    found = FALSE;
    set_char_color( AT_OBJECT, ch );
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	for ( obj = ch->first_carrying; obj; obj = obj->next_content )
	   if ( obj->wear_loc == iWear )
	   {
		send_to_char( where_name[iWear], ch );
		if ( can_see_obj( ch, obj ) )
		{
		    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
		    strcpy( buf , "" );
		    switch ( obj->item_type )
	            {
	                default:
	                break;

	                case ITEM_CONTAINER:
	                	if(IS_SET(obj->wear_flags, ITEM_WEAR_HOLSTER1) || IS_SET(obj->wear_flags, ITEM_WEAR_HOLSTER2))
	                		strcat( buf, (obj->first_content) ? " &z(&Gfull&z)" : " &z(&Rempty&z)");
	                	send_to_char(buf, ch);
						break;
	                case ITEM_ARMOR:
	    		    if ( obj->value[1] == 0 )
	      			obj->value[1] = obj->value[0];
	    		    if ( obj->value[1] == 0 )
	      			obj->value[1] = 1;
	    		    dam = (sh_int) ((obj->value[0] * 10) / obj->value[1]);
			    if (dam >= 10) strcat( buf, " (superb) ");
			    else if (dam >=  7) strcat( buf, " (good) ");
 		            else if (dam >=  5) strcat( buf, " (worn) ");
			    else if (dam >=  3) strcat( buf, " (poor) ");
			    else if (dam >=  1) strcat( buf, " (awful) ");
			    else if (dam ==  0) strcat( buf, " (broken) ");
                  	    send_to_char( buf, ch );
	                    break;

	                 case ITEM_WEAPON:
	                   dam = INIT_WEAPON_CONDITION - obj->value[0];
	                   if (dam < 2) strcat( buf, " (superb) ");
                           else if (dam < 4) strcat( buf, " (good) ");
                           else if (dam < 7) strcat( buf, " (worn) ");
                           else if (dam < 10) strcat( buf, " (poor) ");
                           else if (dam < 12) strcat( buf, " (awful) ");
                           else if (dam ==  12) strcat( buf, " (broken) ");
                	   send_to_char( buf, ch );
	                   if (obj->value[3] == WEAPON_BLASTER )
	                   {
		            if (obj->blaster_setting == BLASTER_FULL)
	    		      ch_printf( ch, "FULL");
	  	            else if (obj->blaster_setting == BLASTER_HIGH)
	    		      ch_printf( ch, "HIGH");
	  	            else if (obj->blaster_setting == BLASTER_NORMAL)
	    		      ch_printf( ch, "NORMAL");
	  	            else if (obj->blaster_setting == BLASTER_HALF)
	    		      ch_printf( ch, "HALF");
	  	            else if (obj->blaster_setting == BLASTER_LOW)
	    		      ch_printf( ch, "LOW");
	  	            else if (obj->blaster_setting == BLASTER_STUN)
	    		      ch_printf( ch, "STUN");
	  	            ch_printf( ch, " %d", obj->value[4] );
	                   }
	                   else if (     ( obj->value[3] == WEAPON_LIGHTSABER || 
		           obj->value[3] == WEAPON_VIBRO_BLADE  
		           || obj->value[3] == WEAPON_FORCE_PIKE 
		           || obj->value[3] == WEAPON_BOWCASTER ) )
	                   {
		             ch_printf( ch, "%d", obj->value[4] );
	                   }        
	                   break;
                    }   
		    send_to_char( "\n\r", ch );
		}
		else
		    send_to_char( "something.\n\r", ch );
		found = TRUE;
	   }
    }

    if ( !found )
	send_to_char( "Nothing.\n\r", ch );

    return;
}



void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( isalpha(title[0]) || isdigit(title[0]) )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
	strcpy( buf, title );

    STRFREE( ch->pcdata->title );
    ch->pcdata->title = STRALLOC( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( IS_SET( ch->pcdata->flags, PCFLAG_NOTITLE ))
    {
        send_to_char( "You try but the Force resists you.\n\r", ch );
        return;
    }
 

    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

    if ((get_trust(ch) <= LEVEL_IMMORTAL) && (!nifty_is_name(ch->name, remand(argument))))
     {
       send_to_char("You must include your name somewhere in your title!", ch);
       return;
     }
 
    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}

void do_email( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	if ( !ch->pcdata->email )
	  ch->pcdata->email = str_dup( "" );
	ch_printf( ch, "Your email is: %s\n\r",
		show_tilde( ch->pcdata->email ) );
	return;
    }

    if ( !str_cmp( argument, "clear" ) )
    {
	if ( ch->pcdata->email )
	  DISPOSE(ch->pcdata->email);
	ch->pcdata->email = str_dup("");

	send_to_char( "Email cleared.\n\r", ch );
	return;
    }

    strcpy( buf, argument );
    
    if ( strlen(buf) > 70 )
	buf[70] = '\0';

    hide_tilde( buf );
    if ( ch->pcdata->email )
      DISPOSE(ch->pcdata->email);
    ch->pcdata->email = str_dup(buf);
    send_to_char( "Email set.\n\r", ch );
}

void do_screenname( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	if ( !ch->pcdata->screenname )
	  ch->pcdata->screenname = str_dup( "" );
	ch_printf( ch, "Your AIM screenname is: %s\n\r",
		show_tilde( ch->pcdata->screenname ) );
	return;
    }

    if ( !str_cmp( argument, "clear" ) )
    {
	if ( ch->pcdata->screenname )
	  DISPOSE(ch->pcdata->screenname);
	ch->pcdata->screenname = str_dup("");

	send_to_char( "AIM Screnname cleared.\n\r", ch );
	return;
    }

    strcpy( buf, argument );
    
    if ( strlen(buf) > 70 )
	buf[70] = '\0';

    hide_tilde( buf );
    if ( ch->pcdata->screenname )
      DISPOSE(ch->pcdata->screenname);
    ch->pcdata->screenname = str_dup(buf);
    send_to_char( "AIM Screnname set.\n\r", ch );
}

void do_homepage( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	if ( !ch->pcdata->homepage )
	  ch->pcdata->homepage = str_dup( "" );
	ch_printf( ch, "Your homepage is: %s\n\r",
		show_tilde( ch->pcdata->homepage ) );
	return;
    }

    if ( !str_cmp( argument, "clear" ) )
    {
	if ( ch->pcdata->homepage )
	  DISPOSE(ch->pcdata->homepage);
	ch->pcdata->homepage = str_dup("");
	send_to_char( "Homepage cleared.\n\r", ch );
	return;
    }

    if ( strstr( argument, "://" ) )
	strcpy( buf, argument );
    else
	sprintf( buf, "http://%s", argument );
    if ( strlen(buf) > 70 )
	buf[70] = '\0';

    hide_tilde( buf );
    if ( ch->pcdata->homepage )
      DISPOSE(ch->pcdata->homepage);
    ch->pcdata->homepage = str_dup(buf);
    send_to_char( "Homepage set.\n\r", ch );
}

void do_wwwimage( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	if ( !ch->pcdata->image )
	  ch->pcdata->image = str_dup( "" );
	ch_printf( ch, "Your www image is: %s\n\r",
		show_tilde( ch->pcdata->image ) );
	return;
    }

    if ( !str_cmp( argument, "clear" ) )
    {
	if ( ch->pcdata->image )
	  DISPOSE(ch->pcdata->image);
	ch->pcdata->image = str_dup("");
	send_to_char( "WWW Image cleared.\n\r", ch );
	return;
    }

    if ( strstr( argument, "://" ) )
	strcpy( buf, argument );
    else
	sprintf( buf, "http://%s", argument );
    if ( strlen(buf) > 70 )
	buf[70] = '\0';

    hide_tilde( buf );
    if ( ch->pcdata->image )
      DISPOSE(ch->pcdata->image);
    ch->pcdata->image = str_dup(buf);
    send_to_char( "WWW Image set.\n\r", ch );
}


/*
 * Set your personal description				-Thoric
 */
void do_description( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Monsters are too dumb to do that!\n\r", ch );
	return;	  
    }

    if ( !ch->desc )
    {
	bug( "do_description: no descriptor", 0 );
	return;
    }

    switch( ch->substate )
    {
	default:
	   bug( "do_description: illegal substate", 0 );
	   return;

	case SUB_RESTRICTED:
	   send_to_char( "You cannot use this command from within another command.\n\r", ch );
	   return;

	case SUB_NONE:
	   ch->substate = SUB_PERSONAL_DESC;
	   ch->dest_buf = ch;
	   start_editing( ch, ch->description );
	   editor_desc_printf( ch, "Your description (%s)", ch->name );
	   return;

	case SUB_PERSONAL_DESC:
	   STRFREE( ch->description );
	   ch->description = copy_buffer( ch );
	   stop_editing( ch );
	   return;	
    }
}

/* Ripped off do_description for whois bio's -- Scryn*/
void do_bio( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs can't set bio's!\n\r", ch );
	return;	  
    }

    if ( !ch->desc )
    {
	bug( "do_bio: no descriptor", 0 );
	return;
    }

    switch( ch->substate )
    {
	default:
	   bug( "do_bio: illegal substate", 0 );
	   return;
	  	   
	case SUB_RESTRICTED:
	   send_to_char( "You cannot use this command from within another command.\n\r", ch );
	   return;

	case SUB_NONE:
	   ch->substate = SUB_PERSONAL_BIO;
	   ch->dest_buf = ch;
	   start_editing( ch, ch->pcdata->bio );
	   editor_desc_printf( ch, "Your bio (%s).", ch->name );
	   return;

	case SUB_PERSONAL_BIO:
	   STRFREE( ch->pcdata->bio );
	   ch->pcdata->bio = copy_buffer( ch );
	   stop_editing( ch );
	   return;	
    }
}



void do_dreport( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    if ( IS_AFFECTED(ch, AFF_POSSESS) )
    {   
       send_to_char("You can't do that in your current state of mind!\n\r", ch);
       return;
    }
   
      send_to_char("You report your information.", ch);
    
      sprintf( buf, "$n reports: %d/%d hp %d/%d.\n\r", ch->hit,  ch->max_hit, ch->move, ch->max_move   );
      sprintf( buf, "Credits:%d", ch->gold );

    act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );

    return;
}

void do_report( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    if ( IS_AFFECTED(ch, AFF_POSSESS) )
    {   
       send_to_char("You can't do that in your current state of mind!\n\r", ch);
       return;
    }

    
      ch_printf( ch, "%sYou report: %d/%d hp %d/%d mv AC: %d.\n\r",
	color_str( AT_REPORT, ch), ch->hit,  ch->max_hit,
	ch->move, ch->max_move, GET_AC(ch)   );

      sprintf( buf, "$n reports:  %d/%d hp %d/%d mv AC: %d.\n\r",
	ch->hit,  ch->max_hit,
	ch->move, ch->max_move, GET_AC(ch)   );

    act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );

    return;
}

void do_prompt( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  
  if ( IS_NPC(ch) )
  {
    send_to_char( "NPC's can't change their prompt..\n\r", ch );
    return;
  }
  smash_tilde( argument );
  one_argument( argument, arg );
  if ( !*arg )
  {
    send_to_char( "Set prompt to what? (try help prompt)\n\r", ch );
    return;
  }
  if (ch->pcdata->prompt)
    STRFREE(ch->pcdata->prompt);

  if ( strlen(argument) > 128 )
    argument[128] = '\0';

  /* Can add a list of pre-set prompts here if wanted.. perhaps
     'prompt 1' brings up a different, pre-set prompt */
  if ( !str_cmp(arg, "default") )
    ch->pcdata->prompt = STRALLOC("");
  else
    ch->pcdata->prompt = STRALLOC(argument);
  send_to_char( "Ok.\n\r", ch );
  return;
}


void do_memorycheck( CHAR_DATA *ch, char *argument )
{
   FELLOW_DATA *fellow = NULL;
   int cnt = 0;

   for(fellow=ch->first_fellow;fellow;fellow=fellow->next)
   {
       cnt++;
       ch_printf( ch, "&z[&C%-2d&z]&w %s\r\n", cnt, fellow->knownas);
   }
   if( cnt == 0 )
      send_to_char( "&RYou don't know anyone.\r\n", ch );
   return;
}

