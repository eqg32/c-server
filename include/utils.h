#ifndef UTILS
#define UTILS

#define allocptr(ptr, size)                                                   \
  do                                                                          \
    {                                                                         \
      ptr = malloc (size);                                                    \
      if (!ptr)                                                               \
        {                                                                     \
          perror ("malloc");                                                  \
          exit (1);                                                           \
        }                                                                     \
    }                                                                         \
  while (0)

#define allocptrt(ptr, type)                                                  \
  do                                                                          \
    {                                                                         \
      ptr = malloc (sizeof (type));                                           \
      if (!ptr)                                                               \
        {                                                                     \
          perror ("malloc");                                                  \
          exit (1);                                                           \
        }                                                                     \
    }                                                                         \
  while (0)

#endif
