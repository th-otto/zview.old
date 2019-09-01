#include "general.h"
#include "unicodemap.h"
#include "latin1.h"


struct UnicodeMapRange {
	unsigned short start;
	unsigned char nBytes;
	unsigned char bytes[3];
};

static struct UnicodeMapRange const latin1UnicodeMapRanges[] = {
  { 0x00a0, 1, { 0x20 } },
  { 0x010c, 1, { 0x43 } },
  { 0x010d, 1, { 0x63 } },
  { 0x0131, 1, { 0x69 } },
  { 0x0141, 1, { 0x4c } },
  { 0x0142, 1, { 0x6c } },
  { 0x0152, 2, { 0x4f, 0x45 } },
  { 0x0153, 2, { 0x6f, 0x65 } },
  { 0x0160, 1, { 0x53 } },
  { 0x0161, 1, { 0x73 } },
  { 0x0178, 1, { 0x59 } },
  { 0x017d, 1, { 0x5a } },
  { 0x017e, 1, { 0x7a } },
  { 0x02c6, 1, { 0x5e } },
  { 0x02da, 1, { 0xb0 } },
  { 0x02dc, 1, { 0x7e } },
  { 0x2010, 1, { 0x2d } },
  { 0x2013, 1, { 0xad } },
  { 0x2014, 2, { 0x2d, 0x2d } },
  { 0x2018, 1, { 0x60 } },
  { 0x2019, 1, { 0x27 } },
  { 0x201a, 1, { 0x2c } },
  { 0x201c, 1, { 0x22 } },
  { 0x201d, 1, { 0x22 } },
  { 0x201e, 2, { 0x2c, 0x2c } },
  { 0x2022, 1, { 0xb7 } },
  { 0x2026, 3, { 0x2e, 0x2e, 0x2e } },
  { 0x2039, 1, { 0x3c } },
  { 0x203a, 1, { 0x3e } },
  { 0x2044, 1, { 0x2f } },
  { 0x2122, 2, { 0x54, 0x4d } },
  { 0x2212, 1, { 0x2d } },
  { 0xf6f9, 1, { 0x4c } },
  { 0xf6fa, 2, { 0x4f, 0x45 } },
  { 0xf6fc, 1, { 0xb0 } },
  { 0xf6fd, 1, { 0x53 } },
  { 0xf6fe, 1, { 0x7e } },
  { 0xf6ff, 1, { 0x5a } },
  { 0xf721, 1, { 0x21 } },
  { 0xf724, 1, { 0x24 } },
  { 0xf726, 1, { 0x26 } },
  { 0xf73f, 1, { 0x3f } },
  { 0xf7a1, 1, { 0xa1 } },
  { 0xf7a2, 1, { 0xa2 } },
  { 0xf7bf, 1, { 0xbf } },
  { 0xf7ff, 1, { 0x59 } },
  { 0xfb00, 2, { 0x66, 0x66 } },
  { 0xfb01, 2, { 0x66, 0x69 } },
  { 0xfb02, 2, { 0x66, 0x6c } },
  { 0xfb03, 3, { 0x66, 0x66, 0x69 } },
  { 0xfb04, 3, { 0x66, 0x66, 0x6c } },
  { 0xfb05, 2, { 0x73, 0x74 } },
  { 0xfb06, 2, { 0x73, 0x74 } }
};
#define latin1UnicodeMapLen (sizeof(latin1UnicodeMapRanges) / sizeof(latin1UnicodeMapRanges[0]))


unsigned short *utf8_to_ucs16(const char *s, size_t len)
{
	unsigned char c0, c1, c2;
	unsigned short *u, *ret;
	
	if (s == NULL)
		return NULL;
	ret = malloc((len + 1) * sizeof(*u));
	if (ret == NULL)
		return NULL;
	u = ret;
	while (len > 0)
	{
		c0 = *s++;
		if (c0 < 0x80)
		{
			*u++ = c0;
			len -= 1;
		} else if (c0 < 0xe0 && len >= 2)
		{
			if (((c1 = s[0]) & 0xc0) == 0x80)
			{
				s++;
				*u++ = (((c0 & 0x1f) << 6) | (c1 & 0x3f));
				len -= 2;
			} else
			{
				*u++ = c0;
				len -= 1;
			}
		} else if (c0 < 0xf0 && len >= 3)
		{
			if (((c1 = s[0]) & 0xc0) == 0x80 && ((c2 = s[1]) & 0xc0) == 0x80)
			{
				s += 2;
				*u++ = (((c0 & 0x0f) << 12) | ((c1 & 0x3f) << 6) | (c2 & 0x3f));
				len -= 3;
			} else
			{
				*u++ = c0;
				len -= 1;
			}
		} else
		{
			*u++ = c0;
			len -= 1;
		}
	}
	*u = 0;
	return ret;
}


char *ucs16_to_latin1(const unsigned short *u, size_t len)
{
	unsigned int a,	b, m;
	char *ret, *buf;
	unsigned short code;
	const struct UnicodeMapRange *ranges = latin1UnicodeMapRanges;
	
	if (u == NULL)
		return NULL;
	ret = (char *)malloc(len * 3 + 1);
	if (ret == NULL)
		return NULL;
	buf = ret;
	while (len > 0)
	{
		code = *u++;
		a = 0;
		b = latin1UnicodeMapLen;
		if (code >= ranges[a].start)
		{
			while (b - a > 1)
			{
				m = (a + b) / 2;
				if (code >= ranges[m].start)
				{
					a = m;
				} else if (code < ranges[m].start)
				{
					b = m;
				}
			}
		}
		if (code == ranges[a].start)
		{
			unsigned char i;

			for (i = 0; i < ranges[a].nBytes; i++)
			{
				*buf++ = ranges[a].bytes[i];
			}
		} else if (code < 0x100)
		{
			*buf++ = code;
		} else
		{
			*buf++ = '?';
		}
		len--;
	}
	*buf = '\0';
	return ret;
}


void latin1_to_atari(char *text)
{
	int length;
	
    if (text == NULL)
    	return;
    for (length = 0; text[length] != '\0'; ++length)
    {
		uint8 c = text[length];
		text[length] = latin1_to_atari_table[c];
	}
}
