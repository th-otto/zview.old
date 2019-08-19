/* ---------------------------------------------------------
 *	Librairie Dynamique GEM
 *	Olivier Landemarre, Dominique B‚r‚ziat, Arnaud Bercegeay
 *	All rights reserved 1997-2003
 * ---------------------------------------------------------
 * $Id: version.h 128 2015-08-27 21:39:47Z landemarre $
 */

/*
 *	Chaque mainteneur d'une version
 *	corrige la partie qui le concerne
 */

/* version Librairie de d‚veloppement */
#define VERSION	"2.35"
#define LDG_NUM 	0x0235

#ifdef __PUREC__ 
#define LDG_NAME	VERSION __DATE__ " - Pure C " __STRINGIFY(__PUREC__)
#endif
#ifdef __VBCC__ 
#define LDG_NAME	VERSION __DATE__ " - VBCC"
#endif
#ifdef __SOZOBONX__
#define LDG_NAME	VERSION __DATE__  "-  Sozobon X " __STRINGIFY(__SOZOBONX__)
#endif
#ifdef __GNUC__
#define LDG_NAME	VERSION __DATE__ " - GCC " __STRINGIFY(__GNUC__) "." __STRINGIFY(__GNUC_MINOR__) "." __STRINGIFY(__GNUC_PATCHLEVEL__)
#endif


/* TSR */

#define TSR_VERSION_TEXT	"2.35"
#define	TSR_VERSION_NUMBER	0x0235

/* Manager */

#define MGR_VERSION_TEXT	"2.00"
#define MGR_VERSION_NUMBER	0x0200

/* Daemon */

#define LDGD_VERSION_TEXT   "1.11"

