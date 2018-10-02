
#pragma once

#include <functional>
#include <utility>
#include <memory>
#include <cassert>
#include <type_traits>

template<typename T>
struct cloner
{
	std::function<T*(const T*)> clone;
	std::function<void(T*)> destroy;
	void clear()
	{
		clone = nullptr;
		destroy = nullptr;
	}
	void swap(cloner& other)
	{
		clone.swap(other.clone);
		destroy.swap(other.destroy);
	}
};

template<typename T, typename A = std::allocator<T> >
cloner<T> make_special_member_cloner()
{
	cloner<T> ret;
	ret.clone = [](const T* p) -> T*
	{
		return new (A{}.allocate(1)) T{*p};
	};
	ret.destroy = [](T* p) -> void
	{
		p->~T();
		A{}.deallocate(p, 1);
	};
	return ret;
}

template<typename T>
cloner<T> make_clone_member_cloner()
{
	cloner<T> ret;
	ret.clone = [](const T* p) -> T*
	{
		return p->clone();
	};
	ret.destroy = [](T* p) -> void
	{
		delete p;
	};
	return ret;
}

template<typename T>
cloner<T> make_clone_destroy_member_cloner()
{
	cloner<T> ret;
	ret.clone = [](const T* p) -> T*
	{
		return p->clone();
	};
	ret.destroy = [](T* p) -> void
	{
		p->destroy();
	};
	return ret;
}

template<typename T>
struct have_clone_member
{
	template<typename U=T>
	static auto test(U* u) -> decltype( u = u->clone(), char() );
	static long test(...);

	static const bool value = (sizeof(test((T*)0)) == 1);
};

template<typename T>
constexpr bool have_clone_member_v = have_clone_member<T>::value;

template<typename T>
struct have_destroy_member
{
	template<typename U=T>
	static auto test(U* u) -> decltype( u->destroy(), char() );
	static long test(...);

	static const bool value = (sizeof(test((T*)0)) == 1);
};

template<typename T>
constexpr bool  have_destroy_member_v = have_destroy_member<T>::value;

namespace detail
{

	struct pick_3 {};
	struct pick_2 : pick_3 {};
	struct pick_1 : pick_2 {};

	template<typename T>
	std::enable_if_t<have_clone_member_v<T> && have_destroy_member_v<T>, cloner<T>>
	make_cloner(pick_1)
	{
		return make_clone_destroy_member_cloner<T>();
	}

	template<typename T>
	std::enable_if_t<have_clone_member_v<T>, cloner<T>>
	make_cloner(pick_2)
	{
		return make_clone_member_cloner<T>();
	}

	template<typename T>
	cloner<T>
	make_cloner(pick_3)
	{
		return make_special_member_cloner<T>();
	}
}

template<typename T>
cloner<T> make_cloner()
{
	return detail::make_cloner<T>(detail::pick_1{});
}


template<typename T>
struct clone_ptr
{
	clone_ptr() noexcept = default;
	clone_ptr(const clone_ptr&);
	clone_ptr(clone_ptr&&) noexcept;
	clone_ptr& operator=(const clone_ptr&);
	clone_ptr& operator=(clone_ptr&&) noexcept;
	~clone_ptr();
	void swap(clone_ptr&) noexcept;
	clone_ptr(const T&);
	clone_ptr(const T&, cloner<T>);
	clone_ptr& operator=(const T&);
	clone_ptr& assign(const T&, cloner<T>);
	T& operator*();
	const T& operator*() const;
	T* operator->();
	const T* operator->() const;
	explicit operator bool() const;

	template<typename... Args>
	clone_ptr& emplace(Args&&... args);

	template<typename U>
	clone_ptr& operator=(const clone_ptr<U>&);
	template<typename U>
	clone_ptr(const clone_ptr<U>&);
	template<typename U>
	clone_ptr(clone_ptr<U>&&);

	template<typename U>
	clone_ptr& operator=(const U&);

	template<typename U>
	clone_ptr(const U&);

	clone_ptr& operator=(std::nullptr_t) {clear();}

	void clear();

	T* get() { return ptr; }
	const T* get() const { return ptr; }

	template<typename U>
	friend struct clone_ptr;

	int operator<=>(const clone_ptr& other) const
	{
		if (!ptr) return (other.ptr ? -1 : 0);
		if (!other.ptr) return +1;
		return (*ptr) <=> (*other.ptr);
	}


private:
	cloner<T> cl;
	T* ptr = nullptr;
};


template<typename T>
clone_ptr<T>::clone_ptr(const clone_ptr& other)
	: cl(other.cl)
{
	ptr = other.ptr ? cl.clone(other.ptr) : nullptr;
}

template<typename T>
clone_ptr<T>::clone_ptr(clone_ptr&& other) noexcept
	: clone_ptr()
{
	swap(other);
}

template<typename T>
auto clone_ptr<T>::operator=(const clone_ptr& other) -> clone_ptr&
{
	if (ptr)
		cl.destroy(ptr);
	cl = other.cl;
	ptr = other.ptr ? cl.clone(other.ptr) : nullptr;
	return *this;
}

