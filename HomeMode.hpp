#include "Mode.hpp"

#include "Overlay.hpp"

#include <glm/glm.hpp>

struct HomeMode : Mode {
	HomeMode();
	virtual ~HomeMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	Overlay overlay;
};
