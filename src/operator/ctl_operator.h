#pragma once

#include <string>


#include "imageoperator.h"


class Image;

class CTLTransform : public ImageOperator
{
public:
    CTLTransform();

public:
    std::string OpName() const override;
    void OpApply(Image & img) override;
    bool OpIsIdentity() const override;
    void OpUpdateParamCallback(const ImageOperatorParameter & op) override;

    void SetBaseFolder(const std::string &baseFolder);

private:
};