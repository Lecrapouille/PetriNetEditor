#include "Renderer/PetriEditor.hpp"
#include <iostream>
#include <unistd.h>

//------------------------------------------------------------------------------
static void usage(const char* name)
{
    std::cout
      << name << " [petri.json]" << std::endl
      << "Where:" << std::endl
      << "  [petri.json] is an optional Petri net file to load (i.e. examples/Howard1.json)" << std::endl
      << std::endl;
}

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Parse the command line
    std::string filename;
    opterr = 0;
    int opt;

    while ((opt = getopt(argc, argv, "h")) != -1)
    {
        switch (opt)
        {
            case 'h':
                usage(argv[0]);
                return EXIT_FAILURE;
            case '?':
                std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
                usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Parse optional arguments
    for (; optind < argc; optind++)
    {
        filename = argv[optind];
    }

    tpne::Editor editor(800, 600, "Petri Net Editor", filename);
    editor.run();
}
