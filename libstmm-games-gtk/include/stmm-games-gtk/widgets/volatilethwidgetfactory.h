/*
 * File:   volatilethwidgetfactory.h
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

#ifndef STMG_VOLATILE_TH_WIDGET_FACTORY_H
#define STMG_VOLATILE_TH_WIDGET_FACTORY_H

#include "stdthemewidgetfactory.h"
#include "themecontainerwidget.h"
#include "themewidget.h"
#include "gtkutil/segmentedfunction.h"

#include <stmm-games/gamewidget.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>
#include <stmm-games/widgets/volatilewidget.h>

#include <array>
#include <memory>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmg { class Image; }
namespace stmg { class StdTheme; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace Glib { template <class T_CppObject> class RefPtr; }
namespace Pango { class Context; }

namespace stmg
{

class VolatileThWidgetFactory : public StdThemeWidgetFactory
{
public:
	explicit VolatileThWidgetFactory(StdTheme* p1Owner) noexcept;

	shared_ptr<ThemeWidget> create(const shared_ptr<GameWidget>& refGameWidget
									, double fTileWHRatio, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept override;
private:
	class VolatileTWidget : public ThemeContainerWidget
	{
	public:
		VolatileTWidget(VolatileThWidgetFactory* p1Owner
					, const shared_ptr<GameWidget>& refGameWidget, VolatileWidget* p0VolatileWidget
					, const shared_ptr<ThemeWidget>& refChildTW, double fTileWHRatio) noexcept;
		void reInit(VolatileThWidgetFactory* p1Owner
					, const shared_ptr<GameWidget>& refGameWidget, VolatileWidget* p0VolatileWidget
					, const shared_ptr<ThemeWidget>& refChildTW, double fTileWHRatio) noexcept;

		bool canChange() const noexcept override { return true; }

		NSize getSize() const noexcept override;
		NPoint getPos() const noexcept override;
		bool drawIfChanged(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override;
		void draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override;
		const std::pair<SegmentedFunction, SegmentedFunction>& getSizeFunctions(int32_t nLayoutConfig) const noexcept override;
		void placeAndMaybeResizeIn(const NRect& oRect) noexcept override;
		inline ThemeContainerWidget* getParent() const noexcept { return ThemeWidget::getParent(); }
		void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
	protected:
		void onRecalcSizeFunctions(ThemeWidget* p0ReferenceThemeWidget) noexcept override;
		void sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept override;
	private:
		void reInitCommon(const shared_ptr<ThemeWidget>& refChildTW) noexcept;
		std::pair<SegmentedFunction, SegmentedFunction> calcSizeFunctions(int32_t nLC) noexcept;
		void setPos(int32_t nPixX, int32_t nPixY) noexcept;
		void setSize(int32_t nPixW, int32_t nPixH) noexcept;
		void drawBase(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
		void drawVariable(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
		bool isChanged() const noexcept { return m_p0VolatileWidget->isChanged(); } 
		NSize getMinSize(int32_t nLayoutConfig) const noexcept;
		void loadAndDrawImg(const Cairo::RefPtr<Cairo::Context>& refCc, int32_t nImgId) noexcept;
	private:
		friend class VolatileThWidgetFactory;
		VolatileThWidgetFactory* m_p1Owner;
		VolatileWidget* m_p0VolatileWidget;
		//
		double m_fTileWHRatio;
		int32_t m_nTileW;
		int32_t m_nLayoutConfig;
		int32_t m_nPixX;
		int32_t m_nPixY;
		int32_t m_nPixW;
		int32_t m_nPixH;
		std::array<std::pair<SegmentedFunction, SegmentedFunction>, g_nTotLayoutConfigs> m_aSizeFunctions;
		struct ImgInfo
		{
			int32_t m_nImgId;
			shared_ptr<Image> m_refImg;
		};
		std::vector< ImgInfo > m_aCachedImgs; // Size: m_p0VolatileWidget->getValueImages().size() + 1
		//TODO for each image addCachedSize and removeCachedSize !
	};
private:
	Recycler<VolatileTWidget> m_oVolatileTWidgets;
};

} // namespace stmg

#endif	/* STMG_VOLATILE_TH_WIDGET_FACTORY_H */

