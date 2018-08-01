#include "image.h"

#include <QtCore/QDebug>
#include <OpenImageIO/imageio.h>
#include <OpenImageIO/filesystem.h>

using namespace OIIO;


struct BitdepthFormat
{
    TypeDesc type;
    float scale;
};

void PrintImageMetadata(const std::string &filepath, ImageSpec spec)
{
    qInfo() << "File -" << QString::fromStdString(filepath);
    qInfo() << "Dimensions :" << spec.width << "x" << spec.height << "~" << spec.nchannels;
    qInfo() << "Type :" << spec.format.elementsize() * 8 << "bits" << (spec.format.is_floating_point() ? "Float" : "Integer");

    for (size_t i = 0; i < spec.channelnames.size(); ++i)
        qInfo() << "Channel " << i << ":" << QString::fromStdString(spec.channelnames[i]);

    qInfo() << "-----------------";
    for (const ParamValue &p : spec.extra_attribs) {
        QDebug info = qInfo();
        info << p.name().c_str() << "\t";

        if (p.type() == TypeString)
            info << *(const char **)p.data();
        else if (p.type() == TypeFloat)
            info << *(const float *)p.data();
        else if (p.type() == TypeInt)
            info << *(const int *)p.data();
        else if (p.type() == TypeDesc::UINT)
            info << *(const unsigned int *)p.data();
        else if (p.type() == TypeMatrix) {
            const float *f = (const float *)p.data();
            info << f[0] << f[1] << f[2] << f[3] << f[4] << f[5] << f[6] << f[7] << f[8] << f[9] << f[10]
                 << f[11] << f[12] << f[13] << f[14] << f[15];
        }
    }
    qInfo() << "-----------------\n";
}

Image::Image()
    : m_width(0), m_height(0), m_channels(0), m_type(PixelType::Uint8),
      m_format(PixelFormat::RGB)
{
}

void Image::save(const std::string & path, PixelType format) const
{
    std::map<PixelType, BitdepthFormat> BitdepthMap;
    BitdepthMap[PixelType::Uint8] = { TypeDesc::UINT8, 255.0f };
    BitdepthMap[PixelType::Uint16] = { TypeDesc::UINT16, 65535.0f };
    BitdepthMap[PixelType::Half] = { TypeDesc::HALF, 1.0f };
    BitdepthMap[PixelType::Float] = { TypeDesc::FLOAT, 1.0f };

    BitdepthFormat oiio_format = BitdepthMap[format];

    ImageSpec spec;
    spec.width = m_width;
    spec.height = m_height;
    spec.nchannels = m_channels;
    spec.set_format(oiio_format.type);
    spec.default_channel_names();

    std::unique_ptr<ImageOutput> out(ImageOutput::create(path));
    if (!out)
        return;
    out->open(path, spec);
    out->write_image(TypeDesc::FLOAT, m_pixels.data());
    out->close();
}

Image Image::FromFile(const std::string &filepath)
{
    int threads;
    OIIO::attribute("threads", 4);
    OIIO::getattribute("threads", threads);
    qInfo() << "OIIO uses " << threads << " threads\n";

    ImageInput *in = ImageInput::open(filepath);
    if (!in) {
        qWarning() << "Could not open image !";
        return Image();
    }

    const ImageSpec &spec = in->spec();
    TypeDesc pixel_type = TypeDesc::FLOAT;
    uint8_t pixel_depth = 4;

    Image res;
    res.m_width = spec.width;
    res.m_height = spec.height;
    res.m_channels = spec.nchannels;
    res.m_type = PixelType::Float;

    if (res.m_channels == 1)
        res.m_format = PixelFormat::GRAY;
    if (res.m_channels == 3)
        res.m_format = PixelFormat::RGB;
    else if (res.m_channels == 4)
        res.m_format = PixelFormat::RGBA;

    res.m_pixels = std::vector<uint8_t>(res.m_width * res.m_height * res.m_channels * pixel_depth);

    in->read_image(pixel_type, res.m_pixels.data());
    in->close();

    PrintImageMetadata(filepath, spec);

    ImageInput::destroy(in);
    return res;
}

// Image Image::FromBuffer(void * buffer, size_t size)
// {
//     ImageSpec config;
//     Filesystem::IOMemReader memreader(buffer, size);
//     void *ptr = &memreader;
//     config.attribute("oiio:ioproxy", TypeDesc::PTR, &ptr);
//
//     ImageSpec spec;
//     ImageInput *in = ImageInput::open("in.png", spec, &config);
//     TypeDesc pixel_type = TypeDesc::FLOAT;
//
//     Image res;
//     res.m_width = spec.width;
//     res.m_height = spec.height;
//     res.m_channels = spec.nchannels;
//     res.m_type = PixelType::Float;
//
//     if (res.m_channels == 1)
//         res.m_format = PixelFormat::GRAY;
//     if (res.m_channels == 3)
//         res.m_format = PixelFormat::RGB;
//     else if (res.m_channels == 4)
//         res.m_format = PixelFormat::RGBA;
//
//     res.m_pixels = std::vector<uint8_t>(res.m_width * res.m_height * res.m_channels * pixel_depth);
//
//     in->read_image(pixel_type, res.m_pixels.data());
//     in->close();
//
//     ImageInput::destroy(in);
//     return res;
// }

