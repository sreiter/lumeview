#ifndef __H__renderer
#define __H__renderer

#include "window_event_listener.h"

namespace lumeview {

WindowEventListener* RendererGetEventListener ();
void RendererInit ();
void RendererDispose ();
void RendererDraw ();
void RendererProcessGUI (bool draw);

}// end of namespace lumeview

#endif	//__H__renderer
