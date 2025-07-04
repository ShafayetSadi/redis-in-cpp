#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <vector>
#include <fcntl.h>

// Set the socket to non-blocking mode
bool set_non_blocking(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1)
  {
    std::cerr << "fcntl F_GETFL failed\n";
    return false;
  }

  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
  {
    std::cerr << "fcntl F_SETFL failed\n";
    return false;
  }

  return true;
}

int main(int argc, char **argv)
{
  // Flush after every std::cout or std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Create server socket
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }

  // Set socket options
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
  {
    std::cerr << "setsockopt failed\n";
    return 1;
  }

  // Bind server socket to an IP address and port
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;         // AF_INET means IPv4
  server_addr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY means the server will listen on all available network interfaces
  server_addr.sin_port = htons(6379);       // Port 6379 is the default port for Redis

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
  {
    std::cerr << "Failed to bind to port 6379\n";
    return 1;
  }

  // Listen for incoming connections
  int connection_backlog = 5; // The maximum number of connections that can be queued
  if (listen(server_fd, connection_backlog) != 0)
  {
    std::cerr << "listen failed\n";
    return 1;
  }

  std::cout << "Server listening on port 6379...\n";

  // Create an epoll instance
  int epoll_fd = epoll_create1(0);
  if (epoll_fd < 0)
  {
    std::cerr << "Failed to create epoll instance\n";
    close(server_fd);
    return 1;
  }

  // Add the server socket to the epoll instance
  struct epoll_event event;
  event.events = EPOLLIN; // Watch for incoming connections (EPOLLIN) and use edge-triggered mode (EPOLLET)
  event.data.fd = server_fd;

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) != 0)
  {
    std::cerr << "Failed to add server socket to epoll instance\n";
    close(server_fd);
    close(epoll_fd);
    return 1;
  }

  // Buffer to store events
  std::vector<struct epoll_event> events(128);

  // The Event Loop
  while (true)
  {
    // Wait for events to occur on the epoll instance, -1 means wait indefinitely
    int num_events = epoll_wait(epoll_fd, events.data(), events.size(), -1);
    if (num_events < 0)
    {
      std::cerr << "epoll_wait failed\n";
      return 1;
    }

    for (int i = 0; i < num_events; i++)
    {
      // Case 1: Event on the server socket -> New client connection
      if (events[i].data.fd == server_fd)
      {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);

        if (client_fd < 0)
        {
          // If errno is EAGAIN or EWOULDBLOCK, we have accepted all pending connections.
          if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;

          std::cerr << "accept failed\n";
          return 1;
        }

        // Set the client socket to non-blocking mode
        if (!set_non_blocking(client_fd))
        {
          close(client_fd);
          continue;
        }

        // Add the new client socket to the epoll instance
        struct epoll_event client_event;
        client_event.events = EPOLLIN | EPOLLET;
        client_event.data.fd = client_fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) != 0)
        {
          std::cerr << "Failed to add client socket to epoll instance\n";
          close(client_fd);
          continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "Client connected from " << client_ip << ":" << ntohs(client_addr.sin_port) << "\n";
      }
      // Case 2: Event on a client socket -> Data received from a client
      else
      {
        int client_fd = events[i].data.fd;

        char buffer[1024];
        while (true)
        {
          ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);
          if (bytes_received < 0)
          {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
              // In edge-triggered mode, we must read until read() returns -1 and errno is EAGAIN/EWOULDBLOCK.
              // Otherwise, we may miss data. No more data to read for now
              break;
            }

            std::cerr << "read() error on client " << client_fd << ": " << strerror(errno) << std::endl;

            // Real error
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
            close(client_fd);
            std::cout << "Client disconnected (error): " << client_fd << "\n";
            break;
          }
          else if (bytes_received == 0)
          {
            // Client disconnected gracefully
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
            close(client_fd);
            std::cout << "Client disconnected: " << client_fd << "\n";
            break;
          }

          buffer[bytes_received] = '\0';
          std::cout << "Received from client " << client_fd << ": " << buffer;

          std::string response = "+PONG\r\n";
          std::cout << "Sending response: " << response << std::endl;

          write(client_fd, response.c_str(), response.size());
        }
      }
    }
  }

  // Cleanup
  close(server_fd);
  close(epoll_fd);

  return 0;
}
