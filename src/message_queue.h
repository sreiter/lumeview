// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__lumeview_message_queue
#define __H__lumeview_message_queue

#include <memory>
#include <queue>

namespace lumeview {

class Message;
class MessageReceiver;

class MessageQueue {
public:
	static void post (const std::shared_ptr <const Message>& msg);

	template <class TMsg, class... TArgs>
	static void post (TArgs&&... args)
	{
		post (std::make_shared <TMsg> (std::forward<TArgs>(args)...));
	}

	static void dispatch ();

	static void add_receiver (MessageReceiver* rec);

	static void remove_receiver (MessageReceiver* rec);

private:
	MessageQueue ();
	static MessageQueue& inst();
	
	std::queue <std::shared_ptr <const Message>>	m_messages;
	std::vector <MessageReceiver*> m_receivers;
};

}//	end of namespace lumeview

#endif	//__H__lumeview_message_queue
