#include "../../include/primitives/torus.hpp"
#include <cmath>
#include <algorithm>

using namespace RayTracer;

// =========================================================================
// OUTILS MATHÉMATIQUES INTERNES
// =========================================================================

static int solveTorusCubic(double c2, double c1, double c0, double roots[3]) {
    double p = c1 - (c2 * c2) / 3.0;
    double q = (2.0 * c2 * c2 * c2) / 27.0 - (c2 * c1) / 3.0 + c0;
    double disc = (q * q) / 4.0 + (p * p * p) / 27.0;

    if (disc > 0) {
        double u = std::cbrt(-q / 2.0 + std::sqrt(disc));
        double v = std::cbrt(-q / 2.0 - std::sqrt(disc));
        roots[0] = u + v - c2 / 3.0;
        return 1;
    } else if (disc == 0) {
        double u = std::cbrt(-q / 2.0);
        roots[0] = 2.0 * u - c2 / 3.0;
        roots[1] = -u - c2 / 3.0;
        return 2;
    } else {
        double r = std::sqrt(-(p * p * p) / 27.0);
        double phi = std::acos(-q / (2.0 * r));
        roots[0] = 2.0 * std::pow(r, 1.0 / 3.0) * std::cos(phi / 3.0) - c2 / 3.0;
        roots[1] = 2.0 * std::pow(r, 1.0 / 3.0) * std::cos((phi + 2.0 * M_PI) / 3.0) - c2 / 3.0;
        roots[2] = 2.0 * std::pow(r, 1.0 / 3.0) * std::cos((phi + 4.0 * M_PI) / 3.0) - c2 / 3.0;
        return 3;
    }
}

static int solveTorusQuartic(double c3, double c2, double c1, double c0, double roots[4]) {
    double A = c3;
    double B = c2;
    double C = c1;
    double D = c0;

    double A2 = A * A;
    double p = B - 0.375 * A2;
    double q = C - 0.5 * A * B + 0.125 * A * A2;
    double r = D - 0.25 * A * C + 0.0625 * A2 * B - 0.01171875 * A2 * A2;

    int numRoots = 0;

    if (std::abs(q) < 1e-9) {
        double disc = p * p - 4.0 * r;
        if (disc >= 0) {
            double sqrtDisc = std::sqrt(disc);
            double z1 = (-p + sqrtDisc) / 2.0;
            double z2 = (-p - sqrtDisc) / 2.0;

            if (z1 >= 0) {
                roots[numRoots++] = std::sqrt(z1) - A / 4.0;
                roots[numRoots++] = -std::sqrt(z1) - A / 4.0;
            }
            if (z2 >= 0) {
                roots[numRoots++] = std::sqrt(z2) - A / 4.0;
                roots[numRoots++] = -std::sqrt(z2) - A / 4.0;
            }
        }
    } else {
        double cbRoots[3];
        solveTorusCubic(-2.0 * p, p * p - 4.0 * r, q * q, cbRoots);
        double z = cbRoots[0];

        if (z > 0) {
            double g = std::sqrt(z);
            double h = 0.5 * (p + z - q / g);
            double k = 0.5 * (p + z + q / g);

            double disc1 = g * g - 4.0 * h;
            if (disc1 >= 0) {
                roots[numRoots++] = (-g + std::sqrt(disc1)) / 2.0 - A / 4.0;
                roots[numRoots++] = (-g - std::sqrt(disc1)) / 2.0 - A / 4.0;
            }
            double disc2 = g * g - 4.0 * k;
            if (disc2 >= 0) {
                roots[numRoots++] = (g + std::sqrt(disc2)) / 2.0 - A / 4.0;
                roots[numRoots++] = (g - std::sqrt(disc2)) / 2.0 - A / 4.0;
            }
        }
    }
    return numRoots;
}

// =========================================================================
// CONSTRUCTEURS
// =========================================================================

Torus::Torus(const Math::Point3D &centerValue, double majorR, double minorR, const Math::Vector3D &col)
    : center(centerValue), majorRadius(majorR), minorRadius(minorR), color(col.x, col.y, col.z, 1.0)
{
}

Torus::Torus(const Math::Point3D &centerValue, double majorR, double minorR, const Math::Vector4D &col)
    : center(centerValue), majorRadius(majorR), minorRadius(minorR), color(col)
{
}

// =========================================================================
// INTERSECTION
// =========================================================================

HitRecord Torus::hit(const Ray &ray) const
{
    HitRecord record;
    record.hit = false;
    record.t = -1.0;

    // Transformation du rayon dans l'espace local du tore
    Math::Vector3D o(ray.origin.x - center.x, ray.origin.y - center.y, ray.origin.z - center.z);
    
    double dirLen = ray.direction.length();
    Math::Vector3D rayDir = (dirLen < 1e-6) ? ray.direction : ray.direction * (1.0 / dirLen);

    double R2 = majorRadius * majorRadius;
    double r2 = minorRadius * minorRadius;

    double od_dot = o.x * rayDir.x + o.y * rayDir.y + o.z * rayDir.z;
    double o_dot  = o.x * o.x + o.y * o.y + o.z * o.z;

    double G = 4.0 * R2 * (rayDir.x * rayDir.x + rayDir.y * rayDir.y);
    double H = 8.0 * R2 * (o.x * rayDir.x + o.y * rayDir.y);
    double I = 4.0 * R2 * (o.x * o.x + o.y * o.y);
    double J = o_dot - (R2 + r2);

    // Coefficients pour l'équation quartique
    double c3 = 4.0 * od_dot;
    double c2 = 2.0 * J + 4.0 * od_dot * od_dot + G;
    double c1 = 4.0 * J * od_dot + H;
    double c0 = J * J - I;

    double roots[4];
    int numRoots = solveTorusQuartic(c3, c2, c1, c0, roots);

    if (numRoots == 0) return record;

    double t = -1.0;
    for (int i = 0; i < numRoots; ++i) {
        if (roots[i] > 0.001) {
            
            Math::Point3D pTest = ray.origin + rayDir * roots[i];
            Math::Point3D pLoc(pTest.x - center.x, pTest.y - center.y, pTest.z - center.z);
            
            double distFromAxis = std::sqrt(pLoc.x * pLoc.x + pLoc.y * pLoc.y);
            
            if (distFromAxis >= (majorRadius - minorRadius * 0.95)) {
                if (t < 0.0 || roots[i] < t) {
                    t = roots[i];
                }
            }
        }
    }

    if (t < 0.0) return record;

    record.hit = true;
    record.t = t;
    record.point = ray.origin + rayDir * t;

    Math::Point3D pLocal(record.point.x - center.x, record.point.y - center.y, record.point.z - center.z);
    double projectionPlaneXY = std::sqrt(pLocal.x * pLocal.x + pLocal.y * pLocal.y);
    
    if (projectionPlaneXY < 1e-6) {
        record.normal = Math::Vector3D(0, 0, pLocal.z > 0 ? 1 : -1);
    } else {
        double paramK = 1.0 - majorRadius / projectionPlaneXY;
        Math::Vector3D normalVec(pLocal.x * paramK, pLocal.y * paramK, pLocal.z);
        
        double len = normalVec.length();
        record.normal = len < 1e-6 ? Math::Vector3D(0, 0, 1) : normalVec * (1.0 / len);
    }

    record.color = color;
    return record;
}
