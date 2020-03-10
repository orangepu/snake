#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "message.hpp"

const int MAX_X=150;
const int MAX_Y=120;

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

using message_queue = std::deque<message>;

//----------------------------------------------------------------------

struct food{
    int x;
    int y;
};

class participant
{
public:
  virtual ~participant() {}
  virtual void deliver(const message& msg) = 0;
};

typedef boost::shared_ptr<participant> participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
  void join(participant_ptr participant)
  {
    participants_.insert(participant);

    std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
        boost::bind(&participant::deliver, participant, _1));          //加入就发送所有消息
  }

  void leave(participant_ptr participant)
  {
    participants_.erase(participant);
  }

  void deliver(const message& msg)
  {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    std::for_each(participants_.begin(), participants_.end(),
        boost::bind(&participant::deliver, _1, boost::ref(msg)));      //某一客户端发送消息后广播
  }

private:
  std::set<participant_ptr> participants_;
  enum { max_recent_msgs =  1};
  message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class session
  : public participant,
    public boost::enable_shared_from_this<session>
{
public:
  session(boost::asio::io_service& io_service, chat_room& room, std::list<food> &Food)
    : socket_(io_service),
      room_(room),
      Food_(Food)
  {
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    room_.join(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), message::header_length),
        boost::bind(
          &session::handle_read_header, shared_from_this(),
          boost::asio::placeholders::error));
  }

  void deliver(const message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      boost::asio::async_write(socket_,
          boost::asio::buffer(write_msgs_.front().data(),
            write_msgs_.front().length()),
          boost::bind(&session::handle_write, shared_from_this(),
            boost::asio::placeholders::error));
    }
  }

  void handle_read_header(const boost::system::error_code& error)
  {
    if (!error && read_msg_.decode_header())
    {
      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
          boost::bind(&session::handle_read_body, shared_from_this(),
            boost::asio::placeholders::error));
    }
    else
    {
      room_.leave(shared_from_this());
    }
  }

  void handle_read_body(const boost::system::error_code& error)
  {
    if (!error)
    {
        if(read_msg_.data()[message::header_length]=='1')
            room_.deliver(read_msg_);
        else if(read_msg_.data()[message::header_length]=='0'){
            if(read_msg_.changeFood())
            {
                for(auto it:Food_)
                {
                    if(it.x==read_msg_.food[0] and
                            it.y==read_msg_.food[1])
                    {
                        it.x = rand()/MAX_X;
                        it.y = rand()/MAX_Y;
                    }
                }
            }

        }
      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.data(), message::header_length),
          boost::bind(&session::handle_read_header, shared_from_this(),
            boost::asio::placeholders::error));
    }
    else
    {
      room_.leave(shared_from_this());
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      write_msgs_.pop_front();
      if (!write_msgs_.empty())
      {
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_msgs_.front().data(),
              write_msgs_.front().length()),
            boost::bind(&session::handle_write, shared_from_this(),
              boost::asio::placeholders::error));
      }
    }
    else
    {
      room_.leave(shared_from_this());
    }
  }

private:
  tcp::socket socket_;
  chat_room& room_;
  message read_msg_;
  message_queue write_msgs_;
  std::list<food> Food_;
};

typedef boost::shared_ptr<session> session_ptr;

//----------------------------------------------------------------------

class server
{
public:
  server(boost::asio::io_service& io_service,
      const tcp::endpoint& endpoint)
    : io_service_(io_service),
      acceptor_(io_service, endpoint)
  {
    session_ptr new_session(new session(io_service_, room_, Food));
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

  void handle_accept(session_ptr session_,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      session_->start();
      session_ptr new_session(new session(io_service_, room_, Food));
      acceptor_.async_accept(new_session->socket(),
          boost::bind(&server::handle_accept, this, new_session,
            boost::asio::placeholders::error));
    }
  }

private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  chat_room room_;
  std::list<food> Food;
};

typedef boost::shared_ptr<server> chat_server_ptr;
typedef std::list<chat_server_ptr> chat_server_list;

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    boost::asio::io_service io_service;

    chat_server_list servers;
    for (int i = 1; i < argc; ++i)
    {
      using namespace std; // For atoi.
      tcp::endpoint endpoint(tcp::v4(), atoi(argv[i]));
      chat_server_ptr server_(new server(io_service, endpoint));
      servers.push_back(server_);
    }

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
