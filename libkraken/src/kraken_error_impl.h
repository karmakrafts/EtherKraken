#ifndef LIBKRAKEN_KRAKEN_ERROR_IMPL_H
#define LIBKRAKEN_KRAKEN_ERROR_IMPL_H

void kraken_last_error_set(const char* error);

#ifdef KRAKEN_DEBUG
#define KRAKEN_CHECK(c, E, m)                                                                                   \
    do {                                                                                                               \
        if(!(c)) {                                                                                                   \
            kraken_last_error_set(m);                                                                                  \
            return E;                                                                                                  \
        }                                                                                                              \
    } while(0)

#define KRAKEN_CHECK_RESULT(r, E, m)                                                                                   \
    do {                                                                                                               \
        if(r != 0) {                                                                                                   \
            kraken_last_error_set(m);                                                                                  \
            return E;                                                                                                  \
        }                                                                                                              \
    } while(0)

#define KRAKEN_CHECK_PTR(p, E, m)                                                                                      \
    do {                                                                                                               \
        if(p == nullptr) {                                                                                             \
            kraken_last_error_set(m);                                                                                  \
            return E;                                                                                                  \
        }                                                                                                              \
    } while(0)
#else
#define KRAKEN_CHECK(c, E, m)
#define KRAKEN_CHECK_RESULT(r, E, m)
#define KRAKEN_CHECK_PTR(p, E, m)
#endif

#endif//LIBKRAKEN_KRAKEN_ERROR_IMPL_H