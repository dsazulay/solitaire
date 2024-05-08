#include "freecell_datamanager.h"

#include <filesystem>
#include "../serializer.h"

auto FreecellDataManager::loadPlayerData() -> void
{
    std::filesystem::path file{ "../../resources/gamedata.dat" };
    if (std::filesystem::exists(file))
    {
        Serializer serializer(m_playerData, "../../resources/gamedata.dat");
        serializer.load();
        serializer.deserialize();
        return;
    }

    // first time openning application
    constexpr const float maxTime = 10000.0f;
    m_playerData.gamesPlayed = 0;
    m_playerData.gamesWon = 0;
    m_playerData.bestTime = maxTime;

    Serializer serializer(m_playerData, "../../resources/gamedata.dat");
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

    Serializer serializer(m_playerData, "../../resources/gamedata.dat");
    serializer.serialize();
    serializer.save();
}

auto FreecellDataManager::playerData() -> PlayerData*
{
    return &m_playerData;
}
