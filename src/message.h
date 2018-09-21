// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__lumeview_message
#define __H__lumeview_message

namespace lumeview {

class Message {
public:
	virtual ~Message ()	{}

	///	returns a pointer to the subject of the message or nullptr
	/** A MessageReceiver may use the subject to quickly dismiss a message.*/
	virtual const void* subject () const	{return nullptr;}
};

}//	end of namespace lumeview

#endif	//__H__lumeview_message
