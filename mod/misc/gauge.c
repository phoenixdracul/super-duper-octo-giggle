/*
 * Function for creating a "graphical" look of a gauge. -- Kasji
 */

#include "../../mud.h"

char * gauge(int curr, int max, int size)
{
    static char buf[MAX_STRING_LENGTH];
    float i = 0.0f;
    int x = 0, y = 0;

    buf[0] = '\0';

    if (max <= 0)
	return NULL;

    size = URANGE(4, size, 80);
    i = (float)curr / (float)max * (float)size;

    buf[0] = '&';
    buf[1] = 'B';
    buf[2] = '<';
    buf[3] = '&';
    buf[4] = 'R';
    x = 5;
    for (y = 0; y < (int)i; x++, y++)
    {
	if (y == (int)((float)size/3))
	{
	    buf[x] = '&';
	    x++;
	    buf[x] = 'Y';
	    x++;
	}
        else if (y == (int)((float)(2*size)/3))
        {
            buf[x] = '&';
            x++;
            buf[x] = 'G';
            x++;
        }
	buf[x] = '|';
    }
    for (; x < size; x++)
    {
	buf[x] = ' ';
    }
    buf[x] = '&';
    buf[++x] = 'B';
    buf[++x] = '>';
    buf[++x] = '\0';

    return buf;
}

void do_gauge(CHAR_DATA *ch, char * argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    if (!argument || argument[0] == '\0')
	return;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

//    gauge_to_char(ch, atoi(arg1), atoi(arg2), atoi(argument));
    ch_printf(ch, "Guage: %s\n\r", gauge(atoi(arg1), atoi(arg2), atoi(argument)));
    send_to_char("\n\r", ch);

    return;
}

