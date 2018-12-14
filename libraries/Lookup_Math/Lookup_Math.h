#ifndef Lookup_Math_h
#define Lookup_Math_h
#define LIBRARY_VERSION	1.0.0



/*!

  This is my attempt to make some decent estimations of
  trig functions with a minimal lookup table for use on
  the Arduino.  The assumption is that the trig doesn't
  need to be exact, since Arduino inputs/outputs are
  never more than 8 or 10 bits anyways.

  The tables should only take up 64 bytes, which for an
  Atmel ATmega328 with 2kb of SRAM is 3% of the RAM so
  it shouldn't interfere much with programs.

  */

#ifdef ARDUINO
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
#else
  ///If this isn't arduino, then standard c libraries apply
  #include <math.h>
#endif

//Everyone needs to know pi
#ifndef M_PI_2
#define M_PI_2   1.57079632679489661923	/* pi/2 */
#endif
#ifndef M_PI
#define M_PI     3.14159265358979323846	/* pi */
#endif
#ifndef M_3PI_2
#define M_3PI_2  4.71238898038	/* 3*pi/2 */
#endif
#ifndef M_2PI
#define M_2PI    6.28318530718	/* 2*pi */
#endif

///
#define TABLE_SIZE 24

/// \todo these could be unsigned ints, so we could go up to 2^16, but 2^15 seems fine.
int16_t sine_table[TABLE_SIZE];
int16_t arcsine_table[TABLE_SIZE];

void init_sine(void)
{
  for (int x = 0 ; x < TABLE_SIZE ; x++)
  {
    sine_table[x] = 32767.0 * sin( x * M_PI_2 / ( TABLE_SIZE - 1 ) );
  }
}

float interpolate_sine(float x)
{
  float x0=x*(TABLE_SIZE-1)/M_PI_2;
  if(x0>=TABLE_SIZE-1){ return sine_table[TABLE_SIZE-1]/ 32767.0;}
  if(x <=0           ){ return sine_table[0           ]/ 32767.0;}
  int x1 = floor(x0);
  float y1 = sine_table[x1]/ 32767.0;
  float y2 = sine_table[x1+1]/ 32767.0;
  return y1 + (y2-y1)*(x0-x1);
}

float lookup_sine(float x)
{
  while(x>M_2PI){x-=M_2PI;}
  while(x<0    ){x+=M_2PI;}
  if      (x < M_PI_2 ){ return  interpolate_sine(x      );}
  else if (x < M_PI   ){ return  interpolate_sine(M_PI -x);}
  else if (x < M_3PI_2){ return -interpolate_sine(x- M_PI);}
  else                 { return -interpolate_sine(M_2PI-x);}
  return 0;
}

float lookup_cosine(float x)
{
  return lookup_sine(x + M_PI_2);
}

float lookup_tan(float x)
{
  return (lookup_sine(x) / lookup_cosine(x));
}






void init_arcsine(void)
{
  for (int x = 0 ; x < TABLE_SIZE ; x++)
  {
    arcsine_table[x] = 32767.0 / M_PI_2 * asin( x / ( TABLE_SIZE - 1.0 ) );
  }
}

float interpolate_arcsine(float x)
{
  float x0 = x*(TABLE_SIZE-1);
  int   x1 = floor(x0);
  float y1 = arcsine_table[x1  ] * M_PI_2 / 32767.0;
  float y2 = arcsine_table[x1+1] * M_PI_2 / 32767.0;
  return y1 + (y2-y1)*(x0-x1);
}

float lookup_arcsine(float x)
{

  if      (x < -1.0 ){ return -M_PI_2;}
  else if (x >  1.0 ){ return  M_PI_2;}
  else if (x <  0.0 ){ return -interpolate_arcsine(-x);}
  else               { return  interpolate_arcsine( x);}
  return 0;
}

float lookup_arccosine(float cosine)
{
  return M_PI_2 - lookup_arcsine(cosine);
}


#if 0
#define APPROX_TABLE_SIZE 32
int8_t sine_approx_table[APPROX_TABLE_SIZE];

float sine_approx_linear(float theta)
{
    return 0.6631490366f*theta+0.1136504277f;
}

void init_sine_approx(void)
{
    for(int i=0;i<APPROX_TABLE_SIZE;i++)
    {
        float theta=i*M_PI_2/(APPROX_TABLE_SIZE-1);
        sine_approx_table[i]=round((sin(theta)-sine_approx_linear(theta))*128/0.2);
    }
}
float sine_approx_first_quadrant(float theta)
{
    ///With linear interpolation gives best results, mean error=0.0003;
    int x1 = floor(theta*(APPROX_TABLE_SIZE-1)/M_PI_2);
    int8_t y1 = sine_approx_table[x1];
    int8_t y2 = sine_approx_table[x1+1];
    return (y1 + (y2-y1)*(theta*(APPROX_TABLE_SIZE-1)/M_PI_2-x1))*0.2/128+sine_approx_linear(theta);

    ///This is without linear interpolation, and it still gives a decent number, mean error=0.0050
    //return sine_approx_table[(int)(theta*APPROX_TABLE_SIZE/M_PI_2)]*0.2/128+sine_approx_linear(theta);

    ///This is terrible, don't use this one mean error=0.0544
    //return sine_approx_linear(theta);
}

float sine_approx(float theta)
{
    while(theta>M_2PI)
    {
      theta-=M_2PI;
    }
    while(theta<0)
    {
      theta+=M_2PI;
    }
    if (theta <  M_PI_2)
    {
      return  sine_approx_first_quadrant(       theta);
    }
    else if (theta < M_PI)
    {
      return  sine_approx_first_quadrant(M_PI-theta);
    }
    else if (theta < M_3PI_2)
    {
      return -sine_approx_first_quadrant(theta-M_PI);
    }
    else
    {
      return -sine_approx_first_quadrant(M_2PI-theta);
    }
    return 0;
}

