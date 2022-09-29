#pragma once

//Helpers I use in my programs
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <cmath>
#include <list>
#include <map>
#include <assert.h>

///NOTE: on windows do not pass stringstreams as arguments to the debug macros. they don't display correctly (or at all really)

#ifdef _WIN32 //windows stuff
#define __PRETTY_FUNCTION__ __FUNCSIG__
#define KC_RESET_COLOR    "\n"
#define KC_FUNCTION_COLOR ""
#define KC_DEFAULT_COLOR  ""

#define KC_OSTREAM_RESET    ""
#define KC_OSTREAM_BOLD     ""
#define KC_OSTREAM_UNDERLINE ""
#define KC_OSTREAM_BLACK    ""
#define KC_OSTREAM_RED       ""
#define KC_OSTREAM_GREEN     ""
#define KC_OSTREAM_YELLOW   ""
#define KC_OSTREAM_BLUE     ""
#define KC_OSTREAM_PINK      ""
#define KC_OSTREAM_CYAN      ""
#define KC_OSTREAM_WHITE    ""

#define KC_DEBUG_HEADER "= DEBUG ="
#define KC_ERROR_HEADER "== ERROR =="
#define KC_WARNING_HEADER "= WARNING ="

#define KC_LINE_END "\r\n"

///Set output type here (for windows dll builds always use kc::GetDebugWriter().Get() )
///For dedicated server builds or local debugging use std::cerr
#define KC_DEFAULT_OSTREAM kc::GetDebugWriter().Get()

#else //non-windows stuff
#define KC_RESET_COLOR    "\x1b[0m"
#define KC_FUNCTION_COLOR "\x1b[36m"
#define KC_DEFAULT_COLOR  "\x1b[37m"

#define KC_OSTREAM_RESET     "\x1b[0m"
#define KC_OSTREAM_BOLD      "\x1b[1m"
#define KC_OSTREAM_UNDERLINE "\x1b[4m"
#define KC_OSTREAM_BLACK     "\x1b[30m"
#define KC_OSTREAM_RED       "\x1b[31m"
#define KC_OSTREAM_GREEN     "\x1b[32m"
#define KC_OSTREAM_YELLOW    "\x1b[33m"
#define KC_OSTREAM_BLUE      "\x1b[34m"
#define KC_OSTREAM_PINK      "\x1b[35m"
#define KC_OSTREAM_CYAN      "\x1b[36m"
#define KC_OSTREAM_WHITE     "\x1b[37m"

#define KC_DEBUG_HEADER "\x1b[32m= DEBUG =\x1b[0m"
#define KC_ERROR_HEADER "\x1b[1;31m== ERROR ==\x1b[0m"
#define KC_WARNING_HEADER "\x1b[33m= WARNING =\x1b[0m"

#define KC_LINE_END "\n"

///Set output type here
#define KC_DEFAULT_OSTREAM std::cerr

#endif

#define GET_MACRO(_1,_2,NAME,...) NAME

//normal output
#define DEBUG_STACKTRACE_PRINT kc::StackPrinter do_print(KC_DEFAULT_OSTREAM,__PRETTY_FUNCTION__);

#define KC_ERROR( msg ) do{ \
KC_DEFAULT_OSTREAM<<KC_ERROR_HEADER<<" "<<KC_FUNCTION_COLOR<<__PRETTY_FUNCTION__<<KC_DEFAULT_COLOR<<" "<<msg <<KC_LINE_END; KC_DEFAULT_OSTREAM<<KC_RESET_COLOR;}while(0)

#define KC_WARNING( msg ) do{ \
KC_DEFAULT_OSTREAM<<KC_WARNING_HEADER<<" "<<KC_FUNCTION_COLOR<<__PRETTY_FUNCTION__<<KC_DEFAULT_COLOR<<" "<<msg <<KC_LINE_END; KC_DEFAULT_OSTREAM<<KC_RESET_COLOR;}while(0)

