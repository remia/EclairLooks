#include "parameter.h"
#include "widget.h"


// ----------------------------------------------------------------------------

CurveParameter::CurveParameter(const std::string &name, const std::string &sname, const ColorRGBA8 &color)
: Parameter(name), m_short_name(sname), m_color(color)
{
    m_knots = KnotsV {
        { 0.0, 0.0, 45.0, 1.0, 1.0 },
        { 1.0, 1.0, 45.0, 1.0, 1.0 },
    };

    m_opacity.reset(new SliderParameter("Opacity", 100., 0., 100., 1.,
                                        Scale::Linear,
                                        SliderParameter::Legend::None));
    m_opacity->Subscribe<Parameter::UpdateValue>([this](const Parameter &p){
        EmitEvent<UpdateValue>(*this);
    });
}

std::string CurveParameter::shortName() const
{
    return m_short_name;
}

void CurveParameter::setShortName(const std::string &v)
{
    m_short_name = v;
    EmitEvent<UpdateSpecification>(*this);
}

ColorRGBA8 CurveParameter::color() const
{
    return m_color;
}

void CurveParameter::setColor(const ColorRGBA8 &v)
{
    m_color = v;
    EmitEvent<UpdateSpecification>(*this);
}

CurveParameter::Type CurveParameter::type() const
{
    return m_type;
}

void CurveParameter::setType(const Type &v)
{
    m_type = v;
    EmitEvent<UpdateSpecification>(*this);
}

SliderParameter* CurveParameter::opacityParameter()
{
    return m_opacity.get();
}

float CurveParameter::opacity() const
{
    return m_opacity->value();
}

void CurveParameter::setOpacity(const float &v)
{
    m_opacity->setValue(v);
}

KnotsV CurveParameter::knots() const
{
    return m_knots;
}

void CurveParameter::setKnots(const KnotsV &k)
{
    m_knots = k;
    EmitEvent<UpdateSpecification>(*this);
}

ParameterWidget *CurveParameter::newWidget(QWidget * parent)
{
    return nullptr;
}

void CurveParameter::load(const QSettings *setting)
{

}

void CurveParameter::save(QSettings *setting) const
{

}

// ----------------------------------------------------------------------------

CurveEditParameter::CurveEditParameter(const std::string &name, CurveInterpolation ci, CoordinateSystem cs)
    : Parameter(name), m_coordinate_system(cs),  m_interpolation_method(ci)
{
}

CurveEditParameter::CoordinateSystem CurveEditParameter::coordinateSystem() const
{
    return m_coordinate_system;
}

void CurveEditParameter::setCoordinateSystem(const CoordinateSystem &v)
{
    m_coordinate_system = v;
    // EmitEvent<UpdateSpecification>(*this);
}

CurveEditParameter::CurveInterpolation CurveEditParameter::interpolation() const
{
    return m_interpolation_method;
}

void CurveEditParameter::setInterpolation(const CurveInterpolation &v)
{
    m_interpolation_method = v;
    // EmitEvent<UpdateSpecification>(*this);
}

ColorRGBA8 CurveEditParameter::backgroundColor() const
{
    return m_background_color;
}

void CurveEditParameter::setBackgroundColor(const ColorRGBA8 &v)
{
    m_background_color = v;
    // EmitEvent<UpdateSpecification>(*this);
}

OptT<ColorCallback> CurveEditParameter::backgroundColorCb() const
{
    return m_background_color_cb;
}

void CurveEditParameter::setBackgroundColorCb(const ColorCallback &v)
{
    m_background_color_cb = v;
    // EmitEvent<UpdateSpecification>(*this);
}

void CurveEditParameter::addCurve(CurveParameter *c)
{
    c->Subscribe<Parameter::UpdateValue>([this](const Parameter &p){
        EmitEvent<UpdateValue>(*this);
    });

    m_curves.push_back(UPtr<CurveParameter>(c));
    // EmitEvent<UpdateSpecification>(*this);
}

const UPtrV<CurveParameter>& CurveEditParameter::curves() const
{
    return m_curves;
}

const CurveParameter *CurveEditParameter::curveByIndex(int index) const
{
    if (index < 0 || index >= m_curves.size())
        return nullptr;

    return m_curves[index].get();
}

const CurveParameter *CurveEditParameter::curveByName(const std::string &name) const
{
    for (int i = 0; i < m_curves.size(); ++i) {
        if (m_curves[i]->name() == name || m_curves[i]->shortName() == name)
            return m_curves[i].get();
    }

    return nullptr;
}

CurveEditParameter::LookupTable CurveEditParameter::curveLookupTable(int index, int size) const
{
    const CurveParameter *c = curveByIndex(index);
    if (!c)
        return LookupTable();

    switch(m_interpolation_method) {
        case CurveInterpolation::CatmullRom: {
            KnotsV ks = c->knots();

            int n   = ks.size() - 1;
            ks.insert(ks.begin(), { ks[0].x - (ks[1].x - ks[0].x), ks[0].y - (ks[1].y - ks[0].y) });
            ks.insert(ks.end(), { ks[n].x + (ks[n].x - ks[n - 1].x), ks[n].y + (ks[n].y - ks[n - 1].y) });

            return CurveInterpolator::CatmullRom(ks, size);
        }
        case CurveInterpolation::Linear: {
            return LookupTable();
        }
    }
}

CurveEditParameter::LookupTable CurveEditParameter::curveLookupTable(const std::string &name, int size) const
{
    for (int i = 0; i < m_curves.size(); ++i) {
        if (m_curves[i]->name() == name || m_curves[i]->shortName() == name)
            return curveLookupTable(i, size);
    }

    return LookupTable();
}

ParameterWidget *CurveEditParameter::newWidget(QWidget * parent)
{
    return new ParameterCurveEditWidget(this, parent);
}

void CurveEditParameter::load(const QSettings *setting)
{

}

void CurveEditParameter::save(QSettings *setting) const
{

}