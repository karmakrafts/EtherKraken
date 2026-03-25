#ifndef LIBKRAKEN_KRAKEN_API_H
#define LIBKRAKEN_KRAKEN_API_H

#ifdef __cplusplus
#define KRAKEN_API_BEGIN extern "C" {
#define KRAKEN_API_END }
#else
#define KRAKEN_API_BEGIN
#define KRAKEN_API_END
#endif

#define KRAKEN_EXPORT __attribute__((visibility("default")))

#endif //LIBKRAKEN_KRAKEN_API_H