#define KC_MSG(...) GET_MACRO(__VA_ARGS__, KC_MSG2, KC_MSG1)(__VA_ARGS__)

#define KC_MSG1( msg ) do{ \
KC_DEFAULT_OSTREAM<<KC_DEBUG_HEADER<<" "<<KC_FUNCTION_COLOR<<__PRETTY_FUNCTION__<<KC_DEFAULT_COLOR<<" "<<msg; KC_DEFAULT_OSTREAM<<KC_RESET_COLOR;}while(0)

#define KC_MSG2( color, msg ) do{ \
KC_DEFAULT_OSTREAM<<KC_DEBUG_HEADER<<" "<<KC_FUNCTION_COLOR<<__PRETTY_FUNCTION__<<color<<" "<<msg; KC_DEFAULT_OSTREAM<<KC_RESET_COLOR;}while(0)

#define KC_PRINT_BITS( type, v ) do{ \
KC_DEFAULT_OSTREAM<<KC_DEBUG_HEADER<<" "<<KC_FUNCTION_COLOR<<__PRETTY_FUNCTION__<<KC_DEFAULT_COLOR<<KC_LINE_END<<"\tprinting bits of " <<#v <<":"; \
for(int i = (8*sizeof(type))-1; i >= 0; --i) \
{ \
  if( (1 << i) & v ) \
    KC_DEFAULT_OSTREAM<<"1"; \
  else \
    KC_DEFAULT_OSTREAM<<"0"; \
  \
  if( (i % 8 == 0) && (i != 0) ) KC_DEFAULT_OSTREAM<<"-"; \
} \
KC_DEFAULT_OSTREAM<<KC_LINE_END; \
KC_DEFAULT_OSTREAM<<KC_RESET_COLOR;}while(0)

//use to do the same as KC_PRINT but it appends whatever text you put first
#define KC_LABEL( label, v ) do{ \
KC_DEFAULT_OSTREAM<<KC_DEBUG_HEADER<<" "<<KC_FUNCTION_COLOR<<__PRETTY_FUNCTION__<<KC_DEFAULT_COLOR<<" "<<label <<v <<KC_LINE_END; KC_DEFAULT_OSTREAM<<KC_RESET_COLOR;}while(0)

#define KC_PRINT( v ) do{ \
KC_DEFAULT_OSTREAM<<KC_DEBUG_HEADER<<" "<<KC_FUNCTION_COLOR<<__PRETTY_FUNCTION__<<KC_DEFAULT_COLOR<<" "<<#v<<" = " <<v <<KC_LINE_END; KC_DEFAULT_OSTREAM<<KC_RESET_COLOR;}while(0)

#define KC_PRINT_FORCEINT( v ) do{ \
KC_DEFAULT_OSTREAM<<KC_DEBUG_HEADER<<" "<<KC_FUNCTION_COLOR<<__PRETTY_FUNCTION__<<KC_DEFAULT_COLOR<<" "<<#v<<" = " <<(int)(v) <<KC_LINE_END; KC_DEFAULT_OSTREAM<<KC_RESET_COLOR;}while(0)

#define KC_PRINT_ARRAY( v, num ) do{ \
KC_DEFAULT_OSTREAM<<KC_DEBUG_HEADER<<" "<<KC_FUNCTION_COLOR<<__PRETTY_FUNCTION__<<KC_DEFAULT_COLOR<<KC_LINE_END<<#v<<" ["<<#num<<"] = "<<KC_LINE_END; \
for(unsigned i = 0; i < num; ++i) KC_DEFAULT_OSTREAM<<"  "<<#v<<"["<<i<<"] = "<<v[i] <<KC_LINE_END; KC_DEFAULT_OSTREAM<<KC_RESET_COLOR;}while(0)

