/**
 * @file prop.h
 * @brief Type-agnotic property system
 *
 * This file provides a type-agnostic property system.
 * It is used to provide a generic interface for convenient property access for objects,
 * entities and entire systems.
 */

#ifndef PROP_H_
#define PROP_H_

#include <any>
#include <variant>

#include "cube.h"

namespace prop
{

/**
 * @brief Storage for a property value
 *
 * This type is used to store a property value.
 * Supported types are:
 * int, float, bvec, ivec, vec, std::string
 */
typedef std::variant<int, float, bvec, ivec, vec, std::string> PropertyValue;

/**
 * @brief Callback type for property changes
 *
 * @param argument - The argument passed to the callback.
 */
typedef std::function<void(std::any argument)> OnChangeCallback;

/**
 * @brief Type of a property
 *
 * This enum is used to determine the type of a property.
 * Mandates what sort of type is stored inside the PropertyValue.
 */
enum class PropertyType
{
    NoType = -1,

    Int,
    Float,
    Color,
    IntVec,
    FloatVec,
    String,

    Count
};

/**
 * @brief Meta information about a property
 *
 * This class stores definition information for a property:
 * - name (string)
 * - type (PropertyType)
 * - min, default and max values (PropertyValue)
 *
 * All properties should be statically bound to their PropertyMeta counterparts.
 */
class PropertyMeta
{
private:
    std::string name;
    PropertyType type;
    PropertyValue min, def, max;
    OnChangeCallback on_change;

public:
    PropertyType get_type() const;
    PropertyValue get_min() const;
    PropertyValue get_def() const;
    PropertyValue get_max() const;
    std::string get_name() const;

    void changed(std::any argument) const;

    template<class T> PropertyMeta(std::string _name, PropertyType _type, T _min, T _def, T _max,
                                   OnChangeCallback _on_change = nullptr) :
        name(_name), type(_type), min(_min), def(_def), max(_max), on_change(_on_change) {}
    template<class T> PropertyMeta(std::string _name, PropertyType _type, T _def,
                                   OnChangeCallback _on_change = nullptr) :
        name(_name), type(_type), def(_def), on_change(_on_change) {}

    PropertyMeta(const PropertyMeta&) = delete;
};

/**
 * @brief A property
 *
 * @tparam PropertyMetaT - The type of the meta class, defaults to PropertyMeta
 *
 * This class is an instance of a property. It stores a value and a reference to its meta class.
 */
template<typename PropertyMetaT = PropertyMeta>
class Property
{
private:
    PropertyValue value;
    const PropertyMetaT& meta;

    bool set_check_type(PropertyType type) const;
    void set_clamped(PropertyValue value);

public:
    const PropertyValue& get_value() const;
    int get_int() const;
    float get_float() const;
    const bvec& get_color() const;
    const ivec& get_ivec() const;
    const vec& get_fvec() const;
    const std::string& get_string() const;
    size_t get_size() const;
    std::string get_name() const;
    PropertyType get_type() const;

    std::string to_string() const;

    void copy(const Property& prop);

    void set(PropertyValue value, std::any on_change_argument = std::any());
    void set_no_cb(PropertyValue value);

    void reset();

    void cmd_result() const;
    void cmd_result_min() const;
    void cmd_result_def() const;
    void cmd_result_max() const;

    virtual void pack(std::vector<uint8_t>& buf) const;
    virtual size_t unpack(const uint8_t* buf, size_t len);

