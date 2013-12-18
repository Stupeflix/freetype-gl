
#include <cstring>
#include <climits>
#include <stdexcept>
#include "ft/gl.hpp"
#include "core/TextureAtlas.hpp"

namespace core {

TextureAtlas::TextureAtlas(const size_t width, const size_t height) :
  _width(width),
  _height(height),
  _used(0),
  _id(0),
  _data(new unsigned char[width*height]) {
  Vector3i node = {1,1,width-2};
  _nodes.push_back(node);
}

TextureAtlas::~TextureAtlas() {
  if (_data)
    delete _data;
  if (_id)
    glDeleteTextures(1, &_id);
}

size_t TextureAtlas::getWidth() const {
  return _width;
}

size_t TextureAtlas::getHeight() const {
  return _height;
}

unsigned char *TextureAtlas::getData() const {
  return _data;
}

void TextureAtlas::bind() {
  if (_id == 0)
    glGenTextures(1, &_id );
  glBindTexture(GL_TEXTURE_2D, _id );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, _width, _height,
                0, GL_ALPHA, GL_UNSIGNED_BYTE, _data);
}

void TextureAtlas::clear() {
  _nodes.clear();
  _used = 0;

  Vector3i node;
  node.x = 1;
  node.y = 1;
  node.z = _width - 2;
  _nodes.push_back(node);
  std::memset(_data, 0, _width * _height);
}

void TextureAtlas::setRegion(const size_t x,
                             const size_t y,
                             const size_t width,
                             const size_t height,
                             const unsigned char * data,
                             const size_t stride) {
  if (x <= 0 || y <= 0 ||
      x >= _width - 1 ||
      x + width > _width - 1 ||
      y >= _height - 1 ||
      y + height > _height - 1)
    throw std::out_of_range("Cannot add the given region into this atlas.");
  for (size_t i = 0; i < height; ++i)
    std::memcpy(_data + ((y + i) * _width + x), data + (i*stride), width);
}

Vector4i TextureAtlas::getRegion(const size_t width,
                              const size_t height) {

  /* Find best index */
  Vector4i region = {{0,0,width,height}};
  int best_index = _findBestIndex(region, width, height);
  if (best_index == -1)
    return region;

  /* Add a node */
  Vector3i newNode;
  newNode.x = region.x;
  newNode.y = region.y + height;
  newNode.z = width;
  _nodes.insert(_nodes.begin() + best_index, newNode);

  /* Reorders the nodes */
  for(size_t i = best_index + 1; i < _nodes.size(); ++i) {
    Vector3i &node = _nodes[i];
    Vector3i &prev = _nodes[i - 1];
    if (node.x < prev.x + prev.z) {
      int k = prev.x + prev.z - node.x;
      node.x += k;
      node.z -= k;
      if (node.z <= 0) {
        _nodes.erase(_nodes.begin() + i);
        --i;
      }
      else
        break;
    }
    else
      break;
  }
  _merge();
  _used += width * height;
  return region;
}

int TextureAtlas::_findBestIndex(Vector4i &region,
                                 size_t width,
                                 size_t height) {
  Vector3i node;
  int best_index = -1;
  int best_width = INT_MAX;
  int best_height = INT_MAX;
  region.x = -1;
  region.y = -1;
  region.width = 0;
  region.height = 0;
  for (size_t i = 0; i < _nodes.size(); ++i) {
    int y = _checkFit( i, width, height );
    if (y >= 0) {
      node = _nodes[i];
      if (y + height < best_height ||
         (y + height == best_height && node.z < best_width)) {
        best_height = y + height;
        best_index = i;
        best_width = node.z;
        region.x = node.x;
        region.y = y;
      }
    }
  }
  return best_index;
}

int TextureAtlas::_checkFit(const size_t index,
                            const size_t width,
                            const size_t height) const {
  Vector3i node;
  int x, y, width_left;
  size_t i;
  node = _nodes[index];
  x = node.x;
  y = node.y;
  width_left = width;
  i = index;
  if (x + width > _width - 1)
    return -1;
  y = node.y;
  while (width_left > 0) {
    node = _nodes[i];
    if (node.y > y)
      y = node.y;
    if (y + height > _height - 1)
      return -1;
    width_left -= node.z;
    ++i;
  }
  return y;
}

void TextureAtlas::_merge() {
  for(size_t i = 0; i < _nodes.size() - 1; ++i) {
    Vector3i &node = _nodes[i];
    Vector3i &next = _nodes[i + 1];
    if (node.y == next.y) {
      node.z += next.z;
      _nodes.erase(_nodes.begin() + i + 1);
      --i;
    }
  }
}

}  // namespace core