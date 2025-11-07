//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    Intersection p_inter = intersect(ray);

    if (!p_inter.happened) {
        return Vector3f(0.0);
    }

    if (p_inter.m->hasEmission()) {
        return p_inter.m->getEmission();
    }

    float EPLISON = 0.0001;
    

    // l_dir = f_r * L_i * cos(theta) * cos(theta') / (distance^2) / pdf_light
    // l_indir = f_r * cos(theta) * castRay(new_ray, depth+1) / pdf_brdf
    Vector3f l_dir;
    Vector3f l_indir;

    // 直接光照：对光源采样
    Intersection light_inter;
    float light_pdf = 0.0f;
    sampleLight(light_inter, light_pdf);

    auto p = p_inter.coords;
    auto wo = ray.direction;
    auto x = light_inter.coords;
    auto ws = (x - p).normalized();
    auto N = p_inter.normal.normalized();
    auto NN = light_inter.normal.normalized();
    auto emit = light_inter.emit;
    auto ws_distance = (x - p).norm();

    Ray ws_ray(p, ws);
    auto ws_ray_inter =  intersect(ws_ray);

    if (ws_ray_inter.distance - ws_distance > -EPLISON) 
    {
        l_dir = emit * p_inter.m->eval(ray.direction, ws_ray.direction, N)
            * dotProduct(ws_ray.direction, N)
            * dotProduct(-ws_ray.direction, NN)
            / std::pow(ws_distance, 2)
            / light_pdf;
    }

    if (get_random_float() > RussianRoulette) {
        return l_dir;
    }

    //if (get_random_float() > RussianRoulette)
    {
        auto wi = p_inter.m->sample(wo, N).normalized();
        Ray wi_ray(p, wi);
        auto wi_ray_inter = intersect(wi_ray);
        if (wi_ray_inter.happened && !wi_ray_inter.m->hasEmission()) 
        {
            auto q = wi_ray_inter.coords;
            l_indir =  castRay(wi_ray,depth + 1) * p_inter.m->eval(ray.direction,wi,N) 
                * std::max(0.0f,dotProduct(wi,N)) / p_inter.m->pdf(wo,wi,N) / RussianRoulette;
        }
    }

    return l_dir + l_indir;
}