#include "log.h"

#define  LOG_DEBUG
#ifdef LOG_DEBUG
#include "myLog.h"
#endif

int transparentMacroFunc(const char *file, const char *func, int line, const char *info)
{
	char buf[BUF_LEN];
	memset(buf, 0, BUF_LEN);
	snprintf(buf, BUF_LEN, "File:%15s, Func:%20s, Line:%4d, info: %s", file, func, line, info);
#ifdef LOG_DEBUG
	logInfo(buf);
#else
	printf("%s\n", buf);
#endif
	return 0;
}

int transparentMacroFuncVargs(const char *file, const char *func, int line, const char *fmt, ...)
{
	char buf[BUF_LEN];
	memset(buf, 0, BUF_LEN);
	va_list args;

	struct tm *t;
	time_t tt;
	time(&tt);
	t = localtime(&tt);

	snprintf(buf, BUF_LEN, "File:%15s, Func:%20s, Line:%4d, info: ", file, func, line);
	va_start(args, fmt);
	vsnprintf(buf + strlen(buf), BUF_LEN - strlen(buf), fmt, args);
	va_end(args);
#ifdef LOG_DEBUG
	logInfo(buf);
#else
	printf("%s\n", buf);
#endif
	return 0;
}
