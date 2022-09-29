/*
//
// Parses much like JSON and can mostly handle the JSON format. JSON Arrays of pairs/objects do not handle properly.
// If you want to use JSON then avoid JSON-style arrays.
// If the data is a user define class then the method Serialize( std::vector< std::string > )
// must be written to handle the conversion from string data.
// GetValue returns true if the key was found, false otherwise.
//
// USAGE EXAMPLES:
//
//
//    DataFile data;
        data.Load("./test_data.txt");
        SerializedObject obj;
        data.GetObject("GO-01", obj);
        std::cout<<" result = " <<obj.HasObject("Tfm") <<"\n";
        SerializedObject tfm;
        obj.GetObject("Tfm", tfm);
        float pos[3];
        tfm.GetValue("pos",pos);
        std::cout << pos[0] <<"," << pos[1] <<"," << pos[2] <<"\n";
//
//
//
//
*/

#ifndef DATAFILE_H
#define DATAFILE_H
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#define SERIALIZE_VARIABLE( serialized_object, key_name, variable ) \
  if( serialized_object.HasValue( key_name ) ) \
          serialized_object.GetValue( key_name, variable ); \
  else \
          std::cerr<< "Cannot find '"<<key_name <<"' in object '"<<serialized_object.GetName() <<"'\n";

namespace LocalDefParser
{
  class Parser;
}
namespace LocalDefWriter
{
  class Writer;
}
class DataFile;

template <typename T>
class ObjectIterator;

class SerializedObject
{
    friend SerializedObject CreateEmptyObject(const std::string& name);
    friend class LocalDefParser::Parser;
    friend class LocalDefWriter::Writer;
    friend class ObjectIterator<SerializedObject>;
    friend class FieldIterator;
    friend void Start(LocalDefParser::Parser& parser);
    friend void ReadField(LocalDefParser::Parser& parser);
    public:
        SerializedObject();
        const std::string& GetName() const { return _name; }

        inline bool HasValue(const std::string& key);
        inline bool HasObject(const std::string& key);

        inline bool GetInnerObject(const std::string& key, SerializedObject& value);
        
        ///use this one for large objects
        inline bool GetInnerObject(const std::string& key, SerializedObject** value);
        
        inline bool GetNumValues(const std::string& key, unsigned& num);

        template <typename T>
        bool GetValue(const std::string& key, T& value);

        template <typename T>
        bool GetValue(const std::string& key, T* value);

        template <typename T>
        bool GetValue(const std::string& key, std::vector<T>& value);

        unsigned GetNumObjects() const { return objects.size(); }
        unsigned GetNumFields() const { return dictionary.size(); }
        
        ///methods for saving data
        SerializedObject& CreateOrGetObject(const std::string& object_name);
        
        ///use this for large objects
        SerializedObject* CreateOrGetObject_ReturnPtr(const std::string& object_name);
        
        template <typename T>
        void SetPODValue(const std::string& key, T value);
        
        template <typename T>
        void SetPODValue(const std::string& key, std::vector<T>& value);
        
        void DebugPrintValues();

    private:
        SerializedObject(const std::string& name);
        void AddValue(const std::string& key, const std::string& value);
        void AddValue(const std::string& key, std::vector< std::string > value);
        bool AddObject(const std::string& key, SerializedObject& value);

        std::string _name;
        std::map< std::string, SerializedObject > objects;
        std::map< std::string, std::vector< std::string > > dictionary;
};


inline bool SerializedObject::HasValue(const std::string& key)
{
    if( dictionary.find( key ) != dictionary.end() )
        return true;
    return false;
}
inline bool SerializedObject::HasObject(const std::string& key)
{
    if( objects.find( key ) != objects.end() )
        return true;
    return false;
}

inline bool SerializedObject::GetInnerObject(const std::string& key, SerializedObject& value)
{
    if( objects.find( key ) != objects.end() )
    {
        value = objects[ key ];
        return true;
    }
    return false;
}

inline bool SerializedObject::GetInnerObject(const std::string& key, SerializedObject** value)
{
    if(!value)
      return false;
  
    if( objects.find( key ) != objects.end() )
    {
        *value = &objects[ key ];
        return true;
    }
    return false;
}

inline bool SerializedObject::GetNumValues(const std::string& key, unsigned& num)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
      num = dictionary[ key ].size();
      return true;
    }
    return false;
}

/////////////////////////////////////////////////////////
// Specializations for POD or other simple data types

