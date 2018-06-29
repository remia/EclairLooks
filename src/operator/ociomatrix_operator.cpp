#include "ociomatrix_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

namespace OCIO = OCIO_NAMESPACE;


OCIOMatrix::OCIOMatrix()
{
    m_config = OCIO::GetCurrentConfig();
    m_processor = OCIO::Processor::Create();
    m_transform = OCIO::MatrixTransform::Create();
}

std::string OCIOMatrix::OpName() const
{
    return "OCIO Matrix";
}

void OCIOMatrix::OpApply(Image & img)
{
    try {
        OCIO::PackedImageDesc imgDesc(img.pixels_asfloat(), img.width(), img.height(), img.channels());
        m_processor->apply(imgDesc);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Process Error: " << exception.what() << "\n";
    }
}

bool OCIOMatrix::OpIsIdentity() const
{
    return m_processor->isNoOp();
}

ImageOperatorParameterVec OCIOMatrix::OpExportParams() const
{
    return
    {
        { ImageOperatorParameter::Type::Matrix4x4, "Matrix", 0, std::vector<float> {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        } },
    };
}

void OCIOMatrix::OpUpdateParamCallback(const ImageOperatorParameter & op)
{
    try {
        if (op.name == "Matrix") {
            auto m = std::any_cast<std::vector<float>>(op.values);
            m_transform->setMatrix(m.data());
        }

        m_processor = m_config->getProcessor(m_transform);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}

void OCIOMatrix::SetMatrix(float * mat)
{
    m_transform->setMatrix(mat);
    m_processor = m_config->getProcessor(m_transform);
}