#pragma once

#include "DrawFont.hpp"
#include "Load.hpp"

#include <vector>
#include <unordered_map>
#include <string>

class Dialog {
private:
    std::vector<DrawFont> texts_;
    bool visible_ { true };
    glm::u8vec4 background_color_;
    glm::vec4 bounding_box_;
    FT_F26Dot6 font_size_;
    glm::u8vec4 font_color_;

    FT_Face face_;

public:
    Dialog(const char* font_path, const glm::u8vec4& background_color, const glm::vec4& bounding_box, FT_F26Dot6 font_size, const glm::u8vec4& font_color);
    virtual ~Dialog() { FT_Done_Face(face_); }
    void AddText(const char* text, const glm::vec2& anchor);
    void SetVisibility(bool visible) { visible_ = visible; }
    void Draw(const glm::uvec2& drawable_size);
};

class MenuDialog : public Dialog {
private:
    int current_choice_;
    int max_choice_;
    glm::u8vec4 current_choice_color_;

public:
    MenuDialog(const char* font_path, const glm::u8vec4& background_color, const glm::vec4& bounding_box, FT_F26Dot6 font_size, const glm::u8vec4& font_color, 
    int max_choice, const glm::u8vec4& current_choice_color, int current_choice);

    void NextChoice();
    void PreviousChoice();
    int GetCurrentChoice() const { return current_choice_; }
};

class DialogSystem {
private:
    std::unordered_map<std::string, Dialog*> dialogs_;

public:
    DialogSystem(const std::string& file_path);
    ~DialogSystem();

    void CloseAllDialogs();
    void CloseDialogs(const std::vector<std::string>& labels);

    void ShowDialogs(const std::vector<std::string>& labels);
    void ShowDialogs(const std::string& file_path);

    void Draw(const glm::uvec2& drawable_size) const;
};

extern Load<DialogSystem> dialog_system;
