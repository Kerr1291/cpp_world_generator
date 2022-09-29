#include "helpers.h"

#include <math.h>


namespace kc
{ 
  DebugWriter& GetDebugWriter()
  {
    static DebugWriter debug_writer;
    return debug_writer;
  }
  
  NullWriter& GetNullWriter()
  {
    static NullWriter null_writer;
    return null_writer;
  }
}
 
namespace Tools
{
  
bool FloatsEqual(float one, float two)
{
  float result = std::fabs(one) - std::fabs(two);
        result = std::fabs(result);

  return (result <= 0.0001f);
}

//JumpConsistentHash algorithm
// Input: 64-bit key, number of buckets
// Output: Bucket index
//Source: http://arxiv.org/abs/1406.2294v1
int JCH(unsigned long long key, int num_buckets)
{
  long long b = -1, j = 0;
  while( j < num_buckets ) {
    b = j;
    key = key * 2862933555777941757ULL + 1;
    j = (b + 1) * (double(1LL << 31) / double((key >> 33) + 1));
  }
  return b;
}  
  
  
  
  
}