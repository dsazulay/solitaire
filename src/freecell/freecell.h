#pragma once

#include <span>
#include <array>
#include <glm/vec2.hpp>

#include "../history.h"
#include "../card.h"
#include "../iinput_handler.h"
#include "../animation/animation_engine.h"
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

class Freecell : public IInputHandler
{
    using IsLegalMoveFunc = bool(FreecellGameLogic::*)(CardEntity* card, const CardStack& stack);
public:
    Freecell();
    auto  init(AnimationEngine* engine) -> void;
    auto  update() -> void;

    auto handleClick(double xPos, double yPos, bool isDragging,
            bool isDragStart) -> void override;
    auto handleDoubleClick(double xpos, double ypos) -> void override;
    auto handleUndo() -> void override;
    auto handleRedo() -> void override;
    auto handleRestart() -> void override;
    auto handleNewGame() -> void override;
    auto handlePause() -> void override;
    auto handlePrintCards() -> void override;

    auto board() -> FreecellBoard&;
    auto playerData() -> PlayerData*;
    auto matchData() -> MatchData*;

private:
    auto createOpenCellsAndFoundations() -> void;
    auto playWinAnimation() -> void;
    auto handleClick(CardStack& stack, glm::vec2 dstPos,
            IsLegalMoveFunc isLegalMove) -> void;
    auto moveBackAndDeselectCard() -> void;
    auto winMoves(CardStack& src, std::span<CardStack> dst,
            std::span<float> dstAreaPos, IsLegalMoveFunc isLegalMove) -> void;
    auto tryMoveFromTo(CardStack& src, std::span<CardStack> dst,
            std::span<float> dstAreaPos, IsLegalMoveFunc isLegalMove) -> bool;


    GameState m_currentState{};
    MatchData m_matchData{};
    std::array<CardBg, SPECIAL_AREAS_SIZE * 2> m_specialAreas;

    FreecellDataManager m_dataManager;
    FreecellBoardManager m_boardManager;
    FreecellGameLogic m_gameLogic{};
    History<Move> m_history;
    AnimationEngine* animationEngine{};
};

