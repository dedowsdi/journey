#include <memory>

// value semantic named_type, if you need reference, consider
// https://github.com/joboccara/NamedType

template <typename T, typename phantom>
class named_type
{
public:
    explicit named_type(T const& value) : value_(value) {}
    explicit named_type(T&& value) : value_(std::move(value)) {}
    T& get() { return value_; }
    T const& get() const {return value_; }
private:
    T value_;
};

// using Width = named_type<double, struct width_phantom>
