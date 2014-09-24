/*--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--+
 *																			*
 *                        _-- ADVANCED UNIVERSE --_ 						*
 *                         													*
 * 							 Do Functions Header	 					    *
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


//Seperated Do Functions in hopes to clean things up -KeB
//No more need for tables.c since Trax's and Samson's Dynamic command support

//Order:
//Do functions
//Force functions
//Spell functions



/*Do Functions List*/
DECLARE_DO_FUN( do_aaccept       );
DECLARE_DO_FUN( do_aassign	);
DECLARE_DO_FUN( do_accelerate );
DECLARE_DO_FUN( do_accept	);
DECLARE_DO_FUN( do_account  );
DECLARE_DO_FUN( do_accountstat );
DECLARE_DO_FUN( do_add_patrol );
DECLARE_DO_FUN( do_addbounty    );
DECLARE_DO_FUN( do_addchange );
DECLARE_DO_FUN( do_addpilot );
DECLARE_DO_FUN( do_addsenator	);
DECLARE_DO_FUN( do_adecline      );
DECLARE_DO_FUN( do_advance	);
DECLARE_DO_FUN( do_affected     );
DECLARE_DO_FUN( do_afk          );
DECLARE_DO_FUN( do_ahall        );
DECLARE_DO_FUN( do_ahelp	);
DECLARE_DO_FUN( do_aid		);
DECLARE_DO_FUN( do_allow	);
DECLARE_DO_FUN( do_allships );
DECLARE_DO_FUN( do_allspeeders  );
DECLARE_DO_FUN( do_ambush );
DECLARE_DO_FUN( do_ammo );
DECLARE_DO_FUN( do_ansi		);
DECLARE_DO_FUN( do_answer	);
DECLARE_DO_FUN( do_apply	);
DECLARE_DO_FUN( do_appoint );
DECLARE_DO_FUN( do_appraise	);
DECLARE_DO_FUN( do_areas	);
DECLARE_DO_FUN( do_arena        );
DECLARE_DO_FUN( do_arm );
DECLARE_DO_FUN( do_aset		);
DECLARE_DO_FUN( do_ask		);
DECLARE_DO_FUN( do_assignpilot	);
DECLARE_DO_FUN( do_astat	);
DECLARE_DO_FUN( do_at		);
DECLARE_DO_FUN( do_attack_run );
DECLARE_DO_FUN( do_auction	);
DECLARE_DO_FUN( do_authorize	);
DECLARE_DO_FUN( do_autopilot  );
DECLARE_DO_FUN( do_autorecharge );
DECLARE_DO_FUN( do_autotrack );
DECLARE_DO_FUN( do_avtalk	);
DECLARE_DO_FUN( do_awho         );
DECLARE_DO_FUN( do_backstab	);
DECLARE_DO_FUN( do_backup	);
DECLARE_DO_FUN( do_balzhur	);
DECLARE_DO_FUN( do_bamfin	);
DECLARE_DO_FUN( do_bamfout	);
DECLARE_DO_FUN( do_ban		);
DECLARE_DO_FUN( do_bank  );
DECLARE_DO_FUN( do_barrel_roll);
DECLARE_DO_FUN( do_bash		);
DECLARE_DO_FUN( do_bashdoor     );
DECLARE_DO_FUN( do_battle_command );
DECLARE_DO_FUN( do_beep         );
DECLARE_DO_FUN( do_beg  );
DECLARE_DO_FUN( do_berserk	);
DECLARE_DO_FUN( do_bestow	);
DECLARE_DO_FUN( do_bestowarea	);
DECLARE_DO_FUN( do_bet          );
DECLARE_DO_FUN( do_bind );
DECLARE_DO_FUN( do_bio		);
DECLARE_DO_FUN( do_bite		);
DECLARE_DO_FUN( do_board        );
DECLARE_DO_FUN( do_boards	);
DECLARE_DO_FUN( do_bodybag	);
DECLARE_DO_FUN( do_bomb );
DECLARE_DO_FUN( do_bomb		);
DECLARE_DO_FUN( do_bounties     );
DECLARE_DO_FUN( do_brandish	);
DECLARE_DO_FUN( do_brew 	);
DECLARE_DO_FUN( do_bribe );
DECLARE_DO_FUN( do_bset		);
DECLARE_DO_FUN( do_bstat	);
DECLARE_DO_FUN( do_bug		);
DECLARE_DO_FUN( do_buildnet );
DECLARE_DO_FUN( do_buildwalk );
DECLARE_DO_FUN( do_bury		);
DECLARE_DO_FUN( do_buy		);
DECLARE_DO_FUN( do_buyhome );
DECLARE_DO_FUN( do_buymobship );
DECLARE_DO_FUN( do_buyship );
DECLARE_DO_FUN( do_buytroops );
DECLARE_DO_FUN( do_buyvendor);	// TODO
DECLARE_DO_FUN( do_buzz         );
DECLARE_DO_FUN( do_calculate );
DECLARE_DO_FUN( do_capture );
DECLARE_DO_FUN( do_cast		);
DECLARE_DO_FUN( do_cedit	);
DECLARE_DO_FUN( do_chaff );
DECLARE_DO_FUN( do_challenge    );
DECLARE_DO_FUN( do_changes );
DECLARE_DO_FUN( do_channels	);
DECLARE_DO_FUN( do_chaos        );
DECLARE_DO_FUN( do_chat		);
DECLARE_DO_FUN( do_check_vnums  );
DECLARE_DO_FUN( do_checkbeacons );
DECLARE_DO_FUN( do_checkprints	);
DECLARE_DO_FUN( do_checkwar   );
DECLARE_DO_FUN( do_chedit       );
DECLARE_DO_FUN( do_circle	);
DECLARE_DO_FUN( do_clan_donate );
DECLARE_DO_FUN( do_clan_withdraw );
DECLARE_DO_FUN( do_clanbuyship );
DECLARE_DO_FUN( do_clanbuytroops );
DECLARE_DO_FUN( do_clangiveship );
DECLARE_DO_FUN( do_clans	);
DECLARE_DO_FUN( do_clansalvage );
DECLARE_DO_FUN( do_clanstat	);
DECLARE_DO_FUN( do_clantalk	);
DECLARE_DO_FUN( do_claw		);
DECLARE_DO_FUN( do_cleanroom	);
DECLARE_DO_FUN( do_climb	);
DECLARE_DO_FUN( do_clone  );
DECLARE_DO_FUN( do_close	);
DECLARE_DO_FUN( do_closebay );
DECLARE_DO_FUN( do_closehatch );
DECLARE_DO_FUN( do_cmdtable	);
DECLARE_DO_FUN( do_cmenu	);
DECLARE_DO_FUN( do_codenet );
DECLARE_DO_FUN( do_codecrack	);
DECLARE_DO_FUN( do_collectgold  );
DECLARE_DO_FUN( do_color	);
DECLARE_DO_FUN( do_commands	);
DECLARE_DO_FUN( do_comment	);
DECLARE_DO_FUN( do_compare	);
DECLARE_DO_FUN( do_concealment	);
DECLARE_DO_FUN( do_config	);
DECLARE_DO_FUN( do_consider	);
DECLARE_DO_FUN( do_contract    );
DECLARE_DO_FUN( do_copyover	);
DECLARE_DO_FUN( do_copyship  );
DECLARE_DO_FUN( do_credits	);
DECLARE_DO_FUN( do_cset		);
DECLARE_DO_FUN( do_cut		);
DECLARE_DO_FUN( do_debitorder   );
DECLARE_DO_FUN( do_deception );
DECLARE_DO_FUN( do_decline	);
DECLARE_DO_FUN( do_deities	);
DECLARE_DO_FUN( do_demote );
DECLARE_DO_FUN( do_deny		);
DECLARE_DO_FUN( do_describe );
DECLARE_DO_FUN( do_description	);
DECLARE_DO_FUN( do_designship   );
DECLARE_DO_FUN( do_destro       );
DECLARE_DO_FUN( do_destroy      );
DECLARE_DO_FUN( do_detrap	);
DECLARE_DO_FUN( do_devote	);
DECLARE_DO_FUN( do_diagnose	);
DECLARE_DO_FUN( do_dig		);
DECLARE_DO_FUN( do_disableship	);
DECLARE_DO_FUN( do_disarm	);
DECLARE_DO_FUN( do_disarmgrenade );
DECLARE_DO_FUN( do_disconnect	);
DECLARE_DO_FUN( do_disguise );
DECLARE_DO_FUN( do_dismount	);
DECLARE_DO_FUN( do_divorce	);
DECLARE_DO_FUN( do_dmesg	);
DECLARE_DO_FUN( do_down		);
DECLARE_DO_FUN( do_drag 	);
DECLARE_DO_FUN( do_draw		);
DECLARE_DO_FUN( do_drink	);
DECLARE_DO_FUN( do_drive );
DECLARE_DO_FUN( do_drop		);
DECLARE_DO_FUN( do_droptroops	);
DECLARE_DO_FUN( do_dualstab	);
DECLARE_DO_FUN( do_east		);
DECLARE_DO_FUN( do_eat		);
DECLARE_DO_FUN( do_echo		);
DECLARE_DO_FUN( do_elite_guard );
DECLARE_DO_FUN( do_emote	);
DECLARE_DO_FUN( do_empower );
DECLARE_DO_FUN( do_empty	);
DECLARE_DO_FUN( do_endsimulator	);
CMDF( do_engage_grav 		);
DECLARE_DO_FUN( do_enlist );
DECLARE_DO_FUN( do_enter	);
DECLARE_DO_FUN( do_equipment	);
DECLARE_DO_FUN( do_examine	);
DECLARE_DO_FUN( do_exempt	);
DECLARE_DO_FUN( do_exits	);
DECLARE_DO_FUN( do_feed		);
DECLARE_DO_FUN( do_fhset    );
DECLARE_DO_FUN( do_fill		);
DECLARE_DO_FUN( do_fire );
DECLARE_DO_FUN( do_first_aid);
DECLARE_DO_FUN( do_fixchar	);
DECLARE_DO_FUN( do_flee		);
DECLARE_DO_FUN( do_fly );
DECLARE_DO_FUN( do_foldarea	);
DECLARE_DO_FUN( do_lookup	);
DECLARE_DO_FUN( do_follow	);
DECLARE_DO_FUN( do_for          );
DECLARE_DO_FUN( do_force	);
DECLARE_DO_FUN( do_forceclose	);
DECLARE_DO_FUN( do_fquit	);     /* Gorog */
DECLARE_DO_FUN( do_freeship	);
DECLARE_DO_FUN( do_freeze	);
DECLARE_DO_FUN( do_fset		);
DECLARE_DO_FUN( do_fsset	);
DECLARE_DO_FUN( do_fslay	);
DECLARE_DO_FUN( do_fsstat   );
DECLARE_DO_FUN( do_fstat    );
DECLARE_DO_FUN( do_gag		);
DECLARE_DO_FUN( do_gain  );
DECLARE_DO_FUN( do_gather_intelligence );
DECLARE_DO_FUN( do_gatherclans );
DECLARE_DO_FUN( do_gemcutting );
DECLARE_DO_FUN( do_generate_market );
DECLARE_DO_FUN( do_get		);
DECLARE_DO_FUN( do_give		);
DECLARE_DO_FUN( do_giveship	);
DECLARE_DO_FUN( do_giveslug	);
DECLARE_DO_FUN( do_glance	);
DECLARE_DO_FUN( do_global_boards);
DECLARE_DO_FUN( do_global_note  );
DECLARE_DO_FUN( do_gcatchup	);
DECLARE_DO_FUN( do_glist	);
DECLARE_DO_FUN( do_gread	);
DECLARE_DO_FUN( do_gremove	);
DECLARE_DO_FUN( do_gwrite	);
DECLARE_DO_FUN( do_gold         );
DECLARE_DO_FUN( do_goto		);
DECLARE_DO_FUN( do_gouge	);
DECLARE_DO_FUN( do_group	);
DECLARE_DO_FUN( do_grub 	);
DECLARE_DO_FUN( do_gtell	);
DECLARE_DO_FUN( do_guilds       );
DECLARE_DO_FUN( do_guildtalk    );
DECLARE_DO_FUN( do_hail );
DECLARE_DO_FUN( do_hale		);
DECLARE_DO_FUN( do_hedit	);
DECLARE_DO_FUN( do_hell		);
DECLARE_DO_FUN( do_help		);
DECLARE_DO_FUN( do_hide		);
DECLARE_DO_FUN( do_hijack   );
DECLARE_DO_FUN( do_hitall	);
DECLARE_DO_FUN( do_hlist	);
DECLARE_DO_FUN( do_holster  );
DECLARE_DO_FUN( do_holonet	);
DECLARE_DO_FUN( do_holylight	);
DECLARE_DO_FUN( do_homepage	);
DECLARE_DO_FUN( do_hset		);
DECLARE_DO_FUN( do_hyperspace );
DECLARE_DO_FUN( do_i103		);
DECLARE_DO_FUN( do_i104		);
DECLARE_DO_FUN( do_i105		);
DECLARE_DO_FUN( do_ide		);
DECLARE_DO_FUN( do_idea		);
DECLARE_DO_FUN( do_idealog	);
DECLARE_DO_FUN( do_immortalize	);
DECLARE_DO_FUN( do_immtalk	);
DECLARE_DO_FUN( do_imports      );
DECLARE_DO_FUN( do_induct	);
DECLARE_DO_FUN( do_info );
DECLARE_DO_FUN( do_inquire	);
DECLARE_DO_FUN( do_installarea	);
DECLARE_DO_FUN( do_installmodule );
DECLARE_DO_FUN( do_instaroom	);
DECLARE_DO_FUN( do_instazone	);
DECLARE_DO_FUN( do_interclan    );
DECLARE_DO_FUN( do_introduce );
DECLARE_DO_FUN( do_inventory	);
DECLARE_DO_FUN( do_invis	);
DECLARE_DO_FUN( do_invite	);
DECLARE_DO_FUN( do_jail );
DECLARE_DO_FUN( do_juke );
DECLARE_DO_FUN( do_jumpvector );
DECLARE_DO_FUN( do_keypad	); 
DECLARE_DO_FUN( do_kick		);
DECLARE_DO_FUN( do_kill		);
DECLARE_DO_FUN( do_land ); 
DECLARE_DO_FUN( do_languages    );
DECLARE_DO_FUN( do_last		);
DECLARE_DO_FUN( do_lastplayed );
DECLARE_DO_FUN( do_launch );
DECLARE_DO_FUN( do_launch2 );
DECLARE_DO_FUN( do_leave	);
DECLARE_DO_FUN( do_leaveship ); 
DECLARE_DO_FUN( do_level	);
DECLARE_DO_FUN( do_light	);
DECLARE_DO_FUN( do_link );
DECLARE_DO_FUN( do_list		);
DECLARE_DO_FUN( do_listmobships );
DECLARE_DO_FUN( do_listvendors  );
DECLARE_DO_FUN( do_litterbug	);
DECLARE_DO_FUN( do_load );
DECLARE_DO_FUN( do_loadarea	);
DECLARE_DO_FUN( do_load_cargo	);
DECLARE_DO_FUN( do_loadup	);
DECLARE_DO_FUN( do_lock		);
DECLARE_DO_FUN( do_log		);
DECLARE_DO_FUN( do_look		);
DECLARE_DO_FUN( do_low_purge	);
DECLARE_DO_FUN( do_mailroom	);
DECLARE_DO_FUN( do_make		);
DECLARE_DO_FUN( do_make_master); 
DECLARE_DO_FUN( do_makearmor );
DECLARE_DO_FUN( do_makebeacon );
DECLARE_DO_FUN( do_makebinders );
DECLARE_DO_FUN( do_makeblade ); 
DECLARE_DO_FUN( do_makeblaster );
DECLARE_DO_FUN( do_makeblueprint );
DECLARE_DO_FUN( do_makeboard	);
DECLARE_DO_FUN( do_makebug );
DECLARE_DO_FUN( do_makeclan	);
DECLARE_DO_FUN( do_makecomlink );
DECLARE_DO_FUN( do_makecommsystem);
DECLARE_DO_FUN( do_makecontainer );
DECLARE_DO_FUN( do_makedatapad	);
DECLARE_DO_FUN( do_planetdata );
DECLARE_DO_FUN( do_makeduallightsaber );
DECLARE_DO_FUN( do_makefree );
DECLARE_DO_FUN( do_makegoggles );
DECLARE_DO_FUN( do_makegrenade );
DECLARE_DO_FUN( do_makeholster	);
DECLARE_DO_FUN( do_makeguild    );
DECLARE_DO_FUN( do_makejetpack	);
DECLARE_DO_FUN( do_makejewelry );
DECLARE_DO_FUN( do_makelandmine );
DECLARE_DO_FUN( do_makelight );
DECLARE_DO_FUN( do_makelightsaber );
DECLARE_DO_FUN( do_makemedpac  );
DECLARE_DO_FUN( do_makemissile );
DECLARE_DO_FUN( do_makemobship );
DECLARE_DO_FUN( do_makemodule );
DECLARE_DO_FUN( do_makepike ); 
DECLARE_DO_FUN( do_makeplanet );
DECLARE_DO_FUN( do_makeprototypeship );
DECLARE_DO_FUN( do_makerepair	);
DECLARE_DO_FUN( do_makeshield );
DECLARE_DO_FUN( do_makeship     );
DECLARE_DO_FUN( do_makeship2     );
DECLARE_DO_FUN( do_makeshipbomb );
DECLARE_DO_FUN( do_makeshop	);
DECLARE_DO_FUN( do_makesimulator );
DECLARE_DO_FUN( do_makeslay );
DECLARE_DO_FUN( do_makespice );
DECLARE_DO_FUN( do_makestarsystem );
DECLARE_DO_FUN( do_makewizlist	);
DECLARE_DO_FUN( do_marry	);
DECLARE_DO_FUN( do_mass_propeganda );
DECLARE_DO_FUN( do_massign      );
DECLARE_DO_FUN( do_mcreate	);
DECLARE_DO_FUN( do_mdelete	);
DECLARE_DO_FUN( do_members	);
DECLARE_DO_FUN( do_memory	);
DECLARE_DO_FUN( do_mentor	);
DECLARE_DO_FUN( do_mentortalk	);
DECLARE_DO_FUN( do_mfind	);
DECLARE_DO_FUN( do_mine );
DECLARE_DO_FUN( do_minvoke	);
DECLARE_DO_FUN( do_mlist	);
DECLARE_DO_FUN( do_mobrepair	);
DECLARE_DO_FUN( do_mount	);
DECLARE_DO_FUN( do_mp_close_passage );
DECLARE_DO_FUN( do_mp_damage );
DECLARE_DO_FUN( do_mp_deposit	);
DECLARE_DO_FUN( do_mp_open_passage );
DECLARE_DO_FUN( do_mp_practice );
DECLARE_DO_FUN( do_mp_restore );
DECLARE_DO_FUN( do_mp_slay);
DECLARE_DO_FUN( do_mp_withdraw	);
DECLARE_DO_FUN( do_mpadvance    );
DECLARE_DO_FUN( do_mpapply	);
DECLARE_DO_FUN( do_mpapplyb  	);
DECLARE_DO_FUN( do_mpasound     );
DECLARE_DO_FUN( do_mpat         );
DECLARE_DO_FUN( do_mpdream	);
DECLARE_DO_FUN( do_mpecho       );
DECLARE_DO_FUN( do_mpechoaround );
DECLARE_DO_FUN( do_mpechoat     );
DECLARE_DO_FUN( do_mpedit       );
DECLARE_DO_FUN( do_mpforce      );
DECLARE_DO_FUN( do_mpgain	);
DECLARE_DO_FUN( do_mpgoto       );
DECLARE_DO_FUN( do_mpinvis	);
DECLARE_DO_FUN( do_mpjunk       );
DECLARE_DO_FUN( do_mpkill       );
DECLARE_DO_FUN( do_mpmload      );
DECLARE_DO_FUN( do_mpmset	);
DECLARE_DO_FUN( do_mpnothing	);
DECLARE_DO_FUN( do_mpoload      );
DECLARE_DO_FUN( do_mposet	);
DECLARE_DO_FUN( do_mppkset	);
DECLARE_DO_FUN( do_mppurge      );
DECLARE_DO_FUN( do_mpstat       );
DECLARE_DO_FUN( do_mptransfer   );
DECLARE_DO_FUN( do_mpquestcomplete ); // new quest shit
DECLARE_DO_FUN( do_mrange       );
DECLARE_DO_FUN( do_mset		);
DECLARE_DO_FUN( do_mstat	);
DECLARE_DO_FUN( do_murde	);
DECLARE_DO_FUN( do_murder	);
DECLARE_DO_FUN( do_music	);
DECLARE_DO_FUN( do_mwhere	);
DECLARE_DO_FUN( do_name		);
DECLARE_DO_FUN( do_newbiechat   );
DECLARE_DO_FUN( do_newbieset    );
DECLARE_DO_FUN( do_newclan );
DECLARE_DO_FUN( do_newexits	);
DECLARE_DO_FUN( do_newzones	);
DECLARE_DO_FUN( do_noemote	);
DECLARE_DO_FUN( do_noresolve	);
DECLARE_DO_FUN( do_north	);
DECLARE_DO_FUN( do_northeast	);
DECLARE_DO_FUN( do_northwest	);
DECLARE_DO_FUN( do_notell	);
DECLARE_DO_FUN( do_noteroom	);
DECLARE_DO_FUN( do_notitle      );
DECLARE_DO_FUN( do_nullifybeacons );
DECLARE_DO_FUN( do_oassign      );
DECLARE_DO_FUN( do_ocreate	);
DECLARE_DO_FUN( do_odelete	);
DECLARE_DO_FUN( do_ofind	);
DECLARE_DO_FUN( do_ogrub 	);
DECLARE_DO_FUN( do_oinvoke	);
DECLARE_DO_FUN( do_oldmstat	);
DECLARE_DO_FUN( do_oldscore	);
DECLARE_DO_FUN( do_olist	);
DECLARE_DO_FUN( do_ooc		);
DECLARE_DO_FUN( do_opedit       );
DECLARE_DO_FUN( do_open		);
DECLARE_DO_FUN( do_openbay );
DECLARE_DO_FUN( do_openhatch );
DECLARE_DO_FUN( do_opentourney  );
DECLARE_DO_FUN( do_opstat       );
DECLARE_DO_FUN( do_orange       );
DECLARE_DO_FUN( do_order	);
DECLARE_DO_FUN( do_orderclanship);
DECLARE_DO_FUN( do_orders	);
DECLARE_DO_FUN( do_ordership    );
DECLARE_DO_FUN( do_ordertalk	);
DECLARE_DO_FUN( do_oset		);
DECLARE_DO_FUN( do_ostat	);
DECLARE_DO_FUN( do_ot		);
DECLARE_DO_FUN( do_outcast	);
DECLARE_DO_FUN( do_outlaw	);
DECLARE_DO_FUN( do_owhere	);
DECLARE_DO_FUN( do_pager	);
DECLARE_DO_FUN( do_pardon	);
DECLARE_DO_FUN( do_password	);
DECLARE_DO_FUN( do_peace	);
DECLARE_DO_FUN( do_pcrename	);
DECLARE_DO_FUN( do_pick		);
DECLARE_DO_FUN( do_pickshiplock  );
DECLARE_DO_FUN( do_placevendor	);
DECLARE_DO_FUN( do_planets );
DECLARE_DO_FUN( do_plantbeacon  );
DECLARE_DO_FUN( do_plantbug	);
DECLARE_DO_FUN( do_plrbuglist   );
DECLARE_DO_FUN( do_pluogus ); 
DECLARE_DO_FUN( do_poison);
DECLARE_DO_FUN( do_pose		);
DECLARE_DO_FUN( do_postguard );
DECLARE_DO_FUN( do_practice	);
DECLARE_DO_FUN( do_pricevendor );
DECLARE_DO_FUN( do_project	);
DECLARE_DO_FUN( do_prompt	);
DECLARE_DO_FUN( do_propeganda );
DECLARE_DO_FUN( do_propose	);
DECLARE_DO_FUN( do_prototypes );
DECLARE_DO_FUN( do_pull		);
DECLARE_DO_FUN( do_punch	);
DECLARE_DO_FUN( do_purge	);
DECLARE_DO_FUN( do_push		);
DECLARE_DO_FUN( do_put		);
DECLARE_DO_FUN( do_qpset	);
DECLARE_DO_FUN( do_quaff	);
DECLARE_DO_FUN( do_quest	);
DECLARE_DO_FUN( do_qui		);
DECLARE_DO_FUN( do_quit		);
DECLARE_DO_FUN( do_radar );
DECLARE_DO_FUN( do_rank	        );
DECLARE_DO_FUN( do_rassign      );
DECLARE_DO_FUN( do_rat		);
DECLARE_DO_FUN( do_rdelete	);
DECLARE_DO_FUN( do_reboo	);
DECLARE_DO_FUN( do_reboot	);
DECLARE_DO_FUN( do_recall ); 
DECLARE_DO_FUN( do_recall	);
DECLARE_DO_FUN( do_recharge );
DECLARE_DO_FUN( do_recho	);
DECLARE_DO_FUN( do_recite	);
DECLARE_DO_FUN( do_redit	);
DECLARE_DO_FUN( do_refuel );
DECLARE_DO_FUN( do_regoto       );
DECLARE_DO_FUN( do_reinforcements );
DECLARE_DO_FUN( do_reload );
DECLARE_DO_FUN( do_rembounty	);
DECLARE_DO_FUN( do_remclan);
DECLARE_DO_FUN( do_remcontract    );
DECLARE_DO_FUN( do_remember );
DECLARE_DO_FUN( do_remove	);
DECLARE_DO_FUN( do_removebug );
DECLARE_DO_FUN( do_removemobship );
DECLARE_DO_FUN( do_removemodule );
DECLARE_DO_FUN( do_removeship);
DECLARE_DO_FUN( do_rempilot);
DECLARE_DO_FUN( do_remsenator	);
DECLARE_DO_FUN( do_remslay	);
DECLARE_DO_FUN( do_rent		);
DECLARE_DO_FUN( do_renumber	);
DECLARE_DO_FUN( do_repair	);
DECLARE_DO_FUN( do_repairset	);
DECLARE_DO_FUN( do_repairshops	);
DECLARE_DO_FUN( do_repairstat	);
DECLARE_DO_FUN( do_reply	);
DECLARE_DO_FUN( do_report	);
DECLARE_DO_FUN( do_dreport	);
DECLARE_DO_FUN( do_request	);
DECLARE_DO_FUN( do_rescue	);
DECLARE_DO_FUN( do_reset	);
DECLARE_DO_FUN( do_resetship	);
DECLARE_DO_FUN( do_resign );
DECLARE_DO_FUN( do_rest		);
DECLARE_DO_FUN( do_restore	);
DECLARE_DO_FUN( do_restorefile	);
DECLARE_DO_FUN( do_restoretime	);
DECLARE_DO_FUN( do_restrict	);
DECLARE_DO_FUN( do_retire       );
DECLARE_DO_FUN( do_retran       );
DECLARE_DO_FUN( do_retreat );
DECLARE_DO_FUN( do_retune );
DECLARE_DO_FUN( do_return	);
DECLARE_DO_FUN( do_revert	);
DECLARE_DO_FUN( do_reward );
DECLARE_DO_FUN( do_rip		);
DECLARE_DO_FUN( do_rlist	);
DECLARE_DO_FUN( do_rpconvert    );
DECLARE_DO_FUN( do_rpedit       );
DECLARE_DO_FUN( do_rpstat       );
DECLARE_DO_FUN( do_rset		);
DECLARE_DO_FUN( do_rstat	);
DECLARE_DO_FUN( do_sabotage	);
DECLARE_DO_FUN( do_sacrifice	);
DECLARE_DO_FUN( do_salvage	);
DECLARE_DO_FUN( do_save		);
DECLARE_DO_FUN( do_savearea	);
DECLARE_DO_FUN( do_say		);
DECLARE_DO_FUN( do_scan         );
DECLARE_DO_FUN( do_scanbugs     );
DECLARE_DO_FUN( do_score	);
DECLARE_DO_FUN( do_screenname   );
DECLARE_DO_FUN( do_scribe       );
DECLARE_DO_FUN( do_search	);
DECLARE_DO_FUN( do_sedit	);
DECLARE_DO_FUN( do_seduce );
DECLARE_DO_FUN( do_sell		);
DECLARE_DO_FUN( do_sellhome	);
DECLARE_DO_FUN( do_sellship );
DECLARE_DO_FUN( do_sellshop );
DECLARE_DO_FUN( do_senate	);
DECLARE_DO_FUN( do_set_boot_time);
DECLARE_DO_FUN( do_setblaster );
DECLARE_DO_FUN( do_setself	);
DECLARE_DO_FUN( do_setclan	);
DECLARE_DO_FUN( do_setcolor );
DECLARE_DO_FUN( do_setinfrared );
DECLARE_DO_FUN( do_setplanet );
DECLARE_DO_FUN( do_setprototype );
DECLARE_DO_FUN( do_setrank );
DECLARE_DO_FUN( do_setship      );
DECLARE_DO_FUN( do_setshop );
DECLARE_DO_FUN( do_setslay	);
DECLARE_DO_FUN( do_setstarsystem );
DECLARE_DO_FUN( do_setwage	);
DECLARE_DO_FUN( do_shiftvnums	);
DECLARE_DO_FUN( do_shiplist );
DECLARE_DO_FUN( do_shiplock	);
DECLARE_DO_FUN( do_shipmaintenance );
DECLARE_DO_FUN( do_shiprepair );
DECLARE_DO_FUN( do_ships        );
DECLARE_DO_FUN( do_shipstat  );
DECLARE_DO_FUN( do_shiptalk  );
DECLARE_DO_FUN( do_shops	);
DECLARE_DO_FUN( do_shopset	);
DECLARE_DO_FUN( do_shopstat	);
DECLARE_DO_FUN( do_shout	);
DECLARE_DO_FUN( do_shove  	);
DECLARE_DO_FUN( do_showbeacons  );
DECLARE_DO_FUN( do_showbugs	);
DECLARE_DO_FUN( do_showclan	);
DECLARE_DO_FUN( do_showcontracts    );
DECLARE_DO_FUN( do_showslay );
DECLARE_DO_FUN( do_showmodules );
DECLARE_DO_FUN( do_showplanet   );
DECLARE_DO_FUN( do_showprototype );
DECLARE_DO_FUN( do_showship     );
DECLARE_DO_FUN( do_showsocial   );
DECLARE_DO_FUN( do_showstarsystem );
DECLARE_DO_FUN( do_shutdow	);
DECLARE_DO_FUN( do_shutdown	);
DECLARE_DO_FUN( do_silence	);
DECLARE_DO_FUN( do_sit		);
DECLARE_DO_FUN( do_sla		);
DECLARE_DO_FUN( do_slay		);
DECLARE_DO_FUN( do_sleep	);
DECLARE_DO_FUN( do_slice        );
DECLARE_DO_FUN( do_slicebank	);
DECLARE_DO_FUN( do_slist        );
DECLARE_DO_FUN( do_slog  	);
DECLARE_DO_FUN( do_slookup	);
DECLARE_DO_FUN( do_smalltalk );
DECLARE_DO_FUN( do_smoke	);
DECLARE_DO_FUN( do_sneak	);
DECLARE_DO_FUN( do_snipe );
DECLARE_DO_FUN( do_snoop	);
DECLARE_DO_FUN( do_sober	);
DECLARE_DO_FUN( do_socials	);
DECLARE_DO_FUN( do_sound  );
DECLARE_DO_FUN( do_south	);
DECLARE_DO_FUN( do_southeast	);
DECLARE_DO_FUN( do_southwest	);
DECLARE_DO_FUN( do_spacetalk );
DECLARE_DO_FUN( do_speak        );
DECLARE_DO_FUN( do_special_forces );
DECLARE_DO_FUN( do_speeders  );
DECLARE_DO_FUN( do_split	);
DECLARE_DO_FUN( do_split_s	);
DECLARE_DO_FUN( do_spousetalk );
DECLARE_DO_FUN( do_sset		);
DECLARE_DO_FUN( do_stand	);
DECLARE_DO_FUN( do_starsystems ); 
DECLARE_DO_FUN( do_starttourney );
DECLARE_DO_FUN( do_status );
DECLARE_DO_FUN( do_std );
DECLARE_DO_FUN( do_steal	);
DECLARE_DO_FUN( do_sting	);
DECLARE_DO_FUN( do_stun		);
DECLARE_DO_FUN( do_suicide  );
DECLARE_DO_FUN( do_supplicate	);
DECLARE_DO_FUN( do_switch	);
DECLARE_DO_FUN( do_sysbuglist   );
DECLARE_DO_FUN( do_systemtalk );
DECLARE_DO_FUN( do_tail		);
DECLARE_DO_FUN( do_takedrug );
DECLARE_DO_FUN( do_tamp		);
DECLARE_DO_FUN( do_target );
DECLARE_DO_FUN( do_taxi   );
DECLARE_DO_FUN( do_teach );
DECLARE_DO_FUN( do_tell		);
DECLARE_DO_FUN( do_tellsnoop	);
DECLARE_DO_FUN( do_throw );
DECLARE_DO_FUN( do_time		);
DECLARE_DO_FUN( do_timecmd	);
DECLARE_DO_FUN( do_title	);
DECLARE_DO_FUN( do_torture );
DECLARE_DO_FUN( do_track	);
DECLARE_DO_FUN( do_tractorbeam ); 
DECLARE_DO_FUN( do_train  );
DECLARE_DO_FUN( do_trajectory );
DECLARE_DO_FUN( do_transfer	);
DECLARE_DO_FUN( do_transmit_call );
DECLARE_DO_FUN( do_transmit_status );
DECLARE_DO_FUN( do_transmit_broadcast );
DECLARE_DO_FUN( do_transship	);
DECLARE_DO_FUN( do_transshipss  );
DECLARE_DO_FUN( do_trespass 	);
DECLARE_DO_FUN( do_trivia);
DECLARE_DO_FUN( do_trivia_answer);
DECLARE_DO_FUN( do_trivia_question);
DECLARE_DO_FUN( do_trivia_chat);
DECLARE_DO_FUN( do_trivia_winner);
DECLARE_DO_FUN( do_trivia_score);
DECLARE_DO_FUN( do_trivia_join);
DECLARE_DO_FUN( do_trust	);
DECLARE_DO_FUN( do_tsavearea	);
DECLARE_DO_FUN( do_tune		);
DECLARE_DO_FUN( do_typo		);
DECLARE_DO_FUN( do_unbind	);
DECLARE_DO_FUN( do_ungag	);
DECLARE_DO_FUN( do_unhell	);
DECLARE_DO_FUN( do_unlink );
DECLARE_DO_FUN( do_unload );
DECLARE_DO_FUN( do_unmentor );
DECLARE_DO_FUN( do_unload_cargo );
DECLARE_DO_FUN( do_unlock	);
DECLARE_DO_FUN( do_unoutlaw     );
DECLARE_DO_FUN( do_unsilence    );
DECLARE_DO_FUN( do_up		);
DECLARE_DO_FUN( do_use ); 
DECLARE_DO_FUN( do_users	);
DECLARE_DO_FUN( do_value	);
DECLARE_DO_FUN( do_vassign      );
DECLARE_DO_FUN( do_viewskills	);
DECLARE_DO_FUN( do_visible	);
DECLARE_DO_FUN( do_vnums	);
DECLARE_DO_FUN( do_vsearch	);
DECLARE_DO_FUN( do_wake		);
DECLARE_DO_FUN( do_war	        );
DECLARE_DO_FUN( do_wartalk      );
DECLARE_DO_FUN( do_wear		);
DECLARE_DO_FUN( do_weather	);
DECLARE_DO_FUN( do_west		);
DECLARE_DO_FUN( do_where	);
DECLARE_DO_FUN( do_whisper	);
DECLARE_DO_FUN( do_who		);
DECLARE_DO_FUN( do_whoinvis	);
DECLARE_DO_FUN( do_whois	);
DECLARE_DO_FUN( do_wimpy	);
DECLARE_DO_FUN( do_wizhelp	);
DECLARE_DO_FUN( do_wizlist	);
DECLARE_DO_FUN( do_wizlock	);
DECLARE_DO_FUN( do_wwwimage	);
DECLARE_DO_FUN( do_yell		);
DECLARE_DO_FUN( do_zap		);
DECLARE_DO_FUN( do_zecho );
DECLARE_DO_FUN( do_zones	);




