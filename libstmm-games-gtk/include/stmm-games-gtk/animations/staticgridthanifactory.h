/*
 * File:   staticgridthanifactory.h
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

#ifndef STMG_STATIC_GRID_TH_ANI_FACTORY_H
#define STMG_STATIC_GRID_TH_ANI_FACTORY_H

#include "stdthemeanimationfactory.h"

#include <cairomm/context.h>
#include <cairomm/refptr.h>
#include <cairomm/surface.h>

#include <memory>

#include "themeanimation.h"
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <stdint.h>

namespace stmg { class LevelAnimation; }
namespace stmg { class StaticGridAnimation; }
namespace stmg { class StdTheme; }
namespace stmg { class StdThemeContext; }

namespace stmg
{

class StaticGridThAniFactory : public StdThemeAnimationFactory
{
public:
	StaticGridThAniFactory(StdTheme* p1Owner, double fOpacity01) noexcept;

	bool supports(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;
	shared_ptr<ThemeAnimation> create(const shared_ptr<StdThemeContext>& refThemeContext, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;
private:
	class StaticGridThAni : public ThemeAnimation
	{
		friend class StaticGridThAniFactory;
	public:
		StaticGridThAni() noexcept = default;
		void reInit() noexcept {}
		int32_t getZ(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
		bool isStarted(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
		bool isDone(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
		void draw(int32_t nViewTick, int32_t nTotViewTicks, const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override;
		void onRemoved() noexcept override;
	private:
		// returns whether size changed
		bool getRectInternal() noexcept;
	private:
		Cairo::RefPtr<Cairo::ImageSurface> m_refSurf;
		Cairo::RefPtr<Cairo::Context> m_refCc;

		shared_ptr<StaticGridAnimation> m_refModel;
		int32_t m_nZ;

		NRect m_oPixRect;
		int32_t m_nPixRectTileW = 0;
		int32_t m_nPixRectTileH = 0;

		shared_ptr<StdThemeContext> m_refThemeContext;
		StaticGridThAniFactory* m_p1Owner;
	};

	double m_fOpacity01;

	Recycler<StaticGridThAniFactory::StaticGridThAni> m_oStaticGridThAnis;

private:
	StaticGridThAniFactory() = delete;
	StaticGridThAniFactory(const StaticGridThAniFactory& oSource) = delete;
	StaticGridThAniFactory& operator=(const StaticGridThAniFactory& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_STATIC_GRID_TH_ANI_FACTORY_H */