#define KC_PRINT_ARRAY_FORCEINT( v, num ) do{ \
KC_DEFAULT_OSTREAM<<KC_DEBUG_HEADER<<" "<<KC_FUNCTION_COLOR<<__PRETTY_FUNCTION__<<KC_DEFAULT_COLOR<<KC_LINE_END<<#v<<" ["<<#num<<"] = "<<KC_LINE_END; \
for(unsigned i = 0; i < num; ++i) KC_DEFAULT_OSTREAM<<"  "<<#v<<"["<<i<<"] = "<<(int)(v[i]) <<KC_LINE_END; KC_DEFAULT_OSTREAM<<KC_RESET_COLOR;}while(0)

#define KC_WATCH( ostream, type, var, val ) do{ \
static kc::WatchVar< type > s__watchvar__(ostream,#var,var,val); s__watchvar__.Check(var);  }while(0)

#define KC_WATCH_RANGE( ostream, type, var, from, to ) do{ \
static kc::WatchVar< type > s__watchvar__(ostream,#var,var,from,to); s__watchvar__.Check(var);  }while(0)

#define KC_WATCH_CHANGE( ostream, type, var ) do{ \
static kc::WatchVarChanged< type > s__watchvar__(ostream,#var,var); s__watchvar__.Check(var);  }while(0)

#define KC_WATCH_CHANGE_CB( callback, data, type, var ) do{ \
static kc::WatchVarChanged< type > s__watchvar__(callback,var); s__watchvar__.Check(data,var);  }while(0)


#define KC_STATIC_PRINT( ostream, type, var ) \
static kc::StaticNotify< type > runonce(ostream,#var,var); 

#define KC_STATIC_PRINT_FORCEINT( ostream, var ) \
static kc::StaticNotify< int > runonce(ostream,#var,(int)(var)); 


#define KC_TRACK_AVERAGE_OF( type, var, outfile, rawfile, label, rate ) do{ \
static kc::LongRunAverager< type > s__averager__(outfile,rawfile,label,rate); s__averager__.Track(var);  }while(0)

#define KC_TRACK_DATA_OF( type, var, rawfile, label, rate ) do{ \
static kc::LongRunAverager< type > s__averager__("",rawfile,label,rate); s__averager__.Track(var);  }while(0)


//Everything in here is a personal debug tool
namespace kc
{ 
  struct DebugWriter
  {
    DebugWriter()
    {
      fout.open("./Server_DebugOutput.txt", std::ios::out);
    }
    std::fstream& Get() {
      fout.flush();
      return fout;
    }
    ~DebugWriter(){
      fout.close();
    }
    std::fstream fout;
  };
  DebugWriter& GetDebugWriter();
  
  struct NullWriter
  {
    NullWriter()
    {
      null_stream.setstate( std::ios::badbit );
    }
    
    std::ostream& Get() {
      //null_stream.flush();
      return null_stream;
    }
    std::fstream null_stream;
  };
  NullWriter& GetNullWriter();

  template <typename T>
  struct WatchVar
  {
    WatchVar(std::ostream& out, std::string name, T var, const T val)
    :out_(out), name_(name), prev_(var), from_(val), to_(val){}
    
    WatchVar(std::ostream& out, std::string name, T var, const T from, const T to)
    :out_(out), name_(name), prev_(var), from_(from), to_(to){}
    
    void Check(T var) const { if( (var < prev_ || var > prev_) && (var >= from_ && var <= to_) ) Report(var); }
  private:
    void Report(T var) const { out_ << KC_DEBUG_HEADER << "\x1b[33m WatchVar (\x1b[1;36m"<<from_ <<"\x1b[0m, \x1b[1;36m" <<to_ <<")\x1b[33m Triggered : " << name_ <<" = \x1b[0m" <<var <<"\x1b[33m. Previous Value = \x1b[0m"<<prev_ <<KC_LINE_END; prev_ = var; }
    std::ostream& out_;
    const std::string name_;
    T prev_;
    const T from_;
    const T to_;
  };
  
  
  template <typename T>
  struct StaticNotify
  {
    StaticNotify(std::ostream& out, std::string name, const T var)
    {
      out << KC_DEBUG_HEADER << KC_OSTREAM_YELLOW << " ::: Printing value of ("<<name<<") at static initialization ::: "<<KC_LINE_END;
      out << KC_DEBUG_HEADER << KC_OSTREAM_BOLD << KC_OSTREAM_RED << name << KC_OSTREAM_WHITE << " = " <<KC_OSTREAM_YELLOW<<" "<<var <<KC_LINE_END; out<<KC_OSTREAM_RESET;
    }
  };
  
  template <typename T>
  struct WatchVarChanged
  {
    WatchVarChanged(std::ostream& out, std::string name, T var)
    :out_(out), name_(name), prev_(var), callback_(0){}
    
    WatchVarChanged(void (*callback)(void* data,T p,T n), T var)
    :out_(std::cerr), name_(""), prev_(var), callback_(callback){}

    void Check(T var) { if( var != prev_ ) Report(var); prev_ = var; }
    void Check(void* data,T var) { if( var != prev_ ) Report(data,var); prev_ = var; }
  private:
    void Report(T var) {  out_ << KC_DEBUG_HEADER << KC_OSTREAM_YELLOW << " WatchVarChanged Triggered : "<<name_ <<KC_OSTREAM_YELLOW <<" ( Prev = " <<KC_OSTREAM_CYAN <<prev_  <<KC_OSTREAM_YELLOW <<" : New = " <<KC_OSTREAM_CYAN <<var <<KC_OSTREAM_YELLOW <<" )" << KC_OSTREAM_RESET <<KC_LINE_END; }
    void Report(void* data,T var) { callback_(data, prev_, var); }
    
    std::ostream& out_;
    const std::string name_;
    T prev_;
    void (*callback_)(void* data,T p,T n);
  };
  
  struct StackPrinter
  {
    StackPrinter(std::ostream& out, const std::string& function_name)
    :function_name_(function_name)
    ,out_(out)
    {
       out_<<KC_DEBUG_HEADER<<" Entering "<<KC_FUNCTION_COLOR<<function_name_<<KC_LINE_END; KC_DEFAULT_OSTREAM<<KC_RESET_COLOR;
    }
    ~StackPrinter()
    {
       out_<<KC_DEBUG_HEADER<<" Exiting "<<KC_FUNCTION_COLOR<<function_name_<<KC_LINE_END; KC_DEFAULT_OSTREAM<<KC_RESET_COLOR;
    }
    std::string function_name_;
    std::ostream& out_;
  };
  
  template <typename T>
  struct LongRunAverager
  {
    //Example:
    //KC_TRACK_AVERAGE_OF( int, value_to_track, "/file_one", "/file_one_RAW", " Average Data ", 1000 ); //last value = count needed to write to file
  
    LongRunAverager(const std::string& out_file, const std::string& raw_file, const std::string& label, unsigned long long rate, bool data_only=false)
    :input_filename_(raw_file)
    ,label_(label)
    ,calc_rate_(rate)
    ,count_(0)
    ,max_per_file_(10000000)
    ,data_only_(data_only)
    {      
#if EGM_BUILD == 1
      return;
#endif
      if( !data_only_ )
        output_.open( out_file.c_str(), std::ios::out );
      
        std::stringstream input_name;
        input_name << input_filename_ << 0;
        
      input_.open( input_name.str().c_str(), std::ios::out | std::ios::trunc );
      
      if( !data_only_ )
        output_ << "Starting data collection with rate : "<<rate <<KC_LINE_END;
      
    }
    
    void Track( T var )
    {     
#if EGM_BUILD == 1
      return;
#endif
      input_ << var <<" ";
      ++count_;
      
      if( count_ % 10000 )
        input_.flush();
      
      if( count_ % calc_rate_ == 0 )
      {
        input_.flush();
        if( !data_only_ )
          CalculateAverage();
      }
      
      if( count_ % max_per_file_ == 0 )
      {
        input_.flush();
        input_.close();
        
        unsigned long long num_files = (count_ / max_per_file_);
        std::stringstream new_input;
        new_input << input_filename_ << num_files;
        input_.open( new_input.str().c_str(), std::ios::in | std::ios::trunc );
      }
    }
    
    /* // old single file way
    void CalculateAverage()
    {
      if( count_ == 0 )
        return;
      
      input_.close();
      input_.open( input_filename_.c_str(), std::ios::in );
      
      long double running_total_avg = 0;
      while( input_.good() )
      {
        long double temp = 0;
        input_ >> temp;
        running_total_avg += temp / count_;
      }
      
      output_ << "Count: "<<count_ <<" | " << label_ << " | Average = " << running_total_avg <<"\n";
      output_.flush();
      
      input_.close();
      input_.open( input_filename_.c_str(), std::ios::out | std::ios::app );
    }
    */
    
    void CalculateAverage()
    {
      if( data_only_ )
        return;
        
      if( count_ == 0 )
        return;
      
      unsigned add_one = 1;
      if( count_ % max_per_file_ == 0 )
        add_one = 0;
      
      input_.close();
      
      unsigned long long num_files = (count_ / max_per_file_);
      
      long double running_total_avg = 0;
      
      unsigned long long i = 0;
      for( ; i < num_files; ++i )
      {
        std::stringstream file_to_avg;
        file_to_avg << input_filename_ << i;
        
        input_.open( file_to_avg.str().c_str(), std::ios::in );
        
        unsigned long long data_count = max_per_file_;
        
        long double file_avg = 0;
        AverageFile( file_avg, input_, data_count );
        running_total_avg += file_avg / (num_files+add_one);
        
        input_.close();
      }
      
      std::stringstream last_file_to_avg;
      last_file_to_avg << input_filename_ << i;
      input_.open( last_file_to_avg.str().c_str(), std::ios::in );
      
      long double file_avg = 0;
      unsigned long long data_count = count_ % max_per_file_;
      AverageFile( file_avg, input_, data_count );
      running_total_avg += file_avg / (num_files+add_one);
      
      output_ << "Count: "<<count_ <<" | " << label_ << " | Average = " << running_total_avg <<KC_LINE_END;
      output_.flush();
      
      input_.close();
      input_.open( last_file_to_avg.str().c_str(), std::ios::out | std::ios::app );
    }
    
    void AverageFile(long double& out_avg, std::fstream& data, unsigned long long num_datas)
    {
      if( data_only_ )
        return;
      
      out_avg = 0;
      while( data.good() )
      {
        long double temp = 0;
        data >> temp;
        out_avg += temp / num_datas;
      }
    }
    
    std::string input_filename_;
    std::string label_;
    unsigned long long calc_rate_;
    unsigned long long count_;
    unsigned long long max_per_file_;
    std::fstream output_;
    std::fstream input_;
    bool data_only_;
  };
  
  
  
  
  
  
  
  
}







//Tools I have created that I feel can go in a release
namespace Tools
{
  /////////////
  //Tool:
  //Usage:
  //Notes:
  
  /////////////
  //Tool: Get array size at compile time
  //Usage: const unsigned array_size = GETARRAYSIZE( array );
  //Notes: Must be used on a variable like this " int somearray[] = [....] "
  //       Pointers will not work and un-initialized arrays will not work.
  //       ...so you can only use it on an array that has been initialized (ie. size known).
  //      Reminder: Macros are not part of namespaces, so just use the macro name below
  template <typename T, int N> char(&dim_helper(T(&)[N]))[N];
  #define TOOL_GETARRAYSIZE(x) (sizeof(Tools::dim_helper(x)))
  
  /////////////
  //Tool: Something used to compare floats for "equality"
  // Just looks for a tiny difference between them
  bool FloatsEqual(float one, float two);
    
  /////////////
  //Tool: JumpConsistentHash algorithm
  // Input: 64-bit key, number of buckets
  // Output: Bucket index
  int JCH(unsigned long long key, int num_buckets);
  
  void DumpAllSprites();
}






































