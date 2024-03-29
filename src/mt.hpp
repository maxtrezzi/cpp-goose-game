#ifndef MT_H
#define MT_H

#include <stdio.h>

#include <memory>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <string>
#include <cstdarg>
#include <cstring>


namespace mt {

  class NonAssignable {
  public:
    NonAssignable(NonAssignable const&) = delete;
    NonAssignable& operator=(NonAssignable const&) = delete;
    NonAssignable() {}
  };

  template<typename... Args>
  inline std::string string_format( const std::string& format, Args &&...args ) {
      std::size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
      std::unique_ptr<char[]> buf( new char[ size ] );
      snprintf( buf.get(), size, format.c_str(), args ... );
      return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
  }

  template<class C>
  class const_range {
  public:
    inline const_range(const C* c) : c(c) {};
    inline auto begin() {return c->cbegin();};
    inline auto end() {return c->cend();};
  private:
    const C* c;
  };

  // trim from start (in place)
  inline void ltrim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(),
              std::not1(std::ptr_fun<int, int>(std::isspace))));
  }

  // trim from end (in place)
  inline void rtrim(std::string &s) {
      s.erase(std::find_if(s.rbegin(), s.rend(),
              std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  }

  // trim from both ends (in place)
  inline void trim(std::string &s) {
      ltrim(s);
      rtrim(s);
  }

  // trim from start (copying)
  inline std::string ltrim_copy(std::string s) {
      ltrim(s);
      return s;
  }

  // trim from end (copying)
  inline std::string rtrim_copy(std::string s) {
      rtrim(s);
      return s;
  }

  // trim from both ends (copying)
  inline std::string trim_copy(std::string s) {
      trim(s);
      return s;
  }
}


#endif
