static uint8_t *depack_line(uint8_t *img, uint8_t *des, int16_t pat_len, int32_t len, int16_t *repcnt)
{
	int16_t i;
	int16_t cnt;

	*repcnt = 1;

	while (len > 0)
	{
		uint8_t tag = *img++;

		if (tag == 0)
		{
			cnt = *img++;
			if (cnt == 0)
			{
				img++;					/* skip 0xff */
				*repcnt = *img++;		/* keep */
			} else
			{
				for (i = 0; i < cnt; i++)
				{
					int16_t j;

					for (j = 0; j < pat_len; j++)
					{
						*des++ = img[j];
					}
				}
				img += pat_len;
				cnt *= pat_len;
			}
		} else if (tag == 0x80)
		{
			cnt = *img++;
			for (i = 0; i < cnt; i++)
			{
				*des++ = *img++;
			}
		} else if (tag & 0x80)
		{
			cnt = tag & 0x7f;
			for (i = 0; i < cnt; i++)
			{
				*des++ = 0xff;
			}
		} else
		{
			cnt = tag & 0x7f;
			for (i = 0; i < cnt; i++)
			{
				*des++ = 0;
			}
		}
		len -= cnt;
	}
	return img;
}


static int16_t decode_standard(uint8_t *temp, uint8_t *bmap, uint16_t ih, uint16_t hdr_pl, uint32_t bps)
{
	uint8_t *line;
	uint8_t *tmp;
	int16_t i;
	int16_t repcount;
	uint32_t y;

	line = malloc(bps + 256L);
	if (line == NULL)
	{
		return 1;						/* error */
	}

	tmp = temp;
	y = 0;
	do
	{
		tmp = depack_line(tmp, line, hdr_pl, bps, &repcount);
		for (i = 0; i < repcount; i++)
		{
			memcpy(&bmap[y * bps], line, bps);
			y++;
		}
	} while (y < ih);
	free(line);
	return 0;							/* ok */
}


static uint32_t decode_ventura(uint8_t *temp, uint8_t *bmap, uint32_t bms)
{
	uint32_t src = 0;
	uint32_t dst = 0;
	uint8_t b, c;
	uint16_t i;

	do
	{
		b = temp[src++];
		if (b == 0x80)
		{
			c = temp[src++];
			for (i = 0; i < c; i++)
			{
				bmap[dst++] = temp[src + 2];	/* red */
				bmap[dst++] = temp[src + 1];	/* green */
				bmap[dst++] = temp[src];	/* blue */
				src = src + 3;
			}
		} else
		{								/* depack error? */
			break;
		}
	} while (dst < bms);
	return dst;
}
