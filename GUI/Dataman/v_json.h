
#ifndef __v_json_h_
#define __v_json_h_

#include <fstream>
#include "v_vals.h"
#include "ejdb.h"


extern void print_bson_to_json(FILE *f, const bson *b);

/// Print bson object to text file
extern void print_bson_object_to_json(FILE *f, const bson *b);

enum {
    jsBeginArray = '[',    //0x5b,
    jsBeginObject = '{',   //0x7b,
    jsEndArray = ']',      //0x5d,
    jsEndObject = '}',     //0x7d,
    jsNameSeparator = ':', //0x3a,
    jsValueSeparator = ',',//0x2c,
    jsQuote = '\"'      //0x22
};

/// Class for read bson structure from text file
class ParserJson
{
protected:

  string jsontext;
  const char *curjson;
  const char *end;

  bool xblanc();
  void getString( string& str );
  void getNumber( double& value, int& type );
  void parseValue( const char *name, bson *brec );

 public:

  ParserJson():curjson(0), end(0)
    {}
  virtual ~ParserJson()
    {}

  /// Parse internal jsontext string to bson structure
  void parseObject( bson *brec );
  void parseArray( bson *brec );

  /// Read one json object from text file
  string readObjectText( fstream& fin );
  /// Set up internal jsontext before parsing
  void  setJsonText( string json );

};

/// Get string from bson structure by name
bool bson_find_string( const char *obj, const char *name, string& str );
/// Get array from bson structure by name
const char* bson_find_array( const char *obj, const char *name );
/// Get object from bson structure by name
const char* bson_find_object( const char *obj, const char *name );
/// Get time from bson structure by name
time_t bson_find_time_t( const char *obj, const char *name );

/// Get value from bson structure by name
template <class T>
bool bson_find_value( const char *obj, const char *name, T& value )
{
    bson_iterator it;
    bson_type type;
    type =  bson_find_from_buffer(&it, obj, name );

    switch( type )
    {
      // impotant datatypes
      case BSON_BOOL:
           value = bson_iterator_bool(&it);
           break;
      case BSON_INT:
           value = bson_iterator_int(&it);
           break;
      case BSON_LONG:
           value =  bson_iterator_long(&it);
           break;
      case BSON_DOUBLE:
           value = bson_iterator_double(&it);
           break;
     case BSON_NULL:
        // return false;
     default: // error
         // bson_errprintf("can't print type : %d\n", type);
         return false;
   }
   return true;
}

#endif	// __v_json_h_
