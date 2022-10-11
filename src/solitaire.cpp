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
glm::vec2 map[8][8];
std::vector<Card> table[8];
int selected = -1;

void setBoardLayout()
{
    float initPosX = 80;
    float offsetX = 160;
    float initPosY = 400;
    float offsetY = 32;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            map[i][j] = glm::vec2(initPosX + i * offsetX, initPosY - j * offsetY);
        }
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
            deck.push_back(c);
        }
    }
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

void Solitaire::onMouseClick(const Event& e)
{
    const auto& event = static_cast<const MouseClickEvent&>(e);
    double xpos = event.xpos();
    int cardSize = 100;
    for (int i = 0; i < 8; i++)
    {
        if (xpos > map[i][0].x - 50 && xpos < map[i][0].x - 50 + cardSize)
        {
            if (selected != -1)
            {
                table[i].push_back(table[selected].back());
                table[selected].pop_back();
                selected = -1;
                return;
            }

            if (table[i].size() > 0)
            {
                selected = i;
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
