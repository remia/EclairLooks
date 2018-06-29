#include "ctl_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <vector>
#include <iostream>

#include <boost/filesystem.hpp>

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

#include "ctl_transform.h"

using namespace boost::filesystem;


CTLTransform::CTLTransform()
{

}

std::string CTLTransform::OpName() const
{
    return "CTL Transform";
}

void CTLTransform::OpApply(Image &img)
{
    std::string basePath = std::any_cast<std::string>(GetParameter("CTL Base Path")->value);

    path p = path(basePath);
    directory_iterator it{p};
    while (it != directory_iterator{})
        std::cout << *it++ << '\n';

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
    catch (Iex_2_2::ArgExc & e) {
        qWarning() << e.what() << "\n";
    }
    catch (Iex_2_2::LogicExc & e) {
        qWarning() << e.what() << "\n";
    }
}

bool CTLTransform::OpIsIdentity() const
{
    return std::any_cast<std::string>(GetParameter("CTL Transform")->value).empty();
}

ImageOperatorParameterVec CTLTransform::OpExportParams() const
{
    return
    {
        { ImageOperatorParameter::Type::FilePath, "CTL Base Path" },
        { ImageOperatorParameter::Type::Text, "CTL Transform" },
    };
}

void CTLTransform::OpUpdateParamCallback(const ImageOperatorParameter & op)
{

}

void CTLTransform::SetBaseFolder(const std::string &baseFolder)
{
    UpdateParameter("CTL Base Path", { ImageOperatorParameter::Type::FilePath, "CTL Base Path", baseFolder });
}