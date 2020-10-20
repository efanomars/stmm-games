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
 * File:   intoption.h
 */

#ifndef STMG_INT_OPTION_H
#define STMG_INT_OPTION_H

#include "option.h"
#include "ownertype.h"

#include <vector>
#include <memory>
#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class Variant; }

namespace stmg
{

using std::shared_ptr;

class IntOption : public Option
{
public:
	IntOption(OwnerType eOwnerType, const std::string& sName, int32_t nDefaultValue, const std::string& sDesc
				, int32_t nMin, int32_t nMax) noexcept;
	IntOption(OwnerType eOwnerType, const std::string& sName, int32_t nDefaultValue, const std::string& sDesc
				, bool bReadOnly, bool bVisible
				, const shared_ptr<Option>& refMaster, const std::vector<Variant>& aMasterValues
				, int32_t nMin, int32_t nMax) noexcept;
	IntOption(OwnerType eOwnerType, const std::string& sName, int32_t nDefaultValue, const std::string& sDesc
				, bool bReadOnly, bool bVisible
				, const std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>>& aMastersValues
				, int32_t nMin, int32_t nMax) noexcept;
	bool isValidValue(const Variant& oValue) const noexcept override;
	std::string getValueAsCode(const Variant& oValue) const noexcept override;
	std::pair<Variant, std::string> getValueFromCode(const std::string& sCode) const noexcept override;
	std::string getValueAsDescriptive(const Variant& oValue) const noexcept override;
	std::pair<Variant, std::string> getValueFromString(const std::string& sValue) const noexcept override;
	bool allowsRange() const noexcept override { return true; }
	inline int32_t getMin() const noexcept { return m_nMin; }
	inline int32_t getMax() const noexcept { return m_nMax; }
private:
	int32_t m_nMin;
	int32_t m_nMax;
private:
	IntOption() = delete;
};

} // namespace stmg

#endif	/* STMG_INT_OPTION_H */

