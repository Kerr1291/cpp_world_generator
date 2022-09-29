#pragma once


//default double/float funcitons generate [0-1] inclusive random number 
//all the rest are [x,y) with endpoint being noninclusive

namespace RNG
{
  void          SetSeed(unsigned long s);
  unsigned long GetSeed();
  
  unsigned long   Rand();
           long   Randl();
            int   Randi();
           
           double Randd();
           float  Randf();
           
  unsigned long   Rand(unsigned long a);
  unsigned long   Rand(unsigned long a, unsigned long b);
           long   Rand(         long a);
           long   Rand(         long a,          long b);
           double Rand(       double a);
           double Rand(       double a,        double b);
           
            int Rand(         int a);
            int Rand(         int a,          int b);
          float Rand(       float a);
          float Rand(       float a,        float b);
    unsigned    Rand(   unsigned  a);
    unsigned    Rand(   unsigned  a,    unsigned  b);
}