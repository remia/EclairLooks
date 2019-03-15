#include "curve.h"

#include <cmath>

#include <QGenericMatrix>


struct CubicPoly {
    float c0, c1, c2, c3;

    float eval(float t)
    {
        float t2 = t * t;
        float t3 = t2 * t;
        return c0 + c1 * t + c2 * t2 + c3 * t3;
    }
};

/*
 * Compute coefficients for a cubic polynomial
 *   p(s) = c0 + c1*s + c2*s^2 + c3*s^3
 * such that
 *   p(0) = x0, p(1) = x1
 *  and
 *   p'(0) = t0, p'(1) = t1.
 */
void InitCubicPoly(float x0, float x1, float t0, float t1, CubicPoly &p)
{
    p.c0 = x0;
    p.c1 = t0;
    p.c2 = -3 * x0 + 3 * x1 - 2 * t0 - t1;
    p.c3 = 2 * x0 - 2 * x1 + t0 + t1;
}

// standard Catmull-Rom spline: interpolate between x1 and x2 with previous/following
// points x1/x4 (we don't need this here, but it's for illustration)
void InitCatmullRom(float x0, float x1, float x2, float x3, CubicPoly &p)
{
    // Catmull-Rom with tension 0.5
    InitCubicPoly(x1, x2, 0.5f * (x2 - x0), 0.5f * (x3 - x1), p);
}

// compute coefficients for a nonuniform Catmull-Rom spline
void InitNonuniformCatmullRom(float x0, float x1, float x2, float x3, float dt0,
                              float dt1, float dt2, CubicPoly &p)
{
    // compute tangents when parameterized in [t1,t2]
    float t1 = (x1 - x0) / dt0 - (x2 - x0) / (dt0 + dt1) + (x2 - x1) / dt1;
    float t2 = (x2 - x1) / dt1 - (x3 - x1) / (dt1 + dt2) + (x3 - x2) / dt2;

    // rescale tangents for parametrization in [0,1]
    t1 *= dt1;
    t2 *= dt1;

    InitCubicPoly(x1, x2, t1, t2, p);
}

float VecDistSquared(const Knots &p, const Knots &q)
{
    float dx = q.x - p.x;
    float dy = q.y - p.y;
    return dx * dx + dy * dy;
}

void InitCentripetalCR(const Knots &p0, const Knots &p1,
                       const Knots &p2, const Knots &p3, CubicPoly &px,
                       CubicPoly &py)
{
    float dt0 = powf(VecDistSquared(p0, p1), 0.25f);
    float dt1 = powf(VecDistSquared(p1, p2), 0.25f);
    float dt2 = powf(VecDistSquared(p2, p3), 0.25f);

    // safety check for repeated points
    if (dt1 < 1e-4f)
        dt1 = 1.0f;
    if (dt0 < 1e-4f)
        dt0 = dt1;
    if (dt2 < 1e-4f)
        dt2 = dt1;

    InitNonuniformCatmullRom(p0.x, p1.x, p2.x, p3.x, dt0, dt1, dt2, px);
    InitNonuniformCatmullRom(p0.y, p1.y, p2.y, p3.y, dt0, dt1, dt2, py);
}

std::vector<float> CurveInterpolator::CatmullRom(const KnotsV &points, int size)
{
    std::vector<float> res(size);

    uint16_t segments_count = points.size() - 3;
    uint16_t index = 0;

    // For each curve segment
    for (int s = 0; s < segments_count; ++s) {
        Knots p0(points[s]);
        Knots p1(points[s + 1]);
        Knots p2(points[s + 2]);
        Knots p3(points[s + 3]);
        CubicPoly px, py;
        InitCentripetalCR(p0, p1, p2, p3, px, py);

        int count = (p2.x - p1.x) * size;
        if (s == segments_count - 1)
            count += size - (index + count);

        for (int p = 0; p < count; ++p) {
            float t = 0.5;
            if (count > 1)
                t = p / (count - 1.f);

            float x = std::clamp(px.eval(t), 0.f, 1.f);
            float y = std::clamp(py.eval(t), 0.f, 1.f);
            res[index + p] = { y };
        }

        index += count;
    }

    return res;
}

float Curve::eval(float x)
{
    // Find segment, ie. 4 points
    // Initialize cubic polynomial wieghts
    // Find the roots, keep only the one between [0..1]
    // Evaluate the curve

    return x;
}