#include <jni.h>
#include <iostream>
#include "jni_event_listener.h"
#include "../renderer.h"

using namespace std;
using namespace slimesh;

static WindowEventListener* g_eventListener = nullptr;

static int MapButton (int btn)
{
    const int mapping[] = {0, 0, 2, 1};
    if(btn > 0 && btn <= 3)
        return mapping[btn];
    return btn;
}

// JNI methods
JNIEXPORT void JNICALL
Java_eu_mihosoft_vrl_v3d_nativeogl_NativeOpenGL_native_1gl_1init(
    JNIEnv *jEnv, jclass jcls)
{
    RendererInit ();
    g_eventListener = RendererGetEventListener ();
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vrl_v3d_nativeogl_NativeOpenGL_native_1gl_1display(
    JNIEnv *jEnv, jclass jcls)
{
    RendererDraw ();
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vrl_v3d_nativeogl_NativeOpenGL_native_1gl_1reshape(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint w, jint h)
{
    if(g_eventListener)
        g_eventListener->set_viewport (glm::ivec4(x, y, w, h));
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vrl_v3d_nativeogl_NativeOpenGL_native_1mouse_1click_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint btn, jint click_count
)
{
}
  
JNIEXPORT void JNICALL
Java_eu_mihosoft_vrl_v3d_nativeogl_NativeOpenGL_native_1gl_1dispose(
    JNIEnv *jEnv, jclass jcls)
{
} 

JNIEXPORT void JNICALL
Java_eu_mihosoft_vrl_v3d_nativeogl_NativeOpenGL_native_1mouse_1move_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y)
{
    if(g_eventListener)
        g_eventListener->mouse_move (glm::vec2(x, y));
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vrl_v3d_nativeogl_NativeOpenGL_native_1mouse_1drag_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint btn)
{
    if(g_eventListener)
        g_eventListener->mouse_move (glm::vec2(x, y));
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vrl_v3d_nativeogl_NativeOpenGL_native_1mouse_1press_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint btn)
{
    if(g_eventListener)
        g_eventListener->mouse_button (MapButton(btn), MouseButtonAction::DOWN, 0);
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vrl_v3d_nativeogl_NativeOpenGL_native_1mouse_1release_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint btn)
{
    if(g_eventListener)
        g_eventListener->mouse_button (MapButton(btn), MouseButtonAction::UP, 0);
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vrl_v3d_nativeogl_NativeOpenGL_native_1mouse_1enter_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y)
{
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vrl_v3d_nativeogl_NativeOpenGL_native_1mouse_1exit_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y)
{
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vrl_v3d_nativeogl_NativeOpenGL_native_1mouse_1wheel_1event(
    JNIEnv *jEnv, jclass jcls, jdouble movement)
{
    if(g_eventListener)
        g_eventListener->mouse_scroll (glm::vec2 (0, movement));
}
