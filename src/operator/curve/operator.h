#pragma once

#include <string>
#include <vector>

#include "../imageoperator.h"
#include "../ocio/filetransform.h"


class Image;

class CurveOperator : public ImageOperator
{
  public:
    CurveOperator();

  public:
    ImageOperator *OpCreate() const override;
    std::string OpName() const override;
    std::string OpLabel() const override;
    void OpApply(Image &img) override;
    bool OpIsIdentity() const override;
    void OpUpdateParamCallback(const Parameter &op) override;

  private:
    OCIOFileTransform m_ftRGB;
};