// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__lumeview_imgui_binding
#define __H__lumeview_imgui_binding

namespace lumeview {

class WindowEventListener;

WindowEventListener* ImGui_GetEventListener ();
bool ImGui_Init(const char* glsl_version = nullptr);
void ImGui_Shutdown();
void ImGui_RenderDrawData(ImDrawData* draw_data);
void ImGui_NewFrame();

}//	end of namespace lumeview

#endif	//__H__lumeview_imgui_binding
