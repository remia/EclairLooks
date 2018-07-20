#include "ociomatrix_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

namespace OCIO = OCIO_NAMESPACE;


OCIOMatrix::OCIOMatrix()
{
    AddParameter(Matrix4x4Parameter("Matrix"));

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

void OCIOMatrix::OpUpdateParamCallback(const ImageOperatorParameter & op)
{
    try {
        if (op.name == "Matrix") {
            auto p = static_cast<const Matrix4x4Parameter *>(&op);
            m_transform->setMatrix(p->value);
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