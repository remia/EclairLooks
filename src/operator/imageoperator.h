#pragma once

#include <any>
#include <vector>

#include "imageoperatorparameterlist.h"


class Image;

class ImageOperator
{
  public:
    ImageOperator();
    ImageOperator(const ImageOperator &) = default;
    virtual ~ImageOperator() = default;

    virtual std::string OpName() const = 0;
    virtual void OpApply(Image &img) = 0;
    virtual bool OpIsIdentity() const { return true; }
    virtual void OpUpdateParamCallback(const ImageOperatorParameter &op) {}

  public:
    ImageOperatorParameterList & Parameters();
    ImageOperatorParameterList const & Parameters() const;

    bool IsIdentity() const;
    void Apply(Image &img);

  private:
    ImageOperatorParameterList m_paramList;
};