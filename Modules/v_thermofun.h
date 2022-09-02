#pragma once

#include "string"
#include "vector"
#include "GEMS3K/v_detail.h"
#include <QJsonObject>
#include <QJsonArray>

std::string thf_string( const char* value, size_t size );

template< class T>
QJsonObject thf_object( const char* key, const T& value )
{
    QJsonObject converted;
    converted[key] = value;
    return converted;
}

template< class T>
T thf_value( const T& value )
{
    if( value > 0 && std::numeric_limits<T>::min() >= value )
       return 0;
    return value;
}

QJsonObject thf_object( const char* key, const char* value );

QJsonObject thf_object( const char* key, const std::string& value );


template< class T>
QJsonObject thf_object( const char* key, const std::vector<T>& values )
{
    QJsonObject converted;
    converted[key] = QJsonArray(values);
    return converted;
}

template< class T>
QJsonObject thf_object( const char* key, const T* arr_values, size_t arr_size )
{
    bool all_zero = false; //true;
    if( !arr_values )
        return QJsonObject();

    QJsonArray arr;
    for( size_t ii=0; ii<arr_size; ii++ )
    {
        if( noZero(arr_values[ii]))
            all_zero = false;
        arr.push_back(arr_values[ii]);
    }
    if( all_zero )
        return QJsonObject();
    return QJsonObject({{ key, arr}});
}

template< class T>
QJsonObject thf_object_column( const char* key, const T* arr_values, size_t row_size, size_t col_size, size_t column )
{
    if( !arr_values )
      return QJsonObject();

    QJsonArray arr;
    for( size_t ii=0; ii<row_size; ii++ )
        arr.push_back(arr_values[ii*col_size+column]);
    return QJsonObject({{ key, arr}});
}


QJsonObject thf_convert_icomp_class( char type );
