#include "connection.h"

#include <vector>
#include <boost/bind/bind.hpp>
#include <boost/beast/http/write.hpp>
#include <iostream>
#include <experimental/filesystem>
#include <string>
#include <sstream>

namespace server3 {
    
        std::stringstream ss;
        Connection::Connection(tcp::socket&& socket)
                : stream_(std::move(socket))
        {
        }

        void Connection::start()
        {
            net::dispatch(stream_.get_executor(),
                          beast::bind_front_handler(&Connection::do_read_first,
                                             shared_from_this()));

        }

        void Connection::do_read_first()
        {
            request_ = {};
            http::async_read(stream_, buffer_, request_,
                             beast::bind_front_handler(
                                     &Connection::handle_read_first,
                                     shared_from_this()));
           
            
        }


        void Connection::handle_read_first(beast::error_code e,
                                     std::size_t bytes_transferred)
        {     
            std::stringstream key_path;
            show_base(key_path);
            if (!e)
            {       if (request_.method()==http::verb::post)
                    {
                        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                        res.set(http::field::content_type, "application/text");
                        res.result(http::status::ok);
                        res.body() =  "file";
                        res.prepare_payload();

                        http::async_write(stream_,std::move(res), beast::bind_front_handler(
                                                            &Connection::handle_write_first,
                                                            shared_from_this()));
                    }
                    else{
                        if (request_.target()=="show")
                        {
                            std::stringstream key_path;
                            show_base(key_path);
                            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                            res.set(http::field::content_type, "application/text");
                            res.result(http::status::ok);
                            res.body() =key_path.str();
                            res.prepare_payload();

                             http::async_write(stream_,std::move(res), beast::bind_front_handler(
                                                            &Connection::handle_write_last,
                                                            shared_from_this()));

                        }else{
                                
                                beast::error_code ec;
                                http::file_body::value_type resp_body;
                                std::cout<<(local_base[request_.body()]).c_str()<<std::endl;
                                resp_body.open((local_base[request_.body()]).c_str(), beast::file_mode::read, ec);

                                // Handle the case where the file doesn't exist
                                if(ec == beast::errc::no_such_file_or_directory)
                                        std::cout << "Error: " << ec << "\n";

                                // Handle an unknown error
                                if(ec)
                                        std::cout << "Error: " << ec.message() << "\n";

                                // Cache the size since we need it after the move
                                auto const size = resp_body.size();

                                res_file.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                                res_file.set(http::field::content_type, "application/text");
                                res_file.body() =  std::move(resp_body);
                                res_file.content_length(resp_body.size());

                                http::async_write(stream_,res_file, beast::bind_front_handler(
                                                            &Connection::handle_write_last,
                                                            shared_from_this()));

                        }
                    }
            }else{
                
                std::cout << "Error: " << e.message() << "\n";
            }
                
;
        }


        void Connection::do_read()
        {
            beast::error_code ec;
            request_file.body().open("/home/liza/Documents/boost.beast/new.txt", 
                                                boost::beast::file_mode::write, ec);

            if(ec == beast::errc::no_such_file_or_directory)
                std::cout << "Error: " << ec << "\n";

            // Handle an unknown error
            if(ec)
                std::cout << "Error: " << ec.message() << "\n";;

            http::async_read(stream_, buffer_File, request_file,
                             beast::bind_front_handler(
                                     &Connection::handle_write_last,
                                     shared_from_this()));

            // Read a request
            
        }

/*
        void Connection::handle_read(beast::error_code e,
                                     std::size_t bytes_transferred)

        {               
            if( e == http::error::end_of_stream){
                 return do_close();
            }  
            if (!e)
            {
                if (!std::strcmp(flag.c_str(),"chat")){

                http::response<http::string_body> res{http::status::bad_request, 
                                                                    request_.version()};
                
                if (request_.body()!= ""){
                ss << "User:  "<< request_.target() << " Message: " << request_.body() << "\r\n\r\n";
                }
        
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "application/text");
                res.result(http::status::ok);
                res.body() =  ss.str();             
                res.prepare_payload();

                auto sp = std::make_shared<http::message<false, http::string_body>>(std::move(res));
                res_ = sp;
                http::async_write(stream_,
                        *sp, beast::bind_front_handler(
                                        &Connection::handle_write,
                                        shared_from_this(),
                                        sp->need_eof()));

                }
                // if (request_.method() == http::verb::post)
                else  {
                    ss << "User: "<< request_file.target()  << " Message: " << "voice message" << "\r\n\r\n";

                    beast::error_code ec;
                    http::file_body::value_type resp_body;
                    resp_body.open("/home/liza/Documents/boost.beast/new.txt", beast::file_mode::read, ec);

                    // Handle the case where the file doesn't exist
                    if(ec == beast::errc::no_such_file_or_directory)
                            std::cout << "Error: " << ec << "\n";

                    // Handle an unknown error
                    if(ec)
                            std::cout << "Error: " << ec.message() << "\n";

                    // Cache the size since we need it after the move
                    auto const size = resp_body.size();

                    res_file.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res_file.set(http::field::content_type, "application/text");
                    res_file.body() =  std::move(resp_body);
                    res_file.content_length(resp_body.size());

                    http::async_write(stream_,res_file, beast::bind_front_handler(
                                            &Connection::handle_write_last,
                                            shared_from_this()));
  
                    
                    }
                /*else
                {
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "application/json");
                    res.result(http::status::not_found);
                    res.body() = " Not found";
                }}

                else{
                     std::cout << "Error: " << e.message() << "\n";
                }
       
        
        }

        void Connection::handle_write(bool close,beast::error_code e,
                                      std::size_t bytes_transferred)
        {   
            if(close)
            {
                // This means we should close the connection, usually because
                // the response indicated the "Connection: close" semantic.
                return do_close();
            }

            if (!e)
            {
                do_read();
            }
        }*/
        void Connection::handle_write_first(beast::error_code e,
                                      std::size_t bytes_transferred)
        {   
            if (!e)
            {
                do_read();
            }
            else
            {           
                std::cout << "Error: " << e.message() << "\n";
                do_close();
            }
        }
        void Connection::handle_write_last(beast::error_code e,
                                      std::size_t bytes_transferred)
        {   
            if (!e)
            {
                do_close();
            }
            else
            {   std::cout << "Pampam " << "\n";
                do_close();      
                std::cout << "Error: " << e.message() << "\n";
            }
        }

        void Connection::do_close()
        {
            beast::error_code ec;
            stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

        }

        void Connection::show_base(std::stringstream& key_path)
        {
            
            if (!std::filesystem::exists("/home/liza/Documents/boost.beast/data_server")) {
            std::cout << "No such directory" <<std::endl;
            }

            for (const auto & entry : std::filesystem::directory_iterator("/home/liza/Documents/boost.beast/data_server")) 
            {
                if (entry.path().extension() == ".txt") 
                {
                local_base[entry.path().stem()] = entry.path();
                key_path << "{ key :"<< entry.path().stem()<<" path "<< entry.path()<<"\r\n";
                }
            }

        }

    } // namespace server3
