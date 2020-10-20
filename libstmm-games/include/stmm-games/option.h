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
 * File:   option.h
 */

#ifndef STMG_OPTION_H
#define STMG_OPTION_H

#include "ownertype.h"

#include "util/variant.h"

#include <vector>
#include <memory>
#include <string>
#include <utility>

namespace stmg
{

using std::shared_ptr;

class Option
{
protected:
	virtual ~Option() = default;
	/** Constructor.
	 * The option is created visible and not readonly.
	 * @param eOwnerType The owner type.
	 * @param sName The option name.
	 * @param oDefaultValue The default value.
	 * @param sDesc The description of the option.
	 */
	Option(OwnerType eOwnerType, const std::string& sName, const Variant& oDefaultValue, const std::string& sDesc) noexcept;
	/** One master constructor.
	 *
	 * If bReadOnly is `true` the preferences dialog might show it but not allow to set its value.
	 *
	 * If bVisible is `false` the preferences dialog doesn't show the option.
	 *
	 * If the instance has a master (it's a slave) it keeps a strong reference to it.
	 * A master option cannot be slave itself.
	 * If refMaster is not null aMasterValues cannot contain duplicates.
	 *
	 * @param eOwnerType The owner type.
	 * @param sName The option name.
	 * @param oDefaultValue The default value.
	 * @param sDesc The description of the option.
	 * @param bReadOnly Whether readonly.
	 * @param bVisible Whether visible.
	 * @param refMaster The master option. Can be null.
	 * @param aMasterValues The values of the master option for which this option is enabled. Must be empty if refMaster is null.
	 */
	Option(OwnerType eOwnerType, const std::string& sName, const Variant& oDefaultValue, const std::string& sDesc
			, bool bReadOnly, bool bVisible
			, const shared_ptr<Option>& refMaster, const std::vector<Variant>& aMasterValues) noexcept;
	/** Generic constructor.
	 *
	 * If bReadOnly is `true` the preferences dialog might show it but not allow to set its value.
	 *
	 * If bVisible is `false` the preferences dialog doesn't show the option.
	 *
	 * If the instance has at least a master (aMastersValues is not empty) it is a slave,
	 * and it keeps a strong reference to those masters.
	 * A master option cannot be slave itself.
	 * For each master the values for which the slave is enabled cannot contain duplicates.
	 *
	 * @param eOwnerType The owner type.
	 * @param sName The option name.
	 * @param oDefaultValue The default value.
	 * @param sDesc The description of the option.
	 * @param bReadOnly Whether readonly.
	 * @param bVisible Whether visible.
	 * @param aMastersValues The values for each master option for which this option is enabled. Can be empty. Value: (refMaster, aMasterValues).
	 */
	Option(OwnerType eOwnerType, const std::string& sName, const Variant& oDefaultValue, const std::string& sDesc
			, bool bReadOnly, bool bVisible
			, const std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>>& aMastersValues) noexcept;
public:
	inline OwnerType getOwnerType() const noexcept { return m_eOwnerType; }
	inline const std::string& getName() const noexcept { return m_sName; }
	inline Variant getDefaultValue() const noexcept { return m_oDefaultValue; }
	inline const std::string& getDesc() const noexcept { return m_sDesc; }

	/** Whether the value is valid for this option.
	 * @param oValue The value.
	 * @return Whether valid.
	 */
	virtual bool isValidValue(const Variant& oValue) const noexcept = 0;
	/** Return a value as code.
	 * @param oValue The value.
	 * @return The code or empty string if invalid.
	 */
	virtual std::string getValueAsCode(const Variant& oValue) const noexcept = 0;
	/** Convert code to value.
	 * @param sCode The code.
	 * @return The value and empty string or null and the error.
	 */
	virtual std::pair<Variant, std::string> getValueFromCode(const std::string& sCode) const noexcept = 0;
	/** Return the value as a human readable string.
	 * @param oValue The value.
	 * @return The descriptive string or empty string if invalid.
	 */
	virtual std::string getValueAsDescriptive(const Variant& oValue) const noexcept = 0;
	/** Whether range of values is supported.
	 * The default implementation returns false.
	 * @return Whether it makes sense to take a range of the values.
	 */
	virtual bool allowsRange() const noexcept { return false; }
	/** Convert string to value.
	 * @param sValue The string representation. Cannot be empty.
	 * @return The value and an empty string ot null and the error string.
	 */
	virtual std::pair<Variant, std::string> getValueFromString(const std::string& sValue) const noexcept = 0;

	inline bool isReadonly() const noexcept { return m_bReadOnly; }
	inline bool isVisible() const noexcept { return m_bVisible; }

	/** Get the masters and their corresonding values for which this option is enabled.
	 * All masters must have one of the given values for this option to be enabled.
	 * @return The masters and their values.
	 */
	inline const std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>>& getMastersValues() const noexcept { return m_aMastersValues; }

	bool isSlave() const noexcept { return ! m_aMastersValues.empty(); }
private:
	OwnerType m_eOwnerType;
	const std::string m_sName;
	const Variant m_oDefaultValue;
	const std::string m_sDesc;
	const bool m_bReadOnly;
	const bool m_bVisible;
	const std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>> m_aMastersValues; // (refMaster, aValuesEnabled)
private:
	Option() = delete;
};

} // namespace stmg

#endif	/* STMG_OPTION_H */

