#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define FROM_CHAR 32
#define TO_CHAR 127

void determine_bitmap_sizes(FT_Face face, size_t *width, size_t *height, size_t *baseline)
{
	// Iterate over all desired glyphs
	// find the maximum ascender height, by checking the glyphs bitmap_top value
	// find the maximum descender depth, by checking bitmap_top - height
	// height of all bitmaps has to be max ascender + max descender (+1? Is baseline included in asc or desc? or none?)

	// width of all bitmaps has to be the max width of any glyph
	// TODO: align glyphs using bitmap_left

	FT_Int max_ascender = 0;
	FT_Int max_descender = 0;
	FT_UInt max_width = 0;

	for (unsigned char c = FROM_CHAR; c < TO_CHAR; ++c) {	
		int glyph_index = FT_Get_Char_Index(face, c);

		if (FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER)) {
			fprintf(stderr, "There was a problem loading the glyph for A\n");
			continue;
		}

		if (face->glyph->bitmap_top > max_ascender) {
			max_ascender = face->glyph->bitmap_top;
		}
		if (((FT_Int)face->glyph->bitmap.rows - face->glyph->bitmap_top) > max_descender) {
			max_descender = (face->glyph->bitmap.rows - face->glyph->bitmap_top);
		}
		if (face->glyph->bitmap.width > max_width) {
			max_width = face->glyph->bitmap.width;
		}
	}

	*height = (size_t) (max_ascender + max_descender);
	*width = (size_t) max_width;
	*baseline = (size_t) max_ascender;
}

void write_byte_array_code_for_char_to_file(FILE *out, FT_Face face, const unsigned char c, const size_t width, const size_t height, const size_t baseline)
{
	int glyph_index = FT_Get_Char_Index(face, c);

	if(FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER)){
		fprintf(stderr, "There was a problem loading the glyph for %c\n", c);
		// TODO: return a fallback, like missing character box
		return;
	}

	// work out how many lines to skip at start
	int skip_lines = (int) baseline - face->glyph->bitmap_top;
	if(skip_lines < 0) skip_lines = 0;

	fprintf(out, "    [%d] = { // \'%c\'\n", c, c);
	FT_Bitmap *bitmap = &face->glyph->bitmap;
	for (size_t y = 0; y < height; ++y) {
		fprintf(out, "        {");
		if (y < (unsigned int) skip_lines || y >= (unsigned int) (skip_lines + bitmap->rows)) {
			for (size_t x = 0; x < width; ++x) {
				fprintf(out, "0, ");
			}
		} else {
			for (size_t x = 0; x < width; ++x) {
				if (x >= bitmap->width) {
					fprintf(out, "0, ");
					continue;
				}
				fprintf(out, "%c, ", bitmap->buffer[bitmap->width * (y - skip_lines) + x] == 0 ? '0' : '1');
			}
		}
		fprintf(out, "},\n");
	}
	fprintf(out, "    },\n");
}

