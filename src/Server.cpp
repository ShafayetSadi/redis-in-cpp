#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

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

  // Accept connections in a loop
  while (true)
  {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
    if (client_fd < 0)
    {
      std::cerr << "accept failed\n";
      return 1;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "Client connected from " << client_ip << ":" << ntohs(client_addr.sin_port) << "\n";

    char buffer[1024];
    while (true)
    {
      ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
      if (bytes_received <= 0)
      {
        break;
      }
      buffer[bytes_received] = '\0';

      std::string command = buffer;
      while (!command.empty() && (command.back() == '\n' || command.back() == '\r'))
      {
        command.pop_back();
      }
      std::cout << "Received command: " << command << std::endl;

      std::string response = "+PONG\r\n";
      std::cout << "Sending response: " << response << std::endl;

      send(client_fd, response.c_str(), response.size(), 0);
    }

    close(client_fd);
  }

  close(server_fd);

  return 0;
}
