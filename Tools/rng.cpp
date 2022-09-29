#include <rng.h>

#include <mt19937int.h>

#include <algorithm>

namespace RNG
{
  
struct rng_core
{
  rng_core()
  {
    LoadSeed(0);
  }
  
  void LoadSeed(unsigned long s)
  {
    seed = s;
    init_genrand(seed);
  }
  
  unsigned long seed;
};

static rng_core rng_source;

void SetSeed(unsigned long s)
{
  rng_source.LoadSeed(s);
}

unsigned long GetSeed()
{
  return rng_source.seed;
}

///RNG functions

unsigned long   Rand()
{
  return genrand_int32();
}

long   Randl()
{
  return genrand_int31();
}

int   Randi()
{
  const int n = Randl();
  return n;
}

double Randd()
{
  return genrand_real1();
}

float  Randf()
{
  const float n = genrand_real1();
  return n;
}

unsigned long   Rand(unsigned long a)
{
  const unsigned long n = Rand()%a;
  return n;
}

unsigned long   Rand(unsigned long a, unsigned long b)
{
  if( a == 0 ) return Rand(b);
  if( a > b ) std::swap(a,b);
  
  const unsigned long long n = 2*b;
  const unsigned long long m = a+b;
  const unsigned long c = n % m;
  return a + Rand()%c;
}

long   Rand(long a)
{
  const long n = Randl()%a;
  return n;
}

long   Rand(long a, long b)
{
  if( a > b ) std::swap(a,b);
  
  const  long c = b - a;
  return a + Randl()%c;
}

double Rand(double a)
{
  const double n = Randd()*a;
  return n;
}

double Rand(double a, double b)
{
  if( a > b ) std::swap(a,b);
  
  const  double c = b - a;
  return a + Randd()*c;
}

int Rand(int a)
{
  const int n = Randi()%a;
  return n;
}

int Rand(int a, int b)
{
  if( a > b ) std::swap(a,b);
  
  const  int c = b - a;
  return a + Randi()%c;
}

float Rand(float a)
{
  const float n = Randf()*a;
  return n;
}

float Rand(float a, float b)
{
  if( a > b ) std::swap(a,b);
  
  const  float c = b - a;
  return a + Randf()*c;
}

unsigned Rand(unsigned a)
{
  const unsigned n = Rand()%a;
  return n;
}

unsigned Rand(unsigned a, unsigned b)
{
  if( a == 0 ) return Rand(b);
  if( a > b ) std::swap(a,b);
  
  const unsigned long long n = 2*b;
  const unsigned long long m = a+b;
  const unsigned long c = n % m;
  return a + Rand()%c;
}








}