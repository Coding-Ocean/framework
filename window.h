void initWindow( const char* caption, int clientWidth, int clientHeight );
void initWindow( const char* caption );
bool msgProc();
bool ultraBasicMsgProc();
unsigned getTime();

extern int ClientWidth;
extern int ClientHeight;
extern float Aspect;
extern unsigned ActiveWindow;
extern int MouseDelta;

