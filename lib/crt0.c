#include "syscall.h"

// Prototype for the main function (created in user code
void main(void);

void _start(void)
{
    // Call main in user code
    main();
}
