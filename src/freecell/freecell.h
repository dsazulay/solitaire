#pragma once

#include <vector>
#include <span>
#include <array>

#include <glm/vec2.hpp>

#include "../history.h"
#include "../card.h"
#include "../dragging_animation.h"
#include "../moving_animation.h"
#include "freecell_boardmanager.h"
#include "freecell_gamelogic.h"
#include "freecell_datamanager.h"

enum class GameState
{
    Playing,
    WinAnimation,
    Won,
    Pause
};

struct Move
{
    CardStack* srcStack;
    CardStack* dstStack;
    int cardQuantity;
    glm::vec2 srcPos;
    glm::vec2 dstPos;

    auto swap() noexcept -> Move
    {
        Move newMove{dstStack, srcStack, cardQuantity, dstPos, srcPos};
        return newMove;
    }
};

class Freecell
{
    using IsLegalMoveFunc = bool(FreecellGameLogic::*)(CardEntity* card, const CardStack& stack);
public:
    Freecell();
    auto  init() -> void;
    auto  update() -> void;

    auto handleInputClick(double xPos, double yPos, bool isDraging, bool isDragStart) -> void;
    auto handleInputDoubleClick(double xPos, double yPos) -> void;
    auto handleInputUndo() -> void;
    auto handleInputRedo() -> void;
    auto handleInputRestart() -> void;
    auto handleInputNewGame() -> void;
    auto handleInputPause() -> void;
    auto handleInputPrintCards() -> void;

    auto board() -> FreecellBoard&;
    auto playerData() -> PlayerData*;
    auto matchData() -> MatchData*;

private:
    auto createOpenCellsAndFoundations() -> void;

    auto playWinAnimation() -> void;

    auto handleClick(CardStack& stack, glm::vec2 dstPos,
            IsLegalMoveFunc isLegalMove) -> void;

    auto moveBackAndDeselectCard() -> void;

    auto winMoves(CardStack& src, std::span<CardStack> dst, std::span<float> dstAreaPos, IsLegalMoveFunc isLegalMove) -> void;
    auto tryMoveFromTo(CardStack& src, std::span<CardStack> dst, std::span<float> dstAreaPos, IsLegalMoveFunc isLegalMove) -> bool;

    History<Move> m_history;
    std::vector<MovingAnimation> m_movingAnimation;
    DraggingAnimation m_draggingAnimation;

    GameState m_currentState{};
    MatchData m_matchData{};

    std::array<CardBg, SPECIAL_AREAS_SIZE * 2> m_specialAreas;

    FreecellDataManager m_dataManager;
    FreecellBoardManager m_boardManager;
    FreecellGameLogic m_gameLogic{};
};

