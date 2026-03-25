#include "kraken_error.h"
#include "kraken_error_impl.h"

static const char* _Atomic g_last_error = nullptr;

KRAKEN_EXPORT const char* kraken_last_error_get() {
    return g_last_error;
}

KRAKEN_EXPORT void kraken_last_error_clear() {
    g_last_error = nullptr;
}

void kraken_last_error_set(const char* error) {
    g_last_error = error;
}