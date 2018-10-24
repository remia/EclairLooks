#pragma once

#include "utils/generic.h"


enum class PixelType
{
    Uint8,
    Uint16,
    Half,
    Float,
    Double,
    Unknown
};

enum class PixelFormat
{
    GRAY,
    RGB,
    RGBA,
    Unknown
};

enum class RampType
{
    NEUTRAL,
    RED,
    GREEN,
    BLUE
};

namespace OpenImageIO_v1_9 { class ImageBuf; }

class Image
{
  public:
    Image();
    Image(const Image &src);
    Image(Image &&src);
    Image & operator=(const Image &src);
    Image & operator=(Image &&src);
    ~Image();

  public:
    uint16_t width() const;
    uint16_t height() const;
    uint8_t channels() const;
    PixelType type() const;
    PixelFormat format() const;

    uint64_t count() const;

    uint8_t const *pixels() const;
    uint8_t *pixels();

    float *pixels_asfloat();
    float const *pixels_asfloat() const;

  public:
    Image to_type(PixelType type) const;

    Image resize(uint16_t width, uint16_t height, bool keepAspectRatio = true) const;

    bool read(const std::string &path);
    bool write(const std::string &path, PixelType type = PixelType::Uint16) const;

  public:
    static Image FromFile(const std::string &path);
    static Image FromBuffer(void *buffer, size_t size);
    static Image Ramp1D(uint16_t size, float min = 0.f, float max = 1.f, RampType t = RampType::NEUTRAL);
    static Image Lattice(uint16_t size, uint16_t maxwidth = 512);

  public:
    explicit operator bool() const;

    Image operator+(const Image &rhs);

    Image operator-(const Image &rhs);

    Image operator*(const Image &rhs);

    Image operator*(float v) const;

    Image operator/(const Image &rhs);

  private:
    UPtr<OpenImageIO_v1_9::ImageBuf> m_imgBuf;
};