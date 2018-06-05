// This file is part of slimesh, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__slimesh_data_buffer
#define __H__slimesh_data_buffer

#include <memory>
#include <vector>
#include "unpack.h"
#include "types.h"

namespace slimesh {

template <class T>
class DataBuffer {
public:
	using value_type = T;
	using value_t = value_type;
	using size_type = index_t;

	DataBuffer ()	: m_tupleSize (1) {}
	DataBuffer (const index_t tupleSize) : m_tupleSize (tupleSize) {}

	/// total number of entries, counting individual components
	index_t size () const			{return static_cast<index_t>(m_data.size());}

	index_t num_tuples () const		{return size() / tuple_size();}

	/// number of individual components making up a tuple
	index_t tuple_size () const				{return m_tupleSize;}
	void set_tuple_size (const index_t ts)	{m_tupleSize = ts;}

	T* raw_data ()					{return &m_data[0];}
	const T* raw_data () const		{return &m_data[0];}

	std::vector <T>& data ()				{return m_data;}
	const std::vector <T>& data () const	{return m_data;}

private:
	std::vector <T>	m_data;
	index_t			m_tupleSize;
};


using RealDataBuffer		= DataBuffer <real_t>;
using IndexDataBuffer	= DataBuffer <index_t>;

using SPRealDataBuffer	= std::shared_ptr <RealDataBuffer>;
using SPIndexDataBuffer	= std::shared_ptr <IndexDataBuffer>;
using SPCRealDataBuffer	= std::shared_ptr <const RealDataBuffer>;
using SPCIndexDataBuffer	= std::shared_ptr <const IndexDataBuffer>;

}//	end of namespace slimesh

#endif	//__H__slimesh_data_buffer