//Force skills
DECLARE_DO_FUN( fskill_awareness	      );
DECLARE_DO_FUN( fskill_convert            );
DECLARE_DO_FUN( fskill_farsee			  );
DECLARE_DO_FUN( fskill_fdisarm			  );
DECLARE_DO_FUN( fskill_fdisguise          );
DECLARE_DO_FUN( fskill_fhelp              );
DECLARE_DO_FUN( fskill_finfo              );
DECLARE_DO_FUN( fskill_finish             );
DECLARE_DO_FUN( fskill_force_lightning    );
DECLARE_DO_FUN( fskill_fshield            );
DECLARE_DO_FUN( fskill_heal               );
DECLARE_DO_FUN( fskill_identify           );
DECLARE_DO_FUN( fskill_instill			  );
DECLARE_DO_FUN( fskill_instruct           );
DECLARE_DO_FUN( fskill_makedualsaber	  );
DECLARE_DO_FUN( fskill_makelightsaber     );
DECLARE_DO_FUN( fskill_master             );
DECLARE_DO_FUN( fskill_promote            );
DECLARE_DO_FUN( fskill_protect            );
DECLARE_DO_FUN( fskill_refresh            );
DECLARE_DO_FUN( fskill_slash              );
DECLARE_DO_FUN( fskill_squeeze            );
DECLARE_DO_FUN( fskill_student            );
DECLARE_DO_FUN( fskill_telepathy		  );
DECLARE_DO_FUN( fskill_whirlwind          );
DECLARE_DO_FUN( skill_notfound	);




