#pragma once

#include <string>

#include <OpenColorIO/OpenColorIO.h>

#include "imageoperator.h"


class Image;

class OCIOFileTransform : public ImageOperator
{
public:
    OCIOFileTransform();

public:
    std::string OpName() const override;
    void OpApply(Image & img) override;
    bool OpIsIdentity() const override;
    void OpUpdateParamCallback(const ImageOperatorParameter & op) override;

    void SetFileTransform(const std::string &lutpath);

private:
    OCIO_NAMESPACE::ConstConfigRcPtr m_config;
    OCIO_NAMESPACE::ConstProcessorRcPtr m_processor;
    OCIO_NAMESPACE::FileTransformRcPtr m_transform;
};