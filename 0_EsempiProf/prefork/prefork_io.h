ssize_t readn (int fd, void *buf, size_t n);
ssize_t writen (int fd, const void *buf, size_t n);
int readline (int fd, void *buf, int maxlen);
ssize_t write_fd (int fd, void *ptr, size_t nbytes, int sendfd);
ssize_t read_fd (int fd, void *ptr, size_t nbytes, int *recvfd);
