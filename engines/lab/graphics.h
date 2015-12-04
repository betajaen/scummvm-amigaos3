/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
 * This code is based on Labyrinth of Time code with assistance of
 *
 * Copyright (c) 1993 Terra Nova Development
 * Copyright (c) 2004 The Wyrmkeep Entertainment Co.
 *
 */

#ifndef LAB_GRAPHICS_H
#define LAB_GRAPHICS_H

namespace Lab {

class LabEngine;

class DisplayMan {
private:
	LabEngine *_vm;

	byte _curapen;
	byte *_curBitmap;

public:
	DisplayMan(LabEngine *lab);
	virtual ~DisplayMan();

	uint16 scaleX(uint16 x);
	uint16 scaleY(uint16 y);
	int16 VGAScaleX(int16 x);
	int16 VGAScaleY(int16 y);
	uint16 SVGACord(uint16 cord);
	void loadPict(const char *filename);
	bool readPict(const char *filename, bool playOnce);
	void freePict();
	byte *readPictToMem(const char *filename, uint16 x, uint16 y);
	void doScrollBlack();
	void copyPage(uint16 width, uint16 height, uint16 nheight, uint16 startline, byte *mem);
	void doScrollWipe(char *filename);
	void doScrollBounce();
	void doTransWipe(CloseDataPtr *cPtr, char *filename);
	void doWipe(uint16 wipeType, CloseDataPtr *cPtr, char *filename);
	void blackScreen();
	void whiteScreen();
	void blackAllScreen();
	void createBox(uint16 y2);
	void drawPanel();
	bool setUpScreens();
	int32 longDrawMessage(const char *str);
	void drawMessage(const char *str);
	void setAPen(byte pennum);
	void rectFill(uint16 x1, uint16 y1, uint16 x2, uint16 y2);
	/* Window text stuff */
	uint32 flowText(void *font,				/* the TextAttr pointer */
					int16 spacing,			/* How much vertical spacing between the lines */
					byte pencolor,			/* pen number to use for text */
					byte backpen,			/* the background color */
					bool fillback,			/* Whether to fill the background */
					bool centerh,			/* Whether to center the text horizontally */
					bool centerv,			/* Whether to center the text vertically */
					bool output,			/* Whether to output any text */
					uint16 x1, uint16 y1,	/* Cords */
					uint16 x2, uint16 y2,
					const char *text);		/* The text itself */

	uint32 flowTextToMem(Image *destIm,
					void *font,				/* the TextAttr pointer */
					int16 spacing,			/* How much vertical spacing between the lines */
					byte pencolor,			/* pen number to use for text */
					byte backpen,			/* the background color */
					bool fillback,			/* Whether to fill the background */
					bool centerh,			/* Whether to center the text horizontally */
					bool centerv,			/* Whether to center the text vertically */
					bool output,			/* Whether to output any text */
					uint16 x1, uint16 y1,	/* Cords */
					uint16 x2, uint16 y2,
					const char *str);		/* The text itself */

	void drawHLine(uint16 x, uint16 y1, uint16 y2);
	void drawVLine(uint16 x1, uint16 y, uint16 x2);
	void screenUpdate();
	bool createScreen(bool HiRes);

	bool _longWinInFront;
	bool _lastMessageLong;
	uint32 _screenBytesPerPage;
};

} // End of namespace Lab

#endif // LAB_GRAPHICS_H
