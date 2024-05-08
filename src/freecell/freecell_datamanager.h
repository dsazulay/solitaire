#pragma once

#include "../gamedata.h"

class FreecellDataManager
{
public:
    auto loadPlayerData() -> void;
    auto updatePlayerData(bool didWon, float time) -> void;
    auto playerData() -> PlayerData*;

private:
    PlayerData m_playerData{};
};

