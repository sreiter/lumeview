// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__lumeview_message_receiver
#define __H__lumeview_message_receiver

namespace lumeview {

class Message;

class MessageReceiver {
public:
	MessageReceiver ();
	virtual ~MessageReceiver ();

	virtual void message_dispatch_begins ()		{}
	virtual void message_dispatch_ends ()		{}
	virtual void receive_message (const Message& msg) {}
};

}//	end of namespace lumeview

#endif	//__H__lumeview_message_receiver
