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
 * File:   helpers.h
 */

#ifndef STMG_HELPERS_H
#define STMG_HELPERS_H

#include "util/basictypes.h"
#include "util/direction.h"

namespace stmg
{

namespace Helpers
{

/** The remove rectangle of a Level::boardInsert().
 * @param oRect The insert rectangle.
 * @param eDir The direction.
 * @return The rectangle.
 */
NRect boardInsertRemove(const NRect& oRect, Direction::VALUE eDir) noexcept;
/** The moved rectangle of a Level::boardInsert() before the move.
 * @param oRect The insert rectangle.
 * @param eDir The The direction.
 * @return The rectangle.
 */
NRect boardInsertMovingPre(const NRect& oRect, Direction::VALUE eDir) noexcept;
/** The moved rectangle of a Level::boardInsert() after the move.
 * @param oRect The insert rectangle.
 * @param eDir The direction.
 * @return The rectangle.
 */
NRect boardInsertMovingPost(const NRect& oRect, Direction::VALUE eDir) noexcept;
/** The add rectangle of a Level::boardInsert().
 * @param oRect The insert rectangle.
 * @param eDir The direction.
 * @return The rectangle.
 */
NRect boardInsertAdd(const NRect& oRect, Direction::VALUE eDir) noexcept;

/** The remove rectangle of a Level::boardInsert() within a delimiting area.
 * @param oRect The insert rectangle.
 * @param eDir The direction.
 * @param oArea The delimiting area.
 * @return The rectangle.
 */
NRect boardInsertInAreaRemove(const NRect& oRect, Direction::VALUE eDir, const NRect& oArea) noexcept;
/** The moved rectangle of a Level::boardInsert() before the move within a delimiting area.
 * @param oRect The insert rectangle.
 * @param eDir The The direction.
 * @param oArea The delimiting area.
 * @return The rectangle.
 */
NRect boardInsertInAreaMovingPre(const NRect& oRect, Direction::VALUE eDir, const NRect& oArea) noexcept;
/** The moved rectangle of a Level::boardInsert() after the move within a delimiting area.
 * @param oRect The insert rectangle.
 * @param eDir The direction.
 * @param oArea The delimiting area.
 * @return The rectangle.
 */
NRect boardInsertInAreaMovingPost(const NRect& oRect, Direction::VALUE eDir, const NRect& oArea) noexcept;
/** The add rectangle of a Level::boardInsert() within a delimiting area.
 * @param oRect The insert rectangle.
 * @param eDir The direction.
 * @param oArea The delimiting area.
 * @return The rectangle.
 */
NRect boardInsertInAreaAdd(const NRect& oRect, Direction::VALUE eDir, const NRect& oArea) noexcept;

/** The remove rectangle of a Level::boardScroll().
 * @param oBoardSize The board size
 * @param eDir The direction.
 * @return The rectangle.
 */
inline NRect boardScrollRemove(const NSize& oBoardSize, Direction::VALUE eDir) noexcept
{
	return boardInsertRemove(NRect{0, 0, oBoardSize.m_nW, oBoardSize.m_nH}, eDir);
}
/** The moved rectangle of a Level::boardScroll() before the move.
 * @param oBoardSize The board size
 * @param eDir The direction.
 * @return The rectangle.
 */
inline NRect boardScrollMovingPre(const NSize& oBoardSize, Direction::VALUE eDir) noexcept
{
	return boardInsertMovingPre(NRect{0, 0, oBoardSize.m_nW, oBoardSize.m_nH}, eDir);
}
/** The moved rectangle of a Level::boardScroll() after the move.
 * @param oBoardSize The board size
 * @param eDir The direction.
 * @return The rectangle.
 */
inline NRect boardScrollMovingPost(const NSize& oBoardSize, Direction::VALUE eDir) noexcept
{
	return boardInsertMovingPost(NRect{0, 0, oBoardSize.m_nW, oBoardSize.m_nH}, eDir);
}
/** The add rectangle of a Level::boardScroll().
 * @param oBoardSize The board size
 * @param eDir The direction.
 * @return The rectangle.
 */
inline NRect boardScrollAdd(const NSize& oBoardSize, Direction::VALUE eDir) noexcept
{
	return boardInsertAdd(NRect{0, 0, oBoardSize.m_nW, oBoardSize.m_nH}, eDir);
}

} // namespace Helpers

} // namespace stmg

#endif	/* STMG_HELPERS_H */

