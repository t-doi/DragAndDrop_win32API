
#include <windows.h>
#include <shellapi.h>

//�}�N���ƃO���[�o���ϐ�---------------------------------------------------------------------------

#define	CLASS_NAME	"Drag & Drop Sample"	//�N���X��
#define	MAX_DROP	100						//�h���b�O���h���b�v�Ŏ󂯎���t�@�C���̍ő吔

char*			g_pDropFiles[ MAX_DROP ];	//�h���b�O���ꂽ�t�@�C�������i�[
unsigned int	g_dropCount = 0;			//�h���b�O���ꂽ�t�@�C������ۑ�

//�v���g�^�C�v�錾---------------------------------------------------------------------------------

LRESULT CALLBACK WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void DrawMessage( HWND hWnd );
void GetDropFiles( char** ppDropFileList, unsigned int listSize, unsigned int* pDropFileCount, WPARAM wParam );
void ShowDropFiles( char** ppDropFileList, unsigned int dropFileCount );
void EraseDropFiles( char** ppDropFileList, unsigned int dropFileCount );

//���C��-------------------------------------------------------------------------------------------

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	//�E�B���h�E�N���X�ݒ聕�o�^
	{
		WNDCLASS	wc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hbrBackground	= ( HBRUSH )GetStockObject( WHITE_BRUSH );
		wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
		wc.hIcon			= LoadIcon( NULL, IDI_WINLOGO );
		wc.hInstance		= hInstance;
		wc.lpfnWndProc		= WinProc;
		wc.lpszClassName	= CLASS_NAME;
		wc.lpszMenuName		= NULL;
		wc.style			= 0;
		if( !RegisterClass( &wc ) ) return 1;
	}

	//�E�B���h�E�쐬
	HWND hWnd = CreateWindow( CLASS_NAME, CLASS_NAME, WS_CAPTION|WS_SYSMENU,
					0, 0, 260, 100, NULL, NULL, hInstance, NULL );
	if( hWnd == NULL ) return 1;

	//�E�B���h�E�\��
	ShowWindow( hWnd, SW_SHOW );
	UpdateWindow( hWnd );

	//���b�Z�[�W���[�v
	while( true )
	{
		MSG	msg;
		if( GetMessage( &msg, NULL, 0, 0 ) <= 0 ) break;
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return 0;
}

//�󂯎�������b�Z�[�W�̑Ή�----------------------------------------------------------------------

LRESULT CALLBACK WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_DESTROY:

		PostQuitMessage( 0 );
		break;

	case WM_PAINT:

		DrawMessage( hWnd );
		break;

	case WM_CREATE:
		
		//�h���b�O���h���b�v��L���ɂ���
		DragAcceptFiles( hWnd, TRUE );
		break;

	case WM_DROPFILES:

		//�h���b�v���ꂽ�t�@�C�����擾�E�\���E���
		GetDropFiles( (char**)&g_pDropFiles, MAX_DROP, &g_dropCount, wParam );
		ShowDropFiles( (char**)&g_pDropFiles, g_dropCount );
		EraseDropFiles( (char**)&g_pDropFiles, g_dropCount );
		break;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//--------------------------------------------------------------------------------------------------
//	WM_PAINT ���b�Z�[�W�̒��ł̕`�揈��
//--------------------------------------------------------------------------------------------------

void DrawMessage( HWND hWnd )
{
	// WM_PAINT �̒��ł� HDC �̎擾�� PAINTSTRUCT �� BeginPaint() ���g��
	PAINTSTRUCT ps;
	BeginPaint( hWnd, &ps );
	
	char message[256];

	wsprintf( message, "���̃E�B���h�E�Ƀt�@�C����" );
	TextOut( ps.hdc, 10, 10, message, lstrlen( message ) );
	
	wsprintf( message, "�h���b�O���h���b�v���ĉ������B" );
	TextOut( ps.hdc, 10, 40, message, lstrlen( message ) );

	// PAINTSTRUCT �̉��
	EndPaint( hWnd, &ps );
}

//--------------------------------------------------------------------------------------------------
//	�h���b�O���h���b�v���ꂽ�t�@�C�����擾
//--------------------------------------------------------------------------------------------------

void GetDropFiles( char** ppDropFileList, unsigned int listSize, unsigned int* pDropFileCount, WPARAM wParam )
{
	//�h���b�v���ꂽ�t�@�C�������ꎞ�I�ɕۑ����镶���z��
	char fileName[ MAX_PATH ];

	//���X�g���N���A
	unsigned int i;
	for( i = 0; i < listSize; i++ ) *( ppDropFileList + i ) = NULL;

	//�h���b�v���ꂽ�t�@�C�������擾
	*pDropFileCount = DragQueryFile( ( HDROP )wParam, 0xffffffff, NULL, NULL );
	if( *pDropFileCount > listSize ) *pDropFileCount = listSize;

	for( i = 0; i < *pDropFileCount; i++ )
	{
		//�i�[�p�z��̒l��������
		memset( fileName, '\0', sizeof( fileName ) );

		//�h���b�v���ꂽ�t�@�C�����擾
		DragQueryFile( (HDROP)wParam, i, fileName, sizeof( fileName ) );
		
		//�t�@�C�����i�[�p�̃��������m��
		char* p_file = ( char* )GlobalAlloc( GPTR, strlen( fileName ) + 1 );
		if( p_file == NULL ) return;

		//�m�ۂ����������Ƀt�@�C�������R�s�[
		strcpy( p_file, fileName );

		//�h���b�v���ꂽ�t�@�C�����̃��X�g�ɒǉ�
		*( ppDropFileList + i ) = p_file;
	}

	//�h���b�O���h���b�v����I��
	DragFinish( ( HDROP )wParam );
}

//--------------------------------------------------------------------------------------------------
//	�h���b�v���ꂽ�t�@�C�����擾���Ɋm�ۂ��������������
//--------------------------------------------------------------------------------------------------

void EraseDropFiles( char** ppDropFileList, unsigned int dropFileCount )
{
	unsigned int i;
	for( i = 0; i < dropFileCount; i++ )
	{
		if( *( ppDropFileList + i ) != NULL )
		{
			GlobalFree( *( ppDropFileList + i ) );
			*( ppDropFileList + i ) = NULL;
		}
	}
}

//--------------------------------------------------------------------------------------------------
//	�h���b�v���ꂽ�t�@�C������\��
//--------------------------------------------------------------------------------------------------

void ShowDropFiles( char** ppDropFileList, unsigned int dropFileCount )
{
	unsigned int i;
	for( i = 0; i < dropFileCount; i++ )
	{
		MessageBox( NULL, *( ppDropFileList + i ), "�h���b�v���ꂽ�t�@�C��", MB_OK );
	}
}
