#include "Material.h"

Material::Material()
{
}
Material::Material(Vector3 kd, Vector3 ks):
k_d(kd), k_s(ks)
{

}
Material::~Material()
{
}

Vector3
Material::shade(Ray&, const HitInfo&, const Scene&) const
{
    return Vector3(1.0f, 1.0f, 1.0f);
}