float cosine_approx(float x)
{
  return sine_approx(x + M_PI_2);
}

float tan_approx(float x)
{
  return (sine_approx(x) / cosine_approx(x));
}

float sine_approx_error_test(int N=10000)
{
    init_sine_approx();
    double error=0;
    for(float theta=-3*M_PI;theta<3*M_PI;theta+=6*M_PI/N)
    {
        float exact_sine=sin(theta);
        float approx_sine=lookup_sine(theta);
        float err=approx_sine-exact_sine;
        if(err<0)
            error-=err;
        else
            error+=err;
    }
    return error/N;
}

float cosine_approx_error_test(int N=10000)
{
    init_sine_approx();
    double error=0;
    for(float theta=-3*M_PI;theta<3*M_PI;theta+=6*M_PI/N)
    {
        float exact_cosine=cos(theta);
        float approx_cosine=cosine_approx(theta);
        float err=approx_cosine-exact_cosine;
        if(err<0)
            error-=err;
        else
            error+=err;
    }
    return error/N;
}






int8_t arcsine_approx_table[APPROX_TABLE_SIZE];

float arcsine_approx_linear(float theta)
{
    return 1.121710044f*theta-0.0221754685f;
}


void init_arcsine_approx(void)
{
    for(int i=0;i<APPROX_TABLE_SIZE;i++)
    {
        float sine=i*1.0/(APPROX_TABLE_SIZE-1);
        arcsine_approx_table[i]=round((asin(sine)-arcsine_approx_linear(sine))*128/0.5);
    }
}

float arcsine_approx_first_quadrant(float sine)
{
    ///With linear interpolation gives best results, mean error=0.00235123
    int x1 = floor(sine*(APPROX_TABLE_SIZE-1));
    int8_t y1 = arcsine_approx_table[x1];
    int8_t y2 = arcsine_approx_table[x1+1];
    return (y1 + (y2-y1)*(sine*(APPROX_TABLE_SIZE-1)-x1))*0.5/128+arcsine_approx_linear(sine);

    ///This is without linear interpolation, and it still gives a decent number, mean error=0.00922342
    //return arcsine_approx_table[(int)(sine*APPROX_TABLE_SIZE)]*0.5/128+arcsine_approx_linear(sine);

    ///This is terrible, don't use this one, mean error=0.0416528
    //return arcsine_approx_linear(sine);
}

float arcsine_approx(float sine)
{
    if(sine<-1.0)
        return M_PI;
    if(sine>1.0)
        return 0.0;
    if(sine<0.0)
        return -arcsine_approx_first_quadrant(-sine);
    return arcsine_approx_first_quadrant(sine);
}

float arccosine_approx(float cosine)
{
  return M_PI_2 - arcsine_approx(cosine);
}

float arcsine_approx_error_test(int N=10000)
{
    init_arcsine_approx();
    double error=0;
    for(float sine=-1.0;sine<=1.0;sine+=2.0/N)
    {
        float exact_arcsine=asin(sine);
        float approx_arcsine=lookup_arcsine(sine);
        float err=approx_arcsine-exact_arcsine;
        if(err<0)
            error-=err;
        else
            error+=err;
    }
    return error/N;
}

float arccosine_approx_error_test(int N=10000)
{
    init_arcsine_approx();
    double error=0;
    for(float cosine=-1.0;cosine<=1.0;cosine+=2.0/N)
    {
        float exact_arccosine=acos(cosine);
        float approx_arccosine=arccosine_approx(cosine);
        float err=approx_arccosine-exact_arccosine;
        if(err<0)
            error-=err;
        else
            error+=err;
    }
    return error/N;
}
#endif


/* Assumes that float is in the IEEE 754 single precision floating point format
 * and that long is 32 bits. */
float sqrt_approx(float z)
{
    int32_t val_int = *(int32_t*)&z; /* Same bits, but as an int */
    /*
     * To justify the following code, prove that
     *
     * ((((val_int / 2^m) - b) / 2) + b) * 2^m = ((val_int - 2^m) / 2) + ((b + 1) / 2) * 2^m)
     *
     * where
     *
     * b = exponent bias
     * m = number of mantissa bits
     *
     * .
     */
 
    val_int -= 1 << 23; /* Subtract 2^m. */
    val_int >>= 1; /* Divide by 2. */
    val_int += 1 << 29; /* Add ((b + 1) / 2) * 2^m. */
 
    float result= *(float*)&val_int; /* Interpret again as float */

    ///Optionally, run it through babylonian for an improvement from ~2% error to ~.03% error
    /// However, this adds some float-float math which may be considerably slower than above.
    result=0.5*(result+z/result);

    return result;
}

float sqrt_approx_error_test(int N=10000)
{
    double error=0;
    int count=0;
    for(float x=0.001;x<N;x*=1.01123525)
    {
        float exact_sqrt=sqrt(x);
        float approx_sqrt=sqrt_approx(x);
        float err=(exact_sqrt-approx_sqrt)/exact_sqrt;
        if(err<0)
            error-=err;
        else
            error+=err;
        count++;
    }
    return error/count*100;
}

float sqrt_babylonian(float z,int N=4)
{
  //Use z as an initial guess
  float x=z;
  for(int i=0;i<N;i++)
  {
    x=0.5*(x+z/x);
  }
  return x;
}

#endif

