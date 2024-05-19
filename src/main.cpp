#include "solitaire.h"
#include <doctest.h>

auto main(int argc, char** argv) -> int
{
    doctest::Context context;
    context.applyCommandLine(argc, argv);

    int res = context.run(); // run doctest

    // important - query flags (and --exit) rely on the user doing this
    if (context.shouldExit())
    {
        // propagate the result of the tests
        return res;
    }

    Solitaire app;
    app.run();

    return 0;
}
