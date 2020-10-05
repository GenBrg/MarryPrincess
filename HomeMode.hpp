#include "Mode.hpp"
#include "Dialog.hpp"
#include "Player.hpp"

#include <glm/glm.hpp>

struct HomeMode : Mode {
	enum class State : uint8_t {
		HOME = 0,
		PRINCESS,
		BUY,
		SELL
	} state_ { State::HOME };
	MenuDialog* current_menu_ { nullptr };

	HomeMode();
	virtual ~HomeMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	void Initialize();
};
