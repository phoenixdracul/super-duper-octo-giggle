/* Semi-deprecated. Get rid of it.	*/

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <dlfcn.h>
#include <string.h>
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
				}


/* global variables */
int top_sn;
int top_herb;

SKILLTYPE *		skill_table	[MAX_SKILL];
SKILLTYPE *		herb_table	[MAX_HERB];

char * const skill_tname[] = { "unknown", "Spell", "Skill", "Weapon", "Tongue", "Herb" };

SPELL_FUN *spell_function( char *name )
{
   void *funHandle;
   const char *error;

   funHandle = dlsym( sysdata.dlHandle, name );
   if( ( error = dlerror() ) != NULL )
   {
	bug( "Error locating %s in symbol table. %s", name, error );
      return (SPELL_FUN *) spell_notfound;
   }
   return (SPELL_FUN *) funHandle;
}

DO_FUN *skill_function( char *name )
{
   void *funHandle;
   const char *error;

   funHandle = dlsym( sysdata.dlHandle, name );
   if( ( error = dlerror() ) != NULL )
   {
	bug( "Error locating %s in symbol table. %s", name, error );
	return (DO_FUN *) skill_notfound;
   }
   return (DO_FUN *) funHandle;
}

/*
 * Function used by qsort to sort skills
 */
int skill_comp( SKILLTYPE **sk1, SKILLTYPE **sk2 )
{
    SKILLTYPE *skill1 = (*sk1);
    SKILLTYPE *skill2 = (*sk2);

    if ( !skill1 && skill2 )
	return 1;
    if ( skill1 && !skill2 )
	return -1;
    if ( !skill1 && !skill2 )
	return 0;
    if ( skill1->type < skill2->type )
	return -1;
    if ( skill1->type > skill2->type )
	return 1;
    return strcmp( skill1->name, skill2->name );
}

/*
 * Sort the skill table with qsort
 */
void sort_skill_table()
{
    log_string( "Sorting skill table..." );
    qsort( &skill_table[1], top_sn-1, sizeof( SKILLTYPE * ),
		(int(*)(const void *, const void *)) skill_comp );
}


/*
 * Write skill data to a file
 */
