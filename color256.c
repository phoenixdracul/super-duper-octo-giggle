// 256 Colors Support -- Kasji

#include <math.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

void do_color_test(CHAR_DATA * ch, char * argument)
{
	char buf[MAX_STRING_LENGTH];

	sprintf(buf, "\e[38;5;%dmTHIS IS A COLOR TEST!\n\r", atoi(argument));
	send_to_char(buf, ch);
	write_to_buffer(ch->desc, buf, 0);

	return;
}
