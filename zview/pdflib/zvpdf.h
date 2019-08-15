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
	long plugin_version;

	long __CDECL (*p_slb_open)(zv_int_t lib);
	void __CDECL (*p_slb_close)(zv_int_t lib);
	SLB *__CDECL (*p_slb_get)(zv_int_t lib);

	/*
	 * functions defined in application
	 */
	int16 (*p_get_text_width)(const char *str);
};
