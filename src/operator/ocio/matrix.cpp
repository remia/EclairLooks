#include "matrix.h"

#include <sstream>

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

#include <core/image.h>
#include <core/imagepipeline.h>

namespace OCIO = OCIO_NAMESPACE;


OCIOMatrix::OCIOMatrix()
{
    m_config = OCIO::GetCurrentConfig();
    m_transform = OCIO::MatrixTransform::Create();

    AddParameterByCategory<MatrixParameter>("Matrix", "Matrix");
    AddParameterByCategory<SelectParameter>("Matrix", "Direction", std::vector<std::string>{"Forward", "Inverse"}, "Forward");

    // Initialize transform with default parameters
    auto dir = GetParameter<SelectParameter>("Direction");
    m_transform->setDirection(OCIO::TransformDirectionFromString(dir->value().c_str()));
}

ImageOperator * OCIOMatrix::OpCreate() const
{
    return new OCIOMatrix();
}

std::string OCIOMatrix::OpName() const
{
    return "OCIO Matrix";
}

std::string OCIOMatrix::OpLabel() const
{
    return "Matrix";
}

std::string OCIOMatrix::OpDesc() const
{
    std::ostringstream oStr;
    oStr << "OCIO Matrix Transform\n" << *m_transform;
    return oStr.str();
}

void OCIOMatrix::OpApply(Image & img)
{
    try {
        OCIO::PackedImageDesc imgDesc(img.pixels_asfloat(), img.width(), img.height(), img.channels());
        m_cpu_processor->apply(imgDesc);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Process Error: " << exception.what() << "\n";
    }
}

bool OCIOMatrix::OpIsIdentity() const
{
    return !m_processor || m_processor->isNoOp();
}

void OCIOMatrix::OpUpdateParamCallback(const Parameter & op)
{
    try {
        if (op.name() == "Matrix") {
            auto p = static_cast<const MatrixParameter *>(&op);
            m_transform->setMatrix(p->value().data());
        }
        else if (op.name() == "Direction") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setDirection(OCIO::TransformDirectionFromString(p->value().c_str()));
        }

        m_processor = m_config->getProcessor(m_transform);
        m_cpu_processor = m_processor->getOptimizedCPUProcessor(OCIO::OPTIMIZATION_LOSSLESS);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}
