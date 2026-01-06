#ifndef ERROR_H_
#define ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

enum error {
    E_SUCCESS = 0,
    E_INVALID_INPUT,
    E_NULL_POINTER,
    E_BUFFER_FULL,
    E_BUFFER_SIZE,
};

#ifdef __cplusplus
}
#endif

#endif // ERROR_H_
