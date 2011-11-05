/*
 *	Routines d'‚dition de texte
 *	Dominique B‚r‚ziat, tous droits r‚serv‚s
 *	module : edit.c
 *  descr. : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../types2b.h"
#include "../pdf/goo/gmem.h"
#include "libtedit.h"

/* NewLine: cr‚‚ une nouvelle ligne */

EDLINE *line_new( char *buf) 
{
	EDLINE *line = (EDLINE*)gmalloc(sizeof(EDLINE));
	
	if( line == NULL) return NULL;

	if( buf) 
	{
		line -> buf = copyString( buf);

		if( line -> buf) 
		{
			line -> len = (int)strlen( buf);
			line -> size = line -> len + 1;
			line -> next = line ->prev = NULL;
			return line;
		} 

	} 
	else 
	{
		line -> buf = gmalloc( sizeof(char)*SIZE_REALLOC);

		if( line -> buf) 
		{
			*line -> buf = '\0';
			line -> len = 0;
			line -> size = SIZE_REALLOC;
			line -> next = line ->prev = NULL;
			return line;
		}
	}
	gfree( line);
	
	return NULL;
}

/* Addline : ajoute une ligne dans le buffer */

void line_add( EDIT *edit, EDLINE *line, int16 mode) 
{
	EDLINE *tmp;

	edit -> maxline ++;

	if( edit->top == NULL)
		edit->top = edit->bot = edit->cur.line = line;
	else
		switch( mode) 
		{
			case ED_TOP:
				tmp = edit -> top;
				edit -> top = line;
				line -> next = tmp;
				tmp -> prev = line;
				break;

			case ED_BOT:
				tmp = edit -> bot;
				edit -> bot = line;
				line -> prev = tmp;
				tmp -> next = line;
				break;	

			/* Insertion … la position du curseur */
			case ED_CUR:
				tmp = edit -> cur.line;
				/* la ligne ajout‚e devient la ligne courante */
				edit -> cur.line = line;
				/* lien avec la ligne suivante (si elle existe ) */
				line -> next = tmp->next;

				if( tmp -> next) 
					tmp -> next -> prev = line;
				/* lien avec la ligne pr‚c‚dente */	
				line -> prev = tmp;
				tmp -> next = line;
				break;
		}
}

/* RemLine : retire la ligne courante */

void line_rem( EDIT *edit) 
{
	EDLINE *line = edit -> cur.line;

	if( edit -> top == NULL) 
		return;

	edit -> maxline --;

	if( line -> prev) 
	{
		/* connecter les lignes pr‚c‚dente et suivante */
		if( line -> prev)
			line -> prev -> next = line -> next;
		else
			edit -> top = line -> next;

		if( line -> next) 
			line -> next -> prev = line -> prev;
		else
			edit -> bot = line -> prev;

		/* nouvelle ligne courante */
		if( line->next)
			edit -> cur.line = line->next;
		else 
		{
			edit -> cur.line = line->prev?line->prev:NULL;

			if( line->prev) 
				edit -> cur.index --;
		}
		edit -> cur.row = 0;
	} 
	else 
	{
		/* Connecter … la ligne suiante */
		edit -> top = edit->cur.line = line->next;

		if( edit->cur.line)
			edit->cur.line->prev = NULL;

		edit -> cur.row = 0;
	}
	
	gfree( line -> buf);
	gfree( line);
}


/*
 * D‚clare un nouveau texte
 */

EDIT* edit_new( void) 
{
	EDIT *edit = ( EDIT *)gmalloc( sizeof( EDIT));

	if( edit == NULL) 
		return NULL;

	edit -> top = edit -> bot = edit -> cur.line = NULL;
	edit -> cur.row = 0;
	edit -> cur.index = 1;
	edit -> maxline = 0L;
	edit -> maxcur = 0;
	edit -> type = T_DOS;
	*edit -> name = '\0';
	return edit;
}

void edit_free( EDIT *edit) {
	EDLINE *scan =  edit -> top;
	EDLINE *next;
	
	while( scan) 
	{
		next = scan -> next;
		gfree( scan->buf);
		gfree( scan);
		scan = next;
	}
	
	gfree( edit);
}

int16 diff_line( EDLINE *up, EDLINE *dn) 
{
	EDLINE *scan;
	int16 diff;

	for( diff = 0, scan = up; scan!=dn && scan!=NULL; diff ++, scan = scan->next);

	if( scan == NULL)
		for( diff = 0, scan = up; scan!=dn && scan!=NULL; diff --, scan = scan->prev);

	return diff;
}
