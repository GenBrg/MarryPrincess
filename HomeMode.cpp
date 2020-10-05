#include "HomeMode.hpp"
#include "Player.hpp"
#include "data_path.hpp"

#include <iostream>

constexpr const char * const kFontPath { "dist\\Inkfree.ttf" };

HomeMode::HomeMode()
{
	Initialize();
}

HomeMode::~HomeMode() 
{
	
}

bool HomeMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) 
{
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_w) {
			if (current_menu_) {
				current_menu_->PreviousChoice();
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			if (current_menu_) {
				current_menu_->NextChoice();
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_RETURN) {
			if (current_menu_) {
				int current_choice = current_menu_->GetCurrentChoice();
				switch (state_) {
					case State::HOME:
						enum class HomeChoice : uint8_t {
							SLEEP = 0,
							VISIT_PRINCESS,
							BUY,
							SELL,
							ADVENTURE
						};
						switch (static_cast<HomeChoice>(current_choice)) {
							case HomeChoice::SLEEP:
								Player::Instance().Sleep();
							break;
							case HomeChoice::VISIT_PRINCESS:
								state_ = State::PRINCESS;
							break;
							case HomeChoice::BUY:
								dialog_system->ShowDialogs(std::vector<std::string>{"buy"});
								current_menu_ = dialog_system->GetDialog("buy");
							break;
							case HomeChoice::SELL:
								dialog_system->ShowDialogs(std::vector<std::string>{"sell"});
								current_menu_ = dialog_system->GetDialog("sell");
							break;
							case HomeChoice::ADVENTURE:

							break;
							default:
							throw std::runtime_error("Unknown HomeChoice!");
						}
					break;
					case State::PRINCESS:

					break;
					case State::BUY:
						state_ = State::HOME;
					break;
					case State::SELL:
						state_ = State::HOME;
					break;
					default:
					throw std::runtime_error("Unknown State!");
				}
			}
			return true;
		}
	}

	return false;
}

void HomeMode::update(float elapsed) 
{
	
}

void HomeMode::draw(glm::uvec2 const &drawable_size) 
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.4f, 0.9f, 0.9f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);

		dialog_system->Draw(drawable_size);
	}
}

void HomeMode::Initialize()
{
	dialog_system->CloseAllDialogs();
	dialog_system->ShowDialogs(data_path("Home.showdialog"));
	state_ = State::HOME;
}
