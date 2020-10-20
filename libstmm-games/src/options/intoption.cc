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
 * File:   intoption.cc
 */

#include "options/intoption.h"

#include "util/util.h"
#include "util/variant.h"

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace stmg
{

IntOption::IntOption(OwnerType eOwnerType, const std::string& sName, int32_t nDefaultValue, const std::string& sDesc
					, int32_t nMin, int32_t nMax) noexcept
: IntOption(eOwnerType, sName, nDefaultValue, sDesc, false, true, {}, {}, nMin, nMax)
{
}
IntOption::IntOption(OwnerType eOwnerType, const std::string& sName, int32_t nDefaultValue, const std::string& sDesc
					, bool bReadOnly, bool bVisible
					, const shared_ptr<Option>& refMaster, const std::vector<Variant>& aMasterValues
					, int32_t nMin, int32_t nMax) noexcept
: Option(eOwnerType, sName, Variant{nDefaultValue}, sDesc, bReadOnly, bVisible, refMaster, aMasterValues)
, m_nMin(nMin)
, m_nMax(nMax)
{
	assert(nMin <= nMax);
	assert(nDefaultValue >= nMin);
	assert(nDefaultValue <= nMax);
}
IntOption::IntOption(OwnerType eOwnerType, const std::string& sName, int32_t nDefaultValue, const std::string& sDesc
					, bool bReadOnly, bool bVisible
					, const std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>>& aMastersValues
					, int32_t nMin, int32_t nMax) noexcept
: Option(eOwnerType, sName, Variant{nDefaultValue}, sDesc, bReadOnly, bVisible, aMastersValues)
, m_nMin(nMin)
, m_nMax(nMax)
{
	assert(nMin <= nMax);
	assert(nDefaultValue >= nMin);
	assert(nDefaultValue <= nMax);
}
bool IntOption::isValidValue(const Variant& oValue) const noexcept
{
	if (oValue.getType() != Variant::TYPE_INT) {
		return false;
	}
	const int32_t nValue = oValue.getInt();
	return (nValue >= m_nMin) && (nValue <= m_nMax);
}
std::string IntOption::getValueAsCode(const Variant& oValue) const noexcept
{
	if (! isValidValue(oValue)) {
		return "";
	}
	const int32_t nValue = oValue.getInt();
	if (nValue >= 0) {
		return std::to_string(nValue);
	} else {
		return "m" + std::to_string(- nValue);
	}
}
std::pair<Variant, std::string> IntOption::getValueFromCode(const std::string& sCode) const noexcept
{
	if (sCode.empty() || Util::strContainsWhitespace(sCode)) {
		return std::make_pair(Variant{}, "Code cannot contain empty spaces or be empty!");
	}
	const bool bNeg = (sCode[0] == 'm');
	const int32_t nIdx = (bNeg ? 1 : 0);
	auto oPairInt = Util::strToNumber<int32_t>(sCode.substr(nIdx), false, true, (bNeg ? 1 : 0), true, (bNeg ? -m_nMin : m_nMax));
	std::string& sError = oPairInt.second;
	if (sError.empty()) {
		return std::make_pair(Variant{oPairInt.first}, std::move(sError)); //-----
	}
	return std::make_pair(Variant{}, std::move(sError));
}
std::string IntOption::getValueAsDescriptive(const Variant& oValue) const noexcept
{
	if (! isValidValue(oValue)) {
		return "";
	}
	return std::to_string(oValue.getInt());
}
std::pair<Variant, std::string> IntOption::getValueFromString(const std::string& sValue) const noexcept
{
	auto oPairInt = Util::strToNumber<int32_t>(sValue, false, true, m_nMin, true, m_nMax);
	std::string& sError = oPairInt.second;
	if (sError.empty()) {
		return std::make_pair(Variant{oPairInt.first}, std::move(sError)); //-----
	}
	return std::make_pair(Variant{}, std::move(sError));
}

} // namespace stmg