    Property(const PropertyMetaT& _meta);
    virtual ~Property() = default;
};



////////////////////
// Implementation //
////////////////////



/**
 * @brief Get the integer value of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @return int
 *
 * Caution: This function does not check the type of the property. A former check is assumed.
 */
template<typename PropertyMetaT>
int Property<PropertyMetaT>::get_int() const
{
    return std::get<int>(value);
}

/**
 * @brief Get the float value of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @return float
 *
 * Caution: This function does not check the type of the property. A former check is assumed.
 */
template<typename PropertyMetaT>
float Property<PropertyMetaT>::get_float() const
{
    return std::get<float>(value);
}

/**
 * @brief Get the color value of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @return bvec
 *
 * Caution: This function does not check the type of the property. A former check is assumed.
 */
template<typename PropertyMetaT>
const bvec& Property<PropertyMetaT>::get_color() const
{
    return std::get<bvec>(value);
}

/**
 * @brief Get the integer vector value of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @return ivec
 *
 * Caution: This function does not check the type of the property. A former check is assumed.
 */
template<typename PropertyMetaT>
const ivec& Property<PropertyMetaT>::get_ivec() const
{
    return std::get<ivec>(value);
}

/**
 * @brief Get the float vector value of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @return vec
 *
 * Caution: This function does not check the type of the property. A former check is assumed.
 */
template<typename PropertyMetaT>
const vec& Property<PropertyMetaT>::get_fvec() const
{
    return std::get<vec>(value);
}

/**
 * @brief Get the string value of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @return std::string
 *
 * Caution: This function does not check the type of the property. A former check is assumed.
 */
template<typename PropertyMetaT>
const std::string& Property<PropertyMetaT>::get_string() const
{
    return std::get<std::string>(value);
}

/**
 * @brief Get the size of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @return size_t
 *
 * Returns the size of the stored value in bytes.
 */
template<typename PropertyMetaT>
size_t Property<PropertyMetaT>::get_size() const
{
    switch(get_type())
    {
    case PropertyType::Int:
        return sizeof(int);
    case PropertyType::Float:
        return sizeof(float);
    case PropertyType::Color:
        return sizeof(bvec);
    case PropertyType::IntVec:
        return sizeof(ivec);
    case PropertyType::FloatVec:
        return sizeof(vec);
    case PropertyType::String:
        return get_string().size();
    default:
        return 0;
    }
}

template<typename PropertyMetaT>
std::string Property<PropertyMetaT>::to_string() const
{
    switch(get_type())
    {
    case PropertyType::Int:
        return std::to_string(get_int());

    case PropertyType::Float:
        return std::to_string(get_float());

    case PropertyType::Color:
        return std::to_string(get_color().r) + " " + std::to_string(get_color().g) + " " + std::to_string(get_color().b);

    case PropertyType::IntVec:
        return std::to_string(get_ivec().x) + " " + std::to_string(get_ivec().y) + " " + std::to_string(get_ivec().z);

    case PropertyType::FloatVec:
        return std::to_string(get_fvec().x) + " " + std::to_string(get_fvec().y) + " " + std::to_string(get_fvec().z);

    case PropertyType::String:
        return get_string();

    default:
        return "";
    }
}

template<typename PropertyMetaT>
bool Property<PropertyMetaT>::set_check_type(PropertyType type) const
{
    static constexpr std::string_view typenames[] =
    {
        "int", "float", "color", "int vector", "float vector", "string"
    };

    if(get_type() != type)
    {
        // conoutf("Error: %s, incorrect property assignment type: got %s, expected %s",
        //         meta.get_name().c_str(), std::string(typenames[static_cast<size_t>(type)]).c_str(),
        //         std::string(typenames[static_cast<size_t>(get_type())]).c_str());

        return false;
    }

    return true;
}

/**
 * @brief Get the name of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @return std::string
 *
 * Returns the name of the property, as per the definition in the meta class.
 */
template<typename PropertyMetaT>
std::string Property<PropertyMetaT>::get_name() const
{
    return meta.get_name();
}

/**
 * @brief Get the type of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @return std::string
 *
 * Returns the type of the property, as per the definition in the meta class.
 */
template<typename PropertyMetaT>
PropertyType Property<PropertyMetaT>::get_type() const
{
    return meta.get_type();
}

/**
 * @brief Copy the value of another property, will not invoke callback
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @param prop - The property to copy the value from.
 * @param on_change_arg - Optional argument for on_change callback.
 *
 * Note: type check is performed automatically, value of an incompatible type is ignored and reported.
 * Value is clamped to the range defined in the meta class, if applicable.
 */
template<typename PropertyMetaT>
void Property<PropertyMetaT>::copy(const Property& prop)
{
    set_no_cb(prop.value);
}

template<typename PropertyMetaT>
void Property<PropertyMetaT>::set_clamped(PropertyValue value)
{
    switch(get_type())
    {
        case PropertyType::Int:
        {
            int v = std::get<int>(value);
            this->value = std::clamp(v, std::get<int>(meta.get_min()), std::get<int>(meta.get_max()));
            break;
        }

        case PropertyType::Float:
        {
            float v = std::get<float>(value);
            this->value = std::clamp(v, std::get<float>(meta.get_min()), std::get<float>(meta.get_max()));
            break;
        }

        case PropertyType::IntVec:
        {
            ivec v = std::get<ivec>(value);
            this->value = ivec(v).min(std::get<ivec>(meta.get_max()))
                                 .max(std::get<ivec>(meta.get_min()));
            break;
        }

        case PropertyType::FloatVec:
        {
            vec v = std::get<vec>(value);
            this->value = vec(v).min(std::get<vec>(meta.get_max()))
                                .max(std::get<vec>(meta.get_min()));
            break;
        }

        default:
            this->value = value;
            break;
    }
}

/**
 * @brief Set the value of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @param value - The new value of the property.
 * @param on_change_arg - Optional argument for on_change callback.
 *
 * Note: type check is performed automatically, value of an incompatible type is ignored and reported.
 * Value is clamped to the range defined in the meta class, if applicable.
 */
template<typename PropertyMetaT>
void Property<PropertyMetaT>::set(PropertyValue value, std::any on_change_arg)
{
    if(set_check_type(static_cast<PropertyType>(value.index())))
    {
        set_clamped(value);
        meta.changed(on_change_arg);
    }
}

/**
 * @brief Set the value of the property without invoking callback
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @param value - The new value of the property.
 *
 * Note: type check is performed automatically, value of an incompatible type is ignored and reported.
 * Value is clamped to the range defined in the meta class, if applicable.
 */
template<typename PropertyMetaT>
void Property<PropertyMetaT>::set_no_cb(PropertyValue value)
{
    if(set_check_type(static_cast<PropertyType>(value.index())))
    {
        set_clamped(value);
    }
}

/**
 * @brief Reset the value of the property to the default value
 *
 * @tparam PropertyMetaT - The type of the meta class.
 */
template<typename PropertyMetaT>
void Property<PropertyMetaT>::reset()
{
    this->value = meta.get_def();
}

static void _prop_cmd_result_value(PropertyValue value, PropertyType type)
{
    switch(type)
    {
        case PropertyType::Int:
            intret(std::get<int>(value));
            break;

        case PropertyType::Float:
            floatret(std::get<float>(value));
            break;

        case PropertyType::Color:
            intret(std::get<bvec>(value).tohexcolor());
            break;

        case PropertyType::IntVec:
        {
            const ivec& v = std::get<ivec>(value);
            result(tempformatstring("%d %d %d", v.x, v.y, v.z));
            break;
        }

        case PropertyType::FloatVec:
        {
            const vec& v = std::get<vec>(value);
            result(tempformatstring("%f %f %f", v.x, v.y, v.z));
            break;
        }

        case PropertyType::String:
            result(std::get<std::string>(value).c_str());
            break;

        default: break;
    }
}

/**
 * @brief Results the value of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 */
template<typename PropertyMetaT>
void Property<PropertyMetaT>::cmd_result() const
{
    _prop_cmd_result_value(value, get_type());
}

/**
 * @brief Results the minimum value of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 */
template<typename PropertyMetaT>
void Property<PropertyMetaT>::cmd_result_min() const
{
    _prop_cmd_result_value(meta.get_min(), get_type());
}

/**
 * @brief Results the default value of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 */
template<typename PropertyMetaT>
void Property<PropertyMetaT>::cmd_result_def() const
{
    _prop_cmd_result_value(meta.get_def(), get_type());
}

/**
 * @brief Results the maximum value of the property
 *
 * @tparam PropertyMetaT - The type of the meta class.
 */
template<typename PropertyMetaT>
void Property<PropertyMetaT>::cmd_result_max() const
{
    _prop_cmd_result_value(meta.get_max(), get_type());
}

/**
 * @brief Packs the property into a buffer
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @param buf - The buffer to pack the property into.
 */
template<typename PropertyMetaT>
void Property<PropertyMetaT>::pack(std::vector<uint8_t>& buf) const
{
    size_t data_size = get_size();

    vectorput(buf, data_size);

    switch(get_type())
    {
        case PropertyType::Int:
        {
            vectorput(buf, get_int());
            break;
        }

        case PropertyType::Float:
        {
            vectorput(buf, get_float());
            break;
        }

        case PropertyType::Color:
        {
            vectorput(buf, get_color());
            break;
        }

        case PropertyType::IntVec:
        {
            vectorput(buf, get_ivec());
            break;
        }

        case PropertyType::FloatVec:
        {
            vectorput(buf, get_fvec());
            break;
        }

        case PropertyType::String:
        {
            std::string data = get_string();
            // Null-terminator not needed, since we know the size
            vectorput(buf, reinterpret_cast<const uint8_t*>(data.data()), data.size());
            break;
        }

        default: break;
    }
}

/**
 * @brief Unpacks the property from a buffer
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @param buf - The buffer to unpack the property from.
 * @param buf_size - The size of the buffer.
 *
 * @return The number of bytes read from the buffer.
 */
template<typename PropertyMetaT>
size_t Property<PropertyMetaT>::unpack(const uint8_t* buf, size_t buf_size)
{
    size_t buf_read = 0;
    const size_t* data_size_packed = nullptr;

    if(buf_size <= sizeof(*data_size_packed))
    {
        // conoutf("Error unpacking prop '%s': not enough data to get the size!", meta.get_name().c_str());
        return 0;
    }

    data_size_packed = reinterpret_cast<const size_t*>(buf);

    buf_read += sizeof(*data_size_packed);

    static auto size_check = [&](size_t needed_size)
    {
        if(buf_size - buf_read < needed_size)
        {
            // conoutf("Error unpacking prop '%s': not enough data!", meta.get_name().c_str());
            return false;
        }

        return true;
    };

    switch(get_type())
    {
        case PropertyType::Int:
        case PropertyType::Color:
        case PropertyType::Float:
            if(*data_size_packed != get_size())
            {
                // conoutf("Error unpacking prop '%s': unexpected data size! Wanted: %lu, got: %lu",
                //         meta.get_name().c_str(), get_size(), *data_size_packed);

                return 0;
            }
            break;

        default: break;
    }

    switch(get_type())
    {
        case PropertyType::Int:
        {
            if(!size_check(get_size()))
            {
                return 0;
            }

            int ival;
            memcpy(&ival, buf + buf_read, get_size());
            buf_read += get_size();
            value = ival;
            break;
        }

        case PropertyType::Float:
        {
            if(!size_check(get_size()))
            {
                return 0;
            }

            float fval;
            memcpy(&fval, buf + buf_read, get_size());
            buf_read += get_size();
            value = fval;
            break;
        }

        case PropertyType::Color:
        {
            if(!size_check(get_size()))
            {
                return 0;
            }

            bvec cval;
            memcpy(&cval, buf + buf_read, get_size());
            buf_read += get_size();
            value = cval;
            break;
        }

        case PropertyType::IntVec:
        {
            if(!size_check(get_size()))
            {
                return 0;
            }

            ivec ivecval;
            memcpy(&ivecval, buf + buf_read, get_size());
            buf_read += get_size();
            value = ivecval;
            break;
        }

        case PropertyType::FloatVec:
        {
            if(!size_check(get_size()))
            {
                return 0;
            }

            vec vecval;
            memcpy(&vecval, buf + buf_read, get_size());
            buf_read += get_size();
            value = vecval;
            break;
        }

        case PropertyType::String:
        {
            if(!size_check(*data_size_packed))
            {
                return 0;
            }

            std::string sval(reinterpret_cast<const char*>(buf + buf_read), *data_size_packed);
            buf_read += *data_size_packed;
            value = sval;
            break;
        }

        default: break;
    }

    return buf_read;
}

/**
 * @brief Creates a new property with the given meta class.
 *
 * @tparam PropertyMetaT - The type of the meta class.
 * @param _meta - The meta class.
 */
template<typename PropertyMetaT>
Property<PropertyMetaT>::Property(const PropertyMetaT& _meta) : meta(_meta)
{
    reset();
}

/**
 * @brief Gets the type of the property definition.
 *
 * @return PropertyType
 */
PropertyType PropertyMeta::get_type() const
{
    return type;
}

/**
 * @brief Gets the minimum value of the property definition.
 *
 * @return PropertyValue
 */
PropertyValue PropertyMeta::get_min() const
{
    return min;
}

/**
 * @brief Gets the default value of the property definition.
 *
 * @return PropertyValue
 */
PropertyValue PropertyMeta::get_def() const
{
    return def;
}

/**
 * @brief Gets the maximum value of the property definition.
 *
 * @return PropertyValue
 */
PropertyValue PropertyMeta::get_max() const
{
    return max;
}

/**
 * @brief Gets the name of the property definition.
 *
 * @return std::string
 */
std::string PropertyMeta::get_name() const
{
    return name;
}

void PropertyMeta::changed(std::any argument) const
{
    if(on_change)
    {
        on_change(argument);
    }
}



//////////////////////
// Helper functions //
//////////////////////



/**
 * @brief Finds a property with the given name in the given array.
 *
 * @tparam PropertyT - Property class type
 * @tparam N - Size of the array
 * @param name - The name of the property to find.
 * @param props - The array of properties to search.
 * @return PropertyT* - The property if found, nullptr otherwise.
 */
template<typename PropertyT, std::size_t N>
PropertyT* find_prop(std::string name, std::array<PropertyT, N>& props)
{
    for(PropertyT& prop : props)
    {
        if(prop.get_name() == name)
        {
            return &prop;
        }
    }

    return nullptr;
}

/**
 * @brief Finds a property with the given name in the given array.
 *
 * @tparam PropertyT - Property class type
 * @tparam N - Size of the array
 * @param name - The name of the property to find.
 * @param props - The array of properties to search.
 * @return PropertyT* - The property if found, nullptr otherwise.
 */
template<typename PropertyT, std::size_t N>
const PropertyT* find_prop(std::string name, const std::array<PropertyT, N>& props)
{
    for(const PropertyT& prop : props)
    {
        if(prop.get_name() == name)
        {
            return &prop;
        }
    }

    return nullptr;
}

/**
 * @brief Sets the value of a property with the given name in the given array.
 *
 * @tparam PropertyT - Property class type
 * @tparam N - Size of the array
 * @param name - The name of the property to set.
 * @param value - The value to set the property to. Value is clamped to the property's min/max values, if applicable.
 * @param props - The array of properties to search.
 * @param on_change_arg - Optional argument to pass to the property's on_change callback.
 * @return true - The property was found and set.
 */
template<typename PropertyT, std::size_t N>
bool set_prop(std::string name, PropertyValue value, std::array<PropertyT, N>& props,
              std::any on_change_arg = std::any())
{
    PropertyT* prop = find_prop(name, props);
    if(!prop)
    {
        // conoutf("Property %s not found!", name.c_str());
        return false;
    }

    prop->set(value, on_change_arg);
    return true;
}

/**
 * @brief Finds a property definition with the given name in the given array.
 *
 * @tparam PropertyMetaT - PropertyMeta class type
 * @tparam N - Size of the array
 * @param name - The name of the property definition to find.
 * @param prop_metas - The array of property definitions to search.
 * @return const PropertyMetaT* - The property definition if found, nullptr otherwise.
 */
template<typename PropertyMetaT, std::size_t N>
const PropertyMetaT* find_prop_meta(std::string name, const std::array<PropertyMetaT, N>& prop_metas)
{
    for(const PropertyMetaT& prop_meta : prop_metas)
    {
        if(prop_meta.get_name() == name)
        {
            return &prop_meta;
        }
    }

    return nullptr;
}

/**
 * @brief Packs the given properties into the given buffer.
 *
 * @tparam PropertyT - Property class type
 * @tparam N - Size of the array
 * @param props - The array of properties to pack.
 * @param buf - The buffer to pack the properties into.
 */
template<typename PropertyT, std::size_t N>
void pack_props(const std::array<PropertyT, N>& props, std::vector<uint8_t>& buf)
{
    for(const PropertyT& prop : props)
    {
        prop.pack(buf);
    }
}

/**
 * @brief Unpacks the given buffer into the given properties.
 *
 * @tparam PropertyT - Property class type
 * @tparam N - Size of the array
 * @param buf - The buffer to unpack.
 * @param props - The array of properties to unpack into.
 * @return size_t - The number of properties unpacked.
 */
template<typename PropertyT, std::size_t N>
size_t unpack_props(const std::vector<uint8_t>& buf, std::array<PropertyT, N>& props)
{
    size_t prop_idx = 0;
    size_t read_pos = 0;

    while(read_pos < buf.size())
    {
        if(prop_idx >= N)
        {
            break;
        }

        PropertyT &prop = props[prop_idx];

        int unpacked_size = prop.unpack(buf.data() + read_pos, buf.size() - read_pos);
        if(!unpacked_size)
        {
            break;
        }

        read_pos += unpacked_size;
        prop_idx++;
    }

    return prop_idx;
}

template<std::size_t N, typename PropertyT, std::size_t... Is, typename PropertyMetaT>
std::array<PropertyT, N> _make_props_array_impl(std::index_sequence<Is...>,
                                               const PropertyMetaT (&prop_meta)[N])
{
    return { PropertyT(prop_meta[Is])... };
}

/**
 * @brief Creates an array of properties from the given property definitions.
 *
 * @tparam N - Size of the array
 * @tparam PropertyT - Property class type
 * @tparam Is - Index sequence
 * @tparam PropertyMetaT - PropertyMeta class type
 * @param prop_meta - The array of property definitions to create properties from.
 * @return std::array<PropertyT, N> - The array of properties bound to meta information.
 */
template<std::size_t N, typename PropertyT, std::size_t... Is, typename PropertyMetaT>
std::array<PropertyT, N> make_props_array(const PropertyMetaT (&prop_meta)[N])
{
    return _make_props_array_impl<N, PropertyT>(std::make_index_sequence<N>{}, prop_meta);
}

}; // namespace prop

#endif // PROP_H_
