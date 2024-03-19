#include <manta/window.hpp>
#include <manta/backend/window/windows/window.windows.hpp>

#include <config.hpp>
#include <debug.hpp>
#include <types.hpp>

#include <manta/engine.hpp>
#include <manta/input.hpp>
#include <manta/gfx.hpp>

#include <vendor/stdlib.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WINDOW_CLASS ( TEXT( BUILD_PROJECT ) )
#define WINDOW_TITLE ( TEXT( PROJECT_CAPTION ) )
#define WINDOW_STYLE ( WS_OVERLAPPEDWINDOW )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace iWindow
{
	HWND handle;

	static LRESULT CALLBACK procedure( HWND wnd, UINT msg, WPARAM wp, LPARAM lp )
	{
		switch( msg )
		{
			// Close Button
			case WM_CLOSE:
			{
				Engine::exit();
				return 0;
			}

			// Min/Max Info
			case WM_GETMINMAXINFO:
			{
				MINMAXINFO *info = reinterpret_cast<MINMAXINFO *>( lp );
				info->ptMinTrackSize.x = Window::widthMin > 0 ? static_cast<LONG>( Window::widthMin ) : 0;
				info->ptMinTrackSize.y = Window::heightMin > 0 ? static_cast<LONG>( Window::heightMin ) : 0;
				info->ptMaxTrackSize.x = Window::widthMax > 0 ? static_cast<LONG>( Window::widthMax ) : I32_MAX;
				info->ptMaxTrackSize.y = Window::heightMax > 0 ? static_cast<LONG>( Window::heightMax ) : I32_MAX;
				return 0;
			}

			// Focus
			case WM_ACTIVATEAPP:
			{
				Window::hasFocus = static_cast<bool>( wp );
				return 0;
			}

			// Resize
			case WM_SIZE:
			{
				BringWindowToTop( iWindow::handle );
				int width = LOWORD( lp );
				int height = HIWORD( lp );
				if( width != 0 && height != 0 )
				{
					Window::width = static_cast<i32>( width );
					Window::height = static_cast<i32>( height );
					Window::resized = true;
					Gfx::viewport_update();
				}
				return 0;
			}


			// Position
			case WM_MOVE:
			{
				// Reset fullscreen
				if( Window::fullscreen ) { Window::fullscreen_set( true ); }
				return 0;
			}

			// Key Presses
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				iKeyboard::keyboards[iKeyboard::primary].keyCurrent[wp] = ( msg == WM_KEYDOWN );
				return 0;
			}

			// Printable Character
			case WM_CHAR:
			{
				static WCHAR hs;

				if( IS_HIGH_SURROGATE( wp ) )
				{
					// This is the first part of the message: the high surrogate. We can't do
					// anything with this alone, so just save it for later, return, and wait
					// until we process the second message.
					hs = static_cast<WCHAR>( wp );
					return 0;
				}
				else if( IS_SURROGATE_PAIR( hs, wp ) )
				{
					wp = ( hs << 10 ) + static_cast<u32>( wp ) + ( 0x10000 - ( 0xD800 << 10 ) - 0xDC00 );
					hs = 0;
				}
				else
				{
					// TODO: ERROR!
				}

				// Keyboard Input Text
				//char buffer[10];
				//int size = WideCharToMultiByte( CP_UTF8, 0, (WCHAR*)&wp, 1, buffer, 10, nullptr, nullptr );
				//buffer[size] = '\0'; // Null terminate
				//keyboard_input_string_submit( buffer );

				return 0;
			}

			// I've never actually seen this message get sent, but apparently it's
			// used by a few third party IMEs. It's also trivial to implement since
			// it's already in UTF-32, so let's just be good boys and support it!
			case WM_UNICHAR:
			{
				// Announce support for WM_UNICHAR to the sender of this message.
				if( wp == UNICODE_NOCHAR ) { return 1; }

				// Keyboard Input Text
				//char buffer[10];
				//int size = WideCharToMultiByte( CP_UTF8, 0, (WCHAR*)&wp, 1, buffer, 10, nullptr, nullptr );
				//buffer[size] = '\0'; // Null terminate
				//keyboard_input_string_submit( buffer );

				return 0;
			}

			// Mouse Button
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			{
				// Update State
				iMouse::mice[iMouse::primary].buttonCurrent = static_cast<int>( wp );

				// Update Capture
				if( wp & ( MK_LBUTTON | MK_RBUTTON | MK_MBUTTON ) )
				{
					SetCapture( iWindow::handle );
				}
				else
				{
					ReleaseCapture();
				}

				return 0;
			}

			case WM_MOUSEWHEEL:
			{
				if( GET_WHEEL_DELTA_WPARAM( wp ) > 0 )
				{
					iMouse::mice[iMouse::primary].wheelY = -1; // up
					iMouse::mice[iMouse::primary].wheelYVelocity = 1.0f;
				}
				else
				{
					iMouse::mice[iMouse::primary].wheelY = 1; // down
					iMouse::mice[iMouse::primary].wheelYVelocity = 1.0f;
				}

				return 0;
			}

			case WM_MOUSEHWHEEL:
			{
				if( GET_WHEEL_DELTA_WPARAM( wp ) > 0 )
				{
					iMouse::mice[iMouse::primary].wheelX = 1; // right
					iMouse::mice[iMouse::primary].wheelXVelocity = 1.0f;
				}
				else
				{
					iMouse::mice[iMouse::primary].wheelX = -1; // left
					iMouse::mice[iMouse::primary].wheelXVelocity = 1.0f;
				}

				return 0;
			}

			// Mouse Movement
			case WM_MOUSEMOVE:
			{
				iMouse::mice[iMouse::primary].xPrevious = iMouse::mice[iMouse::primary].x;
				iMouse::mice[iMouse::primary].yPrevious = iMouse::mice[iMouse::primary].y;
				iMouse::mice[iMouse::primary].x = static_cast<float>( GET_X_LPARAM(lp) );
				iMouse::mice[iMouse::primary].y = static_cast<float>( GET_Y_LPARAM(lp) );
				return 0;
			}

			// Let Windows handle everything else
			default: { return DefWindowProcW( wnd, msg, wp, lp ); }
		}
	}


	bool init( const i32 defaultWidth, const i32 defaultHeight )
	{
		// Set Dimensions
		Window::width = defaultWidth;
		Window::height = defaultHeight;
		Window::resized = true;

		// Get Application Instance
		HINSTANCE instance;
		instance = GetModuleHandleW( nullptr );

		// Get Application Path
		WCHAR path[MAX_PATH];
		GetModuleFileNameW( nullptr, path, MAX_PATH );

		// Get Window Area
		RECT area;
		area.left = 0;
		area.top = 0;
		area.right = Window::width;
		area.bottom = Window::height;

		AdjustWindowRect( &area, WINDOW_STYLE, false );
		Window::width = area.right - area.left;
		Window::height = area.bottom - area.top;

		// Register Class
		WNDCLASSW wclass;

	#if GRAPHICS_OPENGL // TODO (jkrahn): GRAPHICS_VULKAN, GRAPHICS_D3D12, etc. -- consider these?
		wclass.style = CS_OWNDC;
	#else // Everything Else
		wclass.style = 0;
	#endif
		wclass.lpfnWndProc = procedure;
		wclass.cbClsExtra = 0;
		wclass.cbWndExtra = 0;
		wclass.hInstance = instance;
		wclass.hIcon = ExtractIconW( nullptr, path, 0 );
		wclass.hCursor = LoadCursorW( nullptr, IDC_ARROW );
		wclass.hbrBackground = nullptr;
		wclass.lpszMenuName = nullptr;
		wclass.lpszClassName = WINDOW_CLASS;

		// Check Error
		if( !RegisterClassW( &wclass ) ) { ErrorReturnMsg( false, "WIN: Failed to register window class" ); }

		// Create Window
		handle = CreateWindowExW(
	#if GRAPHICS_D3D11 // TODO (jkrahn): GRAPHICS_VULKAN, GRAPHICS_D3D12, etc. -- consider these?
			WS_EX_NOREDIRECTIONBITMAP,
	#else
			0,
	#endif
			WINDOW_CLASS,
			WINDOW_TITLE,
			WINDOW_STYLE,
			( GetSystemMetrics( SM_CXSCREEN ) - Window::width ) >> 1,
			( GetSystemMetrics( SM_CYSCREEN ) - Window::height ) >> 1,
			Window::width,
			Window::height,
			nullptr,
			nullptr,
			instance,
			nullptr );

		// Check Error
		if( handle == nullptr ) { ErrorReturnMsg( false, "WIN: Failed to get window handle" ); }

		// Success
		return true;
	}


	bool free()
	{
		// Success
		return true;
	}


	void show()
	{
		ShowWindow( iWindow::handle, SW_NORMAL );
	}


	void poll()
	{
	//	PROFILE_FUNCTION();

		MSG msg;

		while( PeekMessageW( &msg, nullptr, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessageW( &msg );
		}
	}


	void mouse_set_position( const int x, const int y )
	{
		POINT p;
		p.x = x;
		p.y = y;
		ClientToScreen( iWindow::handle, &p );
		SetCursorPos( p.x, p.y );
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Window
{
	void fullscreen_set( bool enabled )
	{
		// TODO: This function is inordinately gigantic.

		MONITORINFO monitor;
		int mw, mh; // Monitor Dimensions
		int cw, ch; // Client Dimensions
		RECT area;

		// ???
		monitor.cbSize = sizeof( MONITORINFO );

		// Get Nearest Monitor
		GetMonitorInfoW( MonitorFromWindow( iWindow::handle, MONITOR_DEFAULTTONEAREST ), &monitor );

		// Get Monitor Dimensions
		mw = monitor.rcMonitor.right  - monitor.rcMonitor.left;
		mh = monitor.rcMonitor.bottom - monitor.rcMonitor.top;

		if( enabled )
		{
			// Enter Fullscreen
			Window::fullscreen = true;

			// Update Window Style
			SetWindowLongPtrW( iWindow::handle, GWL_STYLE, WINDOW_STYLE & ~( WS_CAPTION | WS_THICKFRAME ) );

			// Update Window Size
			SetWindowPos(
				iWindow::handle,
				nullptr,
				monitor.rcMonitor.left,
				monitor.rcMonitor.top,
				mw,
				mh,
				SWP_FRAMECHANGED | SWP_SHOWWINDOW
			);

		}
		else
		{
			// Exit Fullscreen
			Window::fullscreen = false;

			// Get Client Dimensions
			area.left = 0;
			area.top = 0;
			area.right = WINDOW_WIDTH_DEFAULT;  // TODO
			area.bottom = WINDOW_HEIGHT_DEFAULT; // TODO

			AdjustWindowRect(&area, WINDOW_STYLE, false);

			cw = area.right  - area.left;
			ch = area.bottom - area.top;

			// Update Window Style
			SetWindowLongPtrW( iWindow::handle, GWL_STYLE, WINDOW_STYLE );

			// Update Window Size
			SetWindowPos(
				iWindow::handle,
				nullptr,
				monitor.rcMonitor.left + ( ( mw - cw ) >> 1 ),
				monitor.rcMonitor.top  + ( ( mh - ch ) >> 1 ),
				cw,
				ch,
				SWP_FRAMECHANGED | SWP_SHOWWINDOW
			);
		}
	}
}