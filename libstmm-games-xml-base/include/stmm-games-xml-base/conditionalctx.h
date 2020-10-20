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
 * File:   conditionalctx.h
 */

#ifndef STMG_CONDITIONAL_CTX_H
#define STMG_CONDITIONAL_CTX_H

#include "parserctx.h"

#include <memory>

#include <stdint.h>

namespace stmg { class AppConfig; }
namespace stmg { class AppPreferences; }
namespace stmg { class Named; }
namespace xmlpp { class Element; }

namespace stmg
{

class ConditionalCtx : public ParserCtx
{
public:
	ConditionalCtx(const shared_ptr<AppPreferences>& refAppPreferences, Named& oNamed);
	ConditionalCtx(const shared_ptr<AppConfig>& refAppConfig, Named& oNamed);
	/** The context team.
	 * The preferences team number (not the level team number).
	 *
	 * Default implementation returns -1.
	 * @return The context team or -1.
	 */
	virtual int32_t getContextTeam() const { return -1; }
	/** The context mate.
	 * Default implementation returns -1.
	 * @return The context mate or -1.
	 */
	virtual int32_t getContextMate() const { return -1; }

	void addChecker(const xmlpp::Element* p0Element) override;
	void removeChecker(const xmlpp::Element* p0Element, bool bCheckChildElements, bool bCheckAttrs) override;
	using ParserCtx::removeChecker;
private:
	ConditionalCtx() = delete;
	ConditionalCtx(const ConditionalCtx& oSource) = delete;
	ConditionalCtx& operator=(const ConditionalCtx& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_CONDITIONAL_CTX_H */

