/*
 * File:   booloption.cc
 *
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

#include "options/booloption.h"

#include "util/util.h"
#include "util/variant.h"

#include <type_traits>


namespace stmg
{

BoolOption::BoolOption(OwnerType eOwnerType, const std::string& sName, bool bDefaultValue, const std::string& sDesc) noexcept
: Option(eOwnerType, sName, Variant{bDefaultValue}, sDesc)
{
}
BoolOption::BoolOption(OwnerType eOwnerType, const std::string& sName, bool bDefaultValue, const std::string& sDesc
			, bool bReadOnly, bool bVisible
			, const shared_ptr<Option>& refMaster, const std::vector<Variant>& aMasterValues) noexcept
: Option(eOwnerType, sName, Variant{bDefaultValue}, sDesc, bReadOnly, bVisible, refMaster, aMasterValues)
{
}
BoolOption::BoolOption(OwnerType eOwnerType, const std::string& sName, bool bDefaultValue, const std::string& sDesc
						, bool bReadOnly, bool bVisible
						, const std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>>& aMastersValues) noexcept
: Option(eOwnerType, sName, Variant{bDefaultValue}, sDesc, bReadOnly, bVisible, aMastersValues)
{
}

bool BoolOption::isValidValue(const Variant& oValue) const noexcept
{
	if (oValue.getType() != Variant::TYPE_BOOL) {
		return false;
	}
	return true;
}

std::string BoolOption::getValueAsCode(const Variant& oValue) const noexcept
{
	if (! isValidValue(oValue)) {
		return "";
	}
	if (oValue.getBool()) {
		return "t";
	} else {
		return "f";
	}
}
std::pair<Variant, std::string> BoolOption::getValueFromCode(const std::string& sCode) const noexcept
{
	if (sCode.empty() || Util::strContainsWhitespace(sCode)) {
		return std::make_pair(Variant{}, "Code cannot contain empty spaces or be empty!"); //------
	}
	bool bValue = false;
	if (sCode == "t") {
		bValue = true;
	} else if (sCode == "f") {
		//
	} else {
		return std::make_pair(Variant{}, "Invalid bool code!"); //--------------
	}
	return std::make_pair(Variant{bValue}, "");
}
std::string BoolOption::getValueAsDescriptive(const Variant& oValue) const noexcept
{
	if (! isValidValue(oValue)) {
		return "";
	}
	if (oValue.getBool()) {
		return "true";
	} else {
		return "false";
	}
}
std::pair<Variant, std::string> BoolOption::getValueFromString(const std::string& sValue) const noexcept
{
	auto oPairBool = Util::strToBool(sValue);
	std::string& sError = oPairBool.second;
	if (sError.empty()) {
		return std::make_pair(Variant{oPairBool.first}, std::move(sError)); //-----------------
	}
	return std::make_pair(Variant{}, std::move(sError));
}

} // namespace stmg
