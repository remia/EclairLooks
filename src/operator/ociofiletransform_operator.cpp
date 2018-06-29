#include "ociofiletransform_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

#include <iostream>

namespace OCIO = OCIO_NAMESPACE;


OCIOFileTransform::OCIOFileTransform()
{
    m_config = OCIO::GetCurrentConfig();
    m_processor = OCIO::Processor::Create();
    m_transform = OCIO::FileTransform::Create();
}

std::string OCIOFileTransform::OpName() const
{
    return "OCIO File Transform";
}

void OCIOFileTransform::OpApply(Image & img)
{
    try {
        OCIO::PackedImageDesc imgDesc(img.pixels_asfloat(), img.width(), img.height(), img.channels());
        m_processor->apply(imgDesc);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Process Error: " << exception.what() << "\n";
    }
}

bool OCIOFileTransform::OpIsIdentity() const
{
    return m_processor->isNoOp();
}

ImageOperatorParameterVec OCIOFileTransform::OpExportParams() const
{
    return
    {
        { ImageOperatorParameter::Type::FilePath, "LUT", "" },
        { ImageOperatorParameter::Type::Select, "Interpolation", std::string("Best"), std::vector<std::string>{"Best", "Nearest", "Linear", "Tetrahedral"} },
        { ImageOperatorParameter::Type::Select, "Direction", std::string("Forward"), std::vector<std::string>{"Forward", "Inverse"} }
    };
}

void OCIOFileTransform::OpUpdateParamCallback(const ImageOperatorParameter & op)
{
    try {
        if (op.name == "Interpolation")
            m_transform->setInterpolation(OCIO::InterpolationFromString(std::any_cast<std::string>(op.value).c_str()));
        else if (op.name == "Direction")
            m_transform->setDirection(OCIO::TransformDirectionFromString(std::any_cast<std::string>(op.value).c_str()));

        m_processor = m_config->getProcessor(m_transform);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}

void OCIOFileTransform::SetFileTransform(const std::string &lutpath)
{
    try {
        UpdateParameter("LUT", { ImageOperatorParameter::Type::FilePath, "LUT", lutpath });
        m_transform->setSrc(lutpath.c_str());
        m_transform->setInterpolation(OCIO::InterpolationFromString(std::any_cast<std::string>(GetParameter("Interpolation")->value).c_str()));
        m_transform->setDirection(OCIO::TransformDirectionFromString(std::any_cast<std::string>(GetParameter("Direction")->value).c_str()));
        m_processor = m_config->getProcessor(m_transform);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}