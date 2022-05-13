void HW21(int xr, int yr, FrameBuffer<RGBColor> &rgbfr) {
    // Homework 2 part 1 rectangles

    // numbrer of rectangles in X and Y direction
    const size_t nxrect = 4;
    const size_t nyrect = 4;
    // rectangle size ceil (rounded up)
    const size_t xrect = (xr + nxrect - 1) / nxrect;
    const size_t yrect = (yr + nyrect - 1) / nyrect;
    RGBColor pix{0, 0, 0};
    for(int x = 0; x < xr; ++x) {
        if(0 == (x % xrect)) {
            pix.red += std::numeric_limits<uint8_t>::max() / nxrect;
        }
        pix.green = 0;
        for(int y = 0; y < yr; ++y) {
            if(0 == (y % yrect)) {
                pix.green += std::numeric_limits<uint8_t>::max() / nyrect;
            }
            pix.blue = (std::numeric_limits<uint8_t>::max() * rand()) / RAND_MAX; // blue is random
            rgbfr[x][y] = pix;
        }
    }
}

void HW22(int xr, int yr, FrameBuffer<RGBColor> &rgbfr) {
    // Homework 2 part 2 circle

    // coordinates of the center of the circle
    const size_t xc = xr / 2;
    const size_t yc = yr / 2;
    // radius suqared
    const size_t rsq = min(xr, yr) * min(xr, yr) / (4 * 4);
    for(int x = 0; x < xr; ++x) {
        for(int y = 0; y < yr; ++y) {
            if((x - xc) * (x - xc) + (y - yc) * (y - yc) < rsq) {
                // we are in the circle
                rgbfr[x][y] = {57, 119, 34}; // circle color from my screen shot.
            } else {
                rgbfr[x][y] = {183, 183,
                               183}; // because that is what the backgroud is in my screen shot :-)
            }
        }
    }
}

void HW30(int xr, int yr, FrameBuffer<RGBColor> &rgbfr) {
    // Homework 3 rays

    // Size of the camra film/senzor in meters (world scale)
    const Float xs  = 2.;
    const Float ys  = 2.;
    const Film film = {{xr, yr}, (Float)sqrt(Sqr(xs) + Sqr(ys))};

    const Transform cameraLocation = {
        {.0, .0, .0} /*transaltion*/, {} /*rotation*/, {1., 1., 1.} /*scale*/};
    const Float focalLength = 1;

    Camera camera(cameraLocation, film, focalLength);

    for(int x = 0; x < xr; ++x) {
        for(int y = 0; y < yr; ++y) {
            Ray ray     = camera.GenerateRay({x + (Float)0.5, y + (Float)0.5});
            rgbfr[x][y] = {(uint8_t)abs(ray.d.x * std::numeric_limits<uint8_t>::max()),
                           (uint8_t)abs(ray.d.y * std::numeric_limits<uint8_t>::max()),
                           (uint8_t)abs(ray.d.z * std::numeric_limits<uint8_t>::max())};
        }
    }
}

void HW42(void) {
    // Homework 4. Vector math.
    dbp("\nLecture 4. Vector math.");
    dbp("\n4.2: Cross product");
    const Vector A{3.5, 0, 0};
    const Vector B{1.75, 3.5, 0};
    DBPTRI(A, "cross", B, "=", A.Cross(B));
    const Vector C{3, -3, 1};
    const Vector D{4, 9, 3};
    DBPTRI(C, "cross", D, "=", C.Cross(D));
    dbp("Magnitude (area of parallelogram) %f", C.Cross(D).Magnitude());
    const Vector E{-12, 12, -4};
    DBPTRI(C, "cross", E, "=", C.Cross(E));
    dbp("Magnitude (area of parallelogram) %f", C.Cross(E).Magnitude());
}

void HW43(void) {
    // Homework 4. Triangle normals.
    dbp("\n4.3: Triangle normals");
    const unsigned nTria     = 3;
    Point p[nTria*3] = {{-1.75, -1.75, -3},  {1.75, -1.75, -3}, {0, 1.75, -3},
                              {0, 0, -1}, {1, 0, 1}, {-1, 0, 1},
                              {0.56, 1.11, 1.23}, {0.44, -2.368, -0.54}, {-1.56, 0.15, -1.92}};
    unsigned vertexIndex[nTria*3] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    Normal n[nTria];
    Material material = {};
    TriangleMesh trMesh{nTria * 3, nTria, p, nullptr, vertexIndex, n, &material};

    // Do not normalize the normals to use half their length as the surface of the triangles in the
    // next step.
    trMesh.CalclulateNormals(/*normalize=*/false);

    for(int i = 0; i < nTria; ++i) {
        dbp("Triangle %d", i);
        DBPTRI(trMesh.p[trMesh.v[i + 0]], "", trMesh.p[trMesh.v[i + 1]],
               "", trMesh.p[trMesh.v[i + 2]]);
        dbp("Normal: (%.3f %.3f %.3f), area: %f", CO(trMesh.n[i]), 0.5 * trMesh.n[i].Magnitude());
    }
}

