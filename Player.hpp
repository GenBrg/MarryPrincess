#pragma once
#include "Dialog.hpp"

#include <glm/glm.hpp>

class Player
{
private:
	int hp_{100};
	int max_hp_{100};

	int mp_{100};
	int max_mp_{100};

	int attack_{10};
	// int intelligence_{ 10 };
	int defense_{10};

	int level_{1};
	int money_{0};
	int experience_{0};
	int level_up_experience_{100};

	bool married_{false};

	Dialog player_info_dialog_;
	Player();

	void LevelUp();
	void SetHP(int hp, int max_hp);
	void SetMP(int mp, int max_mp);
	void SetAttack(int attack);
	void SetDefense(int defense);
	void SetLevel(int level);
	void SetMoney(int money);
	void SetExp(int experience, int level_up_experience);
	void Die();

public:
	static Player &Instance()
	{
		static Player player;
		return player;
	}

	void DrawInfo(const glm::uvec2 &drawable_size);
	void Sleep();

	bool MarryPrincess();
	int Attack(int defense);
	bool ApplyDamage(int attack);
	void GainExperience(int exp);
	void GainMoney(int money);
	int GetLevel() const { return level_; }
};
