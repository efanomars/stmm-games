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
 * File:   variantset.h
 */

#ifndef STMG_VARIANT_SET_H
#define STMG_VARIANT_SET_H

#include "util/variant.h"

#include <vector>
#include <utility>

#include <stdint.h>

namespace stmg
{

/** Smallish set of variants.
 */
class VariantSet
{
public:
	/** Constructs empty variant set.
	 */
	VariantSet() noexcept
	: m_oFromValue()
	, m_oToValue()
	{
	}
	/** Constructs variant set with one value.
	 * @param oValue The value. Cannot be null.
	 */
	explicit VariantSet(const Variant& oValue) noexcept;
	/** Constructs variant set with ordered value range.
	 * The two values must have same type and not be null.
	 * Parameter oFromValue can be &gt; oToValue.
	 * @param oFromValue The start value.
	 * @param oToValue The end value.
	 */
	VariantSet(const Variant& oFromValue, const Variant& oToValue) noexcept;
	/** Constructs variant set with array of unsorted values.
	 * If values are repeated they are ignored.
	 * All the values must have same type and not be null.
	 * @param aValues The values.
	 */
	explicit VariantSet(const std::vector<Variant>& aValues) noexcept;
	/** Constructs variant set with array of values.
	 * If values are repeated they are ignored.
	 * All the values must have same type and not be null.
	 * @param aValues The values.
	 * @param bIsSorted Whether the values are already sorted.
	 */
	explicit VariantSet(const std::vector<Variant>& aValues, bool bIsSorted) noexcept;
	/** Constructs variant set with array of unsorted values.
	 * If values are repeated they are ignored.
	 * All the values must have same type and not be null.
	 * @param aValues The values.
	 */
	explicit VariantSet(std::vector<Variant>&& aValues) noexcept;
	/** Constructs variant set with array of values.
	 * If values are repeated they are ignored.
	 * All the values must have same type and not be null.
	 * @param aValues The values.
	 * @param bIsSorted Whether the values are already sorted.
	 */
	explicit VariantSet(std::vector<Variant>&& aValues, bool bIsSorted) noexcept;

	VariantSet(const VariantSet& oSource) noexcept
	: m_oFromValue(oSource.m_oFromValue)
	, m_oToValue(oSource.m_oToValue)
	, m_aValues(oSource.m_aValues)
	{
	}
	VariantSet(VariantSet&& oSource) noexcept
	: m_oFromValue(std::move(oSource.m_oFromValue))
	, m_oToValue(std::move(oSource.m_oToValue))
	, m_aValues(std::move(oSource.m_aValues))
	{
	}
	VariantSet& operator=(const VariantSet& oSource) noexcept
	{
		m_oFromValue = oSource.m_oFromValue;
		m_oToValue = oSource.m_oToValue;
		m_aValues = oSource.m_aValues;
		return *this;
	}
	VariantSet& operator=(VariantSet&& oSource) noexcept
	{
		m_oFromValue = std::move(oSource.m_oFromValue);
		m_oToValue = std::move(oSource.m_oToValue);
		m_aValues = std::move(oSource.m_aValues);
		return *this;
	}
	/** The number of elements.
	 * @return The size. Is non negative.
	 */
	int32_t size() const noexcept;

	/** The value is part of the set.
	 * @param oValue The index. Must be &gt;= 0 and &lt; size().
	 * @return Whether set contains the value.
	 */
	bool contains(const Variant& oValue) const noexcept;

	#ifndef NDEBUG
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept;
	#endif //NDEBUG

private:
	Variant m_oFromValue;
	Variant m_oToValue;
	std::vector<Variant> m_aValues;
};


} // namespace stmg

#endif	/* STMG_VARIANT_SET_H */

