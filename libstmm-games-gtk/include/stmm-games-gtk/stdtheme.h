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
 * File:   stdtheme.h
 */

#ifndef STMG_STD_THEME_H
#define STMG_STD_THEME_H

#include "theme.h"
#include "stdthemecontext.h"
#include "stdthemeanimationfactory.h"
#include "stdthememodifier.h"
#include "stdthemewidgetfactory.h"

#include "gtkutil/tilesizing.h"

#include <stmm-games-file/file.h>
#include <stmm-games/tile.h>
#include <stmm-games/traitset.h>
#include <stmm-games/named.h>

#include <stmm-games/util/recycler.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/intset.h>
#include <stmm-games/util/namedindex.h>

#include <unordered_map>
#include <memory>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <stddef.h>
#include <stdint.h>

namespace stmg { class ThemeContext; }
namespace stmg { class ThemeSound; }
namespace stmi { class PlaybackCapability; }
namespace stmg { class GameWidget; }
namespace stmg { class Image; }
namespace stmg { class LevelAnimation; }
namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class ThemeAnimation; }
namespace stmg { class ThemeWidget; }
namespace stmg { class TileAni; }
namespace stmg { class StdThemeSound; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace Glib { template <class T_CppObject> class RefPtr; }
namespace Pango { class Context; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;


class StdTheme : public Theme
{
public:
	/** Constructor.
	 */
	StdTheme() noexcept;

	/** Add an image file name and associated file.
	 * If the name already exists does nothing and returns false.
	 * @param sImgFileName The image file name. Cannot be empty. Example: "ball.svg".
	 * @param oFile The associated file. Must be defined. Example: "/usr/share/stmm-games/themes/simple.thm/images/ball.svg".
	 */
	bool addKnownImageFile(const std::string& sImgFileName, const File& oFile) noexcept;
	/** Whether an image file name is already defined.
	 * @param sImgFileName The image file name. Cannot be empty. Example: "ball.svg".
	 * @return Whether already defined.
	 */
	bool knowsImageFile(const std::string& sImgFileName) const noexcept;
	/** Get known image file by file name.
	 * @param sImgFileName The image file name. Cannot be empty. Example: "ball.svg".
	 * @return The file. If not found File::isUndefined() is true.
	 */
	const File& getKnownImageFile(const std::string& sImgFileName) const noexcept;

	/** Add a sound file name and associated file.
	 * If the name already exists does nothing and returns false.
	 * @param sSndFileName The sound file name. Cannot be empty. Example: "explode.wav".
	 * @param oFile The associated file. Must be defined. Example: "/usr/share/stmm-games/themes/simple.thm/sounds/explode.wav".
	 */
	bool addKnownSoundFile(const std::string& sSndFileName, const File& oFile) noexcept;
	/** Whether a sound file name is already defined.
	 * @param sSndFileName The sound file name. Cannot be empty. Example: "explode.wav".
	 * @return Whether already defined.
	 */
	bool knowsSoundFile(const std::string& sSndFileName) const noexcept;
	/** Get known sound file by file name.
	 * @param sSndFileName The sound file name. Cannot be empty. Example: "explode.wav".
	 * @return The file. If not found File::isUndefined() is true.
	 */
	const File& getKnownSoundFile(const std::string& sSndFileName) const noexcept;

	NSize getBestTileSize(int32_t nHintTileW) const noexcept override;
	double getTileWHRatio() const noexcept override;

