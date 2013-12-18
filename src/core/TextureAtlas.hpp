
#pragma once

#include <vector>
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"

namespace core {

/**
 * A texture atlas is used to pack several small regions into a single texture.
 */
class TextureAtlas {
 public:

  /**
   * Create a texture atlas from its width and height.
   */
  TextureAtlas(size_t width, size_t height);

  /**
   * Delete the texture atlas and free all allocated memory.
   */
  ~TextureAtlas();

  /**
   * Return the width of the texture atlas.
   */
  size_t getWidth() const;

  /**
   * Return the height of the texture atlas.
   */
  size_t getHeight() const;

  /**
   * Return the array of pixel generated by the texture atlas.
   */
  unsigned char *getData() const;

  /**
   * Bind the texture for OpenGL.
   */
  void bind();

  /**
   * Clear the atlas.
   */
  void clear();

  /**
   * Place the given region into the atlas.
   */
  void setRegion(const size_t x,
                 const size_t y,
                 const size_t width,
                 const size_t height,
                 const unsigned char * data,
                 const size_t stride);

  /**
   * Determinate a region into the atlas for the given size.
   */
  Vector4i getRegion(const size_t width, const size_t height);

 private:

  int _findBestIndex(Vector4i &region,
                     size_t width,
                     size_t height);

  /**
   * Check if we can fit the given region on the height.
   */
  int _checkFit(const size_t index,
                const size_t width,
                const size_t height) const;

  /**
   * Merge nodes if it is possible.
   */
  void _merge();

  std::vector<Vector3i> _nodes;
  size_t _width;
  size_t _height;
  size_t _used;
  unsigned int _id;

  /**
   * Atlas data
   */
  unsigned char *_data;

};

}  // namespace core
