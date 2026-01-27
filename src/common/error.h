/*****************************************************************************/
/*                            Project Error Types                            */
/*****************************************************************************/
#ifndef ERROR_H_
#define ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

enum error {
    E_SUCCESS = 0,
    E_GENERIC,
    E_NOT_IMPLEMENTED,
    E_NULL_POINTER,
    E_INVALID_INPUT,
    E_INVALID_STATE,
    E_BUFFER_FULL,
    E_BUFFER_SIZE,
    E_HARDWARE,
    E_IO,
    E_TIMEOUT,
    E_BUSY,
};

#ifdef __cplusplus
}
#endif

#endif // ERROR_H_
