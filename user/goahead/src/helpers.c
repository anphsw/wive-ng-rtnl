#include "helpers.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#define _PATH_PROCNET_DEV      "/proc/net/dev"

// Sizes table
const char *normalSizes[] =
{
	"B",
	"kB",
	"MB",
	"GB",
	"TB",
	"PB",
	"HB",
	"ZB"
};

const html_replacement_t html_replacement[] =
{
	{ '<',          "&lt;"          },
	{ '>',          "&gt;"          },
	{ '"',          "&quot;"        },
	{ '&',          "&amp;"         },
	{ '\0',         NULL            }
};

// Make replacement by table, return NULL if no match
const char *replaceWords(const char *key, const replacement_t *table)
{
	// Make replacement by table
	for (; table->key != NULL; table++)
	{
		// Check if key matches
		if (strcmp(key, table->key)==0)
			return table->value;
	}
	return NULL;
}

// Normalize size
const char *normalizeSize(long long *size)
{
	const char **result = normalSizes;
	while ((*size)>(128*1024))
	{
		*size /= 1024;
		result++;
	}
	return *result;
}

// Read unsigned number
// Returns -1 on error
long readUnsigned(const char *str)
{
	long result = 0;
	while ((*str)!='\0')
	{
		char ch = *(str++);
		if ((ch<'0') || (ch>'9'))
			return -1;
		result = (result*10) + (ch-'0');
	}
	return result;
}

// Initialize HTML buffer
int initHTMLBuffer(html_buffer_t *buf)
{
	buf->size = HTML_BUFFER_QUANTITY;
	buf->data = (char *)malloc(HTML_BUFFER_QUANTITY);
	return (buf!=NULL) ? 0 : -errno;
}

static const char *html_find_match(char ch)
{
	const html_replacement_t *rep;
	for (rep = html_replacement; rep->token != '\0'; rep++)
		if (rep->token == ch)
			return rep->replacement;
	return NULL;
}

static int append_html_buffer(html_buffer_t *buf, char ch)
{
	// Check if we need to exend buffer
	if (buf->c_pos >= buf->size)
	{
		buf->size      += HTML_BUFFER_QUANTITY; // Increase buffer size
		char *new_buf = realloc(buf->data, buf->size);
		if (new_buf==NULL)
			return -errno;
		buf->data       = new_buf;
	}
	
	// Now append character
	buf->data[buf->c_pos++] = ch;
	return 0;
}

// Encode character data to HTML content
int encodeHTMLContent(const char *data, html_buffer_t *buf)
{
	buf->c_pos = 0; // Reset buffer position

	while ((*data)!='\0')
	{
		const char *rep = html_find_match(*data);
		
		if (rep==NULL) //Append character to buffer
		{
			int ret = append_html_buffer(buf, *data);
			if (ret<0) // Error happened?
				return ret;
		}
		else // Append string to buffer
		{
			while ((*rep)!='\0')
			{
				int ret = append_html_buffer(buf, *(rep++));
				if (ret<0) // Error happened?
					return ret;
			}
		}
		
		// Increment position
		data++;
	}
	
	// Append buffer with null-terminating character
	return append_html_buffer(buf, '\0');
}

// Free HTML buffer
int freeHTMLBuffer(html_buffer_t *buf)
{
	if (buf->data!=NULL)
		free(buf->data);
    return 0;
}

int checkFileExists(const char *fname)
{
	struct stat buf;
	int stat_res = stat(fname, &buf);

	// Return success if stat OK
	return (stat_res == 0) ? 1 : 0;
}
