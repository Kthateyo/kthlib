#include <cstring>
#include <initializer_list>
#include <iterator>

namespace kth
{

// Abstract array with elements of type T and of length N
template <typename T>
class array_base
{

  public:
    /////////////////////////////////////
    // Member Types
    /////////////////////////////////////

    typedef T* iterator;
    typedef const T* const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    /////////////////////////////////////
    // Iterators
    /////////////////////////////////////

    // This returns the ordinary pointer to the underlying array
    virtual T* data() = 0;

    // This returns an iterator that represents the beginning of the elements in the container. The beginning is the position of the first element
    iterator begin()
    {
        return iterator(data());
    }

    // This returns a const iterator that represents the beginning of the elements in the container. The beginning is the position of the first element
    const_iterator cbegin() const
    {
        return const_iterator(begin());
    }

    // This returns a reverse iterator that represents the end of the elements in the container. The end is the position behind the last element
    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    // This returns a const reverse iterator that represents the end of the elements in the container. The end is the position behind the last element
    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(end());
    }

    // This returns an iterator that represents the end of the elements in the container. The end is the position behind the last element
    virtual iterator end() = 0;

    // This returns a const iterator that represents the end of the elements in the container. The end is the position behind the last element
    const_iterator cend() const
    {
        return const_iterator(end());
    }

    // This returns a reverse iterator that represents the beginning of the elements in the container. The beginning is the position of the first element
    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    // This returns a const reverse iterator that represents the beginning of the elements in the container. The beginning is the position of the first element
    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(begin());
    }

    /////////////////////////////////////
    // Accessing
    /////////////////////////////////////

    // Returns a reference to the element of index. Not range safe
    T& operator[](std::size_t index)
    {
        return data()[index];
    }

    // Returns a reference to the element of index. Throws Out Of Range Exception
    virtual T& at(std::size_t index) = 0;

    /////////////////////////////////////
    // Capacity
    /////////////////////////////////////

    // Returns amount of elements in array
    virtual std::size_t length() const = 0;

    // Returns size of array in bytes
    virtual std::size_t size() const
    {
        return length() * sizeof(T);
    }
};

// Stack allocated array with elements of type T and of length N
template <typename T, size_t N>
class array_stack : public array_base<T>
{

  private:
    // Underlying array
    T m_data[N];

  public:
    /////////////////////////////////////
    // Member Types
    /////////////////////////////////////

    typedef T* iterator;

    /////////////////////////////////////
    // Constructors
    /////////////////////////////////////

    array_stack()
    {
    }

    array_stack(const iterator& begin, const iterator& end)
    {
        for (auto i = 0, it = begin; i < N && begin != end; ++i, ++it)
            m_data[i] = *it;
    }

    array_stack(const array_stack& other)
        : array_stack(other.cbegin(), other.cend())
    {
    }

    array_stack(const std::initializer_list<T>& l)
        : array_stack((int*)l.begin(), (int*)l.end())
    {
    }

    /////////////////////////////////////
    // Iterators
    /////////////////////////////////////

    // This returns the ordinary pointer to the underlying array
    T* data() override
    {
        return m_data;
    }

    // This returns an iterator that represents the end of the elements in the container. The end is the position behind the last element
    iterator end() override
    {
        return this->begin() + length();
    }

    /////////////////////////////////////
    // Accessing
    /////////////////////////////////////

    // Returns a reference to the element of index. Throws Out Of Range Exception
    T& at(std::size_t index) override
    {
        if (index < N)
            return data()[index];
        else
            throw std::out_of_range("Out Of Range Exception");
    }

    /////////////////////////////////////
    // Capacity
    /////////////////////////////////////

    // Returns amount of elements in array
    std::size_t length() const override
    {
        return N;
    }
};

// Heap allocated array with elements of type T and of length N
template <typename T, size_t N>
class array_heap : public array_base<T>
{

  private:
    // Underlying array
    T* m_data;

  public:
    /////////////////////////////////////
    // Member Types
    /////////////////////////////////////

    typedef T* iterator;

    /////////////////////////////////////
    // Constructors
    /////////////////////////////////////

    // Value to fill with array
    array_heap()
        : m_data(new T[N])
    {
    }

    array_heap(const iterator& begin, const iterator& end)
        : m_data(new T[N])
    {
        for (auto i = 0, it = begin; i < N && it != end; ++i, ++it)
            m_data[i] = *it;
    }

    array_heap(const array_heap& other)
        : array_heap(other.cbegin(), other.cend())
    {
    }

