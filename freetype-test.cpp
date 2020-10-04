
#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>

#include <iostream>
#include <iomanip>


//This file exists to check that programs that use freetype / harfbuzz link properly in this base code.
//You probably shouldn't be looking here to learn to use either library.

int main(int argc, char **argv) {
	FT_Library library;
	FT_Face face;

	FT_Error error;

	error = FT_Init_FreeType( &library );

	if (error) {
		std::cout << "FT_Init_FreeType error!" << std::endl;
		return 1;
	}

	error = FT_New_Face(library, "dist\\Inkfree.ttf", 0, &face);

	if (error == FT_Err_Unknown_File_Format) {
		std::cout << "The font file could be opened and read, but it appears that its font format is unsupported!" << std::endl;
		return 1;
	} else if (error) {
		std::cout << "The font file could not be opened or read, or that it is broken!" << std::endl;
		return 1;
	}

	if (!face) {
		std::cout << "Wrong font!" << std::endl;
		getchar();
	}

	

	FT_Set_Char_Size(face, 0, 2000, 0, 0);
	hb_font_t* font = hb_ft_font_create(face, nullptr);
	
	hb_buffer_t *buf = hb_buffer_create();
	hb_buffer_add_utf8(buf, "Apple", -1, 0, -1);
	hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
	hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
	hb_buffer_set_language(buf, hb_language_from_string("en", -1));
	hb_shape(font, buf, nullptr, 0);
	unsigned int glyph_count;
	hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
	hb_buffer_destroy(buf);

	double cursor_x = 0.0, cursor_y = 0.0;

	FT_GlyphSlot slot = face->glyph; 
	
	for (unsigned int i = 0; i < glyph_count; ++i) {
		auto glyphid = glyph_info[i].codepoint;
		auto x_offset = glyph_pos[i].x_offset / 64.0;
		auto y_offset = glyph_pos[i].y_offset / 64.0;
		auto x_advance = glyph_pos[i].x_advance / 64.0;
		auto y_advance = glyph_pos[i].y_advance / 64.0;

		FT_UInt glyph_index = FT_Get_Char_Index(face, "Apple"[i]);

		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		if (error)
			continue;

		error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if (error)
			continue;
		
		printf("%d %f %f %d %d\n", glyphid, cursor_x + x_offset, cursor_y + y_offset, slot->bitmap_left, slot->bitmap_top);

		auto& bitmap = slot->bitmap;
		for (unsigned int row = 0; row < bitmap.rows; ++row) {
			int start_idx = row * bitmap.pitch;
			for (unsigned int col = 0; col < bitmap.width; ++col) {
				std::cout << std::setw(3) << (int)bitmap.buffer[start_idx + col] << " ";
			}
			std::cout << std::endl;
		}

		cursor_x += x_advance;
		cursor_y += y_advance;
	}

	
	 
	return 0;
}
