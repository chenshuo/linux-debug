#include <stdlib.h>
#include <strings.h>

#include <map>

std::map<const void*, size_t> g_sizes;

extern "C"
{

void* zalloc(size_t size)
{
  void* ret = malloc(size);
  bzero(ret, size);
  g_sizes[ret] = size;
  return ret;
}
}
