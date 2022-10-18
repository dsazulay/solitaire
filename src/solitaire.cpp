#include "solitaire.h"

#include "dispatcher.h"
#include "resource_manager.h"
#include "shader.h"
#include "utils/log.h"

#include <vector>
#include <glm/vec2.hpp>

#include <time.h>

float Solitaire::deltaTime;

std::vector<Card> deck;
glm::vec2 map[8][12];
glm::vec2 openCellsMap[4];
glm::vec2 foundationMap[4];
std::vector<Card> table[8];
std::vector<Card> openCells[4];
std::vector<Card> foundations[4];
int selectedX = -1;
int selectedY = -1;

void setBoardLayout()
{
    float initPosX = 80;
    float offsetX = 160;
    float initPosY = 400;
    float offsetY = 32;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            map[i][j] = glm::vec2(initPosX + i * offsetX, initPosY - j * offsetY);
        }
    }

    for (int i = 0; i < 4; i++)
    {
        openCellsMap[i] = glm::vec2(initPosX + i * offsetX, 600);
    }

    initPosX += 4 * offsetX;
    for (int i = 0; i < 4; i++)
    {
        foundationMap[i] = glm::vec2(initPosX + i * offsetX, 600);
    }
}

void fillTable(std::vector<Card> deck)
{
    int index = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            table[j].push_back(deck.at(index++));
            if (index > 51)
                return;
        }
    }
}

void createDeck()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            Card c;
            c.number = j;
            c.suit = i;
            c.color = i > 1;
            deck.push_back(c);
        }
    }
}

void createOpenCellsAndFoundations()
{
    for (int i = 0; i < 4; i++)
    {
        openCells[i].push_back(deck.at(13));
    }

    foundations[0].push_back(deck.at(0));
    foundations[1].push_back(deck.at(13));
    foundations[2].push_back(deck.at(26));
    foundations[3].push_back(deck.at(39));
}

void swap(int i, int j)
{
    auto tmp = deck[i];
    deck[i] = deck[j];
    deck[j] = tmp;
}

void shuffle()
{
    for (int i = deck.size() - 1; i >= 0; i--)
    {
        int j = rand() % (i + 1);
        swap(i, j);
    }
}

bool isLegalMoveTable(int src, int dst)
{
    bool diffColor = table[src].back().color != table[dst].back().color;
    bool nextNumber = table[src].back().number == table[dst].back().number - 1;

    return diffColor && nextNumber;
}

void Solitaire::onMouseClick(const Event& e)
{
    const auto& event = static_cast<const MouseClickEvent&>(e);
    double xpos = event.xpos();
    double ypos = event.ypos();
    int cardSize = 100;
    int cardHeight = 180;

    //LOG_INFO(xpos << " " << ypos);

    for (int i = 0; i < 8; i++)
    {
        if (xpos > map[i][0].x - 50 && xpos < map[i][0].x - 50 + cardSize)
        {
            if (ypos > 121 - 74 && ypos < 121 + 74)
            {
                if (xpos < 700)
                {
                    if (selectedX != -1)
                    {
                        if (selectedY == 20)
                        {
                            openCells[i].push_back(openCells[selectedX].back());
                            openCells[selectedX].pop_back();
                        }
                        else
                        {
                            openCells[i].push_back(table[selectedX].back());
                            table[selectedX].pop_back();
                        }
                        selectedX = -1;
                        selectedY = -1;
                        return;
                    }

                    selectedX = i;
                    selectedY = 20;
                    return;
                }
                else
                {
                    LOG_INFO("foundations");
                    return;
                }
            }
            else
            {
                int stackSize = table[i].size();
                ypos = 720 - ypos;
                for (int j = 0; j < stackSize - 1; j++)
                {
                    if (ypos < map[i][j].y + 74 && ypos > map[i][j].y + 30)
                    {
                        LOG_INFO("no support for multiple selection");
                        return;
                    }
                }
                if (stackSize > 0 && ypos < map[i][stackSize - 1].y + 74 && ypos > map[i][stackSize - 1].y - 74)
                {
                    if (selectedX != -1)
                    {
                        if (selectedY == 20)
                        {
                            table[i].push_back(openCells[selectedX].back());
                            openCells[selectedX].pop_back();
                        }
                        else
                        {
                            if (!isLegalMoveTable(selectedX, i))
                            {
                                LOG_INFO("Invalid move");
                            }
                            else
                            {
                                table[i].push_back(table[selectedX].back());
                                table[selectedX].pop_back();
                            }
                        }
                        selectedX = -1;
                        selectedY = -1;
                        return;
                    }

                    selectedX = i;
                    selectedY = stackSize - 1;
                    return;
                }
            }
        }
    }
}

Solitaire::Solitaire()
{
    m_appConfig.windowName = "Solitaire";
    m_appConfig.windowWidth = 1280;
    m_appConfig.windowHeight = 720;
}

void Solitaire::run()
{
    init();
    mainLoop();
    terminate();
}

void Solitaire::init()
{
    m_window = new Window();
    m_window->init();
    m_window->createWindow(m_appConfig.windowWidth, m_appConfig.windowHeight, m_appConfig.windowName.c_str());

    ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD");

    m_renderer = new Renderer();
    m_renderer->init();

    // Game init
    setBoardLayout();
    createDeck();
    createOpenCellsAndFoundations();
    srand(time(NULL));
    shuffle();
    fillTable(deck);

    Dispatcher::instance().subscribe(MouseClickEvent::descriptor,
        std::bind(&Solitaire::onMouseClick, this, std::placeholders::_1));
}

void Solitaire::mainLoop()
{
    while (!m_window->shouldClose())
    {
        calculateDeltaTime();

        m_window->processInput();

        m_renderer->render(map, table);
        m_renderer->renderOpenCellsAndFoundation(openCellsMap, openCells);
        m_renderer->renderOpenCellsAndFoundation(foundationMap, foundations);

        m_window->swapBuffers();
        m_window->pollEvents();
    }
}

void Solitaire::terminate()
{
    m_renderer->terminate();
    m_window->terminate();

    delete m_renderer;
    delete m_window;
}

void Solitaire::calculateDeltaTime()
{
    float currentFrame = (float) glfwGetTime();
    deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;
}
