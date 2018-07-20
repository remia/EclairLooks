#include "ociofiletransform_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

namespace OCIO = OCIO_NAMESPACE;


OCIOFileTransform::OCIOFileTransform()
{
    Parameters().Add(FilePathParameter("LUT"));
    Parameters().Add(SelectParameter("Interpolation", {"Best", "Nearest", "Linear", "Tetrahedral"}));
    Parameters().Add(SelectParameter("Direction", {"Forward", "Inverse"}));

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
        if (op.name == "Interpolation") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setInterpolation(OCIO::InterpolationFromString(p->value.c_str()));
        }
        else if (op.name == "Direction") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setDirection(OCIO::TransformDirectionFromString(p->value.c_str()));
        }
        m_processor = m_config->getProcessor(m_transform);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}

void OCIOFileTransform::SetFileTransform(const std::string &lutpath)
{
    try {
        Parameters().Update(FilePathParameter("LUT", lutpath));
        auto interp = Parameters().Get<SelectParameter>("Interpolation");
        auto dir = Parameters().Get<SelectParameter>("Direction");

        m_transform->setSrc(lutpath.c_str());
        m_transform->setInterpolation(OCIO::InterpolationFromString(interp.value.c_str()));
        m_transform->setDirection(OCIO::TransformDirectionFromString(dir.value.c_str()));
        m_processor = m_config->getProcessor(m_transform);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}