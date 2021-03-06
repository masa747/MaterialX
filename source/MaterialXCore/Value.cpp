//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#include <MaterialXCore/Value.h>

#include <sstream>
#include <type_traits>

namespace MaterialX
{

Value::CreatorMap Value::_creatorMap;

//
// TypedValue methods
//

template <> string TypedValue<string>::getValueString() const
{
    return _data;
}

template <> string TypedValue<bool>::getValueString() const
{
    return _data ? VALUE_STRING_TRUE : VALUE_STRING_FALSE;
}

template <class T> string TypedValue<T>::getValueString() const
{
    std::stringstream ss;
    if (!(ss << _data))
        throw Exception("Unsupported value in getValueString");
    return ss.str();
}

template <class T> const string& TypedValue<T>::getTypeString() const
{
    return TYPE;
}

template <> ValuePtr TypedValue<string>::createFromString(const string& value)
{
    return Value::createValue<string>(value);
}

template <> ValuePtr TypedValue<bool>::createFromString(const string& value)
{
    if (value == VALUE_STRING_TRUE)
        return Value::createValue<bool>(true);
    else if (value == VALUE_STRING_FALSE)
        return Value::createValue<bool>(false);
    return nullptr;
}

template <class T> ValuePtr TypedValue<T>::createFromString(const string& value)
{
    std::stringstream ss;
    if (std::is_base_of<VectorBase, T>::value)
    {
        string fmt = value;
        fmt.erase(std::remove(fmt.begin(), fmt.end(), ' '), fmt.end());
        std::replace(fmt.begin(), fmt.end(), ',', ' ');
        ss << fmt;
    }
    else
    {
        ss << value;
    }

    T data;
    if (ss >> data)
        return Value::createValue<T>(data);
    return nullptr;
}

//
// Value methods
//

ValuePtr Value::createValueFromStrings(const string& value, const string& type)
{
    CreatorMap::iterator it = _creatorMap.find(type);
    if (it != _creatorMap.end())
        return it->second(value);

    return TypedValue<string>::createFromString(value);
}

template<class T> bool Value::isA() const
{
    return dynamic_cast<const TypedValue<T>*>(this) != nullptr;    
}

template<class T> T Value::asA() const
{
    const TypedValue<T>* typedVal = dynamic_cast<const TypedValue<T>*>(this);
    if (!typedVal)
    {
        throw Exception("Incorrect type specified for value");
    }
    return typedVal->getData();
}

//
// Global functions
//

template<class T> const string& getTypeString()
{
    return TypedValue<T>::TYPE;
}

//
// Value registry class
//

template <class T> class ValueRegistry
{
  public:
    ValueRegistry()
    {
        Value::_creatorMap[TypedValue<T>::TYPE] = TypedValue<T>::createFromString;
    }
    ~ValueRegistry() { }
};

//
// Template instantiations
//

#define INSTANTIATE_TYPE(T, name)                       \
template <> const string TypedValue<T>::TYPE = name;    \
template bool Value::isA<T>() const;                    \
template T Value::asA<T>() const;                       \
template const string& getTypeString<T>();              \
ValueRegistry<T> registry##T;

INSTANTIATE_TYPE(int, "integer")
INSTANTIATE_TYPE(bool, "boolean")
INSTANTIATE_TYPE(float, "float")
INSTANTIATE_TYPE(Color2, "color2")
INSTANTIATE_TYPE(Color3, "color3")
INSTANTIATE_TYPE(Color4, "color4")
INSTANTIATE_TYPE(Vector2, "vector2")
INSTANTIATE_TYPE(Vector3, "vector3")
INSTANTIATE_TYPE(Vector4, "vector4")
INSTANTIATE_TYPE(Matrix3x3, "matrix33")
INSTANTIATE_TYPE(Matrix4x4, "matrix44")
INSTANTIATE_TYPE(string, "string")
INSTANTIATE_TYPE(StringVec, "stringarray")

} // namespace MaterialX
