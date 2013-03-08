typedef struct slider 
{
	float 	min, max;
	float 	sinc, linc;
	int 	ori;		/* HORI,VERT*/
	int 	upd;		/* */
	float 	value;	/* valeur courante */
	void 	(*doslid)( WINDOW *win, int mode, float value, void *data); /* Pas fix‚ sur les sp‚cifs */
	void 	*data;		/* user data for user function */
	int 	mode;
	WINDOW 	*win;
	int 	up, dn, sld, bsld;
} SLIDER;

typedef struct
{
	txt_data	*txt;		
	int32       ypos;   	 		 	/* relative data position in the window		*/
	int32       ypos_max;	 			/* Maximal values of previous variables		*/
	int16       h_u;    	   			/* vertical and horizontal scroll offset	*/
	int32       xpos;
	int32       xpos_max;
	int16       w_u;
} exif_box_data;


extern void infobox( void);


