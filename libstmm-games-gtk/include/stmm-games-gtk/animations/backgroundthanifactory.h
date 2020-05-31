/*
 * File:   backgroundthanifactory.h
 *
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

#ifndef STMG_BACKGROUND_TH_ANI_FACTORY_H
#define STMG_BACKGROUND_TH_ANI_FACTORY_H

#include "stdthemeanimationfactory.h"

#include "themeanimation.h"

#include <stmm-games/util/recycler.h>
#include <stmm-games/util/basictypes.h>

#include <memory>

#include <stdint.h>

namespace stmg { class BackgroundAnimation; }
namespace stmg { class Image; }
namespace stmg { class LevelAnimation; }
namespace stmg { class StdTheme; }
namespace stmg { class StdThemeContext; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

class BackgroundThAniFactory : public StdThemeAnimationFactory
{
public:
	explicit BackgroundThAniFactory(StdTheme* p1Owner) noexcept;

	bool supports(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;
	shared_ptr<ThemeAnimation> create(const shared_ptr<StdThemeContext>& refThemeContext, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;
private:
	class BackgroundThAni : public ThemeAnimation
	{
		friend class BackgroundThAniFactory;
	public:
		BackgroundThAni() noexcept = default;
		void reInit() noexcept {}
		int32_t getZ(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
		bool isStarted(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
		bool isDone(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
		void draw(int32_t nViewTick, int32_t nTotViewTicks, const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override;
		void onRemoved() noexcept override;
	private:
		int32_t m_nCachedImgId;
		shared_ptr<Image> m_refCachedImage;
		NSize m_oCachedSize;

		shared_ptr<BackgroundAnimation> m_refModel;
		int32_t m_nZ;

		shared_ptr<StdThemeContext> m_refThemeContext;
		BackgroundThAniFactory* m_p1Owner;
	};

	Recycler<BackgroundThAniFactory::BackgroundThAni> m_oBackgroundThAnis;

private:
	BackgroundThAniFactory() = delete;
	BackgroundThAniFactory(const BackgroundThAniFactory& oSource) = delete;
	BackgroundThAniFactory& operator=(const BackgroundThAniFactory& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_BACKGROUND_TH_ANI_FACTORY_H */

