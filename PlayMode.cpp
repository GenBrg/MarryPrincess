#include "PlayMode.hpp"
#include "Dialog.hpp"

#include <iostream>

constexpr const char * const kFontPath { "dist\\Inkfree.ttf" };

PlayMode::PlayMode()
{

}

PlayMode::~PlayMode() 
{
	
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) 
{
	return true;
}

void PlayMode::update(float elapsed) 
{
	
}

void PlayMode::draw(glm::uvec2 const &drawable_size) 
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
