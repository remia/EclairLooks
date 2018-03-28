#pragma once

#include <string>

#include <OpenColorIO/OpenColorIO.h>

#include "imagetransformation.h"


class Image;

class TransformOCIO : public ImageTransformation
{
public:
    TransformOCIO();

public:
    void Apply(Image & img) override;
    bool IsIdentity() override;

    void SetConfig(const std::string &configpath);
    void SetFileTransform(const std::string &lutpath);

private:
    OCIO_NAMESPACE::ConstConfigRcPtr m_config;
    OCIO_NAMESPACE::ConstProcessorRcPtr m_processor;
};