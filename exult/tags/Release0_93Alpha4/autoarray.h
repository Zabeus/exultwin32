#include <exception>
#include <string>

template<class T>
class autoarray
	{
private:
	std::size_t	size_;
	T *data_;
public:
	class range_error : public std::exception
		{
		std::string	what_;
		public:
		 range_error (const std::string& what_arg): what_ (what_arg) { }
		 const char *what(void) const throw () { return what_.c_str(); }
		 virtual ~range_error() throw () { }
		};
	autoarray() : size_(0), data_(0) 
		{  }
	autoarray(std::size_t n) : size_(n),data_(n?new T[n]:0)
		{  }
	T &operator[](sint32 i)	 throw(range_error)
		{
		if(i>=size_ || i < 0)
			throw range_error("out of bounds");
		if(data_)
			return data_[i];
		throw range_error("no data");
		}
	~autoarray()
		{
		if(data_)
			delete [] data_;
		}
	autoarray(const autoarray &a) : size_(0),data_(0)
		{
		if(a.data_)
			{
			data_=new T[a.size_];
			memcpy(data_,a.data_,a.size_);
			size_=a.size_;
			}
		}
	autoarray &operator=(const autoarray &a)
		{
		if(data_)
			{
			delete [] data_;
			size_=0;
			}
		if(a.data_)
			{
			data_=new T[a.size_];
			memcpy(data_,a.data_,a.size_);
			size_=a.size_;
			}
		return *this;
		}
	void set_size(std::size_t new_size)
		{
		if(data_)
			{
			delete [] data_;
			}
		data_=new T[new_size];
		size_=new_size;
		}
	};

