#pragma once
#include "CIMitar.hpp"

template <typename T>
class Option
{
protected:
	T value;
#pragma warning(push)
#pragma warning(disable: 26495)	// stops complaints about uninitialized value in instantiated classes
	Option() noexcept {}
#pragma warning(pop)
	Option(T Value) noexcept :value(Value) {}
public:
	T Get() const noexcept { return value; }
	virtual void Set(T Value) { value = Value; }
	void operator=(T& Value) { Set(Value); }
	void operator=(const T& Value) { Set(Value); }
};

// TODO: investigate templating an application function that takes the *Set* function and translates parameters

template <typename T>
class NativeOption :public Option<T>
{
private:
	std::set<const void*>& optionlist;
public:
	void swap(NativeOption& SwapSource) noexcept
	{
		std::swap(this->value, SwapSource.value);
		std::swap(optionlist, SwapSource.optionlist);
	}
	NativeOption(std::set<const void*>& OwningOptionList) noexcept :optionlist(OwningOptionList) {}
	NativeOption(const NativeOption& CopySource)noexcept :optionlist(CopySource.optionlist) { this->value = CopySource.value; }
	NativeOption& operator=(NativeOption CopySource) noexcept
	{
		if (this != &CopySource)
		{
			swap(CopySource);
		}
		return *this;
	}
	void Reset() noexcept { optionlist.erase(this); }
	void Set(T Value) override { this->value = Value; optionlist.insert(this); }
	const bool IsOverridden() const noexcept
	{
		auto found{ optionlist.find(this) };
		return found != optionlist.end();
	}
};
template <typename T>
void swap(NativeOption<T>& lhs, NativeOption<T>& rhs)
{
	lhs.swap(rhs);
}

template <typename T>
class CustomSessionOption :public Option<T>
{
private:
	std::wstring customname{};
public:
	CustomSessionOption() noexcept {}
	CustomSessionOption(std::wstring CustomName, T Value) :customname(CustomName), Option<T>(Value) {}
	void SetName(std::wstring& NewName) { customname = NewName; }
};
