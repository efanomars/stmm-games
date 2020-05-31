/*
 * File:   booloption.h
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

#ifndef STMG_BOOL_OPTION_H
#define STMG_BOOL_OPTION_H

#include "option.h"
#include "ownertype.h"

#include <vector>
#include <memory>
#include <string>
#include <utility>

namespace stmg { class Variant; }

namespace stmg
{

using std::shared_ptr;

class BoolOption : public Option
{
public:
	BoolOption(OwnerType eOwnerType, const std::string& sName, bool bDefaultValue, const std::string& sDesc) noexcept;
	BoolOption(OwnerType eOwnerType, const std::string& sName, bool bDefaultValue, const std::string& sDesc
				, bool bReadOnly, bool bVisible
				, const shared_ptr<Option>& refMaster, const std::vector<Variant>& aMasterValues) noexcept;
	BoolOption(OwnerType eOwnerType, const std::string& sName, bool bDefaultValue, const std::string& sDesc
				, bool bReadOnly, bool bVisible
				, const std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>>& aMastersValues) noexcept;
	bool isValidValue(const Variant& oValue) const noexcept override;
	std::string getValueAsCode(const Variant& oValue) const noexcept override;
	std::pair<Variant, std::string> getValueFromCode(const std::string& sCode) const noexcept override;
	std::string getValueAsDescriptive(const Variant& oValue) const noexcept override;
	std::pair<Variant, std::string> getValueFromString(const std::string& sValue) const noexcept override;
private:
	BoolOption() = delete;
};

} // namespace stmg

#endif	/* STMG_BOOL_OPTION_H */

