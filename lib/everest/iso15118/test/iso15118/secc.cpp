#include <cstdio>

#include <iso15118/tbd_controller.hpp>

int main(int argc, char* argv[]) {
    iso15118::TbdController controller;
    controller.loop();

    return 0;
}
