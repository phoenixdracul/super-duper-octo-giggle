/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 							Player Lottery Module 						    *
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

/* Version History */

/*
 * Version 1.0
 * - basic lottery module created and implemented
 * - made completely automated
 * - players can buy 1 lottery ticket with the number 1 - 100
 * - adds 250,000 credits per player online every hour
 * - will carry over the old pot if no one claims the prize after 24 hours plus 1,000,000 
 *   credits
 * - immortals can set the draw time and claim time
 *
 * Version 1.1
 * - time format function added and applied
 * - multiple fixes
 * - bank taxall will take the taxes and add it to the pot
 * - new "SHOW" format
 * - amount of players added
 * - started on date added
 * - immortals able to set the winning number
 *
 * Version 1.2
 * - immortals can set the pot amount
 * - if lottery goes over 25,000,000 it will freeze
 * - players are now able to pick the number they want (if players pick the same number, first come first serve)
 *
 *
 */

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\


/* Locals */
BOOL load_lottery args( ( LOTTERY_DATA * lot ) );
void fread_lottery( LOTTERY_DATA * lot, FILE * fp );
void save_lottery( LOTTERY_DATA lot );
char *format_lottery_time( char *output, int total_minutes );

void lottochan( char* );

char * num_punct(int foo);

void do_lottery( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   int next_draw, claim_time, winning_number, pot_amount;
   char *output = NULL;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( IS_NPC( ch ) )
      return;

   if( arg1[0] == '\0' )
   {
      send_to_char( "&CSyntax&B: &Wlottery buy&B, &Wshow&B, &Wclaim&B, &Wquit\n\r", ch );
      if( IS_IMMORTAL( ch ) )
         send_to_char( "&CImmortal Commands&B: &Wnext, pot, draw, claimtime, win\n\r", ch );
      return;
   }

   if( !str_cmp( arg1, "buy" ) )
   {
      int lottery_number;

      if( lottery.lottery_winner > 0 )
      {
         send_to_char
            ( "&RSorry, the winning number has already been drawn, please wait for an Immortal to reset the lottery.\n\r",
              ch );
         return;
      }

      if( ch->pcdata->lottery_number > 0 )
      {
         send_to_char
            ( "&RIt seems you have already bought a lottery ticket. Please quit the lottery to buy a new ticket.\n\r", ch );
         return;
      }

      lottery_number = atoi( arg2 );

      if( arg2[0] == '\0' )
      {
         send_to_char( "&CSyntax&B: &Wlottery buy &B<&wnumber&B>&D\n\r", ch );
         return;
      }

      if( !is_number( arg2 ) )
      {
         send_to_char( "&RThats not a valid lottery number.\n\r", ch );
         return;
      }

      if( lottery_number < 1 )
      {
         send_to_char( "&RThats not a valid lottery number.\n\r", ch );
         return;
      }

      if( lottery_number > 100 )
      {
         send_to_char( "&RLottery numbers are not greater then 100.\n\r", ch );
         return;
      }

      if( ch->gold < 5000 )
      {
         send_to_char( "&RYou need 5,000 credits to buy a ticket.\n\r", ch );
         return;
      }

      ch->gold -= 5000;
      ch_printf( ch, "&CYou buy a lottery ticket with the number, &w%d&C, &Cprinted on it.\n\r", lottery_number );
      ch->pcdata->lottery_number = lottery_number;
      lottery.lottery_pot += 5000;
      lottery.players++;
      save_lottery( lottery );
      return;
   }

   if( !str_cmp( arg1, "show" ) )
   {
      ch_printf( ch, "&CLottery started on %s", lottery.started );
      send_to_char( "&B-------------------------------------------\n\r", ch );
      send_to_char( "\n\r", ch );
      ch_printf( ch, "&CThe lottery is currently valued at &Y%s &Ccredits.\n\r", num_punct(lottery.lottery_pot) );
      ch_printf( ch, "&CA total of &W%d &Cplayer%s bought tickets for this weeks lottery.\n\r", lottery.players,
                 lottery.players == 1 ? " has" : "s have" );
      send_to_char( "\n\r", ch );
      send_to_char( "&B-------------------------------------------\n\r", ch );
      send_to_char( "\n\r", ch );
      ch_printf( ch, "&CThe last lottery was won by &W%s &Cand it was worth &Y%s &Ccredits.\n\r", lottery.last_winner,
                 num_punct(lottery.last_pot) );
      if( lottery.next_draw > 0 )
         ch_printf( ch, "&CThe next draw will be in %s.\n\r", format_lottery_time( output, lottery.next_draw ) );
      if( lottery.claim_time > 0 )
      {
         ch_printf( ch, "&CThis weeks winning number is &W%d&C.\n\r", lottery.lottery_winner );
         ch_printf( ch, "&CThe winner has %s &Cto claim their prize.\n\r",
                    format_lottery_time( output, lottery.claim_time ) );
      }
      send_to_char( "\n\r", ch );
      send_to_char( "&B-------------------------------------------\n\r", ch );
      if( ch->pcdata->lottery_number > 0 )
      {
         ch_printf( ch, "&CYour have purchased a lottery ticket with &W%d &Cprinted on it.&D\n\r",
                    ch->pcdata->lottery_number );
      }
      return;
   }

   if( !str_cmp( arg1, "claim" ) )
   {
      if( lottery.lottery_winner == 0 )
      {
         send_to_char( "&RThe lottery number has not been drawn yet.\n\r", ch );
         return;
      }
      if( ch->pcdata->lottery_number == 0 )
      {
         send_to_char( "&RYou must have forgotten to buy a lottery ticket.\n\r", ch );
         return;
      }
      if( ch->pcdata->lottery_number != lottery.lottery_winner )
      {
         send_to_char( "&RYou are not a winner, better luck next time.\n\r", ch );
         ch->pcdata->lottery_number = 0;
         return;
      }
      if( ch->pcdata->lottery_number == lottery.lottery_winner )
      {
         ch_printf( ch, "&GCongratulations! You have won the lottery of &Y%s &Gcredits.\n\r",
                    num_punct(lottery.lottery_pot) );
         sprintf( buf, "%s is the winner of this week's lottery.\n\r", ch->name );
         lottochan( buf );
         ch->gold += lottery.lottery_pot;
         ch->pcdata->lottery_number = 0;
         lottery.last_winner = ch->name;
         lottery.last_pot = lottery.lottery_pot;
         lottery.lottery_pot = 1000000;
         lottery.next_draw = 10080;
         lottery.lottery_winner = 0;
         lottery.claim_time = -1;
         lottery.started = ctime( &current_time );
         lottery.players = 0;
         save_lottery( lottery );
      }
   }

   if( !str_cmp( arg1, "quit" ) )
   {
      if( ch->pcdata->lottery_number == 0 )
      {
         send_to_char( "&RYou don't seem to be part of a lottery at the moment.\n\r", ch );
         return;
      }
      ch->pcdata->lottery_number = 0;

      if( lottery.players > 0 )
         lottery.players--;

      send_to_char( "&RYou quit the current lottery and throw your ticket away.\n\r", ch );
      return;
   }

   if( IS_IMMORTAL( ch ) )
   {
      if( !str_cmp( arg1, "next" ) )
      {
         ch_printf( ch, "&CNext update in&B: &W%s.\n\r", format_lottery_time( output, lottery.lottery_next ) );
         return;
      }

      if( !str_cmp( arg1, "pot" ) )
      {
         pot_amount = atoi( arg2 );
         if( arg2[0] == '\0' )
         {
            send_to_char( "&RPlease specify an amount for the pot.\n\r", ch );
            return;
         }
         if( arg2 < 0 )
         {
            send_to_char( "&RAmount must be greater than 0.\n\r", ch );
            return;
         }

         lottery.lottery_pot = pot_amount;
         sprintf( buf, "&CThe lottery pot has been set to &Y%s &Cby an Immortal.\n\r", num_punct(lottery.lottery_pot) );
         lottochan( buf );
         save_lottery( lottery );
         return;
      }

      if( !str_cmp( arg1, "draw" ) )
      {
         next_draw = atoi( arg2 );
         if( arg2[0] == '\0' )
         {
            send_to_char( "&RPlease specify the time, in minutes, until the next draw.\n\r", ch );
            return;
         }
         ch_printf( ch, "&CThe next draw will be held in %s.\n\r", format_lottery_time( output, next_draw ) );
         lottery.lottery_winner = 0;
         lottery.next_draw = next_draw;
         save_lottery( lottery );
      }

      if( !str_cmp( arg1, "claimtime" ) )
      {
         claim_time = atoi( arg2 );
         if( arg2[0] == '\0' )
         {
            send_to_char( "&RPlease specify the time, in minutes, for the winner to claim their prize.\n\r", ch );
            return;
         }
         if( lottery.lottery_winner == 0 )
         {
            send_to_char( "&RThe winning number has not yet been drawn.\n\r", ch );
            return;
         }
         ch_printf( ch, "&CThe winner has %s to claim their prize.\n\r", format_lottery_time( output, claim_time ) );
         lottery.claim_time = claim_time;
         save_lottery( lottery );
         return;
      }

      if( !str_cmp( arg1, "win" ) )
      {
         winning_number = atoi( arg2 );
         if( arg2[0] == '\0' )
         {
            send_to_char( "&RPlease specify a new winning number.\n\r", ch );
            return;
         }
         ch_printf( ch, "&CThe new winning number is &W%d&C.\n\r", winning_number );
         lottery.lottery_winner = winning_number;
         save_lottery( lottery );
         return;
      }
   }
   else
   {
      send_to_char( "That is not a valid lottery command.\n\r", ch );
      return;
   }
   return;
}

