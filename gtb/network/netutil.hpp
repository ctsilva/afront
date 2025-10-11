#ifndef GTB_NETWORK_UTIL_INCLUDED
#define GTB_NETWORK_UTIL_INCLUDED


// Returns 0 on success, -1 on failure.
int get_host_name(char *buffer, int len);

// Unlike ::read, makes sure the entire message is read.
void read_message(int sock_descr, void *msg_ptr, int bytes_to_read);

// Unlike ::write, makes sure the entire message is written.
void write_message(int sock_descr, const void *msg_ptr, int bytes_to_write);


void net_read_bool(bool &b, int fd);
void net_read_char(char &c, int fd);
void net_read_uchar(unsigned char &c, int fd);
void net_read_int(int &x, int fd);
void net_read_uint(unsigned &x, int fd);
void net_read_float(float &x, int fd);
void net_read_str(char *buffer, unsigned size, int fd);

void net_write_bool(bool b, int fd);
void net_write_char(unsigned char c, int fd);
void net_write_uchar(unsigned char c, int fd);
void net_write_int(int x, int fd);
void net_write_uint(unsigned x, int fd);
void net_write_float(float x, int fd);
void net_write_str(const char *s, int fd);


#endif // GTB_NETWORK_UTIL_INCLUDED
