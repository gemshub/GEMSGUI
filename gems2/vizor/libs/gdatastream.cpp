
#include "gdatastream.h"
#include "verror.h"

//--------------------------------------------------------------------


GemDataStream::GemDataStream( gstring& aPath, ios::openmode aMod  ):
        mod( aMod ),
        Path( aPath ),
        byteorder( LittleEndian ),
        ff(aPath.c_str(), aMod)
{
      ErrorIf( !ff.good(), Path.c_str(), "Fileopen error");
}

GemDataStream::~GemDataStream()
{
}


void GemDataStream::setByteOrder( int bo )
{
    byteorder = bo;
/*    if ( systemBigEndian )
	noswap = byteorder == BigEndian;
    else
	noswap = byteorder == LittleEndian;
*/
}


GemDataStream &GemDataStream::operator>>( char &i )
{
    ff.read((char*)&i, sizeof(char));
    return *this;
}

GemDataStream &GemDataStream::operator>>( short &i )
{
    ff.read((char*)&i, sizeof(short));
    return *this;
}

GemDataStream &GemDataStream::operator>>( float &f )
{
    ff.read((char*)&f, sizeof(float));
    return *this;
}

GemDataStream &GemDataStream::operator>>( double &f )
{
    ff.read((char*)&f, sizeof(double));
    return *this;
}

GemDataStream &GemDataStream::operator<<( char i )
{
    ff.write((char*)&i, sizeof(char));
    return *this;
}


GemDataStream &GemDataStream::operator<<( short i )
{
    ff.write((char*)&i, sizeof(short));
    return *this;
}


GemDataStream &GemDataStream::operator<<( float f )
{
    ff.write((char*)&f, sizeof(float));
    return *this;
}


GemDataStream &GemDataStream::operator<<( double f )
{
    ff.write((char*)&f, sizeof(double));
    return *this;
}

void GemDataStream::readArray( char* arr, int size )
{
  if( !arr )
    return;
  for(int ii=0; ii<size; ii++)
   *this >> arr[ii];
}

void GemDataStream::readArray( short* arr, int size )
{
  if( !arr )
    return;
  for(int ii=0; ii<size; ii++)
   *this >> arr[ii];
}

void GemDataStream::readArray( float* arr, int size )
{
  if( !arr )
    return;
  for(int ii=0; ii<size; ii++)
   *this >> arr[ii];
}

void GemDataStream::readArray( double* arr, int size )
{
  if( !arr )
    return;
  for(int ii=0; ii<size; ii++)
   *this >> arr[ii];
}

void GemDataStream::writeArray( char* arr, int size )
{
  if( !arr )
    return;
  for(int ii=0; ii<size; ii++)
   *this << arr[ii];
}

void GemDataStream::writeArray( short* arr, int size )
{
  if( !arr )
    return;
  for(int ii=0; ii<size; ii++)
   *this << arr[ii];
}
void GemDataStream::writeArray( float* arr, int size )
{
  if( !arr )
    return;
  for(int ii=0; ii<size; ii++)
   *this << arr[ii];
}
void GemDataStream::writeArray( double* arr, int size )
{
  if( !arr )
    return;
  for(int ii=0; ii<size; ii++)
   *this << arr[ii];
}

