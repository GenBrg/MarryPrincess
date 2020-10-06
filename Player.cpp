#include "Player.hpp"
#include "EventLog.hpp"
#include "data_path.hpp"

#include <sstream>
#include <algorithm>

static constexpr int kFontSize { 800 };
static constexpr float kFontLineSpace { 0.05f };

Load< Sound::Sample > level_up_sfx_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("level_up.wav"));
});

Load< Sound::Sample > die_sfx_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("die.wav"));
});

Load< Sound::Sample > wedding_music_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("wedding.wav"));
});

enum InfoEntry : uint8_t {
    PLAYER = 0,
    LEVEL,
    HP,
    MP,
    ATTACK,
    DEFENSE,
    MONEY,
    EXP
};

void Player::Sleep() {
    SetHP(max_hp_, max_hp_);
    SetMP(max_mp_, max_mp_);
    EventLog::Instance().LogEvent("You restored all your");
    EventLog::Instance().LogEvent("hp and mp!");
}

Player::Player() :
player_info_dialog_(data_path("ariblk.ttf").c_str(), kFontColors.at("black"), glm::vec4(-0.9f, -0.3f, -0.7f, -0.8f), kFontSize, kFontColors.at("white"))
{
    player_info_dialog_.SetVisibility(true);

    std::ostringstream oss;
    glm::vec2 anchor { -0.89f, -0.35f };

    player_info_dialog_.AddText("Player", anchor);

    anchor[1] -= kFontLineSpace;
    oss.str("");
    oss << "Level: " << level_;
    player_info_dialog_.AddText(oss.str().c_str(), anchor);

    anchor[1] -= kFontLineSpace;
    oss.str("");
    oss << "HP: " << hp_ << " / " << max_hp_;
    player_info_dialog_.AddText(oss.str().c_str(), anchor);

    anchor[1] -= kFontLineSpace;
    oss.str("");
    oss << "MP: " << mp_ << " / " << max_mp_;
    player_info_dialog_.AddText(oss.str().c_str(), anchor);

    anchor[1] -= kFontLineSpace;
    oss.str("");
    oss << "Attack: " << attack_;
    player_info_dialog_.AddText(oss.str().c_str(), anchor);

    anchor[1] -= kFontLineSpace;
    oss.str("");
    oss << "Defense: " << defense_;
    player_info_dialog_.AddText(oss.str().c_str(), anchor);

    anchor[1] -= kFontLineSpace;
    oss.str("");
    oss << "Money: " << money_;
    player_info_dialog_.AddText(oss.str().c_str(), anchor);

    anchor[1] -= kFontLineSpace;
    oss.str("");
    oss << "EXP: " << experience_ << " / " << level_up_experience_;
    player_info_dialog_.AddText(oss.str().c_str(), anchor);
}

void Player::DrawInfo(const glm::uvec2& drawable_size)
{
    player_info_dialog_.Draw(drawable_size);
}

void Player::LevelUp()
{
    EventLog::Instance().LogEvent("Level up!");
    Sound::play(*level_up_sfx_sample);
    SetLevel(level_ + 1);
    int new_hp = 100 + level_ * 50;
    int new_mp = 100 + level_ * 50;
    int new_attack = 10 + level_ * 3;
    int new_defense = 10 + level_ * 3;
    int new_exp = 100 + level_ * level_ * 100;
    SetHP(new_hp, new_hp);
    SetMP(new_mp, new_mp);
    SetAttack(new_attack);
    SetDefense(new_defense);
    SetExp(experience_, new_exp);
}

void Player::SetHP(int hp, int max_hp)
{
    hp_ = hp;
    max_hp_ = max_hp;
    std::ostringstream oss;
    oss << "HP: " << hp_ << " / " << max_hp_;
    player_info_dialog_.GetText(HP)->SetText(oss.str().c_str(), kFontSize);
}

void Player::SetMP(int mp, int max_mp)
{
    mp_ = mp;
    max_mp_ = max_mp;
    std::ostringstream oss;
    oss << "MP: " << mp_ << " / " << max_mp_;
    player_info_dialog_.GetText(MP)->SetText(oss.str().c_str(), kFontSize);
}

void Player::SetAttack(int attack)
{
    attack_ = attack;
    std::ostringstream oss;
    oss << "Attack: " << attack_;
    player_info_dialog_.GetText(ATTACK)->SetText(oss.str().c_str(), kFontSize);
}

void Player::SetDefense(int defense)
{
    defense_ = defense;
    std::ostringstream oss;
    oss << "Defense: " << defense_;
    player_info_dialog_.GetText(DEFENSE)->SetText(oss.str().c_str(), kFontSize);
}

void Player::SetLevel(int level)
{
    level_ = level;
    std::ostringstream oss;
    oss << "Level: " << level_;
    player_info_dialog_.GetText(LEVEL)->SetText(oss.str().c_str(), kFontSize);
}

void Player::SetMoney(int money)
{
    money_ = money;
    std::ostringstream oss;
    oss << "Money: " << money_;
    player_info_dialog_.GetText(MONEY)->SetText(oss.str().c_str(), kFontSize);
}

void Player::SetExp(int experience, int level_up_experience)
{
    experience_ = experience;
    level_up_experience_ = level_up_experience;
    std::ostringstream oss;
    oss << "EXP: " << experience_ << " / " << level_up_experience_;
    player_info_dialog_.GetText(EXP)->SetText(oss.str().c_str(), kFontSize);
}

void Player::Die()
{
    Sound::play(*die_sfx_sample);
    EventLog::Instance().LogEvent("You die!");
    EventLog::Instance().LogEvent("Lose half money!");
    EventLog::Instance().LogEvent("Lose half exp!");
    Sleep();
    SetMoney(money_ / 2);
    SetExp(experience_ / 2, level_up_experience_);
}

bool Player::MarryPrincess() {
    if (married_) {
        EventLog::Instance().LogEvent("You've already");
        EventLog::Instance().LogEvent("married princess!");
        return false;
    }

    if (money_ >= 50000) {
        EventLog::Instance().LogEvent("You married princess!");
        Sound::play(*wedding_music_sample);
        married_ = true;
        return true;
    } else {
        EventLog::Instance().LogEvent("Go away!");
        return false;
    }
}

int Player::Attack(int defense)
{
    int damage = std::max(attack_ - defense, 1);

    return damage;
}

bool Player::ApplyDamage(int attack)
{
    int damage = std::max(attack - defense_, 1);
    int new_hp = hp_ - damage;

    std::ostringstream oss;
    oss << "Player received " << damage << " damage!";
    EventLog::Instance().LogEvent(oss.str());

    if (new_hp <= 0) {
        Die();
        return true;
    } else {
        SetHP(new_hp, max_hp_);
        return false;
    }
}

void Player::GainExperience(int exp) {
    std::ostringstream oss;
    oss << "Gain " << exp << " Exp!";
    EventLog::Instance().LogEvent(oss.str());

    experience_ += exp;
    SetExp(experience_, level_up_experience_);

    while (experience_ >= level_up_experience_) {
        experience_ -= level_up_experience_;
        LevelUp();
    }
}

void Player::GainMoney(int money)
{
    std::ostringstream oss;
    oss << "Gain " << money << " money!";
    EventLog::Instance().LogEvent(oss.str());

    SetMoney(money + money_);
}
