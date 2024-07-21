#ifndef RAY_H
#define RAY_H

#include <SFML/Graphics.hpp>
#include <math.h>
#include <thread>
#include <mutex>
#include <vector>

class Vec3f {
public:
    float x, y, z;

    Vec3f(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    Vec3f normalize() const {
        float len = sqrt(x * x + y * y + z * z);
        return Vec3f(x / len, y / len, z / len);
    }

    float dot(const Vec3f& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3f operator-(const Vec3f& other) const {
        return Vec3f(x - other.x, y - other.y, z - other.z);
    }

    Vec3f operator+(const Vec3f& other) const {
        return Vec3f(x + other.x, y + other.y, z + other.z);
    }

    Vec3f operator*(float scalar) const {
        return Vec3f(x * scalar, y * scalar, z * scalar);
    }

    Vec3f operator-() const {
        return Vec3f(-x, -y, -z);
    }
};

Vec3f operator*(float scalar, const Vec3f& vec) {
    return Vec3f(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

class Color {
public:
    float r, g, b;

    Color(float r = 0.0f, float g = 0.0f, float b = 0.0f) : r(r), g(g), b(b) {}

    Color operator*(const Color& other) const {
        return Color(r * other.r, g * other.g, b * other.b);
    }

    Color operator*(float scalar) const {
        return Color(r * scalar, g * scalar, b * scalar);
    }

    Color operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b);
    }

    sf::Color ToSFColor() {
    	return sf::Color(r * 255, g * 255, b * 255);
    }
};

class Ray {
public:
    Vec3f origin, dir;
    Color col;
    int num_bounces;

    Ray(Vec3f origin, Vec3f dir, Color col = Color(1.0f, 1.0f, 1.0f), int num_bounces = 0)
        : origin(origin), dir(dir.normalize()), col(col), num_bounces(num_bounces) {}
};

class Sphere {
public:
	Vec3f center;
	float radius;
	Color col;
	bool light;
	bool ismirror;

	Sphere() : center(Vec3f()), radius(0.0f), col(Color()), light(false), ismirror(false) {}
	Sphere(Vec3f cen, float r, Color c, bool l, bool im) : center(cen), radius(r), col(c), light(l), ismirror(im) {}
};

float raySphereIntersection(Ray r, Sphere s) {
	Vec3f oc(r.origin.x-s.center.x, r.origin.y-s.center.y, r.origin.z-s.center.z);
	float a = r.dir.x*r.dir.x + r.dir.y*r.dir.y + r.dir.z*r.dir.z;
	float b = 2.0 * (oc.x*r.dir.x + oc.y*r.dir.y + oc.z*r.dir.z);
	float c = oc.x*oc.x + oc.y*oc.y + oc.z*oc.z - s.radius*s.radius;
	float discriminant = b*b - 4*a*c;
	if (discriminant < 0) {
		return -1.0f;
	} else {
		return std::min((-b + sqrt(discriminant)) / (2.0 * a), (-b - sqrt(discriminant)) / (2.0 * a));
	}
}

Vec3f generateRandomUnitVector() {
	Vec3f randomVec;
	randomVec.x = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2)) - 1;
	randomVec.y = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2)) - 1;
	randomVec.z = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2)) - 1;

	randomVec = randomVec.normalize();

	return randomVec;
}

Color calculateRayColor(Ray r) {
    Sphere s(Vec3f(-4.0f, 0.0f, 16.0f), 3.0f, Color(0.2f, 0.2f, 0.2f), false, false);
    Sphere s3(Vec3f(3.5f, 0.0f, 12.0f), 4.0f, Color(0.35f, 0.8f, 0.3f), false, false);
    Sphere s4(Vec3f(0.0f, -100.0f, 10.0f), 94.0f, Color(0.6f, 0.6f, 0.6f), false, false);
    Sphere s5(Vec3f(60.0f, 65.0f, 0.0f), 45.0f, Color(1.0f, 1.0f, 1.0f), true, false);
    Sphere s6(Vec3f(0.0f, 20.0f, 80.0f), 10.0f, Color(0.5f, 0.2f, 0.4f), false, false);
    Sphere s7(Vec3f(-20.0f, 10.0f, 20.0f), 5.0f, Color(0.8f, 0.8f, 0.8f), false, true);

    float closest_t = 1000.0f;
    Sphere* closest_sphere = nullptr;

    float intersection = raySphereIntersection(r, s);
    if (intersection > 0.0f && intersection < closest_t) {
        closest_t = intersection;
        closest_sphere = &s;
    }

    float intersection3 = raySphereIntersection(r, s3);
    if (intersection3 > 0.0f && intersection3 < closest_t) {
        closest_t = intersection3;
        closest_sphere = &s3;
    }

    float intersection4 = raySphereIntersection(r, s4);
    if (intersection4 > 0.0f && intersection4 < closest_t) {
        closest_t = intersection4;
        closest_sphere = &s4;
    }

    float intersection5 = raySphereIntersection(r, s5);
    if (intersection5 > 0.0f && intersection5 < closest_t) {
        closest_t = intersection5;
        closest_sphere = &s5;
    }

    float intersection6 = raySphereIntersection(r, s6);
    if (intersection6 > 0.0f && intersection6 < closest_t) {
        closest_t = intersection6;
        closest_sphere = &s6;
    }

    float intersection7 = raySphereIntersection(r, s7);
    if (intersection7 > 0.0f && intersection7 < closest_t) {
        closest_t = intersection7;
        closest_sphere = &s7;
    }

    if (closest_t < 1000.0f && closest_sphere != nullptr) {
        Vec3f pos = r.origin + r.dir * closest_t;
        Vec3f normal = (pos - closest_sphere->center).normalize();

        if (closest_sphere->light) {
            return closest_sphere->col * r.col;
        } else {
            if (r.num_bounces < 10) {
                if (!closest_sphere->ismirror) {
                    Vec3f new_dir = generateRandomUnitVector();
                    new_dir = new_dir + normal;
                    if (new_dir.dot(normal) < 0.0f) {
                        new_dir = -new_dir;
                    }
                    new_dir = new_dir.normalize();
                    Vec3f adjusted_pos = pos + 0.001f * new_dir;
                    Color new_col = calculateRayColor(Ray(adjusted_pos, new_dir, r.col * closest_sphere->col, r.num_bounces + 1));
                    return new_col;
                } else {
                    Vec3f reflection_dir = r.dir - 2.0f * r.dir.dot(normal) * normal;
                    reflection_dir = reflection_dir.normalize();
                    Vec3f adjusted_pos = pos + 0.001f * reflection_dir;
                    Color new_col = calculateRayColor(Ray(adjusted_pos, reflection_dir, r.col * closest_sphere->col, r.num_bounces + 1));
                    return new_col;
                }
            }
            return Color(0.0f, 0.0f, 0.0f);
        }
    } else {
        return Color(10.0f / 255.0f, 10.0f / 255.0f, 20.0f / 255.0f) * r.col;
    }
}

#endif
