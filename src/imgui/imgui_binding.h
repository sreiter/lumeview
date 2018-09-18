// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__lumeview_imgui_binding
#define __H__lumeview_imgui_binding

struct ImDrawData;

namespace lumeview {

class WindowEventListener;

WindowEventListener* ImGui_GetEventListener ();
bool ImGui_Init(const char* glsl_version = nullptr);
void ImGui_SetClipboardCallbacks (	const char* (*getText) (void* userData),
                            		void (*setText) (void* userData, const char* text),
                            		void* userData);
void ImGui_Shutdown();
void ImGui_RenderDrawData(ImDrawData* draw_data);
void ImGui_Display(ImDrawData* draw_data = nullptr);
void ImGui_NewFrame();

}//	end of namespace lumeview

#endif	//__H__lumeview_imgui_binding
