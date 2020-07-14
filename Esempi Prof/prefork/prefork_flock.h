pid_t child_make(int i, int listenfd, int addrlen);
void my_lock_init(char *pathname);
void my_lock_wait();
void my_lock_release();

