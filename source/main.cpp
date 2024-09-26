#define DEBUG

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <chrono>

#include <error_context.hpp>
#include <variable_bitset.hpp>
#include <math_util.hpp>
#include <windows.h>

#include "constants.hpp"
#include "globals.hpp"
#include "game/game.hpp"
#include "shaders/shader.hpp"

#define GLFW_FIRST_KEY GLFW_KEY_SPACE
#define GLFW_KEY_COUNT GLFW_KEY_LAST - GLFW_FIRST_KEY + 1

bool glInit()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    gGameWnd = glfwCreateWindow(videoMode->width, videoMode->height, "FPS PAKRAC 24", nullptr, nullptr);
    if (gGameWnd == nullptr)
    {
        gErrCtx.setErr("Failed to create GLFW window\n");
        return false;
    }

    glfwMakeContextCurrent(gGameWnd);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        gErrCtx.setErr("Failed to initialize GLAD");
        return false;
    }

    glViewport(0, 0, videoMode->width, videoMode->height);

    glfwSetFramebufferSizeCallback(gGameWnd, [](GLFWwindow*, const int pWidth, const int pHeight)
    {
        glViewport(0, 0, pWidth, pHeight);
    });

    glEnable(GL_DEPTH_TEST);

    initializeShaders();

    return true;
}

typedef struct GameInputsStruct
{
private:
    enum
    {
        KEYSTATE_HELD,
        KEYSTATE_JUST_PRESSED,
        KEYSTATE_JUST_RELEASED,
        KEYSTATE_MAX
    };

    VariableBitset mKeyMap = VariableBitset(GLFW_KEY_COUNT * KEYSTATE_MAX, 2);

public:
    glm::f64vec2 lastMousePos;
    glm::f64vec2 mousePos;
    glm::f64vec2 mouseDelta;

    GameInputsStruct()
    {
        mousePos     = glm::f64vec2();
        mouseDelta   = glm::f64vec2();
        lastMousePos = glm::f64vec2();
    }

    int keyState(const int pKey) const
    {
        return mKeyMap.get(pKey - GLFW_FIRST_KEY);
    }

    bool keyHeld(const int pKey) const
    {
        return keyState(pKey) == KEYSTATE_HELD;
    }

    bool keyJustPressed(const int pKey) const
    {
        return keyState(pKey) == KEYSTATE_JUST_PRESSED;
    }

    bool keyJustReleased(const int pKey) const
    {
        return keyState(pKey) == KEYSTATE_JUST_RELEASED;
    }
} GameInputs;

GameInputs gGameInputs = {};

void gameProcessInput(GLFWwindow* pWnd)
{
    if (glfwGetKey(pWnd, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(pWnd, true);
    }

    gGameInputs.lastMousePos = gGameInputs.mousePos;
    glfwGetCursorPos(pWnd, &gGameInputs.mousePos.x, &gGameInputs.mousePos.y);
    gGameInputs.mouseDelta = gGameInputs.lastMousePos - gGameInputs.mousePos;
}

void gameMain(const std::vector<std::string>& pArgs)
{
    auto time = 0.0F;

    gGame->gameOnStart();

    const float targetFps       = 144;
    const float targetFrameRate = 1 / targetFps;

    float  deltaTime    = 0;
    size_t frameCounter = 0;
    float  fpsTimer     = 0;

    float lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(gGameWnd))
    {
        const auto frameStartTime = glfwGetTime();

        if (frameStartTime - lastFrameTime >= targetFrameRate)
        {
            deltaTime     = frameStartTime - lastFrameTime;
            lastFrameTime = frameStartTime;
        }
        else
        {
            continue;
        }

        gameProcessInput(gGameWnd);

        // render shit to the screen here
        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gGame->gameOnGfxUpdate();

        auto fps = 1 / deltaTime;
        if (fpsTimer >= 1)
        {
            glfwSetWindowTitle(gGameWnd, std::format("FPS: {:.2f}, frame time: {:.2f} ms", fps, deltaTime * 1000).c_str());
            fpsTimer = 0;
        }

        time += deltaTime;
        fpsTimer += deltaTime;
        frameCounter++;

        glfwPollEvents();
        glfwSwapBuffers(gGameWnd);
    }
}

void gameOnExit()
{
    delete gGame;

    glfwTerminate();
}

int main(const int pArgc, char* pArgv[])
{
    atexit(gameOnExit);

    if (glInit())
    {
        gGame = new Game();

        gameMain(std::vector<std::string>(pArgv, pArgv + pArgc));
        printf("%s", gErrCtx.getMsg().c_str());
    }

    return -gErrCtx.isErr();
}
