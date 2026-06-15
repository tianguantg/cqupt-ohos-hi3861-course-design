#ifndef __DEBUG_H_
#define __DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>

#define DEBG 1 

#ifdef DEBG

#define DBG_ERR(...) \
    do { \
            printf("FILE:%s, FUNC:%s(), LINE:%d ",__FILE__,__FUNCTION__,__LINE__);\
            printf(__VA_ARGS__); \
        } \
    } while (0)

#define DBG(...) \
    do { \
        printf("FILE:%s, FUNC:%s(), LINE:%d ",__FILE__,__FUNCTION__,__LINE__);\
        printf(__VA_ARGS__);\
    } while (0)

#elif

#define DBG_ERR(...) \
    do { \
        if (log_is_print(LOG_LVL_DEBUG)) { \
            printf("%s() %d, ",  __func__, __LINE__);\
            printf(__VA_ARGS__); \
        } \
    } while (0)

#define DBG(...)

#endif

#ifdef __cplusplus
}
#endif

#endif
