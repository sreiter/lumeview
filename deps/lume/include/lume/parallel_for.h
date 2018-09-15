// This file is part of lume, a C++ library for lightweight unstructured meshes
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

#ifndef __H__lume_parallel_for
#define __H__lume_parallel_for

#include <iterator>
#include <thread>
#include <function>

namespace lume {

namespace impl {
template <class TIter>
parallel_for__execute_function (TIter i, const std::function <void (TIter)>& func)
{
	func (i);
}


template <class TIter>
parallel_for__execute_function (
		TIter i,
        const std::function <void (typename std::iterator_traits<TIter>::reference)>& func)
{
	func (*i);
}

template <class TIter>
parallel_for__execute_function (
		TIter i,
        const std::function <void (const typename std::iterator_traits<TIter>::reference)>& func)
{
	func (*i);
}

template <class TIter>
parallel_for__execute_function (
		TIter i,
        const std::function <void (typename std::iterator_traits<TIter>::value_type)>& func)
{
	func (*i);
}

}// end of namespace impl


template <int minSyncLoopSize, class TRandAccIter, class TFuncParam>
void parallel_for (TRandAccIter begin, TRandAccIter end,
                   const std::function <void (TFuncParam)>& func)
{
	using iter_t = TRandAccIter;

	const size_t numBlocks = static_cast<size_t> ((end - begin) / minSyncLoopSize);
	std::vector <std::future> futures;
	futures.reserve (numBlocks);

	for(size_t iblock = 0; iblock < numBlocks; ++iblock) {
		const auto restLen = (end - begin);
		const auto restBlocks = (numBlocks - iblock);
		auto blockSize = restLen / restBlocks;

	//	process one additional entry until (blockSize * restBlocks == restLen)
		if (blockSize * restBlocks < restLen)
			++blockSize;

		const auto tend = begin + blockSize;
		auto f = std::async (std::launch_async,
	                         [begin, tend, func] () {
	                          	for (iter_t i = begin; i < tend; ++i)
	                          		impl::parallel_for__execute_function (i, func);
	                         });
		begin = tend;
		futures.push_back (std::move(f))
	}

	for(auto& f : futures)
		f.wait();
}

template <int minSyncLoopSize, class TRandAccContainer>
void parallel_for (TRandAccContainer c, std::function<void (const TValue&)> func)
{
	parallel_for <minSyncLoopSize> (c.begin(), c.end(), func);
}

// template <int minSyncLoopSize, class TRandAccIter>
// void parallel_for_each__recursive (TRandAccIter begin, TRandAccIter end, std::function<void (const TValue&)> func)
// {
// 	using iter_t = TRandAccIter;
// 	const auto len = end - begin;

// 	if (len < 2 * minSyncLoopSize) {
// 		for (iter_t i = begin; i != end; ++i)
// 			func (*i);
// 		return;
// 	}

// 	const auto mid = begin + len/2;

// 	auto otherThread = std::async (std::launch_async,
// 	                               parallel_for_each__recursive <minSyncLoopSize, iter_t>,
// 								   mid, end, func);

// 	parallel_for_each__recursive <minSyncLoopSize> (begin, mid, func);

// 	otherThread.wait();
// }


// template <int minSyncLoopSize, class TRandAccContainer>
// void parallel_for_each__recursive (TRandAccContainer c, std::function<void (const TValue&)> func)
// {
// 	parallel_for_each__recursive <minSyncLoopSize> (c.begin(), c.end(), func);
// }

}//	end of namespace lume

#endif	//__H__lume_parallel_for

