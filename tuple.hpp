#pragma once

#include <cstddef>
#include <tuple>

namespace ctti
{
	template<std::size_t Padding>
	struct pad
	{
		char padding[Padding];
	};

	template<>
	struct pad<0>
	{};

	template<typename T, int Padding>
	struct slot {};

	namespace detail
	{
		template<typename T, int Padding>
		struct tuple_slot
		{
			using elem_type = T;

			template<typename... Args>
			tuple_slot(Args&&... args)
			{
				new(&storage[begin_]) T(std::forward<Args>(args)...);
			}

			tuple_slot(const tuple_slot& ts) : 
				tuple_slot{ts.elem()}
			{}

			tuple_slot(tuple_slot&& ts) :
				tuple_slot{std::move(ts.elem())}
			{}

			tuple_slot& operator=(const tuple_slot& ts)
			{
				elem() = ts.elem();

				return *this;
			}

			tuple_slot& operator=(tuple_slot&& ts)
			{
				elem() = std::move(ts.elem());

				return *this;
			}

			const T& elem() const
			{
				return *(reinterpret_cast<T*>(&storage[begin_]));
			}

			T& elem()
			{
				return *(reinterpret_cast<T*>(&storage[begin_]));
			}

			~tuple_slot()
			{
				elem().~T();
			}

			static constexpr int padding_bytes = Padding;

		private:
			alignas(char) char storage[sizeof(T) + ((Padding >= 0) ? Padding : -Padding)];
			static constexpr std::size_t begin_ = (Padding < 0) ? -Padding : 0;
		};

		template<typename T, int Padding>
		const T& get(const ctti::detail::tuple_slot<T,Padding>& pair)
		{
			return pair.elem();
		}

		template<typename T, int Padding>
		T& get(ctti::detail::tuple_slot<T, Padding>& pair)
		{
			return pair.elem();
		}

		template<typename T, int Padding>
		T&& get(ctti::detail::tuple_slot<T, Padding>&& pair)
		{
			return std::move(pair.elem());
		}
	}

	template<typename... Slots>
	struct interleaved_storage;

	template<typename... Ts, int... Paddings>
	struct interleaved_storage<slot<Ts,Paddings>...> : std::tuple<ctti::detail::tuple_slot<Ts, Paddings>...>
	{
		using base_t = std::tuple<ctti::detail::tuple_slot<Ts, Paddings>...>;

		template<typename... Args>
		explicit interleaved_storage(Args&&... args) :
			base_t{ ctti::detail::tuple_slot<Args, Paddings>{std::forward<Args>(args)}... }
		{}
	};

	template<int... Paddings, typename... Ts>
	interleaved_storage<slot<std::decay_t<Ts>,Paddings>...> make_interleaved_storage(Ts&&... args)
	{
		return interleaved_storage<slot<std::decay_t<Ts>, Paddings>...>( std::forward<Ts>(args)... );
	}
}

namespace std
{
	template<std::size_t Idx, typename... Ts, std::size_t... Paddings>
	struct tuple_element<Idx, ctti::interleaved_storage<ctti::slot<Ts,Paddings>...>>
	{
		using type = typename std::tuple_element<Idx, std::tuple<Ts...>>::type;
	};

	template<std::size_t Idx, typename... Ts, std::size_t... Paddings>
	constexpr std::tuple_element_t<Idx, std::tuple<Ts...>> get(const ctti::interleaved_storage<ctti::slot<Ts, Paddings>...>& tuple)
	{
		return std::get<Idx>(static_cast<std::tuple<ctti::detail::tuple_slot<Ts, Paddings>...>&>(tuple)).first;
	}

	template<std::size_t Idx, typename... Ts, std::size_t... Paddings>
	constexpr std::tuple_element_t<Idx, std::tuple<Ts...>>& get(ctti::interleaved_storage<ctti::slot<Ts, Paddings>...>& tuple)
	{
		return std::get<Idx>(static_cast<std::tuple<ctti::detail::tuple_slot<Ts, Paddings>...>&>(tuple)).first;
	}

	template<std::size_t Idx, typename... Ts, std::size_t... Paddings>
	constexpr std::tuple_element_t<Idx, std::tuple<Ts...>>&& get(ctti::interleaved_storage<ctti::slot<Ts, Paddings>...>&& tuple)
	{
		return std::get<Idx>(static_cast<std::tuple<ctti::detail::tuple_slot<Ts, Paddings>...>&&>(std::move(tuple))).first;
	}
}