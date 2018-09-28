#include <jni.h>
#include <iostream>
#include "jni_event_listener.h"
#include "../log.h"
#include "../lumeview.h"
#include "../scene_util.h"

using namespace std;
using namespace lumeview;

Lumeview g_lumeview;

static int MapButton (int btn)
{
    const int mapping[] = {0, 0, 2, 1};
    if(btn > 0 && btn <= 3)
        return mapping[btn];
    return btn;
}

// JNI methods
JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1gl_1init(
    JNIEnv *jEnv, jclass jcls)
{
	lumeview::LumeviewInit();
	g_lumeview.set_scene (CreateSampleScene ());
	// you could also create a simple scene with one mesh from a file like this:
	//	g_lumeview.set_scene (CreateSceneForMesh (filename));
}


JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1gl_1dispose(
    JNIEnv *jEnv, jclass jcls)
{
	cout << "DISPOSE\n";
	g_lumeview.clear();
	lumeview::LumeviewShutdown();
} 


JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1gl_1display(
    JNIEnv *jEnv, jclass jcls)
{
	g_lumeview.process_gui ();
	g_lumeview.render ();
}


JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1gl_1reshape(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint w, jint h)
{
    g_lumeview.set_viewport (glm::ivec4(x, y, w, h));

    // think about calling g_lumeview.process_gui() here
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
    g_lumeview.mouse_move (glm::vec2(x, y));

    // think about calling g_lumeview.process_gui() here
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1mouse_1drag_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint btn)
{
    g_lumeview.mouse_move (glm::vec2(x, y));

    // think about calling g_lumeview.process_gui() here
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1mouse_1press_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint btn)
{
    g_lumeview.mouse_button (MapButton(btn), MouseButtonAction::DOWN, 0);

    // think about calling g_lumeview.process_gui() here
}

JNIEXPORT void JNICALL
Java_eu_mihosoft_vnativegl_NativeOpenGL_native_1mouse_1release_1event(
    JNIEnv *jEnv, jclass jcls, jint x, jint y, jint btn)
{
    g_lumeview.mouse_button (MapButton(btn), MouseButtonAction::UP, 0);

    // think about calling g_lumeview.process_gui() here
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
    g_lumeview.mouse_scroll (glm::vec2 (0, movement));

    // think about calling g_lumeview.process_gui() here
}
