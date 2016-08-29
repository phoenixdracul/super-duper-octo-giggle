#include <sys/types.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

char *ext_flag_string( EXT_BV * bitvector, const char *const flagarray[] )
{
   static char buf[MSL];
   int x;

   buf[0] = '\0';
   for( x = 0; x < MAX_BITS; x++ )
      if( xIS_SET( *bitvector, x ) )
      {
         mudstrlcat( buf, flagarray[x], MSL );
         //   strncat( buf, flagarray[x], MSL );

         /*
          * don't catenate a blank if the last char is blank  --Gorog
          */
         if( buf[0] != '\0' && ' ' != buf[strlen( buf ) - 1] )
            mudstrlcat( buf, " ", MSL );
      }

   if( ( x = strlen( buf ) ) > 0 )
      buf[--x] = '\0';

   return buf;
}

char *ext_flag_string2( EXT_BV * bitvector, const char *const flagarray[] )
{
   static char buf[MSL];
   int x;

   buf[0] = '\0';
   for( x = 0; x < MAX_BITS; x++ )
      if( xIS_SET( *bitvector, x ) )
      {
         mudstrlcat( buf, "'", MSL );

         mudstrlcat( buf, flagarray[x], MSL );
         //   strncat( buf, flagarray[x], MSL );

         /*
          * don't catenate a blank if the last char is blank  --Gorog
          */
         mudstrlcat( buf, "'", MSL );

         if( buf[0] != '\0' && ' ' != buf[strlen( buf ) - 1] )
            mudstrlcat( buf, " ", MSL );
      }

   if( ( x = strlen( buf ) ) > 0 )
      buf[--x] = '\0';

   return buf;
}

/*
 * Extended Bitvector Routines					-Thoric
 */

/* check to see if the extended bitvector is completely empty */
BOOL ext_is_empty( EXT_BV * bits )
{
   int x;

   for( x = 0; x < XBI; x++ )
      if( bits->bits[x] != 0 )
         return FALSE;

   return TRUE;
}

void ext_clear_bits( EXT_BV * bits )
{
   int x;

   for( x = 0; x < XBI; x++ )
      bits->bits[x] = 0;
}

/* for use by xHAS_BITS() -- works like IS_SET() */
int ext_has_bits( EXT_BV * var, EXT_BV * bits )
{
   int x, bit;

   for( x = 0; x < XBI; x++ )
   {
//        if( var->bits[x] == 0 )
//            break;
      if( ( bit = ( var->bits[x] & bits->bits[x] ) ) != 0 )
         return bit;
   }

   return 0;
}

/* for use by xSAME_BITS() -- works like == */
BOOL ext_same_bits( EXT_BV * var, EXT_BV * bits )
{
   int x;

   for( x = 0; x < XBI; x++ )
      if( var->bits[x] != bits->bits[x] )
         return FALSE;

   return TRUE;
}

/* for use by xSET_BITS() -- works like SET_BIT() */
void ext_set_bits( EXT_BV * var, EXT_BV * bits )
{
   int x;

   for( x = 0; x < XBI; x++ )
      var->bits[x] |= bits->bits[x];
}

/* for use by xREMOVE_BITS() -- works like REMOVE_BIT() */
void ext_remove_bits( EXT_BV * var, EXT_BV * bits )
{
   int x;

   for( x = 0; x < XBI; x++ )
      var->bits[x] &= ~( bits->bits[x] );
}

/* for use by xTOGGLE_BITS() -- works like TOGGLE_BIT() */
void ext_toggle_bits( EXT_BV * var, EXT_BV * bits )
{
   int x;

   for( x = 0; x < XBI; x++ )
      var->bits[x] ^= bits->bits[x];
}

/*
 * Read an extended bitvector from a file.			-Thoric
 */
EXT_BV fread_bitvector( FILE * fp )
{
   EXT_BV ret;
   int c, x = 0;
   int num = 0;

   memset( &ret, '\0', sizeof( ret ) );
   for( ;; )
   {
      num = fread_number( fp );
      if( x < XBI )
         ret.bits[x] = num;
      ++x;
      if( ( c = getc( fp ) ) != '&' )
      {
         ungetc( c, fp );
         break;
      }
   }

   return ret;
}

/* return a string for writing a bitvector to a file */
char *print_bitvector( EXT_BV * bits )
{
   static char buf[XBI * 12];
   char *p = buf;
   int x, cnt = 0;

   for( cnt = XBI - 1; cnt > 0; cnt-- )
      if( bits->bits[cnt] )
         break;
   for( x = 0; x <= cnt; x++ )
   {
      sprintf( p, "%d", bits->bits[x] );
      p += strlen( p );
      if( x < cnt )
         *p++ = '&';
   }
   *p = '\0';

   return buf;
}

/*
 * Write an extended bitvector to a file			-Thoric
 */
void fwrite_bitvector( EXT_BV * bits, FILE * fp )
{
   fputs( print_bitvector( bits ), fp );
}


EXT_BV meb( int bit )
{
   EXT_BV bits;

   xCLEAR_BITS( bits );
   if( bit >= 0 )
      xSET_BIT( bits, bit );

   return bits;
}


EXT_BV multimeb( int bit, ... )
{
   EXT_BV bits;
   va_list param;
   int b;

   xCLEAR_BITS( bits );
   if( bit < 0 )
      return bits;

   xSET_BIT( bits, bit );

   va_start( param, bit );

   while( ( b = va_arg( param, int ) ) != -1 )
        xSET_BIT( bits, b );

   va_end( param );

   return bits;
}

EXT_BV convertBitvectorToExtended( int bv )
{
   EXT_BV newBV;
   int i;

   xCLEAR_BITS( newBV );

   for( i = 0; i < 32; i++ )
      if( IS_SET( bv, 1 << i ) )
         xSET_BIT( newBV, i );

   return newBV;
}