int generate_c_file_from_ttf(const char *input_file_path, const char *output_file_path, const char *name)
{
	FT_Library library;
    FT_Face face;

	int error = 0;

	error = FT_Init_FreeType(&library);
	if (error) {
		fprintf(stderr, "ERROR: there was a problem initializing the FreeType library\n");
		return 1;
	}

	// TODO: allow user to choose a face_index other than 0
	// TODO: check for available face_indices by setting face_index to -1 and checking face->num_faces
	error = FT_New_Face(library, input_file_path, 0, &face);
	if (error == FT_Err_Unknown_File_Format) {
		fprintf(stderr, "ERROR: font file %s: format is not supported\n", input_file_path);
		return 1;
	} else if(error || face == NULL){
		fprintf(stderr, "ERROR: font file %s could not be read\n", input_file_path);
		return 1;
	}

	// TODO: let user specify a resolution and font size
	error = FT_Set_Char_Size(face, 0, 16*64, 72, 72);

	size_t width, height, baseline;
	determine_bitmap_sizes(face, &width, &height, &baseline);

    FILE *out = NULL;

	if (output_file_path) {
		out = fopen(output_file_path, "wb");
		if (out == NULL) {
			fprintf(stderr, "ERROR: could not write to file `%s`: %s\n", output_file_path, strerror(errno));
			return -1;
		}
	} else {
		out = stdout;
	}

	fprintf(out, "#include \"olive.c\"\n\n");
	fprintf(out, "static char %s_glyphs[%zu][%zu][%zu] = {\n", name, TO_CHAR, height, width);

	for (unsigned char c = FROM_CHAR; c < TO_CHAR; ++c) {
		write_byte_array_code_for_char_to_file(out, face, c, width, height, baseline);
	}

	fprintf(out, "};\n");

	// static Olivec_Font olivec_default_font = {
	// 	.glyphs = &olivec_default_glyphs[0][0][0],
	// 	.width = OLIVEC_DEFAULT_FONT_WIDTH,
	// 	.height = OLIVEC_DEFAULT_FONT_HEIGHT,
	// };

	fprintf(out, "\nstatic Olivec_Font %s = {\n", name);
	fprintf(out, "    .glyphs = &%s_glyphs[0][0][0],\n", name);
	fprintf(out, "    .width = %zu,\n", width);
	fprintf(out, "    .height = %zu,\n", height);
	fprintf(out, "};\n");

	return 0;
}

const char *shift(int *argc, char ***argv)
{
    assert(*argc > 0);
    const char *result = *argv[0];
    *argc -= 1;
    *argv += 1;
    return result;
}

void usage(FILE *out, const char *program_name)
{
    fprintf(out, "Usage: %s [OPTIONS] <input/file/path.ttf>\n", program_name);
    fprintf(out, "Options:\n");
    fprintf(out, "    -o <output/file/path.h>\n");
    fprintf(out, "    -n <name>\n");
    // TODO: fprintf(out, "    -i <font face index>\n");
}

int main(int argc, char *argv[])
{
	assert(argc > 0);
    const char *program_name = shift(&argc, &argv);
    const char *output_file_path = NULL;
    const char *input_file_path = NULL;
    const char *name = NULL;

    while (argc > 0) {
        const char *flag = shift(&argc, &argv);
        if (strcmp(flag, "-o") == 0) {
            if (argc <= 0) {
                usage(stderr, program_name);
                fprintf(stderr, "ERROR: no value is provided for flag %s\n", flag);
                return 1;
            }

            if (output_file_path != NULL) {
                usage(stderr, program_name);
                fprintf(stderr, "ERROR: %s was already provided\n", flag);
                return 1;
            }

            output_file_path = shift(&argc, &argv);
        } else if (strcmp(flag, "-n") == 0) {
            if (argc <= 0) {
                usage(stderr, program_name);
                fprintf(stderr, "ERROR: no value is provided for flag %s\n", flag);
                return 1;
            }

            if (name != NULL) {
                usage(stderr, program_name);
                fprintf(stderr, "ERROR: %s was already provided\n", flag);
                return 1;
            }

            name = shift(&argc, &argv);
        } else {
            if (input_file_path != NULL) {
                usage(stderr, program_name);
                fprintf(stderr, "ERROR: input file path was already provided\n");
                return 1;
            }
            input_file_path = flag;
        }
    }

    if (input_file_path == NULL) {
        usage(stderr, program_name);
        fprintf(stderr, "ERROR: expected input file path\n");
        return(1);
    }

    if (name == NULL) {
		// TODO: infer a fitting name from input path
        name = "font";
    } else {
        size_t n = strlen(name);
        if (n == 0) {
            fprintf(stderr, "ERROR: name cannot be empty\n");
            return 1;
        }

        if (isdigit(name[0])) {
            fprintf(stderr, "ERROR: name cannot start from a digit\n");
            return 1;
        }

        for (size_t i = 0; i < n; ++i) {
            if (!isalnum(name[i]) && name[i] != '_') {
                fprintf(stderr, "ERROR: name can only contains alphanumeric characters and underscores\n");
                return 1;
            }
        }
    }

	return generate_c_file_from_ttf(input_file_path, output_file_path, name);
}
