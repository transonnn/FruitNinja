#include "Utils.h"
#include <cmath>

bool lineIntersectsCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float fx = x1 - cx;
    float fy = y1 - cy;
    float a = dx * dx + dy * dy;
    float b = 2 * (fx * dx + fy * dy);
    float c = fx * fx + fy * fy - r * r;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false;
    discriminant = sqrt(discriminant);
    float t1 = (-b - discriminant) / (2 * a);
    float t2 = (-b + discriminant) / (2 * a);
    if (t1 >= 0 && t1 <= 1) return true;
    if (t2 >= 0 && t2 <= 1) return true;
    float dist1 = sqrt((x1 - cx) * (x1 - cx) + (y1 - cy) * (y1 - cy));
    float dist2 = sqrt((x2 - cx) * (x2 - cx) + (y2 - cy) * (y2 - cy));
    if (dist1 <= r || dist2 <= r) return true;
    return false;
}