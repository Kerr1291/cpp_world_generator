#ifndef DATAFILEITERATOR_H
#define DATAFILEITERATOR_H

#include <DataFile.h>

#include <map>
#include <string>
#include <vector>


typedef std::map< std::string, SerializedObject >::iterator DataObjectIter;

template <typename T>
class ObjectIterator
{
    public:
        ObjectIterator(T& obj) : _obj(obj) { iter = _obj.objects.begin(); }
        void Reset() { iter = _obj.objects.begin(); }
        bool Next() { if( iter != _obj.objects.end() ) ++iter; if( iter != _obj.objects.end() ) return true; return false; }
        bool Good() const { if(_obj.objects.empty()) return false; if( iter != _obj.objects.end() ) return true; return false; }
        SerializedObject& Get() { return iter->second; }
    private:
        T& _obj;
        std::map<std::string,SerializedObject>::iterator iter;
};

class FieldIterator
{
    public:
        FieldIterator(const SerializedObject& obj);
        void Reset();
        bool Next();
        bool Good() const;
        const std::string& GetKey() const;
    private:
        const SerializedObject& _obj;
        std::map< std::string, std::vector< std::string > >::const_iterator _iter;
};

#endif
