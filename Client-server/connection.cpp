#include "connection.h"

#include <vector>
#include <boost/bind/bind.hpp>
#include <boost/beast/http/write.hpp>
#include <iostream>

#include <string>
#include <sstream>

namespace server3 {
    
        std::vector <std::string> chat;
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
            //request_ = {};

            // Read a request
            //beast::error_code ec;
            //request_.body().open("/home/liza/Documents/boost.beast/new.txt", boost::beast::file_mode::write, ec);
            http::async_read(stream_, buffer_, request_,
                             beast::bind_front_handler(
                                     &Connection::handle_read_first,
                                     shared_from_this()));
        }


        void Connection::handle_read_first(beast::error_code e,
                                     std::size_t bytes_transferred)
        {
            http::response<http::string_body> res{http::status::bad_request, 
                                                                    request_.version()};
            if (!e)
            {
                
                    flag = request_.body();
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "application/text");
                    res.result(http::status::ok);
                    //res.keep_alive(request_.keep_alive()); ");
                    res.body() =  "ALL Good";
                    //res.body() = "Try doing this " + request_.body();
                    res.prepare_payload();


            }else{

                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "application/text");
                res.result(http::status::ok);
                //res.keep_alive(request_.keep_alive()); ");
                res.body() =  "ERROR";
                //res.body() = "Try doing this " + request_.body();
                res.prepare_payload();

                std::cout << "Error: " << e.message() << "\n";
        }

            auto sp = std::make_shared<http::message<false, http::string_body>>(std::move(res));
            res_ = sp;
                

                http::async_write(stream_,
                        *sp, beast::bind_front_handler(
                                        &Connection::handle_write,
                                        shared_from_this(),
                                        sp->need_eof()));
        }


        void Connection::do_read()
        {
            //request_ = {};
            if (flag == "chat"){
                http::async_read(stream_, buffer_, request_,
                             beast::bind_front_handler(
                                     &Connection::handle_read,
                                     shared_from_this()));
            }
            else{

            beast::error_code ec;
            request_file.body().open("/home/liza/Documents/boost.beast/new.txt", 
                                                boost::beast::file_mode::write, ec);
            http::async_read(stream_, buffer_, request_file,
                             beast::bind_front_handler(
                                     &Connection::handle_read,
                                     shared_from_this()));
            }
            // Read a request
            
        }


        void Connection::handle_read(beast::error_code e,
                                     std::size_t bytes_transferred)
        {   
            http::response<http::string_body> res;
            std::cout << "Welcome"<< std::endl;
            if( e == http::error::end_of_stream)
                return do_close();
            
            if (!e)
            {
                if (flag == "chat"){
                http::response<http::string_body> res{http::status::bad_request, 
                                                                    request_.version()};
                
                if (request_.body()!= ""){
                ss << "User:  "<< request_.target() << " Message: " << request_.body() << "\r\n\r\n";
                }
        
                std::cout << request_.body()<< std::endl;

                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "application/text");
                res.result(http::status::ok);
                //res.keep_alive(request_.keep_alive()); ");
                res.body() =  ss.str();
                //res.body() = "Try doing this " + request_.body();
                res.prepare_payload();

                auto sp = std::make_shared<http::message<false, http::string_body>>(std::move(res));
                res_ = sp;
                }
                // if (request_.method() == http::verb::post)
                else  {
             
                    http::response<http::string_body> res{http::status::bad_request, 
                                                                    request_.version()};
                
                if (request_.body()!= ""){
                ss << "User:  "<< request_.target() << " Message: " << "file" << "\r\n\r\n";
                }
        
                //std::cout << std::make_tuple(std::move(request_.body()))<< std::endl;
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "application/text");
                res.result(http::status::ok);
                //res.keep_alive(request_.keep_alive()); ");
                res.body() =  ss.str();
                //res.body() = "Try doing this " + request_.body();
                res.prepare_payload();

                auto sp = std::make_shared<http::message<false, http::string_body>>(std::move(res));
                res_ = sp;

                
                }
                /*else
                {
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "application/json");
                    res.result(http::status::not_found);
                    res.body() = " Not found";
                }*/
            auto sp = std::make_shared<http::message<false, http::string_body>>(std::move(res));
                res_ = sp;
                

                http::async_write(stream_,
                        *sp, beast::bind_front_handler(
                                        &Connection::handle_write,
                                        shared_from_this(),
                                        sp->need_eof()));
            }
        }

        void Connection::handle_write(bool close,
                                      beast::error_code e,
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
        }

        void Connection::do_close()
        {
            beast::error_code ec;
            stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

        }

    } // namespace server3
