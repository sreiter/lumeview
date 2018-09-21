// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include <algorithm>
#include "message_queue.h"
#include "message.h"
#include "message_receiver.h"

using namespace std;

namespace lumeview {

MessageQueue::MessageQueue ()
{}

MessageQueue& MessageQueue::inst()
{
	static MessageQueue mq;
	return mq;
}

void MessageQueue::post (const std::shared_ptr <const Message>& msg)
{
	inst().m_messages.push (msg);
}

void MessageQueue::dispatch ()
{
	auto& receivers = inst().m_receivers;
	for(auto& receiver : receivers)
		receiver->message_dispatch_begins ();

	auto& messages = inst().m_messages;
	while (!messages.empty()) {
		auto& msg = messages.front();
		for(auto& receiver : receivers)
			receiver->receive_message (*msg);
		messages.pop();
	}

	for(auto& receiver : receivers)
		receiver->message_dispatch_ends ();
}

void MessageQueue::add_receiver (MessageReceiver* rec)
{
	inst().m_receivers.push_back (rec);
}

void MessageQueue::remove_receiver (MessageReceiver* rec)
{
	auto& receivers = inst().m_receivers;
	auto it = find (receivers.begin(), receivers.end(), rec);
	if (it != receivers.end())
		receivers.erase (it);
}

}//	end of namespace lumeview
