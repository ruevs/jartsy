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
    std::vector<Material> materials;

    std::vector<PointLight> pointLights;
    std::vector<SunLight> parallelLights;

    std::vector<Transform> transforms;

    inline bool Intersect(Ray &r, Intersection *ints) const {
        bool ret = false;
        for each(auto mesh in meshes) {
            ret |= mesh.Intersect(r, ints);
        }
        return ret;
    };
};

// This is awfull :-)
void ParseCRTSceneFile(const Platform::Path &input, Scene &scene) {
    using json = nlohmann::json;
    std::ifstream i(input.raw);
    json j;
    i >> j;

    scene.settings.bgColor.r = j["settings"]["background_color"][0];
    scene.settings.bgColor.g = j["settings"]["background_color"][1];
    scene.settings.bgColor.b = j["settings"]["background_color"][2];


    const Float xs = 2.;
    const Float ys = 2.;
    Film film{{j["settings"]["image_settings"]["width"], j["settings"]["image_settings"]["height"]},
              (Float)sqrt(Sqr(xs) + Sqr(ys))};

    const Float focalLength = 1; // PAR@@ crtscene does not have camera focal length.
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
            PointLight light{{
                                 l["position"][0],
                                 l["position"][1],
                                 l["position"][2],
                             },
                             l["intensity"],
                             {1, 1, 1}}; // PAR@@ crtscene does not have coloured lights?!
            scene.pointLights.push_back(light);
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
            if("diffuse" == m["type"]) {
                mat.surfaceSmoothness = 0.;
            } else if("reflective" == m["type"]) {
                mat.surfaceSmoothness = 1.;
            } else {
                mat.surfaceSmoothness = .5; // PAR
            }
            scene.materials.push_back(mat);
        }
    } else {
        scene.materials.push_back({});
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

            mesh.nPoints = tmp.size() / 3;

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
            mesh.nTri = tmp.size() / 3;

            mesh.n = new Normal[mesh.nTri];
            mesh.CalclulateNormals();
        }
    }
}

#endif