/* Spells and such*/
DECLARE_SPELL_FUN(  spell_acetum_primus                 );
DECLARE_SPELL_FUN(  spell_acid_blast	);
DECLARE_SPELL_FUN(  spell_acid_breath	);
DECLARE_SPELL_FUN(  spell_animate_dead	);
DECLARE_SPELL_FUN(  spell_astral_walk	);
DECLARE_SPELL_FUN(  spell_black_fist                    );
DECLARE_SPELL_FUN(  spell_black_hand                    );
DECLARE_SPELL_FUN(  spell_black_lightning               );
DECLARE_SPELL_FUN(  spell_blindness		);
DECLARE_SPELL_FUN(  spell_burning_hands	);
DECLARE_SPELL_FUN(  spell_call_lightning	);
DECLARE_SPELL_FUN(  spell_calm	                    );
DECLARE_SPELL_FUN(  spell_cause_critical	);
DECLARE_SPELL_FUN(  spell_cause_light	);
DECLARE_SPELL_FUN(  spell_cause_serious	);
DECLARE_SPELL_FUN(  spell_caustic_fount                 );
DECLARE_SPELL_FUN(  spell_change_sex	);
DECLARE_SPELL_FUN(  spell_charm_person	);
DECLARE_SPELL_FUN(  spell_chill_touch	);
DECLARE_SPELL_FUN(  spell_colour_spray	);
DECLARE_SPELL_FUN(  spell_control_weather	);
DECLARE_SPELL_FUN(  spell_create_food	);
DECLARE_SPELL_FUN(  spell_create_water	);
DECLARE_SPELL_FUN(  spell_cure_addiction                );
DECLARE_SPELL_FUN(  spell_cure_blindness	);
DECLARE_SPELL_FUN(  spell_cure_poison	);
DECLARE_SPELL_FUN(  spell_curse		);
DECLARE_SPELL_FUN(  spell_detect_poison	);
DECLARE_SPELL_FUN(  spell_dispel_evil	);
DECLARE_SPELL_FUN(  spell_dispel_magic	);
DECLARE_SPELL_FUN(  spell_disruption                    );
DECLARE_SPELL_FUN(  spell_dream             );
DECLARE_SPELL_FUN(  spell_earthquake	);
DECLARE_SPELL_FUN(  spell_enchant_weapon	);
DECLARE_SPELL_FUN(  spell_energy_drain	);
DECLARE_SPELL_FUN(  spell_ethereal_fist                 );
DECLARE_SPELL_FUN(  spell_faerie_fire	);
DECLARE_SPELL_FUN(  spell_faerie_fog	);
DECLARE_SPELL_FUN(  spell_farsight		);
DECLARE_SPELL_FUN(  spell_fire_breath	);
DECLARE_SPELL_FUN(  spell_fireball		);
DECLARE_SPELL_FUN(  spell_flamestrike	);
DECLARE_SPELL_FUN(  spell_force_disarm                    );
DECLARE_SPELL_FUN(  spell_forcepush	                    );
DECLARE_SPELL_FUN(  spell_frost_breath	);
DECLARE_SPELL_FUN(  spell_galvanic_whip                 );
DECLARE_SPELL_FUN(  spell_gas_breath	);
DECLARE_SPELL_FUN(  spell_gate		);
DECLARE_SPELL_FUN(  spell_hand_of_chaos                 );
DECLARE_SPELL_FUN(  spell_helical_flow	);
DECLARE_SPELL_FUN(  spell_identify		);
DECLARE_SPELL_FUN(  spell_injure		);
DECLARE_SPELL_FUN(  spell_invis		);
DECLARE_SPELL_FUN(  spell_knock             );
DECLARE_SPELL_FUN(  spell_know_alignment	);
DECLARE_SPELL_FUN(  spell_lightning_bolt	);
DECLARE_SPELL_FUN(  spell_lightning_breath	);
DECLARE_SPELL_FUN(  spell_locate_object	);
DECLARE_SPELL_FUN(  spell_magic_missile	);
DECLARE_SPELL_FUN(  spell_magnetic_thrust               );
DECLARE_SPELL_FUN(  spell_midas_touch                   );
DECLARE_SPELL_FUN(  spell_mind_wrack                    );
DECLARE_SPELL_FUN(  spell_mind_wrench                   );
DECLARE_SPELL_FUN(  spell_mist_walk		);
DECLARE_SPELL_FUN(  spell_notfound		);
DECLARE_SPELL_FUN(  spell_null		);
DECLARE_SPELL_FUN(  spell_pass_door		);
DECLARE_SPELL_FUN(  spell_plant_pass	);
DECLARE_SPELL_FUN(  spell_poison		);
DECLARE_SPELL_FUN(  spell_polymorph		);
DECLARE_SPELL_FUN(  spell_portal                    );
DECLARE_SPELL_FUN(  spell_possess		);
DECLARE_SPELL_FUN(  spell_quantum_spike                 );
DECLARE_SPELL_FUN(  spell_recharge		);
DECLARE_SPELL_FUN(  spell_remove_curse	);
DECLARE_SPELL_FUN(  spell_remove_invis	);
DECLARE_SPELL_FUN(  spell_remove_trap	);
DECLARE_SPELL_FUN(  spell_revive                        );
DECLARE_SPELL_FUN(  spell_scorching_surge	);
DECLARE_SPELL_FUN(  spell_shocking_grasp	);
DECLARE_SPELL_FUN(  spell_sleep		);
DECLARE_SPELL_FUN(  spell_smaug		);
DECLARE_SPELL_FUN(  spell_solar_flight	);
DECLARE_SPELL_FUN(  spell_sonic_resonance               );
DECLARE_SPELL_FUN(  spell_spectral_furor                );
DECLARE_SPELL_FUN(  spell_spiral_blast	);
DECLARE_SPELL_FUN(  spell_steal_life                    );
DECLARE_SPELL_FUN(  spell_suggest                       );
DECLARE_SPELL_FUN(  spell_sulfurous_spray               );
DECLARE_SPELL_FUN(  spell_summon		);
DECLARE_SPELL_FUN(  spell_teleport		);
DECLARE_SPELL_FUN(  spell_transport      		);
DECLARE_SPELL_FUN(  spell_ventriloquate	);
DECLARE_SPELL_FUN(  spell_weaken		);
DECLARE_SPELL_FUN(  spell_word_of_recall	);

