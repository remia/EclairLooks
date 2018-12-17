#include "ctl_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <vector>
#include <filesystem>

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

#include "ctl_transform.h"

using namespace std::filesystem;


CTLTransform::CTLTransform()
{
    AddParameterByCategory<FilePathParameter>("CTL", "CTL Base Path");
    AddParameterByCategory<TextParameter>("CTL", "CTL Transform");
}

ImageOperator *CTLTransform::OpCreate() const
{
    return new CTLTransform();
}

std::string CTLTransform::OpName() const
{
    return "CTL Transform";
}

std::string CTLTransform::OpLabel() const
{
    return "CTL";
}

ImageOperator *CTLTransform::OpCreateFromPath(const std::string &filepath) const
{
    QFileInfo file = QFileInfo(QString::fromStdString(filepath));
    if (file.isDir()) {
        CTLTransform * ct = new CTLTransform();
        ct->SetBaseFolder(filepath);
        return ct;
    }

    return nullptr;
}

void CTLTransform::OpApply(Image &img)
{
    std::string basePath = Parameters().Get<FilePathParameter>("CTL Base Path")->value();

    path p = path(basePath);
    directory_iterator it{p};
    while (it != directory_iterator{}) {
        qInfo() << QString::fromStdString((*it).path().string()) << '\n';
        ++it;
    }

    std::vector<std::string> searchs {
        { "/Users/remi/Desktop/color/aces-dev/transforms/ctl/rrt" },
        { "/Users/remi/Desktop/color/aces-dev/transforms/ctl/utilities" },
        { "/Users/remi/Desktop/color/aces-dev/transforms/ctl/lib" }};

    CTLOperations ops;
    ops.push_back({"/Users/remi/Desktop/color/aces-dev/transforms/ctl/rrt/RRT.ctl"});
    ops.push_back({"/Users/remi/Desktop/color/aces-dev/transforms/ctl/odt/rec709/ODT.Academy.Rec709_100nits_dim.ctl"});
    ops.push_back({"/Users/remi/Desktop/color/aces-dev/transforms/ctl/odt/rec709/InvODT.Academy.Rec709_100nits_dim.ctl"});
    ops.push_back({"/Users/remi/Desktop/color/aces-dev/transforms/ctl/rrt/InvRRT.ctl"});

    CTLParameters global;
    ctl_parameter_t alpha_param;
    alpha_param.name = "aIn";
    alpha_param.count = 1;
    alpha_param.value[0] = 1.0f;
    global.push_back(alpha_param);

    try {
        transform(img, 1.0f, 1.0f, ops, global, searchs);
    }
    catch (Iex::ArgExc & e) {
        qWarning() << e.what() << "\n";
    }
    catch (Iex::LogicExc & e) {
        qWarning() << e.what() << "\n";
    }
}

bool CTLTransform::OpIsIdentity() const
{
    return GetParameter<TextParameter>("CTL Transform")->value().empty();
}

void CTLTransform::OpUpdateParamCallback(const Parameter & op)
{

}

void CTLTransform::SetBaseFolder(const std::string &baseFolder)
{
    GetParameter<FilePathParameter>("CTL Base Path")->setValue(baseFolder);
}