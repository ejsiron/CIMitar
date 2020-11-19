#pragma once
#pragma comment(lib, "mi.dll")
#include <mi.h>	// caution: earlier versions of mi.h did not have a header guard
#include <memory>

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
		~Instance();
		MI_Instance Clone();
		void Refresh();
	};
}
