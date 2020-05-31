/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   testOption.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "options/booloption.h"
#include "options/enumoption.h"
#include "options/intoption.h"

namespace stmg
{

namespace testing
{

TEST_CASE("testOption, IntOptionConstructor")
{
	const std::string sMasterOptionName = "MasterIntOption";
	const std::string sMasterOptionDesc = "The MasterIntOption desc";
	{
		IntOption oIntOption(OwnerType::GAME, sMasterOptionName, 77, sMasterOptionDesc, 1, 100);
		REQUIRE(oIntOption.getName() == sMasterOptionName);
		REQUIRE(oIntOption.getDesc() == sMasterOptionDesc);
		REQUIRE(oIntOption.getDefaultValue() == Variant{77});
		REQUIRE(oIntOption.isValidValue(Variant{1}));
		REQUIRE_FALSE(oIntOption.isValidValue(Variant{0}));
		REQUIRE(oIntOption.getValueAsCode(Variant{88}) == "88");
		REQUIRE(oIntOption.getValueAsDescriptive(Variant{88}) == "88");
		REQUIRE_FALSE(oIntOption.isReadonly());
		REQUIRE(oIntOption.isVisible());
		REQUIRE(oIntOption.getMastersValues().empty());
		//REQUIRE_FALSE(oIntOption.getMaster().operator bool());
		//REQUIRE(oIntOption.getMasterValues().empty());
		REQUIRE(oIntOption.getMin() == 1);
		REQUIRE(oIntOption.getMax() == 100);
	}
	{
		auto refMaster = std::make_shared<IntOption>(OwnerType::GAME, sMasterOptionName, 77, sMasterOptionDesc, 1, 100);
		const std::string sSlaveOptionName = "SlaveIntOption";
		const std::string sSlaveOptionDesc = "The SlaveIntOption desc";
		IntOption oIntOption(OwnerType::TEAM, sSlaveOptionName, 45, sSlaveOptionDesc
							, true, false
							, refMaster, {Variant{1}, Variant{88}, Variant{100}}
							, -10, 50);
		REQUIRE(oIntOption.getName() == sSlaveOptionName);
		REQUIRE(oIntOption.getDesc() == sSlaveOptionDesc);
		REQUIRE(oIntOption.getDefaultValue() == Variant{45});
		REQUIRE(oIntOption.isValidValue(Variant{-1}));
		REQUIRE_FALSE(oIntOption.isValidValue(Variant{100}));
		REQUIRE(oIntOption.getValueAsCode(Variant{-2}) == "m2");
		REQUIRE(oIntOption.getValueAsDescriptive(Variant{-2}) == "-2");
		{
		const auto oPair = oIntOption.getValueFromString(" -3 ");
		REQUIRE(oPair.second.empty());
		REQUIRE(oPair.first == Variant{-3});
		}
		{
		const auto oPair = oIntOption.getValueFromString(" -3333 ");
		REQUIRE(! oPair.second.empty());
		}
		REQUIRE(oIntOption.isReadonly());
		REQUIRE_FALSE(oIntOption.isVisible());
		const auto& aMastersValues = oIntOption.getMastersValues();
		REQUIRE(aMastersValues.size() == 1);
		const shared_ptr<Option>& refCheck = aMastersValues[0].first;
		REQUIRE(refCheck->getName() == sMasterOptionName);
		const std::vector<Variant>& aCheckValues = aMastersValues[0].second;
		REQUIRE(aCheckValues.size() == 3);
		REQUIRE(aCheckValues[0] == Variant{1});
		REQUIRE(aCheckValues[1] == Variant{88});
		REQUIRE(aCheckValues[2] == Variant{100});
		REQUIRE(oIntOption.getMin() == -10);
		REQUIRE(oIntOption.getMax() == 50);
		REQUIRE(oIntOption.allowsRange());
	}
}

TEST_CASE("testOption, BoolOptionConstructor")
{
	std::string sMasterOptionName = "MasterOption";
	std::string sMasterOptionDesc = "The MasterOption desc";
	{
		BoolOption oBoolOption(OwnerType::PLAYER, sMasterOptionName, false, sMasterOptionDesc);
		REQUIRE(oBoolOption.getName() == sMasterOptionName);
		REQUIRE(oBoolOption.getDesc() == sMasterOptionDesc);
		REQUIRE(oBoolOption.getDefaultValue() == Variant{false});
		REQUIRE(oBoolOption.isValidValue(Variant{true}));
		REQUIRE(oBoolOption.isValidValue(Variant{false}));
		REQUIRE(oBoolOption.getValueAsCode(Variant{true}) == "t");
		REQUIRE(oBoolOption.getValueAsCode(Variant{false}) == "f");
		REQUIRE(oBoolOption.getValueAsDescriptive(Variant{true}) == "true");
		REQUIRE(oBoolOption.getValueAsDescriptive(Variant{false}) == "false");
		REQUIRE_FALSE(oBoolOption.allowsRange());
		{
		const auto oPair = oBoolOption.getValueFromString(" True ");
		REQUIRE(oPair.second.empty());
		REQUIRE(oPair.first == Variant{true});
		}
		{
		const auto oPair = oBoolOption.getValueFromString("false");
		REQUIRE(oPair.second.empty());
		REQUIRE(oPair.first == Variant{false});
		}
		{
		const auto oPair = oBoolOption.getValueFromString(" TrUe ");
		REQUIRE(! oPair.second.empty());
		}
		{
		const auto oPair = oBoolOption.getValueFromString("0");
		REQUIRE(! oPair.second.empty());
		}
		{
		const auto oPair = oBoolOption.getValueFromString("");
		REQUIRE(! oPair.second.empty());
		}
		REQUIRE_FALSE(oBoolOption.isReadonly());
		REQUIRE(oBoolOption.isVisible());
		const auto& aMastersValues = oBoolOption.getMastersValues();
		REQUIRE(aMastersValues.empty());
	}
	{
		auto refMaster = std::make_shared<IntOption>(OwnerType::TEAM, sMasterOptionName, 77, sMasterOptionDesc, 1, 100);
		const std::string sSlaveOptionName = "SlaveBoolOption";
		const std::string sSlaveOptionDesc = "The SlaveBoolOption desc";
		BoolOption oBoolOption(OwnerType::TEAM, sSlaveOptionName, true, sSlaveOptionDesc
								, true, false
								, refMaster, {Variant{1}, Variant{88}, Variant{100}});
		REQUIRE(oBoolOption.getName() == sSlaveOptionName);
		REQUIRE(oBoolOption.getDesc() == sSlaveOptionDesc);
		REQUIRE(oBoolOption.getDefaultValue() == Variant{true});
		REQUIRE(oBoolOption.isReadonly());
		REQUIRE_FALSE(oBoolOption.isVisible());
		const auto& aMastersValues = oBoolOption.getMastersValues();
		REQUIRE(aMastersValues.size() == 1);
		const shared_ptr<Option>& refCheck = aMastersValues[0].first;
		REQUIRE(refCheck->getName() == sMasterOptionName);
		const std::vector<Variant>& aCheckValues = aMastersValues[0].second;

		REQUIRE(aCheckValues.size() == 3);
		REQUIRE(aCheckValues[0] == Variant{1});
		REQUIRE(aCheckValues[1] == Variant{88});
		REQUIRE(aCheckValues[2] == Variant{100});
	}
}

TEST_CASE("testOption, EnumOptionConstructor")
{
	std::string sMasterOptionName = "MasterOption";
	std::string sMasterOptionDesc = "The MasterOption desc";
	{
		EnumOption oEnumOption(OwnerType::TEAM, sMasterOptionName, 3, sMasterOptionDesc
					, std::vector< std::tuple<int32_t, std::string, std::string> >{ 
							std::tuple<int32_t, std::string, std::string>{3, "Val0", "Value 0"}
							, std::tuple<int32_t, std::string, std::string>{5, "Val1", "Value 1"} }
					);
		REQUIRE(oEnumOption.getName() == sMasterOptionName);
		REQUIRE(oEnumOption.getDesc() == sMasterOptionDesc);
		REQUIRE(oEnumOption.getDefaultValue() == Variant{3});
		REQUIRE(oEnumOption.isValidValue(Variant{5}));
		REQUIRE_FALSE(oEnumOption.isValidValue(Variant{4}));
		REQUIRE(oEnumOption.getValueAsCode(Variant{3}) == "Val0");
		REQUIRE(oEnumOption.getValueAsDescriptive(Variant{3}) == "Value 0");
		REQUIRE_FALSE(oEnumOption.allowsRange());
		{
		const auto oPair = oEnumOption.getValueFromString("Val0");
		REQUIRE(oPair.second.empty());
		REQUIRE(oPair.first == Variant{3});
		}
		{
		const auto oPair = oEnumOption.getValueFromString("Val1");
		REQUIRE(oPair.second.empty());
		REQUIRE(oPair.first == Variant{5});
		}
		{
		const auto oPair = oEnumOption.getValueFromString(" Val0 ");
		REQUIRE(oPair.second.empty());
		REQUIRE(oPair.first == Variant{3});
		}
		{
		const auto oPair = oEnumOption.getValueFromString("Value 0");
		REQUIRE(! oPair.second.empty());
		}
		{
		const auto oPair = oEnumOption.getValueFromString("");
		REQUIRE(! oPair.second.empty());
		}
		REQUIRE_FALSE(oEnumOption.isReadonly());
		REQUIRE(oEnumOption.isVisible());
		REQUIRE(oEnumOption.getMastersValues().empty());
		REQUIRE(oEnumOption.size() == 2);
		REQUIRE(oEnumOption.getEnum(0) == 3);
		REQUIRE(oEnumOption.getEnum(1) == 5);
		REQUIRE(oEnumOption.getEnumName(0) == "Val0");
		REQUIRE(oEnumOption.getEnumName(1) == "Val1");
		REQUIRE(oEnumOption.getEnumDesc(0) == "Value 0");
		REQUIRE(oEnumOption.getEnumDesc(1) == "Value 1");
		REQUIRE(oEnumOption.getIdx(3) == 0);
		REQUIRE(oEnumOption.getIdx(5) == 1);
		REQUIRE(oEnumOption.getName(3) == "Val0");
		REQUIRE(oEnumOption.getName(5) == "Val1");
		REQUIRE(oEnumOption.getEnum("Val0") == 3);
		REQUIRE(oEnumOption.getEnum("Val1") == 5);
	}
	{
		auto refMaster = std::make_shared<BoolOption>(OwnerType::TEAM, sMasterOptionName, false, sMasterOptionDesc);
		const std::string sSlaveOptionName = "SlaveEnumOption";
		const std::string sSlaveOptionDesc = "The SlaveEnumOption desc";
		EnumOption oEnumOption(OwnerType::PLAYER, sSlaveOptionName, 5, sSlaveOptionDesc
								, true, false
								, refMaster, {Variant{true}}
								, std::vector< std::tuple<int32_t, std::string, std::string> >{ 
									std::tuple<int32_t, std::string, std::string>{11, "Val0", "Value 0"}
									, std::tuple<int32_t, std::string, std::string>{5, "Val1", "Value 1"}
									, std::tuple<int32_t, std::string, std::string>{3, "Val2", "Value 2"}
									}
								);
		REQUIRE(oEnumOption.getName() == sSlaveOptionName);
		REQUIRE(oEnumOption.getDesc() == sSlaveOptionDesc);
		REQUIRE(oEnumOption.getDefaultValue() == Variant{5});
		REQUIRE(oEnumOption.isValidValue(Variant{3}));
		REQUIRE_FALSE(oEnumOption.isValidValue(Variant{10}));
		REQUIRE(oEnumOption.isReadonly());
		REQUIRE_FALSE(oEnumOption.isVisible());

		const auto& aMastersValues = oEnumOption.getMastersValues();
		REQUIRE(aMastersValues.size() == 1);
		const shared_ptr<Option>& refCheck = aMastersValues[0].first;
		REQUIRE(refCheck->getName() == sMasterOptionName);
		const std::vector<Variant>& aCheckValues = aMastersValues[0].second;
		REQUIRE(aCheckValues.size() == 1);
		REQUIRE(aCheckValues[0] == Variant{true});
		REQUIRE(oEnumOption.size() == 3);
		REQUIRE(oEnumOption.getEnum(0) == 11);
		REQUIRE(oEnumOption.getEnum(1) == 5);
		REQUIRE(oEnumOption.getEnum(2) == 3);
	}
}

} // namespace testing

} // namespace stmg
