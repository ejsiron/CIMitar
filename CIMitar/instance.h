#pragma once
#pragma comment(lib, "mi.dll")
#include <mi.h>	// caution: earlier versions of mi.h did not have a header guard
#include <memory>
#include <string>

namespace CIMitar
{
	// wraps MI_Instance
	class Instance
	{
	private:
		bool OnHeap;
		std::unique_ptr<MI_Instance> TheInstance = nullptr;

	public:
		Instance(MI_Instance*);
		virtual ~Instance();
		MI_Instance& Clone();
		void Refresh();
		MI_Class GetClass() const;
		const std::wstring GetClassName() const;
		void GetElement(const std::wstring& ElementName) const;
		void GetElement(const int Index) const;
		template <typename T>
		const bool SetElementValue(const std::wstring& ElementName, T value); // use type traits to constrain
		template <typename T>
		const bool SetElementValue(const int ElementIndex, T value); // use type traits to constrain
		const int GetElementCount() const;
		const std::wstring GetNamespace() const;
		const std::wstring GetServerName() const;
		const bool IsA(const std::wstring& ClassName) const;
	};

	class DynamicInstance : Instance
	{
	public:
		const bool AddElement();	// use type traits to constrain
		const bool ClearElement(const std::wstring&);
	};
}

