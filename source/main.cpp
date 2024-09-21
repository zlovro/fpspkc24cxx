#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <chrono>
#include <numbers>
#include <bitset>

#include <shaders/basic_vtx.hlsl.hpp>
#include <shaders/basic_frag.hlsl.hpp>
#include <error_context.hpp>
#include <variable_bitset.hpp>
#include <math_util.hpp>
#include <windows.h>

#include "constants.hpp"
#include "shaders/shader.hpp"

#ifdef DEBUG
pragma message ( "C Preprocessor got here!" )
#endif

GLFWwindow* gGameWnd;

#define GLFW_FIRST_KEY GLFW_KEY_SPACE
#define GLFW_KEY_COUNT GLFW_KEY_LAST - GLFW_FIRST_KEY + 1

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
    // VariableBitset::test();
    // return;

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    gGameWnd = glfwCreateWindow(videoMode->width, videoMode->height, "FPS PAKRAC 24", nullptr, nullptr);
    if (gGameWnd == nullptr)
    {
        gErrCtx.setErr("Failed to create GLFW window\n");
        return;
    }

    glfwMakeContextCurrent(gGameWnd);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        gErrCtx.setErr("Failed to initialize GLAD");
        return;
    }

    glViewport(0, 0, videoMode->width, videoMode->height);

    glfwSetFramebufferSizeCallback(gGameWnd, [](GLFWwindow*, const int pWidth, const int pHeight)
    {
        glViewport(0, 0, pWidth, pHeight);
    });

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT_FACE);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    auto time = 0.0F;

    u32 vbo;
    glGenBuffers(1, &vbo);

    auto basicShader = Shader(SHADER_VTX_BASIC, SHADER_FRAG_BASIC);
    basicShader.use();

    u32 vao;
    glGenVertexArrays(1, &vao);

    auto           camPos   = glm::vec3(0, 0, 3);
    constexpr auto cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = 0, pitch = 0;
    float fov = 75;

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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 direction;
        direction.x      = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y      = sin(glm::radians(pitch));
        direction.z      = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        auto cameraFront = glm::normalize(direction);
        auto view        = glm::lookAt(camPos, camPos + cameraFront, cameraUp);
        auto pos         = glm::vec3(6, 5 + (sin(time + 2.0F / 3 * PI) + 1) * 2, 0);


        auto posMat = translate(MAT_4_IDENTITY, pos);
        auto model       = glm::rotate(posMat, glm::radians(time * 180), VEC_3_UP);

        int width = 0, height = 0;
        glfwGetFramebufferSize(gGameWnd, &width, &height);
        auto projection = glm::perspective(glm::radians(fov), (float)width / height, 0.1F, 10000.0F);

        basicShader.setMtx4("view", view);
        basicShader.setMtx4("model", model);
        basicShader.setMtx4("projection", projection);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_TRIANGLES, 0, 3);

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
    glfwTerminate();
}

int main(const int pArgc, char* pArgv[])
{
    atexit(gameOnExit);

    gameMain(std::vector<std::string>(pArgv, pArgv + pArgc));
    printf("%s", gErrCtx.getMsg().c_str());

    return -gErrCtx.isErr();
}
