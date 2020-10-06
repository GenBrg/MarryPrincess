#include "Player.hpp"
#include "EventLog.hpp"

void Player::Sleep() {
    hp_ = max_hp_;
    mp_ = max_mp_;
    EventLog::Instance().LogEvent("You restored all your");
    EventLog::Instance().LogEvent("hp and mp!");
}

Player::Player()
{

}