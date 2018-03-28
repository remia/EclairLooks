#pragma once

#include <string>
#include <vector>
#include <cstdint>


enum class PixelType
{
    Uint8,
    Half,
    Float,
    Double
};

enum class PixelFormat
{
    RGB,
    RGBA
};

class Image
{
  public:
    Image();

    uint16_t width() const { return m_width; }
    uint16_t height() const { return m_height; }
    uint8_t channels() const { return m_channels; }
    PixelType type() const { return m_type; }
    PixelFormat format() const { return m_format; }
    uint8_t const *pixels() const { return m_pixels.data(); }

  public:
    static Image FromFile(const std::string &filepath);

  public:
    explicit operator bool() const;

private:
    uint16_t m_width;
    uint16_t m_height;
    uint8_t m_channels;
    PixelType m_type;
    PixelFormat m_format;

    std::vector<uint8_t> m_pixels;
};