// This file is part of lumeview, a lightweight viewer for unstructured meshes
//
// Copyright (C) 2018 Sebastian Reiter
// Copyright (C) 2018 G-CSC, Goethe University Frankfurt
// Author: Sebastian Reiter <s.b.reiter@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
