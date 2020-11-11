class FILE_BUFFER{
public:
    FILE_BUFFER();
    FILE_BUFFER( const char* fileName );
    ~FILE_BUFFER();
	void setBuffer( const char* fileName );//ファイルを一気にBufferに読み込む
    //バッファポインタを進める系（バッファポインタは読み取り位置を指す）
    const char* readString();
    const char* readLine();
    void readOnAssumption( char* s );
    float readFloat();
    int readInt();
    unsigned readUnsigned();
    void skipNode();
    void restart();
    bool end();
    //取得系
    const char* buffer() const;
    int size();
    const char* string() const;
    const char* bufferPointer() const;
    //オーバーロード
    bool operator==( const char* str );
    bool operator!=( const char* str );
    bool operator==( char c );
    bool operator!=( char c );
private:
    char* Buffer;//ファイルバッファ
    int Size;//バッファサイズ
    const char* BufferPointer;//ファイルバッファのポインタ
    char String[ 256 ];//取り出した文字列
};


