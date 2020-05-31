/*
 * File:   namedobjindex.h
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

#ifndef STMG_NAMED_OBJ_INDEX_H
#define STMG_NAMED_OBJ_INDEX_H

#include <cassert>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <utility>

#include <stdint.h>

namespace stmg
{

/** Named objects indexed template class.
 * This class allows to uniquely associate a string with a number so
 * that the number can be used to get the object instead of the string.
 *
 * Uniqueness of the stored objects is not enforced. The requirements to
 * objects (the template parameter class) is the same as for the objects in a std::vector.
 */
template <class T>
class NamedObjIndex
{
public:
	/** Constructor.
	 */
	NamedObjIndex() noexcept;
	NamedObjIndex(const NamedObjIndex<T>& oSource) noexcept;
	NamedObjIndex(NamedObjIndex<T>&& oSource) noexcept;
	/** The number names in the named object index.
	 * @return The size.
	 */
	inline int32_t size() const noexcept;
	/** Whether an index is associated with a named object.
	 * @param nIndex The index.
	 * @return Whether it is valid.
	 */
	inline bool isIndex(int32_t nIndex) const noexcept;
	/** Get the name associated with an index.
	 * @param nIndex The index. Must be &gt;= 0 and &lt; size().
	 * @return The name. Is not empty.
	 */
	const std::string& getName(int32_t nIndex) const noexcept;
	/** Get the object associated with the index.
	 * @param nIndex The index. Must be &gt;= 0 and &lt; size().
	 * @return The object.
	 */
	const T& getObj(int32_t nIndex) const noexcept;
	/** Add a named object.
	 * If the name already exists the object is not overwritten. To do so use
	 * getModifiableObj().
	 * @param sName The name. Cannot be empty.
	 * @param oT The object.
	 * @return The associated index. Is &gt;= 0.
	 */
	int32_t addNamedObj(const std::string& sName, const T& oT) noexcept;
	/** Add a named object.
	 * If the name already exists the object is not overwritten. To do so use
	 * getModifiableObj().
	 * @param sName The name. Cannot be empty.
	 * @param oT The object.
	 * @return The associated index. Is &gt;= 0.
	 */
	int32_t addNamedObj(std::string&& sName, T&& oT) noexcept;
	/** The index associated with a name.
	 * @param sName The name. Cannot be empty.
	 * @return The index or -1 if name not found.
	 */
	int32_t getIndex(const std::string& sName) const noexcept;
	/** Find the first index with a name that starts with a string.
	 * @param sStartOfName The starting string. Cannot be empty.
	 * @return The index or -1 if name not found.
	 */
	int32_t findIndexNameStarts(const std::string& sStartOfName) const noexcept;
protected:
	// If you want to allow assignment you have to subclass NamedObjIndex<T>
	NamedObjIndex<T>& operator=(const NamedObjIndex<T>& oSource) noexcept;
	NamedObjIndex<T>& operator=(NamedObjIndex<T>&& oSource) noexcept;
	/** Get the modifiable object associated with the index.
	 * If you need this function to be public use class AssignableNamedObjIndex.
	 * @param nIndex The index. Must be &gt;= 0 and &lt; size().
	 * @return The object reference.
	 */
	T& getModifiableObj(int32_t nIndex) noexcept;
	/** Clear all objects.
	 */
	void clear() noexcept;
private:
	int32_t m_nSize;
	std::vector<std::string> m_aName; // Size: m_nSize
	std::vector<T> m_aObj; // Size: m_nSize
};

////////////////////////////////////////////////////////////////////////////////
template <class T>
class AssignableNamedObjIndex : public NamedObjIndex<T>
{
public:
	AssignableNamedObjIndex() noexcept;
	AssignableNamedObjIndex(const AssignableNamedObjIndex<T>& oSource) noexcept;
	AssignableNamedObjIndex(AssignableNamedObjIndex<T>&& oSource) noexcept;

	using NamedObjIndex<T>::operator=;
	using NamedObjIndex<T>::getModifiableObj;
	using NamedObjIndex<T>::clear;
	//
	AssignableNamedObjIndex& operator=(const AssignableNamedObjIndex& oSource) noexcept;
	AssignableNamedObjIndex& operator=(AssignableNamedObjIndex&& oSource) noexcept;
};

