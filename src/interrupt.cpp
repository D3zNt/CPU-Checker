#include <interrupt.hpp>
#include <iostream>

volatile sig_atomic_t INTERRUPT_STATUS = 0;

void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "External interrupt by user, shutting down..." << std::endl;
        INTERRUPT_STATUS = 1;
    }
}
