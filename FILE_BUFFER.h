class FILE_BUFFER{
public:
    FILE_BUFFER();
    FILE_BUFFER( const char* fileName );
    ~FILE_BUFFER();
	void setBuffer( const char* fileName );//�t�@�C������C��Buffer�ɓǂݍ���
    //�o�b�t�@�|�C���^��i�߂�n�i�o�b�t�@�|�C���^�͓ǂݎ��ʒu���w���j
    const char* readString();
    const char* readLine();
    void readOnAssumption( char* s );
    float readFloat();
    int readInt();
    unsigned readUnsigned();
    void skipNode();
    void restart();
    bool end();
    //�擾�n
    const char* buffer() const;
    int size();
    const char* string() const;
    const char* bufferPointer() const;
    //�I�[�o�[���[�h
    bool operator==( const char* str );
    bool operator!=( const char* str );
    bool operator==( char c );
    bool operator!=( char c );
private:
    char* Buffer;//�t�@�C���o�b�t�@
    int Size;//�o�b�t�@�T�C�Y
    const char* BufferPointer;//�t�@�C���o�b�t�@�̃|�C���^
    char String[ 256 ];//���o����������
};