////////////////////////////////////////////////////////////////////////////////
template <class T>
NamedObjIndex<T>::NamedObjIndex() noexcept
: m_nSize(0)
{
}
template <class T>
NamedObjIndex<T>::NamedObjIndex(const NamedObjIndex<T>& oSource) noexcept
: m_nSize(oSource.m_nSize)
, m_aName(oSource.m_aName)
, m_aObj(oSource.m_aObj)
{
}
template <class T>
NamedObjIndex<T>::NamedObjIndex(NamedObjIndex<T>&& oSource) noexcept
: m_nSize(std::move(oSource.m_nSize))
, m_aName(std::move(oSource.m_aName))
, m_aObj(std::move(oSource.m_aObj))
{
}
template <class T>
NamedObjIndex<T>& NamedObjIndex<T>::operator=(const NamedObjIndex<T>& oSource) noexcept
{
	m_nSize = oSource.m_nSize;
	m_aName = oSource.m_aName;
	m_aObj = oSource.m_aObj;
	return *this;
}
template <class T>
NamedObjIndex<T>& NamedObjIndex<T>::operator=(NamedObjIndex<T>&& oSource) noexcept
{
	m_nSize = std::move(oSource.m_nSize);
	m_aName = std::move(oSource.m_aName);
	m_aObj = std::move(oSource.m_aObj);
	return *this;
}
template <class T>
void NamedObjIndex<T>::clear() noexcept
{
	m_nSize = 0;
	m_aName.clear();
	m_aObj.clear();
}

template <class T>
inline int32_t NamedObjIndex<T>::size() const noexcept
{
	return m_nSize;
}
template <class T>
inline bool NamedObjIndex<T>::isIndex(int32_t nIndex) const noexcept
{
	return (nIndex >= 0) && (nIndex < m_nSize);
}
template <class T>
const std::string& NamedObjIndex<T>::getName(int32_t nIndex) const noexcept
{
	assert((nIndex >= 0) && (nIndex < m_nSize));
	return m_aName[nIndex];
}
template <class T>
const T& NamedObjIndex<T>::getObj(int32_t nIndex) const noexcept
{
	assert((nIndex >= 0) && (nIndex < m_nSize));
	return m_aObj[nIndex];
}
template <class T>
T& NamedObjIndex<T>::getModifiableObj(int32_t nIndex) noexcept
{
	assert((nIndex >= 0) && (nIndex < m_nSize));
	return m_aObj[nIndex];
}
template <class T>
int32_t NamedObjIndex<T>::addNamedObj(const std::string& sName, const T& oT) noexcept
{
	auto itFind = std::find_if(m_aName.begin(), m_aName.end()
						, [&sName](const std::string& s) { return sName == s; });
	if (itFind != m_aName.end()) {
		return std::distance(m_aName.begin(), itFind);
	}
	m_aName.resize(m_nSize + 1);
	m_aName[m_nSize] = sName;
	m_aObj.resize(m_nSize + 1);
	m_aObj[m_nSize] = oT;
	++m_nSize;
	return m_nSize - 1;
}
template <class T>
int32_t NamedObjIndex<T>::addNamedObj(std::string&& sName, T&& oT) noexcept
{
	auto itFind = std::find_if(m_aName.begin(), m_aName.end()
						, [&sName](const std::string& s) { return sName == s; });
	if (itFind != m_aName.end()) {
		return std::distance(m_aName.begin(), itFind);
	}
	m_aName.resize(m_nSize + 1);
	m_aName[m_nSize] = std::move(sName);
	m_aObj.resize(m_nSize + 1);
	m_aObj[m_nSize] = std::move(oT);
	++m_nSize;
	return m_nSize - 1;
}
template <class T>
int32_t NamedObjIndex<T>::getIndex(const std::string& sName) const noexcept
{
	auto itFind = std::find_if(m_aName.begin(), m_aName.end()
						, [&sName](const std::string& s) { return sName == s; });
	if (itFind != m_aName.end()) {
		return std::distance(m_aName.begin(), itFind);
	}
	return -1;
}

template <class T>
int32_t NamedObjIndex<T>::findIndexNameStarts(const std::string& sStartOfName) const noexcept
{
	auto itFind = std::find_if(m_aName.begin(), m_aName.end()
						, [&sStartOfName](const std::string& s) { return s.substr(0, sStartOfName.size()) == sStartOfName; });
	if (itFind != m_aName.end()) {
		return std::distance(m_aName.begin(), itFind);
	}
	return -1;
}

template <class T>
AssignableNamedObjIndex<T>::AssignableNamedObjIndex() noexcept
: NamedObjIndex<T>::NamedObjIndex()
{
}
template <class T>
AssignableNamedObjIndex<T>::AssignableNamedObjIndex(const AssignableNamedObjIndex<T>& oSource) noexcept
: NamedObjIndex<T>::NamedObjIndex(oSource)
{
}
template <class T>
AssignableNamedObjIndex<T>::AssignableNamedObjIndex(AssignableNamedObjIndex<T>&& oSource) noexcept
: NamedObjIndex<T>::NamedObjIndex(oSource)
{
}
template <class T>
AssignableNamedObjIndex<T>& AssignableNamedObjIndex<T>::operator=(const AssignableNamedObjIndex& oSource) noexcept
{
	*this = NamedObjIndex<T>::operator=(oSource);
	return *this;
}
template <class T>
AssignableNamedObjIndex<T>& AssignableNamedObjIndex<T>::operator=(AssignableNamedObjIndex&& oSource) noexcept
{
	*this = NamedObjIndex<T>::operator=(oSource);
	return *this;
}


} // namespace stmg

#endif	/* STMG_NAMED_OBJ_INDEX_H */

