#include "tuple.hpp"
#include <iostream>

template<typename T, typename = std::enable_if_t<!std::is_pointer<T>::value>>
std::intptr_t between(const T& lhs, const T& rhs)
{
	return reinterpret_cast<std::intptr_t>(&lhs) - reinterpret_cast<std::intptr_t>(&rhs);
}

template<typename T>
std::intptr_t between(const T* lhs, const T* rhs)
{
	return reinterpret_cast<std::intptr_t>(lhs) - reinterpret_cast<std::intptr_t>(rhs);
}

template<typename T, typename U, typename = std::enable_if_t<!std::is_pointer<T>::value && !std::is_pointer<U>::value>>
std::intptr_t between(const T& lhs, const U& rhs)
{
	return reinterpret_cast<std::intptr_t>(&lhs) - reinterpret_cast<std::intptr_t>(&rhs);
}

template<typename T, typename U>
std::intptr_t between(const T* lhs, const U* rhs)
{
	return reinterpret_cast<std::intptr_t>(lhs) - reinterpret_cast<std::intptr_t>(rhs);
}

struct foo
{
	int i, j, k;
};

int main()
{
    auto tuple = ctti::make_interleaved_storage<-8,0,-8>(1, 2, 3);
	foo f;
	
	auto* tuple_addr = &tuple;
	auto* first  = &std::get<0>(tuple);
	auto* second = &std::get<1>(tuple);
	auto* third  = &std::get<2>(tuple);

	std::cout << "sizeof(int): " << sizeof(int) << "\n";
	std::cout << "sizeof(tuple): " << sizeof(decltype(tuple)) << "\n";
	std::cout << "sizeof(struct): " << sizeof(decltype(f)) << "\n";
	std::cout << "sizeof(tuple_slot<int,0>): " << sizeof(ctti::detail::tuple_slot<int,0>) << "\n";
	std::cout << "tuple:  " << tuple_addr << "\n";
	std::cout << "first:  " << first << "\n";
	std::cout << "second: " << second << "\n";
	std::cout << "third:  " << third << "\n";

	std::cout << "struct: " << &f << "\n";
	std::cout << "i:      " << &f.i << "\n";
	std::cout << "j:      " << &f.j << "\n";
	std::cout << "k:      " << &f.k << "\n";

	std::cout << "third <--> tuple:  " << between(third, &tuple) << " bytes\n";
	std::cout << "second <--> third: " << between(second, third) << " bytes\n";
	std::cout << "first <--> second: " << between(first, second) << " bytes\n";

	std::cin.get();
}