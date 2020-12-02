#ifndef _VC_H
#define _VC_H

#define VC_REGS 0xA8470000

/**
 * @brief Provides a mechanism for force cleaning calls
 * from a function in the vc emulator.
 * @param f The function to attempt to clean calls from.
 */
#ifdef WIIVC
#define FORCE_CLEAN(f)                                  \
    do {                                                \
        volatile void **t = (volatile void**)VC_REGS;   \
        *t = f;                                         \
    } while(0)
#else
#define FORCE_CLEAN(f) (void)0
#endif

#endif
