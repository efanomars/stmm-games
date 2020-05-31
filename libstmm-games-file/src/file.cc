/*
 * File:   file.cc
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

#include "file.h"

#include <cassert>
//#include <iostream>


namespace stmg
{

File::File() noexcept
: m_p0Buffer(nullptr)
, m_nBufferSize(0)
{
}
File::File(const std::string& sFullPath) noexcept
: m_sFullPath(sFullPath)
, m_p0Buffer(nullptr)
, m_nBufferSize(0)
{
	assert(!sFullPath.empty());
}
File::File(uint8_t* p0Buffer, int32_t nBufferSize) noexcept
: m_p0Buffer(p0Buffer)
, m_nBufferSize(nBufferSize)
{
	assert(nBufferSize >= 0);
	assert(p0Buffer != nullptr);
}
File::File(const File& oSource) noexcept
: m_sFullPath(oSource.m_sFullPath)
, m_p0Buffer(oSource.m_p0Buffer)
, m_nBufferSize(oSource.m_nBufferSize)
{
}
File& File::operator=(const File& oSource) noexcept
{
	m_p0Buffer = oSource.m_p0Buffer;
	m_nBufferSize = oSource.m_nBufferSize;
	m_sFullPath = oSource.m_sFullPath;
	return *this;
}
bool File::isDefined() const noexcept
{
	return ((m_p0Buffer != nullptr) || !m_sFullPath.empty());
}
bool File::isBuffered() const noexcept
{
	return (m_p0Buffer != nullptr);
}
uint8_t const* File::getBuffer() const noexcept
{
	return m_p0Buffer;
}
uint32_t File::getBufferSize() const noexcept
{
	return m_nBufferSize;
}
const std::string& File::getFullPath() const noexcept
{
	return m_sFullPath;
}

} // namespace stmg
