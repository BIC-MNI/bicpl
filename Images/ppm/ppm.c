/* $Id: ppm.c 8 2008-10-03 02:18:17Z gophi $ */

#define _XOPEN_SOURCE

#ifdef HAVE_UNISTD_H
  #include <unistd.h>
#endif 

#include <stdlib.h>
#include <string.h>
#include <assert.h>
// #include <endian.h>
#include <stdio.h>
#include <errno.h>

#ifdef HAVE_SYS_TYPES_H
  #include <sys/types.h>
#endif 

#ifdef HAVE_SYS_STAT_H
  #include <sys/stat.h>
#endif //HAVE_SYS_STAT_H

#include <arpa/inet.h>
#include <fcntl.h>
#include "ppm.h"

extern int errno;

static ppm_err_t safety_check(const ppm_t *ppm)
{
	if (!ppm)
		return PPM_ERR_ARGUMENT;

	if (ppm->magic != PPM_MAGIC)
		return PPM_ERR_MAGIC;

	if (ppm->bpp != PPM_BPP_24 && ppm->bpp != PPM_BPP_48)
		return PPM_ERR_ARGUMENT;

	return PPM_ERR_OK;
}

static void token_free(char *s)
{
	free(s);
}

static int token_add(char **s, size_t *sz, char ch)
{
	char *p = (char *) realloc(*s, *sz + 1);
	if (!p)
		return -1;

	p[(*sz)++] = ch;
	*s = p;

	return 0;
}

enum token_read_state
{
	STATE_NEWLINE = 0,
	STATE_COMMENT,
	STATE_WHITESPACE,
	STATE_TOKEN
};

static ppm_err_t token_read(char **s, FILE *fp, enum token_read_state *state)
{
	int done = 0;
	size_t sz = 0;

	*s = NULL;

	while (!done)
	{
		char ch = fgetc(fp);
		if (feof(fp) || ferror(fp))
		{
			token_free(*s);
			return feof(fp) ? PPM_ERR_EOF : PPM_ERR_SYSTEM;
		}

		if (ch == '\r')
			continue;

		switch (*state)
		{
			case STATE_NEWLINE:
				if (ch == '#')
				{
					*state = STATE_COMMENT;
					break;
				}

				/* FALLTHROUGH */
			case STATE_WHITESPACE:
				if (ch == '\n')
				{
					*state = STATE_NEWLINE;
					break;
				}

				if (ch == '\t' || ch == ' ')
					break;

				*state = STATE_TOKEN;
				/* FALLTHROUGH */

			case STATE_TOKEN:
				if (ch == '\t' || ch == ' ' || ch == '\n')
				{
					done = 1;
					break;
				}

				if (token_add(s, &sz, ch) == -1)
				{
					token_free(*s);
					return PPM_ERR_MEMORY;
				}
				break;

			case STATE_COMMENT:
				if (ch == '\n')
					state = STATE_NEWLINE;
				break;
		}
	}

	assert(s && *s);
	if (token_add(s, &sz, '\0') == -1)
	{
		token_free(*s);
		return PPM_ERR_MEMORY;
	}

	return PPM_ERR_OK;
}

static ppm_err_t token_read_int(int *i, FILE *fp, enum token_read_state *state)
{
	char *token;
	ppm_err_t rs;

	if ((rs = token_read(&token, fp, state)) != PPM_ERR_OK)
		return rs;

	*i = atoi(token);
	token_free(token);
	return PPM_ERR_OK;
}

static int pixel_read_24(ppm_pixel_t *pixel, FILE *fp)
{
	return (fread(&pixel->b, sizeof(pixel->b), 1, fp) == 1) ? 0 : -1;
}

