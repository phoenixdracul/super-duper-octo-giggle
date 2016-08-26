/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 						       Mentor System				    			*
 * 																			*
 *  Allows builders and coders to obtain mentors/mentees.
 *  											*
 * 																			*
 +--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 * AU v1.0 coding by Daniel Donaldson								*
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
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "mud.h"

void do_mentor( CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  CHAR_DATA *vic3;
  CHAR_DATA *vic4;
  
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  
  if (arg1[0] == '\0' || arg2[0] == '\0')
    {
    	send_to_char("Syntax: Mentor <victim1> <victim2>\n\r", ch);
    	return;
    }
  
  if ( ( vic3 = get_char_room( ch, arg1 ) ) == NULL )
   {
      sprintf(buf, "%s doesn't appear to be here.\n\r", arg1);
      send_to_char(buf, ch);
      return;
   }

  if ( ( vic4 = get_char_room( ch, arg2 ) ) == NULL )
   {
      sprintf(buf, "%s doesn't appear to be here.\n\r", arg2);
      send_to_char(buf, ch);
      return;
   }  
  
  if(IS_NPC(vic3))
   {
     sprintf(buf, "%s appears to be a mob. Seriously?\n\r", vic3->name);
     send_to_char(buf, ch);
     return;
   }

  if(IS_NPC(vic4))
   {
     sprintf(buf, "%s appears to be a mob. Seriously?\n\r", vic4->name);
     send_to_char(buf, ch);
     return;
   }

   
  if(vic3->top_level < 1)
   {
     sprintf(buf, "%s is to low a level to be a mentee or mentor.\n\r", vic3->name);
     send_to_char(buf, ch);
     return;
   }
   
  if(vic4->top_level < 1)
   {
     sprintf(buf, "%s is to low a level to be a mentee or mentor\n\r", vic4->name);
     send_to_char(buf, ch);
     return;
   }
  
  vic3->pcdata->mentor = vic4->name;
  vic4->pcdata->mentor = vic3->name;
  /*if (vic1->sex == SEX_MALE && vic2->sex == SEX_FEMALE)
    vic2->pcdata->last_name = vic1->pcdata->last_name;
  else if (vic1->sex == SEX_FEMALE && vic2->sex == SEX_MALE)
    vic1->pcdata->last_name = vic2->pcdata->last_name;
  */ 
  sprintf(buf, "&R(&WMentor&R} &W%s: %s and %s are now mentee/mentor.", ch->name, vic3->name, vic4->name);
  echo_to_all(AT_YELLOW, buf, ECHOTAR_ALL);
  return;
}

void do_unmentor( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *vic3;
  CHAR_DATA *vic4;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  
  if (arg1[0] == '\0' && arg2[0] == '\0')
   {
     send_to_char("Syntax: Unmentor <victim1> <victim2>\n\r", ch);
     return;
   }
  
  if ((vic3 = get_char_room(ch, arg1)) == NULL)
   {
     sprintf(buf, "%s doesn't appear to be here. You should wait for them.\n\r", arg1);
     send_to_char(buf, ch);
     return;
   }
  
  if ((vic4 = get_char_room(ch, arg2)) == NULL)
   {
     sprintf(buf, "%s doesn't appear to be here. You should wait for then.\n\r", arg2);
     send_to_char(buf, ch);
     return;
   }
  
  if (!vic3->pcdata->mentor || vic3->pcdata->mentor == NULL)
   {
     sprintf(buf, "%s doesn't appear to be a mentor/mentee.\n\r", vic3->name);
     send_to_char(buf, ch);
     return;
   }

  if (!vic4->pcdata->mentor || vic4->pcdata->mentor == NULL)
   {
     sprintf(buf, "%s doesn't appear to be a mentor/mentee\n\r", vic4->name);
     send_to_char(buf, ch);
     return;
   }
  
  if (vic3->pcdata->mentor != vic4->pcdata->mentor)
   {
     sprintf(buf, "%s is not a mentor or mentee to %s\n\r", vic3->name, vic4->name);
     send_to_char(buf, ch);
     return;
   }
  
  vic3->pcdata->mentor = NULL;
  vic4->pcdata->mentor = NULL;
    sprintf(buf, "&R(&WMentor&R} &W%s: %s and %s are no longer mentors/mentees.", ch->name, vic3->name, vic4->name);
}   

void do_mentortalk (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;

  if(!argument || argument[0] == '\0')
    {
      send_to_char ("What would you like to say?\n\r", ch);
      return;
    }
  

  if ( ch->position == POS_SLEEPING)
    {
      send_to_char ("In your dreams or what?\n\r", ch);
      return;
    }

if ( ch->position == POS_FIGHTING)
  {
    send_to_char("You should finish what your doing first!\n\r", ch);
    return;
  }
    
  if ( IS_NPC( ch ) )
    {
	send_to_char( "your a mob....thats just wrong!\n\r", ch );
	return;	  
    }
  
  if (!ch->pcdata->mentor)
    {
      send_to_char ("But you are not a mentor/mentee!\n\r", ch);
      return;
    }

  if ((victim = get_char_world (ch, ch->pcdata->mentor)) == NULL)
    {
      sprintf (buf, "%s is not connected.\n\r", ch->pcdata->mentor);
      send_to_char (buf, ch);
      return;
    }
    
    sprintf(buf, "&G[&PMentor&G] &Pto %s: &W%s\n\r", victim->name, argument);
    send_to_char(buf, ch); 
    sprintf(buf, "&G[&PMentor&G] &P%s talks: &W%s\n\r", ch->name, argument); 
    send_to_char(buf, victim); 
}
