#include <DataFileIterator.h>
#include <DataFile.h>

#include <iostream>


FieldIterator::FieldIterator(const SerializedObject& obj) 
: _obj(obj) 
{ 
	_iter = _obj.dictionary.begin(); 
}

void FieldIterator::Reset() 
{ 
	_iter = _obj.dictionary.begin(); 
}

bool FieldIterator::Next() 
{ 
	if( _iter != _obj.dictionary.end() ) 
		++_iter;
	if( _iter != _obj.dictionary.end() ) 
		return true; 
	return false; 
}

bool FieldIterator::Good() const 
{ 
	if(_obj.dictionary.empty()) 
		return false; 
	if( _iter != _obj.dictionary.end() ) 
		return true; 
	
	return false; 
}

const std::string& FieldIterator::GetKey() const 
{ 
	return _iter->first; 
}


/*
static std::vector< ComponentObject > _objs;

BEGIN_COMPONENT( Uber )
        virtual void Serialize(SerializedObject& obj)
        {

            std::cerr<<"building an uber\n";
            std::string fancyname ="";
            float pos[3];
            int num_vals;
            std::vector<int> vals;

			//SERIALIZE_MEMBER(obj,"fancyname",fancyname)
			//SERIALIZE_MEMBER(obj,"posa",pos)
			//SERIALIZE_MEMBER(obj,"vals",num_vals)
            //vals.resize(num_vals);
			//SERIALIZE_MEMBER(obj,"vals_list",&vals[0])

            obj.GetValue("fancyname",fancyname);
            obj.GetValue("pos",pos);
            obj.GetValue("vals",num_vals);
            vals.resize(num_vals);
            obj.GetValue("vals_list",&vals[0]);

            std::cerr<< fancyname <<"\n";
            std::cerr<< pos[0]<<"," <<pos[1]<<"," <<pos[2] <<"\n";
            std::cerr<< num_vals <<"\n";
            for(unsigned i = 0; i < vals.size(); ++i)
                std::cerr<< vals[i] <<", ";
        }
END_COMPONENT( Uber )

BEGIN_COMPONENT( Foo )
        virtual void Serialize(SerializedObject& obj)
        {
            std::cerr<<"building a foo\n";
            std::string name = "";
            std::vector<int> ids;
            FieldIterator iter(obj);
            for(; iter.Good(); iter.Next() )
            {
                std::string field_name = iter.GetKey();
                if( field_name == "name" ) {
                    obj.GetValue(field_name, name);
                    continue;
                    }
                int id = 0;
                obj.GetValue(field_name, id);
                ids.push_back(id);
            }

            std::cerr<< name <<"\n";
            for(unsigned i = 0; i < ids.size(); ++i)
                std::cerr<< ids[i] <<"\n";
        }
END_COMPONENT( Foo )

void LoadObjects(const std::string& file)
{
    DataFile objects;
    if(!objects.Load(file))
        return;

    std::cerr<<"load successful\n";

    ObjectIterator<DataFile> iter(objects);

    for(; iter.Good(); iter.Next() )
    {
        SerializedObject& obj = iter.Get();
        ComponentObject c_obj = ComponentObject(obj.GetName());
        c_obj.SerializeComponents(obj);
        _objs.push_back( c_obj );
    }
}
*/