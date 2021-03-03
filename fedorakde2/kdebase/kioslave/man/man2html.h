/* call this with the buffer you have */
void scan_man_page(const char *man_page);

/* implement this somewhere. It will be called
   with HTML contents
*/
extern void output_real(const char *insert);

/*
 * called for requested man pages. filename can be a
 * relative path! Return NULL on errors. The returned
 * char array is freed by man2html
 */
extern char *read_man_page(const char *filename);
