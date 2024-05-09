#pragma once

#include "../card.h"
#include "freecell_boardmanager.h"

class FreecellGameLogic
{
public:
    auto init(FreecellBoardManager* manager) -> void;
    auto checkSequence(const CardStack& stack, int j) -> bool;
    auto checkWinSequence(const CardStack& stack) -> bool;
    auto checkWin() -> bool;
    auto isComplete() -> bool;
    auto getMaxCardsToMove(bool movingToEmptySpace) -> int;

    auto openCellsIsLegalMove(CardEntity* card, const CardStack& stack) -> bool;
    auto foundationsIsLegalMove(CardEntity* card, const CardStack& stack) -> bool;
    auto tableauIsLegalMove(CardEntity* card, const CardStack& stack) -> bool;

private:
    FreecellBoardManager* boardManager;
};

