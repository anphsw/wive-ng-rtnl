#ifndef HELPERS_H_
#define HELPERS_H_

#include <sys/types.h>
#include "webs.h"

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

typedef struct parameter_fetch_t
{
	const char *web_param;
	const char *nvram_param;
	int is_switch;
} parameter_fetch_t;

extern const char *replaceWords(const char *key, const replacement_t *table);
extern const char *normalizeSize(long long *size);
extern long readUnsigned(const char *str);

// HTML content encoding routines
extern int initHTMLBuffer(html_buffer_t *buf);
extern int encodeHTMLContent(const char *data, html_buffer_t *buf);
extern int freeHTMLBuffer(html_buffer_t *buf);

extern int checkFileExists(const char *argv);

// Set-up parameters in NVRAM
extern void setupParameters(webs_t wp, const parameter_fetch_t *fetch, int transaction);

#endif /* HELPERS_H_ */

