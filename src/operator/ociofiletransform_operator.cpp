#include "ociofiletransform_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

namespace OCIO = OCIO_NAMESPACE;


OCIOFileTransform::OCIOFileTransform()
{
    using Type = ImageOperatorParameter::Type;
    using SV = std::vector<std::string>;
    using S = std::string;
    Parameters().Add({ "LUT", Type::FilePath, "" });
    Parameters().Add({ "Interpolation", Type::Select, S("Best"), SV{"Best", "Nearest", "Linear", "Tetrahedral"} });
    Parameters().Add({ "Direction", Type::Select, S("Forward"), SV{"Forward", "Inverse"} });

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

void OCIOFileTransform::OpUpdateParamCallback(const ImageOperatorParameter & op)
{
    try {
        if (op.name == "Interpolation")
            m_transform->setInterpolation(OCIO::InterpolationFromString(GetAny<std::string>(op.value).value().c_str()));
        else if (op.name == "Direction")
            m_transform->setDirection(OCIO::TransformDirectionFromString(GetAny<std::string>(op.value).value().c_str()));
        m_processor = m_config->getProcessor(m_transform);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}

void OCIOFileTransform::SetFileTransform(const std::string &lutpath)
{
    try {
        Parameters().Update({ "LUT", ImageOperatorParameter::Type::FilePath, lutpath });
        auto interp = Parameters().Get<std::string>("Interpolation").value();
        auto dir = Parameters().Get<std::string>("Direction").value();

        m_transform->setSrc(lutpath.c_str());
        m_transform->setInterpolation(OCIO::InterpolationFromString(interp.c_str()));
        m_transform->setDirection(OCIO::TransformDirectionFromString(dir.c_str()));
        m_processor = m_config->getProcessor(m_transform);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}