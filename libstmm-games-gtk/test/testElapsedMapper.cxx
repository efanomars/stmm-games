/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   testElapsedMapper.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "gtkutil/elapsedmapper.h"

namespace stmg
{

namespace testing
{

using EM = ElapsedMapper;

constexpr double g_fErrMargin = 0.00001;

TEST_CASE("testElapsedMapper, MAPPING_TYPE_IDENTITY")
{
	{
	EM oEM{};
	for (double fElapsed = 0.0; fElapsed <= 1.0; fElapsed += 0.1) {
		REQUIRE(oEM.map(fElapsed) == fElapsed);
	}
	}

	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_IDENTITY;
	for (double fElapsed = 0.0; fElapsed <= 1.0; fElapsed += 0.1) {
		REQUIRE(oEM.map(fElapsed) == fElapsed);
	}
	}

	{
	EM oEM{};
	oEM.m_fMin = 0.2;
	oEM.m_fMax = 0.7;
	// shrink is false
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_IDENTITY;
	REQUIRE(oEM.map(0.0) == 0.2);
	REQUIRE(oEM.map(0.1) == 0.2);
	REQUIRE(oEM.map(0.2) == 0.2);
	REQUIRE(oEM.map(0.5) == 0.5);
	REQUIRE(oEM.map(0.6) == 0.6);
	REQUIRE(oEM.map(0.7) == 0.7);
	REQUIRE(oEM.map(1.0) == 0.7);
	}

	{
	EM oEM{};
	oEM.m_fMin = 0.2;
	oEM.m_fMax = 0.7;
	oEM.m_bShrink = true;
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_IDENTITY;
	REQUIRE(Approx(oEM.map(0.0)).epsilon(g_fErrMargin) == 0.2);
	REQUIRE(Approx(oEM.map(0.2)).epsilon(g_fErrMargin) == 0.3);
	REQUIRE(Approx(oEM.map(0.4)).epsilon(g_fErrMargin) == 0.4);
	REQUIRE(Approx(oEM.map(0.5)).epsilon(g_fErrMargin) == 0.45);
	REQUIRE(Approx(oEM.map(0.6)).epsilon(g_fErrMargin) == 0.5);
	REQUIRE(Approx(oEM.map(0.8)).epsilon(g_fErrMargin) == 0.6);
	REQUIRE(Approx(oEM.map(1.0)).epsilon(g_fErrMargin) == 0.7);
	}
}

TEST_CASE("testElapsedMapper, MAPPING_TYPE_OOR_UNDEFINED")
{
	{
	EM oEM{};
	oEM.m_fA = 1.0;
	oEM.m_fB = 0.0;
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_UNDEFINED;
	for (double fElapsed = 0.0; fElapsed <= 1.0; fElapsed += 0.1) {
		REQUIRE(oEM.map(fElapsed) == fElapsed);
	}
	}

	{
	EM oEM{};
	oEM.m_fA = 2.0;
	oEM.m_fB = 0.0;
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_UNDEFINED;
	for (double fElapsed = 0.0; fElapsed <= 0.5; fElapsed += 0.1) {
		REQUIRE(Approx(oEM.map(fElapsed)).epsilon(g_fErrMargin) == 2 * fElapsed);
	}
	for (double fElapsed = 0.6; fElapsed <= 1.0; fElapsed += 0.1) {
		REQUIRE(oEM.map(fElapsed) == -1);
	}
	}
}

TEST_CASE("testElapsedMapper, MAPPING_TYPE_OOR_CLAMP")
{
	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_CLAMP;
	for (double fElapsed = 0.0; fElapsed <= 1.0; fElapsed += 0.1) {
		REQUIRE(oEM.map(fElapsed) == fElapsed);
	}
	}

	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_CLAMP;
	oEM.m_fA = 2.0;
	oEM.m_fB = 0.0;
	REQUIRE(oEM.map(0.0) == 0.0);
	REQUIRE(Approx(oEM.map(0.3)).epsilon(g_fErrMargin) == 0.6);
	REQUIRE(Approx(oEM.map(0.5)).epsilon(g_fErrMargin) == 1.0);
	REQUIRE(Approx(oEM.map(0.7)).epsilon(g_fErrMargin) == 1.0);
	}

	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_CLAMP;
	oEM.m_fA = -3.0;
	oEM.m_fB = 1.0;
	REQUIRE(Approx(oEM.map(0.0)).epsilon(g_fErrMargin) == 1.0);
	REQUIRE(Approx(oEM.map(0.3)).epsilon(g_fErrMargin) == 0.1);
	REQUIRE(Approx(oEM.map(0.5)).epsilon(g_fErrMargin) == 0.0);
	REQUIRE(Approx(oEM.map(1.0)).epsilon(g_fErrMargin) == 0.0);
	}

	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_CLAMP;
	oEM.m_fA = -3.0;
	oEM.m_fB = 1.0;
	oEM.m_fMin = 0.2;
	oEM.m_fMax = 0.7;
	// shrink is false
	REQUIRE(Approx(oEM.map(0.0)).epsilon(g_fErrMargin) == 0.7);
	REQUIRE(Approx(oEM.map(0.1)).epsilon(g_fErrMargin) == 0.7);
	REQUIRE(Approx(oEM.map(0.2)).epsilon(g_fErrMargin) == 0.4);
	REQUIRE(Approx(oEM.map(0.3)).epsilon(g_fErrMargin) == 0.2);
	REQUIRE(Approx(oEM.map(0.5)).epsilon(g_fErrMargin) == 0.2);
	REQUIRE(Approx(oEM.map(1.0)).epsilon(g_fErrMargin) == 0.2);
	}

	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_CLAMP;
	oEM.m_fA = -3.0;
	oEM.m_fB = 1.0;
	oEM.m_fMin = 0.2;
	oEM.m_fMax = 0.7;
	oEM.m_bShrink = true;
	REQUIRE(Approx(oEM.map(0.0)).epsilon(g_fErrMargin) == 0.7);
	REQUIRE(Approx(oEM.map(0.1)).epsilon(g_fErrMargin) == 0.55);
	REQUIRE(Approx(oEM.map(0.2)).epsilon(g_fErrMargin) == 0.4);
	REQUIRE(Approx(oEM.map(0.3)).epsilon(g_fErrMargin) == 0.25);
	REQUIRE(Approx(oEM.map(0.5)).epsilon(g_fErrMargin) == 0.2);
	REQUIRE(Approx(oEM.map(1.0)).epsilon(g_fErrMargin) == 0.2);
	}
}

