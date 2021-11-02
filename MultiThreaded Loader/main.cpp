
#include <Windows.h>
#include <vector>
#include <thread>
#include <string>
#include <chrono>
#include "resource.h"

#define WINDOW_CLASS_NAME L"MultiThreaded Loader Tool"
const unsigned int _kuiWINDOWWIDTH = 1200;
const unsigned int _kuiWINDOWHEIGHT = 1200;
#define MAX_FILES_TO_OPEN 50
#define MAX_CHARACTERS_IN_FILENAME 25

//Global Variables
std::vector<std::wstring> g_vecImageFileNames;
std::vector<std::wstring> g_vecSoundFileNames;
HINSTANCE g_hInstance;
bool g_bIsFileLoaded = false;

using namespace std;

vector <HBITMAP> imagesToPrint;

// The threads that do stuff
vector <thread> workingThreads;

//to be given a value based on a input .txt file
int dedicatedAmountOfWorkThreads = 2;



void  loadImage_Proper(std::wstring File_URL)
{
	//Grabs the image from the file url and formats them into a bitmap image
	HBITMAP LoaderFile = (HBITMAP)LoadImageW(NULL, (LPCWSTR)File_URL.c_str(), IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);

	//Bitmap image is imported into a vector of bitmaps
	imagesToPrint.push_back(LoaderFile);

}

// This function CAN NOT be multi threaded because windows witchcraft and reasons
void Controller(HWND wnd, int ImageNo)
{
	HBITMAP LoaderFile = imagesToPrint.back();
	imagesToPrint.pop_back();
	int xPos = (ImageNo * 100);
	int yPos = 0;

	if (yPos > 0) {
		xPos = ((ImageNo - 8 * 100));
	}
	else {
		xPos = ImageNo * 100;
	}
	if (xPos >= _kuiWINDOWWIDTH)
	{
		yPos += 100;
		xPos = 0;
	}

	wnd = CreateWindow(L"STATIC", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, xPos, yPos, 0, 0, wnd, NULL, NULL, NULL);

	SendMessageW(wnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LoaderFile);

}


bool ChooseImageFilesToLoad(HWND _hwnd)
{
	OPENFILENAME ofn;
	SecureZeroMemory(&ofn, sizeof(OPENFILENAME)); // Better to use than ZeroMemory
	wchar_t wsFileNames[MAX_FILES_TO_OPEN * MAX_CHARACTERS_IN_FILENAME + MAX_PATH]; //The string to store all the filenames selected in one buffer togther with the complete path name.
	wchar_t _wsPathName[MAX_PATH + 1];
	wchar_t _wstempFile[MAX_PATH + MAX_CHARACTERS_IN_FILENAME]; //Assuming that the filename is not more than 20 characters
	wchar_t _wsFileToOpen[MAX_PATH + MAX_CHARACTERS_IN_FILENAME];
	ZeroMemory(wsFileNames, sizeof(wsFileNames));
	ZeroMemory(_wsPathName, sizeof(_wsPathName));
	ZeroMemory(_wstempFile, sizeof(_wstempFile));

	//Fill out the fields of the structure
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = _hwnd;
	ofn.lpstrFile = wsFileNames;
	ofn.nMaxFile = MAX_FILES_TO_OPEN * 20 + MAX_PATH;  //The size, in charactesr of the buffer pointed to by lpstrFile. The buffer must be atleast 256(MAX_PATH) characters long; otherwise GetOpenFileName and 
													   //GetSaveFileName functions return False
													   // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
													   // use the contents of wsFileNames to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.lpstrFilter = L"Bitmap Images(.bmp)\0*.bmp\0"; //Filter for bitmap images
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;

	//If the user makes a selection from the  open dialog box, the API call returns a non-zero value
	if (GetOpenFileName(&ofn) != 0) //user made a selection and pressed the OK button
	{
		//Extract the path name from the wide string -  two ways of doing it
		//First way: just work with wide char arrays
		wcsncpy_s(_wsPathName, wsFileNames, ofn.nFileOffset);
		int i = ofn.nFileOffset;
		int j = 0;

		while (true)
		{
			if (*(wsFileNames + i) == '\0')
			{
				_wstempFile[j] = *(wsFileNames + i);
				wcscpy_s(_wsFileToOpen, _wsPathName);
				wcscat_s(_wsFileToOpen, L"\\");
				wcscat_s(_wsFileToOpen, _wstempFile);
				g_vecImageFileNames.push_back(_wsFileToOpen);
				j = 0;
			}
			else
			{
				_wstempFile[j] = *(wsFileNames + i);
				j++;
			}
			if (*(wsFileNames + i) == '\0' && *(wsFileNames + i + 1) == '\0')
			{
				break;
			}
			else
			{
				i++;
			}

		}

		g_bIsFileLoaded = true;
		return true;
	}
	else // user pressed the cancel button or closed the dialog box or an error occured
	{
		return false;
	}

}

