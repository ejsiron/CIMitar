#pragma once
#include <set>
#include <string>

namespace CIMitar::Infrastructure
{
	class LocalAddresses sealed
	{
	private:
		static size_t refcount;
		static std::set<std::wstring> Addresses;
	public:
		LocalAddresses() noexcept;
		LocalAddresses(const LocalAddresses&) noexcept;
		LocalAddresses& operator=(const LocalAddresses&) noexcept;
		~LocalAddresses();
		const std::set<std::wstring> operator()() const noexcept;
	};
}
