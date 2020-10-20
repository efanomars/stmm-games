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
 * File:   ownertype.h
 */

#ifndef STMG_OWNER_TYPE_H
#define STMG_OWNER_TYPE_H

namespace stmg
{

enum class OwnerType
{
	GAME = 0
	, TEAM = 1
	, PLAYER = 2
};

} // namespace stmg

#endif	/* STMG_OWNER_TYPE_H */

