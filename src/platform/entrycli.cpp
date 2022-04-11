//-----------------------------------------------------------------------------
// Our main() function for the command-line interface.
//
// Copyright 2022 ruevs
//-----------------------------------------------------------------------------
#include "jartsy.h"
#include "framebuffer/framebuffer.h"
#include "exportfile/exportpgm.h"
#include "jartsyla.h"
#include "ray.h"
#include "film.h"
#include "camera.h"
#include "entities.h"

const char *PACKAGE_VERSION = "0.001";

static void ShowUsage(const std::string &cmd) {
    fprintf(stderr, "Usage: %s <command> <options> <filename> [filename...]", cmd.c_str());
//-----------------------------------------------------------------------------> 80 col */
    fprintf(stderr, R"(
    When run, performs an action specified by <command> on every <filename>.

Common options:
    -o, --output <pattern>
        For an input file <name>.jarts, replaces the '%%' symbol in <pattern>
        with <name> and uses it as output file. For example, when using
        --output %%-2d.ppm for input files f/a.jarts and f/b.jrts, output files
        f/a-2d.ppm and f/b-2d.ppm will be written.

Commands:
    version
        Prints the current JARTSy version.
    render --output <pattern> --size <size>
        Outputs a rendered view of the scene.
        <size> is <width>x<height>, in pixels.
)");

/* auto FormatListFromFileFilters = [](const std::vector<Platform::FileFilter> &filters) {
        std::string descr;
        for(auto filter : filters) {
            descr += "\n        ";
            descr += filter.name;
            descr += " (";
            bool first = true;
            for(auto extension : filter.extensions) {
                if(!first) {
                    descr += ", ";
                }
                descr += extension;
                first = false;
            }
            descr += ")";
        }
        return descr;
    };
*/

    fprintf(stderr, R"(
File formats:
    render:%s
)", ".pgm");
}

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
    const int nTria     = 3;
    const Point p[nTria*3] = {{-1.75, -1.75, -3},  {1.75, -1.75, -3}, {0, 1.75, -3},
                              {0, 0, -1}, {1, 0, 1}, {-1, 0, 1},
                              {0.56, 1.11, 1.23}, {0.44, -2.368, -0.54}, {-1.56, 0.15, -1.92}};
    const int vertexIndex[nTria*3] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    Normal n[nTria];
    TriangleMesh trMesh{nTria, p, vertexIndex, n};

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
    int vertexes[3]   = {0, 1, 2};
    Normal normals[1] = {};
    TriangleMesh mesh = {1, points, vertexes, normals};
    mesh.CalclulateNormals();

    for(int x = 0; x < xr; ++x) {
        for(int y = 0; y < yr; ++y) {
            Ray ray     = camera.GenerateRay({x + (Float)0.5, y + (Float)0.5});
            const uint8_t c255 = std::numeric_limits<uint8_t>::max();
            Intersection ints;
            if(mesh.Intersect(ray, &ints)) {
                rgbfr[x][y] = {c255, c255, c255};
            } else {
                rgbfr[x][y] = {(uint8_t)abs(ray.d.x * c255),
                               (uint8_t)abs(ray.d.y * c255),
                               (uint8_t)abs(ray.d.z * c255)};
            }
        }
    }
}

