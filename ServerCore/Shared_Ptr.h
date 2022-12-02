#pragma once

template<typename T>
class Shared_Ptr
{
private:
	T* p = nullptr;
public:
	Shared_Ptr() {};
	Shared_Ptr(T* ptr);
	Shared_Ptr(const Shared_Ptr& ptr) { Set(ptr.p); }
	Shared_Ptr(Shared_Ptr&& ptr) { p = ptr.p; ptr.p = nullptr; }
	
	template<typename t>
	Shared_Ptr(const Shared_Ptr<t>& ptr) { Set(reinterpret_cast<T*>(ptr.p)); }
	~Shared_Ptr() { Release(); }

	Shared_Ptr& operator=(const Shared_Ptr& ptr)
	{
		if (p != ptr.p)
		{
			Release();
			Set(ptr.p);
		}
		return *this;
	}

	Shared_Ptr& operator=(Shared_Ptr&& ptr)
	{
		Release();
		p = ptr.p;
		ptr.p = nullptr;
		return *this;
	}
	
	bool operator==(const Shared_Ptr& ptr) { return p == ptr.p; }
	bool operator==(T* _p)const { return p == _p; }

	bool operator!=(const Shared_Ptr& ptr) { return p != ptr.p; }
	bool operator!=(T* _p) const {return p == _p; }

	bool operator<(const Shared_Ptr& ptr) { return p < ptr.p; }

	T* operator*() { return p; }
	const T* operator*() const { return p; }
	operator T* ()const { return p; }
	T* operator->() { return p; }
	const T* operator->() const { return p; }

	bool IsNull() { return p == nullptr; }
private:

	
	inline void Set(T* ptr)
	{
		p = ptr;
		if (p != nullptr)
		{
			p->AddRef();
		}
	}
	inline void Release()
	{
		if (p != nullptr)
		{
			p->RemoveRef();
			p = nullptr;
		}
	}
};

