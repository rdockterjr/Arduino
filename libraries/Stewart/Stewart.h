#ifndef Stewart_h
#define Stewart_h
#define LIBRARY_VERSION	1.0.0



class Stewart
{


  public:

    struct Stewart_target{
      float P_x;
      float P_y;
      float P_z;
      float r11;
      float r12;
      float r21;
      float r22;
      float r31;
      float r32;
    };

    Stewart(void);
    Stewart(float theta_b,float theta_p,float r_base,float r_p,float arm_length,float shaft_length);
    void set_constants(float theta_b,float theta_p,float r_base,float r_p,float arm_length,float shaft_length);
    float get_angle(int index,Stewart_target &target);
    Stewart::Stewart_target new_target(float x,float y,float z,float roll,float pitch,float yaw);
  private:
    float v[7];
    float lambda[7];
    float Bx[7];
    float By[7];
    float GTx[7];
    float GTy[7];
    float C,D;
};
#endif

