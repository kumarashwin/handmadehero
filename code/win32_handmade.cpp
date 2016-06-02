#include <windows.h>
#include <"C:\Windows\System32\gdi32.dll">

LRESULT CALLBACK MainWindowCallBack(
  HWND   Window,
  UINT   Message,
  WPARAM wParam,
  LPARAM lParam
)
{
	LRESULT Result = 0;
	
	switch(Message)
	{
		case WM_SIZE:
		{
			OutputDebugStringA("WM_SIZE\n");
			break;
		}
		case WM_DESTROY:
		{
			OutputDebugStringA("WM_DESTROY\n");
			break;
		}
		case WM_CLOSE:
		{
			OutputDebugStringA("WM_CLOSE\n");
			break;
		}
		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			
			
			// Note To Ash: Find how to include gdi32.lib
			PatBlt(DeviceContext, X, Y, Width, Height, WHITENESS);

			EndPaint(Window, &Paint);
			break;
		}
		default:
		{
			OutputDebugStringA("default\n");
			Result = DefWindowProc(Window, Message,wParam,lParam);
			break;
		}
	}
	return(Result);
}


int CALLBACK WinMain(
  HINSTANCE Instance,
  HINSTANCE PrevInstance,
  LPSTR     CommandLine,
  int       ShowCode
)
{
	WNDCLASS WindowClass = {};
	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowCallBack;
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon;
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";
	
	if(RegisterClass(&WindowClass))
	{
		HWND WindowHandle = CreateWindowEx(
			0,
			WindowClass.lpszClassName,
			"Handmade Hero",
			WS_OVERLAPPEDWINDOW|WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			Instance,
			0
		);
		
		if(WindowHandle)
		{
			MSG Message;
			for(;;)
			{
				BOOL MessageResult = GetMessage(&Message,0,0,0);
				
				if(MessageResult > 0)
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				else
				{
					break;
				}
			}
		}
		else
		{
		//Logging; if it fails
		}	
	}
	else{
		//Logging; if it fails
	}
}