Image Image::Ramp1D(uint64_t size, float min, float max, RampType t)
{
    uint8_t pixel_depth = 4;

    Image res;
    res.m_width = size;
    res.m_height = 1;
    res.m_channels = 3;
    res.m_type = PixelType::Float;
    res.m_pixels = std::vector<uint8_t>(res.m_width * res.m_height * res.m_channels * pixel_depth);
    float * a = reinterpret_cast<float *>(res.m_pixels.data());

    for (uint64_t i = 0; i < res.m_width; ++i) {
        if (t == RampType::NEUTRAL) {
            a[i * 3] = 1.0f * i / (res.m_width - 1);
            a[i * 3 + 1] = 1.0f * i / (res.m_width - 1);
            a[i * 3 + 2] = 1.0f * i / (res.m_width - 1);
        } else if (t == RampType::RED) {
            a[i * 3] = 1.0f * i / (res.m_width - 1);
            a[i * 3 + 1] = 0.0f;
            a[i * 3 + 2] = 0.0f;
        } else if (t == RampType::GREEN) {
            a[i * 3] = 0.0f;
            a[i * 3 + 1] = 1.0f * i / (res.m_width - 1);
            a[i * 3 + 2] = 0.0f;
        } else if (t == RampType::BLUE) {
            a[i * 3] = 0.0f;
            a[i * 3 + 1] = 0.0f;
            a[i * 3 + 2] = 1.0f * i / (res.m_width - 1);
        }
    }

    return res;
}

Image Image::Lattice(uint64_t size, uint32_t maxwidth)
{
    uint8_t pixel_depth = 4;
    uint64_t elem_count = size * size * size;
    uint32_t width = elem_count > maxwidth ? maxwidth : elem_count;
    uint32_t height = std::ceil(1.f * elem_count / width);
    qInfo() << "Lattice image for size" << size << ":" << width << "x" << height;

    Image res;
    res.m_width = width;
    res.m_height = height;
    res.m_channels = 3;
    res.m_type = PixelType::Float;
    res.m_pixels = std::vector<uint8_t>(res.m_width * res.m_height * res.m_channels * pixel_depth);

    uint32_t i = 0;
    float * pix = reinterpret_cast<float *>(res.m_pixels.data());
    for (uint32_t b = 0; b < size; ++b) {
        float bnorm = 1.0f * b / (size - 1);
        for (uint32_t g = 0; g < size; ++g) {
            float gnorm = 1.0f * g / (size - 1);
            for (uint32_t r = 0; r < size; ++r) {
                float rnorm = 1.0f * r / (size - 1);
                pix[i * 3] = rnorm;
                pix[i * 3 + 1] = gnorm;
                pix[i * 3 + 2] = bnorm;
                i++;
            }
        }
    }

    return res;
}

Image::operator bool() const { return (m_width != 0 && m_height != 0); }

Image & Image::operator +(const Image & rhs)
{
    float * a = pixels_asfloat();
    float const * b = rhs.pixels_asfloat();

    for (uint64_t i = 0; i < count() * channels(); ++i) {
        a[i] = a[i] + b[i];
    }

    return *this;
}

Image & Image::operator -(const Image & rhs)
{
    float * a = pixels_asfloat();
    float const * b = rhs.pixels_asfloat();

    for (uint64_t i = 0; i < count() * channels(); ++i) {
        a[i] = a[i] - b[i];
    }

    return *this;
}

Image & Image::operator *(const Image & rhs)
{
    float * a = pixels_asfloat();
    float const * b = rhs.pixels_asfloat();

    for (uint64_t i = 0; i < count() * channels(); ++i) {
        a[i] = a[i] * b[i];
    }

    return *this;
}

Image & Image::operator *(float v)
{
    float * a = pixels_asfloat();

    for (uint64_t i = 0; i < count() * channels(); ++i) {
        a[i] = a[i] * v;
    }

    return *this;
}

Image Image::operator *(float v) const
{
    Image res = *this;
    float * a = res.pixels_asfloat();

    for (uint64_t i = 0; i < count() * channels(); ++i) {
        a[i] = a[i] * v;
    }

    return res;
}

Image & Image::operator /(const Image & rhs)
{
    float * a = pixels_asfloat();
    float const * b = rhs.pixels_asfloat();

    for (uint64_t i = 0; i < count() * channels(); ++i) {
        a[i] = a[i] / b[i];
    }

    return *this;
}