bool ChooseSoundFilesToLoad(HWND _hwnd)
{
	OPENFILENAME ofn;
	SecureZeroMemory(&ofn, sizeof(OPENFILENAME)); // Better to use than ZeroMemory
	wchar_t wsFileNames[MAX_FILES_TO_OPEN * MAX_CHARACTERS_IN_FILENAME + MAX_PATH]; //The string to store all the filenames selected in one buffer togther with the complete path name.
	wchar_t _wsPathName[MAX_PATH + 1];
	wchar_t _wstempFile[MAX_PATH + MAX_CHARACTERS_IN_FILENAME]; //Assuming that the filename is not more than 20 characters
	wchar_t _wsFileToOpen[MAX_PATH + MAX_CHARACTERS_IN_FILENAME];
	ZeroMemory(wsFileNames, sizeof(wsFileNames));
	ZeroMemory(_wsPathName, sizeof(_wsPathName));
	ZeroMemory(_wstempFile, sizeof(_wstempFile));

	//Fill out the fields of the structure
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = _hwnd;
	ofn.lpstrFile = wsFileNames;
	ofn.nMaxFile = MAX_FILES_TO_OPEN * 20 + MAX_PATH;  //The size, in charactesr of the buffer pointed to by lpstrFile. The buffer must be atleast 256(MAX_PATH) characters long; otherwise GetOpenFileName and 
													   //GetSaveFileName functions return False
													   // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
													   // use the contents of wsFileNames to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.lpstrFilter = L"Wave Files (*.wav)\0*.wav\0All Files (*.*)\0*.*\0"; //Filter for wav files
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;

	//If the user makes a selection from the  open dialog box, the API call returns a non-zero value
	if (GetOpenFileName(&ofn) != 0) //user made a selection and pressed the OK button
	{
		//Extract the path name from the wide string -  two ways of doing it
		//Second way: work with wide strings and a char pointer 

		std::wstring _wstrPathName = ofn.lpstrFile;

		_wstrPathName.resize(ofn.nFileOffset, '\\');

		wchar_t* _pwcharNextFile = &ofn.lpstrFile[ofn.nFileOffset];

		while (*_pwcharNextFile)
		{
			std::wstring _wstrFileName = _wstrPathName + _pwcharNextFile;

			g_vecSoundFileNames.push_back(_wstrFileName);

			_pwcharNextFile += lstrlenW(_pwcharNextFile) + 1;
		}

		g_bIsFileLoaded = true;
		return true;
	}
	else // user pressed the cancel button or closed the dialog box or an error occured
	{
		return false;
	}

}

