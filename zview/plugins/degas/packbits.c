static void decode_packbits(unsigned char *dst, unsigned char *src, long int cnt)
{
	/*
	 * decode packbits
	 *  0 ..  127  n+1 literal bytes
	 * -1 .. -127 -n+1 copy's of next charachter
	 * stop decoding as soon as cnt bytes are done
	 */
	while (cnt > 0)
	{
		signed char cmd = (signed char) *src++;

		if (cmd >= 0)
		{								/* literals */
			int i = 1 + (int) cmd;

			if (i > cnt)
			{
				i = (int) cnt;
			}
			cnt -= i;
			while (i > 0)
			{
				*dst++ = *src++;
				i--;
			}
		} else if (cmd != -128)
		{
			int i = 1 - (int) cmd;
			unsigned char c = *src++;

			if (i > cnt)
			{
				i = (int) cnt;
			}
			cnt -= i;
			while (i > 0)
			{
				*dst++ = c;
				i--;
			}
		}
	}
}
