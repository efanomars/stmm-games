/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   inputstrings.cc
 */

#include "inputstrings.h"

namespace stmg
{

namespace omni
{

void InputStrings::initKeyMap() noexcept
{
	// Generate:
	// from <stmm-input/hardwarekey.h> file copy all keys
	// and transform each line:
	//	, HK_XXX = n
	// to:
	//	addKey(stmi::HK_XXX, "XXX");
	// with:
	//   Find:     <^	, HK_([^ ]+).*$>
	//   Replace:  <	addKey(stmi::HK_\1, "\1");>

	addKey(stmi::HK_NULL, "NULL");
	addKey(stmi::HK_ESC, "ESC");
	addKey(stmi::HK_1, "1");
	addKey(stmi::HK_2, "2");
	addKey(stmi::HK_3, "3");
	addKey(stmi::HK_4, "4");
	addKey(stmi::HK_5, "5");
	addKey(stmi::HK_6, "6");
	addKey(stmi::HK_7, "7");
	addKey(stmi::HK_8, "8");
	addKey(stmi::HK_9, "9");
	addKey(stmi::HK_0, "0");
	addKey(stmi::HK_MINUS, "MINUS");
	addKey(stmi::HK_EQUAL, "EQUAL");
	addKey(stmi::HK_BACKSPACE, "BACKSPACE");
	addKey(stmi::HK_TAB, "TAB");
	addKey(stmi::HK_Q, "Q");
	addKey(stmi::HK_W, "W");
	addKey(stmi::HK_E, "E");
	addKey(stmi::HK_R, "R");
	addKey(stmi::HK_T, "T");
	addKey(stmi::HK_Y, "Y");
	addKey(stmi::HK_U, "U");
	addKey(stmi::HK_I, "I");
	addKey(stmi::HK_O, "O");
	addKey(stmi::HK_P, "P");
	addKey(stmi::HK_LEFTBRACE, "LEFTBRACE");
	addKey(stmi::HK_RIGHTBRACE, "RIGHTBRACE");
	addKey(stmi::HK_ENTER, "ENTER");
	addKey(stmi::HK_LEFTCTRL, "LEFTCTRL");
	addKey(stmi::HK_A, "A");
	addKey(stmi::HK_S, "S");
	addKey(stmi::HK_D, "D");
	addKey(stmi::HK_F, "F");
	addKey(stmi::HK_G, "G");
	addKey(stmi::HK_H, "H");
	addKey(stmi::HK_J, "J");
	addKey(stmi::HK_K, "K");
	addKey(stmi::HK_L, "L");
	addKey(stmi::HK_SEMICOLON, "SEMICOLON");
	addKey(stmi::HK_APOSTROPHE, "APOSTROPHE");
	addKey(stmi::HK_GRAVE, "GRAVE");
	addKey(stmi::HK_LEFTSHIFT, "LEFTSHIFT");
	addKey(stmi::HK_BACKSLASH, "BACKSLASH");
	addKey(stmi::HK_Z, "Z");
	addKey(stmi::HK_X, "X");
	addKey(stmi::HK_C, "C");
	addKey(stmi::HK_V, "V");
	addKey(stmi::HK_B, "B");
	addKey(stmi::HK_N, "N");
	addKey(stmi::HK_M, "M");
	addKey(stmi::HK_COMMA, "COMMA");
	addKey(stmi::HK_DOT, "DOT");
	addKey(stmi::HK_SLASH, "SLASH");
	addKey(stmi::HK_RIGHTSHIFT, "RIGHTSHIFT");
	addKey(stmi::HK_KPASTERISK, "KPASTERISK");
	addKey(stmi::HK_LEFTALT, "LEFTALT");
	addKey(stmi::HK_SPACE, "SPACE");
	addKey(stmi::HK_CAPSLOCK, "CAPSLOCK");
	addKey(stmi::HK_F1, "F1");
	addKey(stmi::HK_F2, "F2");
	addKey(stmi::HK_F3, "F3");
	addKey(stmi::HK_F4, "F4");
	addKey(stmi::HK_F5, "F5");
	addKey(stmi::HK_F6, "F6");
	addKey(stmi::HK_F7, "F7");
	addKey(stmi::HK_F8, "F8");
	addKey(stmi::HK_F9, "F9");
	addKey(stmi::HK_F10, "F10");
	addKey(stmi::HK_NUMLOCK, "NUMLOCK");
	addKey(stmi::HK_SCROLLLOCK, "SCROLLLOCK");
	addKey(stmi::HK_KP7, "KP7");
	addKey(stmi::HK_KP8, "KP8");
	addKey(stmi::HK_KP9, "KP9");
	addKey(stmi::HK_KPMINUS, "KPMINUS");
	addKey(stmi::HK_KP4, "KP4");
	addKey(stmi::HK_KP5, "KP5");
	addKey(stmi::HK_KP6, "KP6");
	addKey(stmi::HK_KPPLUS, "KPPLUS");
	addKey(stmi::HK_KP1, "KP1");
	addKey(stmi::HK_KP2, "KP2");
	addKey(stmi::HK_KP3, "KP3");
	addKey(stmi::HK_KP0, "KP0");
	addKey(stmi::HK_KPDOT, "KPDOT");

	addKey(stmi::HK_ZENKAKUHANKAKU, "ZENKAKUHANKAKU");
	addKey(stmi::HK_102ND, "102ND");
	addKey(stmi::HK_F11, "F11");
	addKey(stmi::HK_F12, "F12");
	addKey(stmi::HK_RO, "RO");
	addKey(stmi::HK_KATAKANA, "KATAKANA");
	addKey(stmi::HK_HIRAGANA, "HIRAGANA");
	addKey(stmi::HK_HENKAN, "HENKAN");
	addKey(stmi::HK_KATAKANAHIRAGANA, "KATAKANAHIRAGANA");
	addKey(stmi::HK_MUHENKAN, "MUHENKAN");
	addKey(stmi::HK_KPJPCOMMA, "KPJPCOMMA");
	addKey(stmi::HK_KPENTER, "KPENTER");
	addKey(stmi::HK_RIGHTCTRL, "RIGHTCTRL");
	addKey(stmi::HK_KPSLASH, "KPSLASH");
	addKey(stmi::HK_SYSRQ, "SYSRQ");
	addKey(stmi::HK_RIGHTALT, "RIGHTALT");
	addKey(stmi::HK_LINEFEED, "LINEFEED");
	addKey(stmi::HK_HOME, "HOME");
	addKey(stmi::HK_UP, "UP");
	addKey(stmi::HK_PAGEUP, "PAGEUP");
	addKey(stmi::HK_LEFT, "LEFT");
	addKey(stmi::HK_RIGHT, "RIGHT");
	addKey(stmi::HK_END, "END");
	addKey(stmi::HK_DOWN, "DOWN");
	addKey(stmi::HK_PAGEDOWN, "PAGEDOWN");
	addKey(stmi::HK_INSERT, "INSERT");
	addKey(stmi::HK_DELETE, "DELETE");
	addKey(stmi::HK_MACRO, "MACRO");
	addKey(stmi::HK_MUTE, "MUTE");
	addKey(stmi::HK_VOLUMEDOWN, "VOLUMEDOWN");
	addKey(stmi::HK_VOLUMEUP, "VOLUMEUP");
	addKey(stmi::HK_POWER, "POWER");
	addKey(stmi::HK_KPEQUAL, "KPEQUAL");
	addKey(stmi::HK_KPPLUSMINUS, "KPPLUSMINUS");
	addKey(stmi::HK_PAUSE, "PAUSE");
	addKey(stmi::HK_SCALE, "SCALE");

	addKey(stmi::HK_KPCOMMA, "KPCOMMA");
	addKey(stmi::HK_HANGEUL, "HANGEUL");
		//, HK_HANGUEL = HK_HANGEUL
	addKey(stmi::HK_HANJA, "HANJA");
	addKey(stmi::HK_YEN, "YEN");
	addKey(stmi::HK_LEFTMETA, "LEFTMETA");
	addKey(stmi::HK_RIGHTMETA, "RIGHTMETA");
	addKey(stmi::HK_COMPOSE, "COMPOSE");

	addKey(stmi::HK_STOP, "STOP");
	addKey(stmi::HK_AGAIN, "AGAIN");
	addKey(stmi::HK_PROPS, "PROPS");
	addKey(stmi::HK_UNDO, "UNDO");
	addKey(stmi::HK_FRONT, "FRONT");
	addKey(stmi::HK_COPY, "COPY");
	addKey(stmi::HK_OPEN, "OPEN");
	addKey(stmi::HK_PASTE, "PASTE");
	addKey(stmi::HK_FIND, "FIND");
	addKey(stmi::HK_CUT, "CUT");
	addKey(stmi::HK_HELP, "HELP");
	addKey(stmi::HK_MENU, "MENU");
	addKey(stmi::HK_CALC, "CALC");
	addKey(stmi::HK_SETUP, "SETUP");
	addKey(stmi::HK_SLEEP, "SLEEP");
	addKey(stmi::HK_WAKEUP, "WAKEUP");
	addKey(stmi::HK_FILE, "FILE");
	addKey(stmi::HK_SENDFILE, "SENDFILE");
	addKey(stmi::HK_DELETEFILE, "DELETEFILE");
	addKey(stmi::HK_XFER, "XFER");
	addKey(stmi::HK_PROG1, "PROG1");
	addKey(stmi::HK_PROG2, "PROG2");
	addKey(stmi::HK_WWW, "WWW");
	addKey(stmi::HK_MSDOS, "MSDOS");
	addKey(stmi::HK_SCREENLOCK, "SCREENLOCK");
		//, HK_COFFEE = HK_SCREENLOCK
	addKey(stmi::HK_DIRECTION, "DIRECTION");
	addKey(stmi::HK_CYCLEWINDOWS, "CYCLEWINDOWS");
	addKey(stmi::HK_MAIL, "MAIL");
	addKey(stmi::HK_BOOKMARKS, "BOOKMARKS");
	addKey(stmi::HK_COMPUTER, "COMPUTER");
	addKey(stmi::HK_BACK, "BACK");
	addKey(stmi::HK_FORWARD, "FORWARD");
	addKey(stmi::HK_CLOSECD, "CLOSECD");
	addKey(stmi::HK_EJECTCD, "EJECTCD");
	addKey(stmi::HK_EJECTCLOSECD, "EJECTCLOSECD");
	addKey(stmi::HK_NEXTSONG, "NEXTSONG");
	addKey(stmi::HK_PLAYPAUSE, "PLAYPAUSE");
	addKey(stmi::HK_PREVIOUSSONG, "PREVIOUSSONG");
	addKey(stmi::HK_STOPCD, "STOPCD");
	addKey(stmi::HK_RECORD, "RECORD");
	addKey(stmi::HK_REWIND, "REWIND");
	addKey(stmi::HK_PHONE, "PHONE");
	addKey(stmi::HK_ISO, "ISO");
	addKey(stmi::HK_CONFIG, "CONFIG");
	addKey(stmi::HK_HOMEPAGE, "HOMEPAGE");
	addKey(stmi::HK_REFRESH, "REFRESH");
	addKey(stmi::HK_EXIT, "EXIT");
	addKey(stmi::HK_MOVE, "MOVE");
	addKey(stmi::HK_EDIT, "EDIT");
	addKey(stmi::HK_SCROLLUP, "SCROLLUP");
	addKey(stmi::HK_SCROLLDOWN, "SCROLLDOWN");
	addKey(stmi::HK_KPLEFTPAREN, "KPLEFTPAREN");
	addKey(stmi::HK_KPRIGHTPAREN, "KPRIGHTPAREN");
	addKey(stmi::HK_NEW, "NEW");
	addKey(stmi::HK_REDO, "REDO");

	addKey(stmi::HK_F13, "F13");
	addKey(stmi::HK_F14, "F14");
	addKey(stmi::HK_F15, "F15");
	addKey(stmi::HK_F16, "F16");
	addKey(stmi::HK_F17, "F17");
	addKey(stmi::HK_F18, "F18");
	addKey(stmi::HK_F19, "F19");
	addKey(stmi::HK_F20, "F20");
	addKey(stmi::HK_F21, "F21");
	addKey(stmi::HK_F22, "F22");
	addKey(stmi::HK_F23, "F23");
	addKey(stmi::HK_F24, "F24");

	addKey(stmi::HK_PLAYCD, "PLAYCD");
	addKey(stmi::HK_PAUSECD, "PAUSECD");
	addKey(stmi::HK_PROG3, "PROG3");
	addKey(stmi::HK_PROG4, "PROG4");
	addKey(stmi::HK_DASHBOARD, "DASHBOARD");
	addKey(stmi::HK_SUSPEND, "SUSPEND");
	addKey(stmi::HK_CLOSE, "CLOSE");
	addKey(stmi::HK_PLAY, "PLAY");
	addKey(stmi::HK_FASTFORWARD, "FASTFORWARD");
	addKey(stmi::HK_BASSBOOST, "BASSBOOST");
	addKey(stmi::HK_PRINT, "PRINT");
	addKey(stmi::HK_HP, "HP");
	addKey(stmi::HK_CAMERA, "CAMERA");
	addKey(stmi::HK_SOUND, "SOUND");
	addKey(stmi::HK_QUESTION, "QUESTION");
	addKey(stmi::HK_EMAIL, "EMAIL");
	addKey(stmi::HK_CHAT, "CHAT");
	addKey(stmi::HK_SEARCH, "SEARCH");
	addKey(stmi::HK_CONNECT, "CONNECT");
	addKey(stmi::HK_FINANCE, "FINANCE");
	addKey(stmi::HK_SPORT, "SPORT");
	addKey(stmi::HK_SHOP, "SHOP");
	addKey(stmi::HK_ALTERASE, "ALTERASE");
	addKey(stmi::HK_CANCEL, "CANCEL");
	addKey(stmi::HK_BRIGHTNESSDOWN, "BRIGHTNESSDOWN");
	addKey(stmi::HK_BRIGHTNESSUP, "BRIGHTNESSUP");
	addKey(stmi::HK_MEDIA, "MEDIA");

	addKey(stmi::HK_SWITCHVIDEOMODE, "SWITCHVIDEOMODE");
	addKey(stmi::HK_KBDILLUMTOGGLE, "KBDILLUMTOGGLE");
	addKey(stmi::HK_KBDILLUMDOWN, "KBDILLUMDOWN");
	addKey(stmi::HK_KBDILLUMUP, "KBDILLUMUP");

	addKey(stmi::HK_SEND, "SEND");
	addKey(stmi::HK_REPLY, "REPLY");
	addKey(stmi::HK_FORWARDMAIL, "FORWARDMAIL");
	addKey(stmi::HK_SAVE, "SAVE");
	addKey(stmi::HK_DOCUMENTS, "DOCUMENTS");

	addKey(stmi::HK_BATTERY, "BATTERY");

	addKey(stmi::HK_BLUETOOTH, "BLUETOOTH");
	addKey(stmi::HK_WLAN, "WLAN");
	addKey(stmi::HK_UWB, "UWB");

	addKey(stmi::HK_UNKNOWN, "UNKNOWN");

	addKey(stmi::HK_VIDEO_NEXT, "VIDEO_NEXT");
	addKey(stmi::HK_VIDEO_PREV, "VIDEO_PREV");
	addKey(stmi::HK_BRIGHTNESS_CYCLE, "BRIGHTNESS_CYCLE");
	addKey(stmi::HK_BRIGHTNESS_ZERO, "BRIGHTNESS_ZERO");
		//, HK_BRIGHTNESS_AUTO = HK_BRIGHTNESS_ZERO
	addKey(stmi::HK_DISPLAY_OFF, "DISPLAY_OFF");

	addKey(stmi::HK_WIMAX, "WIMAX");
		//, HK_WWAN = HK_WIMAX
	addKey(stmi::HK_RFKILL, "RFKILL");

	addKey(stmi::HK_MICMUTE, "MICMUTE");

		//addKey(stmi::HK_BTN_MISC, "BTN_MISC");
	addKey(stmi::HK_BTN_0, "BTN_0");
	addKey(stmi::HK_BTN_1, "BTN_1");
	addKey(stmi::HK_BTN_2, "BTN_2");
	addKey(stmi::HK_BTN_3, "BTN_3");
	addKey(stmi::HK_BTN_4, "BTN_4");
	addKey(stmi::HK_BTN_5, "BTN_5");
	addKey(stmi::HK_BTN_6, "BTN_6");
	addKey(stmi::HK_BTN_7, "BTN_7");
	addKey(stmi::HK_BTN_8, "BTN_8");
	addKey(stmi::HK_BTN_9, "BTN_9");

		//, HK_BTN_MOUSE = 0x110
	addKey(stmi::HK_BTN_LEFT, "BTN_LEFT");
	addKey(stmi::HK_BTN_RIGHT, "BTN_RIGHT");
	addKey(stmi::HK_BTN_MIDDLE, "BTN_MIDDLE");
	addKey(stmi::HK_BTN_SIDE, "BTN_SIDE");
	addKey(stmi::HK_BTN_EXTRA, "BTN_EXTRA");
	addKey(stmi::HK_BTN_FORWARD, "BTN_FORWARD");
	addKey(stmi::HK_BTN_BACK, "BTN_BACK");
	addKey(stmi::HK_BTN_TASK, "BTN_TASK");

		//, HK_BTN_JOYSTICK = 0x120
	addKey(stmi::HK_BTN_TRIGGER, "BTN_TRIGGER");
	addKey(stmi::HK_BTN_THUMB, "BTN_THUMB");
	addKey(stmi::HK_BTN_THUMB2, "BTN_THUMB2");
	addKey(stmi::HK_BTN_TOP, "BTN_TOP");
	addKey(stmi::HK_BTN_TOP2, "BTN_TOP2");
	addKey(stmi::HK_BTN_PINKIE, "BTN_PINKIE");
	addKey(stmi::HK_BTN_BASE, "BTN_BASE");
	addKey(stmi::HK_BTN_BASE2, "BTN_BASE2");
	addKey(stmi::HK_BTN_BASE3, "BTN_BASE3");
	addKey(stmi::HK_BTN_BASE4, "BTN_BASE4");
	addKey(stmi::HK_BTN_BASE5, "BTN_BASE5");
	addKey(stmi::HK_BTN_BASE6, "BTN_BASE6");
	addKey(stmi::HK_BTN_DEAD, "BTN_DEAD");

		//addKey(stmi::HK_BTN_GAMEPAD, "BTN_GAMEPAD");
	addKey(stmi::HK_BTN_A, "BTN_A");
		//, HK_BTN_SOUTH = HK_BTN_A
	addKey(stmi::HK_BTN_B, "BTN_B");
		//, HK_BTN_EAST = HK_BTN_B
	addKey(stmi::HK_BTN_C, "BTN_C");
	addKey(stmi::HK_BTN_X, "BTN_X");
		//, HK_BTN_NORTH = HK_BTN_X
	addKey(stmi::HK_BTN_Y, "BTN_Y");
		//, HK_BTN_WEST = HK_BTN_Y
	addKey(stmi::HK_BTN_Z, "BTN_Z");
	addKey(stmi::HK_BTN_TL, "BTN_TL");
	addKey(stmi::HK_BTN_TR, "BTN_TR");
	addKey(stmi::HK_BTN_TL2, "BTN_TL2");
	addKey(stmi::HK_BTN_TR2, "BTN_TR2");
	addKey(stmi::HK_BTN_SELECT, "BTN_SELECT");
	addKey(stmi::HK_BTN_START, "BTN_START");
	addKey(stmi::HK_BTN_MODE, "BTN_MODE");
	addKey(stmi::HK_BTN_THUMBL, "BTN_THUMBL");
	addKey(stmi::HK_BTN_THUMBR, "BTN_THUMBR");

		//addKey(stmi::HK_BTN_DIGI, "BTN_DIGI");
	addKey(stmi::HK_BTN_TOOL_PEN, "BTN_TOOL_PEN");
	addKey(stmi::HK_BTN_TOOL_RUBBER, "BTN_TOOL_RUBBER");
	addKey(stmi::HK_BTN_TOOL_BRUSH, "BTN_TOOL_BRUSH");
	addKey(stmi::HK_BTN_TOOL_PENCIL, "BTN_TOOL_PENCIL");
	addKey(stmi::HK_BTN_TOOL_AIRBRUSH, "BTN_TOOL_AIRBRUSH");
	addKey(stmi::HK_BTN_TOOL_FINGER, "BTN_TOOL_FINGER");
	addKey(stmi::HK_BTN_TOOL_MOUSE, "BTN_TOOL_MOUSE");
	addKey(stmi::HK_BTN_TOOL_LENS, "BTN_TOOL_LENS");
	addKey(stmi::HK_BTN_TOOL_QUINTTAP, "BTN_TOOL_QUINTTAP");
	addKey(stmi::HK_BTN_TOUCH, "BTN_TOUCH");
	addKey(stmi::HK_BTN_STYLUS, "BTN_STYLUS");
	addKey(stmi::HK_BTN_STYLUS2, "BTN_STYLUS2");
	addKey(stmi::HK_BTN_TOOL_DOUBLETAP, "BTN_TOOL_DOUBLETAP");
	addKey(stmi::HK_BTN_TOOL_TRIPLETAP, "BTN_TOOL_TRIPLETAP");
	addKey(stmi::HK_BTN_TOOL_QUADTAP, "BTN_TOOL_QUADTAP");

		//addKey(stmi::HK_BTN_WHEEL, "BTN_WHEEL");
	addKey(stmi::HK_BTN_GEAR_DOWN, "BTN_GEAR_DOWN");
	addKey(stmi::HK_BTN_GEAR_UP, "BTN_GEAR_UP");

	addKey(stmi::HK_OK, "OK");
	addKey(stmi::HK_SELECT, "SELECT");
	addKey(stmi::HK_GOTO, "GOTO");
	addKey(stmi::HK_CLEAR, "CLEAR");
	addKey(stmi::HK_POWER2, "POWER2");
	addKey(stmi::HK_OPTION, "OPTION");
	addKey(stmi::HK_INFO, "INFO");
	addKey(stmi::HK_TIME, "TIME");
	addKey(stmi::HK_VENDOR, "VENDOR");
	addKey(stmi::HK_ARCHIVE, "ARCHIVE");
	addKey(stmi::HK_PROGRAM, "PROGRAM");
	addKey(stmi::HK_CHANNEL, "CHANNEL");
	addKey(stmi::HK_FAVORITES, "FAVORITES");
	addKey(stmi::HK_EPG, "EPG");
	addKey(stmi::HK_PVR, "PVR");
	addKey(stmi::HK_MHP, "MHP");
	addKey(stmi::HK_LANGUAGE, "LANGUAGE");
	addKey(stmi::HK_TITLE, "TITLE");
	addKey(stmi::HK_SUBTITLE, "SUBTITLE");
	addKey(stmi::HK_ANGLE, "ANGLE");
	addKey(stmi::HK_ZOOM, "ZOOM");
	addKey(stmi::HK_MODE, "MODE");
	addKey(stmi::HK_KEYBOARD, "KEYBOARD");
	addKey(stmi::HK_SCREEN, "SCREEN");
	addKey(stmi::HK_PC, "PC");
	addKey(stmi::HK_TV, "TV");
	addKey(stmi::HK_TV2, "TV2");
	addKey(stmi::HK_VCR, "VCR");
	addKey(stmi::HK_VCR2, "VCR2");
	addKey(stmi::HK_SAT, "SAT");
	addKey(stmi::HK_SAT2, "SAT2");
	addKey(stmi::HK_CD, "CD");
	addKey(stmi::HK_TAPE, "TAPE");
	addKey(stmi::HK_RADIO, "RADIO");
	addKey(stmi::HK_TUNER, "TUNER");
	addKey(stmi::HK_PLAYER, "PLAYER");
	addKey(stmi::HK_TEXT, "TEXT");
	addKey(stmi::HK_DVD, "DVD");
	addKey(stmi::HK_AUX, "AUX");
	addKey(stmi::HK_MP3, "MP3");
	addKey(stmi::HK_AUDIO, "AUDIO");
	addKey(stmi::HK_VIDEO, "VIDEO");
	addKey(stmi::HK_DIRECTORY, "DIRECTORY");
	addKey(stmi::HK_LIST, "LIST");
	addKey(stmi::HK_MEMO, "MEMO");
	addKey(stmi::HK_CALENDAR, "CALENDAR");
	addKey(stmi::HK_RED, "RED");
	addKey(stmi::HK_GREEN, "GREEN");
	addKey(stmi::HK_YELLOW, "YELLOW");
	addKey(stmi::HK_BLUE, "BLUE");
	addKey(stmi::HK_CHANNELUP, "CHANNELUP");
	addKey(stmi::HK_CHANNELDOWN, "CHANNELDOWN");
	addKey(stmi::HK_FIRST, "FIRST");
	addKey(stmi::HK_LAST, "LAST");
	addKey(stmi::HK_AB, "AB");
	addKey(stmi::HK_NEXT, "NEXT");
	addKey(stmi::HK_RESTART, "RESTART");
	addKey(stmi::HK_SLOW, "SLOW");
	addKey(stmi::HK_SHUFFLE, "SHUFFLE");
	addKey(stmi::HK_BREAK, "BREAK");
	addKey(stmi::HK_PREVIOUS, "PREVIOUS");
	addKey(stmi::HK_DIGITS, "DIGITS");
	addKey(stmi::HK_TEEN, "TEEN");
	addKey(stmi::HK_TWEN, "TWEN");
	addKey(stmi::HK_VIDEOPHONE, "VIDEOPHONE");
	addKey(stmi::HK_GAMES, "GAMES");
	addKey(stmi::HK_ZOOMIN, "ZOOMIN");
	addKey(stmi::HK_ZOOMOUT, "ZOOMOUT");
	addKey(stmi::HK_ZOOMRESET, "ZOOMRESET");
	addKey(stmi::HK_WORDPROCESSOR, "WORDPROCESSOR");
	addKey(stmi::HK_EDITOR, "EDITOR");
	addKey(stmi::HK_SPREADSHEET, "SPREADSHEET");
	addKey(stmi::HK_GRAPHICSEDITOR, "GRAPHICSEDITOR");
	addKey(stmi::HK_PRESENTATION, "PRESENTATION");
	addKey(stmi::HK_DATABASE, "DATABASE");
	addKey(stmi::HK_NEWS, "NEWS");
	addKey(stmi::HK_VOICEMAIL, "VOICEMAIL");
	addKey(stmi::HK_ADDRESSBOOK, "ADDRESSBOOK");
	addKey(stmi::HK_MESSENGER, "MESSENGER");
	addKey(stmi::HK_BRIGHTNESS_TOGGLE, "BRIGHTNESS_TOGGLE");
		//, HK_DISPLAYTOGGLE = HK_BRIGHTNESS_TOGGLE
	addKey(stmi::HK_SPELLCHECK, "SPELLCHECK");
	addKey(stmi::HK_LOGOFF, "LOGOFF");

	addKey(stmi::HK_DOLLAR, "DOLLAR");
	addKey(stmi::HK_EURO, "EURO");

	addKey(stmi::HK_FRAMEBACK, "FRAMEBACK");
	addKey(stmi::HK_FRAMEFORWARD, "FRAMEFORWARD");
	addKey(stmi::HK_CONTEXT_MENU, "CONTEXT_MENU");
	addKey(stmi::HK_MEDIA_REPEAT, "MEDIA_REPEAT");
	addKey(stmi::HK_10CHANNELSUP, "10CHANNELSUP");
	addKey(stmi::HK_10CHANNELSDOWN, "10CHANNELSDOWN");
	addKey(stmi::HK_IMAGES, "IMAGES");

	addKey(stmi::HK_DEL_EOL, "DEL_EOL");
	addKey(stmi::HK_DEL_EOS, "DEL_EOS");
	addKey(stmi::HK_INS_LINE, "INS_LINE");
	addKey(stmi::HK_DEL_LINE, "DEL_LINE");

	addKey(stmi::HK_FN, "FN");
	addKey(stmi::HK_FN_ESC, "FN_ESC");
	addKey(stmi::HK_FN_F1, "FN_F1");
	addKey(stmi::HK_FN_F2, "FN_F2");
	addKey(stmi::HK_FN_F3, "FN_F3");
	addKey(stmi::HK_FN_F4, "FN_F4");
	addKey(stmi::HK_FN_F5, "FN_F5");
	addKey(stmi::HK_FN_F6, "FN_F6");
	addKey(stmi::HK_FN_F7, "FN_F7");
	addKey(stmi::HK_FN_F8, "FN_F8");
	addKey(stmi::HK_FN_F9, "FN_F9");
	addKey(stmi::HK_FN_F10, "FN_F10");
	addKey(stmi::HK_FN_F11, "FN_F11");
	addKey(stmi::HK_FN_F12, "FN_F12");
	addKey(stmi::HK_FN_1, "FN_1");
	addKey(stmi::HK_FN_2, "FN_2");
	addKey(stmi::HK_FN_D, "FN_D");
	addKey(stmi::HK_FN_E, "FN_E");
	addKey(stmi::HK_FN_F, "FN_F");
	addKey(stmi::HK_FN_S, "FN_S");
	addKey(stmi::HK_FN_B, "FN_B");

	addKey(stmi::HK_BRL_DOT1, "BRL_DOT1");
	addKey(stmi::HK_BRL_DOT2, "BRL_DOT2");
	addKey(stmi::HK_BRL_DOT3, "BRL_DOT3");
	addKey(stmi::HK_BRL_DOT4, "BRL_DOT4");
	addKey(stmi::HK_BRL_DOT5, "BRL_DOT5");
	addKey(stmi::HK_BRL_DOT6, "BRL_DOT6");
	addKey(stmi::HK_BRL_DOT7, "BRL_DOT7");
	addKey(stmi::HK_BRL_DOT8, "BRL_DOT8");
	addKey(stmi::HK_BRL_DOT9, "BRL_DOT9");
	addKey(stmi::HK_BRL_DOT10, "BRL_DOT10");

	addKey(stmi::HK_NUMERIC_0, "NUMERIC_0");
	addKey(stmi::HK_NUMERIC_1, "NUMERIC_1");
	addKey(stmi::HK_NUMERIC_2, "NUMERIC_2");
	addKey(stmi::HK_NUMERIC_3, "NUMERIC_3");
	addKey(stmi::HK_NUMERIC_4, "NUMERIC_4");
	addKey(stmi::HK_NUMERIC_5, "NUMERIC_5");
	addKey(stmi::HK_NUMERIC_6, "NUMERIC_6");
	addKey(stmi::HK_NUMERIC_7, "NUMERIC_7");
	addKey(stmi::HK_NUMERIC_8, "NUMERIC_8");
	addKey(stmi::HK_NUMERIC_9, "NUMERIC_9");
	addKey(stmi::HK_NUMERIC_STAR, "NUMERIC_STAR");
	addKey(stmi::HK_NUMERIC_POUND, "NUMERIC_POUND");

	addKey(stmi::HK_CAMERA_FOCUS, "CAMERA_FOCUS");
	addKey(stmi::HK_WPS_BUTTON, "WPS_BUTTON");

	addKey(stmi::HK_TOUCHPAD_TOGGLE, "TOUCHPAD_TOGGLE");
	addKey(stmi::HK_TOUCHPAD_ON, "TOUCHPAD_ON");
	addKey(stmi::HK_TOUCHPAD_OFF, "TOUCHPAD_OFF");

	addKey(stmi::HK_CAMERA_ZOOMIN, "CAMERA_ZOOMIN");
	addKey(stmi::HK_CAMERA_ZOOMOUT, "CAMERA_ZOOMOUT");
	addKey(stmi::HK_CAMERA_UP, "CAMERA_UP");
	addKey(stmi::HK_CAMERA_DOWN, "CAMERA_DOWN");
	addKey(stmi::HK_CAMERA_LEFT, "CAMERA_LEFT");
	addKey(stmi::HK_CAMERA_RIGHT, "CAMERA_RIGHT");

	addKey(stmi::HK_ATTENDANT_ON, "ATTENDANT_ON");
	addKey(stmi::HK_ATTENDANT_OFF, "ATTENDANT_OFF");
	addKey(stmi::HK_ATTENDANT_TOGGLE, "ATTENDANT_TOGGLE");
	addKey(stmi::HK_LIGHTS_TOGGLE, "LIGHTS_TOGGLE");

	addKey(stmi::HK_BTN_DPAD_UP, "BTN_DPAD_UP");
	addKey(stmi::HK_BTN_DPAD_DOWN, "BTN_DPAD_DOWN");
	addKey(stmi::HK_BTN_DPAD_LEFT, "BTN_DPAD_LEFT");
	addKey(stmi::HK_BTN_DPAD_RIGHT, "BTN_DPAD_RIGHT");

	addKey(stmi::HK_ALS_TOGGLE, "ALS_TOGGLE");

	addKey(stmi::HK_BUTTONCONFIG, "BUTTONCONFIG");
	addKey(stmi::HK_TASKMANAGER, "TASKMANAGER");
	addKey(stmi::HK_JOURNAL, "JOURNAL");
	addKey(stmi::HK_CONTROLPANEL, "CONTROLPANEL");
	addKey(stmi::HK_APPSELECT, "APPSELECT");
	addKey(stmi::HK_SCREENSAVER, "SCREENSAVER");
	addKey(stmi::HK_VOICECOMMAND, "VOICECOMMAND");

	addKey(stmi::HK_BRIGHTNESS_MIN, "BRIGHTNESS_MIN");
	addKey(stmi::HK_BRIGHTNESS_MAX, "BRIGHTNESS_MAX");

	addKey(stmi::HK_KBDINPUTASSIST_PREV, "KBDINPUTASSIST_PREV");
	addKey(stmi::HK_KBDINPUTASSIST_NEXT, "KBDINPUTASSIST_NEXT");
	addKey(stmi::HK_KBDINPUTASSIST_PREVGROUP, "KBDINPUTASSIST_PREVGROUP");
	addKey(stmi::HK_KBDINPUTASSIST_NEXTGROUP, "KBDINPUTASSIST_NEXTGROUP");
	addKey(stmi::HK_KBDINPUTASSIST_ACCEPT, "KBDINPUTASSIST_ACCEPT");
	addKey(stmi::HK_KBDINPUTASSIST_CANCEL, "KBDINPUTASSIST_CANCEL");

		//addKey(stmi::HK_BTN_TRIGGER_HAPPY, "BTN_TRIGGER_HAPPY");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY1, "BTN_TRIGGER_HAPPY1");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY2, "BTN_TRIGGER_HAPPY2");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY3, "BTN_TRIGGER_HAPPY3");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY4, "BTN_TRIGGER_HAPPY4");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY5, "BTN_TRIGGER_HAPPY5");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY6, "BTN_TRIGGER_HAPPY6");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY7, "BTN_TRIGGER_HAPPY7");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY8, "BTN_TRIGGER_HAPPY8");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY9, "BTN_TRIGGER_HAPPY9");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY10, "BTN_TRIGGER_HAPPY10");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY11, "BTN_TRIGGER_HAPPY11");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY12, "BTN_TRIGGER_HAPPY12");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY13, "BTN_TRIGGER_HAPPY13");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY14, "BTN_TRIGGER_HAPPY14");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY15, "BTN_TRIGGER_HAPPY15");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY16, "BTN_TRIGGER_HAPPY16");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY17, "BTN_TRIGGER_HAPPY17");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY18, "BTN_TRIGGER_HAPPY18");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY19, "BTN_TRIGGER_HAPPY19");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY20, "BTN_TRIGGER_HAPPY20");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY21, "BTN_TRIGGER_HAPPY21");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY22, "BTN_TRIGGER_HAPPY22");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY23, "BTN_TRIGGER_HAPPY23");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY24, "BTN_TRIGGER_HAPPY24");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY25, "BTN_TRIGGER_HAPPY25");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY26, "BTN_TRIGGER_HAPPY26");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY27, "BTN_TRIGGER_HAPPY27");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY28, "BTN_TRIGGER_HAPPY28");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY29, "BTN_TRIGGER_HAPPY29");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY30, "BTN_TRIGGER_HAPPY30");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY31, "BTN_TRIGGER_HAPPY31");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY32, "BTN_TRIGGER_HAPPY32");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY33, "BTN_TRIGGER_HAPPY33");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY34, "BTN_TRIGGER_HAPPY34");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY35, "BTN_TRIGGER_HAPPY35");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY36, "BTN_TRIGGER_HAPPY36");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY37, "BTN_TRIGGER_HAPPY37");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY38, "BTN_TRIGGER_HAPPY38");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY39, "BTN_TRIGGER_HAPPY39");
	addKey(stmi::HK_BTN_TRIGGER_HAPPY40, "BTN_TRIGGER_HAPPY40");

	addKey(stmi::HK_X_SCROLL_LEFT, "X_SCROLL_LEFT");
	addKey(stmi::HK_X_SCROLL_RIGHT, "X_SCROLL_RIGHT");
}

} // namespace omni

} // namespace stmg
