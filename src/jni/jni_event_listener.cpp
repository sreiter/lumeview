#include <jni.h>
#include <iostream>
#include "jni_event_listener.h"
#include "../renderer.h"
#include "imgui/imgui.h"
#include "../imgui/imgui_binding.h"
#include "../log.h"

using namespace std;
using namespace slimesh;

static WindowEventListener* g_imguiListener = nullptr;
static WindowEventListener* g_renderListener = nullptr;

static int MapButton (int btn)
{
    const int mapping[] = {0, 0, 2, 1};
    if(btn > 0 && btn <= 3)
        return mapping[btn];
    return btn;
}

void InitImGui ()
{
    //  init IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    slimesh::ImGui_Init();
    g_imguiListener = ImGui_GetEventListener ();

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
}

// JNI methods
JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1gl_1init(
    JNIEnv *jEnv, jclass jcls)
{
    RendererInit ();
    g_renderListener = RendererGetEventListener ();
    InitImGui ();
}


JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1gl_1dispose(
    JNIEnv *jEnv, jclass jcls)
{
    RendererDispose ();
    g_renderListener = nullptr;
    slimesh::ImGui_Shutdown ();
    g_imguiListener = nullptr;
} 


JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1gl_1display(
    JNIEnv *jEnv, jclass jcls)
{
    ImGui_NewFrame();
    RendererDraw ();
    RendererProcessGUI (true);
}


JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1gl_1reshape(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint w, jint h)
{
    if(g_imguiListener)
        g_imguiListener->set_viewport (glm::ivec4(x, y, w, h));

    if(g_renderListener)
        g_renderListener->set_viewport (glm::ivec4(x, y, w, h));
}


JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1mouse_1click_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint btn, jint click_count
)
{
}
  
JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1mouse_1move_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y)
{
    if(g_imguiListener)
        g_imguiListener->mouse_move (glm::vec2(x, y));

    if(g_renderListener && !ImGui::GetIO().WantCaptureMouse)
        g_renderListener->mouse_move (glm::vec2(x, y));
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1mouse_1drag_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint btn)
{
    if(g_imguiListener)
        g_imguiListener->mouse_move (glm::vec2(x, y));

    if(g_renderListener && !ImGui::GetIO().WantCaptureMouse)
        g_renderListener->mouse_move (glm::vec2(x, y));
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1mouse_1press_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint btn)
{
    if(g_imguiListener)
        g_imguiListener->mouse_button (MapButton(btn), MouseButtonAction::DOWN, 0);

    if(g_renderListener && !ImGui::GetIO().WantCaptureMouse)
        g_renderListener->mouse_button (MapButton(btn), MouseButtonAction::DOWN, 0);
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1mouse_1release_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint btn)
{
    if(g_imguiListener)
        g_imguiListener->mouse_button (MapButton(btn), MouseButtonAction::UP, 0);

    if(g_renderListener && !ImGui::GetIO().WantCaptureMouse)
        g_renderListener->mouse_button (MapButton(btn), MouseButtonAction::UP, 0);
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1mouse_1enter_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y)
{
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1mouse_1exit_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y)
{
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1mouse_1wheel_1event(
    JNIEnv *jEnv, jclass jcls, jdouble movement)
{
    if(g_imguiListener)
        g_imguiListener->mouse_scroll (glm::vec2 (0, movement));

    if(g_renderListener && !ImGui::GetIO().WantCaptureMouse)
        g_renderListener->mouse_scroll (glm::vec2 (0, movement));
}
