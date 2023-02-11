#pragma once

#include <cstddef>
#include <array>
#include <string>
#include <string_view>
#include <cstring>
#include <fstream>

#include "gamedata.h"
#include "utils/log.h"

class Serializer
{
public:
    Serializer(PlayerData& playerData, std::string_view fileName) : m_playerData(playerData), m_fileName(fileName)
    {
    }

    auto serialize() -> std::size_t
    {
        m_position = 0;
        auto out = [&](auto&& value)
        {
            if (sizeof(value) > m_data.size() - m_position)
                return false;
            
            std::memcpy(m_data.data() + m_position, &value, sizeof(value));
            m_position += sizeof(value);

            return true;
        };

        if (!out(m_playerData.gamesPlayed)) { return 0; }
        if (!out(m_playerData.gamesWon)) { return 0; }
        if (!out(m_playerData.bestTime)) { return 0; }

        return m_position;
    }

    auto deserialize() -> std::size_t
    {
        m_position = 0;
        auto in = [&](auto& value)
        {
            if (sizeof(value) > m_data.size() - m_position)
                return false;

            std::memcpy(&value, m_data.data() + m_position, sizeof(value));
            m_position += sizeof(value);
            
            return true;
        };

        if (!in(m_playerData.gamesPlayed)) { return 0; }
        if (!in(m_playerData.gamesWon)) { return 0; }
        if (!in(m_playerData.bestTime)) { return 0; }

        return m_position;
    }
    
    void save()
    {
        std::ofstream out(m_fileName, std::ios::binary);
        if (!out)
        {
            LOG_ERROR("Could not open the file {} for writing", m_fileName);
            return;
        }

        out.write(reinterpret_cast<char*>(m_data.data()), (std::streamsize) m_position);
        out.close();
    }

    void load()
    {
        std::ifstream in(m_fileName, std::ios::binary);
        if (!in.is_open())
        {
            LOG_ERROR("Could not open the file {} for reading", m_fileName);
            return;
        }

        in.seekg(0, std::ios::end);
        std::streampos fileSize = in.tellg();
        in.seekg(0, std::ios::beg);

        in.read(reinterpret_cast<char*>(m_data.data()), fileSize);
        in.close();
    }

private:
    constexpr static int DATA_MAXSIZE = 256;

    std::array<std::byte, DATA_MAXSIZE> m_data{};
    std::size_t m_position{};

    PlayerData& m_playerData;
    std::string m_fileName;
};
