#ifndef __TWIRLING_LOG_H__
#define __TWIRLING_LOG_H__

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define BUF_LEN 500

/* 封装调试函数 */
#define macroFunc(info)  (transparentMacroFunc(__FILE__, __func__, __LINE__, (info)))

/* 封装带可变参数的调试函数 */
#define macroFuncVargs(fmt, ...)  (transparentMacroFuncVargs(__FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__))

/* file， func， line为宏封装好的入参，info为调用者传入参数 */
int transparentMacroFunc(const char *file, const char *func, int line, const char *info);

/* 同上，只不过改为可变参数 */
int transparentMacroFuncVargs(const char *file, const char *func, int line, const char *fmt, ...);

#endif /** __TWIRLING_LOG_H__ **/

