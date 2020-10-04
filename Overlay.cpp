#include "Overlay.hpp"
#include "Timer.hpp"

#include <stdexcept>

#include <iostream>

Overlay::Overlay()
{
    FT_Error error;

	error = FT_Init_FreeType( &library_ );

	if (error) {
        throw std::runtime_error("FT_Init_FreeType error!");
	}
} 

void Overlay::Draw(const glm::uvec2& drawable_size)
{
    for (auto& p : texts_) {
        p.second.Draw(drawable_size);
    }
}

 void Overlay::AddText(const std::string& name, const char* content, const glm::vec2& anchor, const char* font_path, FT_F26Dot6 size, const glm::u8vec4& color)
 {
     texts_.erase(name);
     auto it = texts_.insert({name, {library_, font_path}}).first;
     (*it).second.SetText(content, size, color, anchor);
 }

void Overlay::AddText(const std::string& name, const char* content, const glm::vec2& anchor, const char* font_path, FT_F26Dot6 size, long long start_time, const glm::u8vec4& color)
{
    TimerManager::Instance().AddTimer(start_time, [this, name, content, anchor, color, font_path, size]() {
        texts_.erase(name);
        auto it = texts_.insert({name, {library_, font_path}}).first;
        (*it).second.SetText(content, size, color, anchor);
    });
}

void Overlay::AddText(const std::string& name, const char* content, const glm::vec2& anchor, const char* font_path, FT_F26Dot6 size, long long start_time, long long end_time, const glm::u8vec4& color)
{
    AddText(name, content, anchor, font_path, size, start_time, color);
    TimerManager::Instance().AddTimer(end_time, [this, name, content, anchor, color, font_path, size]() {
        texts_.erase(name);
    });
}
