
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// for non - interactive only
#ifdef dbg
#define dbg_pc(ct, log)                                                        \
  {                                                                            \
    Plaintext pdbg;                                                            \
    decryptor.decrypt((ct), (pdbg));                                           \
    cout << (log) << (pdbg).to_string() << endl;                               \
  }
#define dbg_pp(val, log)                                                       \
  { cout << (log) << (val) << end; }
#else
#define dbg_pc(ct, log) 1
#define dbg_pp(pt, log) 1
#endif

std::size_t get_bitlen(uint64_t x) {
  // 0 is 1 bit...
  std::size_t ret = 1;
  while (x >>= 1)
    ++ret;
  return ret;
}

// set the log level
constexpr bool flag_log = 1;
int dummy_printf(const char *__restrict __fmt, ...) { return 1; }
auto pppt_printf = flag_log ? std::printf : dummy_printf;

// - coordination of pre-send / pre-recv
// - handle the dynamic transmissions
constexpr int SIZE_BUFFER = 128;
char buf[SIZE_BUFFER];

// pre-send the bytes length
void bytes_to_send(int sockfd, std::size_t bytes) {
  memset(buf, 0, SIZE_BUFFER);
  std::string str_bytes = std::to_string(bytes);
  send(sockfd, str_bytes.c_str(), SIZE_BUFFER, 0);
}

// pre-recv the bytes length
std::size_t bytes_to_receive(int sockfd) {
  memset(buf, 0, SIZE_BUFFER);
  recv(sockfd, buf, SIZE_BUFFER, 0);
  std::size_t bytes = std::stoull(buf);
  return bytes;
}

// send by stream (must tell the peer how many bytes)
ssize_t send_by_stream(int sockfd, std::stringstream &ss) {
  // std::cout << "before tell" << std::endl;
  bytes_to_send(sockfd, ss.str().length());
  // std::cout << "after tell" << std::endl;
  ssize_t bytes = send(sockfd, ss.str().c_str(), ss.str().length(), 0);
  return bytes;
}

// recv by stream
ssize_t recv_by_stream(int sockfd, std::stringstream &ss) {
  // std::cout << "before tell" << std::endl;
  auto bytes = bytes_to_receive(sockfd);
  // std::cout << "after tell" << std::endl;

  for (size_t remain_bytes = bytes; remain_bytes != 0;) {
    memset(buf, 0, sizeof(buf));
    auto cur_bytes =
        recv(sockfd, buf, std::min(size_t(remain_bytes), sizeof(buf)), 0);
    // if fail in half
    if (cur_bytes < 0)
      return cur_bytes; // instead... (bytes - remain_bytes)
    // std::cout << cur_bytes << std::endl;

    ss << std::string(buf, cur_bytes);
    remain_bytes -= cur_bytes;
  }
  return bytes;
}

int connect_to_server(std::string ip, uint16_t port, int domain) {

  // ipv4 / ipv6
  // int domain = AF_INET;
  if (domain == AF_INET) {
    // create a socket for server
    int sockfd_server = socket(AF_INET, SOCK_STREAM, 0);
    // std::cout << "sockfd_server" << std::endl;

    if (sockfd_server < 0) {
      perror("socket");
      return sockfd_server; // or -1
    }

    // connect to server
    struct sockaddr_in sockaddr_server;
    memset(&sockaddr_server, 0, sizeof(sockaddr_server));
    sockaddr_server.sin_family = AF_INET;
    sockaddr_server.sin_port = htons(port);
    sockaddr_server.sin_addr.s_addr = inet_addr(ip.c_str());

    // std::cout << "sockfd_server" << std::endl;

    int conn_result =
        connect(sockfd_server, (struct sockaddr *)&sockaddr_server,
                sizeof(sockaddr_server));
    if (conn_result < 0) {
      perror("connect");
      close(sockfd_server);
      return -1;
    }
    // std::cout << "conn_result" << std::endl;

    return sockfd_server;
  }    //
  else // if (domain == AF_INET6) {
  {
    int sockfd_server = socket(AF_INET6, SOCK_STREAM, 0);

    if (sockfd_server < 0) {
      perror("socket");
      return sockfd_server; // or -1
    }

    // connect to server
    // struct sockaddr_in6 sockaddr_server;
    // memset(&sockaddr_server, 0, sizeof(sockaddr_server));
    // sockaddr_server.sin6_family = AF_INET6;
    // sockaddr_server.sin6_port = htons(port);
    // // inet_pton(AF_INET6, ip.c_str(),
    //           &sockaddr_server.sin6_addr); // 绑定指定地址， ipv6

    sockaddr_in6 sockaddr_server;

    memset(&sockaddr_server, 0, sizeof(sockaddr_server));
    sockaddr_server.sin6_family = AF_INET6;
    sockaddr_server.sin6_port = htons(port);

    struct addrinfo *ai, hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI; //
    // std::cout << "a" << std::endl;
    if (getaddrinfo(ip.c_str(), "51022", &hints, &ai) != 0) {
      perror("IN GETADDRINFO()");
      exit(1);
    }
    sockaddr_server = *(struct sockaddr_in6 *)ai->ai_addr;
    // sockaddr_server.sin6_scope_id = 3; // 指定使用的接口

    // std::cout << ip.c_str() << std::endl;

    int conn_result =
        connect(sockfd_server, (struct sockaddr *)&sockaddr_server,
                sizeof(sockaddr_server));
    if (conn_result < 0) {
      perror("connect");
      close(sockfd_server);
      return -1;
    }
    return sockfd_server;
  }
}

