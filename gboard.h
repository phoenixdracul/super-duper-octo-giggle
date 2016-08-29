/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 							 Global Boards Header	 					    *
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

/*
 * This is not needed here. Needs to be put into the connected_state enum.
 * These values will most likely cause conflicts....
 *
#define CON_NOTE_TO						10
#define CON_NOTE_SUBJECT				11
#define CON_NOTE_EXPIRE					12
#define CON_NOTE_TEXT					13
#define CON_NOTE_FINISH					14
 *
 */


#define NOTE_DIR "../gnotes/" /* set it to something you like */

#define DEF_NORMAL  0 /* No forced change, but default (any string)   */
#define DEF_INCLUDE 1 /* 'names' MUST be included (only ONE name!)    */
#define DEF_EXCLUDE 2 /* 'names' must NOT be included (one name only) */

#define MAX_BOARD 	  9

#define DEFAULT_BOARD 0 /* default board is board #0 in the boards      */
                        /* It should be readable by everyone!           */
                        
#define MAX_LINE_LENGTH 80 /* enforce a max length of 80 on text lines, reject longer lines 
*/
						   /* This only applies in the Body of the 
note */                        
						   
#define MAX_NOTE_TEXT (4*MAX_STRING_LENGTH - 1000)
						
#define BOARD_NOTFOUND -1 /* Error code from board_lookup() and board_number */

/* Data about a board */
struct global_board_data
{
    char *short_name; /* Max 8 chars */
    char *long_name;  /* Explanatory text, should be no more than 40 ? chars */

    int read_level; /* minimum level to see board */
    int write_level;/* minimum level to post notes */

    char *names;       /* Default recipient */
    int force_type; /* Default action (DEF_XXX) */

    int purge_days; /* Default expiration */

    /* Non-constant data */

    NOTE_DATA *note_first; /* pointer to board's first note */
    BOOL changed; /* currently unused */

};

typedef struct global_board_data GLOBAL_BOARD_DATA;


/* External variables */

extern GLOBAL_BOARD_DATA boards[MAX_BOARD]; /* Declare */


/* Prototypes */

void finish_note(GLOBAL_BOARD_DATA *board, NOTE_DATA *note); /* attach a note to a board */
void free_note  (NOTE_DATA *note); /* deallocate memory used by a note */
void load_global_boards(void); /* load all boards */
int board_lookup(const char *name); /* Find a board with that name */
BOOL gb_is_note_to (CHAR_DATA *ch, NOTE_DATA *note); /* is tha note to ch? */
void personal_message (const char *sender, const char *to, const char *subject, const int expire_days, const char *text);
void make_note(const char* board_name, const char *sender, const char *to, const char *subject, const int expire_days, const char *text);
void save_notes (void);

/* for nanny */
void handle_con_note_to       	(DESCRIPTOR_DATA *d, char * argument);
void handle_con_note_subject 	(DESCRIPTOR_DATA *d, char * argument);
void handle_con_note_expire 	(DESCRIPTOR_DATA *d, char * argument);
void handle_con_note_text      	(DESCRIPTOR_DATA *d, char * argument);
void handle_con_note_finish 	(DESCRIPTOR_DATA *d, char * argument);
