#pragma once

#include <OpenColorIO/OpenColorIO.h>


template <typename F>
void IsolateColorContrast(Image& img, F f, float color, float contrast)
{
    if (contrast == 1.0f && color == 1.0f) {
        f(img);
        return;
    }

    Image img_apply = img;
    Image img_contrast = img;
    Image img_color = img;
    f(img_apply);

    // Compute 1D LUT, ie. the contrast component of the operator
    uint32_t size = 8192;
    Image img_1d = Image::Ramp1D(size, 0.0f, 1.0f, RampType::NEUTRAL);
    f(img_1d);

    OCIO_NAMESPACE::ConstConfigRcPtr config = OCIO_NAMESPACE::GetCurrentConfig();
    OCIO_NAMESPACE::Lut1DTransformRcPtr transform = OCIO_NAMESPACE::Lut1DTransform::Create();
    transform->setLength(size);

    float *pix = img_1d.pixels_asfloat();
    for (uint64_t i = 0; i < size; ++i)
        transform->setValue(i, pix[i * 3], pix[i * 3 + 1], pix[i * 3 + 2]);

    // Image with contrast only applied
    OCIO_NAMESPACE::PackedImageDesc imgDescContrast(
        img_contrast.pixels_asfloat(),
        img_contrast.width(),
        img_contrast.height(),
        img_contrast.channels());

    OCIO_NAMESPACE::ConstProcessorRcPtr processor = config->getProcessor(transform);
    OCIO_NAMESPACE::ConstCPUProcessorRcPtr cpu_processor;
    cpu_processor = processor->getOptimizedCPUProcessor(OCIO_NAMESPACE::OPTIMIZATION_LOSSLESS);
    cpu_processor->apply(imgDescContrast);

    // Image with color only applied
    OCIO_NAMESPACE::PackedImageDesc imgDescColor(
        img_color.pixels_asfloat(),
        img_color.width(),
        img_color.height(),
        img_color.channels());
    transform->setDirection(OCIO_NAMESPACE::TRANSFORM_DIR_INVERSE);
    processor = config->getProcessor(transform);
    cpu_processor = processor->getOptimizedCPUProcessor(OCIO_NAMESPACE::OPTIMIZATION_LOSSLESS);
    cpu_processor->apply(imgDescColor);
    f(img_color);

    // Mix depending on slider values
    if (contrast == 1.f && color == 1.f) {
        img = img_apply;
    }
    else if (contrast == 0.f && color == 0.f) {
        // Nothing to do
        // img = img;
    }
    else if (contrast == 0.f || color == 0.f) {
        float a = std::max(contrast, color);
        img = img * (1.f - a) + img_contrast * contrast + img_color * color;
    }
    else if (contrast == 1.f || color == 1.f) {
        float a = std::min(contrast, color);
        std::swap(contrast, color);
        img = img_apply * a + img_contrast * (1.f - contrast) + img_color * (1.f - color);
    }
    else {
        const Image *source = nullptr;
        float a_contrast = 0.0f;
        float a_color = 0.0f;

        // Work backward from destination image
        if (contrast + color > 1.f) {
            source = &img_apply;
            a_color = 1.f - contrast;
            a_contrast = 1.f - color;
        }
        // Work forward from source image
        else {
            source = &img;
            a_color = color;
            a_contrast = contrast;
        }

        // qInfo() << "Mix : " << a_source;
        // qInfo() << "\tContrast slider" << contrast;
        // qInfo() << "\tColor slider" << color;
        // qInfo() << "\tSource" << a_source;
        // qInfo() << "\tContrast" << a_contrast;
        // qInfo() << "\tColor" << a_color;
        float a_source = 1.f - (a_color + a_contrast);
        img = *source * a_source + img_contrast * a_contrast + img_color * a_color;
    }
}