void HW5234(int xr, int yr, FrameBuffer<RGBColor> &rgbfr) {
    // Homework 5.2 5.3 5.3 triangle mesh intersection

    // Size of the camra film/senzor in meters (world scale)
    const Float xs  = 2.;
    const Float ys  = 2.;
    const Film film = {{xr, yr}, (Float)sqrt(Sqr(xs) + Sqr(ys))};

    const Transform cameraLocation = {
        {.0, .0, .0} /*transaltion*/, {} /*rotation*/, {1., 1., 1.} /*scale*/};
    const Float focalLength = 1;

    Camera camera(cameraLocation, film, focalLength);

    const int n        = 12; // triangles
    Point points[10]   = {{0., 0., -3.},   {1., 0., -3.},  {1., 1., -4.},
                         {0., 1., -3.},   {-1., 1., -4.}, {-1., 0., -3.},
                         {-1., -1., -4.}, {0., -1., -3.}, {1., -1., -4.}, {0., 0., -3.5}};
    int vertexes[n*3] = {
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
    Normal normals[n] = {};
    TriangleMesh mesh = {n, points, vertexes, normals};
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


static bool RunCommand(const std::vector<std::string> args) {
    if(args.size() < 2) return false;

    for(const std::string &arg : args) {
        if(arg == "--help" || arg == "-h") {
            ShowUsage(args[0]);
            return true;
        }
    }

    std::function<void(const Platform::Path &)> runner;

    std::vector<Platform::Path> inputFiles;
    auto ParseInputFile = [&](size_t &argn) {
        std::string arg = args[argn];
        if(arg[0] != '-') {
            inputFiles.push_back(Platform::Path::From(arg));
            return true;
        } else return false;
    };

    std::string outputPattern;
    auto ParseOutputPattern = [&](size_t &argn) {
        if(argn + 1 < args.size() && (args[argn] == "--output" ||
                                      args[argn] == "-o")) {
            argn++;
            outputPattern = args[argn];
            return true;
        } else return false;
    };

    unsigned width = 0, height = 0;
    if(args[1] == "version") {
        fprintf(stderr, "JARTSy version %s \n\n", PACKAGE_VERSION);
        return false;
    } else if(args[1] == "render") {
        auto ParseSize = [&](size_t &argn) {
            if(argn + 1 < args.size() && args[argn] == "--size") {
                argn++;
                if(sscanf(args[argn].c_str(), "%ux%u", &width, &height) == 2) {
                    return true;
                } else return false;
            } else return false;
        };

        for(size_t argn = 2; argn < args.size(); argn++) {
            if(!(ParseInputFile(argn) ||
                 ParseOutputPattern(argn) ||
                 ParseSize(argn))) {
                fprintf(stderr, "Unrecognized option '%s'.\n", args[argn].c_str());
                return false;
            }
        }

        if(width == 0 || height == 0) {
            fprintf(stderr, "Non-zero viewport size must be specified.\n");
            return false;
        }

        runner = [&](const Platform::Path &output) {
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

            PGMWriter::ExportFrameBufferTo(output, rgbfr);
        };
    } else if(args[1] == "something-else") {
		// Do something else
    } else {
        fprintf(stderr, "Unrecognized command '%s'.\n", args[1].c_str());
        return false;
    }

    if(outputPattern.empty()) {
        fprintf(stderr, "An output pattern must be specified.\n");
        return false;
    } else if(outputPattern.find('%') == std::string::npos && inputFiles.size() > 1) {
        fprintf(stderr,
                "Output pattern must include a %% symbol when using multiple inputs!\n");
        return false;
    }

    if(inputFiles.empty()) {
        fprintf(stderr, "At least one input file must be specified.\n");
        return false;
    }

    for(const Platform::Path &inputFile : inputFiles) {
        Platform::Path absInputFile = inputFile.Expand(/*fromCurrentDirectory=*/true);

        Platform::Path outputFile = Platform::Path::From(outputPattern);
        size_t replaceAt = outputFile.raw.find('%');
        if(replaceAt != std::string::npos) {
            Platform::Path outputSubst = inputFile.Parent();
            if(outputSubst.IsEmpty()) {
                outputSubst = Platform::Path::From(inputFile.FileStem());
            } else {
                outputSubst = outputSubst.Join(inputFile.FileStem());
            }
            outputFile.raw.replace(replaceAt, 1, outputSubst.raw);
        }
        Platform::Path absOutputFile = outputFile.Expand(/*fromCurrentDirectory=*/true);

/*
        JARTSy.Init();
        if(!JARTSy.LoadFromFile(absInputFile)) {
            fprintf(stderr, "Cannot load '%s'!\n", inputFile.raw.c_str());
            return false;
        }
        JARTSy.AfterNewFile();
*/
        runner(absOutputFile);
/*
        JARTSy.Clear();
*/

        fprintf(stderr, "Written '%s'.\n", outputFile.raw.c_str());
    }

    return true;
}

int main(int argc, char **argv) {
    std::vector<std::string> args = Platform::InitCli(argc, argv);

    if(args.size() == 1) {
        ShowUsage(args[0]);
        return 0;
    }

    if(!RunCommand(args)) {
        return 1;
    } else {
        return 0;
    }
}
