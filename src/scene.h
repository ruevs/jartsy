#ifndef JARTSY_SCENE_H
#define JARTSY_SCENE_H

#include <vector>
#include <fstream>
#include "platform/platform.h"
#include "jartsyla.h"
#include "camera.h"
#include "ray.h"
#include "material.h"
#include "entities.h"
#include "lights.h"
#include "nlohman/json.hpp"

struct SceneSettings {
    Color bgColor;
};

class Scene {
public:
    SceneSettings settings;

    Camera *camera;

    std::vector<TriangleMesh> meshes;
    std::vector<Sphere> spheres;
    std::vector<Material> materials;

    std::vector<PointLight> pointLights;
    std::vector<SunLight> parallelLights;

    std::vector<Transform> transforms;

    inline bool Intersect(Ray &r, Intersection *ints) const {
        bool ret = false;
        for(const auto &mesh : meshes) {
            ret |= mesh.Intersect(r, ints);
        }
        for(const auto &sphere : spheres) {
            ret |= sphere.Intersect(r, ints);
        }
        return ret;
    };
};

// This is awfull :-)
void ParseCRTSceneFile(const Platform::Path &input, Scene &scene) {
    using json = nlohmann::json;
    std::ifstream is(input.raw);
    json j;
    is >> j;

    scene.settings.bgColor.r = j["settings"]["background_color"][0];
    scene.settings.bgColor.g = j["settings"]["background_color"][1];
    scene.settings.bgColor.b = j["settings"]["background_color"][2];


    const Float xs = 4.;
    const Float ys = 4.;
    Float diag     = (Float)sqrt(Sqr(xs) + Sqr(ys));

    if(j["camera"].contains("filmdiagonal")) {
        diag = j["camera"]["filmdiagonal"];
    }

    Film film{{j["settings"]["image_settings"]["width"], j["settings"]["image_settings"]["height"]},
              diag};

    Float focalLength = 1; // PAR@@ crtscene does not have camera focal length.
    if(j["camera"].contains("focallength")) {
        focalLength = j["camera"]["focallength"];
    }

    Transform cameraLocation = {{
                                    j["camera"]["position"][0],
                                    j["camera"]["position"][1],
                                    j["camera"]["position"][2],
                                },
                                Quaternion::From( // Quaternion from rotation matrix!
                                    {
                                        j["camera"]["matrix"][0],
                                        j["camera"]["matrix"][1],
                                        j["camera"]["matrix"][2],
                                    },
                                    {
                                        j["camera"]["matrix"][3],
                                        j["camera"]["matrix"][4],
                                        j["camera"]["matrix"][5],
                                    },
                                    {
                                        j["camera"]["matrix"][6],
                                        j["camera"]["matrix"][7],
                                        j["camera"]["matrix"][8],
                                    })};

    scene.camera = new Camera(cameraLocation, film, focalLength);

    if(j.contains("lights")) {
        for(auto &l : j["lights"]) {
            Color c;
            if(l.contains("color")) { // PAR@@ crtscene does not have coloured lights?!
                c.r = l["color"][0];
                c.g = l["color"][1];
                c.b = l["color"][2];
            } else {
                c = {1., 1., 1.};
            }
            PointLight light{{
                                 l["position"][0],
                                 l["position"][1],
                                 l["position"][2],
                             },
                             l["intensity"],
                             c};
            scene.pointLights.push_back(light);
        }
    }

    if(j.contains("parallel_lights")) {
        for(auto &l : j["parallel_lights"]) {
            Color c;
            if(l.contains("color")) { // PAR@@ crtscene does not have coloured lights?!
                c.r = l["color"][0];
                c.g = l["color"][1];
                c.b = l["color"][2];
            } else {
                c = {1., 1., 1.};
            }
            SunLight light{{
                                 l["direction"][0],
                                 l["direction"][1],
                                 l["direction"][2],
                             },
                             l["intensity"],
                             c};
            scene.parallelLights.push_back(light);
        }
    }

    if(j.contains("materials")) {
        for(auto &m : j["materials"]) {
            Material mat{{},
                         {
                             m["albedo"][0],
                             m["albedo"][1],
                             m["albedo"][2],
                         },
                         {},
                         0,
                         m["smooth_shading"]}; // PAR@@@
            const Float PARFudgeFactor = 0.0;   // Just for fun :-) PAR@@@@
            if("diffuse" == m["type"]) {
                mat.surfaceSmoothness = 0. + PARFudgeFactor;
            } else if("reflective" == m["type"]) {
                mat.surfaceSmoothness = 1. - PARFudgeFactor;
            } else if("advanced" == m["type"]) {
                mat.indexOfRefraction = {
                    m["refraction"][0],
                    m["refraction"][1],
                    m["refraction"][2],
                };
                mat.transmission = {
                    m["transmission"][0],
                    m["transmission"][1],
                    m["transmission"][2],
                };
                mat.surfaceSmoothness = m["smoothness"];
                if(m.contains("emission")) {
                    mat.emitter  = true;
                    mat.c = {
                        m["emission"]["color"][0],
                        m["emission"]["color"][1],
                        m["emission"]["color"][2],
                    };
                    mat.intensity = m["emission"]["intensity"];
                }
            } else {
                mat.surfaceSmoothness = .5; // PAR
            }
            scene.materials.push_back(mat);
        }
    } else {
        scene.materials.push_back({{}, {0.5, 0.5, 0.5}});
    }


    for(auto &m : j["objects"]) {
        scene.meshes.push_back({});
        TriangleMesh &mesh = scene.meshes.back();

        if(m.contains("material_index")) {
            mesh.material = &scene.materials[m["material_index"]];
        } else {
            mesh.material = &scene.materials[0];
        }

        {
            // Since the xyz coordinates of the points/vertexes in the file are
            // not structured, first read the whole flat array into a vector...
            std::vector<Float> tmp = m["vertices"];

            jartsyassert(0 == tmp.size() % 3,
                         "Vertex array in input file is not divisible by three!");

            mesh.nPoints = (unsigned)tmp.size() / 3;

            // ...allocate point storage...
            mesh.p = new Point[mesh.nPoints];

            if(true == mesh.material->smoothShading) {
                // allocate vertex normal storage
                mesh.vn = new Normal[mesh.nPoints];
            }

            // ...and copy the points over.
            for(size_t i = 0; i < mesh.nPoints; ++i) {
                mesh.p[i].x = tmp[3 * i];
                mesh.p[i].y = tmp[3 * i + 1];
                mesh.p[i].z = tmp[3 * i + 2];
            }
        }

        {
            std::vector<unsigned> tmp = m["triangles"];

            jartsyassert(0 == tmp.size() % 3,
                         "Triangle array in input file is not divisible by three!");
            
            mesh.v = new unsigned[tmp.size()]; // Yes, flat vertex index array - three per triangle.
            std::copy(tmp.begin(), tmp.end(), mesh.v);
            mesh.nTri = (unsigned)tmp.size() / 3;

            mesh.n = new Normal[mesh.nTri];
            mesh.CalclulateNormals();
        }
    }

    for(auto &m : j["spheres"]) {
        scene.spheres.push_back({{m["center"][0], m["center"][1], m["center"][2]},
                                 m["radius"],
                                 &scene.materials[m["material_index"]]});
    }
}

#endif
