#include "scoundrel.h"
#include "scoundrel_boardmanager.h"

constexpr const int MAX_LIFE = 20;

auto Scoundrel::init(AnimationEngine* engine) -> void
{
    animationEngine = engine;
    createBgCards();
    m_boardManager.createDeck();
    m_boardManager.shuffleDeck();
    m_boardManager.fillTable();
    m_boardManager.fillRoom();
    m_boardManager.updateCardList();
    m_life = MAX_LIFE;
}

auto Scoundrel::createBgCards() -> void
{
    auto& m_boardMap = m_boardManager.boardMap();
    constexpr const glm::vec2 cardbackUV = { 0.0, 0.875f };
    constexpr const glm::vec2 weaponUV = { 0.625, 0.875f };
    constexpr const glm::vec2 potionUV = { 0.500, 0.875f };
    constexpr const glm::vec2 handsUV = { 0.375, 0.875f };

    m_cardBg[0] = CardBg(cardbackUV, glm::vec3{ m_boardMap.dungeon, 0.0 });
    m_cardBg[1] = CardBg(weaponUV, glm::vec3{ m_boardMap.weapon[0], 0.0 });
    m_cardBg[2] = CardBg(potionUV, glm::vec3{ m_boardMap.potion, 0.0 });
    m_cardBg[3] = CardBg(handsUV, glm::vec3{ m_boardMap.hands, 0.0 });

    m_boardManager.board().cardBgs = m_cardBg;
}

auto Scoundrel::moveBackAndDeselectCard() -> void
{
    auto card = m_boardManager.getCardSelected().value();
    std::span<CardEntity*> cards(&card.stack->back(), 1);
    MovingAnimation m(cards, card.stack->back()->transform.pos(),
            card.selectionPos);
    animationEngine->addMovingAnimation(m);

    m_boardManager.deselectCard();
    animationEngine->stopDraggingAnimation();
}

auto Scoundrel::board() -> ScoundrelBoard&
{
    return m_boardManager.board();
}

auto Scoundrel::handleClick(double xpos, double ypos, bool isDragging,
            bool isDragStart) -> void
{
    if (animationEngine->getMovingAnimationQuantity() > 0)
    {
        LOG_WARN("Can't input while card is moving!");
        return;
    }

    auto areaClicked = m_boardManager.getStackAndPos(xpos, ypos);
    if (!areaClicked.has_value())
    {
        if (!m_boardManager.getCardSelected().has_value())
            return;
        moveBackAndDeselectCard();
        return;
    }

    auto cardClicked = areaClicked.value();
    if (!m_boardManager.getCardSelected().has_value())
    {
        if (!isDragStart)
            return;

        int stackSize = static_cast<int>(cardClicked.stack->size());
        if (stackSize == 0)
        {
            LOG_INFO("Cannot select empty stack");
            return;
        }

        if (cardClicked.area == ScoundrelArea::Dungeon)
        {
            if (m_boardManager.getNumberOfAvailableRooms() > 1)
            {
                LOG_INFO("You need to have cleared at least 3 rooms");
                return;
            }
            m_boardManager.clearTableForNextFloor();
            m_boardManager.fillRoom();
            m_boardManager.updateCardList();
            return;
        }

        if (cardClicked.area != ScoundrelArea::Room)
        {
            LOG_INFO("Cannot select this");
            return;
        }

        std::span<CardEntity*> cards(&cardClicked.stack->back(), 1);
        animationEngine->addDraggingAnimation(DraggingAnimation{ cards });

        m_boardManager.selectCard(cardClicked);
        return;
    }

    auto cardSelected = m_boardManager.getCardSelected().value();
    if (cardClicked.area == ScoundrelArea::Room)
    {
        LOG_INFO("Nothing to do");
        moveBackAndDeselectCard();
        return;
    }
    if (cardClicked.area == ScoundrelArea::Potion)
    {
        if (cardSelected.stack->back()->card.suit != 0)
        {
            LOG_INFO("Can only move hearts cards to potion slot");
            moveBackAndDeselectCard();
            return;
        }
        if (cardClicked.stack->size() > 0)
        {
            LOG_INFO("Can't move more than one card on the same room");
            moveBackAndDeselectCard();
            return;
        }
        m_life = glm::min(20, m_life + cardSelected.stack->back()->card.number + 1);
        executeMove(cardSelected, cardClicked);
        return;
    }
    if (cardClicked.area == ScoundrelArea::Hands)
    {
        if (cardSelected.stack->back()->card.suit % 2 == 0)
        {
            LOG_INFO("can only fight mosters");
            moveBackAndDeselectCard();
            return;
        }
        m_life -= cardSelected.stack->back()->card.number + 1;
        executeMove(cardSelected, cardClicked);
        LOG_INFO("{}", m_life);
        return;
    }
    if (cardClicked.area == ScoundrelArea::Weapon)
    {
        int suit = cardSelected.stack->back()->card.suit;
        if (suit == 0)
        {
            LOG_INFO("can't use here");
            moveBackAndDeselectCard();
            return;
        }
        if (suit == 2)
        {
            if (cardClicked.stack->size() != 0)
            {
                LOG_INFO("Cannot have more than one weapon");
                moveBackAndDeselectCard();
                return;
            }
            executeMove(cardSelected, cardClicked);
            return;
        }
        if (cardClicked.stack->size() == 0)
        {
            LOG_INFO("you need a weapon");
            moveBackAndDeselectCard();
            return;
        }

        auto& weaponStack = m_boardManager.board().weapon;
        int lastMonsterPower = weaponStack.back()->card.number;
        int monsterPower = cardSelected.stack->back()->card.number;
        if (weaponStack.size() > 1 && monsterPower > lastMonsterPower)
        {
            LOG_INFO("can't fight stronger mosters than the last one");
            moveBackAndDeselectCard();
            return;
        }

        int weaponValue = weaponStack[0]->card.number;
        m_life += glm::min(0, weaponValue - monsterPower);
        executeMove(cardSelected, cardClicked);
        LOG_INFO("{}", m_life);
    }
}

auto Scoundrel::handleDoubleClick(double xpos, double ypos) -> void
{
    LOG_ERROR("Scoundrel double click");
}

auto Scoundrel::handleNewGame() -> void
{
    LOG_ERROR("scoundrel new game");
}

auto Scoundrel::executeMove(CardClickedScoundrel& selected,
                           CardClickedScoundrel& dst) -> void
{
    CardStack* srcStack = selected.stack;
    CardStack* dstStack = dst.stack;
    glm::vec2 srcPos = srcStack->back()->transform.pos();
    glm::vec2 dstPos = dst.pos;
    std::span<CardEntity*> cards(&srcStack->back(), 1);
    MovingAnimation m(cards, srcPos, dstPos, [&, srcStack, dstStack]()
    {
        m_boardManager.moveCard(*srcStack, *dstStack, 1);
    });
    animationEngine->addMovingAnimation(m);

    m_boardManager.deselectCard();
    animationEngine->stopDraggingAnimation();
}