TEST_CASE("testElapsedMapper, MAPPING_TYPE_OOR_MODULO")
{
	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_MODULO;
	for (double fElapsed = 0.0; fElapsed <= 1.0; fElapsed += 0.1) {
		REQUIRE(oEM.map(fElapsed) == fElapsed);
	}
	}

	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_MODULO;
	oEM.m_fA = 4.0;
	oEM.m_fB = 0.0;
	REQUIRE(oEM.map(0.0) == 0.0);
	REQUIRE(Approx(oEM.map(0.2)).epsilon(g_fErrMargin) == 0.8);
	REQUIRE(Approx(oEM.map(0.3)).epsilon(g_fErrMargin) == 0.2);
	REQUIRE(Approx(oEM.map(0.5)).epsilon(g_fErrMargin) == 0.0);
	REQUIRE(Approx(oEM.map(0.7)).epsilon(g_fErrMargin) == 0.8);
	REQUIRE(Approx(oEM.map(0.75)).epsilon(g_fErrMargin) == 0.0);
	}

	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_MODULO;
	oEM.m_fA = -4.0;
	oEM.m_fB = 1.0;
	REQUIRE(Approx(oEM.map(0.0)).epsilon(g_fErrMargin) == 1.0);
	REQUIRE(Approx(oEM.map(0.2)).epsilon(g_fErrMargin) == 0.2);
	REQUIRE(Approx(oEM.map(0.3)).epsilon(g_fErrMargin) == 0.8);
	REQUIRE(Approx(oEM.map(0.6)).epsilon(g_fErrMargin) == 0.6);
	REQUIRE(Approx(oEM.map(1.0)).epsilon(g_fErrMargin) == 0.0);
	}

	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_MODULO;
	oEM.m_fA = -4.0;
	oEM.m_fB = 1.0;
	oEM.m_fMin = 0.2;
	oEM.m_fMax = 0.7;
	oEM.m_bShrink = true;
	REQUIRE(Approx(oEM.map(0.0)).epsilon(g_fErrMargin) == 0.7);
	REQUIRE(Approx(oEM.map(0.2)).epsilon(g_fErrMargin) == 0.3);
	REQUIRE(Approx(oEM.map(0.3)).epsilon(g_fErrMargin) == 0.6);
	REQUIRE(Approx(oEM.map(0.6)).epsilon(g_fErrMargin) == 0.5);
	REQUIRE(Approx(oEM.map(1.0)).epsilon(g_fErrMargin) == 0.2);
	}
}

