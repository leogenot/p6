#pragma once

#include "ImageCommon.h"

namespace p6 {

/* ------------------------------- */
/** \defgroup canvas Canvas
 * Create, modify and query information about canvases.
 * @{*/
/* ------------------------------- */

/// A canvas is an image that can be drawn onto.
class Canvas {
public:
    /// Creates an empty canvas
    /// texture_layout is an advanced setting; it controls how the pixels are gonna be stored on the GPU.
    explicit Canvas(ImageSize           size,
                    glpp::TextureLayout texture_layout = {glpp::InternalFormat::RGBA16, glpp::Channels::RGBA, glpp::TexelDataType::UnsignedByte});

    /// Returns the size in pixels of the canvas.
    ImageSize size() const { return _render_target.size(); }
    /// Returns the aspect ratio of the canvas (`width / height`)
    float aspect_ratio() const { return size().aspect_ratio(); }

    /// Returns the internal texture, that you can use to do advanced stuff with custom shaders.
    const Texture& texture() const { return _render_target.texture(); }

    /// Resizes the canvas.
    void resize(ImageSize size) { _render_target.conservative_resize(size); }
    /// Resizes the canvas.
    /// :warning: All of its pixel content is lost. If you don't want that you can use resize() instead.
    void destructive_resize(ImageSize size) { _render_target.resize(size); }

private:
    friend class Context;
    glpp::RenderTarget _render_target;
};

/**@}*/

} // namespace p6