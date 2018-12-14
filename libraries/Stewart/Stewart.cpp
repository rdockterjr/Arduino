/**********************************************************************************************
 * Arduino PID Library - Version 1.0.1
 * by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <Stewart.h>
#include <../Lookup_Math/Lookup_Math.h>

/*Constructor (...)*********************************************************
 ***************************************************************************/
Stewart::Stewart(void)
{
  init_sine();
  init_arcsine();
}

Stewart::Stewart(float theta_b,float theta_p,float r_base,float r_p,float arm_length,float shaft_length)
{
  init_sine();
  init_arcsine();
  set_constants( theta_b, theta_p, r_base, r_p, arm_length, shaft_length);
}

void Stewart::set_constants(float theta_b,float theta_p,float r_base,float r_p,float arm_length,float shaft_length)
{
  for(int i=1;i<=6;i++)
  {
    if(i%2==0)
    {
      v[i]=v[i-1]+theta_b;
      lambda[i]=lambda[i-1]+theta_p;
    }else{
      v[i]=i*M_PI/3.0-theta_b/2;
      lambda[i]=i*M_PI/3-theta_p/2;
    }
    Bx[i]=r_base*cos(v[i]);
    By[i]=r_base*sin(v[i]);
    GTx[i]=r_p*cos(lambda[i]);
    GTy[i]=r_p*sin(lambda[i]);
  }
  //These just make the trig of our arm angle a bit simpler.
  float A=-shaft_length*shaft_length+arm_length*arm_length;
  float B=2.0*arm_length;
  C=A/B;
  D=1/B;
}
 
/* Compute() **********************************************************************
 **********************************************************************************/ 
float Stewart::get_angle(int index,Stewart_target &target)
{
  // Since the source I used for the math used 1-indexed
  // arrays, this seemed easier:
  int i=index+1;
  // Calculate the vector from the base end to the top end
  // of the imaginary straight link:
  float Lx=target.P_x-Bx[i]+GTx[i]*target.r11+GTy[i]*target.r12;
  float Ly=target.P_y-By[i]+GTx[i]*target.r21+GTy[i]*target.r22;
  float Lz=target.P_z      +GTx[i]*target.r31+GTy[i]*target.r32;
  // Euclidean distance in 3D:
  float length=sqrt(sq(Lx)+sq(Ly)+sq(Lz));

  /* If we were using pistons, we would be done.
     But we are not, so we need to find out the servo angle */

  // Find the angle of the arm to give the length:
  float cos_beta=C/length+D*length;
  // Check for invalidity for acos:
  if(cos_beta>1.0)
  {
    // Too long!  Can't reach!
    // Maybe set cos_beta=1.0
    // Or throw an error?
  }
  else if(cos_beta<-1.0)
  {
    // Too short!  Can't reach!
    // Maybe set cos_beta=-1.0
    // Or throw an error?
  }else{
    // Finish finding the angle:
    float beta=lookup_arccosine(cos_beta);
    // Find the angle from vertical of the imaginary link:
    float phi=lookup_arccosine(Lz/length);
    float theta=beta-phi;
    return theta;
  }
  return -100.0;
}

Stewart::Stewart_target Stewart::new_target(float x,float y,float z,float roll,float pitch,float yaw)
{
  Stewart::Stewart_target target;
  target.P_x=x;
  target.P_y=y;
  target.P_z=z;
  target.r11=           lookup_cosine(pitch)*lookup_cosine(yaw);
  target.r21=           lookup_cosine(pitch)*lookup_sine(  yaw);
  target.r31=          -lookup_sine(  pitch);
  target.r12= lookup_sine(  roll)*lookup_sine(  pitch)*lookup_cosine(yaw)-lookup_cosine(roll)*lookup_sine(  yaw);
  target.r22= lookup_sine(  roll)*lookup_sine(  pitch)*lookup_sine(  yaw)+lookup_cosine(roll)*lookup_cosine(yaw);
  target.r32= lookup_sine(  roll)*lookup_cosine(pitch);
  
  return target;
}


