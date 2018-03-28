#include "transform_ocio.h"
#include "image.h"

#include <QtCore/QDebug>

namespace OCIO = OCIO_NAMESPACE;


TransformOCIO::TransformOCIO()
{
    m_config = OCIO::GetCurrentConfig();
    m_processor = OCIO::Processor::Create();
}

void TransformOCIO::Apply(Image & img)
{
    OCIO::PackedImageDesc imgDesc(img.pixels_asfloat(), img.width(), img.height(), img.channels());
    m_processor->apply(imgDesc);
}

bool TransformOCIO::IsIdentity()
{
    return m_processor->isNoOp();
}

void TransformOCIO::SetConfig(const std::string &configpath)
{

}

void TransformOCIO::SetFileTransform(const std::string &lutpath)
{
    try {
        OCIO::FileTransformRcPtr transform = OCIO::FileTransform::Create();
        transform->setSrc(lutpath.c_str());
        transform->setDirection(OCIO::TRANSFORM_DIR_FORWARD);
        // INTERP_NEAREST works !
        transform->setInterpolation(OCIO::INTERP_BEST);
        m_processor = m_config->getProcessor(transform);
    } catch (OCIO::Exception &exception) {
        qInfo() << "OpenColorIO Error: " << exception.what() << "\n";
    }
}