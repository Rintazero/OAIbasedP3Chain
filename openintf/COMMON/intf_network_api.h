#ifndef __INTF_NETWORK_API_H__
#define __INTF_NETWORK_API_H__

#include <unistd.h>

typedef struct {
  /* Connection endpoint reference  */
  void* endpoint;
  /* Connection endpoint handlers */
  void*   (*open) (int, const char*, const char*);
  int     (*getfd)(const void*);
  ssize_t (*recv) (void*, char*, size_t);
  ssize_t (*send) (const void*, const char*, size_t);
  void    (*close)(void*);
} INTF_NETWORK_API_ID;

int intf_network_api_initialize(INTF_NETWORK_API_ID* intf_network_api_id,const char* host, const char* port);

int intf_network_api_get_fd(INTF_NETWORK_API_ID* intf_network_api_id);


#endif // __INTF_NETWORK_API_H__