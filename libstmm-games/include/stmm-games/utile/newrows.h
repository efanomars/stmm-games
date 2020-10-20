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
 * File:   newrows.h
 */

#ifndef STMG_NEW_ROWS_H
#define STMG_NEW_ROWS_H

#include "tile.h"

#include "utile/randomtiles.h"

#include "util/intset.h"

#include <vector>
#include <memory>

#include <stdint.h>

namespace stmg { class RandomSource; }
namespace stmg { class TileBuffer; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class NewRows
{
private:
	friend class Distr;
	friend class DistrFixed;
	friend class DistrRandSingle;
	friend class DistrRandPosSingle;
	friend class DistrRandTiles;
	friend class DistrRandPosTiles;
	enum DISTR_TYPE
	{
		DISTR_TYPE_FIRST = 0 /**< First. */
		, DISTR_FIXED = 0 /**< Id associated with type DistrFixed. */
		, DISTR_RAND_SINGLE = 1 /**< Id associated with type DistrRandSingle. */
		, DISTR_RAND_POS_SINGLE = 2 /**< Id associated with type DistrRandPosSingle. */
		, DISTR_RAND_TILES = 3 /**< Id associated with type DistrRandTiles. */
		, DISTR_RAND_POS_TILES = 4 /**< Id associated with type DistrRandPosTiles. */
		, DISTR_TYPE_LAST = 4 /**< Last. */
	};
public:
	/**
	 */
	struct Distr
	{
		DISTR_TYPE getType() const noexcept
		{
			return m_eDistrType;
		}
	protected:
		Distr(DISTR_TYPE eDistrType) noexcept;
	private:
		DISTR_TYPE m_eDistrType;
	private:
		Distr() = delete;
	};
	/**
	 */
	struct DistrFixed : public Distr
	{
		std::vector<Tile> m_aTiles; /**< At least one tile must be non empty. */

		DistrFixed() noexcept
		: Distr(DISTR_FIXED)
		{
		}
	};
	/**
	 */
	struct DistrRandSingle : public Distr
	{
		int32_t m_nLeaveEmpty = 0; /**< The number of positions that should be left empty. Default is 0. */
		int32_t m_nFill = -1; /**< The number of positions to be filled (m_nLeaveEmpty permitting). If -1 all possible positions. Default is -1. */
		Tile m_oTile; /**< The tile used to fill the row. Cannot be empty. */

		DistrRandSingle() noexcept
		: Distr(DISTR_RAND_SINGLE)
		{
		}
	};
	/**
	 */
	struct DistrRandPosSingle : public Distr
	{
		IntSet m_oPositions; /**< The positions this tile distr should try to fill (if they're not already). */
		int32_t m_nLeaveEmpty = 0; /**< The number of positions that should be left empty. Must be &lt; m_oPositions.size(). Default is 0. */
		int32_t m_nFill = -1; /**< The number of positions to be filled (m_nLeaveEmpty permitting). If -1 all possible positions. Default is -1. */
		Tile m_oTile; /**< If non empty the tile that should be set at the positions. If empty m_nRandomTilesIdx is used. */

		DistrRandPosSingle() noexcept
		: Distr(DISTR_RAND_POS_SINGLE)
		{
		}
	};
	/**
	 */
	struct DistrRandTiles : public Distr
	{
		int32_t m_nLeaveEmpty = 0; /**< The number of positions that should be left empty. Default is 0. */
		int32_t m_nFill = -1; /**< The number of positions to be filled (m_nLeaveEmpty permitting). If -1 all possible positions. Default is -1. */
		int32_t m_nRandomTilesIdx = -1; /**< The index into Init::m_aRandomTiles or -1 if m_oTile is non empty. Default is -1. */

		DistrRandTiles() noexcept
		: Distr(DISTR_RAND_TILES)
		{
		}
	};
	/**
	 */
	struct DistrRandPosTiles : public Distr
	{
		IntSet m_oPositions; /**< The positions this tile distr should try to fill (if they're not already). */
		int32_t m_nLeaveEmpty = 0; /**< The number of positions that should be left empty. Must be &lt; m_oPositions.size(). Default is 0. */
		int32_t m_nFill = -1; /**< The number of positions to be filled (m_nLeaveEmpty permitting). If -1 all possible positions. Default is -1. */
		int32_t m_nRandomTilesIdx = -1; /**< The index into Init::m_aRandomTiles or -1 if m_oTile is non empty. Default is -1. */

		DistrRandPosTiles() noexcept
		: Distr(DISTR_RAND_POS_TILES)
		{
		}
	};
	struct NewRowGen
	{
		std::vector< unique_ptr<Distr> > m_aDistrs; /**< The tile distrs successively applied to the new row. Cannot be empty. Values cannot be null. */
	};
	struct Init
	{
		std::vector< RandomTiles::ProbTileGen > m_aRandomTiles; /**< The random tile generators. Can be empty. */
		std::vector< NewRowGen > m_aNewRowGens; /**< The new row generators. Cannot be empty. */
	};
	/** Constructor.
	 * @param oRandomSource The random source. Must persist during he active lifetime of this instance.
	 * @param oInit The initialization data.
	 */
	NewRows(RandomSource& oRandomSource, Init&& oInit) noexcept;
	NewRows(NewRows&& oSource) noexcept = default;

	/** The number of new row generators.
	 * @return The number of new row generators.
	 */
	int32_t getTotNewRowGens() const noexcept;
	/** The number of random tile generators.
	 * @return The number of random tile generators.
	 */
	int32_t getTotRandomTileGens() const noexcept;
	/** The minimum size of the row.
	 * This is the maximum of the size of all Distr objects.
	 * See createNewRow.
	 * @return The minimum size.
	 */
	int32_t getMinSize() const noexcept;
	/** Create a random row.
	 * If the tile buffer has non empty tiles when this function is called, they are not overwritten.
	 * @param nNewRowGenIdx The index into Init::m_aNewRowGens. Must be &gt;= 0 and &lt; getTotNewRowGens().
	 * @param oTileBuffer The tile buffer to fill. It's width must be &gt;= getMinSize().
	 * @param nRow The row to fill in oTileBuffer. Must be &gt;= 0 and &lt; oTileBuffer.getH().
	 */
	void createNewRow(int32_t nNewRowGenIdx, TileBuffer& oTileBuffer, int32_t nRow) noexcept;

private:
	void fillPositions(const DistrFixed& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow) noexcept;
	void fillPositions(const DistrRandSingle& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow) noexcept;
	void fillPositions(const DistrRandPosSingle& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow) noexcept;
	void fillPositions(const DistrRandTiles& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow) noexcept;
	void fillPositions(const DistrRandPosTiles& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow) noexcept;

private:
	RandomSource& m_oRandomSource;
	std::vector< unique_ptr<RandomTiles> > m_aRandomTiles;
	std::vector< NewRowGen > m_aNewRowGens;
	int32_t m_nMinW;
private:
	NewRows() = delete;
	NewRows(const NewRows& oSource) = delete;
	NewRows& operator=(const NewRows& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_NEW_ROWS_H */

