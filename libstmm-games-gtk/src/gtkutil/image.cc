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
 * File:   image.cc
 */

#include "gtkutil/image.h"
#include "gtkutil/cachedsurfaces.h"

#include <stmm-games/util/basictypes.h>

#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <gdkmm.h>
#include <giomm.h>

#include <string>
#include <cassert>
#include <iostream>

namespace stmg
{

Image::Image(const File& oImageFile) noexcept
: m_oImageFile(oImageFile)
, m_nW(0)
, m_nH(0)
, m_pHandle(nullptr)
, m_nSubX(s_nNotSubX)
, m_nSubY(s_nNotSubX)
{
	assert(oImageFile.isDefined());
}
Image::Image(const shared_ptr<Image>& refImage, int32_t nSubX, int32_t nSubY, int32_t nSubW, int32_t nSubH) noexcept
: m_nW(nSubW)
, m_nH(nSubH)
, m_pHandle(nullptr)
, m_nSubX(nSubX)
, m_nSubY(nSubY)
, m_refMasterImage(refImage)
{
	assert(refImage);
	assert(m_nSubX < s_nNotSubX);
	assert(m_nSubY < s_nNotSubX);
	assert((m_nW > 0) && (m_nH > 0));
}
Image::Image(const shared_ptr<Image>& refImage, const NRect& oSubRect) noexcept
: Image(refImage, oSubRect.m_nX, oSubRect.m_nY, oSubRect.m_nW, oSubRect.m_nH)
{
}
Image::~Image() noexcept
{
	if (m_pHandle) {
		::g_object_unref(m_pHandle);
	}
}
bool Image::isLoaded() const noexcept
{
	if (m_nSubX != s_nNotSubX) {
		return m_refMasterImage->isLoaded();
	}
	return ((m_pHandle != nullptr) || m_refPixbuf);
}
bool Image::load() noexcept
{
	if (isLoaded()) {
		return true; //---------------------------------------------------------
	}
	if (m_nSubX != s_nNotSubX) {
		// Subimg
		if (!m_refMasterImage->isLoaded()) {
			return m_refMasterImage->load(); //---------------------------------
		}
		return true; //---------------------------------------------------------
	}
	// either svg or some other image format
	// let's try svg first since Pixbuf also loads svgs as a raster
	GError*	pError = nullptr;
	const bool bIsBuffered = m_oImageFile.isBuffered();
	if (bIsBuffered) {
		m_pHandle = rsvg_handle_new_from_data(m_oImageFile.getBuffer(), m_oImageFile.getBufferSize(), &pError);
	} else {
		m_pHandle = rsvg_handle_new_from_file(m_oImageFile.getFullPath().c_str(), &pError);
	}
	if (m_pHandle != nullptr) { // Svg
		RsvgDimensionData oDim;
		rsvg_handle_get_dimensions(m_pHandle, &oDim);
		m_nW = oDim.width;
		m_nH = oDim.height;
		return true; //---------------------------------------------------------
	}
	try {
		if (bIsBuffered) {
			Glib::RefPtr<Gio::MemoryInputStream> refInput = Gio::MemoryInputStream::create();
			refInput->add_data(m_oImageFile.getBuffer(), m_oImageFile.getBufferSize());
			m_refPixbuf = Gdk::Pixbuf::create_from_stream(refInput);
		} else {
			m_refPixbuf = Gdk::Pixbuf::create_from_file(m_oImageFile.getFullPath());
		}
	} catch (...) {
		return false; //--------------------------------------------------------
	}
	m_nW = m_refPixbuf->get_width();
	m_nH = m_refPixbuf->get_height();
	return true;
}
NSize Image::getNaturalSize() noexcept
{
	load();
	NSize oSize;
	oSize.m_nW = m_nW;
	oSize.m_nH = m_nH;
	return oSize;
}
void Image::addCachedSize(NSize oSize) noexcept
{
	m_oCached.addSize(oSize.m_nW, oSize.m_nH);
}
void Image::addCachedSize(int32_t nW, int32_t nH) noexcept
{
	m_oCached.addSize(nW, nH);
}
void Image::releaseCachedSize(NSize oSize) noexcept
{
	m_oCached.subSize(oSize.m_nW, oSize.m_nH);
}
void Image::releaseCachedSize(int32_t nW, int32_t nH) noexcept
{
	m_oCached.subSize(nW, nH);
}
Cairo::RefPtr<Cairo::Surface> Image::getAsSurface(int32_t nW, int32_t nH) noexcept
{
//std::cout << "StdTheme::SubImage::getAsSurface  nW=" << nW << " nH=" << nH << '\n';
	bool bCreated;
	Cairo::RefPtr<Cairo::Surface> refSurf = m_oCached.getRgba(nW, nH, bCreated);
	if (bCreated) {
		Cairo::RefPtr<Cairo::Context> refCc = Cairo::Context::create(refSurf);
		draw(refCc, 0,0, nW,nH);
	}
	return refSurf;
}
const Cairo::RefPtr<Cairo::Surface>& Image::getAsCachedSurface(int32_t nW, int32_t nH) noexcept
{
//std::cout << "StdTheme::SubImage::getAsSurface  nW=" << nW << " nH=" << nH << '\n';
	bool bCreated;
	const Cairo::RefPtr<Cairo::Surface>& refSurf = m_oCached.getCachedRgba(nW, nH, bCreated);
	if (bCreated) {
		Cairo::RefPtr<Cairo::Context> refCc = Cairo::Context::create(refSurf);
		draw(refCc, 0, 0, nW, nH);
	}
	return refSurf;
}
Cairo::RefPtr<Cairo::Surface> Image::getAsMaskSurface(int32_t nW, int32_t nH) noexcept
{
	bool bCreated;
	Cairo::RefPtr<Cairo::Surface> refSurf = m_oCached.getA(nW, nH, bCreated);
	if (bCreated) {
		Cairo::RefPtr<Cairo::Context> refCc = Cairo::Context::create(refSurf);
		draw(refCc, 0,0, nW,nH);
	}
	return refSurf;
}
const Cairo::RefPtr<Cairo::Surface>& Image::getAsCachedMaskSurface(int32_t nW, int32_t nH) noexcept
{
	bool bCreated;
	const Cairo::RefPtr<Cairo::Surface>& refSurf = m_oCached.getCachedA(nW, nH, bCreated);
	if (bCreated) {
		Cairo::RefPtr<Cairo::Context> refCc = Cairo::Context::create(refSurf);
		draw(refCc, 0,0, nW,nH);
	}
	return refSurf;
}
void Image::draw(const Cairo::RefPtr<Cairo::Context>& refCc, int32_t nX, int32_t nY, int32_t nW, int32_t nH) noexcept
{
	if ((nW <= 0) || (nH <= 0)) {
		return;
	}
	load();
	if ((m_nW <= 0) || (m_nH <= 0)) {
		return;
	}
	if (m_nSubX != s_nNotSubX) {
		// SubImage
		refCc->save();
		refCc->rectangle(nX, nY, nW, nH);
		refCc->clip();

		NSize oImgSize = m_refMasterImage->getNaturalSize();
		refCc->translate(nX, nY);
		const double fScaleX = 1.0 * nW / m_nW;
		const double fScaleY = 1.0 * nH / m_nH;
		const double fPaX = - fScaleX * m_nSubX;
		const double fPaY = - fScaleY * m_nSubY;
		const double fPaW = fScaleX * oImgSize.m_nW;
		const double fPaH = fScaleY * oImgSize.m_nH;
		m_refMasterImage->draw(refCc, fPaX, fPaY, fPaW, fPaH);
		refCc->restore();
		return; //--------------------------------------------------------------
	}
//std::cout << "Image::draw nX=" << nX << " nY=" << nY << " nW=" << nW << " nH=" << nH << " m_nW=" << m_nW << " m_nH=" << m_nH << '\n';
	refCc->save();
	refCc->translate(nX, nY);
	const double fScaleX = 1.0 * nW / m_nW;
	const double fScaleY = 1.0 * nH / m_nH;
	refCc->scale(fScaleX, fScaleY);
	if (m_pHandle != nullptr) {
		// Svg
		const bool bSucceded = rsvg_handle_render_cairo(m_pHandle, refCc->cobj());
		if (!bSucceded) {
			std::cerr << "SvgImage::draw() rsvg_handle_render_cairo error" << '\n';
		}
	} else {
		Gdk::Cairo::set_source_pixbuf(refCc, m_refPixbuf, 0, 0);
		refCc->paint();
	}
	refCc->restore();
}

} // namespace stmg
