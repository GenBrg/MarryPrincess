#pragma once

#include "DrawFont.hpp "

#include <ft2build.h>
#include FT_FREETYPE_H

#include <unordered_map>
#include <string>

class Overlay {
private:
    std::unordered_map<std::string, DrawFont> texts_;
    FT_Library library_;

public:
    Overlay();
    void Draw(const glm::uvec2& drawable_size);
    void AddText(const std::string& name, const char* content, const glm::vec2& anchor, const char* font_path, FT_F26Dot6 size, const glm::u8vec4& color = glm::u8vec4(0xff, 0xff, 0xff, 0xff));
    void AddText(const std::string& name, const char* content, const glm::vec2& anchor, const char* font_path, FT_F26Dot6 size, long long start_time, const glm::u8vec4& color = glm::u8vec4(0xff, 0xff, 0xff, 0xff));
    void AddText(const std::string& name, const char* content, const glm::vec2& anchor, const char* font_path, FT_F26Dot6 size, long long start_time, long long end_time, const glm::u8vec4& color = glm::u8vec4(0xff, 0xff, 0xff, 0xff));
};
