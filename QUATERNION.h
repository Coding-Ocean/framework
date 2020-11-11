#ifndef QUATERNION_HD
#define QUATERNION_HD

#include "MATRIX.h"
class VECTOR3;

class QUATERNION{
public:
    float   w; // real-component      
    float   x; // x-component      
    float   y; // y-component
    float   z; // z-component
    QUATERNION( float* d );
    QUATERNION( float a=1, float b=0, float c=0, float d=0 );
    void create( float radian, const VECTOR3& axis );
    void create( float radian, float axisX, float axisY, float axisZ );
    float getPitch();
    float getYaw();
    float getRoll();
    MATRIX getMatrix();
};

void quaternionSlerp( QUATERNION* q, const QUATERNION& q0, const QUATERNION& q1, const float w );
void quaternionSlerp( float* q, const float* q0, const float* q1, const float w );//array version

QUATERNION mulQuaternion( const QUATERNION& left, const QUATERNION& right );
MATRIX quaternionToMatrix( const QUATERNION& q );

#endif