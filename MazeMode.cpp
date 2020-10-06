#include "MazeMode.hpp"
#include "HomeMode.hpp"
#include "EventLog.hpp"

#include "Player.hpp"
#include "data_path.hpp"

#include <iostream>
#include <stack>
#include <random>
#include <set>
#include <sstream>

std::shared_ptr<MazeMode> mazemode;

static constexpr float kPlayerDrawableRadius{0.02f};
static constexpr int kCircleVertexCount{20};
static constexpr glm::u8vec4 kPlayerColor{0xbb, 0xb0, 0xe8, 0xff};
static constexpr glm::u8vec4 kBackgroundColor{0x0f, 0x02, 0xff, 0xff};

MazeMode::MazeMode()
{
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
		Room &current_room = map_[position_.x][position_.y];
		if (evt.key.keysym.sym == SDLK_w)
		{
			switch (current_room.type_)
			{
			case Room::Type::EXIT:
			case Room::Type::NORMAL:
			case Room::Type::TREASURE:
				EnterRoom(0);
				break;
			case Room::Type::MONSTER:
				dynamic_cast<MenuDialog *>(dialog_system->GetDialog("maze_monster"))->PreviousChoice();
				break;
			default:;
			}
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_d)
		{
			if (current_room.type_ != Room::Type::MONSTER)
			{
				EnterRoom(1);
			}
		}
		else if (evt.key.keysym.sym == SDLK_a)
		{
			if (current_room.type_ != Room::Type::MONSTER)
			{
				EnterRoom(3);
			}
		}
		else if (evt.key.keysym.sym == SDLK_s)
		{
			switch (current_room.type_)
			{
			case Room::Type::EXIT:
			case Room::Type::NORMAL:
			case Room::Type::TREASURE:
				EnterRoom(2);
				break;
			case Room::Type::MONSTER:
				dynamic_cast<MenuDialog *>(dialog_system->GetDialog("maze_monster"))->NextChoice();
				break;
			default:;
			}
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_RETURN)
		{
			switch (current_room.type_)
			{
			case Room::Type::EXIT:
			{
				static int adventure_time = 0;
				++adventure_time;
				EventLog::Instance().LogEvent("Adventure succeeded!");
				Player::Instance().GainMoney(adventure_time * 100);
				Player::Instance().GainExperience(adventure_time * 50);
				Exit();
				break;
			}
			case Room::Type::MONSTER:
			{
				int current_choice = dynamic_cast<MenuDialog *>(dialog_system->GetDialog("maze_monster"))->GetCurrentChoice();
				FightMonster(current_choice);
				break;
			}
			case Room::Type::TREASURE:
				PickupTreasure();
				break;
			default:;
			}
			return true;
		}
	}

	return false;
}

void MazeMode::update(float elapsed)
{
	auto get_screen_pos = [&](const glm::uvec2 &pos) {
		return glm::vec2(kMazeStartPos[0] + (pos[1] + 0.5f) * Room::kRoomSize, kMazeStartPos[1] - (pos[0] + 0.5f) * Room::kRoomSize);
	};

	texture2d_program->SetCircle(player_pos_drawable_, get_screen_pos(position_), kPlayerDrawableRadius, kCircleVertexCount, kPlayerColor);
}

void MazeMode::draw(glm::uvec2 const &drawable_size)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.1f, 0.1f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);

		// Draw maze
		for (auto &row : map_)
		{
			for (auto &room : row)
			{
				room.Draw();
			}
		}

		// Draw player position
		texture2d_program->DrawCircle(player_pos_drawable_);

		// Draw dialogs
		dialog_system->Draw(drawable_size);

		// Draw player info
		Player::Instance().DrawInfo(drawable_size);

		// Draw monster info
		if (monster_) {
			monster_->info_dialog_.Draw(drawable_size);
		}

		// Draw event info
		EventLog::Instance().Draw(drawable_size);
	}
}

void MazeMode::Initialize()
{
	GenerateMaze();
	position_ = glm::uvec2(0, 0);
}

