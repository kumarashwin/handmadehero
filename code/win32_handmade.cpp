#include <windows.h>
#include <stdint.h>
#include <stdlib.h>

#define internal static 
#define local_persist static
#define global_variable static

global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;

internal void Win32ResizeDIBSection(int Width, int Height)
{	
	if(BitmapMemory)
	{
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}
	
	BitmapWidth = Width;
	BitmapHeight = Height;
	
	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	
	int BytesPerPixel = 4;
	int BitmapMemorySize = (BitmapWidth*BitmapHeight)*BytesPerPixel;
	BitmapMemory = VirtualAlloc( 0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	
	int Pitch = Width*BytesPerPixel;
	uint8_t *Row = (uint8_t *)BitmapMemory;
	for(int Y = 0; Y < BitmapHeight; ++Y)
	{
		uint8_t *Pixel = (uint8_t *)Row;
		for(int X = 0; X < BitmapWidth; ++X)
		{
			*Pixel = (uint8_t)X;
			++Pixel;
			*Pixel = (uint8_t)Y;
			++Pixel; 
			*Pixel = 0;
			++Pixel;
			*Pixel = 0;
			++Pixel;
		}
		
		Row += Pitch;
	}
}

internal void Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height)
{
	int WindowWidth = WindowRect->right - WindowRect->left;
	int WindowHeight = WindowRect->bottom - WindowRect->top;
	
	StretchDIBits(
        DeviceContext,
		/*
        X, Y, Width, Height,
		X, Y, Width, Height,
		*/
		0, 0, BitmapWidth, BitmapHeight,
		0, 0, WindowWidth, WindowHeight,
		BitmapMemory,
		&BitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallBack(
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
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			int Width = ClientRect.right - ClientRect.left;
			int Height = ClientRect.bottom - ClientRect.top;
			Win32ResizeDIBSection(Width, Height);
			OutputDebugStringA("WM_SIZE\n");
			break;
		}
		case WM_DESTROY:
		{
			Running = false;
			OutputDebugStringA("WM_DESTROY\n");
			break;
		}
		case WM_CLOSE:
		{
			Running = false;
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
			
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			
			Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
			
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
	WindowClass.lpfnWndProc = Win32MainWindowCallBack;
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
			
			Running = true;
			while(Running)
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

