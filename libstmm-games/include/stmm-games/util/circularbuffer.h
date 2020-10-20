/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   circularbuffer.h
 */

#ifndef STMG_CIRCULAR_BUFFER_H
#define STMG_CIRCULAR_BUFFER_H


#include <vector>
#include <cassert>
#include <iostream>

#include <stdint.h>

namespace stmg
{

template <class T>
class CircularBuffer
{
public:
	/** Constructor.
	 * @param nSize The capacity of the buffer. Must be positive.
	 */
	CircularBuffer(int32_t nSize) noexcept;

	/** The (fixed) capacity of the buffer.
	 * @return The capacity of the buffer.
	 */
	int32_t capacity() const noexcept;
	/** Clears the buffer.
	 * Removes all its values.
	 */
	void clear() noexcept;
	/** Whether the buffer is empty.
	 * Shortcut for (size() == 0).
	 * @return Whether the buffer is empty.
	 */
	bool isEmpty() const noexcept;
	/** Whether the buffer is full.
	 * Shortcut for (size() == capacity()).
	 * @return Whether buffer is full.
	 */
	bool isFull() const noexcept;
	/** Remove a value from the fifo buffer.
	 * The buffer must not be empty.
	 * @return The read value.
	 */
	T read() noexcept;
	/** Add a value to the buffer.
	 * The buffer must not be full.
	 * @param oT The value to add.
	 */
	void write(const T& oT) noexcept;
	/** Add a value to the buffer.
	 * The buffer must not be full.
	 * @param oT The value to add.
	 */
	void write(T&& oT) noexcept;
	/** Total number of buffered values.
	 * @return Number of stored values. Always &lt;= capacity().
	 */
	int32_t size() const noexcept;
	/** Peek the buffered value.
	 * Index 0 corresponds to the value that would be returned by a call to read().
	 * @param nIdx The index. Must be &gt;= 0 and &lt; size().
	 * @return The buffered value.
	 */
	const T& peekValue(int32_t nIdx) const noexcept;
	/** Peek the buffered value.
	 * Index 0 corresponds to the value that would be returned by a call to read().
	 * @param nIdx The index. Must be &gt;= 0 and &lt; size().
	 * @return The buffered value.
	 */
	T& peekValue(int32_t nIdx) noexcept;

private:
	CircularBuffer() = delete;

	int32_t m_nCount;
	int32_t m_nReadIdx;
	int32_t m_nWriteIdx;
	std::vector<T> m_aT;
};

template <class T>
CircularBuffer<T>::CircularBuffer(int32_t nSize) noexcept
: m_nCount(0)
, m_nReadIdx(0)
, m_nWriteIdx(0)
{
	assert(nSize > 0);
	m_aT.resize(nSize);
}

template <class T>
inline int32_t CircularBuffer<T>::capacity() const noexcept
{
	return static_cast<int32_t>(m_aT.size());
}

template <class T>
inline void CircularBuffer<T>::clear() noexcept
{
	m_nCount = 0;
	m_nReadIdx = 0;
	m_nWriteIdx = 0;
}

template <class T>
inline bool CircularBuffer<T>::isEmpty() const noexcept
{
	return (m_nCount == 0);
}
template <class T>
inline bool CircularBuffer<T>::isFull() const noexcept
{
	return (m_nCount == static_cast<int32_t>(m_aT.size()));
}

template <class T>
T CircularBuffer<T>::read() noexcept
{
	assert(!isEmpty());
	const int32_t nReadIdx = m_nReadIdx;
	++m_nReadIdx;
	if (m_nReadIdx == static_cast<int32_t>(m_aT.size())) {
		m_nReadIdx = 0;
	}
	--m_nCount;
	return std::move(m_aT[nReadIdx]);
}

template <class T>
void CircularBuffer<T>::write(const T& oT) noexcept
{
	assert(!isFull());
	m_aT[m_nWriteIdx] = oT;
	++m_nWriteIdx;
	if (m_nWriteIdx == static_cast<int32_t>(m_aT.size())) {
		m_nWriteIdx = 0;
	}
	++m_nCount;
}
template <class T>
void CircularBuffer<T>::write(T&& oT) noexcept
{
	assert(!isFull());
	m_aT[m_nWriteIdx] = std::move(oT);
	++m_nWriteIdx;
	if (m_nWriteIdx == static_cast<int32_t>(m_aT.size())) {
		m_nWriteIdx = 0;
	}
	++m_nCount;
}

template <class T>
inline int32_t CircularBuffer<T>::size() const noexcept
{
	return m_nCount;
}

template <class T>
const T& CircularBuffer<T>::peekValue(int32_t nIdx) const noexcept
{
	assert(nIdx < m_nCount);
	int32_t nReadIdx = m_nReadIdx + nIdx;
	const int32_t nCapacity = capacity();
	if (nReadIdx >= nCapacity) {
		nReadIdx -= nCapacity;
	}
	return m_aT[nReadIdx];
}
template <class T>
T& CircularBuffer<T>::peekValue(int32_t nIdx) noexcept
{
	assert(nIdx < m_nCount);
	int32_t nReadIdx = m_nReadIdx + nIdx;
	const int32_t nCapacity = capacity();
	if (nReadIdx >= nCapacity) {
		nReadIdx -= nCapacity;
	}
	return m_aT[nReadIdx];
}

} // namespace stmg

#endif	/* STMG_CIRCULAR_BUFFER_H */

