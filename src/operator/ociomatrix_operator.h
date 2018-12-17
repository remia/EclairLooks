#pragma once

#include <string>

#include <OpenColorIO/OpenColorIO.h>

#include "imageoperator.h"


class Image;

class OCIOMatrix : public ImageOperator
{
public:
    OCIOMatrix();

public:
    ImageOperator * OpCreate() const override;
    std::string OpName() const override;
    std::string OpLabel() const override;
    void OpApply(Image & img) override;
    bool OpIsIdentity() const override;
    void OpUpdateParamCallback(const Parameter & op) override;

    void SetMatrix(float * mat);

private:
    OCIO_NAMESPACE::ConstConfigRcPtr m_config;
    OCIO_NAMESPACE::ConstProcessorRcPtr m_processor;
    OCIO_NAMESPACE::MatrixTransformRcPtr m_transform;
};