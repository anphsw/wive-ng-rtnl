#ifndef HELPERS_H_
#define HELPERS_H_

#include <sys/types.h>

#define HTML_BUFFER_QUANTITY    1024

/* Special functions */
typedef struct replacement_t
{
	const char *key;
	const char *value;
} replacement_t;

typedef struct html_buffer_t
{
	size_t  size;   // Buffer capacity
	size_t  c_pos;  // Current buffer position
	char   *data;   // Buffer data
} html_buffer_t;

typedef struct html_replacement_t
{
	char            token;
	const char     *replacement;
} html_replacement_t;

extern const char *replaceWords(const char *key, const replacement_t *table);
extern const char *normalizeSize(long long *size);
extern long readUnsigned(const char *str);

// HTML content encoding routines
extern int initHTMLBuffer(html_buffer_t *buf);
extern int encodeHTMLContent(const char *data, html_buffer_t *buf);
extern int freeHTMLBuffer(html_buffer_t *buf);

extern int checkFileExists(const char *argv);

#endif /* HELPERS_H_ */

