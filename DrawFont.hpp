#pragma once

#include "Scene.hpp"
#include "Texture2DProgram.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>

#include <vector>

class DrawFont {
private:
    FT_Library& library_;
    FT_Face face_;

    std::vector<GLuint> texture_ids_;

    unsigned int glyph_count_;
    hb_font_t* font_ { nullptr };
    hb_buffer_t* buf_ { nullptr };
    hb_glyph_info_t* glyph_info_ { nullptr };
    hb_glyph_position_t* glyph_pos_ { nullptr };
    glm::vec2 anchor_;
    std::string text_;
    glm::u8vec4 color_;

    float GetOpenGLPos(float pos, int drawable_size)
    {
        return (2 * pos) / drawable_size - 1;
    }

    float GetPixelPos(float pos, int drawable_size)
    {
        return (pos + 1) * drawable_size / 2.0f;
    }
public:
    static GLuint index_buffer_;

    DrawFont(FT_Library& library, const char* font_path);
    ~DrawFont();

    void Draw(const glm::uvec2& drawable_size);
    void SetText(const char* text, FT_F26Dot6 size, const glm::u8vec4& color, const glm::vec2& anchor);
    void ClearText();
};
