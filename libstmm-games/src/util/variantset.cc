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
 * File:   variantset.cc
 */

#include "util/variantset.h"

#include <algorithm>
#include <iostream>
#include <cassert>
#include <iterator>
#include <memory>
#include <string>

namespace stmg
{

int32_t VariantSet::size() const noexcept
{
//std::cout << "VariantSet::size()" << '\n';
//dump(0, true);
	if (m_oFromValue.isNull()) {
		return static_cast<int32_t>(m_aValues.size());
	}
	return m_oToValue.getInt() - m_oFromValue.getInt() + 1;
}

VariantSet::VariantSet(const Variant& oValue) noexcept
: m_oFromValue(oValue)
, m_oToValue(oValue)
{
	assert(! m_oFromValue.isNull());
//std::cout << "VariantSet::VariantSet single" << '\n';
}

VariantSet::VariantSet(const Variant& oFromValue, const Variant& oToValue) noexcept
: m_oFromValue(oFromValue)
, m_oToValue(oToValue)
{
//std::cout << "VariantSet::VariantSet from-to" << '\n';
	assert(! oFromValue.isNull());
	assert(oFromValue.getType() == oToValue.getType());
	if (oFromValue > oToValue) {
		std::swap(m_oFromValue, m_oToValue);
	}
}

VariantSet::VariantSet(const std::vector<Variant>& aValues) noexcept
: VariantSet(std::vector<Variant>{aValues})
{
}
VariantSet::VariantSet(const std::vector<Variant>& aValues, bool bIsSorted) noexcept
: VariantSet(std::vector<Variant>{aValues}, bIsSorted)
{
}
VariantSet::VariantSet(std::vector<Variant>&& aValues) noexcept
: VariantSet(std::move(aValues), false)
{
}
VariantSet::VariantSet(std::vector<Variant>&& aValues, bool bIsSorted) noexcept
: m_oFromValue()
, m_oToValue()
, m_aValues(std::move(aValues))
{
//std::cout << "VariantSet::VariantSet multi bIsSorted=" << bIsSorted << '\n';
	const auto nSize = m_aValues.size();
	if (nSize == 0) {
	} else if (nSize == 1) {
		m_oFromValue = std::move(m_aValues[0]);
		m_aValues.clear();
		m_oToValue = m_oFromValue;
		assert(! m_oFromValue.isNull());
	} else {
		if (! bIsSorted) {
			std::sort(m_aValues.begin(), m_aValues.end());
		} else {
			assert(std::is_sorted(m_aValues.begin(), m_aValues.end()));
		}
		m_aValues.erase(std::unique(m_aValues.begin(), m_aValues.end()), m_aValues.end());
		//
		#ifndef NDEBUG
		const Variant::TYPE eType = m_aValues[0].getType();
		assert(eType != Variant::TYPE_NULL);
		std::for_each(std::next(m_aValues.begin()), m_aValues.end(), [&](const Variant& oValue)
		{
			assert(oValue.getType() == eType);
		});
		#endif //NDEBUG
	}
}
bool VariantSet::contains(const Variant& oValue) const noexcept
{
	if (m_oFromValue.isNull()) {
		return std::binary_search(m_aValues.begin(), m_aValues.end(), oValue);
	} else {
		return oValue.operator>=(m_oFromValue) && oValue.operator<=(m_oToValue);
	}
}

#ifndef NDEBUG
void VariantSet::dump(int32_t nIndentSpaces, bool bHeader) const noexcept
{
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent;
	if (bHeader) {
		std::cout << "VariantSet";
	}
	std::cout << "(" << '\n';
	if (m_oFromValue.isNull()) {
		bool bFirst = true;
		for (const auto& oVal : m_aValues) {
			std::cout << sIndent;
			if (!bFirst) {
				std::cout << ", ";
			} else {
				std::cout << " ";
				bFirst = false;
			}
			oVal.dump(0);
		}
	} else {
		std::cout << sIndent;
		std::cout << "m_oFromValue=";
		m_oFromValue.dump(0);
		std::cout << sIndent;
		std::cout << "m_oToValue=";
		m_oToValue.dump(0);
	}
	std::cout << sIndent;
	std::cout << ")" << '\n';
}
#endif //NDEBUG

} // namespace stmg
