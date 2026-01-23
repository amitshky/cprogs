#ifndef HANDLERS_H

#include <termios.h>

void* handle_input(void* state);
void handle_sigint();

void restore_terminal();

#endif // HANDLERS_H
