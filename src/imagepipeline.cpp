#include "imagepipeline.h"
#include "utils/generic.h"
#include "utils/chrono.h"

#include <fstream>
#include <iomanip>

#include <QtCore/QDebug>


ImagePipeline::ImagePipeline()
{

}

void ImagePipeline::SetInput(const Image & img)
{
    m_inputImg = img;
    m_outputImg = img;

    EmitEvent<Evt::Reset>(m_inputImg);

    Compute();
}

Image & ImagePipeline::GetOutput()
{
    return m_outputImg;
}

uint8_t ImagePipeline::OperatorCount() const
{
    return m_operators.size();
}

ImageOperator &ImagePipeline::GetOperator(uint8_t index)
{
    return *m_operators[index];
}

bool ImagePipeline::DeleteOperator(uint8_t index)
{
    if (index >= m_operators.size()) {
        qWarning() << "Cannot remove operator at index" << index;
        return false;
    }

    m_operators.erase(m_operators.begin() + index);
    Compute();

    return true;
}

void ImagePipeline::Compute()
{
    Chrono c;
    c.start();

    m_outputImg = m_inputImg;
    for (auto & t : m_operators)
        if (!t->IsIdentity())
            t->Apply(m_outputImg);

    qInfo() << "Compute Pipeline in : " << fixed << qSetRealNumberPrecision(2)
            << c.ellapsed(Chrono::MILLISECONDS) / 1000.f << "sec.\n";

    EmitEvent<Evt::Update>(m_outputImg);
}

void ImagePipeline::ExportLUT(const std::string & filename, uint32_t size)
{
    // Lattice image
    Image lattice = Image::Lattice(size);

    // Run pipeline
    for (auto & t : m_operators)
        if (!t->IsIdentity())
            t->Apply(lattice);

    // Extract lattice image to lut
    std::ofstream ofs(filename);
    ofs << "LUT_3D_SIZE " << size << "\n";
    ofs << "DOMAIN_MIN 0.000000 0.000000 0.000000\n";
    ofs << "DOMAIN_MAX 1.000000 1.000000 1.000000\n";
    ofs << "\n";

    uint32_t i = 0;
    float * pix = lattice.pixels_asfloat();
    for (uint32_t r = 0; r < size; ++r)
        for (uint32_t g = 0; g < size; ++g)
            for (uint32_t b = 0; b < size; ++b) {
                ofs << std::setprecision(6) << std::fixed << pix[i * 3] << " " << pix[i * 3 + 1] << " " << pix[i * 3 + 2] << "\n";
                i++;
            }
}
