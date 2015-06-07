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

#include "sherlock/animation.h"
#include "sherlock/sherlock.h"
#include "common/algorithm.h"

namespace Sherlock {

static const int NO_FRAMES = FRAMES_END;

Animation::Animation(SherlockEngine *vm) : _vm(vm) {
}

bool Animation::play(const Common::String &filename, int minDelay, int fade,
		bool setPalette, int speed) {
	Events &events = *_vm->_events;
	Screen &screen = *_vm->_screen;
	Sound &sound = *_vm->_sound;
	int soundNumber = 0;

	// Check for any any sound frames for the given animation
	const int *soundFrames = checkForSoundFrames(filename);

	// Add on the VDX extension
	Common::String vdxName = filename + ".vdx";

	// Load the animation
	Common::SeekableReadStream *stream;
	if (!_gfxLibraryFilename.empty())
		stream = _vm->_res->load(vdxName, _gfxLibraryFilename);
	else if (_vm->_useEpilogue2)
		stream = _vm->_res->load(vdxName, "epilog2.lib");
	else
		stream = _vm->_res->load(vdxName, "epilogue.lib");

	// Load initial image
	Common::String vdaName = filename + ".vda";
	ImageFile images(vdaName, true, true);

	events.wait(minDelay);
	if (fade != 0 && fade != 255)
		screen.fadeToBlack();

	if (setPalette) {
		if (fade != 255)
			screen.setPalette(images._palette);
	}

	int frameNumber = 0;
	Common::Point pt;
	bool skipped = false;
	while (!_vm->shouldQuit()) {
		// Get the next sprite to display
		int imageFrame = stream->readSint16LE();

		if (imageFrame == -2) {
			// End of animation reached
			break;
		} else if (imageFrame != -1) {
			// Read position from either animation stream or the sprite frame itself
			if (imageFrame < 0) {
				imageFrame += 32768;
				pt.x = stream->readUint16LE();
				pt.y = stream->readUint16LE();
			} else {
				pt = images[imageFrame]._offset;
			}

			// Draw the sprite. Note that we explicitly use the raw frame below, rather than the ImageFrame,
			// since we don't want the offsets in the image file to be used, just the explicit position we specify
			screen.transBlitFrom(images[imageFrame]._frame, pt);
		} else {
			// At this point, either the sprites for the frame has been complete, or there weren't any sprites
			// at all to draw for the frame
			if (fade == 255) {
				// Gradual fade in
				if (screen.equalizePalette(images._palette) == 0)
					fade = 0;
			}

			// Check if we've reached a frame with sound
			if (frameNumber++ == *soundFrames) {
				++soundNumber;
				++soundFrames;
				Common::String fname = _soundLibraryFilename.empty() ?
					Common::String::format("%s%01d", filename.c_str(), soundNumber) :
					Common::String::format("%s%02d", filename.c_str(), soundNumber);

				if (sound._voices)
					sound.playSound(fname, WAIT_RETURN_IMMEDIATELY, 100, _soundLibraryFilename.c_str());
			}

			events.wait(speed * 3);
		}

		if (events.kbHit()) {
			Common::KeyState keyState = events.getKey();
			if (keyState.keycode == Common::KEYCODE_ESCAPE ||
				keyState.keycode == Common::KEYCODE_SPACE) {
				skipped = true;
				break;
			}
		} else if (events._pressed) {
			skipped = true;
			break;
		}
	}

	events.clearEvents();
	sound.stopSound();
	delete stream;

	return !skipped && !_vm->shouldQuit();
}

bool Animation::play3DO(const Common::String &filename, int minDelay, int fade,
		int speed) {
	Events &events = *_vm->_events;
	Screen &screen = *_vm->_screen;
	Sound &sound = *_vm->_sound;
	//int soundNumber = 0;

	// Check for any any sound frames for the given animation
	//const int *soundFrames = checkForSoundFrames(filename);

	// Add on the VDX extension
	Common::String indexName = "prologue/" + filename + ".3dx";

	// Load the animation
	Common::File *indexStream = new Common::File();

	if (!indexStream->open(indexName)) {
		warning("unable to open %s\n", indexName.c_str());
		return false;
	}

	// Load initial image
	Common::String graphicsName = "prologue/" + filename + ".3da";
	ImageFile3DO images(graphicsName, true);

	events.wait(minDelay);
//	if (fade != 0 && fade != 255)
//		screen.fadeToBlack();

//	if (setPalette) {
//		if (fade != 255)
//			screen.setPalette(images._palette);
//	}

	//int frameNumber = 0;
	Common::Point pt;
	bool skipped = false;
	while (!_vm->shouldQuit()) {
		// Get the next sprite to display
		int imageFrame = indexStream->readSint16BE();

		if (imageFrame == -2) {
			// End of animation reached
			break;
		} else if (imageFrame != -1) {
			// Read position from either animation stream or the sprite frame itself
			if (imageFrame < 0) {
				imageFrame += 32768;
				pt.x = indexStream->readUint16BE();
				pt.y = indexStream->readUint16BE();
			} else {
				pt = images[imageFrame]._offset;
			}

			// Draw the sprite. Note that we explicitly use the raw frame below, rather than the ImageFrame,
			// since we don't want the offsets in the image file to be used, just the explicit position we specify
			screen.transBlitFromUnscaled3DO(images[imageFrame]._frame, pt);
			//events.wait(1000);
		} else {
#if 0
			// At this point, either the sprites for the frame has been complete, or there weren't any sprites
			// at all to draw for the frame
			//if (fade == 255) {
			//	// Gradual fade in
			//	if (screen.equalizePalette(images._palette) == 0)
			//		fade = 0;
			//}

			// Check if we've reached a frame with sound
			if (frameNumber++ == *soundFrames) {
				++soundNumber;
				++soundFrames;
				Common::String fname = _soundLibraryFilename.empty() ?
					Common::String::format("%s%01d", filename.c_str(), soundNumber) :
					Common::String::format("%s%02d", filename.c_str(), soundNumber);

				if (sound._voices)
					sound.playSound(fname, WAIT_RETURN_IMMEDIATELY, 100, _soundLibraryFilename.c_str());
			}
#endif

			events.wait(speed * 3);
		}

		if (events.kbHit()) {
			Common::KeyState keyState = events.getKey();
			if (keyState.keycode == Common::KEYCODE_ESCAPE ||
				keyState.keycode == Common::KEYCODE_SPACE) {
				skipped = true;
				break;
			}
		} else if (events._pressed) {
			skipped = true;
			break;
		}
	}

	events.clearEvents();
	sound.stopSound();
	delete indexStream;

	return !skipped && !_vm->shouldQuit();
}

void Animation::setPrologueNames(const char *const *names, int count) {
	for (int idx = 0; idx < count; ++idx, ++names) {
		_prologueNames.push_back(*names);
	}
}

void Animation::setPrologueFrames(const int *frames, int count, int maxFrames) {
	_prologueFrames.resize(count);

	for (int idx = 0; idx < count; ++idx, frames += maxFrames) {
		_prologueFrames[idx].resize(maxFrames);
		Common::copy(frames, frames + maxFrames, &_prologueFrames[idx][0]);
	}
}

void Animation::setTitleNames(const char *const *names, int count) {
	for (int idx = 0; idx < count; ++idx, ++names) {
		_titleNames.push_back(*names);
	}
}

void Animation::setTitleFrames(const int *frames, int count, int maxFrames) {
	_titleFrames.resize(count);

	for (int idx = 0; idx < count; ++idx, frames += maxFrames) {
		_titleFrames[idx].resize(maxFrames);
		Common::copy(frames, frames + maxFrames, &_titleFrames[idx][0]);
	}
}

const int *Animation::checkForSoundFrames(const Common::String &filename) {
	const int *frames = &NO_FRAMES;

	if (_soundLibraryFilename.empty()) {
		for (uint idx = 0; idx < _prologueNames.size(); ++idx) {
			if (filename.equalsIgnoreCase(_prologueNames[idx])) {
				frames = &_prologueFrames[idx][0];
				break;
			}
		}
	} else {
		for (uint idx = 0; idx < _titleNames.size(); ++idx) {
			if (filename.equalsIgnoreCase(_titleNames[idx])) {
				frames = &_titleFrames[idx][0];
				break;
			}
		}
	}

	return frames;
}

} // End of namespace Sherlock
