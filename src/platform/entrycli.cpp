//-----------------------------------------------------------------------------
// Our main() function for the command-line interface.
//
// Copyright 2022 ruevs
//-----------------------------------------------------------------------------
#include "jartsy.h"
#include "framebuffer/framebuffer.h"
#include "exportfile/exportpgm.h"

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
#if 0
            Camera camera = {};
            camera.pixelRatio = 1;
            camera.gridFit    = true;
            camera.width      = width;
            camera.height     = height;
            camera.projUp     = projUp;
            camera.projRight  = projRight;

            SS.GW.projUp      = projUp;
            SS.GW.projRight   = projRight;
            SS.GW.scale       = SS.GW.ZoomToFit(camera);
            camera.scale      = SS.GW.scale;
            camera.offset     = SS.GW.offset;
            SS.GenerateAll();

            CairoPixmapRenderer pixmapCanvas;
            pixmapCanvas.antialias = true;
            pixmapCanvas.SetLighting(SS.GW.GetLighting());
            pixmapCanvas.SetCamera(camera);
            pixmapCanvas.Init();

            pixmapCanvas.StartFrame();
            SS.GW.Draw(&pixmapCanvas);
            pixmapCanvas.FlushFrame();
            pixmapCanvas.FinishFrame();
            pixmapCanvas.ReadFrame()->WritePng(output, /*flip=*/true);

            pixmapCanvas.Clear();
#endif
            FrameBuffer<int> frame(1024, 768);
            frame[80][90] = 7;
            fprintf(stderr, "Doing nothing very quickly %i.\n", frame[80][90]);

            // Just a stupid hack to generate the two images for homework two.
            // To generate the images run:
            //    jartsy.exe render -o %%.ppm --size 1024x768 rectangles circle
            static int homeworkimage;

            const size_t xr = 1024;
            const size_t yr = 768;
            FrameBuffer<RGBColor> rgbfr(xr, yr);

            if(0 == homeworkimage) {
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
                        pix.blue = (std::numeric_limits<uint8_t>::max() * rand()) / RAND_MAX;   // blue is random
                        rgbfr[x][y] = pix;
                    }
                }
            }

            if(1 == homeworkimage) {
                // coordinates of the center of the circle
                const size_t xc = xr/2;
                const size_t yc = yr/2;
                // radius suqared
                const size_t rsq = min(xr, yr) * min(xr, yr) / (4*4);
                for(int x = 0; x < xr; ++x) {
                    for(int y = 0; y < yr; ++y) {
                        if((x - xc)*(x-xc) + (y - yc)*(y-yc) < rsq) {
                            // we are in the circle
                            rgbfr[x][y] = {57, 119, 34};   // circle color from my screen shot.
                        } else {
                            rgbfr[x][y] = {183, 183, 183};  // because that is what the backgroud is in my screen shot :-)
                        }
                    }
                }
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
