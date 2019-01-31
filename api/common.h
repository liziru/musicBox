#ifndef SPEECH_C_DEMO_COMMON_H
#define SPEECH_C_DEMO_COMMON_H

/**
 * @brief 函数返回值定义
 */
typedef enum {
    RETURN_OK = 0, // 返回正常
    RETURN_ERROR = 1, // 返回错误
    RETURN_FAIL = 2, // 返回失败

    ERROR_TOKEN_CURL = 13, // TOKEN CURL 调用错误
    ERROR_TOKEN_PARSE_ACCESS_TOKEN = 15,  // access_token字段在返回结果中不存在
    ERROR_TOKEN_PARSE_SCOPE = 16, // 解析scope字段，或者scope不存在

    ERROR_ASR_FILE_NOT_EXIST = 101, // 本地文件不存在
    ERROR_ASR_CURL = 102, // 识别 curl 错误

    ERROR_TTS_CURL = 23
} RETURN_CODE;

#endif //SPEECH_C_DEMO_COMMON_H