// specify server's listening sock(ip + port)
int connect_to_client(std::string ip, uint16_t port, int domain) {

  // int domain = AF_INET;
  if (domain == AF_INET) {
    // crete a socket
    int sockfd_listening = socket(AF_INET, SOCK_STREAM, 0);
    int option = 1;
    setsockopt(sockfd_listening, SOL_SOCKET, SO_REUSEADDR, &option,
               sizeof(option));

    if (sockfd_listening < 0) {
      perror("socket");
      return -1;
    }
    pppt_printf("socket created..................\n");

    // bind the ip address and port to a socket
    sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(port);
    myaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    int ret =
        bind(sockfd_listening, (struct sockaddr *)&myaddr, sizeof(myaddr));
    if (ret < 0) {
      perror("bind");
      return -1;
    }

    // socket is for listening
    ret = listen(sockfd_listening, 8);
    if (ret < 0) {
      perror("listen");
      return -1;
    }

    pppt_printf("listening...............\n");

    // wait for a connection
    sockaddr_in sockaddr_client;
    unsigned sz_client = sizeof(sockaddr_client);
    int sockfd_client = accept(sockfd_listening, (sockaddr *)&sockaddr_client,
                               (socklen_t *)&sz_client);
    if (sockfd_client < 0) {
      perror("accept");
      return -1;
    }

    // stop listening
    close(sockfd_listening);

    // print host:port of client
    char host[NI_MAXHOST];
    char serv[NI_MAXHOST];
    memset(host, 0, sizeof(host));
    memset(serv, 0, sizeof(serv));

    if (getnameinfo((sockaddr *)&sockaddr_client, sizeof(sockaddr_client), host,
                    NI_MAXHOST, serv, NI_MAXSERV, 0) == 0) {
      pppt_printf("Connected to client: %s:%s\n\n", host, serv);
    } else {
      inet_ntop(AF_INET, &sockaddr_client.sin_addr, host, NI_MAXHOST);
      pppt_printf("Connected to client: %s:%" PRIu16 "\n\n", host,
                  ntohs(sockaddr_client.sin_port));
    }
    return sockfd_client;
  }    //
  else // if (domain == AF_INET6) {
  {
    int sockfd_listening = socket(AF_INET6, SOCK_STREAM, 0);

    int option = 1;
    setsockopt(sockfd_listening, SOL_SOCKET, SO_REUSEADDR, &option,
               sizeof(option));
    if (sockfd_listening < 0) {
      perror("socket");
      return -1;
    }
    pppt_printf("socket created..................\n");

    // bind the ip address and port to a socket
    sockaddr_in6 myaddr;

    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin6_family = AF_INET6;
    myaddr.sin6_port = htons(port);
    // myaddr.sin6_addr = in6addr_any;
    // inet_pton(AF_INET6, ip.c_str(), &myaddr.sin6_addr);

    struct addrinfo *ai, hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI; //
    // std::cout << "a" << std::endl;
    if (getaddrinfo(ip.c_str(), "51022", &hints, &ai) != 0) {
      perror("IN GETADDRINFO()");
      exit(1);
    }
    myaddr = *(struct sockaddr_in6 *)ai->ai_addr;

    // myaddr.sin6_scope_id = 3; // 指定使用的接口

    int ret =
        bind(sockfd_listening, (struct sockaddr *)&myaddr, sizeof(myaddr));
    if (ret < 0) {
      perror("bind");
      return -1;
    }

    // socket is for listening
    ret = listen(sockfd_listening, 8);
    if (ret < 0) {
      perror("listen");
      return -1;
    }
    pppt_printf("listening...............\n");

    // wait for a connection
    sockaddr_in6 sockaddr_client;
    unsigned len = sizeof(sockaddr_client);
    int sockfd_client = accept(sockfd_listening, (sockaddr *)&sockaddr_client,
                               (socklen_t *)&len);
    if (sockfd_client < 0) {
      perror("accept");
      return -1;
    }

    // stop listening
    close(sockfd_listening);

    // print host:port of client
    char host[NI_MAXHOST];
    char serv[NI_MAXHOST];
    memset(host, 0, sizeof(host));
    memset(serv, 0, sizeof(serv));

    // ipv4
    // 使用 gethostbyname 和 gethostbyaddr
    // ipv6
    // getaddrinfo 和 getnameinfo

    if (getnameinfo((sockaddr *)&sockaddr_client, sizeof(sockaddr_client), host,
                    NI_MAXHOST, serv, NI_MAXSERV, 0) == 0) {
      pppt_printf("Connected to client: %s:%s\n\n", host, serv);
    } //
    else {
      inet_ntop(AF_INET6, &sockaddr_client.sin6_addr, host, NI_MAXHOST);
      pppt_printf("Connected to client: %s:%" PRIu16 "\n\n", host,
                  ntohs(sockaddr_client.sin6_port));
    }

    return sockfd_client;
  }
}