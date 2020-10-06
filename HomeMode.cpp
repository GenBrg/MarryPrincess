#include "HomeMode.hpp"
#include "MazeMode.hpp"
#include "Player.hpp"
#include "data_path.hpp"
#include "EventLog.hpp"

#include <iostream>

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
						enum class HomeChoice : uint8_t {
							SLEEP = 0,
							MARRY_PRINCESS,
							ADVENTURE
						};
						switch (static_cast<HomeChoice>(current_choice)) {
							case HomeChoice::SLEEP:
								current_menu_->Choose();
								Player::Instance().Sleep();
							break;
							case HomeChoice::MARRY_PRINCESS:
								current_menu_->Choose();
								if (Player::Instance().MarryPrincess()) {

								}
							break;
							case HomeChoice::ADVENTURE:
								current_menu_->Choose();
								dialog_system->CloseAllDialogs();
								mazemode->Initialize();
								Mode::set_current(mazemode);
							break;
							default:
							throw std::runtime_error("Unknown HomeChoice!");
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

		EventLog::Instance().Draw(drawable_size);

		Player::Instance().DrawInfo(drawable_size);
	}
}

void HomeMode::Initialize()
{
	dialog_system->CloseAllDialogs();
	dialog_system->ShowDialogs(data_path("Home.showdialog"));
	// state_ = State::HOME;
	current_menu_ = dynamic_cast<MenuDialog*>(dialog_system->GetDialog("home_choice"));
}

std::shared_ptr<HomeMode> homemode;