    array_heap(const std::initializer_list<T>& l)
        : array_heap((int*)l.begin(), (int*)l.end())
    {
    }

    /////////////////////////////////////
    // Destructors
    /////////////////////////////////////

    ~array_heap()
    {
        delete[] m_data;
    }

    /////////////////////////////////////
    // Iterators
    /////////////////////////////////////

    // This returns the ordinary pointer to the underlying array
    T* data() override
    {
        return m_data;
    }

    // This returns an iterator that represents the end of the elements in the container. The end is the position behind the last element
    iterator end() override
    {
        return this->begin() + length();
    }

    /////////////////////////////////////
    // Accessing
    /////////////////////////////////////

    // Returns a reference to the element of index. Throws Out Of Range Exception
    T& at(std::size_t index) override
    {
        if (index < N)
            return data()[index];
        else
            throw std::out_of_range("Out Of Range Exception");
    }

    /////////////////////////////////////
    // Capacity
    /////////////////////////////////////

    // Returns amount of elements in array
    std::size_t length() const override
    {
        return N;
    }
};

template <typename T>
class array_dynamic : public array_base<T>
{

  private:
    // Underlying array
    T* m_data;

    // Length of array
    size_t m_length;

    // Capacity of array
    size_t m_capacity;

    void enlarge()
    {
        if (m_capacity == 0)
            m_capacity = 1;
        else
            m_capacity *= 2;

        reserve(m_capacity);
    }

  public:
    /////////////////////////////////////
    // Member Types
    /////////////////////////////////////

    typedef T* iterator;

    /////////////////////////////////////
    // Constructors
    /////////////////////////////////////

    // Value to fill with array
    array_dynamic(const std::size_t& capacity = 0)
        : m_capacity(capacity), m_length(0)
    {
        if (m_capacity > 0)
            m_data = new T[m_capacity];
        else
            m_data = nullptr;
    }

    array_dynamic(const iterator& begin, const iterator& end)
    {
        std::ptrdiff_t n = end - begin;
        m_length = n;
        m_capacity = n;
        m_data = new T[n];

        for (auto it = this->begin(), other = begin; it != this->end(); ++it, ++other)
            *it = *other;
    }

    array_dynamic(const array_dynamic& other)
        : array_dynamic(other.cbegin(), other.cend())
    {
    }

    array_dynamic(const std::initializer_list<T>& l)
        : array_dynamic((int*)l.begin(), (int*)l.end())
    {
    }

    /////////////////////////////////////
    // Destructors
    /////////////////////////////////////

    ~array_dynamic()
    {
        delete[] m_data;
    }

    /////////////////////////////////////
    // Iterators
    /////////////////////////////////////

    // This returns an iterator that represents the end of the elements in the container. The end is the position behind the last element
    iterator end() override
    {
        return iterator(data() + length());
    }

    /////////////////////////////////////
    // Accessing
    /////////////////////////////////////

    // This returns the ordinary pointer to the underlying array
    T* data() override
    {
        return m_data;
    }

    // Returns a reference to the element of index. Throws Out Of Range Exception
    T& at(std::size_t index) override
    {
        if (index < length())
            return data()[index];
        else
            throw std::out_of_range("Out Of Range Exception");
    }

    T& front()
    {
        return *(this->begin());
    }

    T& back()
    {
        return *(end() - 1);
    }

    /////////////////////////////////////
    // Modifiers
    /////////////////////////////////////

    // Push back a given value
    void push_back(const T& value)
    {
        if (length() < capacity())
        {
            data()[length()] = value;
            ++m_length;
        }
        else
        {
            enlarge();
            data()[length()] = value;
            ++m_length;
        }
    }

    // Pop back
    void pop_back()
    {
        --m_length;
    }

    /////////////////////////////////////
    // Capacity
    /////////////////////////////////////

    // Returns amount of elements in array
    std::size_t length() const override
    {
        return m_length;
    }

    // Returns amount of elements in array
    std::size_t capacity() const
    {
        return m_capacity;
    }

    // Returns size of array in bytes
    virtual std::size_t size() const
    {
        return capacity() * sizeof(T);
    }

    bool isEmpty()
    {
        return length() == 0;
    }

    void reserve(std::size_t new_capacity)
    {
        if (m_length < new_capacity)
        {
            T* new_array = new T[new_capacity];

            if (m_data != nullptr)
            {
                std::memcpy(new_array, m_data, length() * sizeof(T));
                delete[] m_data;
            }

            m_data = new_array;
        }
    }
};
} // namespace kth
