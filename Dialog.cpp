#include "Dialog.hpp"
#include "data_path.hpp"

#include <fstream>
#include <stdexcept>
#include <sstream>

#include <iostream>

const std::unordered_map<std::string, glm::u8vec4> kFontColors {
    {"white", {0xff, 0xff, 0xff, 0xff}},
    {"black", {0x00, 0x00, 0x00, 0xff}},
    {"red", {0xff, 0x33, 0x00, 0xff}},
    {"green", {0x66, 0xff, 0x33, 0xff}},
    {"blue", {0xff, 0x99, 0xff, 0xff}},
    {"pink", {0xff, 0x99, 0xff, 0xff}},
    {"yellow", {0xff, 0xff, 0x99, 0xff}},
    {"transparent", {0x00, 0x00, 0x00, 0x00}}
};

DialogSystem* dialog_system { nullptr };

Load<void> load_dialog_system(LoadTagDefault, []() {
	dialog_system = new DialogSystem(data_path("MarryPrincess.dialogs"));
});

Dialog::Dialog(const char* font_path, const glm::u8vec4& background_color, const glm::vec4& bounding_box, FT_F26Dot6 font_size, const glm::u8vec4& font_color) :
font_size_(font_size),
font_color_(font_color)
{
    FT_Error error;

    error = FT_New_Face(DrawFont::library_, font_path, 0, &face_);

	if (error == FT_Err_Unknown_File_Format) {
        throw std::runtime_error("The font file could be opened and read, but it appears that its font format is unsupported!");
	} else if (error) {
        throw std::runtime_error("The font file could not be opened or read, or that it is broken!");
	}

	if (!face_) {
        throw std::runtime_error("Wrong font!");
	}

    texture2d_program->SetBox(bounding_box_drawable_, bounding_box, background_color);
}

void Dialog::AddText(const char* text, const glm::vec2& anchor)
{
    DrawFont* new_text = new DrawFont(face_);
    new_text->SetColor(font_color_);
    new_text->SetPos(anchor);
    new_text->SetText(text, font_size_);
    texts_.emplace_back(new_text);
}

void Dialog::Draw(const glm::uvec2& drawable_size)
{
    if (!visible_)
        return;

    texture2d_program->DrawBox(bounding_box_drawable_);

    for (auto text : texts_) {
        text->Draw(drawable_size);
    }
}

MenuDialog::MenuDialog(const char* font_path, const glm::u8vec4& background_color, const glm::vec4& bounding_box, FT_F26Dot6 font_size, const glm::u8vec4& font_color, 
    int max_choice, const glm::u8vec4& current_choice_color, int current_choice) :
Dialog(font_path, background_color, bounding_box, font_size, font_color),
max_choice_(max_choice),
current_choice_color_(current_choice_color),
current_choice_(current_choice)
{
    assert(max_choice_ > 0 && 0 <= current_choice_ && current_choice_ <= max_choice_);
}

void MenuDialog::NextChoice()
{
    if (current_choice_ < max_choice_) {
        texts_[current_choice_]->SetColor(font_color_);
        ++current_choice_;
    }
}

void MenuDialog::PreviousChoice()
{
    if (current_choice_ > 0) {
        texts_[current_choice_]->SetColor(font_color_);
        --current_choice_;
    }
}

void MenuDialog::Draw(const glm::uvec2& drawable_size)
{
    texts_[current_choice_]->SetColor(current_choice_color_);
    Dialog::Draw(drawable_size);
}

DialogSystem::DialogSystem(const std::string& file_path)
{
    std::ifstream f(file_path);

    std::cout << file_path << std::endl;

    if (!f.is_open()) {
        throw std::runtime_error("Cannot open dialogs file!");
    }

    while (!f.eof()) {
        std::string type, label;
        f >> type >> label;

        std::string background_color;
        glm::vec4 bounding_box;
        FT_F26Dot6 font_size;
        std::string font_color;
        std::string font_path;
        int dialog_num;
        
        if (type == "dialog" || type == "menu") {
            f >> font_path >> background_color;
            
            for (int i = 0; i < 4; ++i) {
                f >> bounding_box[i];
            }

            f >> font_size >> font_color >> dialog_num;
            
            Dialog* dialog;
            if (type == "menu") {
                int current_choice;
                std::string current_choice_color;
                f >> current_choice >> current_choice_color;
                
                dialog = new MenuDialog(data_path(font_path).c_str(), kFontColors.at(background_color), bounding_box, font_size, kFontColors.at(font_color),
                 dialog_num - 1, kFontColors.at(current_choice_color), current_choice);

                f.get();
                while (dialog_num--) {
                    std::string line;
                    std::getline(f, line);
                    std::istringstream iss(line);
                    glm::vec2 anchor;
                    iss >> anchor[0] >> anchor[1];
                    iss.get();
                    getline(iss, line);
                    dialog->AddText(line.c_str(), anchor);
                }
            } else {
                dialog = new Dialog(data_path(font_path).c_str(), kFontColors.at(background_color), bounding_box, font_size, kFontColors.at(font_color));
                
                f.get();
                while (dialog_num--) {
                    std::string line;
                    std::getline(f, line);
                    std::istringstream iss(line);
                    glm::vec2 anchor;
                    iss >> anchor[0] >> anchor[1];
                    iss.get();
                    getline(iss, line);
                    dialog->AddText(line.c_str(), anchor);
                }
            }

            dialogs_[label] = dialog;
        } else {
            throw std::runtime_error("Wrong dialog type!");
        }
    }
}

DialogSystem::~DialogSystem()
{
    for (auto& [label, dialog] : dialogs_) {
        delete dialog;
    }
}

void DialogSystem::CloseAllDialogs()
{
    for (auto& [label, dialog] : dialogs_) {
        dialog->SetVisibility(false);
    }
}

void DialogSystem::CloseDialogs(const std::vector<std::string>& labels)
{
    for (const auto& label : labels) {
        auto it = dialogs_.find(label);
        if (it != dialogs_.end()) {
            (*it).second->SetVisibility(false);
        }
    }
}

void DialogSystem::ShowDialogs(const std::vector<std::string>& labels)
{
    for (const auto& label : labels) {
        auto it = dialogs_.find(label);
        if (it != dialogs_.end()) {
            (*it).second->SetVisibility(true);
        }
    }
}

void DialogSystem::ShowDialogs(const std::string& file_path)
{
    std::ifstream f(file_path);

    if (!f.is_open()) {
        throw std::runtime_error("Cannot open showdialog file!");
    }

    std::string label;
    while (f >> label) {
        auto it = dialogs_.find(label);
        if (it != dialogs_.end()) {
            (*it).second->SetVisibility(true);
        }
    }
}

void DialogSystem::Draw(const glm::uvec2& drawable_size) const
{
    for (auto& [label, dialog] : dialogs_) {
        dialog->Draw(drawable_size);
    }
}

Dialog::~Dialog()
{ 
    FT_Done_Face(face_); 
    for (DrawFont* text : texts_) {
        delete text;
    }
    bounding_box_drawable_.Clear();
}