TEST_CASE("testElapsedMapper, MAPPING_TYPE_OOR_ALT_MODULO")
{
	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_ALT_MODULO;
	for (double fElapsed = 0.0; fElapsed <= 1.0; fElapsed += 0.1) {
		REQUIRE(oEM.map(fElapsed) == fElapsed);
	}
	}

	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_ALT_MODULO;
	oEM.m_fA = 4.0;
	oEM.m_fB = 0.0;
	REQUIRE(oEM.map(0.0) == 0.0);
	REQUIRE(Approx(oEM.map(0.2)).epsilon(g_fErrMargin) == 0.8);
	REQUIRE(Approx(oEM.map(0.3)).epsilon(g_fErrMargin) == 0.8);
	REQUIRE(Approx(oEM.map(0.4)).epsilon(g_fErrMargin) == 0.4);
	REQUIRE(Approx(oEM.map(0.5)).epsilon(g_fErrMargin) == 0.0);
	REQUIRE(Approx(oEM.map(0.6)).epsilon(g_fErrMargin) == 0.4);
	REQUIRE(Approx(oEM.map(0.7)).epsilon(g_fErrMargin) == 0.8);
	REQUIRE(Approx(oEM.map(0.75)).epsilon(g_fErrMargin) == 1.0);
	REQUIRE(Approx(oEM.map(0.8)).epsilon(g_fErrMargin) == 0.8);
	REQUIRE(Approx(oEM.map(1.0)).epsilon(g_fErrMargin) == 0.0);
	}

	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_ALT_MODULO;
	oEM.m_fA = -4.0;
	oEM.m_fB = 1.0;
	REQUIRE(Approx(oEM.map(0.0)).epsilon(g_fErrMargin) == 1.0);
	REQUIRE(Approx(oEM.map(0.1)).epsilon(g_fErrMargin) == 0.6);
	REQUIRE(Approx(oEM.map(0.2)).epsilon(g_fErrMargin) == 0.2);
	REQUIRE(Approx(oEM.map(0.25)).epsilon(g_fErrMargin) == 0.0);
	REQUIRE(Approx(oEM.map(0.3)).epsilon(g_fErrMargin) == 0.2);
	REQUIRE(Approx(oEM.map(0.6)).epsilon(g_fErrMargin) == 0.6);
	REQUIRE(Approx(oEM.map(0.8)).epsilon(g_fErrMargin) == 0.2);
	REQUIRE(Approx(oEM.map(0.9)).epsilon(g_fErrMargin) == 0.6);
	REQUIRE(Approx(oEM.map(1.0)).epsilon(g_fErrMargin) == 1.0);
	}

	{
	EM oEM{};
	oEM.m_eMappingType = ElapsedMapper::MAPPING_TYPE_OOR_ALT_MODULO;
	oEM.m_fA = -4.0;
	oEM.m_fB = 1.0;
	oEM.m_fMin = 0.2;
	oEM.m_fMax = 0.7;
	oEM.m_bShrink = true;
	REQUIRE(Approx(oEM.map(0.0)).epsilon(g_fErrMargin) == 0.7);
	REQUIRE(Approx(oEM.map(0.1)).epsilon(g_fErrMargin) == 0.5);
	REQUIRE(Approx(oEM.map(0.2)).epsilon(g_fErrMargin) == 0.3);
	REQUIRE(Approx(oEM.map(0.25)).epsilon(g_fErrMargin) == 0.2);
	REQUIRE(Approx(oEM.map(0.3)).epsilon(g_fErrMargin) == 0.3);
	REQUIRE(Approx(oEM.map(0.6)).epsilon(g_fErrMargin) == 0.5);
	REQUIRE(Approx(oEM.map(0.8)).epsilon(g_fErrMargin) == 0.3);
	REQUIRE(Approx(oEM.map(0.9)).epsilon(g_fErrMargin) == 0.5);
	REQUIRE(Approx(oEM.map(0.95)).epsilon(g_fErrMargin) == 0.6);
	REQUIRE(Approx(oEM.map(1.0)).epsilon(g_fErrMargin) == 0.7);
	}
}

} // namespace testing

} // namespace stmg
