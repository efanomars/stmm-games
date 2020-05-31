/*
 * File:  previewwidget.h
 *
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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

#ifndef STMG_PREVIEW_WIDGET_H
#define STMG_PREVIEW_WIDGET_H

#include "relsizedgamewidget.h"
#include "block.h"

#include <string>
#include <vector>

#include <stdint.h>

namespace stmg
{

/** Blocks preview widget.
 */
class PreviewWidget : public RelSizedGameWidget
{
public:
	struct LocalInit
	{
		int32_t m_nMinTilesW = 1; /**< The minimum number of horizontal tiles. Default: `1`. */
		int32_t m_nMinTilesH = 1; /**< The minimum number of vertical tiles. Default: `1`. */
	};
	struct Init : public RelSizedGameWidget::Init, public LocalInit
	{
	};

	/** Sets blocks and text of the preview widget.
	 * @param sText Single line short text. Can be empty.
	 * @param oBlocks The blocks to show. Can be empty.
	 */
	void set(const std::string& sText, std::vector<Block>& oBlocks) noexcept;

	/** The minimum number of horizontal tiles. */
	inline int32_t getMinTilesW() const noexcept { return m_oData.m_nMinTilesW; }
	/** The minimum number of vertical tiles. */
	inline int32_t getMinTilesH() const noexcept { return m_oData.m_nMinTilesH; }

	/** The text shown along with the blocks.
	 * @return The string. Can be empty.
	 */
	inline const std::string& getText() const noexcept { return m_sText; }
	/** The shown blocks.
	 * @return The blocks. Can be empty vector.
	 */
	inline const std::vector<Block>& getBlocks() const noexcept { return m_aBlocks; }

	/** Whether set was called since the last game tick.*/
	bool isChanged() const noexcept;

	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit PreviewWidget(Init&& oInit) noexcept;

	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Reinitialization.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept;
private:
	void changed() noexcept;
private:

	LocalInit m_oData;

	std::string m_sText;
	std::vector<Block> m_aBlocks;
	int32_t m_nChangeGameTick;
};


} // namespace stmg

#endif	/* STMG_PREVIEW_WIDGET_H */