BOOL load_lottery( LOTTERY_DATA * lot )
{
   char filename[MAX_INPUT_LENGTH];
   FILE *fp;
   BOOL found;

   found = FALSE;
   sprintf( filename, "%slottery.dat", SYSTEM_DIR );

   if( ( fp = fopen( filename, "r" ) ) != NULL )
   {

      found = TRUE;
      for( ;; )
      {
         char letter;
         char *word;

         letter = fread_letter( fp );
         if( letter == '*' )
         {
            fread_to_eol( fp );
            continue;
         }

         if( letter != '#' )
         {
            bug( "Load_lotterydata_file: # not found." );
            break;
         }

         word = fread_word( fp );
         if( !str_cmp( word, "LOTTERY" ) )
         {
            fread_lottery( lot, fp );
            break;
         }
         else if( !str_cmp( word, "END" ) )
            break;
         else
         {
            bug( "Load_lotterydata_file: bad section." );
            break;
         }
      }
      fclose( fp );
   }

   return found;
}

void save_lottery( LOTTERY_DATA lot )
{
   FILE *fp;
   char filename[MAX_INPUT_LENGTH];

   sprintf( filename, "%slottery.dat", SYSTEM_DIR );

   fclose( fpReserve );
   if( ( fp = fopen( filename, "w" ) ) == NULL )
   {
      bug( "save_lotterydata: fopen" );
   }
   else
   {
      fprintf( fp, "#LOTTERY\n" );
      fprintf( fp, "ClaimTime     %d\n", lot.claim_time );
      fprintf( fp, "LastWinner    %s~\n", lot.last_winner );
      fprintf( fp, "LastPot       %d\n", lot.last_pot );
      fprintf( fp, "LotteryPot    %d\n", lot.lottery_pot );
      fprintf( fp, "LotteryNext   %d\n", lot.lottery_next );
      fprintf( fp, "LotteryWinner %d\n", lot.lottery_winner );
      fprintf( fp, "NextDraw      %d\n", lot.next_draw );
      fprintf( fp, "Players       %d\n", lot.players );
      fprintf( fp, "Started       %s~\n", lot.started );
      fprintf( fp, "End\n\n" );
      fprintf( fp, "#END\n" );
   }
   fclose( fp );
   fpReserve = fopen( NULL_FILE, "r" );
   return;
}

char *format_lottery_time( char *output, int total_minutes )
{
   char buf[MAX_STRING_LENGTH];
   int days = 0, hours = 0, minutes = 0;

   if( total_minutes > 1440 )
   {
      days = total_minutes / 1440;
      total_minutes -= days * 1440;
   }

   if( total_minutes > 60 )
   {
      hours = total_minutes / 60;
      total_minutes -= hours * 60;
   }

   minutes = total_minutes;

   if( days > 0 )
      sprintf( buf, "&W%d &Cday%s, &W%d &Chour%s, and &W%d &Cminute%s", days, days == 1 ? "" : "s", hours,
               hours == 1 ? "" : "s", minutes, minutes == 1 ? "" : "s" );
   else if( hours > 0 )
      sprintf( buf, "&W%d &Chour%s, and &W%d &Cminute%s", hours, hours == 1 ? "" : "s", minutes, minutes == 1 ? "" : "s" );
   else if( minutes > 0 )
      sprintf( buf, "&W%d &Cminute%s", minutes, minutes == 1 ? "" : "s" );

   output = STRALLOC( buf );

   return output;
}
