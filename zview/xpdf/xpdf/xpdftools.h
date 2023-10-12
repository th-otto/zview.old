#ifdef ZVPDF_SLB

#include <sys/stat.h>
#include <slb/freetype.h>

GBool pathIsFile(const char *path) {
#ifdef _WIN32
  wchar_t wPath[winMaxLongPath + 1];
  fileNameToUCS2(path, wPath, winMaxLongPath + 1);
  DWORD attr = GetFileAttributesW(wPath);
  return attr != INVALID_FILE_ATTRIBUTES &&
         !(attr & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE));
#else
  struct stat statBuf;
  return stat(path, &statBuf) == 0 && S_ISREG(statBuf.st_mode);
#endif
}

void fixCommandLine(int *argc, char **argv[]) {
#ifdef _WIN32
  int argcw;
  wchar_t **argvw;
  GString *arg;
  int i;

  argvw = CommandLineToArgvW(GetCommandLineW(), &argcw);
  if (!argvw || argcw < 0) {
    return;
  }

  *argc = argcw;

  *argv = (char **)gmallocn(argcw + 1, sizeof(char *));
  for (i = 0; i < argcw; ++i) {
    arg = fileNameToUTF8(argvw[i]);
    (*argv)[i] = copyString(arg->getCString());
    delete arg;
  }
  (*argv)[argcw] = NULL;

  LocalFree(argvw);
#else
  (void)argc;
  (void)argv;
#endif
}

#ifdef __cplusplus
extern "C" {
#endif

long zvpdf_freetype_open(void)
{
	return slb_freetype_open(NULL);
}


void zvpdf_freetype_close(void)
{
	slb_freetype_close();
}


#ifdef __cplusplus
}
#endif

#endif