void fwrite_skill( FILE *fpout, SKILLTYPE *skill )
{
	SMAUG_AFF *aff;

	fprintf( fpout, "Name         %s~\n",	skill->name	);
	fprintf( fpout, "Type         %s\n",	skill_tname[skill->type]);
	fprintf( fpout, "Flags        %d\n",	skill->flags	);
	if ( skill->target )
	  fprintf( fpout, "Target       %d\n",	skill->target	);
	if ( skill->minimum_position )
	  fprintf( fpout, "Minpos       %d\n",	skill->minimum_position );
	if ( skill->saves )
	  fprintf( fpout, "Saves        %d\n",	skill->saves	);
	if ( skill->slot )
	  fprintf( fpout, "Slot         %d\n",	skill->slot	);
	if ( skill->min_mana )
	  fprintf( fpout, "Mana         %d\n",	skill->min_mana );
	if ( skill->beats )
	  fprintf( fpout, "Rounds       %d\n",	skill->beats	);
	if ( skill->guild != -1 )
	  fprintf( fpout, "Guild        %d\n",	skill->guild	);
	if ( skill->skill_fun )
	  fprintf( fpout, "Code         %s\n",	skill->skill_fun_name );
	else
	if ( skill->spell_fun )
	  fprintf( fpout, "Code         %s\n",	skill->spell_fun_name );
	fprintf( fpout, "Dammsg       %s~\n",	skill->noun_damage );
	if ( skill->msg_off && skill->msg_off[0] != '\0' )
	  fprintf( fpout, "Wearoff      %s~\n",	skill->msg_off	);

	if ( skill->hit_char && skill->hit_char[0] != '\0' )
	  fprintf( fpout, "Hitchar      %s~\n",	skill->hit_char );
	if ( skill->hit_vict && skill->hit_vict[0] != '\0' )
	  fprintf( fpout, "Hitvict      %s~\n",	skill->hit_vict );
	if ( skill->hit_room && skill->hit_room[0] != '\0' )
	  fprintf( fpout, "Hitroom      %s~\n",	skill->hit_room );

	if ( skill->miss_char && skill->miss_char[0] != '\0' )
	  fprintf( fpout, "Misschar     %s~\n",	skill->miss_char );
	if ( skill->miss_vict && skill->miss_vict[0] != '\0' )
	  fprintf( fpout, "Missvict     %s~\n",	skill->miss_vict );
	if ( skill->miss_room && skill->miss_room[0] != '\0' )
	  fprintf( fpout, "Missroom     %s~\n",	skill->miss_room );

	if ( skill->die_char && skill->die_char[0] != '\0' )
	  fprintf( fpout, "Diechar      %s~\n",	skill->die_char );
	if ( skill->die_vict && skill->die_vict[0] != '\0' )
	  fprintf( fpout, "Dievict      %s~\n",	skill->die_vict );
	if ( skill->die_room && skill->die_room[0] != '\0' )
	  fprintf( fpout, "Dieroom      %s~\n",	skill->die_room );

	if ( skill->imm_char && skill->imm_char[0] != '\0' )
	  fprintf( fpout, "Immchar      %s~\n",	skill->imm_char );
	if ( skill->imm_vict && skill->imm_vict[0] != '\0' )
	  fprintf( fpout, "Immvict      %s~\n",	skill->imm_vict );
	if ( skill->imm_room && skill->imm_room[0] != '\0' )
	  fprintf( fpout, "Immroom      %s~\n",	skill->imm_room );

	if ( skill->dice && skill->dice[0] != '\0' )
	  fprintf( fpout, "Dice         %s~\n",	skill->dice );
	if ( skill->value )
	  fprintf( fpout, "Value        %d\n",	skill->value );
	if ( skill->difficulty )
	  fprintf( fpout, "Difficulty   %d\n",	skill->difficulty );
	if ( skill->participants )
	  fprintf( fpout, "Participants %d\n",	skill->participants );
	if ( skill->components && skill->components[0] != '\0' )
	  fprintf( fpout, "Components   %s~\n",	skill->components );
	if ( skill->teachers && skill->teachers[0] != '\0' )
	  fprintf( fpout, "Teachers     %s~\n",	skill->teachers );
	for ( aff = skill->affects; aff; aff = aff->next )
	  fprintf( fpout, "Affect       '%s' %d '%s' %d\n",
	  	aff->duration, aff->location, aff->modifier, aff->bitvector );
	if ( skill->alignment )
	  fprintf( fpout, "Alignment   %d\n",	skill->alignment );
	
	if ( skill->type != SKILL_HERB )
	{
	    fprintf( fpout, "Minlevel     %d\n",	skill->min_level	);
	}
	fprintf( fpout, "Maxlevel     %d\n",		skill->max_level	);
	fprintf( fpout, "End\n\n" );
}

/*
 * Save the skill table to disk
 */
