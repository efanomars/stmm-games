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
 * File:   namedindex.h
 */

#ifndef STMG_NAMED_INDEX_H
#define STMG_NAMED_INDEX_H

#include <vector>
#include <string>

#include <stdint.h>

namespace stmg
{

/** Named indexes class.
 * This class allows to uniquely associate a string with a number so that
 * the number can be used instead of the string.
 * Names cannot be removed from this class unless it is assigned (`operator=()`)
 * or NamedIndex::clear() is called.
 * This ensures that the index associated with the string is always `>= 0` and `&lt; size()`.
 */
class NamedIndex
{
public:
	/** Default constructor.
	 */
	NamedIndex() noexcept;
	NamedIndex(const NamedIndex& oSource) noexcept;
	NamedIndex(NamedIndex&& oSource) noexcept;
	/** Returns the number of names. */
	inline int32_t size() const noexcept { return m_nSize; }
	/** Whether an index is valid.
	 * Index is always `>= 0` and `&lt; size()`.
	 */
	inline bool isIndex(int32_t nIndex) const noexcept { return (nIndex >= 0) && (nIndex < m_nSize); }
	/** The name associated with a given index.
	 * @param nIndex The index of the name. Must be valid.
	 * @return The string.
	 */
	const std::string& getName(int32_t nIndex) const noexcept;
	/** Adds a name.
	 * If the name already exists nothing is added and the index of the existing
	 * name is returned.
	 * @param sName The name to be added.
	 * @return The index of the added name.
	 */
	int32_t addName(const std::string& sName) noexcept;
	/** The index of a name.
	 * @param sName The name to lookup.
	 * @return The index or -1 if not found.
	 */
	int32_t getIndex(const std::string& sName) const noexcept;
	/** The first index of a name starting with a certain string.
	 * @param sStartOfName The start of the name to match
	 * @return The index or -1 if not found.
	 */
	int32_t findIndexNameStarts(const std::string& sStartOfName) const noexcept;
#ifndef NDEBUG
	void dump() const noexcept;
#endif //NDEBUG
protected:
	/** The assignment operator.
	 * If you want to allow assignment you have to subclass NamedIndex
	 * and make this function public.
	 */
	NamedIndex& operator=(const NamedIndex& oSource) noexcept;
	NamedIndex& operator=(NamedIndex&& oSource) noexcept;
	/** Clear all names.
	 * If you want to allow assignment you have to subclass NamedIndex
	 * and make this function public.
	 */
	void clear() noexcept;
private:
	int32_t m_nSize;
	std::vector<std::string> m_aName;
};

/** Assignable and clearable named indexes class.
 */
class AssignableNamedIndex : public NamedIndex
{
public:
	using NamedIndex::operator=;
	using NamedIndex::clear;
	//
	AssignableNamedIndex& operator=(const AssignableNamedIndex& oSource) noexcept;
	AssignableNamedIndex& operator=(AssignableNamedIndex&& oSource) noexcept;
};

} // namespace stmg

#endif	/* STMG_NAMED_INDEX_H */

