#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>


class Client {
    int clientID = 0;
    
    public:
        int createID(int clientID)
        {
            if (clientID == 0)
            {
                clientID = rand();
            }
        return clientID;
        }
} ;


Class ClientMessage
{
    public:
    
        string mergeData(Client client,string client_msg)
        {
            return client_msg = str(client.clientID) + str("::") + client_msg;
        }
        
        string detectData(Client client,string client_msg)
        {   int i = 0;
            clientID = char[];
            
            while(client_msg!="::"){
                clientID = client_msg[i]
                i++;
                
            }
            client.clientID = clientID;
            return client_msg[i:];
        }
        
        void sendData(int filedes,string client_msg) 
        {
              size_t left = client_msg.size();
              ssize_t sent = 0;
            
              int flags = 0;
              while (left > 0) 
              {
                sent = ::send(filedes, client_msg.data() + sent, client_msg.size() - sent, flags);
                if (-1 == sent)
                  throw std::runtime_error("spend to server failed: " + std::string(strerror(errno)));
            
                left -= sent;
              }
         printf("start chat %s\n", client_msg.c_str());
        }
        
        string recievData(int filedes) 
        {
            char buf[1024];
            int n = ::recv(filedes, buf, sizeof(buf), MSG_NOSIGNAL);
    
          if (-1 == n && errno != EAGAIN)
            throw std::runtime_error("read failed: " + std::string(strerror(errno)));
          if (0 == n)
                throw std::runtime_error("client: " + std::to_string(filedes) +
                                         " disconnected");
                                         
          if (-1 == n)
            throw std::runtime_error("client: " + std::to_string(filedes) +
                                     " timeouted");
        
          return ret(buf, buf + n);
          std::cerr << "client: " << filedes << ", recv: \n"
                    << ret << " [" << n << " bytes]" << std::endl;
        }
    
        void init_sockaddr(struct sockaddr_in* name,const char* hostname,
                       uint16_t port) 
        {
          name->sin_family = AF_INET;
          name->sin_port = htons(port);
        
          struct hostent* hostinfo;
          hostinfo = gethostbyname(hostname);
          if (hostinfo == NULL) 
              {
                fprintf(stderr, "Unknown host %s.\n", hostname);
                exit(EXIT_FAILURE);
              }
          name->sin_addr = *(struct in_addr*)hostinfo->h_addr;
        }
        
        
        
        void Room(Client client, int client_count, string client_msg)
        {    
             int i = 0;
             string result_string ;
             string* arr_string = (string*)malloc(10*(string*));
             string client_msg_new = mergeData(client,client_msg);
             int sock = socket(PF_INET, SOCK_STREAM, 0);
     
              std::string host(80);
              int port = std::stoi(client_msg_new);
              struct sockaddr_in servername;
              init_sockaddr(&servername, host.c_str(), port);
            
  
              sendData(sock, client_msg);
              
              result_string = detectData(client,recievData(client));
              if (i<10){
                arr_string[i] =result_string;
                i++;
              }
              if (result_string=="0")
              {
                printf (arr_string);
              } 
              close(sock);
    
        }

}
