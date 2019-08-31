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

extern "C" {

int16 (*p_get_text_width)(const char *str);

static unsigned char const latin1_to_atari[] = {
	/* 0x0000 */ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	/* 0x0008 */ 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	/* 0x0010 */ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	/* 0x0018 */ 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	/* 0x0020 */ 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	/* 0x0028 */ 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	/* 0x0030 */ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	/* 0x0038 */ 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	/* 0x0040 */ 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	/* 0x0048 */ 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	/* 0x0050 */ 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	/* 0x0058 */ 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	/* 0x0060 */ 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	/* 0x0068 */ 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	/* 0x0070 */ 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	/* 0x0078 */ 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
	/* 0x0080 */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	/* 0x0088 */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	/* 0x0090 */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	/* 0x0098 */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	/* 0x00A0 */ 0x2A, 0xAD, 0x9B, 0x9C, 0xde, 0x9D, 0x7c, 0xDD,
	/* 0x00A8 */ 0xB9, 0xBD, 0xA6, 0xAE, 0xAA, 0x2D, 0xBE, 0xFF,
	/* 0x00b0 */ 0xF8, 0xF1, 0xFD, 0xFE, 0xBA, 0xE6, 0xBC, 0xFA,
	/* 0x00b8 */ 0x2A, 0x27, 0xA7, 0xAF, 0xAC, 0xAB, 0x2A, 0xA8,
	/* 0x00c0 */ 0xB6, 0x41, 0x41, 0xB7, 0x8E, 0x8F, 0x92, 0x80,
	/* 0x00c8 */ 0x45, 0x90, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49,
	/* 0x00d0 */ 0x44, 0xA5, 0x4F, 0x4F, 0x4F, 0xB8, 0x99, 0x78,
	/* 0x00d8 */ 0xB2, 0x55, 0x55, 0x55, 0x9A, 0x59, 0x2A, 0x9E,
	/* 0x00e0 */ 0x85, 0xA0, 0x83, 0xB0, 0x84, 0x86, 0x91, 0x87,
	/* 0x00e8 */ 0x8A, 0x82, 0x88, 0x89, 0x8D, 0xA1, 0x8C, 0x8B,
	/* 0x00f0 */ 0xEB, 0xA4, 0x95, 0xA2, 0x93, 0xB1, 0x94, 0xF6,
	/* 0x00f8 */ 0xB3, 0x97, 0xA3, 0x96, 0x81, 0x79, 0x2A, 0x98
};


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
		text[length] = latin1_to_atari[c];
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

void pdf_get_info( IMAGE *img, txt_data *txtdata)
{
	Object info;
	UnicodeMap *uMap;
  	PDFDoc *doc = ( PDFDoc*)img->_priv_ptr;
	char text[256];

	txtdata->txt[0][0] = '\0';
	txtdata->txt[1][0] = '\0';
	txtdata->txt[2][0] = '\0';
	txtdata->txt[3][0] = '\0';
	txtdata->txt[4][0] = '\0';
	txtdata->txt[5][0] = '\0';
	txtdata->txt[6][0] = '\0';
	txtdata->txt[7][0] = '\0';

	if (!( uMap = globalParams->getTextEncoding()))
	{
	    return;
	}

	doc->getDocInfo(&info);

	if (info.isDict())
	{
		InfoString(info.getDict(), "Title", uMap, text);
		conv_latin1_to_atari(text);
		sprintf( txtdata->txt[0] , "Title:  %s", text);

		InfoString(info.getDict(), "Subject", uMap, text);
		conv_latin1_to_atari(text);
		sprintf( txtdata->txt[1] , "Subject:  %s", text);

		InfoString(info.getDict(), "Keywords", uMap, text);
		conv_latin1_to_atari(text);
		sprintf( txtdata->txt[2] , "Keywords:  %s", text);

		InfoString(info.getDict(), "Author", uMap, text);
		conv_latin1_to_atari(text);
		sprintf( txtdata->txt[3] , "Author:  %s", text);

		InfoString(info.getDict(), "Creator", uMap, text);
		conv_latin1_to_atari(text);
		sprintf( txtdata->txt[4] , "Creator:  %s", text);

		InfoString(info.getDict(), "Producer", uMap, text);
		conv_latin1_to_atari(text);
		sprintf( txtdata->txt[5] , "Producer:  %s", text);

//		sprintf( txtdata->txt[6] , "PDF version:       %.1f", doc->getPDFVersion());

		InfoDate(info.getDict(), "CreationDate", text);
		conv_latin1_to_atari(text);
		sprintf( txtdata->txt[6] , "Creation Date:  %s", text);

		InfoDate(info.getDict(), "ModDate", text);
		conv_latin1_to_atari(text);
		sprintf( txtdata->txt[7] , "Modification Date:  %s", text);
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
