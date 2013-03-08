#include "general.h"
#include "prefs.h"
#include "winimg.h"
#include "aconf.h"
#include "custom_font.h"
#include "goo/gmem.h"
#include "goo/GString.h"
#include "goo/GList.h"
#include "xpdf/GlobalParams.h"
#include "xpdf/Object.h"
#include "xpdf/PDFDoc.h"
#include "splash/SplashBitmap.h"
#include "splash/Splash.h"
#include "xpdf/SplashOutputDev.h"
#include "xpdf/CharTypes.h"
#include "xpdf/UnicodeMap.h"
#include "xpdf/Outline.h"
#include "xpdf/Link.h"

extern "C" {


static char latin_to_atari[] = { /*0x00A0*/'*','�','�','�','*','�',17,'�','�','�','�','�','�','-','�',
'�','�','�','�','�','�','�','�','�','*','\'','�','�','�','�','*','�','�','A','A','�','�','�','�',
'�','E','�','E','E','I','I','I','I','D','�','O','O','O','�','�','x','�','U','U','U','�','Y','*',
'�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�','�',
'�','�','�','�','�','y','*','�'};

char pdf_title[256];


void delete_bookmark_child( Bookmark *book)
{
	int i;

	/* if nothing to do, end the function */
	if( book->child == NULL)
		return;

	/* make a loop to see if the childs mini entries have child, if it's true, delete it */
	for ( i = 0; i < book->nbr_child ; i++)
	{
		if ( book->child[i].nbr_child)
			delete_bookmark_child( &book->child[i]);
	}

	/* Free the memory and put the counter to zero */
	gfree( book->child);
	book->child 		= NULL;
	book->nbr_child 	= 0;
}


int setupOutlineItems( WINDOW * win, PDFDoc *doc, GList *items, UnicodeMap *uMap, Bookmark *book, Bookmark *parent)
{
	OutlineItem *item;
	GList *kids;
	LinkAction	*link_action;
	char buf[8], *test;
	int i, j, n, count, lenght;

	for (i = 0; i < items->getLength(); ++i)
	{
	    item = (OutlineItem *)items->get(i);

		book[i].parent		= parent;
		book[i].child		= NULL;
		book[i].nbr_child	= 0;
		book[i].state 		= UNKNOWN;
		book[i].valid		= FALSE;

		link_action = item->getAction();

		if( link_action && link_action->getKind () == actionGoTo)
		{
			LinkGoTo *link = dynamic_cast <LinkGoTo *> (link_action);
			LinkDest *link_dest = link->getDest();
			GString *named_dest = link->getNamedDest ();

			if (link_dest != NULL)
			{
				if (link_dest->isPageRef ())
				{
					Ref page_ref = link_dest->getPageRef ();
					book[i].linked_page = doc->findPage( page_ref.num, page_ref.gen) - 1;
				}
				else
				{
					book[i].linked_page = link_dest->getPageNum() - 1;
				}

				book[i].valid = TRUE;
			}
			else if( named_dest != NULL)
			{
				link_dest = doc->findDest (named_dest);

				if (link_dest->isPageRef ())
				{
					Ref page_ref = link_dest->getPageRef ();
					book[i].linked_page = doc->findPage( page_ref.num, page_ref.gen) - 1;
				}
				else
				{
					book[i].linked_page = link_dest->getPageNum () - 1;
				}

				delete link_dest;
				book[i].valid = TRUE;
			}
		}

		if( book[i].valid == FALSE)
			continue;

		test = book[i].name;


		/* Unicode to Latin 1 */
	    for (j = 0, lenght = 0; j < item->getTitleLength(); ++j)
	    {
      		n = uMap->mapUnicode(item->getTitle()[j], buf, sizeof(buf));

			for( count = 0; n > count && lenght < 254; count++)
	   			test[lenght++] = buf[count++];
    	}

	  	test[lenght] = '\0';

		/* Latin 1 to Atari */
	    for ( lenght = 0; test[lenght] != '\0'; ++lenght)
	    {
			if( ( uint8)test[lenght] > 0x9F)
	    	{
  				uint8 c = ( uint8)test[lenght] - 0xA0;
				test[lenght] = latin_to_atari[c];
    		}
    	}

		book[i].txt_width = get_text_width( book[i].name);

	    item->open();

		if( item->hasKids() && ( kids = item->getKids()) && kids->getLength() > 0)
	    {
			book[i].nbr_child	= kids->getLength();
			book[i].child		= ( Bookmark*)gmalloc( sizeof( Bookmark) * kids->getLength());
      		book[i].state 		= item->isOpen();

      		if( !setupOutlineItems(win, doc, kids, uMap, book[i].child, book))
			{
				gfree( book[i].child);
				book[i].child 		= NULL;
				book[i].nbr_child	= 0;
				return( 0);
			}
    	}
	}
	return( 1);
}

void pdf_build_bookmark( WINDATA *windata, WINDOW *win)
{
	IMAGE		*img = &windata->img;
	PDFDoc		*doc = ( PDFDoc*)img->_priv_ptr;
	Outline 	*outline;
	GList 		*items;
	int			i;

	outline = doc->getOutline();

	if( outline == NULL)
		return;

	items = outline->getItems();

	if( items == NULL)
		return;

	if ( items->getLength() > 0)
	{
		windata->root = ( Bookmark*)gmalloc( sizeof( Bookmark) * items->getLength());

		if( windata->root == NULL)
			return;

    	GString *enc = new GString("Latin1");
    	UnicodeMap *uMap = globalParams->getUnicodeMap(enc);
    	delete enc;

    	windata->nbr_bookmark = items->getLength();

    	if( !setupOutlineItems( win, doc, items, uMap, windata->root, NULL))
		{
			for ( i = 0; i < windata->nbr_bookmark; i++)
			{
				if ( windata->root[i].nbr_child)
					delete_bookmark_child( &windata->root[i]);
			}

			gfree( windata->root);
			windata->root 			= NULL;
			windata->nbr_bookmark	= 0;
		}

    	uMap->decRefCnt();
	}
}


boolean lib_pdf_load( const char *name, IMAGE *img)
{
	PDFDoc *doc = NULL;
	SplashColor paperColor;
	SplashOutputDev *splashOut = NULL;

	char* aaString = const_cast<char*>( pdf_aa ? "yes" : "no");
	globalParams->setAntialias( aaString);

        GString nameString( name);
	doc = new PDFDoc( &nameString);

	if (!doc->isOk())
	{
		delete doc;
		return FALSE;
	}

	paperColor[0] = paperColor[1] = paperColor[2] = 0xff;
	splashOut = new SplashOutputDev( splashModeRGB8, 1, gFalse, paperColor);
	splashOut->startDoc(doc->getXRef());

	img->bits 				= 24;
	img->colors  			= ( 1L << ( uint32)img->bits) - 1;
	img->page	 			= ( uint16)doc->getNumPages();
	img->comments 			= NULL;
	img->_priv_ptr			= ( void*)doc;
	img->_priv_ptr_more		= ( void*)splashOut;

	strcpy( ( char*)img->info, "PDF");
	strcpy( ( char*)img->compression, "None");

	return TRUE;
}



void pdf_get_page_size( IMAGE *img, int page, uint16 *width, uint16 *height)
{
	PDFDoc *doc = ( PDFDoc*)img->_priv_ptr;
	int rotation = doc->getPageRotate( page);

	if( ( rotation == 90) || ( rotation == 270))
	{
		*width	= (uint16)doc->getPageCropHeight( page);
		*height	= (uint16)doc->getPageCropWidth( page);
	}
	else
	{
		*width	= (uint16)doc->getPageCropWidth( page);
		*height	= (uint16)doc->getPageCropHeight( page);
	}
}


void pdf_decode_page( IMAGE *img, int page, double scale)
{
	PDFDoc *doc = ( PDFDoc*)img->_priv_ptr;
	SplashOutputDev *splashOut = ( SplashOutputDev*)img->_priv_ptr_more;
	int rotation = doc->getPageRotate( page);

	if( ( rotation == 90) || ( rotation == 270))
	{
		img->img_w	= (uint16)MAX(( doc->getPageCropHeight( page) * scale + 0.5), 1);
		img->img_h	= (uint16)MAX(( doc->getPageCropWidth( page) * scale + 0.5), 1);
	}
	else
	{
		img->img_w	= (uint16)MAX(( doc->getPageCropWidth( page) * scale + 0.5), 1);
		img->img_h	= (uint16)MAX(( doc->getPageCropHeight( page) * scale + 0.5), 1);
	}

	doc->displayPage( splashOut, page, 72.0 * scale, 72.0 * scale, 0, gFalse, gTrue, gFalse);
}


uint32 *pdf_get_page_addr( IMAGE *img)
{
	SplashOutputDev *splashOut = ( SplashOutputDev*)img->_priv_ptr_more;
  	SplashColorPtr data = splashOut->getBitmap()->getDataPtr();

	return ( uint32*)( data); // ( data.rgb8);
}

static char *InfoString(Dict *infoDict, char *key, UnicodeMap *uMap)
{
	Object obj;
	GString *s1;
	GBool isUnicode;
	Unicode u;
	char buf[8];
	static char text[256];
	int i, n, lenght = 0, count;

	if (infoDict->lookup(key, &obj)->isString())
	{
    	s1 = obj.getString();

    	if ((s1->getChar(0) & 0xff) == 0xfe && (s1->getChar(1) & 0xff) == 0xff)
    	{
    		isUnicode = gTrue;
      		i = 2;
    	}
    	else
    	{
      		isUnicode = gFalse;
      		i = 0;
    	}

		while ( i < obj.getString()->getLength())
		{
      		if (isUnicode)
      		{
				u = ((s1->getChar(i) & 0xff) << 8) | (s1->getChar(i+1) & 0xff);
				i += 2;
      		}
      		else
      		{
				u = s1->getChar(i) & 0xff;
				++i;
      		}

		    n = uMap->mapUnicode(u, buf, sizeof(buf));

			count = 0;

			if(( n + lenght) > 234)
			{
				text[lenght] = '\0';
				obj.free();
				return ( char*)&text;
			}

			while( n > count)
      			text[lenght++] = buf[count++];
    	}
  	}

  	text[lenght] = '\0';
  	obj.free();

  	return ( char*)&text;
}

static char *InfoDate(Dict *infoDict, char *key)
{
	Object obj;
	char *s;
	int year, mon, day, hour, min, sec;
	struct tm tmStruct;
	static char buf[256];

	if (infoDict->lookup(key, &obj)->isString())
	{
		s = obj.getString()->getCString();

    	if (s[0] == 'D' && s[1] == ':')
    	{
	      	s += 2;
	    }

    	if (sscanf(s, "%4d%2d%2d%2d%2d%2d", &year, &mon, &day, &hour, &min, &sec) == 6)
    	{
      		tmStruct.tm_year = year - 1900;
      		tmStruct.tm_mon = mon - 1;
      		tmStruct.tm_mday = day;
      		tmStruct.tm_hour = hour;
      		tmStruct.tm_min = min;
      		tmStruct.tm_sec = sec;
      		tmStruct.tm_wday = -1;
      		tmStruct.tm_yday = -1;
      		tmStruct.tm_isdst = -1;
      		// compute the tm_wday and tm_yday fields
      		if (mktime(&tmStruct) != (time_t)-1 && strftime(buf, sizeof(buf), "%c", &tmStruct))
      		{
				obj.free();
				return ( char*)&buf;
      		}
    	}

		strcpy( buf, s);
  	}
	else buf[0] = '\0';

  	obj.free();
  	return ( char*)&buf;
}

void pdf_get_info( IMAGE *img, txt_data *txtdata)
{
	Object info;
	UnicodeMap *uMap;
  	PDFDoc *doc = ( PDFDoc*)img->_priv_ptr;
	char *txt;

	if (!( uMap = globalParams->getTextEncoding()))
	{
		pdf_title[0] = '\0';
		txtdata->txt[0][0] = '\0';
		txtdata->txt[1][0] = '\0';
		txtdata->txt[2][0] = '\0';
		txtdata->txt[3][0] = '\0';
		txtdata->txt[4][0] = '\0';
		txtdata->txt[5][0] = '\0';
		txtdata->txt[6][0] = '\0';
		txtdata->txt[7][0] = '\0';
	    return;
	}

	doc->getDocInfo(&info);

	if (info.isDict())
	{
		txt = InfoString(info.getDict(), "Title", uMap);
		sprintf( ( char*)txtdata->txt[0] , "Title:  %s", txt);
		strcpy( pdf_title, txt);
		txt = InfoString(info.getDict(), "Subject", uMap);
		sprintf( ( char*)txtdata->txt[1] , "Subject:  %s", txt);
		txt = InfoString(info.getDict(), "Keywords", uMap);
		sprintf( ( char*)txtdata->txt[2] , "Keywords:  %s", txt);
		txt = InfoString(info.getDict(), "Author", uMap);
		sprintf( ( char*)txtdata->txt[3] , "Author:  %s", txt);
		txt = InfoString(info.getDict(), "Creator", uMap);
		sprintf( ( char*)txtdata->txt[4] , "Creator:  %s", txt);
		txt = InfoString(info.getDict(), "Producer", uMap);
		sprintf( ( char*)txtdata->txt[5] , "Producer:  %s", txt);
//		sprintf( ( char*)txtdata->txt[6] , "PDF version:       %.1f", doc->getPDFVersion());

		txt = InfoDate(info.getDict(), "CreationDate");
		sprintf( ( char*)txtdata->txt[6] , "Creation Date:  %s", txt);
		txt = InfoDate(info.getDict(), "ModDate");
		sprintf( ( char*)txtdata->txt[7] , "Modification Date:  %s", txt);
	}
	else
	{
		pdf_title[0] = '\0';
		txtdata->txt[0][0] = '\0';
		txtdata->txt[1][0] = '\0';
		txtdata->txt[2][0] = '\0';
		txtdata->txt[3][0] = '\0';
		txtdata->txt[4][0] = '\0';
		txtdata->txt[5][0] = '\0';
		txtdata->txt[6][0] = '\0';
		txtdata->txt[7][0] = '\0';
	}

	info.free();
	uMap->decRefCnt();
}

void pdf_quit( IMAGE *img)
{
	PDFDoc *doc = ( PDFDoc*)img->_priv_ptr;
	SplashOutputDev *splashOut = ( SplashOutputDev*)img->_priv_ptr_more;

	delete splashOut;
	delete doc;
}

char *get_pdf_title( void)
{
  	return ( char*)&pdf_title;
}

boolean pdf_init( char *path)
{
	globalParams = new GlobalParams( NULL);
	globalParams->setupBaseFonts( path);
	return TRUE;
}

void pdf_exit( void)
{
	delete globalParams;
}

}
