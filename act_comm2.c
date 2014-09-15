// More character communications by Kasji
// psay - Pose Say

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

#define	POSE_MAX	59

char * const pose_key[] =
{ "advise", "announce", "ask", "assert", "attack", "bark", "bellow",
  "command", "cry", "declare", "defend", "deny", "divulge", "drawl",
  "exclaim", "explode", "fuss", "groan", "gripe", "growl", "grumble",
  "grunt", "haggle", "holler", "hussle", "improvise", "interject", "intimidate",
  "imply", "inform", "jest", "joke", "jump", "moan", "mumble",
  "mutter", "nudge", "offer", "order", "remark", "roar", "scream",
  "shout", "shriek", "sing", "snarl", "snort", "sob", "squawk",
  "squeak", "stammer", "stutter", "swear", "test", "utter", "vow",
  "weep", "whisper", "yell" };

char * const pose_self[] =
{ "advise", "announce", "ask", "assert", "attack", "bark", "bellow",
  "command", "cry", "declare", "defend", "deny", "divulge", "drawl",
  "exclaim", "explode", "fuss", "groan", "gripe", "growl", "grumble",
  "grunt", "haggle", "holler", "hussle", "improvise", "interject", "intimidate",
  "imply", "inform", "jest", "joke", "jump", "moan", "mumble",
  "mutter", "nudge", "offer", "order", "remark", "roar", "scream",
  "shout", "shriek", "sing", "snarl", "snort", "sob", "squawk",
  "squeak", "stammer", "stutter", "swear", "test", "utter", "vow",
  "weep", "whisper", "yell" };

char * const pose_others[] =
{ "advises", "announces", "asks", "asserts", "attacks", "barks", "bellows",
  "commands", "cries", "declares", "defends", "denies", "divulges", "drawls",
  "exclaims", "explodes", "fusses", "groans", "gripes", "growls", "grumbles",
  "grunts", "haggles", "hollers", "hussles", "improvises", "interjects", "intimidates",
  "implies", "informs", "jests", "jokes", "jumps", "moans", "mumbles",
  "mutters", "nudges", "offers", "orders", "remarks", "roars", "screams",
  "shouts", "shrieks", "sings", "snarls", "snorts", "sobs", "squawks",
  "squeaks", "stammers", "stutters", "swears", "tests", "utters", "vows",
  "weeps", "whispers", "yells" };

char * const conj1[] =
{ " to ", " to ", " ", " to ", " ", " at ", " at ",
  " ", " to ", " to ", " ", " to ", " to ", " on with ",
  " to ", " at ", " to ", " to ", " to ", " at ", " to ",
  " at ", " with ", " at ", " ", " with ", " to ", " ",
  " to ", " ", " with ", " with ", " on ", " to ", " to ",
  " to ", " ", " ", " ", " to ", " at ", " at ",
  " at ", " at ", " for ", " at ", " at ", " to ", " at ",
  " to ", " to ", " to ", " to ", " ", " to ", " to ",
  " to ", " to ", " at " };

int get_pose_index(char * key)
{
        int x;

        for ( x = 0; x < POSE_MAX; x++ )
                if ( !str_cmp( key, pose_key[x] ) )
                        return x;
        return -1;
}

void do_psay(CHAR_DATA * ch, char * argument)
{
	CHAR_DATA * victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int x;
	char txt[strlen(argument) + 2];

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0')
	{
                int i;
		send_to_char("&WSyntax: psay <pose> <name|all> <text>\n\r", ch);
                send_to_char("&WAvailable poses are:\n\r", ch);
                for (i = 0; i < POSE_MAX; i++)
                {
                        ch_printf(ch, "%12s", pose_key[i]);
                        if ((i+1) % 4 == 0 || i + 1 >= POSE_MAX)
                        {
                                send_to_char("\n\r", ch);
                        }
                }
		return;
	}

	x = get_pose_index(arg1);
	if (x < 0 || x >= POSE_MAX)
	{
		int i;
		send_to_char("&WInvalid pose type. Available poses are:\n\r", ch);
		for (i = 0; i < POSE_MAX; i++)
		{
			ch_printf(ch, "%12s", pose_key[i]);
			if ((i+1) % 4 == 0 || i + 1 >= POSE_MAX)
			{
				send_to_char("\n\r", ch);
			}
		}
		return;
	}

	if (arg2[0] == '\0')
	{
		send_to_char("&WWho do you want to talk to?\n\r", ch);
		return;
	}

	if (!str_cmp(arg2, "all"))
	{
		char buf[MAX_STRING_LENGTH];

		if (strlen(argument) <= 0)
		{
			send_to_char("&WWhat do you want to say?\n\r", ch);
			return;
		}

	        txt[0] = '\0';
	        if(IS_NPC(ch) || !ch->textcolor)
	                strcat(txt, "&w");
	        else
	                strcat(txt, ch->textcolor);
	        strcat(txt, argument);

		ch_printf(ch, "&wYou %s%severyone, &c\"%s&c\"&w\n\r", pose_self[x], conj1[x], txt);
		sprintf(buf, "&w$n %s%severyone, &c\"%s&c\"&w\n\r", pose_others[x], conj1[x], txt);
		act(AT_PLAIN, buf, ch, NULL, NULL, TO_ROOM);
		return;
	}
	else
	{
                char buf[MAX_STRING_LENGTH];

		victim = get_char_room(ch, arg2);
		if (!victim)
		{
			send_to_char("&WThat person isn't here.\n\r", ch);
			return;
		}

                if (strlen(argument) <= 0)
                {
                        send_to_char("&WWhat do you want to say?\n\r", ch);
                        return;
                }

                txt[0] = '\0';
                if(IS_NPC(ch) || !ch->textcolor)
                        strcat(txt, "&w");
                else
                        strcat(txt, ch->textcolor);
                strcat(txt, argument);

                sprintf(buf, "&wYou %s%s$N, &c\"%s&c\"&w\n\r", pose_self[x], conj1[x], txt);
		act(AT_PLAIN, buf, ch, NULL, victim, TO_CHAR);
                sprintf(buf, "&w$n %s%s$N, &c\"%s&c\"&w\n\r", pose_others[x], conj1[x], txt);
                act(AT_PLAIN, buf, ch, NULL, victim, TO_NOTVICT);
		sprintf(buf, "&w$n %s%syou, &c\"%s&c\"&w\n\r", pose_others[x], conj1[x], txt);
		act(AT_PLAIN, buf, ch, NULL, victim, TO_VICT);
                return;
	}

	return;
}