template<typename T>
auto clone_ptr<T>::operator=(clone_ptr&& other) noexcept -> clone_ptr&
{
	swap(other);
	return *this;
}

template<typename T>
clone_ptr<T>::~clone_ptr()
{
	if (ptr)
		cl.destroy(ptr);
}

template<typename T>
void clone_ptr<T>::swap(clone_ptr& other) noexcept
{
	cl.swap(other.cl);
	std::swap(ptr, other.ptr);
}

template<typename T>
clone_ptr<T>::clone_ptr(const T& val)
	: cl(make_cloner<T>())
{
	ptr = cl.clone(&val);
}

template<typename T>
clone_ptr<T>::clone_ptr(const T& val, cloner<T> cl)
	: cl(cl)
{
	ptr = cl.clone(&val);
}

template<typename T>
auto clone_ptr<T>::operator=(const T& val) -> clone_ptr&
{
	if (ptr)
		cl.destroy(ptr);
	cl = make_cloner<T>();
	ptr = cl.clone(&val);
	return *this;
}

template<typename T>
auto clone_ptr<T>::assign(const T& val, cloner<T> val_cl) -> clone_ptr&
{
	if (ptr)
		cl.destroy(ptr);
	cl = val_cl;
	ptr = cl.clone(&val);
	return *this;
}

template<typename T>
T& clone_ptr<T>::operator*()
{
	assert(ptr);
	return *ptr;
}

template<typename T>
const T& clone_ptr<T>::operator*() const
{
	assert(ptr);
	return *ptr;
}

template<typename T>
T* clone_ptr<T>::operator->()
{
	assert(ptr);
	return ptr;
}

template<typename T>
const T* clone_ptr<T>::operator->() const
{
	assert(ptr);
	return ptr;
}

template<typename T>
clone_ptr<T>::operator bool() const
{
	return ptr;
}

template<typename T>
template<typename... Args>
auto clone_ptr<T>::emplace(Args&&... args) -> clone_ptr&
{
	(*this) = T{std::forward<Args>(args)...};
	return *this;
}

template<typename T>
template<typename U>
auto clone_ptr<T>::operator=(const clone_ptr<U>& other) -> clone_ptr&
{
	static_assert( std::is_base_of<T, U>::value );
	if (ptr)
		cl.destroy(ptr);
	if (other.ptr)
	{
		auto oth_cl = other.cl;
		cl.clone = [oth_cl](const T* t) -> T* { return (T*)oth_cl.clone((U*)t); };
		cl.destroy = [oth_cl](T* t) -> void { oth_cl.destroy((U*)t); };
		ptr = cl.clone(other.ptr);
	}
	else
	{
		ptr = nullptr;
		cl.clear();
	}
	return *this;
}

template<typename T>
template<typename U>
clone_ptr<T>::clone_ptr(const clone_ptr<U>& other)
	: clone_ptr()
{
	static_assert( std::is_base_of<T, U>::value );
	if (other.ptr)
	{
		auto oth_cl = other.cl;
		cl.clone = [oth_cl](const T* t) -> T* { return (T*)oth_cl.clone((U*)t); };
		cl.destroy = [oth_cl](T* t) -> void { oth_cl.destroy((U*)t); };
		ptr = cl.clone(other.ptr);
	}
}

template<typename T>
template<typename U>
clone_ptr<T>::clone_ptr(clone_ptr<U>&& other)
	: clone_ptr()
{
	static_assert( std::is_base_of<T, U>::value );
	if (other.ptr)
	{
		auto oth_cl = other.cl;
		cl.clone = [oth_cl](const T* t) -> T* { return (T*)oth_cl.clone((U*)t); };
		cl.destroy = [oth_cl](T* t) -> void { oth_cl.destroy((U*)t); };
		ptr = other.ptr;
		other.ptr = nullptr;
	}
}

template<typename T>
template<typename U>
auto clone_ptr<T>::operator=(const U& other) -> clone_ptr&
{
	static_assert( std::is_base_of<T, U>::value );
	if (ptr)
		cl.destroy(ptr);

	auto oth_cl = make_cloner<U>();
	cl.clone = [oth_cl](const T* t) -> T* { return (T*)oth_cl.clone((U*)t); };
	cl.destroy = [oth_cl](T* t) -> void { oth_cl.destroy((U*)t); };

	ptr = cl.clone(&other);

	return *this;
}

template<typename T>
template<typename U>
clone_ptr<T>::clone_ptr(const U& other) 
{
	static_assert( std::is_base_of<T, U>::value );

	auto oth_cl = make_cloner<U>();
	cl.clone = [oth_cl](const T* t) -> T* { return (T*)oth_cl.clone((U*)t); };
	cl.destroy = [oth_cl](T* t) -> void { oth_cl.destroy((U*)t); };

	ptr = cl.clone(&other);

}


template<typename T>
void clone_ptr<T>::clear()
{
	if (ptr)
		cl.destroy(ptr);
	ptr = nullptr;
	cl.clear();
}

template<typename T, typename... Args>
clone_ptr<T> make_cloned(Args&&... args)
{
	return clone_ptr<T>{ T{std::forward<Args>(args)...} };
}



