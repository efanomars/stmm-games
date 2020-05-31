/*
 * File:   xybuffer.h
 *
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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

#ifndef STMG_XYBUFFER_H
#define STMG_XYBUFFER_H

#include "basictypes.h"

#include <cassert>
#include <vector>

#include <stdint.h>

namespace stmg
{

template <class T>
class XYBuffer
{
public:
	/** Constructor.
	 * T must be default constructible.
	 * @param oWH The size of the buffer. Must be positive.
	 */
	explicit XYBuffer(const NSize& oWH) noexcept;
	/** Constructor.
	 * T must be copy constructible.
	 * @param oWH The size of the buffer. Must be positive.
	 * @param oT The value with which the buffer is filled.
	 */
	XYBuffer(const NSize& oWH, const T& oT) noexcept;
	
	void resize(const NSize& oWH) noexcept;
	void resize(const NSize& oWH, const T& oT) noexcept;
	inline int32_t getW() const noexcept
	{
		return m_oWH.m_nW;
	}
	inline int32_t getH() const noexcept
	{
		return m_oWH.m_nH;
	}
	inline NSize getSize() const noexcept
	{
		return m_oWH;
	}
	const T& get(const NPoint& oXY) const noexcept;
	T& get(const NPoint& oXY) noexcept;
	void set(const NPoint& oXY, const T& oT) noexcept;
	void setAll(const T& oT) noexcept;

private:
	XYBuffer() noexcept;
	inline int32_t idx(int32_t nX, int32_t nY) const noexcept
	{
		return nX + m_oWH.m_nW * nY;
	}

	NSize m_oWH;
	std::vector<T> m_aT;
};

template <class T>
XYBuffer<T>::XYBuffer(const NSize& oWH) noexcept
: m_oWH(oWH)
, m_aT(oWH.m_nW * oWH.m_nH)
{
	assert((oWH.m_nW > 0) && (oWH.m_nH > 0));
}
template <class T>
XYBuffer<T>::XYBuffer(const NSize& oWH, const T& oT) noexcept
: m_oWH(oWH)
{
	assert((oWH.m_nW > 0) && (oWH.m_nH > 0));
	m_aT.resize(oWH.m_nW * oWH.m_nH, oT);
}

template <class T>
void XYBuffer<T>::resize(const NSize& oWH) noexcept
{
	assert((oWH.m_nW > 0) && (oWH.m_nH > 0));
	m_oWH = oWH;
	const int32_t nSize = static_cast<int32_t>(m_aT.size());
	const int32_t nNewSize = oWH.m_nW * oWH.m_nH;
	if (nNewSize > nSize) {
		m_aT.resize(nNewSize);
	}
}
template <class T>
void XYBuffer<T>::resize(const NSize& oWH, const T& oT) noexcept
{
	assert((oWH.m_nW > 0) && (oWH.m_nH > 0));
	m_oWH = oWH;
	const int32_t nSize = static_cast<int32_t>(m_aT.size());
	const int32_t nNewSize = oWH.m_nW * oWH.m_nH;
	if (nNewSize > nSize) {
		m_aT.resize(nNewSize, oT);
	}
}

template <class T>
const T& XYBuffer<T>::get(const NPoint& oXY) const noexcept
{
	assert((oXY.m_nX >= 0) && (oXY.m_nX < m_oWH.m_nW) && (oXY.m_nY >= 0) && (oXY.m_nY < m_oWH.m_nH));
	return m_aT[idx(oXY.m_nX, oXY.m_nY)];
}
template <class T>
T& XYBuffer<T>::get(const NPoint& oXY) noexcept
{
	assert((oXY.m_nX >= 0) && (oXY.m_nX < m_oWH.m_nW) && (oXY.m_nY >= 0) && (oXY.m_nY < m_oWH.m_nH));
	return m_aT[idx(oXY.m_nX, oXY.m_nY)];
}

template <class T>
void XYBuffer<T>::set(const NPoint& oXY, const T& oT) noexcept
{
	assert((oXY.m_nX >= 0) && (oXY.m_nX < m_oWH.m_nW) && (oXY.m_nY >= 0) && (oXY.m_nY < m_oWH.m_nH));
	m_aT[idx(oXY.m_nX, oXY.m_nY)] = oT;
}
template <class T>
void XYBuffer<T>::setAll(const T& oT) noexcept
{
	const int32_t nTotIdx = m_oWH.m_nW * m_oWH.m_nH;
	for (int32_t nIdx = 0; nIdx < nTotIdx; ++nIdx) {
		m_aT[nIdx] = oT;
	}
}

} // namespace stmg

#endif	/* STMG_XYBUFFER_H */
