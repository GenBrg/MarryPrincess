#include "PlayMode.hpp"

#include <iostream>

constexpr const char * const kFontPath { "dist\\Inkfree.ttf" };

PlayMode::PlayMode()
{
	overlay.AddText("test", "Hello World!", {0.0f, 0.0f}, kFontPath, 2000, glm::u8vec4(0x12, 0x34, 0x56, 0x78));
	overlay.AddText("test2", "AAABBBCCC", {-1.0f, -1.0f}, kFontPath, 2000, glm::u8vec4(0x00, 0xff, 0x00, 0xff));
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

		overlay.Draw(drawable_size);
	}
}
