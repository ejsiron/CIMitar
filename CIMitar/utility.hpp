#include <memory>

namespace CIMitar::Utility
{
	// Start with a class that has a data member of a pointer type.
	// Supply the pointer (ptr)
	// Supply the type of the class targeted by the pointer (PointedClass)
	//   and the name of its data member that you want to retrieve (ClassMemberName)
	// Supply the value that you want to use as a default (DefaultValue)
	//
	// If the class pointer (ptr) is nullptr, then the function returns DefaultValue
	// If the class pointer (ptr) is not nullptr, then the function returns the value
	//   in ptr->ClassMemberName.
	// The function does not check if ptr->ClassMemberName has a valid value. If it
	//   does not, this function may throw
	template <typename PointedClassMemberType, typename PointedClass>
	PointedClassMemberType GetValueOrDefault(const PointedClass* ptr, const PointedClassMemberType PointedClass::* ClassMemberName, PointedClassMemberType&& DefaultValue)
	{
		return ptr == nullptr ?
			DefaultValue : ptr->*ClassMemberName;
	}

	// Start with a class that has a data member of a pointer type.
	// Supply the pointer (ptr)
	// Supply the type of the class targeted by the pointer (PointedClass)
	//   and the name of its data member that you want to retrieve (ClassMemberName)
	//
	// If the class pointer (ptr) is nullptr, then the function returns a default-
	//   constructed instance of PointedClassMemberType
	// If the class pointer (ptr) is not nullptr, then the function returns the value
	//   in ptr->ClassMemberName.
	// The function does not check if ptr->ClassMemberName has a valid value. If it
	//   does not, this function may throw
	// If PointedClassMemberType does not have a default constructor, use
	//   GetValueOrDefault(PointedClass* ptr, PointedClassMemberType PointedClass::* ClassMemberName)
	template <typename PointedClassMemberType, typename PointedClass>
	PointedClassMemberType GetValueOrDefault(const PointedClass* ptr, const PointedClassMemberType PointedClass::* ClassMemberName)
	{
		return GetValueOrDefault(ptr, ClassMemberName, PointedClassMemberType{});
	}
}
