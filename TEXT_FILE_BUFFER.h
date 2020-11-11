class TEXT_FILE_BUFFER{
public:
    TEXT_FILE_BUFFER( const char* fileName, int usePath=1, int maxStrLen=1024 );//�e�L�X�g�t�@�C������C��Buffer�ɓǂݍ��ށB
    ~TEXT_FILE_BUFFER();
    const char* bufferPointer();
    const char* readLine();
    int end();
    void restart();
    unsigned size();
private:
    char* Buffer;//�t�@�C���o�b�t�@
    const char* BufferPointer;//�t�@�C���o�b�t�@�̃|�C���^�B���ꂩ��ǂݎ�镶�����w���Ă���B
    char* String;//���o����������
    unsigned Size;//�o�b�t�@�T�C�Y
};


