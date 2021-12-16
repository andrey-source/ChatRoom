#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/strand.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>


class Client : public std::enable_shared_from_this<Client>
{
public:
    Client(boost::asio::io_context& io_context)
            : resolver_(net::make_strand(io_context)),
              stream_(net::make_strand(io_context))
    {
    }

    void push(const std::string& server, 
                const std::string& port, 
                const std::string& message)
    {
        /// send server request if it chat  
        request_.version(10);
        request_.method(http::verb::post);
        request_.target("record");
        request_.set(http::field::host, server);
        request_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request_.prepare_payload();

        /// send server request if it file
        beast::error_code ec;
        http::file_body::value_type body;
        body.open(message.c_str(), beast::file_mode::read, ec);

        // Handle the case where the file doesn't exist
        if(ec == beast::errc::no_such_file_or_directory)
                std::cout << "Error: " << ec << "\n";

        // Handle an unknown error
        if(ec)
                std::cout << "Error: " << ec.message() << "\n";;

        // Cache the size since we need it after the move
        // use the full host:port here
        request_file.method(http::verb::post);
        request_file.target("record");
        request_file.version(10);
        request_file.content_length(body.size());
        request_file.set(http::field::host, server);
        request_file.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request_file.body() = std::move(body);
        request_file.prepare_payload();        
    
    
        resolver_.async_resolve(server, port,
                                beast::bind_front_handler(
                                        &Client::handle_resolve,
                                        shared_from_this()));
    }

void show_server(const std::string& server, 
                const std::string& port)
    { 
            request_.version(10);
            request_.method(http::verb::get);
            request_.target("show");
            request_.set(http::field::host, server);
            request_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            request_.prepare_payload();

        resolver_.async_resolve(server, port,
                                beast::bind_front_handler(
                                        &Client::handle_resolve,
                                        shared_from_this()));
    }

    void download(const std::string& server, 
                const std::string& port, 
                const std::string& message)
    {
            request_.version(10);
            request_.method(http::verb::get);
            request_.target("play");
            request_.body() = message;
            request_.set(http::field::host, server);
            request_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            request_.prepare_payload();

        resolver_.async_resolve(server, port,
                                beast::bind_front_handler(
                                        &Client::handle_resolve,
                                        shared_from_this()));
    }
private:
    void handle_resolve(beast::error_code err,
                        tcp::resolver::results_type results)
    {
        if (!err)
        {
            stream_.async_connect(
                    results,
                    beast::bind_front_handler(
                            &Client::handle_connect,
                            shared_from_this()));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_connect(beast::error_code err, tcp::resolver::results_type::endpoint_type)
    {
        if (!err)
        {
            if (request_.target() == "play"){

                http::async_write(stream_, request_,
                              beast::bind_front_handler(
                                      &Client::handle_write_file,
                                      shared_from_this()));
            }
            else {

                http::async_write(stream_, request_,
                              beast::bind_front_handler(
                                      &Client::handle_write,
                                      shared_from_this()));

            }       
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_write( beast::error_code err,
                              std::size_t bytes_transferred)
    {
        if (!err)
        {
                http::async_read(stream_, buffer_, response_,
                             beast::bind_front_handler(
                                     &Client::handle_read,
                                     shared_from_this()));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_write_file( beast::error_code err,
                              std::size_t bytes_transferred)
    {
        if (!err)
        {
                beast::error_code ecdd;
            
            // need create file for file from server
                response_file.body().open("/home/liza/Documents/boost.beast/client.txt", boost::beast::file_mode::write, ecdd);

                // Handle the case where the file doesn't exist
                if(ecdd == beast::errc::no_such_file_or_directory)
                        std::cout << "Error: " << ecdd << "\n";

                // Handle an unknown error
                if(ecdd)
                        std::cout << "Error: " << ecdd.message() << "\n";

                http::async_read(stream_, buffer_, response_file,
                             beast::bind_front_handler(
                                     &Client::handle_read,
                                     shared_from_this()));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
            do_close();
        }
    }

    void handle_read(beast::error_code err,
                              std::size_t bytes_transferred)
    {
        
        if (!err)
        { 
            if (response_.body().size() > 4){
                std::cout << response_.body()<<std::endl;
            }
            
            if(response_.body() == "file"){
                http::async_write(stream_, request_file,
                             beast::bind_front_handler(
                                     &Client::handle_write_last,
                                     shared_from_this()));
            }
            
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
            do_close();
            
        }
    }

    void handle_write_last(beast::error_code err,
                              std::size_t bytes_transferred)
    {
            do_close();
    }
    /*void handle_write(beast::error_code err,
                     std::size_t bytes_transferred)
    { 
        if (!err)
        {
            if (!std::strcmp(flag.c_str(),"chat")){
                http::async_read(stream_, buffer_, response_,
                             beast::bind_front_handler(
                                     &Client::handle_read,
                                     shared_from_this()));
                
            }else{

                beast::error_code ecdd;
                response_file.body().open("/home/liza/Documents/boost.beast/client.txt", boost::beast::file_mode::write, ecdd);

                // Handle the case where the file doesn't exist
                if(ecdd == beast::errc::no_such_file_or_directory)
                        std::cout << "Error: " << ecdd << "\n";

                // Handle an unknown error
                if(ecdd)
                        std::cout << "Error: " << ecdd.message() << "\n";;

                // Cache the size since we need it after the move
                // use the 
                http::async_read(stream_, buffer_file, response_file,
                             beast::bind_front_handler(
                                     &Client::handle_read,
                                     shared_from_this()));

        }
        else
        {           
            std::cout << "Error: " << err << "\n";
            do_close();
        }
    }
*/
    void do_close()
        {
            beast::error_code ec;
            stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

        }
   

private:
    tcp::resolver resolver_;
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_; // (Must persist between reads)
    beast::multi_buffer buffer_file; // (Must persist between reads)
    std::string flag = "chat";
;
    http::request<http::file_body> request_file;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
    http::response<http::file_body> response_file;
    //http::response<http::string_body> response_;
    std::shared_ptr<void> res_;
};

int main(int argc, char* argv[])
{
    
        try
        {
            const std::string body = "";
            const std::string server = "localhost";
            const std::string port = "5000";
            const std::string versus;
            //const std::string path = "/";
        
            net::io_context io_context;
            if (argv[1] == "show"){
                std::make_shared<Client>(io_context)->show_server(server,port);
            }
            else if(argv[1] =="play"){
                std::make_shared<Client>(io_context)->download(server,port,argv[2]);
            }
            else{
                std::make_shared<Client>(io_context)->push(server,port,argv[2]);
 //               std::stringstream ss;
  //              for(int i = 3;i<argc;i++){
 //                   ss<<argv[i]<<" ";
  //              }
                //std::make_shared<Client>(io_context)->run(server,port,argv[1],argv[2]);
            }
            io_context.run();
        }
        catch (std::exception& e)
        {
            std::cout << "Exception: " << e.what() << "\n";
        }
   
    return 0;
}
