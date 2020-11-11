class TEXT_FILE_BUFFER{
public:
    TEXT_FILE_BUFFER( const char* fileName, int usePath=1, int maxStrLen=1024 );//テキストファイルを一気にBufferに読み込む。
    ~TEXT_FILE_BUFFER();
    const char* bufferPointer();
    const char* readLine();
    int end();
    void restart();
    unsigned size();
private:
    char* Buffer;//ファイルバッファ
    const char* BufferPointer;//ファイルバッファのポインタ。これから読み取る文字を指している。
    char* String;//取り出した文字列
    unsigned Size;//バッファサイズ
};


