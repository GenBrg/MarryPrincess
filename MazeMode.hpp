#pragma once
#include "Mode.hpp"
#include "Dialog.hpp"
#include "Player.hpp"
#include "Texture2DProgram.hpp"

#include <glm/glm.hpp>
#include <vector>

enum class Direction : uint8_t
{
	UP = 0,
	RIGHT,
	DOWN,
	LEFT
};

inline constexpr glm::uvec2 kDirection[]{
	{-1, 0},
	{0, 1},
	{1, 0},
	{0, -1}};

struct MazeMode : Mode
{
	inline static constexpr int kMazeHeight{10};
	inline static constexpr int kMazeWidth{10};
	inline static constexpr int kMonsterNum{3};
	inline static constexpr glm::vec2 kMazeStartPos{-0.5f, 0.5f};

	// enum class State : uint8_t {
	// 	BATTLE = 0,
	// 	WALK,
	// 	TREASURE
	// } state_ { State::WALK };
	MenuDialog *current_menu_{nullptr};

	struct Monster
	{
		int level_;
		int hp_;
		int attack_;
		int defense_;
		int exp_;
	};

	struct Room
	{
		inline static constexpr float kRoomSize{0.1f};
		inline static constexpr float kWallSize{0.02f};
		inline static const glm::u8vec4 kRoomColors[]{
			{0x66, 0xff, 0x33, 0xff},
			{0xff, 0xff, 0xff, 0xff},
			{0xff, 0x33, 0x00, 0xff},
			{0xff, 0xff, 0x99, 0xff}};
		inline static const glm::u8vec4 kWallColor{0x00, 0x00, 0x00, 0xff};

		enum class Type : uint8_t
		{
			EXIT,
			NORMAL,
			MONSTER,
			TREASURE
		} type_;

		enum Flag : int
		{
			VISITED = 0x01,
			CONNECT_UP = 0x02,
			CONNECT_RIGHT = 0x04,
			CONNECT_DOWN = 0x08,
			CONNECT_LEFT = 0x10
		};

		int flag_;

		inline static constexpr Flag kDirectionToFlag[]{
			CONNECT_UP, CONNECT_RIGHT, CONNECT_DOWN, CONNECT_LEFT};

		MenuDialog *menu_;
		Texture2DProgram::BoxDrawable room_drawable_;
		Texture2DProgram::BoxDrawable wall_drawables_[2];

		Room() { Clear(); }
		Room(const Room &) = delete;

		void Draw();
		void Clear();
	};

	Room map_[kMazeHeight][kMazeWidth];
	glm::uvec2 position_;
	Texture2DProgram::CircleDrawable player_pos_drawable_;

	MazeMode();
	virtual ~MazeMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	void ClearRooms();
	void GenerateMaze();
	void Initialize();

	void EnterRoom(int direction);
	void PickupTreasure();
	void Exit();
	void FightMonster(int choice);
	void UpdateRoomColor(const glm::uvec2& pos);
};
