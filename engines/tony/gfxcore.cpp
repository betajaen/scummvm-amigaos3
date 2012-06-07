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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
 * This code is based on original Tony Tough source code
 *
 * Copyright (c) 1997-2003 Nayma Software
 */

#include "tony/gfxengine.h"
#include "tony/mpal/mpalutils.h"
#include "tony/tony.h"

namespace Tony {

/****************************************************************************\
*       RMGfxTask Methods
\****************************************************************************/

RMGfxTask::RMGfxTask() {
	_nPrior = 0;
	_nInList = 0;
}

int RMGfxTask::priority() {
	return _nPrior;
}

void RMGfxTask::removeThis(CORO_PARAM, bool &result) {
	result = true;
}


/****************************************************************************\
*       RMGfxTaskSetPrior Methods
\****************************************************************************/

void RMGfxTaskSetPrior::setPriority(int nPrior) {
	_nPrior = nPrior;
}


/****************************************************************************\
*       RMGfxBuffer Methods
\****************************************************************************/

RMGfxBuffer::RMGfxBuffer() {
	_dimx = _dimy = 0;
	_bUseDDraw = false;
	_origBuf = _buf = NULL;
}

RMGfxBuffer::~RMGfxBuffer() {
	destroy();
}

void RMGfxBuffer::create(int dimx, int dimy, int nBpp, bool bUseDDraw) {
	// Destroy the buffer it is already exists
	if (_buf != NULL)
		destroy();

	// Copy the parameters in the private members
	_dimx = dimx;
	_dimy = dimy;
	_bUseDDraw = bUseDDraw;

	if (!_bUseDDraw) {
		// Allocate a buffer
		_origBuf = _buf = new byte[_dimx * _dimy * nBpp / 8];
		assert(_buf != NULL);
		Common::fill(_origBuf, _origBuf + _dimx * _dimy * nBpp / 8, 0);
	}
}

void RMGfxBuffer::destroy(void) {
	if (!_bUseDDraw) {
		if (_origBuf != NULL && _origBuf == _buf) {
			delete[] _origBuf;
			_origBuf = _buf = NULL;
		}
	}
}

void RMGfxBuffer::lock(void) {
	if (_bUseDDraw) {
		// Manages acceleration
	}
}

void RMGfxBuffer::unlock(void) {
	if (_bUseDDraw) {
		// Manages acceleration
	}
}

void RMGfxBuffer::offsetY(int nLines, int nBpp) {
	_buf += nLines * getDimx() * nBpp / 8;
}


RMGfxBuffer::operator byte *() {
	return _buf;
}

RMGfxBuffer::operator void *() {
	return (void *)_buf;
}

RMGfxBuffer::RMGfxBuffer(int dimx, int dimy, int nBpp, bool bUseDDraw) {
	create(dimx, dimy, nBpp, bUseDDraw);
}

/****************************************************************************\
*       RMGfxSourceBuffer Methods
\****************************************************************************/

int RMGfxSourceBuffer::init(const byte *buf, int dimx, int dimy, bool bLoadPalette) {
	create(dimx, dimy, Bpp());
	CopyMemory(_buf, buf, dimx * dimy * Bpp() / 8);

	// Invokes the method for preparing the surface (inherited)
	prepareImage();

	return dimx * dimy * Bpp() / 8;
}


void RMGfxSourceBuffer::init(RMDataStream &ds, int dimx, int dimy, bool bLoadPalette) {
	create(dimx, dimy, Bpp());
	ds.read(_buf, dimx * dimy * Bpp() / 8);

	// Invokes the method for preparing the surface (inherited)
	prepareImage();
}

RMGfxSourceBuffer::~RMGfxSourceBuffer() {
}

void RMGfxSourceBuffer::prepareImage(void) {
	// Do nothing. Can be overloaded if necessary
}

bool RMGfxSourceBuffer::clip2D(int &x1, int &y1, int &u, int &v, int &width, int &height, bool bUseSrc, RMGfxTargetBuffer *buf) {
	int destw, desth;

	destw = buf->getDimx();
	desth = buf->getDimy();

	if (!bUseSrc) {
		u = v = 0;
		width = _dimx;
		height = _dimy;
	}

	if (x1 > destw - 1)
		return false;

	if (y1 > desth - 1)
		return false;

	if (x1 < 0) {
		width += x1;
		if (width < 0)
			return false;
		u -= x1;
		x1 = 0;
	}

	if (y1 < 0) {
		height += y1;
		if (height < 0)
			return false;
		v -= y1;
		y1 = 0;
	}

	if (x1 + width - 1 > destw - 1)
		width = destw - x1;

	if (y1 + height - 1 > desth - 1)
		height = desth - y1;

	return true;
}


/**
 * Initialises a surface by resource Id
 *
 * @param resID                 Resource ID
 * @param dimx                  Buffer X dimension
 * @param dimy                  Buffer Y dimension
 */
int RMGfxSourceBuffer::init(uint32 resID, int dimx, int dimy, bool bLoadPalette) {
	return init(RMRes(resID), dimx, dimy, bLoadPalette);
}

/****************************************************************************\
*       RMGfxWoodyBuffer Methods
\****************************************************************************/

RMGfxWoodyBuffer::~RMGfxWoodyBuffer() {

}

void RMGfxWoodyBuffer::draw(CORO_PARAM, RMGfxTargetBuffer &bigBuf, RMGfxPrimitive *prim) {
	CORO_BEGIN_CONTEXT;
	CORO_END_CONTEXT(_ctx);

	CORO_BEGIN_CODE(_ctx);

	// Draw the OT list
	CORO_INVOKE_0(drawOT);

	// Draw itself into the target buffer
	CORO_INVOKE_2(RMGfxSourceBuffer16::draw, bigBuf, prim);

	CORO_END_CODE;
}

RMGfxWoodyBuffer::RMGfxWoodyBuffer() {

}

RMGfxWoodyBuffer::RMGfxWoodyBuffer(int dimx, int dimy, bool bUseDDraw)
	: RMGfxBuffer(dimx, dimy, 16, bUseDDraw) {

}


/****************************************************************************\
*       RMGfxTargetBuffer Methods
\****************************************************************************/

RMGfxClearTask RMGfxTargetBuffer::taskClear;

RMGfxTargetBuffer::RMGfxTargetBuffer() {
	otlist = NULL;
	_otSize = 0;
//	csModifyingOT = g_system->createMutex();
}

RMGfxTargetBuffer::~RMGfxTargetBuffer() {
	clearOT();
//	g_system->deleteMutex(csModifyingOT);
}

void RMGfxTargetBuffer::clearOT(void) {
	OTList *cur, *n;

//	g_system->lockMutex(csModifyingOT);

	cur = otlist;

	while (cur != NULL) {
		cur->prim->_task->Unregister();
		delete cur->prim;
		n = cur->next;
		delete cur;
		cur = n;
	}

	otlist = NULL;

//	g_system->unlockMutex(csModifyingOT);
}

void RMGfxTargetBuffer::drawOT(CORO_PARAM) {
	CORO_BEGIN_CONTEXT;
	OTList *cur;
	OTList *prev;
	OTList *next;
	RMGfxPrimitive *myprim;
	bool result;
	CORO_END_CONTEXT(_ctx);

	CORO_BEGIN_CODE(_ctx);

	_ctx->prev = NULL;
	_ctx->cur = otlist;

	// Lock the buffer to access it
	lock();
//	g_system->lockMutex(csModifyingOT);

	while (_ctx->cur != NULL) {
		// Call the task Draw method, passing it a copy of the original
		_ctx->myprim = _ctx->cur->prim->duplicate();
		CORO_INVOKE_2(_ctx->cur->prim->_task->draw, *this, _ctx->myprim);
		delete _ctx->myprim;

		// Check if it's time to remove the task from the OT list
		CORO_INVOKE_1(_ctx->cur->prim->_task->removeThis, _ctx->result);
		if (_ctx->result) {
			// De-register the task
			_ctx->cur->prim->_task->Unregister();

			// Delete task, freeing the memory
			delete _ctx->cur->prim;
			_ctx->next = _ctx->cur->next;
			delete _ctx->cur;

			// If it was the first item, update the list head
			if (_ctx->prev == NULL)
				otlist = _ctx->next;
			// Otherwise update the next pinter of the previous item
			else
				_ctx->prev->next = _ctx->next;

			_ctx->cur = _ctx->next;
		} else {
			// Update the pointer to the previous item, and the current to the next
			_ctx->prev = _ctx->cur;
			_ctx->cur = _ctx->cur->next;
		}
	}

//	g_system->unlockMutex(csModifyingOT);

	//Unlock after writing
	unlock();

	CORO_END_CODE;
}

void RMGfxTargetBuffer::addPrim(RMGfxPrimitive *prim) {
	int nPrior;
	OTList *cur, *n;

//	g_system->lockMutex(csModifyingOT);

	// Warn of the OT listing
	prim->_task->Register();

	// Check the priority
	nPrior = prim->_task->priority();
	n = new OTList(prim);

	// Empty list
	if (otlist == NULL) {
		otlist = n;
		otlist->next = NULL;
	}
	// Inclusion in the head
	else if (nPrior < otlist->prim->_task->priority()) {
		n->next = otlist;
		otlist = n;
	} else {
		cur = otlist;
		while (cur->next != NULL && nPrior > cur->next->prim->_task->priority())
			cur = cur->next;

		n->next = cur->next;
		cur->next = n;
	}

//	g_system->unlockMutex(csModifyingOT);
}

void RMGfxTargetBuffer::addClearTask(void) {
	addPrim(new RMGfxPrimitive(&taskClear));
}


/****************************************************************************\
*               RMGfxSourceBufferPal Methods
\****************************************************************************/

RMGfxSourceBufferPal::~RMGfxSourceBufferPal() {

}

int RMGfxSourceBufferPal::loadPaletteWA(const byte *buf, bool bSwapped) {
	int i;

	if (bSwapped)
		for (i = 0; i < (1 << Bpp()); i++) {
			_pal[i * 3 + 0] = buf[i * 3 + 2];
			_pal[i * 3 + 1] = buf[i * 3 + 1];
			_pal[i * 3 + 2] = buf[i * 3 + 0];
		}
	else
		CopyMemory(_pal, buf, (1 << Bpp()) * 3);

	preparePalette();

	return (1 << Bpp()) * 3;
}

int RMGfxSourceBufferPal::loadPalette(const byte *buf) {
	int i;

	for (i = 0; i < 256; i++)
		CopyMemory(_pal + i * 3, buf + i * 4, 3);

	preparePalette();

	return (1 << Bpp()) * 4;
}


void RMGfxSourceBufferPal::preparePalette(void) {
	int i;

	for (i = 0; i < 256; i++) {
		_palFinal[i] = (((int)_pal[i * 3 + 0] >> 3) <<  10) |
		                (((int)_pal[i * 3 + 1] >> 3) <<  5) |
		                (((int)_pal[i * 3 + 2] >> 3) <<  0);
	}
}

int RMGfxSourceBufferPal::init(const byte *buf, int dimx, int dimy, bool bLoadPalette) {
	int read;

	// Load the RAW image
	read = RMGfxSourceBuffer::init(buf, dimx, dimy);

	// Load the palette if necessary
	if (bLoadPalette)
		read += loadPaletteWA(&buf[read]);

	return read;
}

void RMGfxSourceBufferPal::init(RMDataStream &ds, int dimx, int dimy, bool bLoadPalette) {
	// Load the RAW image
	RMGfxSourceBuffer::init(ds, dimx, dimy);

	// Load the palette if necessary
	if (bLoadPalette) {
		byte *suxpal = new byte[256 * 3];
		ds.read(suxpal, 256 * 3);
		loadPaletteWA(suxpal);
		delete[] suxpal;
	}
}

int RMGfxSourceBufferPal::loadPalette(uint32 resID) {
	return loadPalette(RMRes(resID));
}

int RMGfxSourceBufferPal::loadPaletteWA(uint32 resID, bool bSwapped) {
	return loadPaletteWA(RMRes(resID), bSwapped);
}

/****************************************************************************\
*               RMGfxSourceBuffer4 Methods
\****************************************************************************/

void RMGfxSourceBuffer4::draw(CORO_PARAM, RMGfxTargetBuffer &bigBuf, RMGfxPrimitive *prim) {
}

RMGfxSourceBuffer4::RMGfxSourceBuffer4(int dimx, int dimy, bool bUseDDraw)
	: RMGfxBuffer(dimx, dimy, 4, bUseDDraw) {
	setPriority(0);
}


/**
 * Returns the number of bits per pixel of the surface
 *
 * @returns     Bit per pixel
 */
int RMGfxSourceBuffer4::Bpp() {
	return 4;
}

void RMGfxSourceBuffer4::Create(int dimx, int dimy, bool bUseDDraw) {
	RMGfxBuffer::create(dimx, dimy, 4, bUseDDraw);
}

/****************************************************************************\
*               RMGfxSourceBuffer8 Methods
\****************************************************************************/

RMGfxSourceBuffer8::~RMGfxSourceBuffer8() {

}

void RMGfxSourceBuffer8::draw(CORO_PARAM, RMGfxTargetBuffer &bigBuf, RMGfxPrimitive *prim) {
	int x, y, width, height, u, v;
	int bufx = bigBuf.getDimx();
	uint16 *buf = bigBuf;
	byte *raw = _buf;

	// Destination buffer
	RMRect dst;
	if (prim->haveDst())
		dst = prim->Dst();

	// Clipping
	if (prim->haveSrc()) {
		u = prim->Src().x1;
		v = prim->Src().y1;

		width = prim->Src().width();
		height = prim->Src().height();
	}

	if (!clip2D(dst.x1, dst.y1, u, v, width, height, prim->haveSrc(), &bigBuf))
		return;

	// Starting offset into the buffer
	buf += dst.y1 * bufx + dst.x1;

	// Normal step
	if (_bTrasp0) {
		for (y = 0; y < height; y++) {
			raw = _buf + (y + v) * _dimx + u;

			for (x = 0; x < width; x++) {
				if (*raw)
					*buf = _palFinal[*raw];
				buf++;
				raw++;
			}

			buf += bufx - width;
		}
	} else {
		for (y = 0; y < height; y++) {
			raw = _buf + (y + v) * _dimx + u;

			for (x = 0; x < width; x += 2) {
				buf[0] = _palFinal[raw[0]];
				buf[1] = _palFinal[raw[1]];

				buf += 2;
				raw += 2;
			}

			buf += bufx - width;
		}
	}
}

RMGfxSourceBuffer8::RMGfxSourceBuffer8(int dimx, int dimy, bool bUseDDraw)
	: RMGfxBuffer(dimx, dimy, 8, bUseDDraw) {
	setPriority(0);
}

RMGfxSourceBuffer8::RMGfxSourceBuffer8(bool bTrasp0) {
	_bTrasp0 = bTrasp0;
}


/**
 * Returns the number of bits per pixel of the surface
 *
 * @returns     Bit per pixel
 */
int RMGfxSourceBuffer8::Bpp() {
	return 8;
}

void RMGfxSourceBuffer8::create(int dimx, int dimy, bool bUseDDraw) {
	RMGfxBuffer::create(dimx, dimy, 8, bUseDDraw);
}

#define GETRED(x)   (((x) >> 10) & 0x1F)
#define GETGREEN(x) (((x) >> 5) & 0x1F)
#define GETBLUE(x) ((x) & 0x1F)


/****************************************************************************\
*               RMGfxSourceBuffer8AB Methods
\****************************************************************************/

RMGfxSourceBuffer8AB::~RMGfxSourceBuffer8AB() {

}

int RMGfxSourceBuffer8AB::calcTrasp(int fore, int back) {
	int r, g, b;

	r = (GETRED(fore) >> 2) + (GETRED(back) >> 1);
	g = (GETGREEN(fore) >> 2) + (GETGREEN(back) >> 1);
	b = (GETBLUE(fore) >> 2) + (GETBLUE(back) >> 1);

	if (r > 0x1F) r = 0x1F;
	if (g > 0x1F) g = 0x1F;
	if (b > 0x1F) b = 0x1F;

	return (r << 10) | (g << 5) | b;
}


void RMGfxSourceBuffer8AB::draw(CORO_PARAM, RMGfxTargetBuffer &bigBuf, RMGfxPrimitive *prim) {
	int x, y, width, height, u, v;
	int bufx = bigBuf.getDimx();
	uint16 *buf = bigBuf;
	byte *raw = _buf;

	// Destination buffer
	RMRect dst;
	if (prim->haveDst())
		dst = prim->Dst();

	// Clipping
	if (prim->haveSrc()) {
		u = prim->Src().x1;
		v = prim->Src().y1;

		width = prim->Src().width();
		height = prim->Src().height();
	}

	if (!clip2D(dst.x1, dst.y1, u, v, width, height, prim->haveSrc(), &bigBuf))
		return;

	// Starting offset into the buffer
	buf += dst.y1 * bufx + dst.x1;

	// Passaggio normale
	if (_bTrasp0) {
		for (y = 0; y < height; y++) {
			raw = _buf + (y + v) * _dimx + u;

			for (x = 0; x < width; x++) {
				if (*raw)
					*buf = calcTrasp(_palFinal[*raw], *buf);

				buf++;
				raw++;
			}

			buf += bufx - width;
		}
	} else {
		for (y = 0; y < height; y++) {
			raw = _buf + (y + v) * _dimx + u;

			for (x = 0; x < width; x += 2) {
				buf[0] = calcTrasp(_palFinal[raw[0]], buf[0]);
				buf[1] = calcTrasp(_palFinal[raw[1]], buf[1]);

				buf += 2;
				raw += 2;
			}

			buf += bufx - width;
		}
	}

	return;
}



/****************************************************************************\
*               RMGfxSourceBuffer8RLE Methods
\****************************************************************************/

byte RMGfxSourceBuffer8RLE::MegaRLEBuf[512 * 1024];

void RMGfxSourceBuffer8RLE::setAlphaBlendColor(int color) {
	alphaBlendColor = color;
}

RMGfxSourceBuffer8RLE::RMGfxSourceBuffer8RLE() {
	alphaBlendColor = -1;
	bNeedRLECompress = true;
	_buf = NULL;
}

RMGfxSourceBuffer8RLE::~RMGfxSourceBuffer8RLE() {
	if (_buf != NULL) {
		delete[] _buf;
		_buf = NULL;
	}
}


int RMGfxSourceBuffer8RLE::init(const byte *buf, int dimx, int dimy, bool bLoadPalette) {
	return RMGfxSourceBufferPal::init(buf, dimx, dimy, bLoadPalette);
}

void RMGfxSourceBuffer8RLE::init(RMDataStream &ds, int dimx, int dimy, bool bLoadPalette) {
	if (bNeedRLECompress) {
		RMGfxSourceBufferPal::init(ds, dimx, dimy, bLoadPalette);
	} else {
		int size;

		ds >> size;
		_buf = new byte[size];
		ds.read(_buf, size);

		_dimx = dimx;
		_dimy = dimy;
	}
}

void RMGfxSourceBuffer8RLE::preparePalette(void) {
	// Invoke the parent method
	RMGfxSourceBuffer8::preparePalette();

	// Handle RGB alpha blending
	if (alphaBlendColor != -1) {
		alphaR = (_palFinal[alphaBlendColor] >> 10) & 0x1F;
		alphaG = (_palFinal[alphaBlendColor] >> 5) & 0x1F;
		alphaB = (_palFinal[alphaBlendColor]) & 0x1F;
	}
}

void RMGfxSourceBuffer8RLE::prepareImage(void) {
	// Invoke the parent method
	RMGfxSourceBuffer::prepareImage();

	// Compress
	compressRLE();
}

void RMGfxSourceBuffer8RLE::setAlreadyCompressed(void) {
	bNeedRLECompress = false;
}


void RMGfxSourceBuffer8RLE::compressRLE(void) {
	int x, y;
	byte *startline;
	byte *cur;
	byte curdata;
	byte *src;
	byte *startsrc;
	int rep;

	// Perform RLE compression for lines
	cur = MegaRLEBuf;
	src = _buf;
	for (y = 0; y < _dimy; y++) {
		// Save the beginning of the line
		startline = cur;

		// Leave space for the length of the line
		cur += 2;

		// It starts from the empty space
		curdata = 0;
		rep = 0;
		startsrc = src;
		for (x = 0; x < _dimx;) {
			if ((curdata == 0 && *src == 0) || (curdata == 1 && *src == alphaBlendColor)
			        || (curdata == 2 && (*src != alphaBlendColor && *src != 0))) {
				src++;
				rep++;
				x++;
			} else {
				if (curdata == 0) {
					RLEWriteTrasp(cur, rep);
					curdata++;
				} else if (curdata == 1) {
					RLEWriteAlphaBlend(cur, rep);
					curdata++;
				} else {
					RLEWriteData(cur, rep, startsrc);
					curdata = 0;
				}

				rep = 0;
				startsrc = src;
			}
		}

		// Pending data?
		if (curdata == 1) {
			RLEWriteAlphaBlend(cur, rep);
			RLEWriteData(cur, 0, NULL);
		}

		if (curdata == 2) {
			RLEWriteData(cur, rep, startsrc);
		}

		// End of line
		RLEWriteEOL(cur);

		// Write the length of the line
		WRITE_LE_UINT16(startline, (uint16)(cur - startline));
	}

	// Delete the original image
	delete[] _buf;

	// Copy the compressed image
	x = cur - MegaRLEBuf;
	_buf = new byte[x];
	Common::copy(MegaRLEBuf, MegaRLEBuf + x, _buf);
}

void RMGfxSourceBuffer8RLE::draw(CORO_PARAM, RMGfxTargetBuffer &bigBuf, RMGfxPrimitive *prim) {
	int y;
	byte *src;
	uint16 *buf = bigBuf;
	int x1, y1, u, v, width, height;

	// Clipping
	x1 = prim->Dst().x1;
	y1 = prim->Dst().y1;
	if (!clip2D(x1, y1, u, v, width, height, false, &bigBuf))
		return;

	// Go forward through the RLE lines
	src = _buf;
	for (y = 0; y < v; y++)
		src += READ_LE_UINT16(src);

	// Calculate the position in the destination buffer
	buf += y1 * bigBuf.getDimx();

	// Loop
	if (prim->isFlipped()) {
// Eliminate horizontal clipping
//		width = m_dimx;
//		x1=prim->Dst().x1;

		// Clipping
		u = _dimx - (width + u);
		x1 = (prim->Dst().x1 + _dimx - 1) - u;

		for (y = 0; y < height; y++) {
			// Decompressione
			RLEDecompressLineFlipped(buf + x1, src + 2, u, width);

			// Next line
			src += READ_LE_UINT16(src);

			// Skip to the next line
			buf += bigBuf.getDimx();
		}
	} else {
		for (y = 0; y < height; y++) {
			// Decompression
			RLEDecompressLine(buf + x1, src + 2, u, width);

			// Next line
			src += READ_LE_UINT16(src);

			// Skip to the next line
			buf += bigBuf.getDimx();
		}
	}
}


/****************************************************************************\
*               RMGfxSourceBuffer8RLEByte Methods
\****************************************************************************/

RMGfxSourceBuffer8RLEByte::~RMGfxSourceBuffer8RLEByte() {

}

void RMGfxSourceBuffer8RLEByte::RLEWriteTrasp(byte *&cur, int rep) {
	assert(rep < 255);
	*cur ++ = rep;
}

void RMGfxSourceBuffer8RLEByte::RLEWriteAlphaBlend(byte *&cur, int rep) {
	assert(rep < 255);
	*cur ++ = rep;
}

void RMGfxSourceBuffer8RLEByte::RLEWriteData(byte *&cur, int rep, byte *src) {
	assert(rep < 256);

	*cur ++ = rep;
	if (rep > 0) {
		CopyMemory(cur, src, rep);
		cur += rep;
		src += rep;
	}

	return;
}

void RMGfxSourceBuffer8RLEByte::RLEWriteEOL(byte *&cur) {
	*cur ++ = 0xFF;
}

void RMGfxSourceBuffer8RLEByte::RLEDecompressLine(uint16 *dst, byte *src, int nStartSkip, int nLength) {
	int i, n;
	int r, g, b;

	if (nStartSkip == 0)
		goto RLEByteDoTrasp;

	while (1) {
		assert(nStartSkip > 0);

		// TRASP
		n =  *src++;
		if (n == 0xFF)
			return;

		if (n >= nStartSkip) {
			dst += n - nStartSkip;
			nLength -= n - nStartSkip;
			if (nLength > 0)
				goto RLEByteDoAlpha;
			else
				return;
		}
		nStartSkip -= n;


		assert(nStartSkip > 0);

		// ALPHA
		n = *src++;
		if (n >= nStartSkip) {
			n -= nStartSkip;
			goto RLEByteDoAlpha2;
		}
		nStartSkip -= n;

		assert(nStartSkip > 0);

		// DATA
		n = *src++;
		if (n >= nStartSkip) {
			src += nStartSkip;
			n -= nStartSkip;
			goto RLEByteDoCopy2;
		}
		nStartSkip -= n;
		src += n;
	}


	while (1) {
RLEByteDoTrasp:
		// Get the trasp of s**t
		n = *src++;

		// EOL?
		if (n == 0xFF)
			return;

		dst += n;
		nLength -= n;
		if (nLength <= 0)
			return;

RLEByteDoAlpha:
		// Alpha
		n = *src++;

RLEByteDoAlpha2:
		if (n > nLength)
			n = nLength;
		for (i = 0; i < n; i++) {
			r = (*dst >> 10) & 0x1F;
			g = (*dst >> 5) & 0x1F;
			b = *dst & 0x1F;

			r = (r >> 2) + (alphaR >> 1);
			g = (g >> 2) + (alphaG >> 1);
			b = (b >> 2) + (alphaB >> 1);

			*dst ++ = (r << 10) | (g << 5) | b;
		}

		nLength -= n;
		if (!nLength)
			return;
		assert(nLength > 0);

//RLEByteDoCopy:
		// Copy the stuff
		n = *src++;

RLEByteDoCopy2:
		if (n > nLength)
			n = nLength;

		for (i = 0; i < n; i++)
			*dst ++ = _palFinal[*src++];

		nLength -= n;
		if (!nLength)
			return;
		assert(nLength > 0);
	}
}

void RMGfxSourceBuffer8RLEByte::RLEDecompressLineFlipped(uint16 *dst, byte *src, int nStartSkip, int nLength) {
	int i, n;
	int r, g, b;

	if (nStartSkip == 0)
		goto RLEByteFlippedDoTrasp;

	while (1) {
		assert(nStartSkip > 0);

		// TRASP
		n = *src++;
		if (n == 0xFF)
			return;

		if (n >= nStartSkip) {
			dst -= n - nStartSkip;
			nLength -= n - nStartSkip;
			if (nLength > 0)
				goto RLEByteFlippedDoAlpha;
			else
				return;
		}
		nStartSkip -= n;


		assert(nStartSkip > 0);

		// ALPHA
		n = *src++;
		if (n >= nStartSkip) {
			n -= nStartSkip;
			goto RLEByteFlippedDoAlpha2;
		}
		nStartSkip -= n;

		assert(nStartSkip > 0);

		// DATA
		n = *src++;
		if (n >= nStartSkip) {
			src += nStartSkip;
			n -= nStartSkip;
			goto RLEByteFlippedDoCopy2;
		}
		nStartSkip -= n;
		src += n;
	}


	while (1) {
RLEByteFlippedDoTrasp:
		// Get the trasp of s**t
		n = *src++;

		// EOL?
		if (n == 0xFF)
			return;

		dst -= n;
		nLength -= n;
		if (nLength <= 0)
			return;

RLEByteFlippedDoAlpha:
		// Alpha
		n = *src++;

RLEByteFlippedDoAlpha2:
		if (n > nLength)
			n = nLength;
		for (i = 0; i < n; i++) {
			r = (*dst >> 10) & 0x1F;
			g = (*dst >> 5) & 0x1F;
			b = *dst & 0x1F;

			r = (r >> 2) + (alphaR >> 1);
			g = (g >> 2) + (alphaG >> 1);
			b = (b >> 2) + (alphaB >> 1);

			*dst-- = (r << 10) | (g << 5) | b;
		}

		nLength -= n;
		if (!nLength)
			return;
		assert(nLength > 0);

//RLEByteFlippedDoCopy:
		// Copy the data
		n = *src++;

RLEByteFlippedDoCopy2:
		if (n > nLength)
			n = nLength;

		for (i = 0; i < n; i++)
			*dst-- = _palFinal[*src++];

		nLength -= n;
		if (!nLength)
			return;
		assert(nLength > 0);
	}
}


/****************************************************************************\
*               RMGfxSourceBuffer8RLEWord Methods
\****************************************************************************/

RMGfxSourceBuffer8RLEWord::~RMGfxSourceBuffer8RLEWord() {

}

void RMGfxSourceBuffer8RLEWord::RLEWriteTrasp(byte *&cur, int rep) {
	WRITE_LE_UINT16(cur, rep);
	cur += 2;
}

void RMGfxSourceBuffer8RLEWord::RLEWriteAlphaBlend(byte *&cur, int rep) {
	WRITE_LE_UINT16(cur, rep);
	cur += 2;
}

void RMGfxSourceBuffer8RLEWord::RLEWriteData(byte *&cur, int rep, byte *src) {
	WRITE_LE_UINT16(cur, rep);
	cur += 2;

	if (rep > 0) {
		CopyMemory(cur, src, rep);
		cur += rep;
		src += rep;
	}
}

void RMGfxSourceBuffer8RLEWord::RLEWriteEOL(byte *&cur) {
	*cur ++ = 0xFF;
	*cur ++ = 0xFF;
}

void RMGfxSourceBuffer8RLEWord::RLEDecompressLine(uint16 *dst, byte *src, int nStartSkip, int nLength) {
	int i, n;
	int r, g, b;

	if (nStartSkip == 0)
		goto RLEWordDoTrasp;

	while (1) {
		assert(nStartSkip > 0);

		// TRASP
		n = READ_LE_UINT16(src);
		src += 2;

		if (n == 0xFFFF)
			return;

		if (n >= nStartSkip) {
			dst += n - nStartSkip;
			nLength -= n - nStartSkip;

			if (nLength > 0)
				goto RLEWordDoAlpha;
			else
				return;
		}
		nStartSkip -= n;

		assert(nStartSkip > 0);

		// ALPHA
		n = READ_LE_UINT16(src);
		src += 2;

		if (n >= nStartSkip) {
			n -= nStartSkip;
			goto RLEWordDoAlpha2;
		}
		nStartSkip -= n;

		// DATA
		n = READ_LE_UINT16(src);
		src += 2;

		if (n >= nStartSkip) {
			src += nStartSkip;
			n -= nStartSkip;
			goto RLEWordDoCopy2;
		}
		nStartSkip -= n;
		src += n;
	}


	while (1) {
RLEWordDoTrasp:
		// Get the trasp of s**t
		n = READ_LE_UINT16(src);
		src += 2;

		// EOL?
		if (n == 0xFFFF)
			return;

		dst += n;

		nLength -= n;
		if (nLength <= 0)
			return;

RLEWordDoAlpha:
		n = READ_LE_UINT16(src);
		src += 2;

RLEWordDoAlpha2:

		if (n > nLength)
			n = nLength;

		for (i = 0; i < n; i++) {
			r = (*dst >> 10) & 0x1F;
			g = (*dst >> 5) & 0x1F;
			b = *dst & 0x1F;

			r = (r >> 2) + (alphaR >> 1);
			g = (g >> 2) + (alphaG >> 1);
			b = (b >> 2) + (alphaB >> 1);

			*dst++ = (r << 10) | (g << 5) | b;
		}

		nLength -= n;
		if (!nLength)
			return;

		assert(nLength > 0);

//RLEWordDoCopy:
		// Copy the data
		n = READ_LE_UINT16(src);
		src += 2;

RLEWordDoCopy2:
		if (n > nLength)
			n = nLength;

		for (i = 0; i < n; i++)
			*dst++ = _palFinal[*src++];

		nLength -= n;
		if (!nLength)
			return;

		assert(nLength > 0);

	}
}

void RMGfxSourceBuffer8RLEWord::RLEDecompressLineFlipped(uint16 *dst, byte *src, int nStartSkip, int nLength) {
	int i, n;
	int r, g, b;

	if (nStartSkip == 0)
		goto RLEWordFlippedDoTrasp;

	while (1) {
		assert(nStartSkip > 0);

		// TRASP
		n = READ_LE_UINT16(src);
		src += 2;

		if (n == 0xFFFF)
			return;

		if (n >= nStartSkip) {
			dst -= n - nStartSkip;
			nLength -= n - nStartSkip;

			if (nLength > 0)
				goto RLEWordFlippedDoAlpha;
			else
				return;
		}
		nStartSkip -= n;

		assert(nStartSkip > 0);

		// ALPHA
		n = READ_LE_UINT16(src);
		src += 2;

		if (n >= nStartSkip) {
			n -= nStartSkip;
			goto RLEWordFlippedDoAlpha2;
		}
		nStartSkip -= n;

		// DATA
		n = READ_LE_UINT16(src);
		src += 2;

		if (n >= nStartSkip) {
			src += nStartSkip;
			n -= nStartSkip;
			goto RLEWordFlippedDoCopy2;
		}
		nStartSkip -= n;
		src += n;
	}


	while (1) {
RLEWordFlippedDoTrasp:
		// Get the trasp of s**t
		n = READ_LE_UINT16(src);
		src += 2;

		// EOL?
		if (n == 0xFFFF)
			return;

		dst -= n;

		nLength -= n;
		if (nLength <= 0)
			return;

RLEWordFlippedDoAlpha:
		n = READ_LE_UINT16(src);
		src += 2;

RLEWordFlippedDoAlpha2:

		if (n > nLength)
			n = nLength;

		for (i = 0; i < n; i++) {
			r = (*dst >> 10) & 0x1F;
			g = (*dst >> 5) & 0x1F;
			b = *dst & 0x1F;

			r = (r >> 2) + (alphaR >> 1);
			g = (g >> 2) + (alphaG >> 1);
			b = (b >> 2) + (alphaB >> 1);

			*dst-- = (r << 10) | (g << 5) | b;
		}

		nLength -= n;
		if (!nLength)
			return;

		assert(nLength > 0);

//RLEWordFlippedDoCopy:
		// Copy the data
		n = READ_LE_UINT16(src);
		src += 2;

RLEWordFlippedDoCopy2:
		if (n > nLength)
			n = nLength;

		for (i = 0; i < n; i++)
			*dst-- = _palFinal[*src++];

		nLength -= n;
		if (!nLength)
			return;

		assert(nLength > 0);
	}
}

/****************************************************************************\
*               Metodi di RMGfxSourceBuffer8RLEWord
\****************************************************************************/

RMGfxSourceBuffer8RLEWordAB::~RMGfxSourceBuffer8RLEWordAB() {

}

void RMGfxSourceBuffer8RLEWordAB::RLEDecompressLine(uint16 *dst, byte *src,  int nStartSkip, int nLength) {
	int i, n;
	int r, g, b, r2, g2, b2;

	if (!GLOBALS.bCfgTransparence) {
		RMGfxSourceBuffer8RLEWord::RLEDecompressLine(dst, src, nStartSkip, nLength);
		return;
	}

	if (nStartSkip == 0)
		goto RLEWordDoTrasp;

	while (1) {
		assert(nStartSkip > 0);

		// TRASP
		n = READ_LE_UINT16(src);
		src += 2;

		if (n == 0xFFFF)
			return;

		if (n >= nStartSkip) {
			dst += n - nStartSkip;
			nLength -= n - nStartSkip;

			if (nLength > 0)
				goto RLEWordDoAlpha;
			else
				return;
		}
		nStartSkip -= n;

		assert(nStartSkip > 0);

		// ALPHA
		n = READ_LE_UINT16(src);
		src += 2;

		if (n >= nStartSkip) {
			n -= nStartSkip;
			goto RLEWordDoAlpha2;
		}
		nStartSkip -= n;

		// DATA
		n = READ_LE_UINT16(src);
		src += 2;

		if (n >= nStartSkip) {
			src += nStartSkip;
			n -= nStartSkip;
			goto RLEWordDoCopy2;
		}
		nStartSkip -= n;
		src += n;
	}


	while (1) {
RLEWordDoTrasp:
		// Get the trasp of s**t
		n = READ_LE_UINT16(src);
		src += 2;

		// EOL?
		if (n == 0xFFFF)
			return;

		dst += n;

		nLength -= n;
		if (nLength <= 0)
			return;

RLEWordDoAlpha:
		n = READ_LE_UINT16(src);
		src += 2;

RLEWordDoAlpha2:

		if (n > nLength)
			n = nLength;

		// @@@ SHOULD NOT BE THERE !!!!!
		for (i = 0; i < n; i++) {
			r = (*dst >> 10) & 0x1F;
			g = (*dst >> 5) & 0x1F;
			b = *dst & 0x1F;

			r = (r >> 2) + (alphaR >> 1);
			g = (g >> 2) + (alphaG >> 1);
			b = (b >> 2) + (alphaB >> 1);

			*dst++ = (r << 10) | (g << 5) | b;
		}

		nLength -= n;
		if (!nLength)
			return;

		assert(nLength > 0);

//RLEWordDoCopy:
		// Copy the data
		n = READ_LE_UINT16(src);
		src += 2;

RLEWordDoCopy2:
		if (n > nLength)
			n = nLength;

		for (i = 0; i < n; i++) {
			r = (*dst >> 10) & 0x1F;
			g = (*dst >> 5) & 0x1F;
			b = *dst & 0x1F;

			r2 = (_palFinal[*src] >> 10) & 0x1F;
			g2 = (_palFinal[*src] >> 5) & 0x1F;
			b2 = _palFinal[*src] & 0x1F;

			r = (r >> 1) + (r2 >> 1);
			g = (g >> 1) + (g2 >> 1);
			b = (b >> 1) + (b2 >> 1);

			*dst ++ = (r << 10) | (g << 5) | b;
			src++;
		}

		nLength -= n;
		if (!nLength)
			return;

		assert(nLength > 0);

	}
}


/****************************************************************************\
*               Metodi di RMGfxSourceBuffer8AA
\****************************************************************************/

byte RMGfxSourceBuffer8AA::MegaAABuf[256 * 1024];
byte RMGfxSourceBuffer8AA::MegaAABuf2[64 * 1024];

void RMGfxSourceBuffer8AA::prepareImage(void) {
	// Invoke the parent method
	RMGfxSourceBuffer::prepareImage();

	// Prepare the buffer for anti-aliasing
	calculateAA();
}


void RMGfxSourceBuffer8AA::calculateAA(void) {
	// I suck, you suck, he sucks, we suck, they all suck  ---> ANTI ALIASING SUX!
	// ************************************************************

	int x, y;
	byte *src, *srcaa;

	/* First pass: fill the edges */
	Common::fill(MegaAABuf, MegaAABuf + _dimx * _dimy, 0);

	src = _buf;
	srcaa = MegaAABuf;
	for (y = 0; y < _dimy; y++) {
		for (x = 0; x < _dimx; x++) {
			if (*src == 0) {
				if ((y > 0 && src[-_dimx] != 0) ||
				        (y < _dimy - 1 && src[_dimx] != 0) ||
				        (x > 0 && src[-1] != 0) ||
				        (x < _dimx - 1 && src[1] != 0))
					*srcaa = 1;
			}

			src++;
			srcaa++;
		}
	}

	src = _buf;
	srcaa = MegaAABuf;
	for (y = 0; y < _dimy; y++) {
		for (x = 0; x < _dimx; x++) {
			if (*src != 0) {
				if ((y > 0 && srcaa[-_dimx] == 1) ||
				        (y < _dimy - 1 && srcaa[_dimx] == 1) ||
				        (x > 0 && srcaa[-1] == 1) ||
				        (x < _dimx - 1 && srcaa[1] == 1))
					*srcaa = 2;
			}

			src++;
			srcaa++;
		}
	}

	if (_aabuf != NULL)
		delete[] _aabuf;

	_aabuf = new byte[_dimx * _dimy];
	CopyMemory(_aabuf, MegaAABuf, _dimx * _dimy);
}

RMGfxSourceBuffer8AA::RMGfxSourceBuffer8AA() : RMGfxSourceBuffer8() {
	_aabuf = NULL;
}

RMGfxSourceBuffer8AA::~RMGfxSourceBuffer8AA() {
	if (_aabuf != NULL)
		delete[] _aabuf;
}

void RMGfxSourceBuffer8AA::drawAA(RMGfxTargetBuffer &bigBuf, RMGfxPrimitive *prim) {
	int x, y;
	byte *src;
	uint16 *mybuf;
	uint16 *buf;
	int x1, y1, u, v, width, height;
	int r, g, b;
	int step;

	// Clip the sprite
	x1 = prim->Dst().x1;
	y1 = prim->Dst().y1;
	if (!clip2D(x1, y1, u, v, width, height, false, &bigBuf))
		return;

	// Go forward through the RLE lines
	src = _buf;
	for (y = 0; y < v; y++)
		src += READ_LE_UINT16(src);

	// Eliminate horizontal clipping

	if (prim->isFlipped()) {
		u = _dimx - (width + u);
		x1 = (prim->Dst().x1 + _dimx - 1) - u;
	}
//	width = _dimx;
//	x1 = prim->Dst().x1;


	// Poisition into the destination buffer
	buf = bigBuf;
	buf += y1 * bigBuf.getDimx();

	if (prim->isFlipped())
		step = -1;
	else
		step = 1;

	// Loop
	buf += bigBuf.getDimx(); // Skip the first line
	for (y = 1; y < height - 1; y++) {
		/*
		        if (prim->IsFlipped())
		            mybuf=&buf[x1+m_dimx-1];
		        else
		*/
		mybuf = &buf[x1];

		for (x = 0; x < width; x++, mybuf += step)
			if (_aabuf[(y + v) * _dimx + x + u] == 2 && x != 0 && x != width - 1) {
				r = GETRED(mybuf[1]) + GETRED(mybuf[-1]) + GETRED(mybuf[-bigBuf.getDimx()]) + GETRED(mybuf[bigBuf.getDimx()]);
				g = GETGREEN(mybuf[1]) + GETGREEN(mybuf[-1]) + GETGREEN(mybuf[-bigBuf.getDimx()]) + GETGREEN(mybuf[bigBuf.getDimx()]);
				b = GETBLUE(mybuf[1]) + GETBLUE(mybuf[-1]) + GETBLUE(mybuf[-bigBuf.getDimx()]) + GETBLUE(mybuf[bigBuf.getDimx()]);

				r += GETRED(mybuf[0]);
				g += GETGREEN(mybuf[0]);
				b += GETBLUE(mybuf[0]);

				r /= 5;
				g /= 5;
				b /= 5;

				if (r > 31) r = 31;
				if (g > 31) g = 31;
				if (b > 31) b = 31;

				mybuf[0] = (r << 10) | (g << 5) | b;
			}

		// Skip to the next line
		buf += bigBuf.getDimx();
	}

// Position into the destination buffer
	buf = bigBuf;
	buf += y1 * bigBuf.getDimx();

	// Looppone
	buf += bigBuf.getDimx();
	for (y = 1; y < height - 1; y++) {
		/*
		        if (prim->IsFlipped())
		            mybuf=&buf[x1+m_dimx-1];
		        else
		*/
		mybuf = &buf[x1];

		for (x = 0; x < width; x++, mybuf += step)
			if (_aabuf[(y + v) * _dimx + x + u] == 1 && x != 0 && x != width - 1) {
				r = GETRED(mybuf[1]) + GETRED(mybuf[-1]) + GETRED(mybuf[-bigBuf.getDimx()]) + GETRED(mybuf[bigBuf.getDimx()]);
				g = GETGREEN(mybuf[1]) + GETGREEN(mybuf[-1]) + GETGREEN(mybuf[-bigBuf.getDimx()]) + GETGREEN(mybuf[bigBuf.getDimx()]);
				b = GETBLUE(mybuf[1]) + GETBLUE(mybuf[-1]) + GETBLUE(mybuf[-bigBuf.getDimx()]) + GETBLUE(mybuf[bigBuf.getDimx()]);

				r += GETRED(mybuf[0]) * 2;
				g += GETGREEN(mybuf[0]) * 2;
				b += GETBLUE(mybuf[0]) * 2;

				r /= 6;
				g /= 6;
				b /= 6;

				if (r > 31) r = 31;
				if (g > 31) g = 31;
				if (b > 31) b = 31;

				mybuf[0] = (r << 10) | (g << 5) | b;
			}

		// Skippa alla linea successiva
		buf += bigBuf.getDimx();
	}
}



void RMGfxSourceBuffer8AA::draw(CORO_PARAM, RMGfxTargetBuffer &bigBuf, RMGfxPrimitive *prim) {
	CORO_BEGIN_CONTEXT;
	CORO_END_CONTEXT(_ctx);

	CORO_BEGIN_CODE(_ctx);

	CORO_INVOKE_2(RMGfxSourceBuffer8::draw, bigBuf, prim);
	drawAA(bigBuf, prim);

	CORO_END_CODE;
}


/****************************************************************************\
*               RMGfxSourceBuffer8RLEAA Methods
\****************************************************************************/

RMGfxSourceBuffer8RLEByteAA::~RMGfxSourceBuffer8RLEByteAA() {

}

void RMGfxSourceBuffer8RLEByteAA::prepareImage(void) {
	RMGfxSourceBuffer::prepareImage();
	calculateAA();
	compressRLE();
}

void RMGfxSourceBuffer8RLEByteAA::draw(CORO_PARAM, RMGfxTargetBuffer &bigBuf, RMGfxPrimitive *prim) {
	CORO_BEGIN_CONTEXT;
	CORO_END_CONTEXT(_ctx);

	CORO_BEGIN_CODE(_ctx);

	CORO_INVOKE_2(RMGfxSourceBuffer8RLE::draw, bigBuf, prim);
	if (GLOBALS.bCfgAntiAlias)
		drawAA(bigBuf, prim);

	CORO_END_CODE;
}

int RMGfxSourceBuffer8RLEByteAA::init(const byte *buf, int dimx, int dimy, bool bLoadPalette) {
	return RMGfxSourceBuffer8RLE::init(buf, dimx, dimy, bLoadPalette);
}

void RMGfxSourceBuffer8RLEByteAA::init(RMDataStream &ds, int dimx, int dimy, bool bLoadPalette) {
	RMGfxSourceBuffer8RLE::init(ds, dimx, dimy, bLoadPalette);

	if (!bNeedRLECompress) {
		// Load the anti-aliasing mask
		_aabuf = new byte[dimx * dimy];
		ds.read(_aabuf, dimx * dimy);
	}
}


RMGfxSourceBuffer8RLEWordAA::~RMGfxSourceBuffer8RLEWordAA() {

}

void RMGfxSourceBuffer8RLEWordAA::prepareImage(void) {
	RMGfxSourceBuffer::prepareImage();
	calculateAA();
	compressRLE();
}

void RMGfxSourceBuffer8RLEWordAA::draw(CORO_PARAM, RMGfxTargetBuffer &bigBuf, RMGfxPrimitive *prim) {
	CORO_BEGIN_CONTEXT;
	CORO_END_CONTEXT(_ctx);

	CORO_BEGIN_CODE(_ctx);

	CORO_INVOKE_2(RMGfxSourceBuffer8RLE::draw, bigBuf, prim);
	if (GLOBALS.bCfgAntiAlias)
		drawAA(bigBuf, prim);

	CORO_END_CODE;
}

int RMGfxSourceBuffer8RLEWordAA::init(byte *buf, int dimx, int dimy, bool bLoadPalette) {
	return RMGfxSourceBuffer8RLE::init(buf, dimx, dimy, bLoadPalette);
}

void RMGfxSourceBuffer8RLEWordAA::init(RMDataStream &ds, int dimx, int dimy, bool bLoadPalette) {
	RMGfxSourceBuffer8RLE::init(ds, dimx, dimy, bLoadPalette);

	if (!bNeedRLECompress) {
		// Load the anti-aliasing mask
		_aabuf = new byte[dimx * dimy];
		ds.read(_aabuf, dimx * dimy);
	}
}


/****************************************************************************\
*               RMGfxSourceBuffer16 Methods
\****************************************************************************/

RMGfxSourceBuffer16::RMGfxSourceBuffer16(bool bTrasp0) {
	_bTrasp0 = bTrasp0;
}

RMGfxSourceBuffer16::~RMGfxSourceBuffer16() {
}

void RMGfxSourceBuffer16::draw(CORO_PARAM, RMGfxTargetBuffer &bigBuf, RMGfxPrimitive *prim) {
	int x, y;
	uint16 *buf = bigBuf;
	uint16 *raw = (uint16 *)_buf;
	int dimx, dimy;
	int u, v;
	int x1, y1;

	dimx = _dimx;
	dimy = _dimy;
	u = 0;
	v = 0;
	x1 = 0;
	y1 = 0;

	if (prim->haveSrc()) {
		u = prim->Src().x1;
		v = prim->Src().y1;
		dimx = prim->Src().width();
		dimy = prim->Src().height();
	}

	if (prim->haveDst()) {
		x1 = prim->Dst().x1;
		y1 = prim->Dst().y1;
	}

	if (!clip2D(x1, y1, u, v, dimx, dimy, true, &bigBuf))
		return;

	raw += v * _dimx + u;
	buf += y1 * bigBuf.getDimx() + x1;

	if (_bTrasp0) {
		for (y = 0; y < dimy; y++) {
			for (x = 0; x < dimx;) {
				while (x < dimx && raw[x] == 0)
					x++;

				while (x < dimx && raw[x] != 0) {
					buf[x] = raw[x];
					x++;
				}
			}

			raw += _dimx;
			buf += bigBuf.getDimx();
		}
	} else {
		for (y = 0; y < dimy; y++) {
			Common::copy(raw, raw + dimx, buf);
			buf += bigBuf.getDimx();
			raw += _dimx;
		}
	}
}

void RMGfxSourceBuffer16::PrepareImage(void) {
	// Colour space conversion if necessary!
	int i;
	uint16 *buf = (uint16 *)_buf;

	for (i = 0; i < _dimx * _dimy; i++)
		WRITE_LE_UINT16(&buf[i], FROM_LE_16(buf[i]) & 0x7FFF);
}


RMGfxSourceBuffer16::RMGfxSourceBuffer16(int dimx, int dimy, bool bUseDDraw)
	: RMGfxBuffer(dimx, dimy, 16, bUseDDraw) {
	setPriority(0);
}


/**
 * Returns the number of bits per pixel of the surface
 *
 * @returns     Bit per pixel
 */
int RMGfxSourceBuffer16::Bpp() {
	return 16;
}

void RMGfxSourceBuffer16::Create(int dimx, int dimy, bool bUseDDraw) {
	RMGfxBuffer::create(dimx, dimy, 16, bUseDDraw);
}

/****************************************************************************\
*               RMGfxBox Methods
\****************************************************************************/

void RMGfxBox::removeThis(CORO_PARAM, bool &result) {
	result = true;
}

void RMGfxBox::SetColor(byte r, byte g, byte b) {
	r >>= 3;
	g >>= 3;
	b >>= 3;
	wFillColor = (r << 10) | (g << 5) | b;
}

void RMGfxBox::draw(CORO_PARAM, RMGfxTargetBuffer &bigBuf, RMGfxPrimitive *prim) {
	int i, j;
	uint16 *buf = bigBuf;
	RMRect rcDst;

	// It takes the destination rectangle
	rcDst = prim->Dst();
	buf += rcDst.y1 * bigBuf.getDimx() + rcDst.x1;

	// Loop through the pixels
	for (j = 0; j < rcDst.height(); j++) {
		for (i = 0; i < rcDst.width(); i++)
			*buf ++ = wFillColor;

		buf += bigBuf.getDimx() - rcDst.width();
	}
}


/****************************************************************************\
*       RMGfxClearTask Methods
\****************************************************************************/

int RMGfxClearTask::priority() {
	// Maximum priority (must be done first)
	return 1;
}

void RMGfxClearTask::draw(CORO_PARAM, RMGfxTargetBuffer &bigBuf, RMGfxPrimitive *) {
	// Clean the target buffer
	Common::fill((byte *)bigBuf, (byte *)bigBuf + (bigBuf.getDimx() * bigBuf.getDimy() * 2), 0x0);
}

void RMGfxClearTask::removeThis(CORO_PARAM, bool &result) {
	// The task is fine to be removed
	result = true;
}

} // End of namespace Tony
