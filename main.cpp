#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _WIN32
int
WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#else
int
main()
#endif
{
  return 0;
}
