#ifndef __POLLER__H__
#define __POLLER__H__

#include <openssl/ssl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

typedef struct __poller         poller_t;
typedef struct __poller_message poller_message_t;

struct __poller_message {
  int (*append)(const void *, size_t *, poller_message_t *);
  char data[0];
};

struct poller_data {
#define PD_OP_READ         1
#define PD_OP_WRITE        2
#define PD_OP_LISTEN       3
#define PD_OP_CONNECT      4
#define PD_OP_SSL_READ     PD_OP_READ
#define PD_OP_SSL_WRITE    PD_OP_WRITE
#define PD_OP_SSL_ACCEPT   5
#define PD_OP_SSL_CONNECT  6
#define PD_OP_SSL_SHUTDOWN 7
#define PD_OP_EVENT        8
#define PD_OP_NOTIFY       9
#define PD_OP_TIMER        10

  short          operation;
  unsigned short iovcnt;
  int            fd;

  union {
    SSL *ssl;
    void *(*accept)(const struct sockaddr *, socklen_t, int, void *);
    void *(*event)(void *);
    void *(*notify)(void *, void *);
  };

  void *context;
  union {
    poller_message_t *message;
    struct iovec     *write_iov;
    void             *result;
  };
};

struct poller_result {
#define PR_ST_SUCESS   0
#define PR_ST_FINISHED 1
#define PR_ST_ERROR    2
#define PR_ST_DELETED  3
#define PR_ST_MODIFIED 4
#define PR_ST_TOPPED   5

  int                state;
  int                error;
  struct poller_data data;
};

struct poller_params {
  size_t max_open_files;

  poller_message_t *(*create_message)(void *);
  int (*partial_written)(size_t, void *);
  void (*callback)(struct poller_result *, void *);

  void *context;
};

#ifdef __cplusplus
extern "C" {
#endif

poller_t *poller_create(const poller_params *params);
int       poller_start(poller_t *poller);
int  poller_add(const struct poller_data *data, int timeout, poller_t *poller);
int  poller_del(int fd, poller_t *poller);
int  poller_mod(const struct poller_data *data, int timeout, poller_t *poller);
int  poller_set_timeout(int fd, int timeout, poller_t *poller);
int  poller_add_timer(const struct timespec *value,
                      void                  *context,
                      poller_t              *poller);
void poller_stop(poller_t *poller);
void poller_destroy(poller_t *poller);
#ifdef __cplusplus
}
#endif
#endif /* __POLLER__H__ */