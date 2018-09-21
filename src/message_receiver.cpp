// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include "message_receiver.h"
#include "message_queue.h"

namespace lumeview {

MessageReceiver::MessageReceiver ()
{
	MessageQueue::add_receiver (this);
}

MessageReceiver::~MessageReceiver ()
{
	MessageQueue::remove_receiver (this);
}

}//	end of namespace lumeview
