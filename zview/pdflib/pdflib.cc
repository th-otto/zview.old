#include "../general.h"
#include "../winimg.h"
#include "../aconf.h"
#include "../custom_font.h"
#include "../xpdf/goo/GString.h"
#include "../goo/GList.h"
#include "../xpdf/GlobalParams.h"
#include "../xpdf/Object.h"
#include "../xpdf/PDFDoc.h"
#include "../splash/SplashBitmap.h"
#include "../splash/Splash.h"
#include "../xpdf/SplashOutputDev.h"
#include "../xpdf/CharTypes.h"
#include "../xpdf/UnicodeMap.h"
#include "../xpdf/Outline.h"
#include "../xpdf/Link.h"
#include "pdflib.h"
#include "../latin1.h"

extern "C" {

int16 (*p_get_text_width)(const char *str);

static void delete_bookmark_child( Bookmark *book)
{
	int i;

	/* if nothing to do, end the function */
	if (book == NULL || book->child == NULL)
		return;

	/* make a loop to see if the childs mini entries have child, if it's true, delete it */
	for ( i = 0; i < book->nbr_child ; i++)
	{
		if ( book->child[i].nbr_child)
			delete_bookmark_child( &book->child[i]);
	}

	/* Free the memory and put the counter to zero */
	free( book->child);
	book->child 		= NULL;
	book->nbr_child 	= 0;
}


void delete_bookmarks(WINDATA *windata)
{
	int i;

	if (windata->root == NULL)
		return;
	for (i = 0; i < windata->nbr_bookmark; i++)
	{
		if ( windata->root[i].nbr_child)
			delete_bookmark_child( &windata->root[i]);
	}

	free( windata->root);
	windata->root 			= NULL;
	windata->nbr_bookmark	= 0;
}


static void conv_latin1_to_atari(char *text)
{
	int length;
	
    for (length = 0; text[length] != '\0'; ++length)
    {
		uint8 c = text[length];
		text[length] = latin1_to_atari_table[c];
	}
}


static int setupOutlineItems( WINDOW * win, PDFDoc *doc, GList *items, UnicodeMap *uMap, Bookmark *book, Bookmark *parent)
{
	OutlineItem *item;
	GList *kids;
	LinkAction	*link_action;
	char buf[8], *test;
	int i, j, n, count, length;

	if (book == NULL)
		return FALSE;
	for (i = 0; i < items->getLength(); ++i)
	{
		book[i].parent		= parent;
		book[i].child		= NULL;
		book[i].nbr_child	= 0;
		book[i].state 		= UNKNOWN;
		book[i].valid		= FALSE;
	}
	for (i = 0; i < items->getLength(); ++i)
	{
	    item = (OutlineItem *)items->get(i);

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
	    for (j = 0, length = 0; j < item->getTitleLength(); ++j)
	    {
      		n = uMap->mapUnicode(item->getTitle()[j], buf, sizeof(buf));

			for( count = 0; n > count && length < 254; count++)
	   			test[length++] = buf[count++];
    	}

	  	test[length] = '\0';

		/* Latin 1 to Atari */
		conv_latin1_to_atari(test);

		book[i].txt_width = p_get_text_width( book[i].name);

	    item->open();

		if( item->hasKids() && ( kids = item->getKids()) && kids->getLength() > 0)
	    {
			book[i].nbr_child	= kids->getLength();
			book[i].child		= ( Bookmark*)malloc( sizeof( Bookmark) * kids->getLength());
      		book[i].state 		= item->isOpen();

      		if( !setupOutlineItems(win, doc, kids, uMap, book[i].child, book))
			{
				free( book[i].child);
				book[i].child 		= NULL;
				book[i].nbr_child	= 0;
				return FALSE;
			}
    	}
	}
	return TRUE;
}

void pdf_build_bookmark( WINDATA *windata, WINDOW *win)
{
	IMAGE		*img = &windata->img;
	PDFDoc		*doc = ( PDFDoc*)img->_priv_ptr;
	Outline 	*outline;
	GList 		*items;

	outline = doc->getOutline();

	if( outline == NULL)
		return;

	items = outline->getItems();

	if( items == NULL)
		return;

	if ( items->getLength() > 0)
	{
		windata->root = ( Bookmark*)malloc( sizeof( Bookmark) * items->getLength());

		if( windata->root == NULL)
			return;

    	GString *enc = new GString("Latin1");
    	UnicodeMap *uMap = globalParams->getUnicodeMap(enc);
    	delete enc;

    	windata->nbr_bookmark = items->getLength();

    	if( !setupOutlineItems( win, doc, items, uMap, windata->root, NULL))
		{
			delete_bookmarks(windata);
		}

    	uMap->decRefCnt();
	}
}


boolean lib_pdf_load( const char *name, IMAGE *img, boolean antialias)
{
	PDFDoc *doc = NULL;
	SplashColor paperColor;
	SplashOutputDev *splashOut = NULL;

	const char* aaString = antialias ? "yes" : "no";
	globalParams->setAntialias( aaString);

    GString *nameString = new GString(name);
	doc = new PDFDoc(nameString);

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

	strcpy( img->info, "PDF");
	strcpy( img->compression, "None");

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

static void InfoString(Dict *infoDict, const char *key, UnicodeMap *uMap, char *text)
{
	Object obj;
	GString *s1;
	GBool isUnicode;
	Unicode u;
	char buf[8];
	int i, n, length = 0, count;

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

			if(( n + length) > 234)
			{
				text[length] = '\0';
				obj.free();
				return;
			}

			while( n > count)
      			text[length++] = buf[count++];
    	}
  	}

  	text[length] = '\0';
  	obj.free();
}

