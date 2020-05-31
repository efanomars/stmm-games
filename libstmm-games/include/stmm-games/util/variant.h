/*
 * File:   variant.h
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

#ifndef STMG_VARIANT_H
#define STMG_VARIANT_H

#include <stdint.h>

namespace stmg
{

class Variant
{
public:
	enum TYPE {
		TYPE_NULL = 0
		, TYPE_INT = 1
		, TYPE_BOOL = 2
		//, TYPE_VECTOR = 10 /**< vector of variants */
	};
	Variant() noexcept
	: m_eType(TYPE_NULL)
	, m_nValue(0)
	{
	}
	explicit Variant(int32_t nValue) noexcept
	: m_eType(TYPE_INT)
	, m_nValue(nValue)
	{
	}
	explicit Variant(bool bValue) noexcept
	: m_eType(TYPE_BOOL)
	, m_nValue(bValue ? 1 : 0)
	{
	}
	//Variant(const std::vector<Variant>& aVariants)
	//: m_eType(TYPE_VECTOR)
	//, m_nValue(0)
	//, m_aVariants(aVariants)
	//{
	//}

	inline bool isNull() const noexcept
	{
		return (m_eType == TYPE_NULL);
	}
	inline Variant::TYPE getType() const noexcept
	{
		return m_eType;
	}

	inline int32_t getInt() const noexcept
	{
		return m_nValue;
	}
	inline bool getBool() const noexcept
	{
		return ((m_nValue != 0) ? true : false);
	}
	//inline const std::vector<Variant>& getVector() const { return m_aVariants; }

	inline void setNull() noexcept
	{
		m_eType = TYPE_NULL;
		m_nValue = 0;
	}
	inline void setInt(int32_t nValue) noexcept
	{
		m_eType = TYPE_INT;
		m_nValue = nValue;
	}
	inline void setBool(bool bValue) noexcept
	{
		m_eType = TYPE_BOOL;
		m_nValue = (bValue ? 1 : 0);
	}
	//inline void setVector(const std::vector<Variant>& aVariants) { m_eType = TYPE_VECTOR; m_aVariants = aVariants; }

	inline bool operator==(const Variant& oOther) const noexcept
	{
		if (m_eType != oOther.m_eType) {
			return false;
		}
		//if (m_eType != TYPE_VECTOR) {
		return (m_nValue == oOther.m_nValue);
		//}
		//return (m_aVariants == oOther.m_aVariants);
	}
	inline bool operator!=(const Variant& oOther) const noexcept
	{
		return !operator==(oOther);
	}
	inline bool operator<(const Variant& oOther) const noexcept
	{
		return (m_nValue < oOther.m_nValue);
	}
	inline bool operator<=(const Variant& oOther) const noexcept
	{
		return (m_nValue <= oOther.m_nValue);
	}
	inline bool operator>(const Variant& oOther) const noexcept
	{
		return (m_nValue > oOther.m_nValue);
	}
	inline bool operator>=(const Variant& oOther) const noexcept
	{
		return (m_nValue >= oOther.m_nValue);
	}
	#ifndef NDEBUG
	void dump(int32_t nIndentSpaces) const noexcept;
	#endif //NDEBUG
private:
	TYPE m_eType;
	int32_t m_nValue;
	//std::vector<Variant> m_aVariants;
};

} // namespace stmg

#endif	/* STMG_VARIANT_H */
