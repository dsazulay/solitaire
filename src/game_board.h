#pragma once

constexpr const float CARD_HALF_WIDTH = 50.0f;
constexpr const float CARD_HALF_HEIGHT = 74.0f;

enum class GameState
{
    Playing,
    WinAnimation,
    Won,
    Pause
};

class BoardManager
{
public:
    virtual ~BoardManager() = default;
    virtual auto createDeck() -> void = 0;
    virtual auto shuffleDeck() -> void = 0;
    virtual auto emptyTable() -> void = 0;
    virtual auto fillTable() -> void = 0;
    virtual auto updateCardList() -> void = 0;
};

