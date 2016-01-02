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

#ifndef BACKENDS_GRAPHICS_OPENGL_TEXTURE_H
#define BACKENDS_GRAPHICS_OPENGL_TEXTURE_H

#include "backends/graphics/opengl/opengl-sys.h"

#include "graphics/pixelformat.h"
#include "graphics/surface.h"

#include "common/rect.h"

namespace OpenGL {

/**
 * A simple GL texture object abstraction.
 *
 * This is used for low-level GL texture handling.
 */
class GLTexture {
public:
	/**
	 * Constrcut a new GL texture object.
	 *
	 * @param glIntFormat The internal format to use.
	 * @param glFormat    The input format.
	 * @param glType      The input type.
	 */
	GLTexture(GLenum glIntFormat, GLenum glFormat, GLenum glType);
	~GLTexture();

	/**
	 * Enable or disable linear texture filtering.
	 *
	 * @param enable true to enable and false to disable.
	 */
	void enableLinearFiltering(bool enable);

	/**
	 * Test whether linear filtering is enabled.
	 */
	bool isLinearFilteringEnabled() const { return (_glFilter == GL_LINEAR); }

	/**
	 * Destroy the OpenGL texture name.
	 */
	void destroy();

	/**
	 * Create the OpenGL texture name.
	 */
	void create();

	/**
	 * Bind the texture to the active texture unit.
	 */
	void bind();

	/**
	 * Sets the size of the texture in pixels.
	 *
	 * The internal OpenGL texture might have a different size. To query the
	 * actual size use getWidth()/getHeight().
	 *
	 * @param width  The desired logical width.
	 * @param height The desired logical height.
	 */
	void setSize(uint width, uint height);

	/**
	 * Copy image data to the texture.
	 *
	 * @param area     The area to update.
	 * @param src      Surface for the whole texture containing the pixel data
	 *                 to upload. Only the area described by area will be
	 *                 uploaded.
	 */
	void updateArea(const Common::Rect &area, const Graphics::Surface &src);

	uint getWidth() const { return _width; }
	uint getHeight() const { return _height; }

	/**
	 * Obtain texture coordinates for rectangular drawing.
	 */
	const GLfloat *getTexCoords() const { return _texCoords; }
private:
	const GLenum _glIntFormat;
	const GLenum _glFormat;
	const GLenum _glType;

	uint _width, _height;
	GLfloat _texCoords[4*2];

	GLint _glFilter;

	GLuint _glTexture;
};

/**
 * An OpenGL texture wrapper. It automatically takes care of all OpenGL
 * texture handling issues and also provides access to the texture data.
 */
class Texture {
public:
	/**
	 * Create a new texture with the specific internal format.
	 *
	 * @param glIntFormat The internal format to use.
	 * @param glFormat    The input format.
	 * @param glType      The input type.
	 * @param format      The format used for the texture input.
	 */
	Texture(GLenum glIntFormat, GLenum glFormat, GLenum glType, const Graphics::PixelFormat &format);
	virtual ~Texture();

	/**
	 * Destroy the OpenGL texture name.
	 */
	void releaseInternalTexture();

	/**
	 * Create the OpenGL texture name and flag the whole texture as dirty.
	 */
	void recreateInternalTexture();

	/**
	 * Enable or disable linear texture filtering.
	 *
	 * @param enable true to enable and false to disable.
	 */
	void enableLinearFiltering(bool enable);

	/**
	 * Allocate texture space for the desired dimensions. This wraps any
	 * handling of requirements for POT textures.
	 *
	 * @param width  The desired logical width.
	 * @param height The desired logical height.
	 */
	virtual void allocate(uint width, uint height);

	void copyRectToTexture(uint x, uint y, uint w, uint h, const void *src, uint srcPitch);

	void fill(uint32 color);

	void draw(GLfloat x, GLfloat y, GLfloat w, GLfloat h);

	void flagDirty() { _allDirty = true; }
	bool isDirty() const { return _allDirty || !_dirtyArea.isEmpty(); }

	uint getWidth() const { return _userPixelData.w; }
	uint getHeight() const { return _userPixelData.h; }

	/**
	 * @return The logical format of the texture data.
	 */
	virtual Graphics::PixelFormat getFormat() const { return _format; }

	virtual Graphics::Surface *getSurface() { return &_userPixelData; }
	virtual const Graphics::Surface *getSurface() const { return &_userPixelData; }

	/**
	 * @return Whether the texture data is using a palette.
	 */
	virtual bool hasPalette() const { return false; }

	/**
	 * Set color key for paletted textures.
	 *
	 * This needs to be called after any palette update affecting the color
	 * key. Calling this multiple times will result in multiple color indices
	 * to be treated as color keys.
	 */
	virtual void setColorKey(uint colorKey) {}
	virtual void setPalette(uint start, uint colors, const byte *palData) {}
protected:
	const Graphics::PixelFormat _format;

	virtual void updateTexture();

	Common::Rect getDirtyArea() const;
private:
	GLTexture _glTexture;

	Graphics::Surface _textureData;
	Graphics::Surface _userPixelData;

	bool _allDirty;
	Common::Rect _dirtyArea;
	void clearDirty() { _allDirty = false; _dirtyArea = Common::Rect(); }
};

class TextureCLUT8 : public Texture {
public:
	TextureCLUT8(GLenum glIntFormat, GLenum glFormat, GLenum glType, const Graphics::PixelFormat &format);
	virtual ~TextureCLUT8();

	virtual void allocate(uint width, uint height);

	virtual Graphics::PixelFormat getFormat() const;

	virtual bool hasPalette() const { return true; }

	virtual void setColorKey(uint colorKey);
	virtual void setPalette(uint start, uint colors, const byte *palData);

	virtual Graphics::Surface *getSurface() { return &_clut8Data; }
	virtual const Graphics::Surface *getSurface() const { return &_clut8Data; }

protected:
	virtual void updateTexture();

private:
	Graphics::Surface _clut8Data;
	byte *_palette;
};

#if !USE_FORCED_GL
class TextureRGB555 : public Texture {
public:
	TextureRGB555();
	virtual ~TextureRGB555();

	virtual void allocate(uint width, uint height);

	virtual Graphics::PixelFormat getFormat() const;

	virtual Graphics::Surface *getSurface() { return &_rgb555Data; }
	virtual const Graphics::Surface *getSurface() const { return &_rgb555Data; }

protected:
	virtual void updateTexture();

private:
	Graphics::Surface _rgb555Data;
};
#endif // !USE_FORCED_GL

} // End of namespace OpenGL

#endif
