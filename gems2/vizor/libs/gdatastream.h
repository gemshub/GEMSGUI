
#ifndef _gemdatastream_h_
#define _gemdatastream_h_

#include <fstream>
#include "gstring.h"

class GemDataStream				// data stream class
{

    ios::openmode mod;
    gstring Path;

    int	byteorder;
    fstream ff;

public:
//    GemDataStream( fstream& ff  );
    GemDataStream( gstring& aPath, ios::openmode aMod  );
    virtual ~GemDataStream();

    const gstring& GetPath() const
    {
        return Path;
    }

//    bool	 atEnd() const;
//    bool	 eof() const;

    enum ByteOrder { BigEndian, LittleEndian };
    int	 byteOrder() const { return byteorder; };
    void setByteOrder( int );


    GemDataStream &operator>>( char &i );
    GemDataStream &operator>>( short &i );
    GemDataStream &operator>>( float &f );
    GemDataStream &operator>>( double &f );
//    GemDataStream &operator>>( char *&str );

    GemDataStream &operator<<( char i );
    GemDataStream &operator<<( short i );
    GemDataStream &operator<<( float f );
    GemDataStream &operator<<( double f );
//    GemDataStream &operator<<( const char *str );

    void readArray( char* arr, int size );
    void readArray( short* arr, int size );
    void readArray( float* arr, int size );
    void readArray( double* arr, int size );

    void writeArray( char* arr, int size );
    void writeArray( short* arr, int size );
    void writeArray( float* arr, int size );
    void writeArray( double* arr, int size );

};

#endif