void MazeMode::ClearRooms()
{
	for (auto &row : map_)
	{
		for (auto &room : row)
		{
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

	while (!fringe.empty())
	{
		current_pos = fringe.top();
		auto &current_room = map_[current_pos.x][current_pos.y];
		current_room.flag_ |= Room::Flag::VISITED;

		std::vector<int> choices;
		for (int i = 0; i < 4; ++i)
		{
			const glm::uvec2 &dir = kDirection[i];
			glm::uvec2 new_pos = current_pos + dir;
			if (new_pos.x < kMazeHeight && new_pos.x >= 0 && new_pos.y < kMazeWidth && new_pos.y >= 0)
			{
				if (!(map_[new_pos.x][new_pos.y].flag_ & Room::Flag::VISITED))
				{
					choices.push_back(i);
				}
			}
		}

		int choice_num = static_cast<int>(choices.size());
		if (choice_num > 0)
		{
			int choice = choices[std::uniform_int_distribution<int>(0, choice_num - 1)(mt)];
			glm::uvec2 new_pos = current_pos + kDirection[choice];
			auto &new_room = map_[new_pos.x][new_pos.y];
			current_room.flag_ |= Room::kDirectionToFlag[choice];
			new_room.flag_ |= Room::kDirectionToFlag[(choice + 2) % 4];
			fringe.push(new_pos);
		}
		else
		{
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
	std::uniform_int_distribution<int> special_room_dist(1, kMazeHeight * kMazeWidth - 1);
	std::set<int> room_set;

	auto set_room_type = [&](int room_num, Room::Type type) {
		map_[room_num / kMazeWidth][room_num % kMazeWidth].type_ = type;
	};

	auto generate_room = [&](Room::Type type) {
		while (true)
		{
			int room_num = special_room_dist(mt);
			if (room_set.count(room_num) == 0)
			{
				room_set.insert(room_num);
				set_room_type(room_num, type);
				break;
			}
		}
	};

	for (int i = 0; i < kMonsterNum; ++i)
	{
		generate_room(Room::Type::MONSTER);
	}

	generate_room(Room::Type::EXIT);
	generate_room(Room::Type::TREASURE);

	for (int i = 0; i < kMazeHeight; ++i)
	{
		for (int j = 0; j < kMazeWidth; ++j)
		{
			Room &room = map_[i][j];
			UpdateRoomColor(glm::uvec2(i, j));

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

			// std::cout << i << " " << j << std::endl;
			if (!(room.flag_ & Room::Flag::CONNECT_RIGHT))
			{
				// std::cout << "right!" << std::endl;
				texture2d_program->SetBox(room.wall_drawables_[0],
										  glm::vec4(
											  kMazeStartPos[0] + (j + 1) * Room::kRoomSize - Room::kWallSize,
											  kMazeStartPos[1] - i * Room::kRoomSize,
											  kMazeStartPos[0] + (j + 1) * Room::kRoomSize,
											  kMazeStartPos[1] - (i + 1) * Room::kRoomSize),
										  Room::kWallColor);
			}

			if (!(room.flag_ & Room::Flag::CONNECT_DOWN))
			{
				// std::cout << "down!" << std::endl;
				texture2d_program->SetBox(room.wall_drawables_[1],
										  glm::vec4(
											  kMazeStartPos[0] + j * Room::kRoomSize,
											  kMazeStartPos[1] - ((i + 1) * Room::kRoomSize - Room::kWallSize),
											  kMazeStartPos[0] + (j + 1) * Room::kRoomSize,
											  kMazeStartPos[1] - (i + 1) * Room::kRoomSize),
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
	for (auto &wall_drawable : wall_drawables_)
	{
		texture2d_program->DrawBox(wall_drawable);
	}
}

void MazeMode::Room::Clear()
{
	type_ = Type::NORMAL;
	flag_ = 0;
	room_drawable_.Clear();
	for (auto &wall_drawable : wall_drawables_)
	{
		wall_drawable.Clear();
	}
}

void MazeMode::EnterRoom(int direction)
{
	glm::uvec2 new_pos = position_ + kDirection[direction];
	if (new_pos.x < kMazeHeight && new_pos.x >= 0 && new_pos.y < kMazeWidth && new_pos.y >= 0)
	{
		if (map_[position_.x][position_.y].flag_ & (Room::Flag::CONNECT_UP << direction))
		{
			position_ = new_pos;
			// Exit room clean up
			dialog_system->CloseAllDialogs();
			// Enter room
			switch (map_[new_pos.x][new_pos.y].type_)
			{
			case Room::Type::EXIT:
				dialog_system->ShowDialogs(std::vector<std::string>{"maze_exit"});
				break;
			case Room::Type::NORMAL:
				break;
			case Room::Type::TREASURE:
				dialog_system->ShowDialogs(std::vector<std::string>{"maze_treasure"});
				break;
			case Room::Type::MONSTER:
				dialog_system->ShowDialogs(std::vector<std::string>{"maze_monster"});
				monster_ = new Monster(Player::Instance().GetLevel());
				break;
			default:;
			}
		}
	}
}

void MazeMode::FightMonster(int choice)
{
	std::random_device rd;
	std::mt19937 mt{rd()};
	auto fight_succeed = [&]() {
		int i = position_.x;
		int j = position_.y;
		Room &current_room = map_[i][j];
		current_room.type_ = Room::Type::NORMAL;
		dialog_system->CloseAllDialogs();
		UpdateRoomColor(position_);
		delete monster_;
		monster_ = nullptr;
	};

	enum Choice
	{
		ATTACK = 0,
		ESCAPE
	};

	switch (choice)
	{
	case ATTACK: {
		int damage = Player::Instance().Attack(monster_->defense_);
		if (monster_->ApplyDamage(damage)) {
			int money = monster_->level_ * 50;
			int exp = monster_->level_ * 20;
			Player::Instance().GainMoney(money);
			Player::Instance().GainExperience(exp);
			fight_succeed();
			return;
		}
		break;
	}
	case ESCAPE:
		if (std::uniform_int_distribution<int>(1, 10)(mt) <= 3)
		{
			EventLog::Instance().LogEvent("Escape succeed!");
			fight_succeed();
			return;
		}
		EventLog::Instance().LogEvent("Escape failed!");
		break;
	default:
		throw std::runtime_error("Unknown fight monster option!");
	}

	// Monster attack you
	if (Player::Instance().ApplyDamage(monster_->attack_))
	{
		delete monster_;
		monster_ = nullptr;
		Exit();
	}
}

void MazeMode::UpdateRoomColor(const glm::uvec2 &pos)
{
	int i = pos.x;
	int j = pos.y;
	Room &current_room = map_[i][j];
	texture2d_program->SetBox(current_room.room_drawable_,
							  glm::vec4(
								  kMazeStartPos[0] + j * Room::kRoomSize,
								  kMazeStartPos[1] - i * Room::kRoomSize,
								  kMazeStartPos[0] + (j + 1) * Room::kRoomSize,
								  kMazeStartPos[1] - (i + 1) * Room::kRoomSize),

							  Room::kRoomColors[static_cast<int>(current_room.type_)]);
}

void MazeMode::Exit()
{
	dialog_system->CloseAllDialogs();
	homemode->Initialize();
	Mode::set_current(homemode);
}

void MazeMode::PickupTreasure()
{
	std::random_device rd;
	std::mt19937 mt{rd()};

	map_[position_.x][position_.y].type_ = Room::Type::NORMAL;
	UpdateRoomColor(position_);
	dialog_system->CloseAllDialogs();

	int treasure = std::uniform_int_distribution<int>(0, 1)(mt);

	enum Treasure : uint8_t
	{
		MONEY = 0,
		EXP
	};

	int level = Player::Instance().GetLevel();
	switch (treasure)
	{
	case MONEY:
	{
		EventLog::Instance().LogEvent("Found money!");
		int money = (std::uniform_int_distribution<int>(50, 500)(mt) * level) / 2;
		Player::Instance().GainMoney(money);
		break;
	}
	case EXP:
	{
		EventLog::Instance().LogEvent("Found exp!");
		int exp = (std::uniform_int_distribution<int>(50, 500)(mt) * level) / 10;
		Player::Instance().GainExperience(exp);
		break;
	}

	default:;
	}
}

MazeMode::Monster::Monster(int player_level) :
info_dialog_(data_path("ariblk.ttf").c_str(), kFontColors.at("black"), glm::vec4(-0.9f, 0.8f, -0.7f, 0.5f), kFontSize, kFontColors.at("white"))
{
	info_dialog_.SetVisibility(true);

    std::ostringstream oss;
    glm::vec2 anchor { -0.89f, 0.75f };
	info_dialog_.AddText("Monster", anchor);

	std::random_device rd;
	std::mt19937 mt { rd() };
	level_ = std::max(1, std::uniform_int_distribution<int>(-3, 3)(mt) + player_level);

	anchor[1] -= kFontLineSpace;
    oss.str("");
    oss << "Level: " << level_;
    info_dialog_.AddText(oss.str().c_str(), anchor);

	hp_ = std::max(1, static_cast<int>((std::uniform_real_distribution<float>(-2.0f, 2.0f)(mt) + 10.0f) * level_));
	anchor[1] -= kFontLineSpace;
    oss.str("");
    oss << "HP: " << hp_;
    info_dialog_.AddText(oss.str().c_str(), anchor);

	attack_ = std::max(1, 5 + level_ * 3 + std::uniform_int_distribution<int>(-5, 5)(mt));
	anchor[1] -= kFontLineSpace;
    oss.str("");
    oss << "Attack: " << attack_;
    info_dialog_.AddText(oss.str().c_str(), anchor);

	defense_ = std::max(1, 5 + level_ * 3 + std::uniform_int_distribution<int>(-5, 5)(mt));
	anchor[1] -= kFontLineSpace;
    oss.str("");
    oss << "Defense: " << defense_;
    info_dialog_.AddText(oss.str().c_str(), anchor);
}

bool MazeMode::Monster::ApplyDamage(int damage)
{
	std::ostringstream oss;
    oss << "Monster received " << damage << " damage!";
    EventLog::Instance().LogEvent(oss.str());

	hp_ -= damage;
	if (hp_ <= 0) {
		info_dialog_.SetVisibility(false);
		return true;
	}

	oss.str("");
    oss << "HP: " << hp_;
    info_dialog_.GetText(2)->SetText(oss.str().c_str(), kFontSize);

	return false;
}
