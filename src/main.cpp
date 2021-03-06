#pragma warning(disable: 4267)

#include <iomanip>
#include <stdexcept>

#include <Windows.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#include "Camera.h"
#include "DirectInput.h"
#include "Present.h"
#include "Render.h"
#include "Text.h"
#include "Vulkan.h"

using std::exception;
using std::setprecision;
using std::fixed;
using std::runtime_error;
using std::setw;

#define WIN32_CHECK(e, m) if (e != S_OK) throw new std::runtime_error(m)

const int WIDTH = 800;
const int HEIGHT = 800;

const float DELTA_MOVE_PER_S = 200.f;
const float DELTA_ROTATE_PER_S = 3.14f;
const float MOUSE_SENSITIVITY = 0.0001f;
const float JOYSTICK_SENSITIVITY = 0.1f;

bool keyboard[VK_OEM_CLEAR] = {};

VkSurfaceKHR getSurface(
    HWND window,
    HINSTANCE instance,
    const VkInstance& vkInstance
) {
    VkSurfaceKHR surface;

    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hinstance = instance;
    createInfo.hwnd = window;

    auto result = vkCreateWin32SurfaceKHR(
        vkInstance,
        &createInfo,
        nullptr,
        &surface
    );

    if (result != VK_SUCCESS) {
        throw runtime_error("could not create win32 surface");
    } else {
        return surface;
    }
}

LRESULT
WindowProc(
    HWND    window,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
) {
    switch (message) {
        case WM_SIZE:
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) PostQuitMessage(0);
            else keyboard[(uint16_t)wParam] = true;
            break;
        case WM_KEYUP:
            keyboard[(uint16_t)wParam] = false;
            break;
    }
    return DefWindowProc(window, message, wParam, lParam);
}

int
WinMain(
    HINSTANCE instance,
    HINSTANCE prevInstance,
    LPSTR commandLine,
    int showCommand
) {
    LARGE_INTEGER counterFrequency;
    QueryPerformanceFrequency(&counterFrequency);
    
    WNDCLASSEX windowClassProperties = {};
    windowClassProperties.cbSize = sizeof(windowClassProperties);
    windowClassProperties.style = CS_HREDRAW | CS_VREDRAW;
    windowClassProperties.lpfnWndProc = WindowProc;
    windowClassProperties.hInstance = instance;
    windowClassProperties.lpszClassName = "MainWindowClass";
    ATOM windowClass = RegisterClassEx(&windowClassProperties);
    if (!windowClass) {
        LOG(ERROR) << "could not create window class";
    }

    HWND window = CreateWindowEx(
        0,
        "MainWindowClass",
        "crispy-carnival",
        WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WIDTH,
        HEIGHT,
        NULL,
        NULL,
        instance,
        NULL
    );
    if (window == NULL) {
        LOG(ERROR) << "could not create window";
    }

    SetWindowPos(
        window,
        HWND_TOP,
        0,
        0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        SWP_FRAMECHANGED
    );
    ShowCursor(FALSE);

    Vulkan vk;
    vk.extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    createVKInstance(vk);
    vk.swap.surface = getSurface(window, instance, vk.handle);
    initVK(vk);

    vector<vector<VkCommandBuffer>> cmdss(2);
    recordCommandBuffers(vk, cmdss[0]);

    int errorCode = 0;

    Camera camera;
    camera.setFOV(45);
    camera.setAR(vk.swap.extent.width, vk.swap.extent.height);
    camera.nearz = 1.f;
    camera.farz = 1000000.f;

    DirectInput directInput(instance);
    Controller* controller = directInput.controller;
    Mouse* mouse = directInput.mouse;

    LARGE_INTEGER frameStart = {}, frameEnd = {};
    int64_t frameDelta = {};
    float fps = 0;
    float frameTime = 0;

    BOOL done = false;
    while (!done) {
        MSG msg;
        BOOL messageAvailable; 
        do {
            messageAvailable = PeekMessage(
                &msg,
                (HWND)nullptr,
                0, 0,
                PM_REMOVE
            );
            TranslateMessage(&msg); 
            if (msg.message == WM_QUIT) {
                done = true;
                errorCode = (int)msg.wParam;
            }
            DispatchMessage(&msg); 
        } while(!done && messageAvailable);

        if (!done) {
            char cameraDebugString[256];
            camera.getDebugString(cameraDebugString);

            char buffer[256];
            sprintf_s(buffer, "%.2f FPS", fps);

            char debugString[1024];
            snprintf(debugString, 1024, "%s\n%s", cameraDebugString, buffer);

            recordTextCommandBuffers(vk, cmdss[1], debugString);

            QueryPerformanceCounter(&frameStart);
                auto mvp = camera.get();
                updateMVP(vk, &mvp, sizeof(mvp));
                present(vk, cmdss);
            QueryPerformanceCounter(&frameEnd);
            // SetWindowText(window, buffer);
            frameDelta = frameEnd.QuadPart - frameStart.QuadPart;
            frameTime = (float)frameDelta / counterFrequency.QuadPart;
            camera.tick(frameTime);
            fps = counterFrequency.QuadPart / (float)frameDelta;

            float deltaMove = DELTA_MOVE_PER_S * frameTime;
            float deltaRotate = DELTA_ROTATE_PER_S * frameTime;
            if (keyboard['W']) {
                camera.forward(deltaMove);
            }
            if (keyboard['S']) {
                camera.back(deltaMove);
            }
            if (keyboard['A']) {
                camera.left(deltaMove);
            }
            if (keyboard['D']) {
                camera.right(deltaMove);
            }
            if (keyboard['E']) {
                camera.rotateZ(deltaRotate);
            }
            if (keyboard['Q']) {
                camera.rotateZ(-deltaRotate);
            }
            if (keyboard['C']) {
                camera.stop();
            }
            if (keyboard['F']) {
                SetWindowPos(
                    window,
                    HWND_TOP,
                    0,
                    0,
                    WIDTH,
                    HEIGHT,
                    SWP_FRAMECHANGED
                );
            }

            float deltaMouseRotate =
                MOUSE_SENSITIVITY;
            auto mouseDelta = mouse->getDelta();

            camera.rotateY((float)mouseDelta.x * deltaMouseRotate);
            camera.rotateX((float)-mouseDelta.y * deltaMouseRotate);

            float deltaJoystickRotate =
                DELTA_ROTATE_PER_S * frameTime * JOYSTICK_SENSITIVITY;
            if (controller) {
                auto state = controller->getState();

                camera.rotateY(state.rX * deltaJoystickRotate);
                camera.rotateX(-state.rY * deltaJoystickRotate);
                camera.right(state.x * deltaMove);
                camera.forward(-state.y * deltaMove);
            }

            resetTextCommandBuffers(vk, cmdss[1]);
        }
    } 

    return errorCode; 
}
