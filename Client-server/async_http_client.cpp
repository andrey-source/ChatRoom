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
    void run_first(const std::string& server, 
                const std::string& port, 
                const std::string& path, 
                const std::string& body)
    {
        //flag = body;
        request_first.version(10);
        request_first.method(http::verb::get);
        request_first.target(path);
        request_.body() = body;
        request_first.set(http::field::host, server);
        request_first.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request_first.prepare_payload();
        //std::cout << body << std::endl;
        //std::cout << request_ << std::endl;
        resolver_.async_resolve(server, port,
                                beast::bind_front_handler(
                                        &Client::handle_resolve,
                                        shared_from_this()));
    }

    void run(const std::string& server, 
                const std::string& port, 
                const std::string& path, 
                const std::string& body)
    {
        
        request_.version(10);
        request_.method(http::verb::get);
        request_.target(path);
        request_.body() = body;
        request_.set(http::field::host, server);
        request_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request_.prepare_payload();
        std::cout << "flag" << flag<< std::endl;
        //std::cout << request_ << std::endl;
        resolver_.async_resolve(server, port,
                                beast::bind_front_handler(
                                        &Client::handle_resolve,
                                        shared_from_this()));
    }
    void run_file(const std::string& server, 
                const std::string& port, 
                const std::string& path, 
                const std::string& file,
                const char* file_path)
    {
        flag = file;
        beast::error_code ec;
        http::file_body::value_type body;
        body.open(file_path, beast::file_mode::read, ec);

        // Handle the case where the file doesn't exist
        if(ec == beast::errc::no_such_file_or_directory)
                std::cout << "Error: " << ec << "\n";

        // Handle an unknown error
        if(ec)
                std::cout << "Error: " << ec.message() << "\n";;

        // Cache the size since we need it after the move
        auto const size = body.size();

        //http::request<http::file_body> request_;
        // use the full host:port here
        request_file.method(http::verb::post);
        request_file.target(path);
        request_file.version(10);
        request_file.content_length(body.size());
        request_file.set(http::field::host, server);
        request_file.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        //request_.set(http::field::content_length, size);
        request_file.body() = std::move(body);

        //std::cout << body << std::endl;
        
        std::cout << body.size()<< std::endl;
        resolver_.async_resolve(server, port,       
                                beast::bind_front_handler(
                                        &Client::handle_resolve,
                                        shared_from_this()));
          std::cout << request_.method()<< std::endl;                              
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
        std::cout << "flag_handle_connect "<<flag << std::endl;
        if (!err)
        {
            if (flag == "chat"){
            http::async_write(stream_, request_,
                              beast::bind_front_handler(
                                      &Client::handle_write_request,
                                      shared_from_this()));
            }
            else{

            http::request_serializer<http::file_body,  http::fields> sre{request_file};
            
            http::async_write(stream_, sre,
                              beast::bind_front_handler(
                                      &Client::handle_write_request,
                                      shared_from_this()));
            }
            
            std::cout << "Good request: "  << "\n";
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_write_request( beast::error_code err,
                              std::size_t bytes_transferred)
    {
        std::cout << "In reading" << std::endl;
        std::cout << flag << std::endl;
        if (!err)
        {
            if (flag=="chat"){
                http::async_read(stream_, buffer_, response_,
                             beast::bind_front_handler(
                                     &Client::handle_read,
                                     shared_from_this()));
            }else{
                beast::error_code ec;
                response_file.body().open("/home/liza/Documents/boost.beast/new.txt", 
                                                boost::beast::file_mode::write, ec);
                http::async_read(stream_, buffer_, response_file,
                             beast::bind_front_handler(
                                     &Client::handle_read,
                                     shared_from_this()));

            }

        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_read(beast::error_code err,
                              std::size_t bytes_transferred)
    {
        std::cout << "In writing" << std::endl;
        std::cout << flag << std::endl;
        if (!err)
        {
            if(flag=="chat"){

                    http::async_write(stream_, request_,
                             beast::bind_front_handler(
                                     &Client::handle_write,
                                     shared_from_this()));
            }else{
                http::request_serializer<http::file_body,  http::fields> sre{ request_file};
                http::async_write(stream_, sre,
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

    void handle_write(beast::error_code err,
                     std::size_t bytes_transferred)
    { 
        
         std::cout << "In reading" << std::endl;
         std::cout << flag << std::endl;
         //std::cout << close << std::endl;
        if (!err)
        {
            if (flag=="chat"){
                http::async_read(stream_, buffer_, response_,
                             beast::bind_front_handler(
                                     &Client::handle_read,
                                     shared_from_this()));
              std::cout << response_.body() << std::endl;  
            }else{
                beast::error_code ec;
                response_file.body().open("/home/liza/Documents/boost.beast/new.txt", 
                                                boost::beast::file_mode::write, ec);
                http::async_read(stream_, buffer_, response_file,
                             beast::bind_front_handler(
                                     &Client::handle_read,
                                     shared_from_this()));

            }

                        
        }
        else
        {           
            std::cout << "Error: " << err << "\n";
            do_close();
        }
        //if(close)
          // {return do_close();}
    }

    void do_close()
        {
            beast::error_code ec;
            stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

        }
   



private:
    tcp::resolver resolver_;
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_; // (Must persist between reads)
    std::string flag = "chat";
    http::request<http::file_body> request_file;
    http::request<http::string_body> request_first;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
    http::request<http::file_body> response_file;
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
            if (argc<3){
                std::cout << "Error data. Please try again" << "\n";
            }
            else{
                    std::make_shared<Client>(io_context)->run_first(server,port,argv[1],argv[2]);
            }
            std::cout<< argc <<std::endl;
            if(argc == 2){       
            std::make_shared<Client>(io_context)->run(server,port,argv[1],body);
            }
            else{
                std::cout<<argv[2]<<std::endl;
                if (argv[2] == "file"){
                    std::cout<<"you use file"<<std::endl;
                    std::make_shared<Client>(io_context)->run_file(server,port,argv[1],argv[2],
                                                                "/home/liza/Documents/boost.beast/text.txt");
                    
                }

                std::stringstream ss;
                for(int i = 2;i<argc;i++){
                    ss<<argv[i]<<" ";
                }
                std::cout<<"you use chat"<<std::endl;
                std::make_shared<Client>(io_context)->run(server,port,argv[1],ss.str());
            }
            io_context.run();
            //boost::asio::io_service io;

            //boost::asio::deadline_timer t(io, boost::posix_time::seconds(25));
            ///t.wait();
        }
        catch (std::exception& e)
        {
            std::cout << "Exception: " << e.what() << "\n";
        }
   
    return 0;
}
