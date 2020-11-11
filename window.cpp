#include "common.h"
#include "window.h"
#include "graphic.h"
#include "input.h"

extern int ClientWidth = 0;
extern int ClientHeight = 0;
extern float Aspect = 0;
extern unsigned ActiveWindow = 0;
extern int MouseDelta = 0;

//���b�Z�[�W����
LRESULT CALLBACK winProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp ) {	
    switch( msg ) {
    case WM_SIZE:
        if( wp == SIZE_RESTORED || wp == SIZE_MAXIMIZED ){
            ClientWidth  =  lp & 0xFFFF;
            ClientHeight = (lp >> 16) & 0xFFFF;
            changeSize();
        }
        return 0;
    case WM_KEYDOWN:
        if( wp == VK_ESCAPE ) {
            PostMessage( hwnd, WM_CLOSE, 0, 0 );
        }
        return 0;
    //case WM_MOUSEWHEEL:
        //MouseDelta = GET_WHEEL_DELTA_WPARAM( wp );
        //return 0;
    case WM_ACTIVATE:
        ActiveWindow = wp & 0xFFFF;
        return 0;
    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    default:
        return DefWindowProc( hwnd, msg, wp, lp );
    }
}

void initWindow( const char* caption, int clientWidth, int clientHeight ){
	HINSTANCE hinst = GetModuleHandle( 0 );
	//�E�B���h�E�N���X�����
    WNDCLASS wc = {0, winProc, 0, 0, hinst,
        LoadIcon( hinst, "MYICON" ),
        LoadCursor( 0, IDC_ARROW ),
		( HBRUSH )2, 0, CLASS_NAME };
    RegisterClass( &wc );

	//�E�B���h�E�����
    HWND hwnd = CreateWindow(
        CLASS_NAME, // �o�^����Ă���N���X��
        caption, // �E�B���h�E��
        WS_OVERLAPPEDWINDOW,// �E�B���h�E�X�^�C��
        //WS_POPUP,
        //WS_OVERLAPPED|WS_SYSMENU,// �E�B���h�E�X�^�C��
        50, // �E�B���h�E�̉������̈ʒu
        50, // �E�B���h�E�̏c�����̈ʒu
        128, // �E�B���h�E�̕��i�E�B���h�E�g�̕����܂ށj
        128, // �E�B���h�E�̍����i�^�C�g���o�[�ƃE�B���h�E�g�̍������܂ށj
        0, // �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
        0, // ���j���[�n���h���܂��͎q�E�B���h�E ID
        wc.hInstance, // �A�v���P�[�V�����C���X�^���X�̃n���h��
        0 // �E�B���h�E�쐬�f�[�^
    );

    //�E�B���h�E�������ɕ\�������悤��wx��wy���v�Z����B
    int desktopWidth  = GetSystemMetrics( SM_CXSCREEN );//�f�X�N�g�b�v�̕�
    int desktopHeight  = GetSystemMetrics( SM_CYSCREEN );//�f�X�N�g�b�v�̍���
    int wx = ( desktopWidth - clientWidth ) / 2;//�E�B���h�E�\���ʒux
    int wy = ( desktopHeight - clientHeight ) / 2;//�E�B���h�E�\���ʒuy
    //�N���C�A���g�̈�̑傫����clientWidth,clientHeight�ɂȂ�悤��ww��wh���v�Z����B
    RECT rect;
    GetWindowRect( hwnd, &rect );
    int ww = rect.right - rect.left;
    int wh = rect.bottom - rect.top;
    GetClientRect( hwnd, &rect );
    int cw = rect.right - rect.left;
    int ch = rect.bottom - rect.top;
    int fw = ww - cw;
    int fh = wh - ch;
    ww = clientWidth + fw;
    wh = clientHeight + fh;
    //�E�B���h�E�̈ʒu�Ƒ傫����ݒ肵�Ȃ���
    SetWindowPos( hwnd, 0, wx, wy, ww, wh, 0 );

	ClientWidth = clientWidth;
    ClientHeight = clientHeight;
    Aspect = ( float )clientWidth / clientHeight;

    //IME off
    HIMC hIMC = ImmGetContext( hwnd );
    ImmSetOpenStatus( hIMC, 0 );
    ImmReleaseContext( hwnd, hIMC );

    ShowWindow( hwnd, SW_SHOW );
}

void initWindow( const char* caption ){
	HINSTANCE hinst = GetModuleHandle( 0 );
	//�E�B���h�E�N���X�����
    WNDCLASS wc = {0, winProc, 0, 0, hinst,
        LoadIcon( hinst, "MYICON" ),
        LoadCursor( 0, IDC_ARROW ),
		( HBRUSH )2, 0, CLASS_NAME };
    RegisterClass( &wc );

	//�E�B���h�E�����
	ClientWidth = GetSystemMetrics( SM_CXSCREEN );
    ClientHeight = GetSystemMetrics( SM_CYSCREEN );
    Aspect = ( float )ClientWidth / ClientHeight;

    HWND hwnd = CreateWindow(
        CLASS_NAME, // �o�^����Ă���N���X��
        caption, // �E�B���h�E��
        //WS_OVERLAPPEDWINDOW,// �E�B���h�E�X�^�C��
        WS_POPUP,// �E�B���h�E�X�^�C��
        //WS_OVERLAPPED|WS_SYSMENU,// �E�B���h�E�X�^�C��
        0, // �E�B���h�E�̉������̈ʒu
        0, // �E�B���h�E�̏c�����̈ʒu
        ClientWidth, // �E�B���h�E�̕��i�E�B���h�E�g�̕����܂ށj
        ClientHeight, // �E�B���h�E�̍����i�^�C�g���o�[�ƃE�B���h�E�g�̍������܂ށj
        0, // �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
        0, // ���j���[�n���h���܂��͎q�E�B���h�E ID
        wc.hInstance, // �A�v���P�[�V�����C���X�^���X�̃n���h��
        0 // �E�B���h�E�쐬�f�[�^
    );

    //IME off
    HIMC hIMC = ImmGetContext( hwnd );
    ImmSetOpenStatus( hIMC, 0 );
    ImmReleaseContext( hwnd, hIMC );

    ShowWindow( hwnd, SW_SHOW );
}


unsigned getTime(){
    return timeGetTime();
}

bool msgProc() {
	MSG msg;
    if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    return ( msg.message != WM_QUIT );
}

bool ultraBasicMsgProc() {
    present();
    clearTarget();
    getInput();
	MSG msg;
    if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    return ( msg.message != WM_QUIT );
}