	shared_ptr<ThemeContext> createContext(NSize oTileWH, bool bRegister
											, double fSoundScaleX, double fSoundScaleY, double fSoundScaleZ
											, const Glib::RefPtr<Pango::Context>& refFontContext
											, RuntimeVariablesEnv* p0RuntimeVariablesEnv) noexcept override;
	shared_ptr<ThemeWidget> createWidget(const shared_ptr<GameWidget>& refGameWidget, double fTileWHRatio
										, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept override;
	//
	const Named& getNamed() const noexcept override { return m_oNamed; }
	/** The named values.
	 * Contains the strings relevant to the game.
	 * @return The named values.
	 */
	inline Named& getNamed() noexcept { return m_oNamed; }

	/** The image by image file name.
	 * This will load the image if necessary.
	 * @param sImgFileName The image file name. Cannot be empty. Example: "ball.svg".
	 * @return The image or null if not defined.
	 */
	const shared_ptr<Image>& getImageByFileName(const std::string& sImgFileName) noexcept;
	/** The image by string image id.
	 * This will load the image if necessary.
	 * @param sImgId The image string id. Cannot be empty. Example: "ball".
	 * @return The image or null if not defined.
	 */
	const shared_ptr<Image>& getImageById(const std::string& sImgId) noexcept;
	/** The image by image id.
	 * This will load the image if necessary.
	 *
	 * To get the id from a string id use getNamed().images().getIndex(sImgId).
	 *
	 * This function is faster than getImageById(const std::string& sImgId).
	 * @param nImgId The image id. Must be &gt;= 0.
	 * @return The image or null if not defined.
	 */
	const shared_ptr<Image>& getImageById(int32_t nImgId) noexcept;

	/** Adds a named color.
	 * If the name already exists. this function does nothing.
	 * @param sColorName The color name. Cannot be empty.
	 * @param nR The red.
	 * @param nG The green.
	 * @param nB The blue.
	 */
	void addColorName(const std::string& sColorName, uint8_t nR, uint8_t nG, uint8_t nB) noexcept;
	/** Set the color of the palette.
	 * Uninitialized palette numbers are black (0,0,0).
	 * @param nPal The palette number. Must be &gt;= 0 and &lt;= TileColor::COLOR_PAL_LAST.
	 * @param nR The red.
	 * @param nG The green.
	 * @param nB The blue.
	 */
	void addColorPal(int32_t nPal, uint8_t nR, uint8_t nG, uint8_t nB) noexcept;
	/** Get a named color.
	 * @param sColorName The color name.
	 * @param nR [output] The red.
	 * @param nG [output] The green.
	 * @param nB [output] The blue.
	 * @return Whether the name was defined.
	 */
	bool getNamedColor(const std::string& sColorName, uint8_t& nR, uint8_t& nG, uint8_t& nB) const noexcept;
	/** Get a named color by index.
	 * @param nIdx The index into getNamed().colors().
	 * @param nR [output] The red.
	 * @param nG [output] The green.
	 * @param nB [output] The blue.
	 * @return Whether the (indexed) name was defined.
	 */
	bool getNamedColor(int32_t nIdx, uint8_t& nR, uint8_t& nG, uint8_t& nB) const noexcept;
	/** Get palette color.
	 * @param nPal The palette number. Must be &gt;= 0 and &lt;= TileColor::COLOR_PAL_LAST.
	 * @param nR [output] The red.
	 * @param nG [output] The green.
	 * @param nB [output] The blue.
	 */
	void getPalColor(int32_t nPal, uint8_t& nR, uint8_t& nG, uint8_t& nB) const noexcept;
	/** Get the rgb of a TileColor.
	 * @param oColor The tile color.
	 * @param nR [output] The red.
	 * @param nG [output] The green.
	 * @param nB [output] The blue.
	 */
	void getColorRgb(const TileColor& oColor, uint8_t& nR, uint8_t& nG, uint8_t& nB) const noexcept;

	/** Set the default font.
	 * @param sFontDesc The font name. Cannot be empty.
	 */
	void setDefaultFont(const std::string& sFontDesc) noexcept;
	/** Add named font.
	 * @param sFontName The font name. Cannot be empty.
	 * @param sFontDesc The font description as used with Pango::FontDescription. Cannot be empty.
	 */
	void addFontName(const std::string& sFontName, const std::string& sFontDesc) noexcept;
	/** Get a named font.
	 * @param sFontName The font name. Cannot be empty.
	 * @param sFontDesc [output] The font description (see Pango::FontDescription).
	 * @return Whether the font name was defined.
	 */
	bool getNamedFont(const std::string& sFontName, std::string& sFontDesc) noexcept;
	/** Get a named font by index.
	 * @param nFontIdx The index into getNamed().fonts().
	 * @param sFontDesc [output] The font description (see Pango::FontDescription).
	 * @return Whether the font name was defined.
	 */
	bool getNamedFont(int32_t nFontIdx, std::string& sFontDesc) noexcept;
	/** Whether the default font was set.
	 * @return Whether set.
	 */
	bool isDefaultFontDefined() const noexcept;
	/** Get default font description.
	 * @return The default font description (see Pango::FontDescription).
	 */
	const std::string& getDefaultFont() const noexcept;
	/** Get font description by named index or default.
	 * @param nFontIdx The index into getNamed().fonts(). If -1 or invalid default is returned.
	 * @return The font description (see Pango::FontDescription).
	 */
	const std::string& getFontDesc(int32_t nFontIdx) const noexcept;

	/** Whether a image string id was defined.
	 * @param sImgId The image string id. Cannot be empty. Example: "ball".
	 * @return Whether id defined.
	 */
	bool hasImageId(const std::string& sImgId) const noexcept;
	/** The image file name of a image string id.
	 * @param sImgId The image string id. Cannot be empty. Example: "ball".
	 * @return The file name or empty string if not defined.
	 */
	const std::string& getImageIdFileName(const std::string& sImgId) const noexcept;
	/** Define string image id for a known image file name.
	 * The image file name must already have been added with addKnownImageFile().
	 * @param sImgId The image string id. Cannot be empty. Example: "ball".
	 * @param sImgFileName The image file name. Cannot be empty. Example: "ball.svg".
	 */
	void addImageId(const std::string& sImgId, const std::string& sImgFileName) noexcept;

	/** Whether a sound string id was defined.
	 * @param sSndId The sound string id. Cannot be empty. Example: "explode".
	 * @return Whether id defined.
	 */
	bool hasSoundId(const std::string& sSndId) const noexcept;
	/** The sound file name of a sound string id.
	 * @param sSndId The sound string id. Cannot be empty. Example: "explode".
	 * @return The file name or empty string if not defined.
	 */
	const std::string& getSoundIdFileName(const std::string& sSndId) const noexcept;
	/** Define string sound id for a known sound file name.
	 * The sound file name must already have been added with addKnownSoundFile().
	 * @param sSndId The sound string id. Cannot be empty. Example: "explode".
	 * @param sSndFileName The sound file name. Cannot be empty. Example: "explode.wav".
	 */
	void addSoundId(const std::string& sSndId, const std::string& sSndFileName) noexcept;

	/** Whether a named array of images was definrd.
	 * @param sArrayId The array string id. Cannot be empty.
	 * @return Whether array defined.
	 */
	bool hasArray(const std::string& sArrayId) const noexcept;
	/** The size of the array.
	 * @param sArrayId The array string id. Cannot be empty.
	 * @return The number of images in the array.
	 */
	int32_t arraySize(const std::string& sArrayId) const noexcept;
	/** Add an array of sub images of a master image.
	 * If an array (of any kind) with the given name already exists, this function
	 * does nothing.
	 *
	 * The sub images start at position 0,0 within the master image (defined by sImgFileName).
	 * @param sArrayId The string array id. Cannot be empty.
	 * @param sImgFileName The image file. Must have been added with addKnownImageFile().
	 * @param nImgW The sub images width in pixwls. Must be positive.
	 * @param nImgH The sub images height in pixwls. Must be positive.
	 * @param nSpacingX The horizontal spacing of the sub images. Cannot be negative.
	 * @param nSpacingY The vertical spacing of the sub images. Cannot be negative.
	 * @param nPerRow The number of sub images per row. Must be positive.
	 * @param nArraySize The total number of sub images. Must be positive.
	 */
	void addSubArray(const std::string& sArrayId, const std::string& sImgFileName
					, int32_t nImgW, int32_t nImgH, int32_t nSpacingX, int32_t nSpacingY
					, int32_t nPerRow, int32_t nArraySize) noexcept;
	/** Add an array of images defined by image files.
	 * If an array (of any kind) with the given name already exists, this function
	 * does nothing.
	 *
	 * @param sArrayId The string array id. Cannot be empty.
	 * @param aFileArray The image file names. Must have been added with addKnownImageFile(). Cannot be empty.
	 */
	void addFileArray(const std::string& sArrayId, const std::vector<std::string>& aFileArray) noexcept;

	/** Tells whether a theme widget factory with the given name exists.
	 * The names are stored getNamed().widgets() and are used in the GameWidget
	 * to express a preference for a factory.
	 * @param sName The widget factory name. Cannot be empty.
	 * @return Whether factory exists.
	 */
	bool hasWidgetFactory(const std::string& sName) const noexcept;
	/** Add a theme widget factory.
	 * If a factory with the given name already exists nothing is added.
	 * @param sName The name of the factory. Cannot be empty.
	 * @param refWidgetFactory The theme widget factory. Cannot be null.
	 * @param bAnonymousWidgets Whether the factory should be used for anonymous model widgets.
	 */
	void addWidgetFactory(const std::string& sName, unique_ptr<StdThemeWidgetFactory> refWidgetFactory
						, bool bAnonymousWidgets) noexcept;
	/** Get the theme widget factory with a given name.
	 * @param sName The name of the factory. Cannot be empty.
	 * @return The factory or null if not defined.
	 */
	StdThemeWidgetFactory* getWidgetFactory(const std::string& sName) noexcept;

	/** Tells whether a theme animation factory with the given name exists.
	 * The names are stored getNamed().animations() and are used in the GameWidget
	 * to express a preference for a factory.
	 * @param sName The animation factory name. Cannot be empty.
	 * @return Whether factory exists.
	 */
	bool hasAnimationFactory(const std::string& sName) const noexcept;
	/** Add a theme animation factory.
	 * If a factory with the given name already exists nothing is added.
	 * @param sName The name. Cannot be empty.
	 * @param refAnimationFactory The factory. Cannot be null.
	 * @param bAnonymousAnimations Whether the factory should be used for anonymous model animations.
	 */
	void addAnimationFactory(const std::string& sName, unique_ptr<StdThemeAnimationFactory> refAnimationFactory
							, bool bAnonymousAnimations) noexcept;
	/** Get the theme animation factory with a given name.
	 * @param sName The name of the factory. Cannot be empty.
	 * @return The factory or null if not defined.
	 */
	StdThemeAnimationFactory* getAnimationFactory(const std::string& sName) noexcept;

	/** Tells whether a tile animation with the given id exists.
	 * @param sId The id. Cannot be e empty.
	 * @return Whether a tile animation was added.
	 */
	bool hasTileAniId(const std::string& sId) const noexcept;
	/** Add a tile animation.
	 * If a tile animation with the given id already exists nothing is added.
	 * @param sId The id. Cannot be e empty.
	 * @param refTileAni The tile animation. Cannot be null.
	 */
	void addTileAni(const std::string& sId, const shared_ptr<TileAni>& refTileAni) noexcept;
	/** Get a tile animation
	 * @param sId The id. Must exist.
	 * @return The tile animation.
	 */
	const shared_ptr<TileAni>& getTileAni(const std::string& sId) noexcept;

	/** Get the assign id from the string assign id.
	 * @param sAssId The string assign id. Cannot be empty.
	 * @return The assign id or -1 if not defined.
	 */
	int32_t getAssignId(const std::string& sAssId) const noexcept;
	/** Whether a string assign id was added.
	 * @param sAssId The string assign id. Cannot be empty.
	 * @return Whether defined.
	 */
	bool hasAssignId(const std::string& sAssId) const noexcept;
	/** Whether an assign id was added.
	 * @param nAssId The assign id.
	 * @return Whether defined.
	 */
	bool hasAssignId(int32_t nAssId) const noexcept;
	/** Get string assign id from assign id.
	 * @param nAssId The assign id.
	 * @return The string id.
	 */
	const std::string& getAssignId(int32_t nAssId) const noexcept;
	/** Add an image array to tile traits and players assignment.
	 * If an assign with the given id already exists nothing is added.
	 *
	 * The total number of traits must be positive. The array of images must be
	 * big enough (from nArrayFromIdx) to provide an image for each trait set combination.
	 *
	 * Example: the images of a snake's head, body and tail for two players are assigned
	 * to tiles with TileChar "H","B","T" and players 0 and 1.
	 * The array of images would be H0, B0, T0, H1, B1, T1. In this case the player
	 * is more significant since all the images of the first player (0) come first.
	 * @param sAssId The string assign id. Cannot be empty.
	 * @param refPlayerTraitSet The player set. Can be null if a non empty trait set is defined.
	 * @param bPlayerFirst Whether players are more significant than tile traits or vice versa.
	 * @param aTileTraitSets The tile trait sets. Can be empty if a non empty player trait set is defined.
	 * @param sArrayId The array of images. Must exist.
	 * @param nArrayFromIdx From which image to start.
	 * @return The created or already existing assign id.
	 */
	int32_t addAssign(const std::string& sAssId
					, std::unique_ptr<IntSet>&& refPlayerTraitSet, bool bPlayerFirst
					, std::vector< std::unique_ptr<TraitSet> >&& aTileTraitSets
					, const std::string& sArrayId, int32_t nArrayFromIdx) noexcept;

	/** Get an assign image given a tile and player.
	 * @param sAssId The string assign id. Cannot be empty.
	 * @param oTile The tile that chooses the image.
	 * @param nPlayer The player. Can be -1 if not defined.
	 * @return The image or null if outside the ranges of the assign.
	 */
	shared_ptr<Image> getAssignImage(int32_t sAssId, const Tile& oTile, int32_t nPlayer) const noexcept;

	/** Add modifiers to a painter.
	 * If a painter with given name doesn't already exist, one is created with an
	 * associated (empty) array of modifiers. Modifiers are sort of instructions used
	 * to draw a single tile taking into account tile animations.
	 *
	 * The passed aModifiers are appended to the array of modifiers of that painter,
	 * followed by a StopModifier. These are considered as a sub painter.
	 *
	 * A painter starts painting from index 0 of the array of modifiers and stops
	 * at the first occurrence of a StopModifier. If a NextSubPainterModifier is encountered
	 * it jumps to the first modifier of the next sub painter or stops if there isn't one.
	 * @param sPainterName The painter`s name. Cannot be empty.
	 * @param aModifiers The modifiers. The vector's values cannot be null.
	 * @return The painter index into Named::painters().
	 */
	int32_t addPainter(const std::string& sPainterName, std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept;
	/** Sets the default painter name.
	 * If a default was already set this function does nothing.
	 *
	 * The painter must have been previously added with addPainter().
	 * @param nPainterIdx The default painter index into Named::painters(). Must be valid.
	 */
	void setDefaultPainter(int32_t nPainterIdx) noexcept;
	/** The default painter.
	 * @return The default painter index into Named::painters() or -1 if no painters defined.
	 */
	int32_t getDefaultPainterIdx() noexcept;

	int32_t getVariableIndex(const std::string& sVariableName) noexcept;
private:
	friend class StdThemeDrawingContext;
	int32_t getTotVariableIndexes() const noexcept;
	const std::string& getVariableNameFromIndex(int32_t nVariableIdx) const noexcept;
	friend class StdThemeContext;
	void registerTileSize(int32_t nW, int32_t nH) noexcept;
	void unregisterTileSize(int32_t nW, int32_t nH) noexcept;
	void drawTile(int32_t nPainterId, const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeContext& oTc
				, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept;
	void drawTileFromPP(size_t nPP, const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oTc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed
						, std::vector< unique_ptr<StdThemeModifier> >& aModifiers) noexcept;

	void registerTileSize(int32_t nW, int32_t nH, bool bUn) noexcept;

	friend class NextSubPainterModifier;
	void drawTileFromPP(size_t nPP, const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oTc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept;

	shared_ptr<ThemeAnimation> createAnimation(const shared_ptr<StdThemeContext>& refCtx, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept;
	shared_ptr<ThemeSound> createSound(StdThemeContext* p0Ctx, int32_t nSoundIdx, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks
										, FPoint oXYPosition, double fZPosition, bool bRelative
										, double fVolume, bool bLoop) noexcept;
	void preloadSound(int32_t nSoundIdx, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks) noexcept;

	friend class StdThemeSound;
	void removeCapability(int32_t nCapabilityId) noexcept;
	// return -1 if not cached yet, enlarges m_aCachedFileIds if necessary
	int32_t getCachedFileIdFromCapaAndSoundIdx(int32_t nSoundIdx, int32_t nCapaId) noexcept;

#ifndef NDEBUG
	void dumpNames(bool bDumpCharNames, bool bDumpColorNames, bool bDumpFontNames) const noexcept;
	void dumpTile(const Tile& oTile) const noexcept;
#endif

private:
	TileSizing m_oBoardTileSizing;

	class PrivateStdThemeContext : public StdThemeContext
	{
	public:
		using StdThemeContext::StdThemeContext;
		using StdThemeContext::reInit;
	};
	Recycler<PrivateStdThemeContext> m_oContexts;

	Recycler<StdThemeSound> m_oThemeSounds;
	struct CapaToFileId
	{
		int32_t m_nCapabilityId;
		int32_t m_nFileId;
	};
	std::vector< std::vector<CapaToFileId> > m_aCachedFileIds; // Size: m_oNamed.sounds().size() or less. Value: file id for each capability.

	Named m_oNamed;

	std::vector<double> m_aNoAniElapsed; // Size: m_oNamed.tileAnis().size(),  Value: -1.0

	std::vector<TileColor> m_aNamedColorIdx;
	TileColor m_oDefaultPalColor; // set to black
	std::vector<TileColor> m_aPal256;
	std::map<int32_t, TileColor> m_oPalRest; //TODO unordered_map

	std::vector<std::string> m_aNamedFontIdx;
	std::string m_sDefaultFont;

	// The known image file names
	NamedIndex m_oImageFileNames;
	// The full path file (or buffered file) associated with a file name
	std::vector< File > m_aKnownImageFiles; // Size: m_oImageFileNames.size()
	// The image associated with a file
	std::vector< shared_ptr<Image> > m_aImageByFileIdxs; // Size: m_oImageFileNames.size(),  Value: null if not loaded yet

	// maps image id to index into m_aImageByFileIdxs
	std::vector<int32_t> m_aImageIdFileIdx; // Size: m_oNamed.images().size(), Value: m_oImageFileNames index

	// The known image file names
	NamedIndex m_oSoundFileNames;
	// The full path file (or buffered file) associated with a file name
	std::vector< File > m_aKnownSoundFiles; // Size: m_oSoundFileNames.size()

	// maps sound id to index into m_aSoundByFileIdxs
	std::vector<int32_t> m_aSoundIdFileIdx; // Size: m_oNamed.sounds().size(), Value: m_oSoundFileNames index

	// All the animation factories.
	// Size: m_oNamed.animations().size(), Index: m_oNamed.animations().getIndex(sName)
	std::vector< unique_ptr<StdThemeAnimationFactory> > m_aNamedAnimationFactories;
	// All the factories that should be checked when model animation doesn't
	// define a factory name.
	std::vector< StdThemeAnimationFactory* > m_aAnonymousModelAnimationFactories;
	// "Fast" search by typeid of model. Filled lazily as animations are created.
	// Key: typeid(LavelAnimation).hash_code(), Value: vector of factories supporting that class
	std::unordered_map< size_t, std::vector< StdThemeAnimationFactory* > > m_oAnimationTypeIdFactories;

	// All the widget factories.
	// Size: m_oNamed.widgets().size(), Index: m_oNamed.widgets().getIndex(sName)
	std::vector< unique_ptr<StdThemeWidgetFactory> > m_aNamedWidgetFactories;
	// All the factories that should be checked when model widget doesn't
	// define a factory name.
	std::vector< StdThemeWidgetFactory* > m_aAnonymousModelWidgetFactories;

	std::map<std::string, shared_ptr<TileAni> > m_oTileAniIds; // Key: sTileAniId

	struct SubArrayData
	{
		int32_t m_nImgW; // The width in pixels of the sub images
		int32_t m_nImgH; // The height in pixels of the sub images
		int32_t m_nSpacingX; // The horizontal spacing between sub images in pixels
		int32_t m_nSpacingY; // The vertical between spacing between sub images in pixels
		int32_t m_nPerRow; // The number of sub images per row
		int32_t m_nArraySize; // The total number of sub images
		shared_ptr<Image> m_refImage; // The image from which sub images are extracted
		std::vector< shared_ptr<Image> > m_aSubImages; // Size: <= m_nArraySize
	};
	std::map<std::string, SubArrayData> m_oSubArrayData;

	struct FileArrayData
	{
		std::vector< shared_ptr<Image> > m_aImageArray;
	};
	std::map<std::string, FileArrayData> m_oFileArrayData;

	struct AssignData
	{
		unique_ptr<IntSet> m_refPlayerTrait; // if null any player
		bool m_bPlayerFirst; // if true the player-set is the most significant, if false the least
		std::vector< unique_ptr<TraitSet> > m_aTileTraits;
		// cache
		std::vector< std::pair<bool, int32_t> > m_aTileTraitsCaches; // Value: <bHasEmptyValue, nTotValues>,  Size: m_aTraitSets.size())
		int32_t m_nTotValues = 0; // Multiplication of the size of all trait and player sets.
		std::vector< shared_ptr<Image> > m_aImages; // The images
	};
	NamedIndex m_oAssignIds;
	std::vector< AssignData > m_aAssign; // Size: m_oAssignId.size()

	struct TilePainter
	{
		std::vector< unique_ptr<StdThemeModifier> > m_aModifiers;
		bool m_bFinished = false; // set to true when an added sub painter has no NextSubPainterModifier in it
//		std::vector< size_t > m_aSubStartPP; // Size: number of sub-painters, Value: Paint Pointer into TilePainter::m_aModifiers
	};
	std::vector< TilePainter > m_aTilePainters; // Size: m_oNamed.painters().size()
	int32_t m_nDefaultPainterIdx; // -1 ot index into m_oNamed.painters()

	NamedIndex m_oVariableNames;

	static const std::string s_sSansFontDesc;

private:
	shared_ptr<Image> getSubImage(SubArrayData& oSubArrayData, int32_t nArrayIdx) noexcept;
private:
	StdTheme(const StdTheme& oSource) = delete;
	StdTheme& operator=(const StdTheme& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_STD_THEME_H */

