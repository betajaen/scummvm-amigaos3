/* Copyright (C) 1994-2004 Revolution Software Ltd
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 */

#include "common/stdafx.h"
#include "sword2/sword2.h"
#include "sword2/driver/d_draw.h"
#include "sword2/driver/menu.h"

namespace Sword2 {

/**
 * Tell updateDisplay() that the scene needs to be completely updated.
 */

void Graphics::setNeedFullRedraw(void) {
	_needFullRedraw = true;
}

/**
 * Mark an area of the screen as dirty, first generation.
 */

void Graphics::markAsDirty(int16 x0, int16 y0, int16 x1, int16 y1) {
	int16 gridX0 = x0 / CELLWIDE;
	int16 gridY0 = y0 / CELLDEEP;
	int16 gridX1 = x1 / CELLWIDE;
	int16 gridY1 = y1 / CELLDEEP;

	for (int16 i = gridY0; i <= gridY1; i++)
		for (int16 j = gridX0; j <= gridX1; j++)
			_dirtyGrid[i * _gridWide + j] = 2;
}

/**
 * This function has two purposes: It redraws the scene, and it handles input
 * events, palette fading, etc. It should be called at a high rate (> 20 per
 * second), but the scene is usually only redrawn about 12 times per second,
 * except when then screen is scrolling.
 *
 * @param redrawScene If true, redraw the scene.
 */

void Graphics::updateDisplay(bool redrawScene) {
	_vm->parseEvents();
	fadeServer();

	if (redrawScene) {
		int i;

		// Note that the entire scene is always rendered, which is less
		// than optimal, but at least we can try to be intelligent
		// about updating the screen afterwards.

		if (_needFullRedraw) {
			// Update the entire screen. This is necessary when
			// scrolling, fading, etc.

			_vm->_system->copyRectToScreen(_buffer + MENUDEEP * _screenWide, _screenWide, 0, MENUDEEP, _screenWide, _screenDeep - 2 * MENUDEEP);
			_needFullRedraw = false;
		} else {
			// Update only the dirty areas of the screen

			int j, x, y;
			int stripWide;

			for (i = 0; i < _gridDeep; i++) {
				stripWide = 0;

				for (j = 0; j < _gridWide; j++) {
					if (_dirtyGrid[i * _gridWide + j]) {
						stripWide++;
					} else if (stripWide) {
						x = CELLWIDE * (j - stripWide);
						y = CELLDEEP * i;
						_vm->_system->copyRectToScreen(_buffer + y * _screenWide + x, _screenWide, x, y, stripWide * CELLWIDE, CELLDEEP);
						stripWide = 0;
					}
				}

				if (stripWide) {
					x = CELLWIDE * (j - stripWide);
					y = CELLDEEP * i;
					_vm->_system->copyRectToScreen(_buffer + y * _screenWide + x, _screenWide, x, y, stripWide * CELLWIDE, CELLDEEP);
					stripWide = 0;
				}
			}
		}

		// Age the dirty cells one generation. This way we keep track
		// of both the cells that were updated this time, and the ones
		// that were updated the last time.

		for (i = 0; i < _gridWide * _gridDeep; i++)
			_dirtyGrid[i] >>= 1;
	}

	// We always need to update because of fades, menu animations, etc.
	_vm->_system->updateScreen();
}

} // End of namespace Sword2
