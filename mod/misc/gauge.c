/*
 * Function for creating a "graphical" look of a gauge. -- Kasji
 */

#include "../../mud.h"

#include <string.h>

char * gauge(int curr, int max, int size)
{
    static char buf[81];
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

// Making the gauge 256 color pretty-fied!

const char * const	gauge_colors[] =
{
    // red to orange to yellow
    "&196", "&202", "&208", "&214", "&220", "&226",
    // yellow to pale green to solid green
    "&190", "&154", "&118", "&82",  "&46",  "&10"
    // 12 colors
};

// min and max are in 0.0 to 1.0 range
char * min_max_gauge_256(float min, float max, int size)
{
    static char buf[81];
    int i1 = 0, i2 = 0;
    int x = 0, y = 0, i = 0;
    int col1, col2, len, blend;

    if (min < 0.0 || max > 1.0 || min > max)
	return "&R(Gauge Malfunction)";

    size = URANGE(4, size, 80);
    i1 = (int)(min * (float)size);
    i2 = (int)(max * (float)size);
    i1 = URANGE(0, i1, size);
    i2 = URANGE(0, i2, size);

    col1 = URANGE(0, (int)(min * 12.0) - 1, 11);
    col2 = URANGE(0, (int)(max * 12.0) - 1, 11);
    blend = col2 - col1 + 1;
    len = (i2 - i1) / blend;

    buf[0] = '&';
    buf[1] = 'B';
    x = 2;

    for (y = 0; y < i1; ++x, ++y)
    {
	buf[x] = '-';
    }
    for (i = 0; i < strlen(gauge_colors[col1]); ++x, ++i)
    {
	buf[x] = gauge_colors[col1][i];
    }
    blend = 0;
    for (; y <= i2; ++x, ++y)
    {
	if (blend == len)
	{
	    blend = 0;
	    ++col1;
	    col1 = URANGE(0, col1, col2);
	    for (i = 0; i < strlen(gauge_colors[col1]); ++x, ++i)
	    {
	        buf[x] = gauge_colors[col1][i];
	    }
	}
	else
	    ++blend;
	buf[x] = '|';
    }
    buf[x] = '&';
    ++x;
    buf[x] = 'B';
    ++x;
    for (; y < size; ++x, ++y)
    {
	buf[x] = '-';
    }
    buf[x] = '\0';

    return buf;
}

// min and max are in 0.0 to 1.0 range
char * min_max_gauge(float min, float max, int size)
{
    static char buf[81];
    int i1 = 0, i2 = 0;
    int x = 0, y = 0;

    if (min < 0.0 || max > 1.0 || min > max)
	return "&R(Gauge Malfunction)";

    size = URANGE(4, size, 80);
    i1 = (int)(min * (float)size);
    i2 = (int)(max * (float)size);
    i1 = URANGE(0, i1, size);
    i2 = URANGE(0, i2, size);

    buf[0] = '&';
    buf[1] = 'B';
    x = 2;

    for (y = 0; y < i1; ++x, ++y)
    {
	buf[x] = '-';
    }
    buf[x] = '&';
    ++x;
    if (y <= (int)((float)size/3.0))
	buf[x] = 'R';
    else if (y <= (int)((float)(2*size)/3.0))
	buf[x] = 'Y';
    else
	buf[x] = 'G';
    ++x;
    for (; y <= i2; ++x, ++y)
    {
        if (y == (int)((float)size/3.0))
        {
            buf[x] = '&';
            ++x;
            buf[x] = 'Y';
            ++x;
        }
        else if (y == (int)((float)(2*size)/3.0))
        {
            buf[x] = '&';
            ++x;
            buf[x] = 'G';
            ++x;
        }
	buf[x] = '|';
    }
    buf[x] = '&';
    ++x;
    buf[x] = 'B';
    ++x;
    for (; y < size; ++x, ++y)
    {
	buf[x] = '-';
    }
    buf[x] = '\0';

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

void do_minmaxgauge(CHAR_DATA *ch, char * argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    if (!argument || argument[0] == '\0')
        return;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

//    gauge_to_char(ch, atoi(arg1), atoi(arg2), atoi(argument));
    ch_printf(ch, "Guage: |%s|\n\r", min_max_gauge(atof(arg1), atof(arg2), atoi(argument)));
    send_to_char("\n\r", ch);

    return;
}

