
#include <windows.h>
#include <shellapi.h>

//マクロとグローバル変数---------------------------------------------------------------------------

#define	CLASS_NAME	"Drag & Drop Sample"	//クラス名
#define	MAX_DROP	100						//ドラッグ＆ドロップで受け取れるファイルの最大数

char*			g_pDropFiles[ MAX_DROP ];	//ドラッグされたファイル名を格納
unsigned int	g_dropCount = 0;			//ドラッグされたファイル数を保存

//プロトタイプ宣言---------------------------------------------------------------------------------

LRESULT CALLBACK WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void DrawMessage( HWND hWnd );
void GetDropFiles( char** ppDropFileList, unsigned int listSize, unsigned int* pDropFileCount, WPARAM wParam );
void ShowDropFiles( char** ppDropFileList, unsigned int dropFileCount );
void EraseDropFiles( char** ppDropFileList, unsigned int dropFileCount );

//メイン-------------------------------------------------------------------------------------------

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	//ウィンドウクラス設定＆登録
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

	//ウィンドウ作成
	HWND hWnd = CreateWindow( CLASS_NAME, CLASS_NAME, WS_CAPTION|WS_SYSMENU,
					0, 0, 260, 100, NULL, NULL, hInstance, NULL );
	if( hWnd == NULL ) return 1;

	//ウィンドウ表示
	ShowWindow( hWnd, SW_SHOW );
	UpdateWindow( hWnd );

	//メッセージループ
	while( true )
	{
		MSG	msg;
		if( GetMessage( &msg, NULL, 0, 0 ) <= 0 ) break;
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return 0;
}

//受け取ったメッセージの対応----------------------------------------------------------------------

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
		
		//ドラッグ＆ドロップを有効にする
		DragAcceptFiles( hWnd, TRUE );
		break;

	case WM_DROPFILES:

		//ドロップされたファイルを取得・表示・解放
		GetDropFiles( (char**)&g_pDropFiles, MAX_DROP, &g_dropCount, wParam );
		ShowDropFiles( (char**)&g_pDropFiles, g_dropCount );
		EraseDropFiles( (char**)&g_pDropFiles, g_dropCount );
		break;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//--------------------------------------------------------------------------------------------------
//	WM_PAINT メッセージの中での描画処理
//--------------------------------------------------------------------------------------------------

void DrawMessage( HWND hWnd )
{
	// WM_PAINT の中では HDC の取得に PAINTSTRUCT と BeginPaint() を使う
	PAINTSTRUCT ps;
	BeginPaint( hWnd, &ps );
	
	char message[256];

	wsprintf( message, "このウィンドウにファイルを" );
	TextOut( ps.hdc, 10, 10, message, lstrlen( message ) );
	
	wsprintf( message, "ドラッグ＆ドロップして下さい。" );
	TextOut( ps.hdc, 10, 40, message, lstrlen( message ) );

	// PAINTSTRUCT の解放
	EndPaint( hWnd, &ps );
}

//--------------------------------------------------------------------------------------------------
//	ドラッグ＆ドロップされたファイルを取得
//--------------------------------------------------------------------------------------------------

void GetDropFiles( char** ppDropFileList, unsigned int listSize, unsigned int* pDropFileCount, WPARAM wParam )
{
	//ドロップされたファイル名を一時的に保存する文字配列
	char fileName[ MAX_PATH ];

	//リストをクリア
	unsigned int i;
	for( i = 0; i < listSize; i++ ) *( ppDropFileList + i ) = NULL;

	//ドロップされたファイル数を取得
	*pDropFileCount = DragQueryFile( ( HDROP )wParam, 0xffffffff, NULL, NULL );
	if( *pDropFileCount > listSize ) *pDropFileCount = listSize;

	for( i = 0; i < *pDropFileCount; i++ )
	{
		//格納用配列の値を初期化
		memset( fileName, '\0', sizeof( fileName ) );

		//ドロップされたファイルを取得
		DragQueryFile( (HDROP)wParam, i, fileName, sizeof( fileName ) );
		
		//ファイル名格納用のメモリを確保
		char* p_file = ( char* )GlobalAlloc( GPTR, strlen( fileName ) + 1 );
		if( p_file == NULL ) return;

		//確保したメモリにファイル名をコピー
		strcpy( p_file, fileName );

		//ドロップされたファイル名のリストに追加
		*( ppDropFileList + i ) = p_file;
	}

	//ドラッグ＆ドロップ操作終了
	DragFinish( ( HDROP )wParam );
}

//--------------------------------------------------------------------------------------------------
//	ドロップされたファイル名取得時に確保したメモリを解放
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
//	ドロップされたファイル名を表示
//--------------------------------------------------------------------------------------------------

void ShowDropFiles( char** ppDropFileList, unsigned int dropFileCount )
{
	unsigned int i;
	for( i = 0; i < dropFileCount; i++ )
	{
		MessageBox( NULL, *( ppDropFileList + i ), "ドロップされたファイル", MB_OK );
	}
}
