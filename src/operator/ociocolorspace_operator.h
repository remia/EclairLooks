#pragma once

#include <string>

#include <OpenColorIO/OpenColorIO.h>

#include "imageoperator.h"


class Image;

class OCIOColorSpace : public ImageOperator
{
public:
    OCIOColorSpace();

public:
    std::string OpName() const override;
    void OpApply(Image & img) override;
    bool OpIsIdentity() const override;
    void OpUpdateParamCallback(const ImageOperatorParameter & op) override;

    void SetConfig(const std::string &configpath);

private:
    OCIO_NAMESPACE::ConstConfigRcPtr m_config;
    OCIO_NAMESPACE::ConstProcessorRcPtr m_processor;
    OCIO_NAMESPACE::LookTransformRcPtr m_transform;
};