void save_skill_table(int delnum)
{
    int x;
    FILE *fpout;

    if ( (fpout=fopen( SKILL_FILE, "w" )) == NULL )
    {
	bug( "Cannot open skills.dat for writting", 0 );
	perror( SKILL_FILE );
	return;
    }

    for ( x = 0; x < top_sn; x++ )
    {
	if (x == delnum)
	 continue;
	if ( !skill_table[x]->name || skill_table[x]->name[0] == '\0' )
	   break;
	fprintf( fpout, "#SKILL\n" );
	fwrite_skill( fpout, skill_table[x] );
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}

/*
 * Save the herb table to disk
 */
void save_herb_table()
{
    int x;
    FILE *fpout;

    if ( (fpout=fopen( HERB_FILE, "w" )) == NULL )
    {
	bug( "Cannot open herbs.dat for writting", 0 );
	perror( HERB_FILE );
	return;
    }

    for ( x = 0; x < top_herb; x++ )
    {
	if ( !herb_table[x]->name || herb_table[x]->name[0] == '\0' )
	   break;
	fprintf( fpout, "#HERB\n" );
	fwrite_skill( fpout, herb_table[x] );
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}

/*
 * Save the socials to disk
 */
void save_socials()
{
    FILE *fpout;
    SOCIALTYPE *social;
    int x;

    if ( (fpout=fopen( SOCIAL_FILE, "w" )) == NULL )
    {
	bug( "Cannot open socials.dat for writting", 0 );
	perror( SOCIAL_FILE );
	return;
    }

    for ( x = 0; x < 27; x++ )
    {
	for ( social = social_index[x]; social; social = social->next )
	{
	    if ( !social->name || social->name[0] == '\0' )
	    {
		bug( "Save_socials: blank social in hash bucket %d", x );
		continue;
	    }
	    fprintf( fpout, "#SOCIAL\n" );
	    fprintf( fpout, "Name        %s~\n",	social->name );
	    if ( social->char_no_arg )
		fprintf( fpout, "CharNoArg   %s~\n",	social->char_no_arg );
	    else
	        bug( "Save_socials: NULL char_no_arg in hash bucket %d", x );
	    if ( social->others_no_arg )
		fprintf( fpout, "OthersNoArg %s~\n",	social->others_no_arg );
	    if ( social->char_found )
		fprintf( fpout, "CharFound   %s~\n",	social->char_found );
	    if ( social->others_found )
		fprintf( fpout, "OthersFound %s~\n",	social->others_found );
	    if ( social->vict_found )
		fprintf( fpout, "VictFound   %s~\n",	social->vict_found );
	    if ( social->char_auto )
		fprintf( fpout, "CharAuto    %s~\n",	social->char_auto );
	    if ( social->others_auto )
		fprintf( fpout, "OthersAuto  %s~\n",	social->others_auto );
	    fprintf( fpout, "End\n\n" );
	}
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}

int get_skill( char *skilltype )
{
    if ( !str_cmp( skilltype, "Spell" ) )
      return SKILL_SPELL;
    if ( !str_cmp( skilltype, "Skill" ) )
      return SKILL_SKILL;
    if ( !str_cmp( skilltype, "Weapon" ) )
      return SKILL_WEAPON;
    if ( !str_cmp( skilltype, "Tongue" ) )
      return SKILL_TONGUE;
    if ( !str_cmp( skilltype, "Herb" ) )
      return SKILL_HERB;
    return SKILL_UNKNOWN;
}

/*
 * Save the commands to disk
 */
void save_commands()
{
    FILE *fpout;
    CMDTYPE *command;
    int x;

    if ( (fpout=fopen( COMMAND_FILE, "w" )) == NULL )
    {
	bug( "Cannot open commands.dat for writing", 0 );
	perror( COMMAND_FILE );
	return;
    }

    for ( x = 0; x < 126; x++ )
    {
	for ( command = command_hash[x]; command; command = command->next )
	{
	    if ( !command->name || command->name[0] == '\0' )
	    {
		bug( "Save_commands: blank command in hash bucket %d", x );
		continue;
	    }
	    fprintf( fpout, "#COMMAND\n" );
	    fprintf( fpout, "Name        %s~\n", command->name		);
	    fprintf( fpout, "Code        %s\n",	 command->fun_name?command->fun_name:"" ); // Modded to use new field - Trax
	    fprintf( fpout, "Position    %d\n",	 command->position	);
	    fprintf( fpout, "Level       %d\n",	 command->level		);
	    fprintf( fpout, "Log         %d\n",	 command->log		);
            fprintf( fpout, "Ooc         %d\n",  command->ooc           );

	    fprintf( fpout, "End\n\n" );
	}
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}

SKILLTYPE *fread_skill( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    SKILLTYPE *skill;

    CREATE( skill, SKILLTYPE, 1 );

    skill->guild = -1;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    KEY( "Alignment",	skill->alignment,	fread_number( fp ) );
	    if ( !str_cmp( word, "Affect" ) )
	    {
		SMAUG_AFF *aff;

		CREATE( aff, SMAUG_AFF, 1 );
		aff->duration = str_dup( fread_word( fp ) );
		aff->location = fread_number( fp );
		aff->modifier = str_dup( fread_word( fp ) );
		aff->bitvector = fread_number( fp );
		aff->next = skill->affects;
		skill->affects = aff;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'C':
	    if ( !str_cmp( word, "Code" ) )
	    {
		SPELL_FUN *spellfun;
		DO_FUN *dofun;
		char *w = fread_word( fp );

		fMatch = TRUE;
		if( !str_prefix( "do_", w ) && ( dofun = skill_function(w) ) != skill_notfound )
		{
		   skill->skill_fun = dofun;
		   skill->spell_fun = NULL;
		   skill->skill_fun_name = str_dup(w);
		}
		else if( str_prefix( "do_", w ) && ( spellfun = spell_function(w) ) != spell_notfound )
		{
		   skill->spell_fun = spellfun;
		   skill->skill_fun = NULL;
		   skill->spell_fun_name = str_dup(w);
		}
		else
		{
		   bug( "fread_skill: unknown skill/spell %s", w );
		   skill->spell_fun = spell_null;
		}
		break;
	    }
	    KEY( "Components",	skill->components,	fread_string_nohash( fp ) );
	    break;

	case 'D':
            KEY( "Dammsg",	skill->noun_damage,	fread_string_nohash( fp ) );
	    KEY( "Dice",	skill->dice,		fread_string_nohash( fp ) );
	    KEY( "Diechar",	skill->die_char,	fread_string_nohash( fp ) );
	    KEY( "Dieroom",	skill->die_room,	fread_string_nohash( fp ) );
	    KEY( "Dievict",	skill->die_vict,	fread_string_nohash( fp ) );
	    KEY( "Difficulty",	skill->difficulty,	fread_number( fp ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
		return skill;
	    break;

	case 'F':
	    KEY( "Flags",	skill->flags,		fread_number( fp ) );
	    break;

	case 'G':
	    KEY( "Guild",	skill->guild,		fread_number( fp ) );
	    break;

	case 'H':
	    KEY( "Hitchar",	skill->hit_char,	fread_string_nohash( fp ) );
	    KEY( "Hitroom",	skill->hit_room,	fread_string_nohash( fp ) );
	    KEY( "Hitvict",	skill->hit_vict,	fread_string_nohash( fp ) );
	    break;

	case 'I':
	    KEY( "Immchar",	skill->imm_char,	fread_string_nohash( fp ) );
	    KEY( "Immroom",	skill->imm_room,	fread_string_nohash( fp ) );
	    KEY( "Immvict",	skill->imm_vict,	fread_string_nohash( fp ) );
	    break;

	case 'M':
	    KEY( "Mana",	skill->min_mana,	fread_number( fp ) );
	    KEY( "Maxlevel",	skill->max_level,	fread_number( fp ) );
	    KEY( "Minlevel",	skill->min_level,	fread_number( fp ) );
	    KEY( "Minpos",	skill->minimum_position, fread_number( fp ) );
	    KEY( "Misschar",	skill->miss_char,	fread_string_nohash( fp ) );
	    KEY( "Missroom",	skill->miss_room,	fread_string_nohash( fp ) );
	    KEY( "Missvict",	skill->miss_vict,	fread_string_nohash( fp ) );
	    break;

	case 'N':
            KEY( "Name",	skill->name,		fread_string_nohash( fp ) );
	    break;

	case 'P':
	    KEY( "Participants",skill->participants,	fread_number( fp ) );

	    if ( !str_cmp(word, "Prereq") )
	    {
		SKILL_REQ * req;
		char * name;

		CREATE(req, SKILL_REQ, 1);
		req->level = fread_number( fp );
		name = fread_string( fp );
		// bkm
	    }

	    break;

	case 'R':
	    KEY( "Rounds",	skill->beats,		fread_number( fp ) );
	    break;

	case 'S':
	    KEY( "Slot",	skill->slot,		fread_number( fp ) );
	    KEY( "Saves",	skill->saves,		fread_number( fp ) );
	    break;

	case 'T':
	    KEY( "Target",	skill->target,		fread_number( fp ) );
	    KEY( "Teachers",	skill->teachers,	fread_string_nohash( fp ) );
	    KEY( "Type",	skill->type,  get_skill(fread_word( fp ))  );
	    break;

	case 'V':
	    KEY( "Value",	skill->value,		fread_number( fp ) );
	    break;

	case 'W':
	    KEY( "Wearoff",	skill->msg_off,		fread_string_nohash( fp ) );
	    break;
	}

	if ( !fMatch )
	{
            sprintf( buf, "Fread_skill: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void load_skill_table()
{
    FILE *fp;

    if ( ( fp = fopen( SKILL_FILE, "r" ) ) != NULL )
    {
	top_sn = 0;
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
                bug( "Load_skill_table: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "SKILL"      ) )
	    {
		if ( top_sn >= MAX_SKILL )
		{
		    bug( "load_skill_table: more skills than MAX_SKILL %d", MAX_SKILL );
		    fclose( fp );
		    return;
		}
		skill_table[top_sn++] = fread_skill( fp );
		continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_skill_table: bad section.", 0 );
		continue;
	    }
	}
	fclose( fp );
    }
    else
    {
	bug( "Cannot open skills.dat", 0 );
 	exit(0);
    }
}

void load_herb_table()
{
    FILE *fp;

    if ( ( fp = fopen( HERB_FILE, "r" ) ) != NULL )
    {
	top_herb = 0;
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_herb_table: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "HERB"      ) )
	    {
		if ( top_herb >= MAX_HERB )
		{
		    bug( "load_herb_table: more herbs than MAX_HERB %d", MAX_HERB );
		    fclose( fp );
		    return;
		}
		herb_table[top_herb++] = fread_skill( fp );
		if ( herb_table[top_herb-1]->slot == 0 )
		    herb_table[top_herb-1]->slot = top_herb-1;
		continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_herb_table: bad section.", 0 );
		continue;
	    }
	}
	fclose( fp );
    }
    else
    {
	bug( "Cannot open herbs.dat", 0 );
 	exit(0);
    }
}

void fread_social( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    SOCIALTYPE *social;

    CREATE( social, SOCIALTYPE, 1 );

    for ( ;; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'C':
	    KEY( "CharNoArg",	social->char_no_arg,	fread_string_nohash(fp) );
	    KEY( "CharFound",	social->char_found,	fread_string_nohash(fp) );
	    KEY( "CharAuto",	social->char_auto,	fread_string_nohash(fp) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !social->name )
		{
		    bug( "Fread_social: Name not found", 0 );
		    free_social( social );
		    return;
		}
		if ( !social->char_no_arg )
		{
		    bug( "Fread_social: CharNoArg not found", 0 );
		    free_social( social );
		    return;
		}
		add_social( social );
		return;
	    }
	    break;

	case 'N':
	    KEY( "Name",	social->name,		fread_string_nohash(fp) );
	    break;

	case 'O':
	    KEY( "OthersNoArg",	social->others_no_arg,	fread_string_nohash(fp) );
	    KEY( "OthersFound",	social->others_found,	fread_string_nohash(fp) );
	    KEY( "OthersAuto",	social->others_auto,	fread_string_nohash(fp) );
	    break;

	case 'V':
	    KEY( "VictFound",	social->vict_found,	fread_string_nohash(fp) );
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_social: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void load_socials()
{
    FILE *fp;

    if ( ( fp = fopen( SOCIAL_FILE, "r" ) ) != NULL )
    {
	top_sn = 0;
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
                bug( "Load_socials: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "SOCIAL"      ) )
	    {
                fread_social( fp );
	    	continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_socials: bad section.", 0 );
		continue;
	    }
	}
	fclose( fp );
    }
    else
    {
	bug( "Cannot open socials.dat", 0 );
 	exit(0);
    }
}

void fread_command( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    CMDTYPE *command;

    CREATE( command, CMDTYPE, 1 );

    for ( ;; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'C':
	    KEY( "Code",	command->fun_name, str_dup( fread_word( fp ) ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if( !command->name )
		{
		   bug( "%s", "Fread_command: Name not found" );
		   free_command( command );
		   return;
		}
		if( !command->fun_name )
		{
		   bug( "fread_command: No function name supplied for %s", command->name );
		   free_command( command );
		   return;
		}
		/*
	 	 * Mods by Trax
		 * Fread in code into char* and try linkage here then
		 * deal in the "usual" way I suppose..
		 */
	      command->do_fun = skill_function( command->fun_name );
		if( command->do_fun == skill_notfound )
		{
		   bug( "Fread_command: Function %s not found for %s", command->fun_name, command->name );
		   free_command( command );
		   return;
		}
             	if ( !command->ooc)
		  command->ooc = 0;

		add_command( command );
		return;
	    }
	    break;

	case 'L':
	    KEY( "Level",	command->level,		fread_number(fp) );
	    KEY( "Log",		command->log,		fread_number(fp) );
	    break;

	case 'N':
	    KEY( "Name",	command->name,		fread_string_nohash(fp) );
	    break;

	case 'O':
	   KEY( "Ooc",		command->ooc,		fread_number(fp) );
	   break;

	case 'P':
	    KEY( "Position",	command->position,	fread_number(fp) );
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_command: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void load_commands()
{
    FILE *fp;

    if ( ( fp = fopen( COMMAND_FILE, "r" ) ) != NULL )
    {
	top_sn = 0;
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
                bug( "Load_commands: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "COMMAND"      ) )
	    {
                fread_command( fp );
	    	continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_commands: bad section.", 0 );
		continue;
	    }
	}
	fclose( fp );
    }
    else
    {
	bug( "Cannot open commands.dat", 0 );
 	exit(0);
    }

}

void copy_files_contents(FILE *fsource, FILE *fdestination)
{
   int ch;
   int cnt = 1;
   
   for (;;)
   {
      ch = fgetc( fsource );
      if (!feof(fsource))
      {
          fputc( ch, fdestination);
          if (ch == '\n')
          {
             cnt++;
             if (cnt >= LAST_FILE_SIZE) //limit size of this file please :-)
                break;
          }
      }
      else
          break;
   }
}

void write_last_file(char *entry)
{
   FILE *fpout;
   FILE *fptemp;
   char filename[MAX_INPUT_LENGTH];
   char tempname[MAX_INPUT_LENGTH];
   
   sprintf(filename, "%s", LAST_LIST);
   sprintf(tempname, "%s", LAST_TEMP_LIST);
   if ((fptemp = fopen(tempname, "w")) == NULL)
   {
      bug("Cannot open: %s for writing", tempname);
      return;
   }
   fprintf(fptemp, "%s\n", entry); //adds new entry to top of the file
   if ((fpout = fopen(filename, "r")) != NULL)
   {
      copy_files_contents(fpout, fptemp); //copy the rest to the file
      fclose(fpout); //close the files since writing is done
   }
   fclose(fptemp);
   
   if (remove(filename) != 0 && fopen(filename, "r") != NULL)
   {
      bug("Do not have permission to delete the %s file", filename);
      return;
   }
   if (rename(tempname, filename) != 0)
   {
      bug("Do not have permission to rename the %s file", tempname);
      return;
   }
   return;
}

void read_last_file(CHAR_DATA *ch, int count, char *name)
{
   FILE *fpout;
   char filename[MAX_INPUT_LENGTH];
   char charname[100];
   int cnt = 0;
   int letter = 0;
   char *ln;
   char *c;
   char d, e;
   struct tm *tme;
   time_t now;
   char day[MAX_INPUT_LENGTH];
   char sday[5];
   int fnd = 0;
   
   sprintf(filename, "%s", LAST_LIST);
   if ((fpout = fopen(filename, "r")) == NULL)
   {
      send_to_char("There is no last file to look at.\n\r", ch);
      return;
   }
   
   for (;;)
   {
      if (feof(fpout))
      {
         fclose(fpout);
         ch_printf(ch, "&B---------------------------------------------------------------------------\n\r&W%d &CEntries Listed.\n\r", cnt);
         return;
      }
      else
      {
         if (count == -2 || ++cnt <= count || count == -1)
         {
            ln = fread_line(fpout);
            strcpy(charname, "");
            if (name) //looking for a certain name
            {
               c = ln; 
               for (;;)
               {
                  if (isalpha(*c) && !isspace(*c))
                  {
                     charname[letter] = *c;   
                     letter++;
                     c++;
                  }
                  else
                  {
                     charname[letter] = '\0';
                     if (!str_cmp(charname, name))
                     {
                        ch_printf(ch, "%s", ln);
                        letter = 0;
                        strcpy(charname, "");
                        break;  
                     }
                     else
                     {
                        if (!feof(fpout))
                        {
                           fread_line(fpout);    
                           c = ln;
                           letter = 0;
                           strcpy(charname, "");
                           continue;
                        }
                        else
                        {
                           cnt--;
                           break;
                        }
                     }
                  }
               }
            }
            else if (count == -2) //only today's entries
            {
               c = ln;
               now = time(0);
               tme = localtime(&now);
               strftime(day, 10, "%d", tme);
               for (;;)
               {
                  if (!isdigit(*c))
                  {
                     c++;
                  }
                  else
                  {
                     d = *c;
                     c++;
                     e = *c;
                     sprintf(sday, "%c%c", d, e);
                     if (!str_cmp(sday, day))
                     {
                         fnd = 1;
                         cnt++;
                         ch_printf(ch, "%s", ln);
                         break;
                     }
                     else
                     {
                        if (fnd == 1)
                        {
                           fclose(fpout);
                           ch_printf(ch, "&B---------------------------------------------------------------------------\n\r&W%d &CEntries Listed.\n\r", cnt);
                           return;
                        }
                        else
                           break;
                     }
                  }
               }
            }
            else                  
            {  
               ch_printf(ch, "%s", ln);
            }
               
         }
         else
         {
            fclose(fpout);
            ch_printf(ch, "&B--------------------------------------------------------------------------\n\r&W%d &CEntries Listed.\n\r", count);
            return;
         }
      }
   }
}
