#include <math.h>
#include "vector3.h"
#include "quaternion.h"     

QUATERNION::QUATERNION( float* d ):
w( d[ 0 ] ),
x( d[ 1 ] ),
y( d[ 2 ] ),
z( d[ 3 ] ){
}

QUATERNION::QUATERNION( float a, float b, float c, float d ):
w( a ),
x( b ),
y( c ),
z( d ){
}

void QUATERNION::create( float radian, float axisX, float axisY, float axisZ ){
    //axis正規化
    float n = axisX *  axisX +  axisY *  axisY +  axisZ *  axisZ;
    if( n ){
        n = 1.0f / sqrtf( n );
    }
    float ax = axisX * n;      
    float ay = axisY * n;      
    float az = axisZ * n;      

    float s = sinf( radian * 0.5f );
    w = cosf( radian * 0.5f );
    x = s * ax;
    y = s * ay;
    z = s * az;
}

void QUATERNION::create( float radian, const VECTOR3 &axis ){
    create( radian, axis.x, axis.y, axis.z );
}

//この方法で抽出したPitch Yaw Rollでマトリックスを作る場合はzyxの順でmulすること。
float QUATERNION::getPitch(){//x
    return atan2f( 2.0f * ( y*z + w*x ), w*w - x*x - y*y + z*z );
}
float QUATERNION::getYaw(){//y
    //return atan2f( -2.0f * ( x*z - w*y ), 1.0f - 2.0f * ( y*y + z*z ) );
    return asinf( -2.0f * ( x*z - w*y ) );
}
float QUATERNION::getRoll(){//z
    return atan2f( 2.0f * ( x*y + w*z ), w*w + x*x - y*y - z*z );
}

MATRIX QUATERNION::getMatrix(){
    MATRIX m;

    m._11 = 1.0f - 2.0f * ( y*y + z*z );
    m._12 = 2.0f * ( x*y - w*z );
    m._13 = 2.0f * ( x*z + w*y );
    
    m._21 = 2.0f * ( x*y + w*z );
    m._22 = 1.0f - 2.0f * ( x*x + z*z );
    m._23 = 2.0f * ( y*z - w*x );
    
    m._31 = 2.0f * ( x*z - w*y );
    m._32 = 2.0f * ( y*z + w*x );
    m._33 = 1.0f - 2.0f * ( x*x + y*y );
    
    return m;
}

void quaternionSlerp(QUATERNION* q, const QUATERNION& q0, const QUATERNION& q1, const float w){
    float qq = q0.w * q1.w + q0.x * q1.x + q0.y * q1.y + q0.z * q1.z;
    if( qq > 1.0 ){
        qq = 1.0;//補正
    }
    float ss = 1.0f - qq * qq;

    if( ss == 0.0 ){
        q->w = q0.w;
        q->x = q0.x;
        q->y = q0.y;
        q->z = q0.z;
    }
    else{
        float sp = sqrtf( ss );
        float ph = acosf( qq );
        float pt = ph * w;
        float t1 = sinf( pt ) / sp;
        float t0 = sinf( ph - pt ) / sp;

        q->w = q0.w * t0 + q1.w * t1;
        q->x = q0.x * t0 + q1.x * t1;
        q->y = q0.y * t0 + q1.y * t1;
        q->z = q0.z * t0 + q1.z * t1;
    }
}

void quaternionSlerp( float* q, const float* q0, const float* q1, const float w ){
    quaternionSlerp( (QUATERNION*)q, (QUATERNION&)*q0, (QUATERNION&)*q1, w);
}

// Quaternionどうしの掛け算      
QUATERNION mulQuaternion( const QUATERNION& left, const QUATERNION& right ){    
    QUATERNION   ans;      
    float   d1, d2, d3, d4;      

    d1   =  left.w  * right.w;      
    d2   = -left.x  * right.x;      
    d3   = -left.y  * right.y;      
    d4   = -left.z  * right.z;      
    ans.w = d1 + d2 + d3 + d4; 

    d1   =  left.w  * right.x;      
    d2   =  right.w * left.x;      
    d3   =  left.y  * right.z;      
    d4   = -left.z  * right.y;      
    ans.x =  d1 + d2 + d3 + d4; 

    d1   =  left.w  * right.y;      
    d2   =  right.w * left.y;      
    d3   =  left.z  * right.x;      
    d4   = -left.x  * right.z;      
    ans.y =  d1 + d2 + d3 + d4; 

    d1   =  left.w * right.z;      
    d2   =  right.w * left.z;      
    d3   =  left.x * right.y;      
    d4   = -left.y * right.x;      
    ans.z =  d1 + d2 + d3 + d4; 
        
    return   ans;      
}

