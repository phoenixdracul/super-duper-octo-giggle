// Dice roller for players to do their own skill checks, etc.

#include <math.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

void do_rolldice(CHAR_DATA * ch, char * argument)
{
	CHAR_DATA * vch;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int amount, sides, plus;
	int i, sum, roll[20];
	int col;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Syntax: rolldice <amount> <number of sides> [optional plus]\n\r", ch);
		return;
	}

	amount = atoi(arg1);

	if (amount < 1 || amount > 20)
	{
		send_to_char("Amount of dice to roll must be between 1 to 20.\n\r", ch);
		return;
	}

	sides = atoi(arg2);

	if (sides < 2 || sides > 1000000)
	{
		send_to_char("Numbers of sides of dice must be between 2 to 1000000.\n\r", ch);
		return;
	}

	if (arg3[0] != '\0')
		plus = atoi(arg3);
	else
		plus = 0;

	sum = plus;

	sprintf(buf, "&C(&zOOC&C) &w$n rolls some dice:\n\r    ");

	for (i = 0; i < amount; i++)
	{
		roll[i] = number_range(1, sides);
		sum += roll[i];
		sprintf(buf2, "&c%d", roll[i]);
		strcat(buf, buf2);
		if (i + 1 < amount)
			strcat(buf, " &w+ ");
		else if (plus != 0)
			strcat(buf, " &w(+ ");
		else
			strcat(buf, " &w= ");
		if ((++col) % 10 == 0 && i + 1 < amount)
			strcat(buf, "\n\r     ");
	}

	if (plus != 0)
	{
		sprintf(buf2, "&c%d&w) = ", plus);
		strcat(buf, buf2);
	}

	sprintf(buf2, "&C%d\n\r", sum);
	strcat(buf, buf2);

        for(vch = ch->in_room->first_person; vch; vch = vch->next_in_room)
        {
//                arg = argument;
                if(vch == ch)
                        continue;

                act( AT_ACTION, buf, ch, NULL, vch, TO_VICT );
        }

        act( AT_ACTION, buf, ch, NULL, NULL, TO_CHAR);

	return;
}

void do_skillcheck(CHAR_DATA * ch, char * argument)
{
	return;
}
