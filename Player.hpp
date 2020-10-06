#pragma once

#include <glm/glm.hpp>

class Player {
private:
    float hp_ { 100.0f };
    float max_hp_{ 100.0f };

    float mp_{ 100.0f };
    float max_mp_{ 100.0f };

    float attack_{ 10.0f };
    float intelligence_{ 10.0f };
    float defence_{ 10.0f };
    
    int level_ { 1 };
    int money_ { 0 };
    int experience_ { 0 };
    int level_up_experience_ { 100 };
    
public:
    Player() {}
    static Player& Instance()
    {
        static Player player;
        return player;
    }

    void DrawInfo(const glm::uvec2& drawable_size);
    void Sleep();
};