static int pixel_read_48(ppm_pixel_t *pixel, FILE *fp)
{
	int rs = fread(&pixel->w, sizeof(pixel->w), 1, fp);
	if (rs != 1)
		return -1;

//	if (__BYTE_ORDER == __LITTLE_ENDIAN)
	if (htons(0x1234) == 0x3412)
		swab(&pixel->w, &pixel->w, sizeof(pixel->w));

	return 0;
}

static int pixel_write_24(const ppm_pixel_t *pixel, FILE *fp)
{
	return (fwrite(&pixel->b, sizeof(pixel->b), 1, fp) == 1) ? 0 : -1;
}

static int pixel_write_48(const ppm_pixel_t *pixel, FILE *fp)
{
	const ppm_pixel_t *p = pixel;
	ppm_pixel_t pix;

//	if (__BYTE_ORDER == __LITTLE_ENDIAN)
	if (htons(0x1234) == 0x3412)
	{
		swab(&pixel->w, &pix.w, sizeof(pixel->w));
		p = &pix;
	}

	return (fwrite(&p->w, sizeof(p->w), 1, fp) == 1) ? 0 : -1;
}

uint8_t ppm_version_major(void)
{
	return PPM_VERSION_MAJOR;
}

uint8_t ppm_version_minor(void)
{
	return PPM_VERSION_MINOR;
}

ppm_err_t ppm_create(ppm_t *ppm, uint16_t width, uint16_t height, ppm_bpp_t bpp)
{
	if (!ppm || (bpp != PPM_BPP_24 && bpp != PPM_BPP_48))
		return PPM_ERR_ARGUMENT;

	ppm->data = (ppm_pixel_t *) calloc(width * height, sizeof(ppm_pixel_t));
	if ((width || height) && !ppm->data)
		return PPM_ERR_MEMORY;

	memset(ppm->data, 0, width * height * sizeof(ppm_pixel_t));

	ppm->magic = PPM_MAGIC;
	ppm->width = width;
	ppm->height = height;
	ppm->bpp = bpp;

	return PPM_ERR_OK;
}

ppm_err_t ppm_destroy(ppm_t *ppm)
{
	ppm_err_t rs;
	if ((rs = safety_check(ppm)) != PPM_ERR_OK)
		return rs;

	free(ppm->data);
	ppm->magic = 0;

	return PPM_ERR_OK;
}

ppm_err_t ppm_load_fp(ppm_t *ppm, FILE *fp)
{
	ppm_err_t err;
	char *token;
	enum token_read_state state = STATE_NEWLINE;
	int (*rdfn)(ppm_pixel_t *pixel, FILE *fp);
	int w, h, b;
	ppm_bpp_t bpp;
	size_t i;

	if ((err = token_read(&token, fp, &state)) != PPM_ERR_OK)
		return err;

	if (strcmp(token, "P6"))
	{
		token_free(token);
		return PPM_ERR_DATA;
	}
	token_free(token);

	if ((err = token_read_int(&w, fp, &state)) != PPM_ERR_OK || 
		(err = token_read_int(&h, fp, &state)) != PPM_ERR_OK || 
		(err = token_read_int(&b, fp, &state)) != PPM_ERR_OK)
		return err;

	if (b >= 0 && b <= 255)
	{
		rdfn = pixel_read_24;
		bpp = PPM_BPP_24;
	}
	else if (b <= 65535)
	{
		rdfn = pixel_read_48;
		bpp = PPM_BPP_48;
	}
	else
		return PPM_ERR_DATA;

	ppm_err_t rs;
	if ((rs = ppm_create(ppm, w, h, bpp)) != PPM_ERR_OK)
		return rs;

	for (i = 0; i < (size_t) w * h; ++i)
		if (rdfn(&ppm->data[i], fp) == -1)
		{
			int se = errno;
			ppm_destroy(ppm);
			errno = se;
			return PPM_ERR_EOF;
		}

	return PPM_ERR_OK;
}

