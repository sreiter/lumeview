// This file is part of slimesh, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__slimesh_imgui_binding
#define __H__slimesh_imgui_binding

namespace slimesh {

class WindowEventListener;

WindowEventListener* ImGui_GetEventListener ();
bool ImGui_Init(const char* glsl_version = nullptr);
void ImGui_Shutdown();
void ImGui_RenderDrawData(ImDrawData* draw_data);
void ImGui_NewFrame();

}//	end of namespace slimesh

#endif	//__H__slimesh_imgui_binding