//右手座標バージョン
MATRIX quaternionToMatrix( const QUATERNION& q ){
  MATRIX m;
  m._11 = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
  m._12 = 2.0f * (q.x * q.y - q.w * q.z);
  m._13 = 2.0f * (q.x * q.z + q.w * q.y);
  m._21 = 2.0f * (q.x * q.y + q.w * q.z);
  m._22 = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
  m._23 = 2.0f * (q.y * q.z - q.w * q.x);
  m._31 = 2.0f * (q.x * q.z - q.w * q.y);
  m._32 = 2.0f * (q.y * q.z + q.w * q.x);
  m._33 = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
  return m;
}


/*
    float qq = q0[ 0 ] * q1[ 0 ] + q0[ 1 ] * q1[ 1 ] 
              + q0[ 2 ] * q1[ 2 ] + q0[ 3 ] * q1[ 3 ];
    if( qq > 1.0 ) qq = 1.0;//補正
    float ss = 1.0 - qq * qq;
    if (ss == 0.0) {
        q[ 0 ] = q0[ 0 ];
        q[ 1 ] = q0[ 1 ];
        q[ 2 ] = q0[ 2 ];
        q[ 3 ] = q0[ 3 ];
    }
    else {
        float sp = sqrt(ss);
        float ph = acos(qq);
        float pt = ph * w;
        float t1 = sin(pt) / sp;
        float t0 = sin(ph - pt) / sp;
        q[ 0 ] = q0[ 0 ] * t0 + q1[ 0 ] * t1;
        q[ 1 ] = q0[ 1 ] * t0 + q1[ 1 ] * t1;
        q[ 2 ] = q0[ 2 ] * t0 + q1[ 2 ] * t1;
        q[ 3 ] = q0[ 3 ] * t0 + q1[ 3 ] * t1;
    }
}
*/
/*
//// Make Rotational QUATERNION      
QUATERNION makeRotQuat( float radian, float AxisX, float AxisY, float AxisZ ){      
    QUATERNION   ans;      
    float   norm;      
    float   ccc, sss;      
        
    ans.w = ans.x = ans.y = ans.z = 0.0;      

    norm   = AxisX *  AxisX +  AxisY *  AxisY +  AxisZ *  AxisZ;      
    if(norm <= 0.0) return ans;      

    norm   = 1.0 / sqrt(norm);      
    AxisX   *= norm;      
    AxisY   *= norm;      
    AxisZ   *= norm;      

    ccc   = cos(0.5 * radian);      
    sss   = sin(0.5 * radian);      

    ans.w   = ccc;      
    ans.x   = sss * AxisX;      
    ans.y   = sss * AxisY;      
    ans.z   = sss * AxisZ;      

    return   ans;      
}      





//// Kakezan      
QUATERNION Kakezan(QUATERNION left,   QUATERNION right){      
    QUATERNION   ans;      
    float   d1, d2, d3, d4;      

    d1   =  left.w   * right.w;      
    d2   = -left.x * right.x;      
    d3   = -left.y * right.y;      
    d4   = -left.z * right.z;      
    ans.w = d1+ d2+ d3+ d4; 

    d1   =  left.w * right.x;      
    d2   =  right.w * left.x;      
    d3   =  left.y * right.z;      
    d4   = -left.z * right.y;      
    ans.x =  d1+ d2+ d3+ d4; 

    d1   =  left.w * right.y;      
    d2   =  right.w * left.y;      
    d3   =  left.z * right.x;      
    d4   = -left.x * right.z;      
    ans.y =  d1+ d2+ d3+ d4; 

    d1   =  left.w * right.z;      
    d2   =  right.w * left.z;      
    d3   =  left.x * right.y;      
    d4   = -left.y * right.x;      
    ans.z =  d1+ d2+ d3+ d4; 
        
    return   ans;      
}      
*/