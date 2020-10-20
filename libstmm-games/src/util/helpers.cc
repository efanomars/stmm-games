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
 * File:   helpers.cc
 */

#include "util/helpers.h"

#include "util/basictypes.h"
#include "util/direction.h"

//#include <cassert>
//#include <iostream>


namespace stmg
{

namespace Helpers
{

NRect boardInsertRemove(const NRect& oRect, Direction::VALUE eDir) noexcept
{
	switch (eDir) {
	case Direction::DOWN: {
		return NRect{oRect.m_nX, oRect.m_nY + oRect.m_nH - 1, oRect.m_nW, 1};
	}
	case Direction::UP: {
		return NRect{oRect.m_nX, oRect.m_nY, oRect.m_nW, 1};
	}
	case Direction::LEFT: {
		return NRect{oRect.m_nX, oRect.m_nY, 1, oRect.m_nH};
	}
	case Direction::RIGHT: {
		return NRect{oRect.m_nX + oRect.m_nW - 1, oRect.m_nY, 1, oRect.m_nH};
	}
	default: {
		return NRect{};
	}
	}
}

NRect boardInsertMovingPre(const NRect& oRect, Direction::VALUE eDir) noexcept
{
	switch (eDir) {
	case Direction::DOWN: {
		return NRect{oRect.m_nX, oRect.m_nY, oRect.m_nW, oRect.m_nH - 1};
	}
	case Direction::UP: {
		return NRect{oRect.m_nX, oRect.m_nY + 1, oRect.m_nW, oRect.m_nH - 1};
	}
	case Direction::LEFT: {
		return NRect{oRect.m_nX + 1, oRect.m_nY, oRect.m_nW - 1, oRect.m_nH};
	}
	case Direction::RIGHT: {
		return NRect{oRect.m_nX, oRect.m_nY, oRect.m_nW -1, oRect.m_nH};
	}
	default: {
		return NRect{};
	}
	}
}
NRect boardInsertMovingPost(const NRect& oRect, Direction::VALUE eDir) noexcept
{
	switch (eDir) {
	case Direction::DOWN: {
		return NRect{oRect.m_nX, oRect.m_nY + 1, oRect.m_nW, oRect.m_nH - 1};
	}
	case Direction::UP: {
		return NRect{oRect.m_nX, oRect.m_nY, oRect.m_nW, oRect.m_nH - 1};
	}
	case Direction::LEFT: {
		return NRect{oRect.m_nX, oRect.m_nY, oRect.m_nW - 1, oRect.m_nH};
	}
	case Direction::RIGHT: {
		return NRect{oRect.m_nX + 1, oRect.m_nY, oRect.m_nW -1, oRect.m_nH};
	}
	default: {
		return NRect{};
	}
	}
}

NRect boardInsertAdd(const NRect& oRect, Direction::VALUE eDir) noexcept
{
	switch (eDir) {
	case Direction::DOWN: {
		return NRect{oRect.m_nX, oRect.m_nY, oRect.m_nW, 1};
	}
	case Direction::UP: {
		return NRect{oRect.m_nX, oRect.m_nY + oRect.m_nH - 1, oRect.m_nW, 1};
	}
	case Direction::LEFT: {
		return NRect{oRect.m_nX + oRect.m_nW - 1, oRect.m_nY, 1, oRect.m_nH};
	}
	case Direction::RIGHT: {
		return NRect{oRect.m_nX, oRect.m_nY, 1, oRect.m_nH};
	}
	default: {
		return NRect{};
	}
	}
}

NRect boardInsertInAreaRemove(const NRect& oRect, Direction::VALUE eDir, const NRect& oArea) noexcept
{
	NRect oIntersect = NRect::intersectionRect(oRect, oArea);
	if (oIntersect.m_nW == 0) {
		return oIntersect;
	}
	return boardInsertRemove(oIntersect, eDir);
}
NRect boardInsertInAreaMovingPre(const NRect& oRect, Direction::VALUE eDir, const NRect& oArea) noexcept
{
	NRect oIntersect = NRect::intersectionRect(oRect, oArea);
	if (oIntersect.m_nW == 0) {
		return oIntersect;
	}
	return boardInsertMovingPre(oIntersect, eDir);
}
NRect boardInsertInAreaMovingPost(const NRect& oRect, Direction::VALUE eDir, const NRect& oArea) noexcept
{
	NRect oIntersect = NRect::intersectionRect(oRect, oArea);
	if (oIntersect.m_nW == 0) {
		return oIntersect;
	}
	return boardInsertMovingPost(oIntersect, eDir);
}
NRect boardInsertInAreaAdd(const NRect& oRect, Direction::VALUE eDir, const NRect& oArea) noexcept
{
	NRect oIntersect = NRect::intersectionRect(oRect, oArea);
	if (oIntersect.m_nW == 0) {
		return oIntersect;
	}
	return boardInsertAdd(oIntersect, eDir);
}

} // namespace Helpers

} // namespace stmg
