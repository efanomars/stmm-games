/*
 * File:   enumoption.h
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

#ifndef STMG_ENUM_OPTION_H
#define STMG_ENUM_OPTION_H

#include "option.h"
#include "util/namedobjindex.h"
#include "ownertype.h"
#include "util/variant.h"

#include <vector>
#include <unordered_map>
#include <tuple>
#include <memory>

#include <string>
#include <utility>

#include <stdint.h>


namespace stmg
{

using std::shared_ptr;

class EnumOption : public Option
{
public:
	/** Constructor.
	 * Parameter aEnum is a vector of tuple&lt;nEnum, sEnumName, sDesc&gt; where
	 *    - nEnum is the value (must be unique among all tuples, cannot be std::numeric_limits<int32_t>::min()),
	 *    - sEnumName is a string name (must be unique among all tuples),
	 *    - sDesc is a description (maybe localized)
	 *
	 * nDefaultEnum must be one of the nEnum values in aEnum.
	 * @param eOwnerType The owner type.
	 * @param sName The option name.
	 * @param nDefaultEnum The default value.
	 * @param sDesc The description of the option.
	 * @param aEnum The array of values with their name and description.
	 */
	EnumOption(OwnerType eOwnerType, const std::string& sName, int32_t nDefaultEnum, const std::string& sDesc
				, const std::vector< std::tuple<int32_t, std::string, std::string> >& aEnum // tuple<nEnum, sEnumName, sDesc>
				) noexcept
	: EnumOption(eOwnerType, sName, nDefaultEnum, sDesc, false, true, {}, {}, aEnum)
	{
	}
	/** Constructor.
	 * Parameter aEnum is a vector of tuple&lt;nEnum, sEnumName, sDesc&gt; where
	 *    - nEnum is the value (must be unique among all tuples, cannot be std::numeric_limits<int32_t>::min()),
	 *    - sEnumName is a string name (must be unique among all tuples) and satisfy `[A-Za-z0-9]+`,
	 *    - sDesc is a description (maybe localized)
	 *
	 * @param eOwnerType The owner type.
	 * @param sName The option name.
	 * @param nDefaultEnum The default value.
	 * @param sDesc The description of the option.
	 * @param bReadOnly Whether the option is read-only.
	 * @param bVisible Whether the option is visible.
	 * @param refMaster The master option. Can be null.
	 * @param aMasterValues The master option values for which this option is valid.
	 * @param aEnum The array of values with their name and description.
	 */
	EnumOption(OwnerType eOwnerType, const std::string& sName, int32_t nDefaultEnum, const std::string& sDesc
				, bool bReadOnly, bool bVisible
				, const shared_ptr<Option>& refMaster, const std::vector<Variant>& aMasterValues
				, const std::vector< std::tuple<int32_t, std::string, std::string> >& aEnum // tuple<nEnum, sEnumName, sDesc>
				) noexcept;
	/** Constructor.
	 * Parameter aEnum is a vector of tuple&lt;nEnum, sEnumName, sDesc&gt; where
	 *    - nEnum is the value (must be unique among all tuples, cannot be std::numeric_limits<int32_t>::min()),
	 *    - sEnumName is a string name (must be unique among all tuples) and satisfy `[A-Za-z0-9]+`,
	 *    - sDesc is a description (maybe localized)
	 *
	 * @param eOwnerType The owner type.
	 * @param sName The option name.
	 * @param nDefaultEnum The default value.
	 * @param sDesc The description of the option.
	 * @param bReadOnly Whether the option is read-only.
	 * @param bVisible Whether the option is visible.
	 * @param aMastersValues The values for each master option for which this option is enabled. Can be empty. Value: (refMaster, aMasterValues).
	 * @param aEnum The array of values with their name and description.
	 */
	EnumOption(OwnerType eOwnerType, const std::string& sName, int32_t nDefaultEnum, const std::string& sDesc
				, bool bReadOnly, bool bVisible
				, const std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>>& aMastersValues
				, const std::vector< std::tuple<int32_t, std::string, std::string> >& aEnum // tuple<nEnum, sEnumName, sDesc>
				) noexcept;

	bool isValidValue(const Variant& oValue) const noexcept override;
	std::string getValueAsCode(const Variant& oValue) const noexcept override;
	std::pair<Variant, std::string> getValueFromCode(const std::string& sCode) const noexcept override;
	std::string getValueAsDescriptive(const Variant& oValue) const noexcept override;
	std::pair<Variant, std::string> getValueFromString(const std::string& sValue) const noexcept override;
	/** The total number of enums.
	 * @return The total number of enum values.
	 */
	int32_t size() const noexcept { return m_oNamedEnum.size(); }
	/** Returns enum data for a given index.
	 * The order is the same as passed to the constructor.
	 * @param nIdx The index must be `>= 0` and `< size()`.
	 * @return The tuple containing enum value, enum name, enum description.
	 */
	std::tuple<int32_t, std::string, std::string> get(int32_t nIdx) const noexcept;
	int32_t getEnum(int32_t nIdx) const noexcept;
	const std::string& getEnumName(int32_t nIdx) const noexcept;
	const std::string& getEnumDesc(int32_t nIdx) const noexcept;
	/** The index of an enum value.
	 * If the the enum value is valid the index is `>= 0` and `< size()`.
	 * @param nEnum The enum value. Cannot be std::numeric_limits<int32_t>::min().
	 * @return The index of the enum or -1 if not a valid value.
	 */
	int32_t getIdx(int32_t nEnum) const noexcept;
	using Option::getName;
	const std::string& getName(int32_t nEnum) const noexcept;
	/** Get the enum value from the enum name.
	 * @param sEnumName The enum name.
	 * @return The enum value or std::numeric_limits<int32_t>::min() if invalid.
	 */
	int32_t getEnum(const std::string& sEnumName) const noexcept;
private:
	NamedObjIndex< std::pair<int32_t, std::string> > m_oNamedEnum; // Keys: (nNameIdx, sEnumName) Value: pair<nEnum, sDesc>
	std::unordered_map<int32_t, int32_t> m_oEnumIdx; // Key: enum, Value: nNameIdx in m_oNamedEnum
private:
	EnumOption() = delete;
};

} // namespace stmg

#endif	/* STMG_ENUM_OPTION_H */

