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
