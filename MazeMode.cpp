#include "MazeMode.hpp"

#include "Player.hpp"
#include "data_path.hpp"

#include <iostream>
#include <stack>
#include <random>

static constexpr float kPlayerDrawableRadius { 0.02f };
static constexpr int kCircleVertexCount { 20 };
static constexpr glm::u8vec4 kPlayerColor { 0xff, 0x00, 0x00, 0xff };

MazeMode::MazeMode()
{
	Initialize();
}

MazeMode::~MazeMode()
{
	player_pos_drawable_.Clear();
	ClearRooms();
}

bool MazeMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size)
{
	if (evt.type == SDL_KEYDOWN)
	{
		if (evt.key.keysym.sym == SDLK_w)
		{
			
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_s)
		{
			
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_RETURN)
		{
			
			
			return true;
		}
	}

	return false;
}

void MazeMode::update(float elapsed)
{
	auto get_screen_pos = [&](const glm::uvec2& pos) {
		return glm::vec2(kMazeStartPos[0] + (pos[1] + 0.5f) * Room::kRoomSize, kMazeStartPos[1] - (pos[0] + 0.5f) * Room::kRoomSize);
	};

	texture2d_program->SetCircle(player_pos_drawable_, get_screen_pos(position_), kPlayerDrawableRadius, kCircleVertexCount, kPlayerColor);
}

void MazeMode::draw(glm::uvec2 const &drawable_size)
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

		// Draw maze
		for (auto& row : map_) {
			for (auto& room : row) {
				room.Draw();
			}
		}

		// Draw player position
		texture2d_program->DrawCircle(player_pos_drawable_);

		// Draw dialogs
		dialog_system->Draw(drawable_size);
	}
}

void MazeMode::Initialize()
{
	dialog_system->CloseAllDialogs();
	dialog_system->ShowDialogs(data_path("Maze.showdialog"));
	GenerateMaze();
	position_ = glm::uvec2(0, 0);
}

void MazeMode::ClearRooms()
{
	for (auto& row : map_) {
		for (auto& room : row) {
			room.Clear();
		}
	}
}

void MazeMode::GenerateMaze()
{
	ClearRooms();
	glm::uvec2 current_pos;
	std::stack<glm::uvec2> fringe;
	fringe.emplace(0, 0);
	// std::vector<std::vector<bool>> visited(kMazeHeight, std::vector<bool>(kMazeWidth, false));
	std::random_device rd;
	std::mt19937 mt{rd()};

	while (!fringe.empty()) {
		current_pos = fringe.top();
		auto& current_room = map_[current_pos.x][current_pos.y];
		current_room.flag_ |= Room::Flag::VISITED;

		std::vector<int> choices;
		for (int i = 0; i < 4; ++i) {
			const glm::uvec2& dir = direction[i];
			glm::uvec2 new_pos = current_pos + dir;
			if (new_pos.x < kMazeHeight && new_pos.x >= 0 && new_pos.y < kMazeWidth && new_pos.y >= 0) {
				if (!(map_[new_pos.x][new_pos.y].flag_ & Room::Flag::VISITED)) {
					choices.push_back(i);
				}
			}
		}

		int choice_num = static_cast<int>(choices.size());
		if (choice_num > 0) {
			int choice = choices[std::uniform_int_distribution<int>(0, choice_num - 1)(mt)];
			glm::uvec2 new_pos = current_pos + direction[choice];
			auto& new_room = map_[new_pos.x][new_pos.y];
			current_room.flag_ |= Room::kDirectionToFlag[choice];
			new_room.flag_ |= Room::kDirectionToFlag[(choice + 2) % 4];
			fringe.push(new_pos);
		} else {
			fringe.pop();
		}
	}

	// for (int i = 0; i < kMazeHeight; ++i) {
		
	// 	for (int j = 0; j < kMazeWidth; ++j) {
	// 		int connect = Room::Flag::CONNECT_UP;
	// 		for (int k = 0; k < 4; k++) {
	// 			if (map_[i][j].flag_ & connect) {
	// 				std::cout << "1";
	// 			} else {
	// 				std::cout << "0";
	// 			}
	// 			connect <<= 1;
	// 		}
	// 		std::cout << " ";
	// 	}
	// 	std::cout << std::endl;
	// }

	// Select treasure room, monster room, etc...

	for (int i = 0; i < kMazeHeight; ++i) {
		for (int j = 0; j < kMazeWidth; ++j) {
			Room& room = map_[i][j];
			texture2d_program->SetBox(room.room_drawable_,
			glm::vec4(
				kMazeStartPos[0] + j * Room::kRoomSize,
				kMazeStartPos[1] - i * Room::kRoomSize,
				kMazeStartPos[0] + (j + 1) * Room::kRoomSize,
				kMazeStartPos[1] - (i + 1) * Room::kRoomSize
			), 

			Room::kRoomColors[static_cast<int>(room.type_)]);

			// if (!(room.flag_ & Room::Flag::CONNECT_UP)) {
			// 	texture2d_program->SetBox(room.wall_drawables_[0],
			// 	glm::vec4(
			// 		kMazeStartPos[0] + j * Room::kRoomSize,
			// 		kMazeStartPos[1] - i * Room::kRoomSize,
			// 		kMazeStartPos[0] + (j + 1) * Room::kRoomSize,
			// 		kMazeStartPos[1] - (i * Room::kRoomSize + Room::kWallSize)
			// 	), 
			// 	Room::kWallColor);
			// }

			if (!(room.flag_ & Room::Flag::CONNECT_RIGHT)) {
				texture2d_program->SetBox(room.wall_drawables_[0],
				glm::vec4(
					kMazeStartPos[0] + (j + 1) * Room::kRoomSize - Room::kWallSize,
					kMazeStartPos[1] - i * Room::kRoomSize,
					kMazeStartPos[0] + (j + 1) * Room::kRoomSize,
					kMazeStartPos[1] - (i + 1) * Room::kRoomSize
				), 
				Room::kWallColor);
			}

			if (!(room.flag_ & Room::Flag::CONNECT_DOWN)) {
				texture2d_program->SetBox(room.wall_drawables_[1],
				glm::vec4(
					kMazeStartPos[0] + j * Room::kRoomSize,
					kMazeStartPos[1] - ((i + 1) * Room::kRoomSize - Room::kWallSize),
					kMazeStartPos[0] + (j + 1) * Room::kRoomSize,
					kMazeStartPos[1] - (i + 1) * Room::kRoomSize
				), 
				Room::kWallColor);
			}

			// if (!(room.flag_ & Room::Flag::CONNECT_LEFT)) {
			// 	texture2d_program->SetBox(room.wall_drawables_[3],
			// 	glm::vec4(
			// 		kMazeStartPos[0] + j * Room::kRoomSize,
			// 		kMazeStartPos[1] - i * Room::kRoomSize,
			// 		kMazeStartPos[0] + j * Room::kRoomSize + Room::kWallSize,
			// 		kMazeStartPos[1] - (i + 1) * Room::kRoomSize
			// 	), 
			// 	Room::kWallColor);
			// }
		}
	}
}

void MazeMode::Room::Draw()
{
	texture2d_program->DrawBox(room_drawable_);
	for (auto& wall_drawable : wall_drawables_) {
		texture2d_program->DrawBox(wall_drawable);
	}
}

void MazeMode::Room::Clear()
{
	type_ = Type::NORMAL;
	flag_ = 0;
	room_drawable_.Clear();
	for (auto& wall_drawable : wall_drawables_) {
		wall_drawable.Clear();
	}
}
