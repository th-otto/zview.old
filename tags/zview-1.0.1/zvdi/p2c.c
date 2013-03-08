#include "../general.h"

void ( *planar_to_chunky)( uint8 *in, uint8 *out, int16 width);


inline void planar8_to_chunky8( uint8 *in, uint8 *out, int16 width)
{
	uint8		*l;
	uint16		x, c, p0, p1, p2, p3, p4, p5, p6, p7;

	for( x = 0, l = in; x < width; l += 16)
	{
		p0 = ( l[0] << 8)  | l[1]; 
		p1 = ( l[2] << 8)  | l[3];
		p2 = ( l[4] << 8)  | l[5];
		p3 = ( l[6] << 8)  | l[7];
		p4 = ( l[8] << 8)  | l[9]; 
		p5 = ( l[10] << 8) | l[11];
		p6 = ( l[12] << 8) | l[13];
		p7 = ( l[14] << 8) | l[15];

        for ( c = 0; c < 16; c++)
        {
     	   out[x++] = ((p0 >> 15) & 1) | ((p1 >> 14) & 2) | ((p2 >> 13) & 4) | ((p3 >> 12) & 8) |
					  ((p4 >> 11) & 16) | ((p5 >> 10) & 32)| ((p6 >> 9) & 64) | ((p7 >> 8)  & 128);
           p0 <<= 1;
           p1 <<= 1;
           p2 <<= 1;
           p3 <<= 1;
           p4 <<= 1;
           p5 <<= 1;
           p6 <<= 1;
           p7 <<= 1;
        }
	}
}


inline void planar4_to_chunky8( uint8 *in, uint8 *out, int16 width)
{
	uint8		*l;
	uint16		x, c, p0, p1, p2, p3;

	for( x = 0, l = in; x < width; l += 8)
	{
		p0 = ( l[0] << 8)  | l[1]; 
		p1 = ( l[2] << 8)  | l[3];
		p2 = ( l[4] << 8)  | l[5];
		p3 = ( l[6] << 8)  | l[7];

        for ( c = 0; c < 16; c++)
        {
     	   out[x++] = ((p0 >> 15) & 1) | ((p1 >> 14) & 2) | ((p2 >> 13) & 4) | ((p3 >> 12) & 8);

           p0 <<= 1;
           p1 <<= 1;
           p2 <<= 1;
           p3 <<= 1;
        }
	}
}


inline void planar2_to_chunky8( uint8 *in, uint8 *out, int16 width)
{
	uint8		*l;
	uint16		x, c, p0, p1;

	for ( x = 0, l = in; x < width; l += 4)
    {
    	p0 = (l[0] << 8) | l[1];
    	p1 = (l[2] << 8) | l[3];

    	for (c = 0; c < 16; c++)
        {
    	    out[x++] = ((p0 >> 15) & 1) | ((p1 >> 14) & 2);
            p0 <<= 1;
            p1 <<= 1;
        }
    }
}

