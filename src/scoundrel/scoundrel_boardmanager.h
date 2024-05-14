#pragma once

#include <span>
#include <glm/vec2.hpp>
#include "../card.h"
#include "../game_board.h"
#include "../dealer.h"

constexpr const int CARD_BG_SIZE = 4;
constexpr const int ROOM_SIZE = 4;
constexpr const int WPN_MAX_SIZE = 10;
constexpr const glm::vec2 DUNGEON_POS{400.0, 560.0};
constexpr const glm::vec2 DISCARD_POS{790.0, 560.0};
constexpr const glm::vec2 POTION_POS{660.0, 240.0};
constexpr const glm::vec2 HANDS_POS{790.0, 240.0};
constexpr const glm::vec2 ROOM_POS{400.0, 400.0};
constexpr const glm::vec2 WPN_POS{400.0, 240.0};
constexpr const float ROOM_OFFSET = 130.0;
constexpr const float WPN_OFFSET = 20.0;

enum class ScoundrelArea
{
    Dungeon,
    Discard,
    Room,
    Weapon,
    Potion,
    Hands,
    None,
};

struct CardClickedScoundrel
{
    CardStack* stack;
    glm::vec2 pos;
    glm::vec2 selectionPos;
    ScoundrelArea area;
};

struct ScoundrelBoard
{
    CardStack cards;
    std::span<CardBg> cardBgs;

    CardStack dungeon;
    CardStack discard;
    CardStack weapon;
    CardStack potion;
    CardStack hands;
    std::array<CardStack, ROOM_SIZE> room;
};

struct ScoundrelBoardMap
{
    glm::vec2 dungeon;
    glm::vec2 discard;
    glm::vec2 potion;
    glm::vec2 hands;
    std::array<glm::vec2, ROOM_SIZE> room{};
    std::array<glm::vec2, WPN_MAX_SIZE> weapon{};

    // move this to struct constructor
    ScoundrelBoardMap() : dungeon{ DUNGEON_POS }, discard{ DISCARD_POS },
        potion{ POTION_POS }, hands{ HANDS_POS }
    {
        for (int i = 0; i < ROOM_SIZE; ++i)
        {
            room[i] = { ROOM_POS.x + (float) i * ROOM_OFFSET, ROOM_POS.y };
        }

        for (int i = 0; i < WPN_MAX_SIZE; ++i)
        {
            weapon[i] = { WPN_POS.x + (float) i * WPN_OFFSET, WPN_POS.y };
        }
    }
};


class ScoundrelBoardManager : public BoardManager
{
public:
    auto createDeck() -> void override;
    auto shuffleDeck() -> void override;
    auto emptyTable() -> void override;
    auto fillTable() -> void override;
    auto updateCardList() -> void override;

    auto getNumberOfAvailableCards() -> int;
    auto fillRoom() -> void;
    auto clearTableForNextFloor() -> void;
    auto discardWeapon() -> void;
    auto run() -> void;

    auto moveCard(CardStack& src, CardStack& dst, int n) -> void;

    auto getArea(double x, double y) -> ScoundrelArea;
    auto getStackAndPos(double x, double y) -> std::optional<CardClickedScoundrel>;

    auto selectCard(CardClickedScoundrel c) -> void;
    auto deselectCard() -> void;
    auto getCardSelected() -> std::optional<CardClickedScoundrel>;

    // tmp stuff
    auto board() -> ScoundrelBoard&;
    auto boardMap() -> ScoundrelBoardMap&;

private:
    auto getIndexX(std::span<glm::vec2> area, double xPos) -> int;
    auto getIndexY(int stackSize, double yPos) -> int;

    Dealer m_dealer;
    ScoundrelBoard m_board;
    ScoundrelBoardMap m_boardMap;
    std::optional<CardClickedScoundrel> m_cardSelected;
};

