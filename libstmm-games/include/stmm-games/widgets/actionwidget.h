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
 * File:   actionwidget.h
 */

#ifndef STMG_ACTION_WIDGET_H
#define STMG_ACTION_WIDGET_H

#include "relsizedgamewidget.h"

#include <memory>
#include <string>

#include <stdint.h>

namespace stmg { class Event; }
namespace stmi { class Event; }

namespace stmg
{

/** Action widget.
 * When its view is pressed by mouse or touched either sends a KeyActionEvent to the game,
 * sends a message to an event or interrupts the game.
 */
class ActionWidget : public RelSizedGameWidget
{
public:
	/** What happens when the widget is pressed.
	 */
	enum ACTION_TYPE
	{
		ACTION_KEY_ACTION = 1 /**< Execute a key action. */
		, ACTION_INTERRUPT = 2 /**< Execute an interrupt. Ex: PAUSE. */
		, ACTION_EVENT_MSG = 3 /**< Trigger an event with a message. */
	};
	struct LocalInit
	{
		ACTION_TYPE m_eActionType = ACTION_KEY_ACTION;
		int32_t m_nActionId = -1; /**< Depending on m_eActionType, the valid key action id, GameProxy::INTERRUPT_TYPE value or event's message (must be &gt;= -1). */
		int32_t m_nValue = 0; /**< Only used if m_eActionType is ACTION_EVENT_MSG. The event's value. Default is 0. */
		std::string m_sActionName; /**< If m_eActionType is ACTION_EVENT_MSG the non empty event id. See Level::addEvent(const std::string& sId, unique_ptr<Event>). */
		int32_t m_nImgId = -1; /**< The id of the shown image (icon) or -1 if not defined. */
		std::string m_sText; /**< The shown text. Can be empty. */
		double m_fTextSize = - 1.0; /**< The font height in tile heights. Non positive value means as big as it fits. Default is -1.0. */
	};
	struct Init : public RelSizedGameWidget::Init, public LocalInit
	{
	};

	/** The shown image.
	 * @return The image id of the widget or -1 if not defined.
	 */
	inline int32_t getImageId() const noexcept { return m_oData.m_nImgId; }
	/** The shown text.
	 * @return The text of the widget. Can be empty.
	 */
	inline const std::string& getText() const noexcept { return m_oData.m_sText; }
	/** The text height in tiles.
	 * @return The text size. Is positive.
	 */
	inline double getTextSize() const noexcept { return m_oData.m_fTextSize; }
	/** Execute action out of a stmi::XYEvent for a player.
	 * @param refXYEvent The event generated in the view. Cannot be null.
	 * @param nTeam The team of the player. Must be `&gt;=-1`.
	 * @param nMate The mate number of the player. Must be `&gt;=-1`. If team is -1 must be -1 too.
	 */
	void handleXYInput(const shared_ptr<stmi::Event>& refXYEvent, int32_t nTeam, int32_t nMate) noexcept;

	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit ActionWidget(Init&& oInit) noexcept;

	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Reinitialization.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept;
private:
	void checkData() noexcept;
private:
	LocalInit m_oData;
	Event* m_p0Event;
	bool m_bEventChecked;
};


} // namespace stmg

#endif	/* STMG_ACTION_WIDGET_H */

