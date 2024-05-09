#include "freecell_gamelogic.h"

auto FreecellGameLogic::init(FreecellBoardManager* manager) -> void
{
    boardManager = manager;
}

auto FreecellGameLogic::checkSequence(const CardStack& stack, int j) -> bool
{
    int currentCard = static_cast<int>(stack.size()) - 1;

    for (int n = currentCard; n > j; n--)
    {
        bool diffColor = stack[n]->card.suit % 2 != stack[n - 1]->card.suit % 2;
        bool nextNumber = stack[n]->card.number == stack[n - 1]->card.number - 1;

        if (!(diffColor && nextNumber))
        {
            return false;
        }
    }

    return true;
}

auto FreecellGameLogic::checkWinSequence(const CardStack& stack) -> bool
{
    int currentCard = static_cast<int>(stack.size()) - 1;

    for (int n = currentCard; n > 0; n--)
    {
        bool nextNumber = stack[n]->card.number <= stack[n - 1]->card.number;

        if (!nextNumber)
        {
            return false;
        }
    }

    return true;
}

auto FreecellGameLogic::checkWin() -> bool
{
    for (const CardStack& stack : boardManager->board().tableau)
    {
        if (!checkWinSequence(stack))
            return false;
    }

    return true;
}

auto FreecellGameLogic::isComplete() -> bool
{
    constexpr static int winStackSize = 13;
    for (const CardStack& stack : boardManager->board().foundations)
    {
        if (stack.size() != winStackSize)
        {
            return false;
        }
    }

    return true;
}

auto FreecellGameLogic::getMaxCardsToMove(bool movingToEmptySpace) -> int
{
    // Initialize with one because you can always move one card
    int emptyOpenCells = 1;
    for (auto stack : boardManager->board().openCells)
    {
        emptyOpenCells += static_cast<int>(stack.empty());
    }

    int emptyTableColumns = movingToEmptySpace ? -1 : 0;
    for (auto stack : boardManager->board().tableau)
    {
        emptyTableColumns += static_cast<int>(stack.empty());
    }

    return emptyTableColumns * emptyOpenCells + emptyOpenCells;
}

auto FreecellGameLogic::openCellsIsLegalMove(CardEntity* card,
        const CardStack& stack) -> bool
{
    auto cardClicked = boardManager->getCardSelected();
    if (cardClicked.has_value())
    {
        auto stacksize = static_cast<int>(cardClicked->stack->size());
        if (cardClicked->index != stacksize - 1)
            return false;
    }

    return stack.size() == 0;
}

auto FreecellGameLogic::foundationsIsLegalMove(CardEntity* card,
        const CardStack& stack) -> bool
{
    auto cardClicked = boardManager->getCardSelected();
    if (cardClicked.has_value())
    {
        auto stacksize = static_cast<int>(cardClicked->stack->size());
        if (cardClicked->index != stacksize - 1)
            return false;
    }

    if (stack.size() == 0)
        return card->card.number == 0;

    return stack.back()->card.number == card->card.number - 1
        && stack.back()->card.suit == card->card.suit;
}

auto FreecellGameLogic::tableauIsLegalMove(CardEntity* card,
        const CardStack& stack) -> bool
{
    // Note: check if we alway gonna have a selected card here
    auto cardClicked = boardManager->getCardSelected().value();

    int cardSelectedRow = cardClicked.index;
    int diff = static_cast<int>(cardClicked.stack->size()) - cardSelectedRow;
    if (getMaxCardsToMove(stack.empty()) < diff)
        return false;

    if (stack.size() == 0)
        return true;

    bool diffColor =  stack.back()->card.suit % 2 != card->card.suit % 2;
    bool nextNumber = stack.back()->card.number - 1 == card->card.number;

    return diffColor && nextNumber;
}

