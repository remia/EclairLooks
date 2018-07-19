#include "ociocolorspace_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

namespace OCIO = OCIO_NAMESPACE;


OCIOColorSpace::OCIOColorSpace()
{
    using Type = ImageOperatorParameter::Type;
    Parameters().Add({ "Config File", Type::FilePath });
    Parameters().Add({ "Source", Type::Select });
    Parameters().Add({ "Destination", Type::Select });
    Parameters().Add({ "Look", Type::Select });
    Parameters().Add({ "Direction", Type::Select, "Forward", std::vector<std::string>{"Forward", "Inverse"} });

    m_config = OCIO::GetCurrentConfig();
    m_processor = OCIO::Processor::Create();
    m_transform = OCIO::LookTransform::Create();
}

std::string OCIOColorSpace::OpName() const
{
    return "OCIO ColorSpace";
}

void OCIOColorSpace::OpApply(Image & img)
{
    try {
        OCIO::PackedImageDesc imgDesc(img.pixels_asfloat(), img.width(), img.height(), img.channels());
        m_processor->apply(imgDesc);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Process Error: " << exception.what() << "\n";
    }
}

bool OCIOColorSpace::OpIsIdentity() const
{
    return m_processor->isNoOp();
}

void OCIOColorSpace::OpUpdateParamCallback(const ImageOperatorParameter & op)
{
    try {
        if (op.name == "Config File")
            SetConfig(GetAny<std::string>(op.value).value());
        else if (op.name == "Source")
            m_transform->setSrc(GetAny<std::string>(op.value).value().c_str());
        else if (op.name == "Destination")
            m_transform->setDst(GetAny<std::string>(op.value).value().c_str());
        else if (op.name == "Look")
            m_transform->setLooks(GetAny<std::string>(op.value).value().c_str());
        else if (op.name == "Direction")
            m_transform->setDirection(OCIO::TransformDirectionFromString(GetAny<std::string>(op.value).value().c_str()));

        m_processor = m_config->getProcessor(m_transform);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}

void OCIOColorSpace::SetConfig(const std::string &configpath)
{
    m_config = OCIO::Config::CreateFromFile(configpath.c_str());
    Parameters().Update({ "Config File", ImageOperatorParameter::Type::FilePath, configpath });

    std::vector<std::string> colorspaces;
    for (int i = 0; i < m_config->getNumColorSpaces(); i++)
        colorspaces.push_back(m_config->getColorSpaceNameByIndex(i));
    Parameters().Update({ "Source", ImageOperatorParameter::Type::Select, "0", colorspaces });
    Parameters().Update({ "Destination", ImageOperatorParameter::Type::Select, "0", colorspaces });

    std::vector<std::string> looks;
    for (int i = 0; i < m_config->getNumLooks(); ++i)
        looks.push_back(m_config->getLookNameByIndex(i));
    Parameters().Update({ "Look", ImageOperatorParameter::Type::Select, "0", looks });
}