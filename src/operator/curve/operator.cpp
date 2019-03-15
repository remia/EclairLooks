#include "operator.h"

#include <fstream>

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

#include <core/image.h>
#include <core/imagepipeline.h>
#include <utils/chrono.h>
#include "curve.h"
#include "parameter.h"


CurveOperator::CurveOperator()
{
    CurveEditParameter *yrgbEdit = AddParameterByCategory<CurveEditParameter>("YRGB", "YRGB");
    yrgbEdit->setDisplayName("");
    yrgbEdit->setBackgroundColor(ColorRGBA8{ 200, 200, 200, 255 });
    yrgbEdit->addCurve(new CurveParameter("Luminance", "Y", {25, 25, 25}));
    yrgbEdit->addCurve(new CurveParameter("Red", "R", {200, 25, 25}));
    yrgbEdit->addCurve(new CurveParameter("Green", "G", {25, 200, 25}));
    yrgbEdit->addCurve(new CurveParameter("Blue", "B", {25, 25, 200}));

    CurveEditParameter *hvhEdit = AddParameterByCategory<CurveEditParameter>("Hue v. Hue", "HvH");
    hvhEdit->setDisplayName("");
    hvhEdit->setBackgroundColorCb([](float x, float y) -> ColorRGBA8 {
            QColor c = QColor::fromHsvF(x, 0.75, y);
            return { (uint8_t) c.red(), (uint8_t) c.green(), (uint8_t) c.blue() };
    });
    hvhEdit->addCurve(new CurveParameter("Hue", "H", {200, 200, 200}));
}

ImageOperator *CurveOperator::OpCreate() const
{
    return new CurveOperator();
}

std::string CurveOperator::OpName() const
{
    return "Curve";
}

std::string CurveOperator::OpLabel() const
{
    return OpName();
}

void CurveOperator::OpApply(Image & img)
{
    m_ftRGB.OpApply(img);
}

bool CurveOperator::OpIsIdentity() const
{
    return false;
}

void CurveOperator::OpUpdateParamCallback(const Parameter & op)
{
    qInfo() << "Update callback...";

    Chrono ch;
    ch.start();

    if (op.name() == "YRGB") {
        const CurveEditParameter &cp = static_cast<const CurveEditParameter&>(op);

        int lut_1d_size = 33;
        auto points_y = cp.curveLookupTable("Y", lut_1d_size);
        auto points_r = cp.curveLookupTable("R", lut_1d_size);
        auto points_g = cp.curveLookupTable("G", lut_1d_size);
        auto points_b = cp.curveLookupTable("B", lut_1d_size);

        float ar = cp.curveByName("R")->opacity() / 100.;
        float ag = cp.curveByName("G")->opacity() / 100.;
        float ab = cp.curveByName("B")->opacity() / 100.;

        // // Build a LUT from R,G,B curve
        // std::string lut_name = "test.spi1d";
        // std::ofstream fs(lut_name);
        // fs << "Version 1\n";
        // fs << "From 0.000000 1.000000\n";
        // fs << "Length " << lut_1d_size << "\n";
        // fs << "Components 3\n";
        // fs << "{\n";

        // for (uint64_t i = 0; i < lut_1d_size; ++i) {
        //     float in = i / (lut_1d_size - 1.0);

        //     fs << (1 - ar) * in + ar * points_r[i].y << "\t"
        //        << (1 - ab) * in + ag * points_g[i].y << "\t"
        //        << (1 - ab) * in + ab * points_b[i].y << "\n";
        // }
        // fs << "}\n";
        // fs.close();

        // m_ftRGB.SetFileTransform(lut_name);

        // Build a single LUT from R,G,B
        std::string lut_name = "test.cube";
        std::ofstream ofs(lut_name);
        uint16_t size = 33;
        ofs << "LUT_3D_SIZE " << size << "\n";
        ofs << "\n";

        uint32_t i = 0;
        for (uint32_t b = 0; b < size; ++b)
            for (uint32_t g = 0; g < size; ++g)
                for (uint32_t r = 0; r < size; ++r) {
                    float rn = r / (size - 1.0);
                    float gn = g / (size - 1.0);
                    float bn = b / (size - 1.0);

                    // float luma = rn * 0.2126 + gn * 0.7152 + bn * 0.0722;
                    // if (luma > 0) {
                    //     float gain = points_y[luma * (lut_1d_size - 1)].y / luma;

                    //     rn *= gain;
                    //     bn *= gain;
                    //     gn *= gain;
                    // }

                    float rr = points_r[rn * (lut_1d_size - 1)];
                    float gg = points_g[gn * (lut_1d_size - 1)];
                    float bb = points_b[bn * (lut_1d_size - 1)];
                    ofs << std::setprecision(6) << std::fixed << rr << " " << gg << " " << bb << "\n";
                }
        ofs.close();

        m_ftRGB.SetFileTransform(lut_name);
    }


    // Set opacity corresponding to slider value
    // Can't set opacity individually on R,G,B with a single LUT...

    qInfo() << "CurveOperator update - " << fixed << qSetRealNumberPrecision(2)
            << ch.ellapsed(Chrono::MILLISECONDS) / 1000.f << "sec.\n";
}
