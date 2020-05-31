/*
 * File:   imagesequencethanifactory.h
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

#ifndef STMG_IMAGE_SEQUENCE_TH_ANI_FACTORY_H
#define STMG_IMAGE_SEQUENCE_TH_ANI_FACTORY_H

#include "stdthemeanimationfactory.h"

#include "themeanimation.h"

#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <cairomm/context.h>
#include <cairomm/refptr.h>

#include <memory>                      // for shared_ptr

#include <stdint.h>

namespace stmg { class DynAnimation; }
namespace stmg { class LevelAnimation; }
namespace stmg { class StdTheme; }
namespace stmg { class StdThemeContext; }

namespace stmg
{

class ImageSequenceThAniFactory : public StdThemeAnimationFactory
{
public:
	ImageSequenceThAniFactory(StdTheme* p1Owner, const shared_ptr<DynAnimation>& refDyn) noexcept;

	bool supports(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;
	shared_ptr<ThemeAnimation> create(const shared_ptr<StdThemeContext>& refThemeContext, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;

	shared_ptr<ThemeAnimation> createAny(const shared_ptr<StdThemeContext>& refThemeContext, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept;
private:
	class ImageSeqThAni : public ThemeAnimation
	{
		friend class ImageSequenceThAniFactory;
	public:
		ImageSeqThAni() noexcept = default;
		void reInit() noexcept {}
		int32_t getZ(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
		bool isStarted(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
		bool isDone(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
		void draw(int32_t nViewTick, int32_t nTotViewTicks, const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override;
		void onRemoved() noexcept override;
	private:
		int32_t calcBestPic(int32_t nViewTick, int32_t nTotViewTicks) const noexcept;
		FRect calcPicSize(int32_t nBestPic) const noexcept;
		void getRectAndBestPic(int32_t nViewTick, int32_t nTotViewTicks
								, NRect& oPixRect, int32_t& nBestPic) noexcept;
	private:
		double m_fInverseDuration;
		shared_ptr<DynAnimation> m_refAnimation;

		int32_t m_nLastPicDrawn;
		int32_t m_nCachePic;
		FRect m_oCacheRelRect;

		shared_ptr<LevelAnimation> m_refModel;
		int32_t m_nZ;

		shared_ptr<StdThemeContext> m_refThemeContext;
		ImageSequenceThAniFactory* m_p1Owner;
	};

	Recycler<ImageSequenceThAniFactory::ImageSeqThAni> m_oImageSeqThAnis;

	shared_ptr<DynAnimation> m_refDynAnimation;
private:
	ImageSequenceThAniFactory() = delete;
	ImageSequenceThAniFactory(const ImageSequenceThAniFactory& oSource) = delete;
	ImageSequenceThAniFactory& operator=(const ImageSequenceThAniFactory& oSource) = delete;
};


} // namespace stmg

#endif	/* STMG_IMAGE_SEQUENCE_TH_ANI_FACTORY_H */

