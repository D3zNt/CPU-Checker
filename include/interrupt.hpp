#ifndef EXT_INTERRUPT_HPP
#define EXT_INTERRUPT_HPP

#include <csignal>

extern volatile sig_atomic_t INTERRUPT_STATUS;
void signal_handler(int signal);

#endif