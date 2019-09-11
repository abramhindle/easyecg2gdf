/*
easyecg2gdf: command-line tool to convert EasyECG device recordings to an open format
Copyright (C) 2017 Peter Lawrence

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <string.h>
#include "easyecg.h"
#include "read_scp.h"
#include "write_gdf.h"

/* read a uint16 from a byte pointer */

static unsigned short get16(const unsigned char *p)
{
    unsigned short result;

    result  = *p++ & 0xff;
    result |= *p << 8;
    return (result);
}

/* write the EasyECG data pointed to by ctx->dataptr and ctx->datalen into a GDF-accepted data form */

static void write_data(FILE *output, struct easyecg_context *ctx)
{
	unsigned short val;
	const unsigned char *ptr;
	unsigned long samples;
	float sample;

	samples = ctx->datalen / 2;
	ptr = ctx->dataptr;

	while (samples--)
	{
		val = get16(ptr);
		ptr += sizeof(val);

		/*
		the upper 4 bits (perhaps only the MSB) of the EasyECG data signal conditions like 'no contact' and/or heartbeat detection
		The GDF specification says to use values outside the range to indicate this, but "SigViewer" doesn't gracefully handle this.
		So, there is no apparent choice but to just mask them out.
		*/
		val &= 0xFFF;

		/* EasyECG conveys data as a 12-bit unsigned value (0:4095) */
		sample = (float)val - 2048.0;
		/* apply the prescribed scalar to convert to nanovolts */
		sample *= (float)ctx->amplitude;
		/* scale nanovolts to millivolts */
		sample /= 1000000.0;
		fwrite(&sample, 1, sizeof(sample), output);
	}
}

int main(int argc, char *argv[])
{
	FILE *input, *output = NULL, *readme;
	long file_size;
	struct easyecg_context ctx;
	struct write_gdf_parameters gdf_parms;
#if defined(_MSC_VER) || defined(__MINGW32__)
	const char *subdir[4] = { "ECG_0\\", "ECG_1\\", "ECG_2\\", "ECG_3\\" };
#else
	const char *subdir[4] = { "ECG_0/", "ECG_1/", "ECG_2/", "ECG_3/" };
#endif
	int first_record, last_record, current_record;
	char filename[256];
	static unsigned char data[16384];

	if (argc < 2)
	{
		fprintf(stderr, "%s ecgfile.scp\n", argv[0]);
		return -1;
	}
	strncpy(filename, argv[1], 256);

			input = fopen(filename, "rb");

			if (NULL == input)
			{
				fprintf(stderr, "ERROR: unable to open file (%s)\n", filename);
				return -1;
			}

			file_size = fread(data, 1, sizeof(data), input);

			fclose(input);

			/* now that we've read the record into RAM, CRC-check and parse it */
			if (read_scp(data, file_size, &ctx))
			{
				fprintf(stderr, "ERROR: unable to parse EasyECG SCP file\n");
				return -1;
			}

			/* if this is the first record, then we need to open the output file and write the GDF header to it */

			/* if (current_record == first_record) */
			if (1) /* only 1 file */
			{
				snprintf(filename, sizeof(filename), "%04d%02d%02d-%02d%02d%02d.GDF", ctx.date.year, ctx.date.month, ctx.date.day, ctx.time.hour, ctx.time.minute, ctx.time.second);

				output = fopen(filename, "wb");

				if (!output)
				{
					fprintf(stderr, "ERROR: unable to open output file (%s)\n", filename);
					return -1;
				}

				fprintf(stderr, "writing %s\n", filename);

				gdf_parms.data_points = (ctx.datalen / 2) /* * (1 + last_record - first_record) */;
				gdf_parms.sample_rate_hz = 150;
				gdf_parms.data_type = GDFTYP_float32;
				gdf_parms.max = 2047.0 * ctx.amplitude / 1000000.0;
				gdf_parms.min = -2048.0 * ctx.amplitude / 1000000.0;

				write_gdf_header(output, &gdf_parms);
			}

			/* for every record, we write the data */
			write_data(output, &ctx);

	/* we're done, so close the two open files */
	/* fclose(readme); */
	fclose(output);

	return 0;
}

