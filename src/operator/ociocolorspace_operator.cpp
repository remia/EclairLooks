#include "ociocolorspace_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <sstream>

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

namespace OCIO = OCIO_NAMESPACE;


OCIOColorSpace::OCIOColorSpace()
{
    AddParameterByCategory<FilePathParameter>("ColorSpace Transform", "Config File", "", "Choose an OpenColorIO config", "OCIO Config (*.ocio)");
    AddParameterByCategory<SelectParameter>("ColorSpace Transform", "Source");
    AddParameterByCategory<SelectParameter>("ColorSpace Transform", "Destination");
    AddParameterByCategory<SelectParameter>("ColorSpace Transform", "Look");
    AddParameterByCategory<SelectParameter>("ColorSpace Transform", "Direction", std::vector<std::string>{"Forward", "Inverse"});

    m_config = OCIO::GetCurrentConfig();
    m_processor = OCIO::Processor::Create();
    m_transform = OCIO::LookTransform::Create();
}

ImageOperator *OCIOColorSpace::OpCreate() const
{
    return new OCIOColorSpace();
}

ImageOperator *OCIOColorSpace::OpCreateFromPath(const std::string &filepath) const
{
    QFileInfo file = QFileInfo(QString::fromStdString(filepath));
    if (file.completeSuffix() == "ocio") {
        OCIOColorSpace * ct = new OCIOColorSpace();
        ct->SetConfig(filepath);
        return ct;
    }

    return nullptr;
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

void OCIOColorSpace::OpUpdateParamCallback(const Parameter & op)
{
    try {
        if (op.name() == "Config File") {
            auto p = static_cast<const FilePathParameter *>(&op);
            SetConfig(p->value());
        }
        else if (op.name() == "Source") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setSrc(p->value().c_str());
        }
        else if (op.name() == "Destination") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setDst(p->value().c_str());
        }
        else if (op.name() == "Look") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setLooks(p->value().c_str());
        }
        else if (op.name() == "Direction") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setDirection(OCIO::TransformDirectionFromString(p->value().c_str()));
        }

        m_processor = m_config->getProcessor(m_transform);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}

void OCIOColorSpace::SetConfig(const std::string &configpath)
{
    {
        auto m = EventMute(this, UpdateParam);
        m_config = OCIO::Config::CreateFromFile(configpath.c_str());
        GetParameter<FilePathParameter>("Config File")->setValue(configpath);
    }

    auto showDesc = [](auto v){
        std::ostringstream oStr;
        oStr  << *v;
        return oStr.str();
     };

    std::vector<std::string> colorspaces;
    std::vector<std::string> colorspacesDescs;
    for (int i = 0; i < m_config->getNumColorSpaces(); i++) {
        auto colorspaceName = m_config->getColorSpaceNameByIndex(i);
        auto colorspace = m_config->getColorSpace(colorspaceName);

        colorspaces.push_back(colorspaceName);
        colorspacesDescs.push_back(showDesc(colorspace));
    }
    if (colorspaces.size() > 0) {
        GetParameter<SelectParameter>("Source")->setChoices(colorspaces, colorspacesDescs);
        GetParameter<SelectParameter>("Source")->setValue(colorspaces[0]);
        GetParameter<SelectParameter>("Destination")->setChoices(colorspaces, colorspacesDescs);
        GetParameter<SelectParameter>("Destination")->setValue(colorspaces[0]);
    }

    std::vector<std::string> looks;
    std::vector<std::string> looksDescs;
    looks.push_back("");
    looksDescs.push_back("");
    for (int i = 0; i < m_config->getNumLooks(); ++i) {
        auto lookName = m_config->getLookNameByIndex(i);
        auto look = m_config->getLook(lookName);

        looks.push_back(lookName);
        looksDescs.push_back(showDesc(look));
    }
    if (looks.size() > 0) {
        GetParameter<SelectParameter>("Look")->setChoices(looks, looksDescs);
        GetParameter<SelectParameter>("Look")->setValue(looks[0]);
    }
}