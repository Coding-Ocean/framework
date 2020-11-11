class BITMAP_RESOURCE{
public:
    BITMAP_RESOURCE( const char* resourceName );
    ~BITMAP_RESOURCE();
    unsigned width(){ return Width; }
    unsigned height(){ return Height; }
    unsigned char* pixels(){ return Pixels; }
private:
    HGLOBAL HMem;
    unsigned Width;
    unsigned Height;
    unsigned char* Pixels;
};