#include "freecell_datamanager.h"

#include <filesystem>
#include "../serializer.h"

constexpr const char* DATA_PATH = "resources/gamedata.dat";

auto FreecellDataManager::loadPlayerData() -> void
{
    std::filesystem::path file{ DATA_PATH };
    if (std::filesystem::exists(file))
    {
        Serializer serializer(m_playerData, DATA_PATH);
        serializer.load();
        serializer.deserialize();
        return;
    }

    // first time openning application
    constexpr const float maxTime = 10000.0f;
    m_playerData.gamesPlayed = 0;
    m_playerData.gamesWon = 0;
    m_playerData.bestTime = maxTime;

    Serializer serializer(m_playerData, DATA_PATH);
    serializer.serialize();
    serializer.save();
}

auto FreecellDataManager::updatePlayerData(bool didWon, float time) -> void
{
    m_playerData.gamesPlayed++;
    if (didWon)
    {
        m_playerData.gamesWon++;
        if (time < m_playerData.bestTime)
        {
            m_playerData.bestTime = time;
        }
    }

    Serializer serializer(m_playerData, DATA_PATH);
    serializer.serialize();
    serializer.save();
}

auto FreecellDataManager::playerData() -> PlayerData*
{
    return &m_playerData;
}
