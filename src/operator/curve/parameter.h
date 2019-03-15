#pragma once

#include <string>
#include <vector>

#include <parameter/parameter.h>
#include "curve.h"


// ----------------------------------------------------------------------------

class CurveParameter : public Parameter
{
  public:
    enum class Type { Regular, Looped };

  public:
    CurveParameter() = default;
    CurveParameter(const std::string &name, const std::string &sname,
                   const ColorRGBA8 &color);

  public:
    std::string shortName() const;
    void setShortName(const std::string &v);

    ColorRGBA8 color() const;
    void setColor(const ColorRGBA8 &v);

    Type type() const;
    void setType(const Type &v);

    SliderParameter* opacityParameter();
    float opacity() const;
    void setOpacity(const float &v);

    KnotsV knots() const;
    void setKnots(const KnotsV &k);

  public:
    ParameterWidget *newWidget(QWidget * parent = nullptr) override;

    void load(const QSettings *setting) override;
    void save(QSettings *setting) const override;

  private:
    std::string m_short_name;
    ColorRGBA8 m_color;
    Type m_type;

    KnotsV m_knots;
    UPtr<SliderParameter> m_opacity;
};

// ----------------------------------------------------------------------------

class CurveEditParameter : public Parameter
{
  public:
    enum class CoordinateSystem { Cartesian, Polar };
    enum class CurveInterpolation { Linear, CatmullRom };
    using LookupTable = std::vector<float>;

  public:
    CurveEditParameter() = default;
    CurveEditParameter(const std::string &name,
                       CurveInterpolation ci = CurveInterpolation::CatmullRom,
                       CoordinateSystem cs   = CoordinateSystem::Cartesian);

  public:
    CoordinateSystem coordinateSystem() const;
    void setCoordinateSystem(const CoordinateSystem &v);

    CurveInterpolation interpolation() const;
    void setInterpolation(const CurveInterpolation &v);

    ColorRGBA8 backgroundColor() const;
    void setBackgroundColor(const ColorRGBA8 &v);

    OptT<ColorCallback> backgroundColorCb() const;
    void setBackgroundColorCb(const ColorCallback &v);

  public:
    void addCurve(CurveParameter *c);

    const UPtrV<CurveParameter>& curves() const;

    const CurveParameter *curveByIndex(int index) const;
    const CurveParameter *curveByName(const std::string &name) const;

    LookupTable curveLookupTable(int index, int size) const;
    LookupTable curveLookupTable(const std::string &name, int size) const;

  public:
    ParameterWidget *newWidget(QWidget * parent = nullptr) override;

    void load(const QSettings *setting) override;
    void save(QSettings *setting) const override;

  private:
    CoordinateSystem m_coordinate_system;
    CurveInterpolation m_interpolation_method;

    ColorRGBA8 m_background_color;
    OptT<ColorCallback> m_background_color_cb;

    UPtrV<CurveParameter> m_curves;
};
