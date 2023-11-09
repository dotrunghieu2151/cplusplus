#include <helpers.hpp>

#define TREE_DEBUG 0

#if TREE_DEBUG == 1
#define TREE_DEBUG_MS(mes)                                                     \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define TREE_DEBUG_MS(mes)                                                     \
  do {                                                                         \
  } while (0)
#endif
