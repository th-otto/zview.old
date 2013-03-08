extern void raster_chunks ( uint8 *src, uint16 *dst, uint16 num, uint16 depth);

#ifdef USE_ASM
extern void raster_chunk4 (uint8 *src, uint16* dst, size_t num);
extern void raster_chunk8 (uint8 *src, uint16* dst, size_t num);
#else
#define raster_chunk4(s,d,n)   raster_chunks ( s, d, n, 4)
#define raster_chunk8(s,d,n)   raster_chunks ( s, d, n, 8)
#endif

extern void raster_mono ( DECDATA info, void * _dst);
extern void raster_D1 ( DECDATA info, void * _dst);
extern void dither_D1 ( DECDATA info, void * _dst);
extern void gscale_D1 ( DECDATA info, void * _dst);

extern void raster_D2 ( DECDATA info, void * _dst);
extern void dither_D2 ( DECDATA info, void * _dst);
extern void gscale_D2 ( DECDATA info, void * _dst);

extern void raster_I4 ( DECDATA info, void * _dst);
extern void dither_I4 ( DECDATA info, void * _dst);
extern void gscale_I4 ( DECDATA info, void * _dst);

extern void raster_I8 ( DECDATA info, void * _dst);
extern void dither_I8 ( DECDATA info, void * _dst);
extern void gscale_I8 ( DECDATA info, void * _dst);

extern void raster_P8 ( DECDATA info, void * _dst);
extern void dither_P8 ( DECDATA info, void * _dst);
extern void gscale_P8 ( DECDATA info, void * _dst);

extern void raster_15 ( DECDATA info, void * _dst);
extern void dither_15 ( DECDATA info, void * _dst);
extern void gscale_15 ( DECDATA info, void * _dst);

extern void raster_15r ( DECDATA info, void * _dst);
extern void dither_15r ( DECDATA info, void * _dst);
extern void gscale_15r ( DECDATA info, void * _dst);

extern void raster_16 ( DECDATA info, void * _dst);
extern void dither_16 ( DECDATA info, void * _dst);
extern void gscale_16 ( DECDATA info, void * _dst);

extern void raster_16r ( DECDATA info, void * _dst);
extern void dither_16r ( DECDATA info, void * _dst);
extern void gscale_16r ( DECDATA info, void * _dst);

extern void raster_24 ( DECDATA info, void * _dst);
extern void dither_24 ( DECDATA info, void * _dst);
extern void gscale_24 ( DECDATA info, void * _dst);

extern void raster_24r ( DECDATA info, void * _dst);
extern void dither_24r ( DECDATA info, void * _dst);
extern void gscale_24r ( DECDATA info, void * _dst);

extern void raster_32 ( DECDATA info, void * _dst);
extern void dither_32 ( DECDATA info, void * _dst);
extern void gscale_32 ( DECDATA info, void * _dst);

extern void raster_32r ( DECDATA info, void * _dst);
extern void dither_32r ( DECDATA info, void * _dst);
extern void gscale_32r ( DECDATA info, void * _dst);

extern void raster_32z ( DECDATA info, void * _dst);
extern void dither_32z ( DECDATA info, void * _dst);
extern void gscale_32z ( DECDATA info, void * _dst);

