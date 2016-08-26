// tga area loading

#include "mud.h"


void shutdown_mud( char *reason );
char    *fread_flagstring   args(( FILE * fp ));
char * fread_string(FILE * fp);
int get_actflag( char *flag );
int get_rflag( char *flag );
int get_rflag2( char *flag );
int get_exflag( char *flag );
void   		mprog_read_programs     args ( ( FILE* fp,
		MOB_INDEX_DATA *pMobIndex) );
void   		oprog_read_programs     args ( ( FILE* fp,
		OBJ_INDEX_DATA *pObjIndex) );
void   		rprog_read_programs     args ( ( FILE* fp,
		ROOM_INDEX_DATA *pRoomIndex) );
void    renumber_put_resets  args( ( ROOM_INDEX_DATA *room ) );
void	boot_log		args( ( const char *str, ... ) );
void load_tga_room_reset( ROOM_INDEX_DATA * room, FILE * fp );


extern bool fBootDb;
extern MOB_INDEX_DATA *	mob_index_hash		[MAX_KEY_HASH];
extern OBJ_INDEX_DATA *	obj_index_hash		[MAX_KEY_HASH];
extern ROOM_INDEX_DATA *	room_index_hash		[MAX_KEY_HASH];
extern int			top_affect;
extern int			top_area;
extern int			top_ed;
extern int			top_exit;
extern int			top_help;
extern int			top_mob_index;
extern int			top_obj_index;
extern int			top_reset;
extern int			top_room;
extern int			top_shop;
extern int			top_repair;
extern int			top_vroom;


