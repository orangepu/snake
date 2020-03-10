#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>

class message
{
public:
  enum { header_length = 4 };
  enum { max_body_length = 512 };

  message()
    : body_length_(0)
  {
  }

  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  size_t length() const
  {
    return header_length + body_length_;
  }

  const char* body() const
  {
    return data_ + header_length ;
  }

  char* body()
  {
    return data_ + header_length ;
  }

  size_t body_length() const
  {
    return body_length_;
  }

  void body_length(size_t length)
  {
    body_length_ = length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool decode_header()
  {
    using namespace std; // For strncat and atoi.
    char header[header_length + 1] = "";
    strncat(header, data_, header_length);
    body_length_ = atoi(header);
    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void encode_header(int i)
  {
    using namespace std; // For sprintf and memcpy.
    char header[header_length + 2] = "";
    if(i==1)
        header[header_length] = '1';
    else if(i==2)
        header[header_length] = '2';
    else
        header[header_length] = '0';
    sprintf(header, "%4d", body_length_);
    memcpy(data_, header, header_length);
  }

  bool changeFood()
  {
      using namespace std;
      char header[header_length + 1] = "";
      strncat(header, data_, header_length);
      body_length_ = atoi(header);
      if(body_length_==3)
      {
          food[0]=int(data_[header_length+1]);
          food[1]=int(data_[header_length+2]);
          return true;
      }
      return false;
  }
    int food[2];
private:
  char data_[header_length+ max_body_length];
  size_t body_length_;
};

#endif // CHAT_MESSAGE_HPP