ppm_err_t ppm_save_fp(const ppm_t *ppm, FILE *fp)
{
	int (*wrfn)(const ppm_pixel_t *pixel, FILE *fp);
	ppm_err_t rs;
	size_t i;

	if ((rs = safety_check(ppm)) != PPM_ERR_OK)
		return rs;

	wrfn = (ppm->bpp == PPM_BPP_24) ? pixel_write_24 : pixel_write_48;

	if (fprintf(fp, "P6\n%u %u\n%u\n", ppm->width, ppm->height, (ppm->bpp == PPM_BPP_24) ? 255 : 65535) < 0)
		return PPM_ERR_SYSTEM;

	for (i = 0; i < (size_t) ppm->width * ppm->height; ++i)
		if (wrfn(&ppm->data[i], fp) == -1)
			return PPM_ERR_SYSTEM;

	return PPM_ERR_OK;
}

ppm_err_t ppm_load_file(ppm_t *ppm, const char *path)
{
	FILE *fp = fopen(path, "rb");
	if (!fp)
		return PPM_ERR_SYSTEM;

	ppm_err_t rs = ppm_load_fp(ppm, fp);

	int se = errno;
	fclose(fp);
	errno = se;

	return rs;
}

ppm_err_t ppm_save_file(const ppm_t *ppm, const char *path)
{
	FILE *fp;
	ppm_err_t rs;
	if ((rs = safety_check(ppm)) != PPM_ERR_OK)
		return rs;

	fp = fopen(path, "wb");
	if (!fp)
		return PPM_ERR_SYSTEM;

	if ((rs = ppm_save_fp(ppm, fp)) == PPM_ERR_OK)
	{
		fclose(fp);
		return PPM_ERR_OK;
	}

	int se = errno;
	fclose(fp);
	unlink(path);
	errno = se;

	return rs;
}

ppm_err_t ppm_pixel(ppm_t *ppm, ppm_pixel_t **pixel, uint16_t x, uint16_t y)
{
	ppm_err_t rs;
	if ((rs = safety_check(ppm)) != PPM_ERR_OK)
		return rs;

	if (x >= ppm->width || y >= ppm->height)
		return PPM_ERR_POSITION;

	*pixel = &ppm->data[y * ppm->width + x];
	return PPM_ERR_OK;
}

ppm_err_t ppm_pixel_const(const ppm_t *ppm, const ppm_pixel_t **pixel, uint16_t x, uint16_t y)
{
	ppm_err_t rs;
	if ((rs = safety_check(ppm)) != PPM_ERR_OK)
		return rs;

	if (x >= ppm->width || y >= ppm->height)
		return PPM_ERR_POSITION;

	*pixel = &ppm->data[y * ppm->width + x];
	return PPM_ERR_OK;
}

void ppm_perror(ppm_err_t err, const char *s)
{
	if (s && *s)
	{
		fputs(s, stderr);
		fputs(": ", stderr);
	}

	fputs(ppm_strerror(err), stderr);
	fputs("\n", stderr);
}

const char *ppm_strerror(ppm_err_t err)
{
	static struct
	{
		ppm_err_t err;
		const char *s;
	} const errors[] = 
	{
		{ PPM_ERR_OK,		"No error" },
		{ PPM_ERR_MAGIC,	"Invalid magic" },
		{ PPM_ERR_ARGUMENT,	"Invalid argument" },
		{ PPM_ERR_MEMORY,	"Out of memory" },
		{ PPM_ERR_SYSTEM,	"System error, consult errno" },
		{ PPM_ERR_POSITION,	"Position does not contain image" },
		{ PPM_ERR_DATA,		"Data format error" },
		{ PPM_ERR_EOF,		"Premature EOF" },
		// new errors go here
		{ PPM_ERR_UNKNOWN,	"Unknown error" }
	};
	size_t i;

	for (i = 0; i < sizeof(errors) / sizeof(*errors) - 1; ++i)
		if (err == errors[i].err)
			return errors[i].s;

	return errors[i].s;
}