LRESULT CALLBACK WindowProc(HWND _hwnd, UINT _uiMsg, WPARAM _wparam, LPARAM _lparam)
{
	PAINTSTRUCT ps;
	HDC _hWindowDC;
	//RECT rect;
	switch (_uiMsg)
	{
	case WM_KEYDOWN:
	{
		switch (_wparam)
		{
		case VK_ESCAPE:
		{
			SendMessage(_hwnd, WM_CLOSE, 0, 0);
			return(0);
		}
		break;
		default:
			break;
		}
	}
	break;
	case WM_PAINT:
	{

		_hWindowDC = BeginPaint(_hwnd, &ps);
		//Do all our painting here

		EndPaint(_hwnd, &ps);
		return (0);
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(_wparam))
		{
		case ID_FILE_LOADIMAGE:
		{
			if (ChooseImageFilesToLoad(_hwnd))
			{
				//TODO: Noman - "Write code here to create multiple threads to load image files in parallel"
				auto ImageLoadTime_StartFlag = std::chrono::high_resolution_clock::now();

				//Shit to do goes here


				if (g_vecImageFileNames.size() > MAX_FILES_TO_OPEN) {
					g_vecImageFileNames.resize(MAX_FILES_TO_OPEN);
				}



				//Records Start Time of the algorithm
				auto startTime = chrono::steady_clock::now();

				// work loop
				int WorkToBeDone = g_vecImageFileNames.size();
				for (int TaskesAllocated = 0; TaskesAllocated < WorkToBeDone;)
				{
					thread* MultiThread_ImageLoader = new thread[dedicatedAmountOfWorkThreads];



					if (dedicatedAmountOfWorkThreads < WorkToBeDone - TaskesAllocated)
					{
						for (int threadIndex_No = 0; threadIndex_No < dedicatedAmountOfWorkThreads; threadIndex_No++)
						{
							MultiThread_ImageLoader[threadIndex_No] = thread(loadImage_Proper, g_vecImageFileNames.back());
							g_vecImageFileNames.pop_back();
						}
						for (int threadIndex_No = 0; threadIndex_No < dedicatedAmountOfWorkThreads; threadIndex_No++)
						{
							if (MultiThread_ImageLoader[threadIndex_No].joinable())
								MultiThread_ImageLoader[threadIndex_No].join();

							TaskesAllocated++;
						}
					}
					else
					{
						int PendingWork = WorkToBeDone - TaskesAllocated;
						for (int threadIndex_No = 0; threadIndex_No < PendingWork; threadIndex_No++)
						{
							MultiThread_ImageLoader[threadIndex_No] = thread(loadImage_Proper, g_vecImageFileNames.back());
							g_vecImageFileNames.pop_back();
						}
						for (int threadIndex_No = 0; threadIndex_No < PendingWork; threadIndex_No++)
						{
							if (MultiThread_ImageLoader[threadIndex_No].joinable())
								MultiThread_ImageLoader[threadIndex_No].join();

							TaskesAllocated++;
						}
					}

				}//ENDFOR

				int CurrentImage_Slot = 0;
				while (imagesToPrint.size() > 0)
				{
					Controller(_hwnd, CurrentImage_Slot);
					CurrentImage_Slot++;

				}



				auto ImageLoadTime_FinishFlag = std::chrono::high_resolution_clock::now();

				auto ImageLoadTime_TotalExecutionTime = ImageLoadTime_FinishFlag - ImageLoadTime_StartFlag;

			}
			else
			{
				MessageBox(_hwnd, L"No Image File selected", L"Error Message", MB_ICONWARNING);
			}

			return (0);
		}
		break;

		case ID_FILE_LOADSOUND:
		{
			if (ChooseSoundFilesToLoad(_hwnd))
			{
				//Noman - "Write code here to create multiple threads to load sound files in parallel"
				// Noman's example of what to do

				//THREADPOOL ImageLoader{(size_t) UserSpecifiedThreadCount };

			}
			else
			{
				MessageBox(_hwnd, L"No Sound File selected", L"Error Message", MB_ICONWARNING);
			}
			return (0);
		}
		break;

		case ID_EXIT:
		{
			SendMessage(_hwnd, WM_CLOSE, 0, 0);
			return (0);
		}
		break;

		default:
			break;
		}
	}
	break;
	case WM_CLOSE:
	{
		PostQuitMessage(0);
	}
	break;
	default:
		break;
	}
	return (DefWindowProc(_hwnd, _uiMsg, _wparam, _lparam));
}


HWND CreateAndRegisterWindow(HINSTANCE _hInstance)
{
	WNDCLASSEX winclass; // This will hold the class we create.
	HWND hwnd;           // Generic window handle.

						 // First fill in the window class structure.
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = _hInstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground =
		static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register the window class
	if (!RegisterClassEx(&winclass))
	{
		return (0);
	}

	HMENU _hMenu = LoadMenu(_hInstance, MAKEINTRESOURCE(IDR_MENU1));

	// create the window
	hwnd = CreateWindowEx(NULL, // Extended style.
		WINDOW_CLASS_NAME,      // Class.
		L"MultiThreaded Loader Tool",   // Title.
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		10, 10,                    // Initial x,y.
		_kuiWINDOWWIDTH, _kuiWINDOWHEIGHT,                // Initial width, height.
		NULL,                   // Handle to parent.
		_hMenu,                   // Handle to menu.
		_hInstance,             // Instance of this application.
		NULL);                  // Extra creation parameters.

	return hwnd;
}



//Noman - Week 6 (56:00) - "It's already pushing the boundey how window is setup"
int WINAPI WinMain(HINSTANCE _hInstance,
	HINSTANCE _hPrevInstance,
	LPSTR _lpCmdLine,
	int _nCmdShow)
{
	MSG msg;  //Generic Message

	//Noman - Week 6 (56:07) - "What we need to know here is, this application it actually gets into the entry point it creates the handle 
	//                          of the window it creates an instance and it creates a, window itself"
	//                          "So, main entry point it basicly creates the window."
	//
	//Matthew's Translation - "Below we are creating an instance of an application window"

	HWND _hwnd = CreateAndRegisterWindow(_hInstance);

	//Noman - Week 6 (56:35) - "So after creation, if the creation is not done succfully, ah return 0. Which means that we are trying to run multipal instances and what not"
	if (!(_hwnd))
	{
		return (0);
	}
	//Noman - Week 6 (56:45) - "So after that main entry point"


	// Enter main event loop
	while (true)
	{
		// Test if there is a message in queue, if so get it.

		//Noman - Week 6 (57:11) - "We are using PeekMessage, PeekMessage is if there is an event there will be a message. For example minimising or anything"
		//Matthew's Translation - "What the... I have no idea what your going on about here"
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Test if this is a quit.
			if (msg.message == WM_QUIT)
			{

				break;
			}

			// Translate any accelerator keys.
			TranslateMessage(&msg);
			// Send the message to the window proc.
			DispatchMessage(&msg);
		}

	}

	// Return to Windows like this...
	return (static_cast<int>(msg.wParam));
}