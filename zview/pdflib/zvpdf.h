#include <stdarg.h>

struct _zvpdf_funcs {
	/*
	 * sizeof(this struct), as
	 * used by the application.
	 */
	size_t struct_size;
	/*
	 * sizeof of an int of the caller.
	 * As for now, must match the one which
	 * was used to compile the library (32 bit)
	 */
	size_t int_size;
	/*
	 * version of zview.h the caller used.
	 * As for now, should match the version that was
	 * used to compile the library.
	 */
	long interface_version;

	long __CDECL (*p_slb_open)(zv_int_t lib);
	void __CDECL (*p_slb_close)(zv_int_t lib);
	SLB *__CDECL (*p_slb_get)(zv_int_t lib);

	/*
	 * functions defined in application
	 */
	int16 (*p_get_text_width)(const char *str);

	/*
	 * functions from C library
	 */
	FILE *stderr_location;

	void * (*p_malloc)(size_t s);
	void (*p_free)(void *p);
	void *(*p_realloc)(void *ptr, size_t s);

	int (*p_memcmp)(const void *d, const void *s, size_t n);
	void *(*p_memcpy)(void *d, const void *s, size_t n);
	void *(*p_memset)(void *d, int c, size_t n);

	size_t (*p_strlen)(const char *s);
	int (*p_strcmp)(const char *d, const char *s);
	int (*p_strncmp)(const char *d, const char *s, size_t n);
	char *(*p_strcpy)(char *d, const char *s);
	char *(*p_strncpy)(char *d, const char *s, size_t n);
	char *(*p_strtok)(char *str, const char *delim);
	char *(*p_strstr)(const char *str, const char *delim);
	char *(*p_strchr)(const char *str, int c);
	char *(*p_strrchr)(const char *str, int c);
	int (*p_strcasecmp)(const char *d, const char *s);
	size_t (*p_strcspn)(const char *s, const char *reject);
	
	char *(*p_getcwd)(char *buf, size_t size);
	void (*p_qsort)(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
	void (*p_srand)(unsigned int seed);
	int (*p_rand)(void);
	char *(*p_getenv)(const char *name);
	char *(*p_tmpnam)(char *s);
	int (*p_mkstemp)(char *template);
	
	time_t (*p_mktime)(struct tm *tm);
	size_t (*p_strftime)(char *s, size_t max, const char *format, const struct tm *tm);
	
	int (*p_isspace)(int c);
	int (*p_isalnum)(int c);
	int (*p_isalpha)(int c);
	int (*p_isxdigit)(int c);
	int (*p_isupper)(int c);
	int (*p_islower)(int c);
	int (*p_toupper)(int c);
	int (*p_tolower)(int c);
	
	int (*p_vsprintf)(char *str, const char *format, va_list args);
	int (*p_vfprintf)(FILE *fp, const char *format, va_list args);
	int (*p_fflush)(FILE *fp);
	int (*p_fputs)(const char *s, FILE *stream);
	int (*p_fputc)(int c, FILE *stream);
	FILE *(*p_fopen)(const char *pathname, const char *mode);
	int (*p_fclose)(FILE *fp);
	size_t (*p_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
	size_t (*p_fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);
	int (*p_fseek)(FILE *stream, long offset, int whence);
	long (*p_ftell)(FILE *stream);
	int (*p_unlink)(const char *pathname);
	int (*p_vsscanf)(const char *str, const char *format, va_list args);
	int (*p_atoi)(const char *nptr);
	double (*p_atof)(const char *nptr);
	int (*p_fgetc)(FILE *stream);
	int (*p_ungetc)(int c, FILE *stream);
	FILE *(*p_fdopen)(int fd, const char *mode);
};
