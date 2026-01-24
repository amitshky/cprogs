#ifndef HANDLERS_H
#define HANDLERS_H

void* handle_input(void* p_state);

void handle_sigint();
void restore_terminal();

#endif // HANDLERS_H
