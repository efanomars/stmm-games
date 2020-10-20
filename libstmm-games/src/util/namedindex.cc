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
 * File:   namedindex.cc
 */

#include "util/namedindex.h"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>

namespace stmg
{

NamedIndex::NamedIndex() noexcept
: m_nSize(0)
{
}
NamedIndex::NamedIndex(const NamedIndex& oSource) noexcept
: m_nSize(oSource.m_nSize)
, m_aName(oSource.m_aName)
{
}
NamedIndex::NamedIndex(NamedIndex&& oSource) noexcept
: m_nSize(std::move(oSource.m_nSize))
, m_aName(std::move(oSource.m_aName))
{
}
NamedIndex& NamedIndex::operator=(const NamedIndex& oSource) noexcept
{
	m_nSize = oSource.m_nSize;
	m_aName = oSource.m_aName;
	return *this;
}
NamedIndex& NamedIndex::operator=(NamedIndex&& oSource) noexcept
{
	m_nSize = std::move(oSource.m_nSize);
	m_aName = std::move(oSource.m_aName);
	return *this;
}
void NamedIndex::clear() noexcept
{
	m_nSize = 0;
	m_aName.clear();
}
const std::string& NamedIndex::getName(int32_t nIndex) const noexcept
{
	assert((nIndex >= 0) && (nIndex < m_nSize));
	return m_aName[nIndex];
}
int32_t NamedIndex::addName(const std::string& sName) noexcept
{
	auto itFind = std::find_if(m_aName.begin(), m_aName.end()
						, [&sName](const std::string& s) { return sName == s; });
	if (itFind != m_aName.end()) {
		return std::distance(m_aName.begin(), itFind);
	}
	m_aName.push_back(sName);
	++m_nSize;
	return m_nSize - 1;
}
int32_t NamedIndex::getIndex(const std::string& sName) const noexcept
{
	auto itFind = std::find_if(m_aName.begin(), m_aName.end()
						, [&sName](const std::string& s) { return sName == s; });
	if (itFind != m_aName.end()) {
		return std::distance(m_aName.begin(), itFind);
	}
	return -1;
}
int32_t NamedIndex::findIndexNameStarts(const std::string& sStartOfName) const noexcept
{
	auto itFind = std::find_if(m_aName.begin(), m_aName.end()
						, [&sStartOfName](const std::string& s) { return s.substr(0, sStartOfName.size()) == sStartOfName; });
	if (itFind != m_aName.end()) {
		return std::distance(m_aName.begin(), itFind);
	}
	return -1;
}
#ifndef NDEBUG
void NamedIndex::dump() const noexcept
{
	std::cout << "NamedIndex::dump()" << '\n';
	int32_t nIdx = 0;
	for (auto& sName : m_aName) {
		std::cout << "  [" << nIdx << "]=" << sName << '\n';
		++nIdx;
	}
}
#endif //NDEBUG

AssignableNamedIndex& AssignableNamedIndex::operator=(const AssignableNamedIndex& oSource) noexcept
{
	*this = NamedIndex::operator=(oSource);
	return *this;
}
AssignableNamedIndex& AssignableNamedIndex::operator=(AssignableNamedIndex&& oSource) noexcept
{
	*this = NamedIndex::operator=(std::move(oSource));
	return *this;
}

} // namespace stmg
