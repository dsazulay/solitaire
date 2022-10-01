#include "solitaire.h"

#include "resource_manager.h"
#include "shader.h"
#include "utils/log.h"

#include <vector>
#include <glm/vec2.hpp>

#include <time.h>

float Solitaire::deltaTime;

std::vector<Card> deck;
glm::vec2 map[8][8];

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
}

void Solitaire::mainLoop()
{
    while (!m_window->shouldClose())
    {
        calculateDeltaTime();

        m_window->processInput();

        m_renderer->render(map, deck);

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
