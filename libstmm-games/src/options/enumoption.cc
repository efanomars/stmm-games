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
 * File:   enumoption.cc
 */

#include "options/enumoption.h"

#include "util/util.h"

#include <cassert>
#include <limits>
#include <type_traits>

namespace stmg
{

using MastersValues = std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>>;

EnumOption::EnumOption(OwnerType eOwnerType, const std::string& sName, int32_t nDefaultEnum, const std::string& sDesc
						, bool bReadOnly, bool bVisible
						, const shared_ptr<Option>& refMaster, const std::vector<Variant>& aMasterValues
						, const std::vector< std::tuple<int32_t, std::string, std::string> >& aEnum // tuple<nEnum, sEnumName, sDesc>
						) noexcept
: EnumOption(eOwnerType, sName, nDefaultEnum, sDesc, bReadOnly, bVisible, (refMaster ? MastersValues{std::make_pair(refMaster, aMasterValues)} : MastersValues{}), aEnum)
{
	if (! refMaster) {
		assert(aMasterValues.empty());
	}
}
EnumOption::EnumOption(OwnerType eOwnerType, const std::string& sName, int32_t nDefaultEnum, const std::string& sDesc
				, bool bReadOnly, bool bVisible
				, const std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>>& aMastersValues
				, const std::vector< std::tuple<int32_t, std::string, std::string> >& aEnum // tuple<nEnum, sEnumName, sDesc>
				) noexcept
: Option(eOwnerType, sName, Variant{nDefaultEnum}, sDesc, bReadOnly, bVisible, aMastersValues)
{
	#ifndef NDEBUG
	bool bDefaultValueValid = false;
	#endif //NDEBUG
	for (auto& oTuple : aEnum) {
		const int32_t nEnum = std::get<0>(oTuple);
		assert(nEnum >= 0); // enum can't be negative
		const std::string& sEnumName = std::get<1>(oTuple);
		assert(!sEnumName.empty()); // enum name can't be empty
		#ifndef NDEBUG
		for (auto& c : sEnumName) {
			assert( ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')) || ((c >= '0') && (c <= '9')));
		}
		#endif //NDEBUG
		const std::string& sDesc = std::get<2>(oTuple);
		if (nDefaultEnum == nEnum) {
			#ifndef NDEBUG
			bDefaultValueValid = true;
			#endif //NDEBUG
		}
		assert(m_oNamedEnum.getIndex(sEnumName) < 0); // Name already exists
		const int32_t nNameIdx = m_oNamedEnum.addNamedObj(std::move(sEnumName), std::make_pair(nEnum, sDesc));
		assert(m_oEnumIdx.find(nEnum) == m_oEnumIdx.end()); // Enum already exists
		m_oEnumIdx.insert(std::make_pair(nEnum, nNameIdx));
	}
	assert(m_oNamedEnum.size() > 0);
	assert(bDefaultValueValid); // Default value not valid
}
bool EnumOption::isValidValue(const Variant& oValue) const noexcept
{
	if (oValue.getType() != Variant::TYPE_INT) {
		return false;
	}
	const int32_t nEnum = oValue.getInt();
	return (getIdx(nEnum) >= 0);
}

std::string EnumOption::getValueAsCode(const Variant& oValue) const noexcept
{
	if (! isValidValue(oValue)) {
		return "";
	}
	const int32_t nEnum = oValue.getInt();
	return getName(nEnum);
}
std::pair<Variant, std::string> EnumOption::getValueFromCode(const std::string& sCode) const noexcept
{
	if (sCode.empty() || Util::strContainsWhitespace(sCode)) {
		return std::make_pair(Variant{}, "Code cannot contain empty spaces or be empty!");
	}
	const int32_t nEnum = getEnum(sCode);
	if (nEnum == std::numeric_limits<int32_t>::lowest()) {
		return std::make_pair(Variant{}, "Invalid enum code!");
	}
	return std::make_pair(Variant{nEnum}, "");
}
std::string EnumOption::getValueAsDescriptive(const Variant& oValue) const noexcept
{
	if (! isValidValue(oValue)) {
		return "";
	}
	const int32_t nEnum = oValue.getInt();
	const int32_t nIdx = getIdx(nEnum);
	return getEnumDesc(nIdx);
}
std::pair<Variant, std::string> EnumOption::getValueFromString(const std::string& sValue) const noexcept
{
	auto sEnumName = Util::strStrip(sValue);
	int32_t nNameIdx = m_oNamedEnum.getIndex(sEnumName);
	if (nNameIdx < 0) {
		return std::make_pair(Variant{}, "Not a valid enum name!");
	}
	return std::make_pair(Variant{m_oNamedEnum.getObj(nNameIdx).first}, "");
}
std::tuple<int32_t, std::string, std::string> EnumOption::get(int32_t nIdx) const noexcept
{
	assert((nIdx >= 0) && (nIdx < size()));
	const std::pair<int32_t, std::string>& oEnumDesc = m_oNamedEnum.getObj(nIdx);
	const int32_t nEnum = oEnumDesc.first;
	const std::string& sDesc = oEnumDesc.second;
	const std::string& sEnumName = m_oNamedEnum.getName(nIdx);
	return std::make_tuple(nEnum, sEnumName, sDesc);
}
int32_t EnumOption::getEnum(int32_t nIdx) const noexcept
{
	assert((nIdx >= 0) && (nIdx < size()));
	const std::pair<int32_t, std::string>& oEnumDesc = m_oNamedEnum.getObj(nIdx);
	const int32_t nEnum = oEnumDesc.first;
	return nEnum;
}
const std::string& EnumOption::getEnumName(int32_t nIdx) const noexcept
{
	assert((nIdx >= 0) && (nIdx < size()));
	const std::string& sEnumName = m_oNamedEnum.getName(nIdx);
	return sEnumName;
}
const std::string& EnumOption::getEnumDesc(int32_t nIdx) const noexcept
{
	assert((nIdx >= 0) && (nIdx < size()));
	const std::pair<int32_t, std::string>& oEnumDesc = m_oNamedEnum.getObj(nIdx);
	const std::string& sDesc = oEnumDesc.second;
	return sDesc;
}
int32_t EnumOption::getIdx(int32_t nEnum) const noexcept
{
	assert(std::numeric_limits<int32_t>::lowest() != nEnum);
	std::unordered_map<int32_t, int32_t>::const_iterator it = m_oEnumIdx.find(nEnum);
	if (it == m_oEnumIdx.end()) {
		return -1;
	}
	const int32_t nNameIdx = it->second;
	return nNameIdx;
}
const std::string& EnumOption::getName(int32_t nEnum) const noexcept
{
	std::unordered_map<int32_t, int32_t>::const_iterator it = m_oEnumIdx.find(nEnum);
	if (it == m_oEnumIdx.end()) {
		return Util::s_sEmptyString;
	}
	const int32_t nNameIdx = it->second;
	return m_oNamedEnum.getName(nNameIdx);
}
int32_t EnumOption::getEnum(const std::string& sEnumName) const noexcept
{
	int32_t nNameIdx = m_oNamedEnum.getIndex(sEnumName);
	if (nNameIdx < 0) {
		return std::numeric_limits<int32_t>::lowest();
	}
	return m_oNamedEnum.getObj(nNameIdx).first;
}

} // namespace stmg
