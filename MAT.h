#ifndef MAT_HD
#define MAT_HD


class MAT{
public:
    float   _11,_12,_13,_14,
            _21,_22,_23,_24,
            _31,_32,_33,_34,
            _41,_42,_43,_44;
    MAT();

    void mulTranslate( float x, float y, float z );
    void mulScale( float x, float y, float z );
    void mulRotateZ( float r );
    void ortho( float l, float r, float b, float t, float n, float f );
    VECTOR3 operator*( const VECTOR3& v ) const;
};


#endif
