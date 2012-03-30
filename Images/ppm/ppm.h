/* $Id: ppm.h 2 2008-09-21 01:55:30Z gophi $
 *
 * Portable Pixel Map (PPM) manipulation library.
 * (C) 2008 Adma Wysocki <gophi at chmurka.net>
 *
 * Latest version can be downloaded from: http://www.chmurka.net/p/libppm/
 */

#ifndef PPM_H__
#define PPM_H__

#include <inttypes.h>
#include <stdio.h>

#define PPM_MAGIC		0x004D5050	/* "PPM" */
#define PPM_VERSION_MAJOR	1
#define PPM_VERSION_MINOR	0

/* Union representing a pixel. Represents both 24-bit and 48-bit pixels. */
union ppm_pixel
{
	struct ppm_pixel_24bit_t
	{
		/* red, green and blue 8-bit values */
		uint8_t rgb[3];
	} b;	/* "b" like "byte" */

	struct ppm_pixel_48bit_t
	{
		/* red, green and blue 16-bit values */
		uint16_t rgb[3];
	} w;	/* "w" like "word" */
};

typedef union ppm_pixel ppm_pixel_t;

/* Enumeration of possible bit depths */
enum ppm_bpp
{
	PPM_BPP_24 = 0,	/* 8 bits per subpixel, 24 bits per pixel */
	PPM_BPP_48		/* 16 bits per subpixel, 48 bits per pixel */
};

typedef enum ppm_bpp ppm_bpp_t;

/* Structure representing ppm image */
struct ppm
{
	uint32_t magic;			/* must be PPM_MAGIC */
	uint16_t width, height;		/* width and height of the pixmap */
	ppm_bpp_t bpp;			/* bit depth */
	ppm_pixel_t *data;		/* data pointer */
};

typedef struct ppm ppm_t;

/* Enumeration of possible errors */
enum ppm_err
{
	PPM_ERR_OK = 0,		/* No error */
	PPM_ERR_MAGIC,		/* Invalid magic */
	PPM_ERR_ARGUMENT,	/* Invalid argument */
	PPM_ERR_MEMORY,		/* Out of memory */
	PPM_ERR_SYSTEM,		/* System error, consult errno */
	PPM_ERR_POSITION,	/* Position does not contain image */
	PPM_ERR_DATA,		/* Data format error */
	PPM_ERR_EOF,		/* Premature EOF */
	// new errors go here
	PPM_ERR_UNKNOWN		/* Unknown error */
};

typedef enum ppm_err ppm_err_t;

/* Used to get library version. Major version number in library must be the same 
 * as in header file. */
uint8_t ppm_version_major(void);
uint8_t ppm_version_minor(void);

/* Create black ppm image with given size and bit depth */
ppm_err_t ppm_create(ppm_t *ppm, uint16_t width, uint16_t height, ppm_bpp_t bpp);

/* Free ppm structures (but NOT ppm pointer itself!) */
ppm_err_t ppm_destroy(ppm_t *ppm);

/* Load pixmap from stdio open stream */
ppm_err_t ppm_load_fp(ppm_t *ppm, FILE *fp);

/* Save pixmap to stdio open stream */
ppm_err_t ppm_save_fp(const ppm_t *ppm, FILE *fp);

/* Load pixmap from named file */
ppm_err_t ppm_load_file(ppm_t *ppm, const char *path);

/* Save pixmap to named file */
ppm_err_t ppm_save_file(const ppm_t *ppm, const char *path);

/* Calculate pixel location for reading and writing */
ppm_err_t ppm_pixel(ppm_t *ppm, ppm_pixel_t **pixel, uint16_t x, uint16_t y);

/* Calculate pixel location for reading */
ppm_err_t ppm_pixel_const(const ppm_t *ppm, const ppm_pixel_t **pixel, uint16_t x, uint16_t y);

/* Print error to stderr */
void ppm_perror(ppm_err_t err, const char *s);

/* Get error string */
const char *ppm_strerror(ppm_err_t err);

#endif
