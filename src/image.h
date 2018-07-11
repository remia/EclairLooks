#pragma once

#include <string>
#include <vector>
#include <cstdint>


enum class PixelType
{
    Uint8,
    Uint16,
    Half,
    Float,
    Double
};

enum class PixelFormat
{
    GRAY,
    RGB,
    RGBA
};

enum class RampType
{
    NEUTRAL,
    RED,
    GREEN,
    BLUE
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

    uint64_t count() const { return width() * height(); }

    uint8_t const * pixels() const { return m_pixels.data(); }
    uint8_t * pixels() { return m_pixels.data(); }

    float *pixels_asfloat() { return reinterpret_cast<float *>(m_pixels.data()); }
    float const * pixels_asfloat() const { return reinterpret_cast<float const *>(m_pixels.data()); }

    void save(const std::string & path, PixelType format = PixelType::Uint16) const;

  public:
    static Image FromFile(const std::string &filepath);
    static Image Ramp1D(uint64_t size, float min = 0.0f, float max = 1.0f, RampType t = RampType::NEUTRAL);
    static Image Lattice(uint64_t size, uint32_t maxwidth = 512);

  public:
    explicit operator bool() const;

    Image & operator +(const Image & rhs);

    Image & operator -(const Image & rhs);

    Image & operator *(const Image & rhs);

    Image & operator *(float v);
    Image operator *(float v) const;

    Image & operator /(const Image & rhs);

private:
    uint16_t m_width;
    uint16_t m_height;
    uint8_t m_channels;
    PixelType m_type;
    PixelFormat m_format;

    std::vector<uint8_t> m_pixels;
};
