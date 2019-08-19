/* ---------------------------------------------------------
 *	Librairie Dynamique GEM
 *	Olivier Landemarre, Dominique B‚r‚ziat, Arnaud Bercegeay
 *	All rights reserved 1997-2003
 * ---------------------------------------------------------
 *	version : 2.00
 *	module : options.h
 *	r“le : les options de compilation de la librairie,
 *         du TSR et du d‚mon.
 *
 * $Id: options.h 70 2008-01-08 23:23:26Z landemarre $
 */

/* #define DEBUG */			/* D‚commenter pour l'activer ou bien a passer au 
							 * compilo avec le paramŠtre -DDEBUG */

#define LDG231				/* Activer les modifs en cours */

/* Capacit‚ du TSR pour stocker des libraries */

#define MAX_CLIENT 			 20
#define MAX_LIB   			250
#define MAX_CLIENT_TOTAL   	200

/* Longueur des buffers pour stocker les noms de fichiers */

#define BUFLEN				256

/* Nom des librairies dans /u/shm */

#define LDG_SHM_PATTERN	"ldg"
#define LDG_SHM_FILENAME	"U:\\shm\\" LDG_SHM_PATTERN " %03d.shm"

/* Nom des m‚moires multiutilisateur dans /u/shm */

#define MEM_SHM_FILENAME	"U:\\shm\\mem%03d.shm"

/* Nom des blocs m‚moires LDG dans /u/shm */

#define LDGBK_SHM_PATTERN	"ldgbk"
#define LDGBK_SHM_FILENAME	"U:\\shm\\" LDGBK_SHM_PATTERN " %03d.shm"

/* Mettre … 1 pour forcer l'installation du cookie lorsqu'une 
 * version identique est d‚j… pr‚sente. Utilis‚ pour tester le
 * TSR sans avoir a rebooter continuellement. */

#define	TSR_FORCE_INSTALL	1

/* Nombre max de bloc m‚moire multiutilisateur */

#define MAX_MEM 100 

/* Requˆte au d‚mon - interne */

#define LDGD_SHM_REQUEST	0x4C5F
#define LDGD_SHARE_LDG	 1
#define LDGD_SHARE_MEM	 2
#define LDGD_UNSHARE_LDG 3
#define LDGD_UNSHARE_MEM 4

/* EOF */