void HW51(int xr, int yr, FrameBuffer<RGBColor> &rgbfr) {
    // Homework 5.1 triangle intersection

    // Size of the camra film/senzor in meters (world scale)
    const Float xs  = 5.;
    const Float ys  = 5.;
    const Film film = {{xr, yr}, (Float)sqrt(Sqr(xs) + Sqr(ys))};

    const Transform cameraLocation = {
        {.0, .0, .0} /*transaltion*/, {} /*rotation*/, {1., 1., 1.} /*scale*/};
    const Float focalLength = 1;

    Camera camera(cameraLocation, film, focalLength);

    Point points[3]   = {{-1.75, -1.75, -3}, {1.75, -1.75, -3}, {0, 1.75, -3}};
    Normal vertexNormals[3] = {};
    unsigned vertexes[3]   = {0, 1, 2};
    Normal normals[1] = {};
    Material material = {{}, {}, {}, 0, true};
    TriangleMesh mesh = {3, 1, points, vertexNormals, vertexes, normals, &material};
    mesh.CalclulateNormals();

    for(int x = 0; x < xr; ++x) {
        for(int y = 0; y < yr; ++y) {
            Ray ray     = camera.GenerateRay({x + (Float)0.5, y + (Float)0.5});
            const uint8_t c255 = std::numeric_limits<uint8_t>::max();
            Intersection ints;
            if(mesh.Intersect(ray, &ints)) {
                rgbfr[x][y] = {(uint8_t)(ints.uc * c255), (uint8_t)(ints.vc * c255), 0};
            } else {
                rgbfr[x][y] = {(uint8_t)abs(ray.d.x * c255),
                               (uint8_t)abs(ray.d.y * c255),
                               (uint8_t)abs(ray.d.z * c255)};
            }
        }
    }
}

void HW5234(int xr, int yr, FrameBuffer<RGBColor> &rgbfr) {
    // Homework 5.2 5.3 5.4 triangle mesh intersection

    // Size of the camra film/senzor in meters (world scale)
    const Float xs  = 2.;
    const Float ys  = 2.;
    const Film film = {{xr, yr}, (Float)sqrt(Sqr(xs) + Sqr(ys))};

    const Quaternion q             = Quaternion::From({0, 0, 1} , 0.*M_PI/180);
    const Transform cameraLocation = {
        {.0, .0, .0} /*transaltion*/, q /*rotation*/, {1., 1., 1.} /*scale*/};
    const Float focalLength = 1;

    Camera camera(cameraLocation, film, focalLength);

    const int nt       = 12; // triangles
    const int np       = 10; // points/vertexes
    Point points[np]   = {{0., 0., -3.},  {1., 0., -3.},  {1., 1., -4.},
                         {0., 1., -3.},   {-1., 1., -4.}, {-1., 0., -3.},
                         {-1., -1., -4.}, {0., -1., -3.}, {1., -1., -4.}, {0., 0., -3.5}};
    Normal vertexNormals[np] = {};
    unsigned vertexes[nt * 3] = {
        0,1,2,
        0,2,3,
        0,3,4,
        0,4,5,
        0,5,6,
        0,6,7,
        0,7,8,
        0,8,1,

        9,1,3,
        9,3,5,
        9,5,7,
        9,7,1
    };
    Normal normals[nt] = {};
    Material material  = {{}, {}, {}, 0, false};
    TriangleMesh mesh  = {np, nt, points, vertexNormals, vertexes, normals, &material};
    mesh.CalclulateNormals();

    for(int x = 0; x < xr; ++x) {
        for(int y = 0; y < yr; ++y) {
            Ray ray            = camera.GenerateRay({x + (Float)0.5, y + (Float)0.5});
            const uint8_t c255 = std::numeric_limits<uint8_t>::max();
            Intersection ints;
            if(mesh.Intersect(ray, &ints)) {
                rgbfr[x][y] = {(uint8_t)abs(ints.n.x * c255), (uint8_t)abs(ints.n.y * c255),
                               (uint8_t)abs(ints.n.z * c255)};
            } else {
                rgbfr[x][y] = {(uint8_t)abs(ray.d.x * c255), (uint8_t)abs(ray.d.y * c255),
                               (uint8_t)abs(ray.d.z * c255)};
            }
        }
    }
}