template <>
bool SerializedObject::GetValue<std::vector<std::string> >(const std::string& key, std::vector<std::string>& value);
template <>
bool SerializedObject::GetValue<std::vector<int> >(const std::string& key, std::vector<int>& value);

template <>
bool SerializedObject::GetValue<unsigned char>(const std::string& key, unsigned char& value);
template <>
bool SerializedObject::GetValue<char>(const std::string& key, char& value);
template <>
bool SerializedObject::GetValue<int>(const std::string& key, int& value);
template <>
bool SerializedObject::GetValue<unsigned int>(const std::string& key, unsigned int& value);
template <>
bool SerializedObject::GetValue<long>(const std::string& key, long& value);
template <>
bool SerializedObject::GetValue<short>(const std::string& key, short& value);
template <>
bool SerializedObject::GetValue<float>(const std::string& key, float& value);
template <>
bool SerializedObject::GetValue<bool>(const std::string& key, bool& value);
template <>
bool SerializedObject::GetValue<std::string>(const std::string& key, std::string& value);

template <>
bool SerializedObject::GetValue<float>(const std::string& key, float* value);
template <>
bool SerializedObject::GetValue<int>(const std::string& key, int* value);
template <>
bool SerializedObject::GetValue<unsigned int>(const std::string& key, unsigned int* value);
template <>
bool SerializedObject::GetValue<char>(const std::string& key, char* value);

//
/////////////////////////////////////////////////////////

template <typename T>
bool SerializedObject::GetValue(const std::string& key, std::vector<T>& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        for(size_t i = 0; i < dictionary[ key ].size(); ++i)
        {
            value.push_back( dictionary[ key ][i] );
        }
        return true;
    }
    return false;
}


template <typename T>
bool SerializedObject::GetValue(const std::string& key, T& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        value.Serialize( dictionary[ key ] );
        return true;
    }
    return false;
}

template <typename T>
bool SerializedObject::GetValue(const std::string& key, T* value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        value->Serialize( dictionary[ key ] );
        return true;
    }
    return false;
}

template <typename T>
void SerializedObject::SetPODValue(const std::string& key, T value)
{
    std::stringstream data;
    data << value;
    dictionary[key] = std::vector< std::string >(1);
    dictionary[key][0] = data.str();
}

template <typename T>
void SerializedObject::SetPODValue(const std::string& key, std::vector<T>& value)
{
    dictionary[key] = std::vector< std::string >(value.size());
    for(size_t i = 0; i < dictionary[ key ].size(); ++i)
    {
        std::stringstream data;
        data << value[i];
        dictionary[ key ][i] = data.str();
    }
}




class DataFile
{
    friend class LocalDefParser::Parser;
    friend class LocalDefWriter::Writer;
    friend class SerializedObject;
    friend class ObjectIterator<DataFile>;
    friend void Start(LocalDefParser::Parser& parser);
    friend void ReadField(LocalDefParser::Parser& parser);
    public:

        DataFile();

        bool HasObject(const std::string& key);
        bool GetFileObject(const std::string& key, SerializedObject& value);
        
        ///use this method for large objects
        bool GetFileObject(const std::string& key, SerializedObject** value);
        inline std::map< std::string, SerializedObject >::iterator GetFileIterator() { return objects.begin(); }

        //if true, JSON parsing format "errors" will not be reported
        bool Load(const std::string& filename, bool JSON = false);
        inline bool Loaded() const { return isGood; }
        
        
        //methods for writing data
        bool Save(const std::string& filename);
        SerializedObject& CreateOrGetObject(const std::string& object_name);
        
        SerializedObject* CreateOrGetObject_ReturnPtr(const std::string& object_name);

    private:
        bool AddObject(const std::string& key, SerializedObject& value);
        std::map< std::string, SerializedObject > objects;
        bool isGood;
};

inline bool DataFile::HasObject(const std::string& key)
{
    if( objects.find( key ) != objects.end() )
        return true;
    return false;
}

inline bool DataFile::GetFileObject(const std::string& key, SerializedObject& value)
{
    if( objects.find( key ) != objects.end() )
    {
        value = objects[ key ];
        return true;
    }
    return false;
}

inline bool DataFile::GetFileObject(const std::string& key, SerializedObject** value)
{
  if(!value)
    return false;
  
    if( objects.find( key ) != objects.end() )
    {
        *value = &objects[ key ];
        return true;
    }
    return false;
}

#endif