void load_tga_mobiles( AREA_DATA * tarea, FILE * fp )
{
   MOB_INDEX_DATA *pMobIndex;
   char *ln;
   int x1, x2, x3, x4, x5, x6, x7;
   char * buf2 = NULL;
   int i;

   if( !tarea )
   {
      bug( "Load_mobiles: no #AREA seen yet." );
      if( fBootDb )
      {
         shutdown_mud( "No #AREA" );
         exit( 1 );
      }
      else
         return;
   }

   for( ;; )
   {
      char buf[MAX_STRING_LENGTH];
      int vnum;
      char letter;
      int iHash;
      bool oldmob;
      bool tmpBootDb;

      letter = fread_letter( fp );
      if( letter != '#' )
      {
         bug( "Load_mobiles: # not found." );
         if( fBootDb )
         {
            shutdown_mud( "# not found" );
            exit( 1 );
         }
         else
            return;
      }

      vnum = fread_number( fp );
      if( vnum == 0 )
         break;

      tmpBootDb = fBootDb;
      fBootDb = FALSE;
      if( get_mob_index( vnum ) )
      {
         if( tmpBootDb )
         {
            bug( "Load_mobiles: vnum %d duplicated.", vnum );
            shutdown_mud( "duplicate vnum" );
            exit( 1 );
         }
         else
         {
            pMobIndex = get_mob_index( vnum );
            sprintf( buf, "Cleaning mobile: %d", vnum );
            log_string_plus( buf, LOG_BUILD, LEVEL_IMMORTAL );
            clean_mob( pMobIndex );
            oldmob = TRUE;
         }
      }
      else
      {
         oldmob = FALSE;
         CREATE( pMobIndex, MOB_INDEX_DATA, 1 );
      }
      fBootDb = tmpBootDb;

      pMobIndex->vnum = vnum;
      if( fBootDb )
      {
         if( !tarea->low_m_vnum )
            tarea->low_m_vnum = vnum;
         else if( vnum < tarea->low_m_vnum )
            tarea->low_m_vnum = vnum;
         if( vnum > tarea->hi_m_vnum )
            tarea->hi_m_vnum = vnum;
      }
//      pMobIndex->area = tarea;
      pMobIndex->player_name = fread_string( fp );
      pMobIndex->short_descr = fread_string( fp );
      pMobIndex->long_descr = fread_string( fp );
      pMobIndex->description = fread_string( fp );
      pMobIndex->mudprogs = NULL;

      pMobIndex->long_descr[0] = UPPER( pMobIndex->long_descr[0] );
      pMobIndex->description[0] = UPPER( pMobIndex->description[0] );

      char *actflags = NULL;
      char *affectflags = NULL;
      char flag[MIL];
      int value;

      actflags = fread_flagstring( fp );

      while( actflags[0] != '\0' )
      {
         actflags = one_argument( actflags, flag );
         value = get_actflag( flag );
         
         if( value < 0 || value >= MAX_BITS )
            bug( "Unknown actflag: %s\r\n", flag );
         else
            SET_ACT_FLAG( pMobIndex, value);
      }

      affectflags = fread_flagstring( fp );

      while( affectflags[0] != '\0' )
      {
         affectflags = one_argument( affectflags, flag );
         value = get_aflag( flag );
         if( value < 0 || value > 31 )
            bug( "Unknown affectflag: %s\r\n", flag );
         else
            SET_BIT( pMobIndex->affected_by, 1 << value );
      }

      SET_ACT_FLAG( pMobIndex, ACT_IS_NPC );

      pMobIndex->pShop = NULL;
      pMobIndex->rShop = NULL;
      pMobIndex->alignment = fread_number( fp );
      letter = fread_letter( fp );
      pMobIndex->level = fread_number( fp );

      pMobIndex->mobthac0 = fread_number( fp );
      pMobIndex->ac = fread_number( fp );
      pMobIndex->hitnodice = fread_number( fp );
      /*
       * 'd'      
       */ fread_letter( fp );
      pMobIndex->hitsizedice = fread_number( fp );
      /*
       * '+'      
       */ fread_letter( fp );
      pMobIndex->hitplus = fread_number( fp );
      pMobIndex->damnodice = fread_number( fp );
      /*
       * 'd'      
       */ fread_letter( fp );
      pMobIndex->damsizedice = fread_number( fp );
      /*
       * '+'      
       */ fread_letter( fp );
      pMobIndex->damplus = fread_number( fp );
      pMobIndex->gold = fread_number( fp );
      pMobIndex->exp = fread_number( fp );

      int position = 8;
      buf2 = fread_flagstring( fp );
      if (!str_cmp(buf2, "sitting"))
         position = 6;
      else if (!str_cmp(buf2, "resting"))
         position = 5;
      else if (!str_cmp(buf2, "sleeping"))
         position = 4;
      if( position < 0 || position >= POS_MAX )
      {
         bug( "load_mobiles: vnum %d: Mobile in invalid position! Defaulting to standing.", vnum );
         position = POS_STANDING;
      }
      pMobIndex->position = position;

//      position = get_npc_position( fread_flagstring( fp ) );
      position = 8;
      buf2 = fread_flagstring( fp );
      if (!str_cmp(buf2, "sitting"))
         position = 6;
      else if (!str_cmp(buf2, "resting"))
         position = 5;
      else if (!str_cmp(buf2, "sleeping"))
         position = 4;
      if( position < 0 || position >= POS_MAX )
      {
         bug( "load_mobiles: vnum %d: Mobile in invalid default position! Defaulting to standing.", vnum );
         position = POS_STANDING;
      }
      pMobIndex->defposition = position;

      buf2 = fread_flagstring( fp );
//      int sex = get_npc_sex( fread_flagstring( fp ) );
      int sex;
      if (!str_cmp(buf2, "male"))
         sex = 1;
      else if (!str_cmp(buf2, "female"))
         sex = 2;
      else
         sex = 0;
/*      if( sex < 0 || sex >= SEX_MAX )
      {
         bug( "load_mobiles: vnum %d: Mobile has invalid sex! Defaulting to neuter.", vnum );
         sex = SEX_NEUTRAL;
      } */
      pMobIndex->sex = sex;

      buf2 = fread_string( fp );
      pMobIndex->race = 0;
      for (i = 0; i < MAX_NPC_RACE; ++i)
      {
         if (!str_cmp(buf2, npc_race[i]))
         {
            pMobIndex->race = i;
            break;
         }
      }
//      pMobIndex->race_name = fread_string( fp );
//      RACE_DATA *race = NULL;

      //if the race is deleted, set them as monster
//      if( ( race = get_race_by_name( pMobIndex->race_name, TRUE, TRUE ) ) == NULL )
//         RESTRALLOC( pMobIndex->race_name, "Monster" );


      char *speaks = NULL;
      char *speaking = NULL;

      speaks = fread_flagstring( fp );
      while( speaks[0] != '\0' )
      {
         speaks = one_argument( speaks, flag );
//         value = get_langnum( flag );
         for (value = 31; value >= 0; --value)
         {
            if (!str_cmp(flag, lang_names[value]))
               break;
         }
         if (!str_cmp(flag, "the universal language"))
            value = LANG_CLAN;
         if( value == -1 )
            bug( "Unknown speaks language: %s\r\n", flag );
         else if( value == LANG_CLAN )
            SET_BIT( pMobIndex->speaks, VALID_LANGS );
         else
            SET_BIT( pMobIndex->speaks, (1 << value) );
      }

		if ( !pMobIndex->speaks )
			pMobIndex->speaks = race_table[pMobIndex->race].language | LANG_BASIC;

      speaking = fread_flagstring( fp );

      while( speaking[0] != '\0' )
      {
         speaking = one_argument( speaking, flag );
//         value = get_langnum( flag );
         for (value = 31; value >= 0; --value)
         {
            if (!str_cmp(flag, lang_names[value]))
               break;
         }
         if (!str_cmp(flag, "the universal language"))
            value = LANG_CLAN;
         if( value == -1 )
            bug( "Unknown speaking language: %s\r\n", flag );
         else if( value == LANG_CLAN )
            SET_BIT( pMobIndex->speaking, VALID_LANGS );
         else
            SET_BIT( pMobIndex->speaking, (1 << value) );
      }

		if ( !pMobIndex->speaking )
			pMobIndex->speaking = race_table[pMobIndex->race].language;

/*      if( xIS_EMPTY( pMobIndex->speaks ) )
         xSET_BIT( pMobIndex->speaks, LANG_BASIC );
      if( xIS_EMPTY( pMobIndex->speaking ) )
         xSET_BIT( pMobIndex->speaking, LANG_BASIC ); */

      char *bodyparts = NULL;
      char *resist = NULL;
      char *immune = NULL;
      char *suscep = NULL;
      char *attacks = NULL;
      char *defenses = NULL;

      bodyparts = fread_flagstring( fp );

/*      while( bodyparts[0] != '\0' )
      {
         bodyparts = one_argument( bodyparts, flag );
         value = get_partflag( flag );
         if( value < 0 || value > 31 )
            bug( "Unknown bodypart: %s", flag );
         else
            SET_BIT( pMobIndex->xflags, 1 << value );
      } */

      resist = fread_flagstring( fp );

/*      while( resist[0] != '\0' )
      {
         resist = one_argument( resist, flag );
         value = get_risflag( flag );
         if( value < 0 || value >= MAX_RIS_FLAG )
            bug( "Unknown RIS flag (R): %s", flag );
         else
            SET_RESIS( pMobIndex, 1 << value );
      } */

      immune = fread_flagstring( fp );

/*      while( immune[0] != '\0' )
      {
         immune = one_argument( immune, flag );
         value = get_risflag( flag );
         if( value < 0 || value >= MAX_RIS_FLAG )
            bug( "Unknown RIS flag (I): %s", flag );
         else
            SET_IMMUNE( pMobIndex, 1 << value );
      } */

      suscep = fread_flagstring( fp );

/*      while( suscep[0] != '\0' )
      {
         suscep = one_argument( suscep, flag );
         value = get_risflag( flag );
         if( value < 0 || value >= MAX_RIS_FLAG )
            bug( "Unknown RIS flag (S): %s", flag );
         else
            SET_SUSCEP( pMobIndex, 1 << value );
      } */
      attacks = fread_flagstring( fp );

/*      while( attacks[0] != '\0' )
      {
         attacks = one_argument( attacks, flag );
         value = get_attackflag( flag );
         if( value < 0 || value > 31 )
            bug( "Unknown attackflag: %s\r\n", flag );
         else
            SET_ATTACK( pMobIndex, 1 << value );
      } */

      defenses = fread_flagstring( fp );

/*      while( defenses[0] != '\0' )
      {
         defenses = one_argument( defenses, flag );
         value = get_defenseflag( flag );
         if( value < 0 || value > 31 )
            bug( "Unknown defenseflag: %s\r\n", flag );
         else
            SET_DEFENSE( pMobIndex, 1 << value );
      } */

      if( letter != 'S' && letter != 'C' && letter != 'Z' )
      {
         bug( "Load_mobiles: vnum %d: letter '%c' not Z, S or C.", vnum, letter );
         shutdown_mud( "bad mob data" );
         exit( 1 );
      }
      if( letter == 'S' || letter == 'C' || letter == 'Z' ) /* Realms complex mob  -Thoric  */
      {
         pMobIndex->perm_str = fread_number( fp );
         pMobIndex->perm_int = fread_number( fp );
         pMobIndex->perm_wis = fread_number( fp );
         pMobIndex->perm_dex = fread_number( fp );
         pMobIndex->perm_con = fread_number( fp );
         pMobIndex->perm_cha = fread_number( fp );
         pMobIndex->perm_lck = fread_number( fp );
         pMobIndex->saving_poison_death = fread_number( fp );
         pMobIndex->saving_wand = fread_number( fp );
         pMobIndex->saving_para_petri = fread_number( fp );
         pMobIndex->saving_breath = fread_number( fp );
         pMobIndex->saving_spell_staff = fread_number( fp );
         ln = fread_line( fp );
         x1 = x2 = x3 = x4 = x5 = x6 = x7 = 0;
         sscanf( ln, "%d %d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6, &x7 );
         pMobIndex->height = x2;
         pMobIndex->weight = x3;
         pMobIndex->numattacks = x4;
         pMobIndex->hitroll = x5;
         pMobIndex->damroll = x6;
      }
      else
      {
         pMobIndex->perm_str = 10;
         pMobIndex->perm_dex = 10;
         pMobIndex->perm_int = 10;
         pMobIndex->perm_wis = 10;
         pMobIndex->perm_cha = 10;
         pMobIndex->perm_con = 10;
         pMobIndex->perm_lck = 10;
         pMobIndex->numattacks = 1;
      }

      letter = fread_letter( fp );
      if( letter == '>' )
      {
         ungetc( letter, fp );
         mprog_read_programs( fp, pMobIndex );
      }
      else
         ungetc( letter, fp );

      if( !oldmob )
      {
         iHash = vnum % MAX_KEY_HASH;
         pMobIndex->next = mob_index_hash[iHash];
         mob_index_hash[iHash] = pMobIndex;
         top_mob_index++;
      }
   }

   return;
}

void load_tga_objects( AREA_DATA * tarea, FILE * fp, bool stockfote )
{
   OBJ_INDEX_DATA *pObjIndex = NULL;
   char letter;
   char *ln;
   int x1, x2, x3, x4, x5, x6, x7;

   if( !tarea )
   {
      bug( "Load_objects: no #AREA seen yet." );
      if( fBootDb )
      {
         shutdown_mud( "No #AREA" );
         exit( 1 );
      }
      else
         return;
   }
   for( ;; )
   {
      char buf[MAX_STRING_LENGTH];
      int vnum;
      int iHash;
      bool tmpBootDb;
      bool oldobj;

      letter = fread_letter( fp );
      if( letter != '#' )
      {
         bug( "Load_objects: # not found." );
         if( fBootDb )
         {
            shutdown_mud( "# not found" );
            exit( 1 );
         }
         else
            return;
      }

      vnum = fread_number( fp );
      if( vnum == 0 )
         break;

      tmpBootDb = fBootDb;
      fBootDb = FALSE;
      if( get_obj_index( vnum ) )
      {
         if( tmpBootDb )
         {
            bug( "Load_objects: vnum %d duplicated.", vnum );
            shutdown_mud( "duplicate vnum" );
            exit( 1 );
         }
         else
         {
            pObjIndex = get_obj_index( vnum );
            sprintf( buf, "Cleaning object: %d", vnum );
            log_string_plus( buf, LOG_BUILD, LEVEL_IMMORTAL );
            clean_obj( pObjIndex );
            oldobj = TRUE;
         }
      }
      else
      {
         oldobj = FALSE;
         CREATE( pObjIndex, OBJ_INDEX_DATA, 1 );
      }
      fBootDb = tmpBootDb;
      pObjIndex->vnum = vnum;
//      pObjIndex->area = tarea;
      if( fBootDb )
      {
         if( !tarea->low_o_vnum )
            tarea->low_o_vnum = vnum;
         else if( vnum < tarea->low_o_vnum )
            tarea->low_o_vnum = vnum;
         if( vnum > tarea->hi_o_vnum )
            tarea->hi_o_vnum = vnum;
      }
      pObjIndex->name = fread_string( fp );
      pObjIndex->short_descr = fread_string( fp );
      pObjIndex->description = fread_string( fp );
      pObjIndex->action_desc = fread_string( fp );
      pObjIndex->mudprogs = NULL;
      pObjIndex->description[0] = UPPER( pObjIndex->description[0] );
      {
         int value = -1;

         value = get_otype( fread_flagstring( fp ) );

         if( value < 0 )
         {
            bug( "load_objects: vnum %d: Object has invalid type! Defaulting to trash.", vnum );
            value = get_otype( "trash" );
         }

         pObjIndex->item_type = value;

         {
            char *eflags = NULL;
            char *wflags = NULL;
            char flag[MIL];

            eflags = fread_flagstring( fp );

            while( eflags[0] != '\0' )
            {
               eflags = one_argument( eflags, flag );
               value = get_oflag( flag );
               if( value < 0 || value > 31 )
                  bug( "Unknown object extraflag: %s\r\n", flag );
               else
                  SET_BIT( pObjIndex->extra_flags, (1 << value) );
            }

            wflags = fread_flagstring( fp );

            while( wflags[0] != '\0' )
            {
               wflags = one_argument( wflags, flag );
               value = get_wflag( flag );
               if( value < 0 || value > 31 )
                  bug( "Unknown wear flag: %s", flag );
               else
                  SET_BIT( pObjIndex->wear_flags, (1 << value) );
            }
         }
      }
      {
         char *magflags = NULL;
         char flag[MIL];
         int value;

         magflags = fread_flagstring( fp );

/*         while( magflags[0] != '\0' )
         {
            magflags = one_argument( magflags, flag );
            value = get_magflag( flag );
            if( value < 0 || value > 31 )
               bug( "Unknown magic flag: %s", flag );
            else
               SET_BIT( pObjIndex->magic_flags, (1 << value) );
         } */
      }

      pObjIndex->layers = fread_number( fp );


      ln = fread_line( fp );
      x1 = x2 = x3 = x4 = x5 = x6 = x7 = 0;
      sscanf( ln, "%d %d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6, &x7 );
      pObjIndex->value[0] = x1;
      pObjIndex->value[1] = x2;
      pObjIndex->value[2] = x3;
      pObjIndex->value[3] = x4;
      pObjIndex->value[4] = x5;
      pObjIndex->value[5] = x6;
      pObjIndex->value[6] = x7;
      pObjIndex->weight = fread_number( fp );
      pObjIndex->weight = UMAX( 1, pObjIndex->weight );
      pObjIndex->cost = fread_number( fp );
      pObjIndex->rent = fread_number( fp );  /* unused */

      for( ;; )
      {
         letter = fread_letter( fp );

         if( letter == 'A' )
         {
            AFFECT_DATA *paf;

            CREATE( paf, AFFECT_DATA, 1 );
            paf->type = -1;
            paf->duration = -1;
            paf->location = fread_number( fp );
            if( paf->location == APPLY_WEAPONSPELL
                || paf->location == APPLY_WEARSPELL || paf->location == APPLY_REMOVESPELL || paf->location == APPLY_STRIPSN )
               paf->modifier = slot_lookup( fread_number( fp ) );
            else
               paf->modifier = fread_number( fp );
            paf->bitvector = 0;
            LINK( paf, pObjIndex->first_affect, pObjIndex->last_affect, next, prev );
            top_affect++;
         }

         else if( letter == 'E' )
         {
            EXTRA_DESCR_DATA *ed;

            CREATE( ed, EXTRA_DESCR_DATA, 1 );
            ed->keyword = fread_string( fp );
            ed->description = fread_string( fp );
            LINK( ed, pObjIndex->first_extradesc, pObjIndex->last_extradesc, next, prev );
            top_ed++;
         }
         else if( letter == '>' )
         {
            ungetc( letter, fp );
            oprog_read_programs( fp, pObjIndex );
         }

         else
         {
            ungetc( letter, fp );
            break;
         }
      }

      /*
       * Translate spell "slot numbers" to internal "skill numbers."
       */
      switch ( pObjIndex->item_type )
      {
         case ITEM_POTION:
            pObjIndex->value[1] = slot_lookup( pObjIndex->value[1] );
            pObjIndex->value[2] = slot_lookup( pObjIndex->value[2] );
            pObjIndex->value[3] = slot_lookup( pObjIndex->value[3] );
            break;

         case ITEM_DEVICE:
            pObjIndex->value[3] = slot_lookup( pObjIndex->value[3] );
            break;
         case ITEM_SALVE:
            pObjIndex->value[4] = slot_lookup( pObjIndex->value[4] );
            pObjIndex->value[5] = slot_lookup( pObjIndex->value[5] );
            break;
      }

/*      if( pObjIndex && pObjIndex->item_type == ITEM_WEAPON && ( pObjIndex->value[6] == 0 || pObjIndex->value[6] > 10 ) )
      {
         if( pObjIndex->value[3] == WEAPON_BLASTER_PISTOL )
         {
            pObjIndex->value[6] = 2;
         }
         else if( pObjIndex->value[3] == WEAPON_BLASTER_RIFLE )
         {
            pObjIndex->value[6] = 1;
         }
         else if( pObjIndex->value[3] == WEAPON_BLASTER_REPEAT )
         {
            pObjIndex->value[6] = 4;
         }
         else
            pObjIndex->value[6] = 0;
      } */

      if( !oldobj )
      {
         iHash = vnum % MAX_KEY_HASH;
         pObjIndex->next = obj_index_hash[iHash];
         obj_index_hash[iHash] = pObjIndex;
         top_obj_index++;
      }
   }

   return;
}

void load_tga_rooms( AREA_DATA * tarea, FILE * fp, bool stockfote )
{
   ROOM_INDEX_DATA *pRoomIndex;
   char buf[MAX_STRING_LENGTH];
   char *ln;

   if( !tarea )
   {
      bug( "Load_rooms: no #AREA seen yet." );
      shutdown_mud( "No #AREA" );
      exit( 1 );
   }


   tarea->first_room = tarea->last_room = NULL;

   for( ;; )
   {
      int vnum;
      char letter;
      int door;
      int iHash;
      bool tmpBootDb;
      bool oldroom;

      int x1, x2, x3, x4, x5, x6, x7;

      letter = fread_letter( fp );
      if( letter != '#' )
      {
         bug( "Load_rooms: # not found." );
         if( fBootDb )
         {
            shutdown_mud( "# not found" );
            exit( 1 );
         }
         else
            return;
      }

      vnum = fread_number( fp );
      if( vnum == 0 )
         break;

      tmpBootDb = fBootDb;
      fBootDb = FALSE;
      if( get_room_index( vnum ) != NULL )
      {
         if( tmpBootDb )
         {
            bug( "Load_rooms: vnum %d duplicated.", vnum );
            shutdown_mud( "duplicate vnum" );
            exit( 1 );
         }
         else
         {
            pRoomIndex = get_room_index( vnum );
            sprintf( buf, "Cleaning room: %d", vnum );
            log_string_plus( buf, LOG_BUILD, LEVEL_IMMORTAL );
            clean_room( pRoomIndex );
            oldroom = TRUE;
         }
      }
      else
      {
         oldroom = FALSE;
         CREATE( pRoomIndex, ROOM_INDEX_DATA, 1 );
         pRoomIndex->first_person = NULL;
         pRoomIndex->last_person = NULL;
         pRoomIndex->first_content = NULL;
         pRoomIndex->last_content = NULL;
      }

      fBootDb = tmpBootDb;
      pRoomIndex->area = tarea;
      pRoomIndex->vnum = vnum;
      pRoomIndex->first_extradesc = NULL;
      pRoomIndex->last_extradesc = NULL;
      pRoomIndex->mudprogs = NULL;

      if( fBootDb )
      {
         if( !tarea->low_r_vnum )
            tarea->low_r_vnum = vnum;
         else if( vnum < tarea->low_r_vnum )
            tarea->low_r_vnum = vnum;
         if( vnum > tarea->hi_r_vnum )
            tarea->hi_r_vnum = vnum;
      }
      pRoomIndex->name = fread_string( fp );
      pRoomIndex->description = fread_string( fp );

      char *roomflags = NULL;
      char flag[MIL];
      int value;

      roomflags = fread_flagstring( fp );

      while( roomflags[0] != '\0' )
      {
         roomflags = one_argument( roomflags, flag );
         value = get_rflag( flag );
         if( value < 0 || value > 31 )
         {
            value = get_rflag2(flag);
            if (value < 0 || value > 31)
               bug( "Unknown roomflag: %s\r\n", flag );
            else
               SET_BIT( pRoomIndex->room_flags2, (1 << value));
         }
         else
            SET_BIT( pRoomIndex->room_flags, (1 << value) );
      }

      {
         int sector = 0;
         char * flags = fread_flagstring( fp );
         char * flag = NULL;
         int i;
         
//         flags = one_argument(flags, flag);
         
         for (i = 0; i < SECT_MAX; ++i)
         {
            if (!str_cmp(flags, sector_name[i]))
               break;
         }

         if( sector < 0 || sector >= SECT_MAX )
         {
            bug( "Room #%d has bad sector type.", vnum );
            sector = 1;
         }

         pRoomIndex->sector_type = sector;
      }

      int area_number = 0;
      area_number = fread_number( fp );

      if( area_number > 0 )
      {
         ln = fread_line( fp );
         x1 = x2 = x3 = 0;
         sscanf( ln, "%d %d %d", &x1, &x2, &x3 );

         pRoomIndex->tele_delay = x1;
         pRoomIndex->tele_vnum = x2;
         pRoomIndex->tunnel = x3;
      }
      else
      {
         pRoomIndex->tele_delay = 0;
         pRoomIndex->tele_vnum = 0;
         pRoomIndex->tunnel = 0;
      }

      pRoomIndex->light = 0;
      pRoomIndex->first_exit = NULL;
      pRoomIndex->last_exit = NULL;

      for( ;; )
      {
         letter = fread_letter( fp );

         if( letter == 'S' )
            break;

         if( letter == 'D' )
         {
            EXIT_DATA *pexit;

            door = get_dir( fread_flagstring( fp ) );

            if( door < 0 || door > 10 )
            {
               bug( "Fread_rooms: vnum %d has bad door number %d.", vnum, door );
               if( fBootDb )
                  exit( 1 );
            }
            else
            {
               char *exitflags = NULL;

/*               if( tarea->version < 4 )
               {
                  pexit = make_exit( pRoomIndex, NULL, door );
                  pexit->description = fread_string( fp );
                  pexit->keyword = fread_string( fp );
                  exitflags = fread_flagstring( fp );

                  while( exitflags[0] != '\0' )
                  {
                     exitflags = one_argument( exitflags, flag );
                     value = get_exflag( flag );
                     if( value < 0 || value > MAX_EXFLAG )
                        bug( "Unknown exitflag: %s\r\n", flag );
                     else
                        SET_EXIT_FLAG( pexit, 1 << value );
                  }
                  int hasxdist = 0;

                  hasxdist = fread_number( fp );
                  ln = fread_line( fp );
                  x1 = x2 = x3 = x4 = x5 = 0;

                  if( hasxdist > 0 )
                  {
                     sscanf( ln, "%d %d %d %d", &x1, &x2, &x3, &x4 );

                     pexit->key = x1;
                     pexit->vnum = x2;
                     pexit->distance = x3;
                     pexit->keypad = x4;
                  }
                  else
                  {
                     sscanf( ln, "%d %d %d", &x1, &x2, &x3 );
                     pexit->key = x1;
                     pexit->vnum = x2;
                     pexit->keypad = x3;
                  }
               }
               else */
               {
                  pexit = make_exit( pRoomIndex, NULL, door );
                  pexit->description = fread_string( fp );
                  pexit->keyword = fread_string( fp );
                  //   if( tarea->version > 5 )
                  //     pexit->exit_name = fread_string( fp );

                  exitflags = fread_flagstring( fp );

                  while( exitflags[0] != '\0' )
                  {
                     exitflags = one_argument( exitflags, flag );
                     value = get_exflag( flag );
                     if( value < 0 || value > MAX_EXFLAG )
                        bug( "Unknown exitflag: %s\r\n", flag );
                     else
                        SET_BIT( pexit->exit_info, (1 << value) );
                  }
                  int hasxdist = 0;

                  hasxdist = fread_number( fp );
                  ln = fread_line( fp );

                  x1 = x2 = x3 = x4 = x5 = x6 = x7 = 0;

                  if( hasxdist > 0 )
                  {
                     sscanf( ln, "%d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6 );

                     pexit->key = x1;
                     pexit->vnum = x2;
                     pexit->distance = x3;
//                     pexit->mx = x4;
//                     pexit->my = x5;
                     pexit->keypad = x6;
                  }
                  else
                  {
                     sscanf( ln, "%d %d %d %d %d", &x1, &x2, &x3, &x4, &x5 );
                     pexit->key = x1;
                     pexit->vnum = x2;
//                     pexit->mx = x3;
//                     pexit->my = x4;
                     pexit->keypad = x5;
                  }
               }
            }
         }
         else if( letter == 'E' )
         {
            EXTRA_DESCR_DATA *ed;

            CREATE( ed, EXTRA_DESCR_DATA, 1 );
            ed->keyword = fread_string( fp );
            ed->description = fread_string( fp );
            LINK( ed, pRoomIndex->first_extradesc, pRoomIndex->last_extradesc, next, prev );
            top_ed++;
         }

         else if( letter == 'R' )
            load_tga_room_reset( pRoomIndex, fp );

         else if( letter == '>' )
         {
            ungetc( letter, fp );
            rprog_read_programs( fp, pRoomIndex );
         }
         else
         {
            bug( "Load_rooms: vnum %d has flag '%c' not 'DES'.", vnum, letter );
            shutdown_mud( "Room flag not DES" );
            exit( 1 );
         }

      }

      if( !oldroom )
      {
         iHash = vnum % MAX_KEY_HASH;
         pRoomIndex->next = room_index_hash[iHash];
         room_index_hash[iHash] = pRoomIndex;
         LINK( pRoomIndex, tarea->first_room, tarea->last_room, next_aroom, prev_aroom );
         top_room++;
      }
   }

   return;
}

void load_tga_flags( AREA_DATA * tarea, FILE * fp )
{
   char *areaflags = NULL;
   char flag[MIL];
   int value;

   if( !tarea )
   {
      bug( "%s", "Load_flags: no #AREA seen yet." );
      if( fBootDb )
      {
         shutdown_mud( "No #AREA" );
         exit( 1 );
      }
      else
         return;
   }

   areaflags = fread_flagstring( fp );

   while( areaflags[0] != '\0' )
   {
      areaflags = one_argument( areaflags, flag );
      if (!str_cmp(flag, "nopkill"))
         SET_BIT(tarea->flags, AFLAG_NOPKILL);
/*      value = get_areaflag( flag );
      if( value < 0 || value > 31 )
         bug( "Unknown area flag: %s", flag );
      else
         SET_BIT( tarea->flags, 1 << value ); */
   }
   return;
}

void load_tga_continent( AREA_DATA * tarea, FILE * fp )
{
   char *pointer;
   int value;


   if( !tarea )
   {
      bug( "Load_continent: no #AREA seen yet." );
      if( fBootDb )
      {
         shutdown_mud( "No #AREA" );
         exit( 1 );
      }
      else
         return;
   }

   pointer = fread_string_nohash( fp );
//   value = get_continent( pointer );
   DISPOSE( pointer );
   
/*   if( value < 0 || value > ACON_MAX )
   {
      tarea->continent = 0;
      bug( "%s", "load_continent: Invalid area continent, set to 'alsherok' by default." );
   }
   else
      tarea->continent = value; */

   return;
}

void load_tga_resets( AREA_DATA * tarea, FILE * fp )
{
   ROOM_INDEX_DATA *pRoomIndex = NULL;
   ROOM_INDEX_DATA *roomlist;
   bool not01 = FALSE;
   int count = 0;

   if( !tarea )
   {
      bug( "%s", "Load_resets: no #AREA seen yet." );
      if( fBootDb )
      {
         shutdown_mud( "No #AREA" );
         exit( 1 );
      }
      else
         return;
   }

   if( !tarea->first_room )
   {
      bug( "%s: No #ROOMS section found. Cannot load resets.", __FUNCTION__ );
      if( fBootDb )
      {
         shutdown_mud( "No #ROOMS" );
         exit( 1 );
      }
      else
         return;
   }

   for( ;; )
   {
      EXIT_DATA *pexit;
      char letter;
      int extra, arg1, arg2, arg3;
      short arg4, arg5, arg6;

      if( ( letter = fread_letter( fp ) ) == 'S' )
         break;

      if( letter == '*' )
      {
         fread_to_eol( fp );
         continue;
      }

      extra = fread_number( fp );
      if( letter == 'M' || letter == 'O' )
         extra = 0;
      arg1 = fread_number( fp );
      arg2 = fread_number( fp );
      arg3 = ( letter == 'G' || letter == 'R' ) ? 0 : fread_number( fp );
      arg4 = arg5 = arg6 = -1;
//      if( tarea->version > 2 )
      {
         if( letter == 'O' || letter == 'M' )
         {
            arg4 = (short)fread_number( fp );
            arg5 = (short)fread_number( fp );
            arg6 = (short)fread_number( fp );
         }
      }
      fread_to_eol( fp );
      ++count;

      /*
       * Validate parameters.
       * We're calling the index functions for the side effect.
       */
      switch ( letter )
      {
         default:
            bug( "%s: bad command '%c'.", __FUNCTION__, letter );
            if( fBootDb )
               boot_log( "%s: %s (%d) bad command '%c'.", __FUNCTION__, tarea->filename, count, letter );
            return;

         case 'M':
            if( get_mob_index( arg1 ) == NULL && fBootDb )
               boot_log( "%s: %s (%d) 'M': mobile %d doesn't exist.", __FUNCTION__, tarea->filename, count, arg1 );

            if( ( pRoomIndex = get_room_index( arg3 ) ) == NULL && fBootDb )
               boot_log( "%s: %s (%d) 'M': room %d doesn't exist.", __FUNCTION__, tarea->filename, count, arg3 );
            else
               add_reset( pRoomIndex, letter, extra, arg1, arg2, arg3 /*, arg4, arg5, arg6*/ );
            break;

         case 'O':
            if( get_obj_index( arg1 ) == NULL && fBootDb )
               boot_log( "%s: %s (%d) '%c': object %d doesn't exist.", __FUNCTION__, tarea->filename, count, letter, arg1 );

            if( ( pRoomIndex = get_room_index( arg3 ) ) == NULL && fBootDb )
               boot_log( "%s: %s (%d) '%c': room %d doesn't exist.", __FUNCTION__, tarea->filename, count, letter, arg3 );
            else
            {
               if( !pRoomIndex )
                  bug( "%s: Unable to add room reset - room not found.", __FUNCTION__ );
               else
                  add_reset( pRoomIndex, letter, extra, arg1, arg2, arg3 /*, arg4, arg5, arg6*/ );
            }
            break;

         case 'P':
            if( get_obj_index( arg1 ) == NULL && fBootDb )
               boot_log( "%s: %s (%d) '%c': object %d doesn't exist.", __FUNCTION__, tarea->filename, count, letter, arg1 );
            if( arg3 > 0 )
            {
               if( get_obj_index( arg3 ) == NULL && fBootDb )
                  boot_log( "%s: %s (%d) 'P': destination object %d doesn't exist.", __FUNCTION__, tarea->filename, count,
                            arg3 );
               if( extra > 1 )
                  not01 = TRUE;
            }
            if( !pRoomIndex )
               bug( "%s: Unable to add room reset - room not found.", __FUNCTION__ );
            else
            {
               if( arg3 == 0 )
                  arg3 = OBJ_VNUM_MONEY_ONE; // This may look stupid, but for some reason it works.
               add_reset( pRoomIndex, letter, extra, arg1, arg2, arg3 /*, arg4, arg5, arg6*/ );
            }
            break;

         case 'G':
         case 'E':
            if( get_obj_index( arg1 ) == NULL && fBootDb )
               boot_log( "%s: %s (%d) '%c': object %d doesn't exist.", __FUNCTION__, tarea->filename, count, letter, arg1 );
            if( !pRoomIndex )
               bug( "%s: Unable to add room reset - room not found.", __FUNCTION__ );
            else
               add_reset( pRoomIndex, letter, extra, arg1, arg2, arg3 /*, arg4, arg5, arg6*/ );
            break;

         case 'H':
            bug( "%s: Unable to convert legacy hide reset. Must be converted manually.", __FUNCTION__ );
            break;

         case 'D':
            if( !( pRoomIndex = get_room_index( arg1 ) ) )
            {
               bug( "%s: 'D': room %d doesn't exist.", __FUNCTION__, arg1 );
               bug( "Reset: %c %d %d %d %d", letter, extra, arg1, arg2, arg3 );
               if( fBootDb )
                  boot_log( "%s: %s (%d) 'D': room %d doesn't exist.", __FUNCTION__, tarea->filename, count, arg1 );
               break;
            }

            if( arg2 < 0 || arg2 > MAX_DIR + 1 || !( pexit = get_exit( pRoomIndex, arg2 ) )
                || !IS_SET( pexit->exit_info, EX_ISDOOR ) )
            {
               bug( "%s: 'D': exit %d not door.", __FUNCTION__, arg2 );
               bug( "Reset: %c %d %d %d %d", letter, extra, arg1, arg2, arg3 );
               if( fBootDb )
                  boot_log( "%s: %s (%d) 'D': exit %d not door.", __FUNCTION__, tarea->filename, count, arg2 );
            }

            if( arg3 < 0 || arg3 > 2 )
            {
               bug( "%s: 'D': bad 'locks': %d.", __FUNCTION__, arg3 );
               if( fBootDb )
                  boot_log( "%s: %s (%d) 'D': bad 'locks': %d.", __FUNCTION__, tarea->filename, count, arg3 );
            }
            add_reset( pRoomIndex, letter, extra, arg1, arg2, arg3 /*, arg4, arg5, arg6*/ );
            break;

         case 'R':
            if( !( pRoomIndex = get_room_index( arg1 ) ) && fBootDb )
               boot_log( "%s: %s (%d) 'R': room %d doesn't exist.", __FUNCTION__, tarea->filename, count, arg1 );
            else
               add_reset( pRoomIndex, letter, extra, arg1, arg2, arg3 /*, arg4, arg5, arg6*/ );
            if( arg2 < 0 || arg2 > 10 )
            {
               bug( "%s: 'R': bad exit %d.", __FUNCTION__, arg2 );
               if( fBootDb )
                  boot_log( "%s: %s (%d) 'R': bad exit %d.", __FUNCTION__, tarea->filename, count, arg2 );
               break;
            }
            break;
      }
   }
   if( !not01 )
   {
      for( roomlist = tarea->first_room; roomlist; roomlist = roomlist->next_aroom )
         renumber_put_resets( roomlist );
   }
   return;
}

void load_tga_room_reset( ROOM_INDEX_DATA * room, FILE * fp )
{
   EXIT_DATA *pexit;
   char letter;
   int extra, arg1, arg2, arg3;
   short arg4 = -1, arg5 = -1, arg6 = -1;
   bool not01 = FALSE;
   int count = 0;

   letter = fread_letter( fp );
   extra = fread_number( fp );
   if( letter == 'M' || letter == 'O' )
      extra = 0;
   arg1 = fread_number( fp );
   arg2 = fread_number( fp );
   arg3 = ( letter == 'G' || letter == 'R' ) ? 0 : fread_number( fp );
//   if( room->area->version > 3 )
   {
      if( letter == 'O' || letter == 'M' )
      {
         arg4 = (short)fread_number( fp );
         arg5 = (short)fread_number( fp );
         arg6 = (short)fread_number( fp );
      }
   }

   fread_to_eol( fp );
   ++count;

   /*
    * Validate parameters.
    * We're calling the index functions for the side effect.
    */
   switch ( letter )
   {
      default:
         bug( "%s: bad command '%c'.", __FUNCTION__, letter );
         if( fBootDb )
            boot_log( "%s: %s (%d) bad command '%c'.", __FUNCTION__, room->area->filename, count, letter );
         return;

      case 'M':
         if( get_mob_index( arg1 ) == NULL && fBootDb /*&& resetdebug*/ )
            boot_log( "%s: %s (%d) 'M': mobile %d doesn't exist.", __FUNCTION__, room->area->filename, count, arg1 );
         break;

      case 'O':
         if( get_obj_index( arg1 ) == NULL && fBootDb /*&& resetdebug*/ )
            boot_log( "%s: %s (%d) '%c': object %d doesn't exist.", __FUNCTION__, room->area->filename, count, letter,
                      arg1 );
         break;

      case 'P':
         if( get_obj_index( arg1 ) == NULL && fBootDb /*&& resetdebug*/ )
            boot_log( "%s: %s (%d) '%c': object %d doesn't exist.", __FUNCTION__, room->area->filename, count, letter,
                      arg1 );

         if( arg3 <= 0 )
            arg3 = OBJ_VNUM_MONEY_ONE; // This may look stupid, but for some reason it works.
         if( get_obj_index( arg3 ) == NULL && fBootDb /*&& resetdebug*/ )
            boot_log( "%s: %s (%d) 'P': destination object %d doesn't exist.", __FUNCTION__, room->area->filename, count,
                      arg3 );
         if( extra > 1 )
            not01 = TRUE;
         break;

      case 'G':
      case 'E':
         if( get_obj_index( arg1 ) == NULL && fBootDb /*&& resetdebug*/ )
            boot_log( "%s: %s (%d) '%c': object %d doesn't exist.", __FUNCTION__, room->area->filename, count, letter,
                      arg1 );
         break;

      case 'T':
      case 'H':
         break;

      case 'D':
         if( arg2 < 0 || arg2 > MAX_DIR + 1 || !( pexit = get_exit( room, arg2 ) )
             || !IS_SET( pexit->exit_info, EX_ISDOOR ) )
         {
            bug( "%s: 'D': exit %d not door (AREA:%s) (VNUM:%d)", __FUNCTION__, arg2, room->area->filename, room->vnum );
            bug( "Reset: %c %d %d %d %d", letter, extra, arg1, arg2, arg3 );
            if( fBootDb )
               boot_log( "%s: %s (%d) 'D': exit %d not door (AREA:%s) (VNUM:%d)", __FUNCTION__, room->area->filename,
                         count, arg2, room->area->filename, room->vnum );
         }

         if( arg3 < 0 || arg3 > 2 )
         {
            bug( "%s: 'D': bad 'locks': %d (AREA:%s) (VNUM:%d)", __FUNCTION__, arg3, room->area->filename, room->vnum );
            if( fBootDb )
               boot_log( "%s: %s (%d) 'D': bad 'locks': %d (AREA:%s) (VNUM:%d)", __FUNCTION__, room->area->filename,
                         count, arg3, room->area->filename, room->vnum );
         }
         break;

      case 'R':
         if( arg2 < 0 || arg2 > 10 )
         {
            bug( "%s: 'R': bad exit %d (AREA:%s) (VNUM:%d)", __FUNCTION__, arg2, room->area->filename, room->vnum );
            if( fBootDb )
               boot_log( "%s: %s (%d) 'R': bad exit %d (AREA:%s) (VNUM:%d)", __FUNCTION__, room->area->filename, count,
                         arg2, room->area->filename, room->vnum );
            break;
         }
         break;
   }
   add_reset( room, letter, extra, arg1, arg2, arg3 /*, arg4, arg5, arg6*/ );

   if( !not01 )
      renumber_put_resets( room );
   return;
}

void load_tga_shops( AREA_DATA * tarea, FILE * fp )
{
   SHOP_DATA *pShop;

   for( ;; )
   {
      MOB_INDEX_DATA *pMobIndex;
      int iTrade;

      CREATE( pShop, SHOP_DATA, 1 );
      pShop->keeper = fread_number( fp );
      if( pShop->keeper == 0 )
      {
         DISPOSE( pShop );
         break;
      }
      for( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
         pShop->buy_type[iTrade] = fread_number( fp );
      pShop->profit_buy = fread_number( fp );
      pShop->profit_sell = fread_number( fp );
      pShop->profit_buy = URANGE( pShop->profit_sell + 5, pShop->profit_buy, 1000 );
      pShop->profit_sell = URANGE( 0, pShop->profit_sell, pShop->profit_buy - 5 );
      pShop->open_hour = fread_number( fp );
      pShop->close_hour = fread_number( fp );
      pShop->clanshop = 0;
      pShop->playershop = 0;
      pShop->owner = STRALLOC("");
      fread_to_eol( fp );
      pMobIndex = get_mob_index( pShop->keeper );
      pMobIndex->pShop = pShop;

      if( !first_shop )
         first_shop = pShop;
      else
         last_shop->next = pShop;
      pShop->next = NULL;
      pShop->prev = last_shop;
      last_shop = pShop;
      top_shop++;
   }
   return;
}