void Homework(int width, int height, const Platform::Path &output) {

    // Just stupid hacks to generate the homework images
    // To generate the images run:
    //    jartsy.exe render -o %.ppm --size 1024x768 rectangles circle rays
    static int homeworkimage;

    fprintf(stderr, "Doing nothing very quickly %i.\n", homeworkimage);

    // Pixel resolution of the generated images from the command line
    FrameBuffer<RGBColor> rgbfr(width, height);

    if(0 == homeworkimage) {
        HW21(width, height, rgbfr);
    }

    if(1 == homeworkimage) {
        HW22(width, height, rgbfr);
    }

    if(2 == homeworkimage) {
        HW30(width, height, rgbfr);
    }

    if(3 == homeworkimage) {
        HW42();
        HW43();
    }

    if(4 == homeworkimage) {
        HW51(width, height, rgbfr);
    }

    if(5 == homeworkimage) {
        HW5234(width, height, rgbfr);
    }

    ++homeworkimage;

    if("ppm" == output.Extension()) {
        PGMWriter::ExportFrameBufferTo(output, rgbfr);
    } else if("png" == output.Extension()) {
        PNGWriter::ExportFrameBufferTo(output, rgbfr);
    }
}

void RenderNormals(const Scene &scene, FrameBuffer<Color> &rgbfr) {
    for(int x = 0; x < scene.camera->film.resolution.x; ++x) {
        fprintf(stderr, "x = %5d %5.2f%%\r", x, 100. * (x + 1) / scene.camera->film.resolution.x);
        for(int y = 0; y < scene.camera->film.resolution.y; ++y) {
            Ray ray            = scene.camera->GenerateRay({x + (Float)0.5, y + (Float)0.5});
            Intersection ints;
            if(scene.Intersect(ray, &ints)) {
                rgbfr[x][y] = {abs(ints.n.x), abs(ints.n.y), abs(ints.n.z)};
            } else {
                rgbfr[x][y] = {abs(ray.d.x), abs(ray.d.y), abs(ray.d.z)};
            }
        }
    }
}

Color WhittedIntegrate(const Scene &scene, Ray ray, int depth) {
    if(0 < depth) {
        Intersection ints;
        if(scene.Intersect(ray, &ints)) {
            Color color{};
            for each(auto light in scene.pointLights) {
                Intersection lightints;
                Vector lightDir = light.VectorTo(ints.ip /* + ints.n * 0.01*/);

                Ray raytolight{ints.ip, lightDir.WithMagnitude(1), 0.,
                               10*LENGTH_EPS,
                               lightDir.Magnitude()};
                if(scene.Intersect(raytolight, &lightints)) {
                    // We hit an object PATH tracing ?!?
                } else {
                    // return ints.material->reflectance;
                    auto lambertian = ints.n.Dot(raytolight.d);
                    if(0 < lambertian) {
                        color += (light.intensity * light.c * ints.material->reflectance /
                                 (4 * M_PI * raytolight.tInt *
                                  raytolight.tInt)); // tInt is the distance to the light
                    } else {
                    }
                }
            }
            return color;
        } else {
            return scene.settings.bgColor;
        }
    } else {
        return {};  // Darkness at the end of rays :-)
    }
}

void RenderWhitted(const Scene &scene, FrameBuffer<Color> &rgbfr) {
#pragma omp parallel for
    for(int x = 0; x < scene.camera->film.resolution.x; ++x) {
        // The progress indicator is crazy with OpenMP but I'll not waste time on synchronization.
        fprintf(stderr, "x = %5d %5.2f%%\r", x, 100. * (x + 1) / scene.camera->film.resolution.x);
        for(int y = 0; y < scene.camera->film.resolution.y; ++y) {
            Ray ray            = scene.camera->GenerateRay({x + (Float)0.5, y + (Float)0.5});
            Color color = WhittedIntegrate(scene, ray, 10);

/*            const uint8_t c255 = std::numeric_limits<uint8_t>::max();

            rgbfr[x][y] = {(uint8_t)(color.r * c255), (uint8_t)(color.g * c255),
                           (uint8_t)(color.b * c255)};*/
            rgbfr[x][y] = color;

        }
    }
}

void Render(const Scene &scene, FrameBuffer<Color> &rgbfr) {
    if((0 == scene.pointLights.size()) && (0 == scene.parallelLights.size())) {
        // Darkness rules the land. Visualize the underlying maths :-)
        RenderNormals(scene, rgbfr);
    } else {
        RenderWhitted(scene, rgbfr);
    }
}