static void InfoDate(Dict *infoDict, const char *key, char *text)
{
	Object obj;
	char *s;
	int year, mon, day, hour, min, sec;
	struct tm tmStruct;

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
      		if (mktime(&tmStruct) != (time_t)-1 && strftime(text, 256 - 1, "%c", &tmStruct))
      		{
				obj.free();
				return;
      		}
    	}

		strcpy( text, s);
  	}
	else text[0] = '\0';

  	obj.free();
}


static char *info_string(Object &info, 	UnicodeMap *uMap, const char *tag, const char *prefix, bool isdate = false)
{
	char text[256];
	char *str;

	if (isdate)
		InfoDate(info.getDict(), tag, text);
	else
		InfoString(info.getDict(), tag, uMap, text);
	if (*text == '\0')
		return NULL;
	conv_latin1_to_atari(text);
	str = (char *)malloc(strlen(prefix) + 3 + strlen(text) + 1);
	if (str != NULL)
		strcat(strcat(strcpy(str, prefix), ":  "), text);
	return str;
}

void pdf_get_info( IMAGE *img, txt_data *txtdata)
{
	Object info;
	UnicodeMap *uMap;
  	PDFDoc *doc = ( PDFDoc*)img->_priv_ptr;
	int num_comments = 0;

	if (!( uMap = globalParams->getTextEncoding()))
	{
	    return;
	}

	doc->getDocInfo(&info);

	if (info.isDict())
	{
		if ((txtdata->txt[num_comments] = info_string(info, uMap, "Title", "Title")) != NULL)
			num_comments++;
		if ((txtdata->txt[num_comments] = info_string(info, uMap, "Subject", "Subject")) != NULL)
			num_comments++;
		if ((txtdata->txt[num_comments] = info_string(info, uMap, "Keywords", "Keywords")) != NULL)
			num_comments++;
		if ((txtdata->txt[num_comments] = info_string(info, uMap, "Author", "Author")) != NULL)
			num_comments++;
		if ((txtdata->txt[num_comments] = info_string(info, uMap, "Creator", "Creator")) != NULL)
			num_comments++;
		if ((txtdata->txt[num_comments] = info_string(info, uMap, "Producer", "Producer")) != NULL)
			num_comments++;
		if ((txtdata->txt[num_comments] = info_string(info, uMap, "CreationDate", "Creation Date", true)) != NULL)
			num_comments++;
		if ((txtdata->txt[num_comments] = info_string(info, uMap, "ModDate", "Modification Date", true)) != NULL)
			num_comments++;

//		sprintf( txtdata->txt[num_comments++] , "PDF version:       %.1f", doc->getPDFVersion());

	}
	txtdata->lines = num_comments;

	info.free();
	uMap->decRefCnt();
}

void pdf_quit( IMAGE *img)
{
	PDFDoc *doc = ( PDFDoc*)img->_priv_ptr;
	SplashOutputDev *splashOut = ( SplashOutputDev*)img->_priv_ptr_more;

	delete splashOut;
	delete doc;
	img->_priv_ptr = NULL;
	img->_priv_ptr_more = NULL;
}

boolean pdf_init( const char *path)
{
	globalParams = new GlobalParams( NULL);
	if (globalParams == NULL)
		return FALSE;
	globalParams->setupBaseFonts( path);
	return TRUE;
}

void pdf_exit( void)
{
	if (globalParams)
	{
		delete globalParams;
		globalParams = NULL;
	}
}

}

/*
 * avoid pulling in references to std::terminate() and write()
 */
#include <cxxabi.h>

extern "C" void
__cxxabiv1::__cxa_pure_virtual (void)
{
  Cconws("pure virtual method called\n");
  Pterm(-1);
  __builtin_unreachable();
}
