#ifndef HANDLERS_H

#include <termios.h>

void* handle_input(void* p_state);

void handle_sigint();
void restore_terminal();

#endif // HANDLERS_H
