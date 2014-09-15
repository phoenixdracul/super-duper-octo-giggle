/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 						      	Feats System					  		  	*
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

 // TODO First
void do_stance( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	int iStance, stance;

	argument = one_argument(argument, arg);

	if( arg[0] == '\0' )
	{
		send_to_char( "\n\r", ch );
		send_to_char( "&bStances:&w\n\r", ch );
		if( !HAS_STANCE(ch, ECHANI_BASIC) )
			send_to_char( "&wEchani, ", ch );
		else if( HAS_STANCE(ch, ECHANI_MASTERY) )
			send_to_char( "&rEchani&w, ", ch );
		else if( HAS_STANCE(ch, ECHANI_EXPERTISE) )
			send_to_char( "&YEchani&w, ", ch );
		else if( HAS_STANCE(ch, ECHANI_BASIC) )
			send_to_char( "&gEchani&w, ", ch );
		if( !HAS_STANCE(ch, KTARA_BASIC) )
			send_to_char( "&wK'tara, ", ch );
		else if( HAS_STANCE(ch, KTARA_MASTERY) )
			send_to_char( "&rK'tara&w, ", ch );
		else if( HAS_STANCE(ch, KTARA_EXPERTISE) )
			send_to_char( "&YK'tara&w, ", ch );
		else if( HAS_STANCE(ch, KTARA_BASIC) )
			send_to_char( "&gK'tara&w, ", ch );
		if( !HAS_STANCE(ch, KTHRI_BASIC) )
			send_to_char( "&wK'thri, ", ch );
		else if( HAS_STANCE(ch, KTHRI_MASTERY) )
			send_to_char( "&rK'thri&w, ", ch );
		else if( HAS_STANCE(ch, KTHRI_EXPERTISE) )
			send_to_char( "&YK'thri&w, ", ch );
		else if( HAS_STANCE(ch, KTHRI_BASIC) )
			send_to_char( "&gK'thri&w, ", ch );
		if( !HAS_STANCE(ch, STAVA_BASIC) )
			send_to_char( "&wStava, ", ch );
		else if( HAS_STANCE(ch, STAVA_MASTERY) )
			send_to_char( "&rStava&w, ", ch );
		else if( HAS_STANCE(ch, STAVA_EXPERTISE) )
			send_to_char( "&YStava&w, ", ch );
		else if( HAS_STANCE(ch, STAVA_BASIC) )
			send_to_char( "&wStava&w, ", ch );
		if( !HAS_STANCE(ch, WRRUUSHI_BASIC) )
			send_to_char( "&wWrruushi\n\r", ch );
		else if( HAS_STANCE(ch, WRRUUSHI_MASTERY) )
			send_to_char( "&rWrruushi&w\n\r", ch );
		else if( HAS_STANCE(ch, WRRUUSHI_EXPERTISE) )
			send_to_char( "&YWrruushi&w\n\r", ch );
		else if( HAS_STANCE(ch, WRRUUSHI_BASIC) )
			send_to_char( "&gWrruushi&w\n\r", ch );
		if( IS_IMMORTAL(ch) || HAS_FEAT2(ch, WEAP_PROF_LIGHTSABER) )
		{
			send_to_char( "\n\r", ch );
			send_to_char( "&bLightsaber Stances:&w\n\r", ch );
			if( !HAS_STANCE( ch, FORM1_MASTERY ) )
				send_to_char( "&wForm One, ", ch );
			else
				send_to_char( "&gForm One&w, ", ch );
			if( !HAS_STANCE( ch, FORM2_MASTERY ) )
				send_to_char( "&wForm Two, ", ch );
			else
				send_to_char( "&gForm Two&w, ", ch );
			if( !HAS_STANCE( ch, FORM3_MASTERY ) )
				send_to_char( "&wForm Three, ", ch );
			else
				send_to_char( "&gForm Three&w, ", ch );
			if( !HAS_STANCE( ch, FORM4_MASTERY ) )
				send_to_char( "&wForm Four, ", ch );
			else
				send_to_char( "&gForm Four&w, ", ch );
			if( !HAS_STANCE( ch, FORM5_MASTERY ) )
				send_to_char( "&wForm Five, ", ch );
			else
				send_to_char( "&gForm Five&w, ", ch );
			if( !HAS_STANCE( ch, FORM6_MASTERY ) )
				send_to_char( "&wForm Six, ", ch );
			else
				send_to_char( "&gForm Six&w, ", ch );
			if( !HAS_STANCE( ch, FORM7_MASTERY ) )
				send_to_char( "&wForm Seven\n\r", ch );
			else
				send_to_char( "&gForm Seven&w\n\r", ch );
		}
		send_to_char( "\n\r", ch );
		if( ch->stance == STANCE_NONE )
		{
			send_to_char( "&wYou are currently not in a stance.\n\r", ch );
			return;
		}
		ch_printf( ch, "&wYou are currently utilizing the &g%s&w stance.\n\r", stance_name[ch->stance] );   
		return;
	}
	if( !str_cmp( arg, "learn" ) )
	{
		if( argument[0] == '\0' )
		{
			send_to_char( "&wLearn which stance?\n\r", ch );
			return;
		}
		if( ch->unused_feats < 1 )
		{
			send_to_char( "&wLearning stances requires free feat points, which you do not have.\n\r", ch );
			return;
		}
		stance = -1;
		for( iStance = 0; iStance < MAX_STANCE; iStance++ )
		{
			if ( !str_prefix( argument , stance_name[iStance] ) )
			{
				stance = iStance;
				break;
			}
		}
		if( stance == -1 || stance == STANCE_NONE )
		{
			send_to_char( "&wThat is not a stance, which do you want to learn?\n\r", ch );
			return;
		}
		if( stance == ECHANI_STANCE )
		{
			if( HAS_STANCE( ch, ECHANI_MASTERY ) )
			{
				send_to_char( "&wYou are already a &rMaster&w of the &bEchani&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE( ch, ECHANI_MASTERY ) && HAS_STANCE( ch, ECHANI_EXPERTISE ) )
			{
				SET_BIT(ch->has_stance, ECHANI_MASTERY);
				ch->unused_feats -= 1;
				send_to_char( "&wYou have now become a &rMaster&w of the &bEchani&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE( ch, ECHANI_EXPERTISE ) && HAS_STANCE( ch, ECHANI_BASIC ) )
			{
				SET_BIT(ch->has_stance, ECHANI_EXPERTISE);
				ch->unused_feats -= 1;
				send_to_char( "&wYou have now become an &rExpert&w of the &bEchani&w stance.\n\r", ch );
				return;
			}
			SET_BIT(ch->has_stance, ECHANI_BASIC);
			ch->unused_feats -= 1;
			send_to_char( "&wYou have now become an &rApprentice&w of the &bEchani&w stance.\n\r", ch );
			return;
		}
		if( stance == KTARA_STANCE )
		{
			if( HAS_STANCE(ch, KTARA_MASTERY) )
			{
				send_to_char( "&wYou are already a &rMaster&w of the &bK'tara&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE(ch, KTARA_BASIC) )
			{
				ch->unused_feats -= 1;
				SET_BIT(ch->has_stance, KTARA_BASIC);
				send_to_char( "&wYou have now become an &rApprentice&w in the &bK'tara&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE(ch, KTARA_EXPERTISE) )
			{
				SET_BIT(ch->has_stance, KTARA_EXPERTISE);
				ch->unused_feats -= 1;
				send_to_char( "&wYou have now become an &rExpert&w in the &bK'tara&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE(ch, KTARA_MASTERY) )
			{
				SET_BIT(ch->has_stance, KTARA_MASTERY);
				ch->unused_feats -= 1;
				send_to_char( "&wYou have now become an &rMaster&w in the &bK'tara&w stance.\n\r", ch );
				return;
			}
		}
		if( stance == KTHRI_STANCE )
		{
			if( HAS_STANCE(ch, KTHRI_MASTERY) )
			{
				send_to_char( "&wYou are already a &rMaster&w in the &bK'thri&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE(ch, KTHRI_BASIC) )
			{
				ch->unused_feats -= 1;
				SET_BIT(ch->has_stance, KTHRI_BASIC);
				send_to_char( "&wYou have now become an &rApprentice&w in the &bK'thri&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE(ch, KTHRI_EXPERTISE) )
			{
				SET_BIT(ch->has_stance, KTHRI_EXPERTISE);
				ch->unused_feats -= 1;
				send_to_char( "&wYou have now become an &rExpert&w in the &bK'thri&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE(ch, KTHRI_MASTERY) )
			{
				SET_BIT(ch->has_stance, KTHRI_MASTERY);
				ch->unused_feats -= 1;
				send_to_char( "&wYou have now become a &rMaster&w in the &bK'thri&w stance.\n\r", ch );
				return;
			}
		}
		if( stance == STAVA_STANCE )
		{
			if( HAS_STANCE(ch, STAVA_MASTERY) )
			{
				send_to_char( "&wYou are already a &rMaster&w in the &bStava&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE(ch, STAVA_BASIC) )
			{
				ch->unused_feats -= 1;
				SET_BIT(ch->has_stance, STAVA_BASIC);
				send_to_char( "&wYou have now become an &rApprentice&w in the &bStava&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE(ch, STAVA_EXPERTISE) )
			{
				SET_BIT(ch->has_stance, STAVA_EXPERTISE);
				ch->unused_feats -= 1;
				send_to_char( "&wYou have now become an &rExpert&w in the &bStavaw stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE(ch, STAVA_MASTERY) )
			{
				SET_BIT(ch->has_stance, STAVA_MASTERY);
				ch->unused_feats -= 1;
				send_to_char( "&wYou have now become a &rMaster&w in the &bStava&w stance.\n\r", ch );
				return;
			}
		}
		if( stance == WRRUUSHI_STANCE )
		{
			if( ch->race != RACE_WOOKIEE )
			{
				send_to_char( "&wOnly &YWookiees&w can learn the &bWrruushi&w fighting stance.\n\r", ch );
				return;
			}
			if( HAS_STANCE(ch, WRRUUSHI_MASTERY) )
			{
				send_to_char( "&wYou are already a &rMaster&w in the &bWrruushi&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE(ch, WRRUUSHI_BASIC) )
			{
				ch->unused_feats -= 1;
				SET_BIT(ch->has_stance, WRRUUSHI_BASIC);
				send_to_char( "&wYou have now become an &rApprentice&w in the &bWrruushi&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE(ch, WRRUUSHI_EXPERTISE) )
			{
				SET_BIT(ch->has_stance, WRRUUSHI_EXPERTISE);
				ch->unused_feats -= 1;
				send_to_char( "&wYou have now become an &rExpert&w in the &bWrruushi&w stance.\n\r", ch );
				return;
			}
			if( !HAS_STANCE(ch, WRRUUSHI_MASTERY) )
			{
				SET_BIT(ch->has_stance, WRRUUSHI_MASTERY);
				ch->unused_feats -= 1;
				send_to_char( "&wYou have now become a &rMaster&w in the &bWrruushi&w stance.\n\r", ch );
				return;
			}
		}
		if( !HAS_FEAT2(ch, WEAP_PROF_LIGHTSABER) && ( stance == FORM1_STANCE || stance == FORM2_STANCE || stance == FORM3_STANCE
		|| stance == FORM4_STANCE || stance == FORM5_STANCE || stance == FORM6_STANCE || stance == FORM7_STANCE ) )
		{
			send_to_char( "&wOnly those proficient with Lightsabers may take Form stances.\n\r", ch );
			return;
		}
		if( stance == FORM1_STANCE )
		{
			if( HAS_STANCE(ch, FORM1_STANCE) )
			{
				send_to_char( "&wYou already possess this stance.\n\r", ch );
				return;
			}
			if( !HAS_FFEAT(ch, CONTROL_FEAT) || !HAS_FFEAT(ch, ATTUNED_FEAT) || ch->top_level < 7 )
			{
				send_to_char( "&wYou lack one of the requirements for this stance.\n\r", ch );
				return;
			}
			SET_BIT(ch->has_stance, FORM1_STANCE);
			ch->unused_feats -= 1;
			send_to_char( "&wYou are now a &rMaster&w with &bLightsaber Form One&w.\n\r", ch );
			return;
		}
		if( stance == FORM2_STANCE )
		{
			if( HAS_STANCE(ch, FORM2_STANCE) )
			{
				send_to_char( "&wYou already possess this stance.\n\r", ch );
				return;
			}
			if( !HAS_FFEAT(ch, CONTROL_FEAT) || get_curr_dex(ch) < 15 || ch->top_level < 11 )
			{
				send_to_char( "&wYou lack one of the requirements for this stance.\n\r", ch );
				return;
			}
			SET_BIT(ch->has_stance, FORM2_STANCE);
			ch->unused_feats -= 1;
			send_to_char( "&wYou are now a &rMaster&w with &bLightsaber Form Two&w.\n\r", ch );
			return;
		}
		if( stance == FORM3_STANCE )
		{
			if( HAS_STANCE(ch, FORM3_STANCE) )
			{
				send_to_char( "&wYou already possess this stance.\n\r", ch );
				return;
			}
			if( !HAS_FFEAT(ch, CONTROL_FEAT) || !HAS_FFEAT(ch, LIGHTSABER_DEFENSE) || !HAS_FEAT(ch, DODGE_FEAT) || ch->top_level < 7 )
			{
				send_to_char( "&wYou lack one of the requirements for this stance.\n\r", ch );
				return;
			}
			SET_BIT(ch->has_stance, FORM3_STANCE);
			ch->unused_feats -= 1;
			send_to_char( "&wYou are now a &rMaster&w with &bLightsaber Form Three&w.\n\r", ch );
			return;
		}
		if( stance == FORM4_STANCE )
		{
			if( HAS_STANCE(ch, FORM4_STANCE) )
			{
				send_to_char( "&wYou already possess this stance.\n\r", ch );
				return;
			}
			if( !HAS_FFEAT(ch, CONTROL_FEAT) || ch->top_level < 7 )
			{
				send_to_char( "&wYou lack one of the requirements for this stance.\n\r", ch );
				return;
			}
			SET_BIT(ch->has_stance, FORM4_STANCE);
			ch->unused_feats -= 1;
			send_to_char( "&wYou are now a &rMaster&w with &bLightsaber Form Four&w.\n\r", ch );
			return;
		}
		if( stance == FORM5_STANCE )
		{
			if( HAS_STANCE(ch, FORM5_STANCE) )
			{
				send_to_char( "&wYou already possess this stance.\n\r", ch );
				return;
			}
			if( !HAS_FFEAT(ch, CONTROL_FEAT) || !HAS_FFEAT(ch, LIGHTSABER_DEFENSE) || ch->top_level < 7 )
			{
				send_to_char( "&wYou lack one of the requirements for this stance.\n\r", ch );
				return;
			}
			SET_BIT(ch->has_stance, FORM5_STANCE);
			ch->unused_feats -= 1;
			send_to_char( "&wYou are now a &rMaster&w with &bLightsaber Form Five&w.\n\r", ch );
			return;
		}
		if( stance == FORM6_STANCE )
		{
			if( HAS_STANCE(ch, FORM6_STANCE) )
			{
				send_to_char( "&wYou already possess this stance.\n\r", ch );
				return;
			}
			if( !HAS_FFEAT(ch, SENSE_FEAT) || !HAS_FFEAT(ch, LINK_FEAT) || ch->top_level < 7 )
			{
				send_to_char( "&wYou lack one of the requirements for this stance.\n\r", ch );
				return;
			}
			SET_BIT(ch->has_stance, FORM6_STANCE);
			ch->unused_feats -= 1;
			send_to_char( "&wYou are now a &rMaster&w with &bLightsaber Form Six&w.\n\r", ch );
			return;
		}
		if( stance == FORM7_STANCE )
		{
			if( HAS_STANCE(ch, FORM7_STANCE) )
			{
				send_to_char( "&wYou already possess this stance.\n\r", ch );
				return;
			}
			if( get_curr_dex(ch) < 13 || !HAS_FEAT(ch, DODGE_FEAT) || !HAS_FEAT2(ch, IMP_CRITICAL_LIGHTSABER) || ch->top_level < 7 )
			{
				send_to_char( "&wYou lack one of the requirements for this stance.\n\r", ch );
				return;
			}
			SET_BIT(ch->has_stance, FORM7_STANCE);
			ch->unused_feats -= 1;
			send_to_char( "&wYou are now a &rMaster&w with &bLightsaber Form Seven&w.\n\r", ch );
			return;
		}
	}
	stance = -1;
	for( iStance = 0; iStance < MAX_STANCE; iStance++ )
	{
		if ( !str_prefix( arg , stance_name[iStance] ) )
		{
			stance = iStance;
			break;
		}
	}
	if( stance == -1 )
	{
		send_to_char( "&wThat is not a stance, which do you want to utilize?\n\r", ch );
		return;
	}
	if( stance == ECHANI_STANCE )
		if( !HAS_STANCE(ch, ECHANI_BASIC) )
		{
		send_to_char( "&wYou don't know that stance.\n\r", ch );
		return;
		}
	if( stance == KTARA_STANCE )
		if( !HAS_STANCE(ch, KTARA_STANCE) )
		{
		send_to_char( "&wYou don't know that stance.\n\r", ch );
		return;
		}
	if( stance == STAVA_STANCE )
		if( !HAS_STANCE(ch, STAVA_STANCE) )
		{
			send_to_char( "&wYou don't know that stance.\n\r", ch );
			return;
		}
	if( stance == WRRUUSHI_STANCE )
		if( !HAS_STANCE(ch, WRRUUSHI_STANCE) )
		{
			send_to_char( "&wYou don't know that stance.\n\r", ch );
			return;
		}
	if( stance == FORM1_STANCE )
		if( !HAS_STANCE(ch, FORM1_STANCE) )
		{
			send_to_char( "&wYou don't know that stance.\n\r", ch );
			return;
		}
	if( stance == FORM2_STANCE )
		if( !HAS_STANCE(ch, FORM2_STANCE) )
		{
			send_to_char( "&wYou don't know that stance.\n\r", ch );
			return;
		}
	if( stance == FORM3_STANCE )
		if( !HAS_STANCE(ch, FORM3_STANCE) )
		{
			send_to_char( "&wYou don't know that stance.\n\r", ch );
			return;
		}
	if( stance == FORM4_STANCE )
		if( !HAS_STANCE(ch, FORM4_STANCE) )
		{
			send_to_char( "&wYou don't know that stance.\n\r", ch );
			return;
		}
	if( stance == FORM5_STANCE )
		if( !HAS_STANCE(ch, FORM5_STANCE) )
		{
			send_to_char( "&wYou don't know that stance.\n\r", ch );
			return;
		}
	if( stance == FORM6_STANCE )
		if( !HAS_STANCE(ch, FORM6_STANCE) )
		{
			send_to_char( "&wYou don't know that stance.\n\r", ch );
			return;
		}
	if( stance == FORM7_STANCE )
		if( !HAS_STANCE(ch, FORM7_STANCE) )
		{
			send_to_char( "&wYou don't know that stance.\n\r", ch );
			return;
		}
	ch->stance = stance;
	ch_printf( ch, "&wYou adjust your body into the &b%s&w stance.\n\r", stance_name[ch->stance] );
	return;
}

// TODO
void racial_bonus( CHAR_DATA *ch, int iRace )
{
  if( IS_NPC(ch) )
	return;

  if( iRace == RACE_ANOMID )
   {
	ch->skill_level[REPAIR_SKILL] += 2;
	ch->skill_level[CRAFT_DROID] += 2;
	ch->gold += 10000;
	ch->skill_level[DIPLOMACY_SKILL] += 4;
   }
  if( iRace == RACE_DROID )
   	SET_BIT(ch->feats, AMBIDEXTERITY_FEAT);
  if( iRace == RACE_ARKANIAN || iRace == RACE_RYN )
   {
	ch->reputation += 3;
   	SET_BIT(ch->feats, INFAMY_FEAT);
   }
  if( iRace == RACE_BARTOKK )
	ch->armor += 1;
  if( iRace == RACE_CHAGRIAN )
 	ch->skill_level[SWIM_SKILL] += 4;
  if( iRace == RACE_DEFEL )
   {
	ch->hitroll += 1;
	ch->armor += 1;
	ch->skill_level[HIDE_SKILL] += 4;
   }
  if( iRace == RACE_HUMAN )
   {
	ch->unused_feats += 1;
	ch->skillpoints += 10;
   }
  if( iRace == RACE_HODIN )
   {
	ch->hitroll -= 1;
	ch->armor -= 1;
	ch->skill_level[HIDE_SKILL] -= 4;
	ch->skill_level[CLIMB_SKILL] += 2;
	ch->skill_level[SURVIVAL_SKILL] += 4;
   }
  if( iRace == RACE_HUTT )
   {
	ch->hitroll -= 1;
	ch->armor -= 1;
	ch->skill_level[HIDE_SKILL] -= 4;
	SET_BIT(ch->feats, INFAMY_FEAT);
	ch->reputation += 3;
   }
  if( iRace == RACE_IKTOTCHI )
   {
	ch->hitroll += 1;
	ch->armor += 1;
	ch->skill_level[PILOT_SKILL] += 2;
	SET_BIT(ch->feats, FIGHTER_OPS);
   }
  if( iRace == RACE_JAWA )
   {
	ch->armor += 1;
	ch->hitroll += 1;
	ch->skill_level[HIDE_SKILL] += 4;
	ch->skill_level[SURVIVAL_SKILL] += 2;
   }
  if( iRace == RACE_KLATOOINIAN )
   	ch->willpower += 2;
  if( iRace == RACE_MIRALUKA )
   	SET_BIT(ch->feats, FORCE_SENSITIVE);
  if( iRace == RACE_MON_CALAMARI )
   {
	ch->skill_level[SWIM_SKILL] += 4;
	ch->skill_level[CRAFT_MEDPAC] += 4;
   }
  if( iRace == RACE_MUUN )
   {
	ch->skill_level[APPRAISE_SKILL] += 2;
	ch->skill_level[DIPLOMACY_SKILL] += 4;
   }
  if( iRace == RACE_NOGHRI )
   {
	ch->hitroll += 1;
	ch->armor += 1;
	ch->skill_level[HIDE_SKILL] += 4;
   }
  if( iRace == RACE_QUARREN )
	ch->skill_level[SWIM_SKILL] += 4;
  if( iRace == RACE_RODIAN )
   {
	ch->skill_level[LISTEN_SKILL] += 2;
	ch->skill_level[SPOT_SKILL] += 2;
	ch->skill_level[SEARCH_SKILL] += 2;
	SET_BIT(ch->feats2, TRACK_FEAT);
   }
  if( iRace == RACE_SKYTRI )
   	ch->willpower += 1;
  if( iRace == RACE_SULLUSTAN )
   {
	ch->skill_level[CLIMB_SKILL] += 2;
	ch->skill_level[LISTEN_SKILL] += 2;
   }
  if( iRace == RACE_TOGORIAN )
   {
	ch->hitroll -= 1;
	ch->armor -= 1;
	ch->skill_level[HIDE_SKILL] -= 4;
	SET_BIT(ch->feats2, WEAP_PROF_SIMPLE);
   }
  if( iRace == RACE_TOGRUTA )
 	ch->skill_level[HIDE_SKILL] += 2;
  if( iRace == RACE_TRANDOSHAN )
	ch->armor += 1;
  if( iRace == RACE_TWILEK )
	ch->fortitude += 1;
  if( iRace == RACE_UBESE )
   {
	ch->skill_level[SURVIVAL_SKILL] += 2;
	SET_BIT(ch->feats, GEARHEAD_FEAT);
	ch->skill_level[COMPUTER_SKILL] += 2;
	ch->skill_level[REPAIR_SKILL] += 2;
	SET_BIT(ch->feats2, ARMOR_LIGHT);
   }
  if( iRace == RACE_UMBARAN )
   {
	ch->reputation += 2;
	SET_BIT(ch->feats, INFLUENCE_FEAT);
   }
  if( iRace == RACE_VERPINE )
   {
	ch->armor += 2;
	ch->skill_level[SPOT_SKILL] += 2;
	ch->skill_level[SEARCH_SKILL] += 2;
	ch->skill_level[REPAIR_SKILL] += 2;
	ch->skill_level[COMPUTER_SKILL] += 2;
	SET_BIT(ch->feats, GEARHEAD_FEAT);
   }
  if( iRace == RACE_WOOKIEE )
   {
	ch->skill_level[CLIMB_SKILL] += 2;
	ch->skill_level[INTIMIDATE_SKILL] += 4;
   }
  if( iRace == RACE_WOOSTOID )
	ch->skill_level[COMPUTER_SKILL] += 2;
  return;
}

// TODO
void do_dfeats( CHAR_DATA *ch, char *argument )
{
  if( IS_NPC(ch) )
 	return;
  if( !IS_IMMORTAL(ch) && !IS_DROID(ch) )
   {
	send_to_char( "Huh?\n\r", ch );
	return;
   }
  if( argument[0] == '\0' )
   {
    send_to_char("\n\r", ch );
    send_to_char( "&bAccessories:&w\n\r", ch );
    if( !HAS_DROID(ch, COMLINK_FEAT ) )
    	send_to_char( "&wComlink\n\r", ch );
    else
    	send_to_char( "&gComlink&w\n\r", ch );
    if( !HAS_DROID(ch, DIAGNOSTICS_PACKAGE) )
	send_to_char( "&wDiagnostics Package\n\r", ch );
    else
	send_to_char( "&gDiagnostics Package&w\n\r", ch );
    if( !HAS_DROID(ch, ELECTROSHOCK_PROBE) )
	send_to_char( "&wElectroshock Probe&w\n\r", ch );
    else
	send_to_char( "&gElectroshock Probe\n\r", ch );
    if( !HAS_DROID(ch, FLAMETHROWER_FEAT) )
	send_to_char( "&wFlamethrower&w\n\r", ch );
    else
	send_to_char( "&gFlamethrower\n\r", ch );
    if( !HAS_DROID(ch, INTERNAL_STORAGE) )
	send_to_char( "&wInternal Storage&w\n\r", ch );
    else
	send_to_char( "&gInternal Storage\n\r", ch );
    if( !HAS_DROID(ch, LOCKED_ACCESS) )
	send_to_char( "&wLocked Access&w\n\r", ch );
    else
	send_to_char( "&gLocked Access\n\r", ch );
    if( !HAS_DROID(ch, MAGNETIC_FEET ) )
  	send_to_char( "&wMagnetic Feet&w\n\r", ch );
    else
	send_to_char( "&gMagnetic Feet\n\r", ch );
    if( !HAS_DROID(ch, RUST_INHIBITOR) )
	send_to_char( "&wRust Inhibitor&w\n\r", ch );
    else
	send_to_char( "&gRust Inhibitor\n\r", ch );
    if( !HAS_DROID(ch, SELFDESTRUCT_SYSTEM) )
	send_to_char( "&wSelf-Destruct System&w\n\r", ch );
    else
	send_to_char( "&gSelf-Destruct System\n\r", ch );
    if( !HAS_DROID(ch, IMPROVED_SENSOR) )
	send_to_char( "&wImproved Sensor Package&w\n\r", ch );
    else
	send_to_char( "&gImproved Sensor Package\n\r", ch );
    if( !HAS_DROID(ch, INFRARED_VISION) )
	send_to_char( "&wInfrared Vision&w\n\r", ch );
    else
	send_to_char( "&gInfrared Vision\n\r", ch );
    if( !HAS_DROID(ch, MOTION_SENSORS) )
	send_to_char( "&wMotion Sensors&w\n\r", ch );
    else
	send_to_char( "&gMotion Sensors&w\n\r", ch );
    if( !HAS_DROID(ch, SONIC_SENSORS) )
	send_to_char( "&wSonic Sensors&w\n\r", ch );
    else
	send_to_char( "&gSonic Sensors\n\r", ch );
    if( !HAS_DROID(ch, DEGREE_VISION) )
	send_to_char( "&w360-Degree Vision&w\n\r", ch );
    else
	send_to_char( "&g360-Degree Vision\n\r", ch );
    if( !HAS_DROID(ch, SHIELD_GENERATOR) )
 	send_to_char( "&wShield Generator&w\n\r", ch );
    else
	send_to_char( "&gShield Generator\n\r", ch );
    if( !HAS_DROID(ch, VOCABULATOR_FEAT) )
	send_to_char( "&wVocabulator&w\n\r", ch );
    else
	send_to_char( "&gVocabulator\n\r", ch );
    send_to_char( "\n\r", ch );
    send_to_char( "&bMartial Arts Programming:&w\n\r", ch );
    if( !HAS_DROID(ch, MARTIAL_PROGRAMMING) )
	send_to_char( "&wMartial Programming&w, ", ch );
    else
	send_to_char( "&gMartial Programing, ", ch );
    if( !HAS_DROID(ch, IMP_MARTIAL_PROGRAMMING) )
	send_to_char( "&wImproved Martial&w, ", ch );
    else
	send_to_char( "&gImproved Martial, ", ch );
    if( !HAS_DROID(ch, ADV_MARTIAL_PROGRAMMING) )
	send_to_char( "&wAdvanced Martial&w\n\r", ch );
    else
	send_to_char( "&gAdvanced Martial\n\r", ch );
    send_to_char( "&bArmor Enhancements:&w\n\r", ch );
    if( !HAS_DROID(ch, ARMOR_LIGHT_DROID) )
	send_to_char( "&wLight Armor&w, ", ch );
    else
	send_to_char( "&gLight Armor, ", ch );
    if( !HAS_DROID(ch, ARMOR_MEDIUM_DROID) )
	send_to_char( "&wMedium Armor&w, ", ch );
    else
	send_to_char( "&gMedium Armor, ", ch );
    if( !HAS_DROID(ch, ARMOR_HEAVY_DROID) )
	send_to_char( "&wHeavy Armor&w\n\r", ch );
    else
	send_to_char( "&gHeavy Armor\n\r", ch );
    return;
  }

 if( !IS_SET(ch->in_room->room_flags2, ROOM_DROIDSHOP) )
  {
	send_to_char( "&wYou must be in a &bDroid Maitenance Facility&w inorder to do this.\n\r", ch );
	return;
  }

 if( !str_cmp(argument, "light armor") )
  {
	if( HAS_DROID(ch, ARMOR_LIGHT_DROID) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 5000 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 5000;
	ch->damage_reduction += 3;
	SET_BIT(ch->droid_feat, ARMOR_LIGHT_DROID);
	send_to_char( "&wYou now possess a &gLight Armor&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "medium armor") )
  {
	if( HAS_DROID(ch, ARMOR_MEDIUM_DROID) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 15000 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 15000;
	ch->damage_reduction += 2;
	SET_BIT(ch->droid_feat, ARMOR_MEDIUM_DROID);
	send_to_char( "&wYou now possess a &gMedium Armor&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "heavy armor") )
  {
	if( HAS_DROID(ch, ARMOR_HEAVY_DROID) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 50000 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 50000;
	ch->damage_reduction += 2;
	SET_BIT(ch->droid_feat, ARMOR_HEAVY_DROID);
	send_to_char( "&wYou now possess a &gHeavy Armor&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "comlink") )
  {
	if( HAS_DROID(ch, COMLINK_FEAT) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 250 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 250;
	SET_BIT(ch->droid_feat, COMLINK_FEAT);
	send_to_char( "&wYou now possess a &gComlink&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "diagnostics") || !str_cmp(argument, "diagnostics package") )
  {
	if( HAS_DROID(ch, DIAGNOSTICS_PACKAGE) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 250 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 250;
	ch->skill_level[REPAIR_SKILL] += 4;
	SET_BIT(ch->droid_feat, DIAGNOSTICS_PACKAGE);
	send_to_char( "&wYou now possess a &gDiagnostics&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "electroshock") || !str_cmp(argument, "electroshock probe") )
  {
	if( HAS_DROID(ch, ELECTROSHOCK_PROBE) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 400 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 400;
	SET_BIT(ch->droid_feat, ELECTROSHOCK_PROBE);
	send_to_char( "&wYou now possess an &gElectroshock Probe&w.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "flamethrower") )
  {
	if( HAS_DROID(ch, FLAMETHROWER_FEAT) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 400 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 400;
	SET_BIT(ch->droid_feat, FLAMETHROWER_FEAT);
	send_to_char( "&wYou now possess a &gFlamethrower&w.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "internal storage") )
  {
	if( HAS_DROID(ch, INTERNAL_STORAGE) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 150000 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 150000;
	SET_BIT(ch->droid_feat, INTERNAL_STORAGE);
	send_to_char( "&wYou now possess an &gInternal Storage&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "locked access") )
  {
	if( HAS_DROID(ch, LOCKED_ACCESS) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 50 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 50;
	SET_BIT(ch->droid_feat, LOCKED_ACCESS);
	send_to_char( "&wYou now possess a &gLocked Access&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "magnetic feet") )
  {
	if( HAS_DROID(ch, MAGNETIC_FEET) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 100 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 100;
	SET_BIT(ch->droid_feat, MAGNETIC_FEET);
	send_to_char( "&wYou now possess a &gMagnetic Feet&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "rust inhibitor") )
  {
	if( HAS_DROID(ch, RUST_INHIBITOR) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 250 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 250;
	SET_BIT(ch->droid_feat, RUST_INHIBITOR);
	send_to_char( "&wYou now possess prevention against &grust&w.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "improved sensor") || !str_cmp(argument, "improved sensor package") )
  {
	if( HAS_DROID(ch, IMPROVED_SENSOR) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 1000 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 1000;
	SET_BIT(ch->droid_feat, IMPROVED_SENSOR);
	ch->skill_level[LISTEN_SKILL] += 2;
	ch->skill_level[SEARCH_SKILL] += 2;
	ch->skill_level[SPOT_SKILL] += 2;
	send_to_char( "&wYou now possess a &gSensor Improvement&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "infrared vision") )
  {
	if( HAS_DROID(ch, INFRARED_VISION) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 1000 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 1000;
	SET_BIT(ch->droid_feat, INFRARED_VISION);
	send_to_char( "&wYou now possess an &gInfrared Vision&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "motion sensors") )
  {
	if( HAS_DROID(ch, MOTION_SENSORS) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 1000 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 1000;
	ch->skill_level[SPOT_SKILL] += 2;
	SET_BIT(ch->droid_feat, MOTION_SENSORS);
	send_to_char( "&wYou now possess a &gMotion Sensors&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "sonic sensors") )
  {
	if( HAS_DROID(ch, SONIC_SENSORS) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 1000 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 1000;
	ch->skill_level[LISTEN_SKILL] += 2;
	SET_BIT(ch->droid_feat, SONIC_SENSORS);
	send_to_char( "&wYou now possess a &gSonic Sensor&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "360-degree vision") || !str_cmp(argument, "360 degree vision") )
  {
	if( HAS_DROID(ch, DEGREE_VISION) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 1000 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 1000;
	ch->skill_level[SPOT_SKILL] += 2;
	SET_BIT(ch->droid_feat, DEGREE_VISION);
	send_to_char( "&wYou now possess a &g360 Degree Vision&w.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "shield generator") )
  {
	if( HAS_DROID(ch, SHIELD_GENERATOR) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 250000 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 250000;
	ch->damage_reduction += 6;
	SET_BIT(ch->droid_feat, SHIELD_GENERATOR);
	send_to_char( "&wYou now possess a &gShield Generator&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "vocabulator") )
  {
	if( HAS_DROID(ch, VOCABULATOR_FEAT) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 250 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 250;
	SET_BIT(ch->droid_feat, VOCABULATOR_FEAT);
	send_to_char( "&wYou now possess a &gVocabulator&w upgrade.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "martial arts") || !str_cmp(argument, "martial arts programming") )
  {
	if( HAS_DROID(ch, MARTIAL_PROGRAMMING) )
	 {
		send_to_char( "&wYou already possess this programming.\n\r", ch );
		return;
	 }
	if( ch->gold < 2500 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 2500;
	SET_BIT(ch->droid_feat, MARTIAL_PROGRAMMING);
	send_to_char( "&wYou have now been programed for &gMartial Arts&w.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "defensive martial arts") || !str_cmp(argument, "defensive martial arts programming" ) )
  {
	if( HAS_DROID(ch, DEF_MARTIAL_PROGRAMMING) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 2500 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 2500;
	SET_BIT(ch->droid_feat, DEF_MARTIAL_PROGRAMMING);
	send_to_char( "&wYou have now been programed for &gDefensive Martial Arts&w.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "improved martial arts") || !str_cmp(argument, "improved martial arts programming" ) )
  {
	if( HAS_DROID(ch, IMP_MARTIAL_PROGRAMMING) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 2500 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 2500;
	SET_BIT(ch->droid_feat, IMP_MARTIAL_PROGRAMMING);
	send_to_char( "&wYou have now been programed for &gImproved Martial Arts&w.\n\r", ch );
	return;
  }
 if( !str_cmp(argument, "advanced martial arts") || !str_cmp(argument, "advanced martial arts programming" ) )
  {
	if( HAS_DROID(ch, ADV_MARTIAL_PROGRAMMING) )
	 {
		send_to_char( "&wYou already possess this enhancement.\n\r", ch );
		return;
	 }
	if( ch->gold < 2500 )
	 {
		send_to_char( "&wYou don't have enough credits.\n\r", ch );
		return;
	 }
	ch->gold -= 2500;
	SET_BIT(ch->droid_feat, ADV_MARTIAL_PROGRAMMING);
	send_to_char( "&wYou have now been programed for &gAdvanced Martial Arts&w.\n\r", ch );
	return;
  }
send_to_char( "&wPurchase which upgrade or programming?\n\r", ch );
return;
}