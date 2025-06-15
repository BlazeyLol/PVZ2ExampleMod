#pragma once

#include "../memUtils.hpp"

#ifdef __arm__
#define OFFSET_RTWEAKPTR_GETREGISTRY 0x10D99A4
#define OFFSET_RTWEAKPTR_UNKFUNC1 0x10DFA5C
#define OFFSET_RTWEAKPTR_UNKFUNC2 0x10DF644
#define OFFSET_RTWEAKPTR_EQUALS 0x24D110
#define OFFSET_RTWEAKPTR_ISVALIDPTR 0x279198
#define OFFSET_RTWEAKPTR_RESOLVE 0x10DB9C0
#define OFFSET_RTWEAKPTR_FROMOTHER 0x10C8BF8
#define OFFSET_RTWEAKPTR_SETWEAKPTR 0x10C88C8
#elif __aarch64__
#define OFFSET_RTWEAKPTR_GETREGISTRY 0x14EAD90
#define OFFSET_RTWEAKPTR_UNKFUNC1 0x0
#define OFFSET_RTWEAKPTR_UNKFUNC2 0x0
#define OFFSET_RTWEAKPTR_EQUALS 0x0
#define OFFSET_RTWEAKPTR_ISVALIDPTR 0x0
#define OFFSET_RTWEAKPTR_RESOLVE 0x0
#define OFFSET_RTWEAKPTR_FROMOTHER 0x14D920C
#define OFFSET_RTWEAKPTR_SETWEAKPTR 0x14D8B08
#endif

namespace Sexy
{
	template <typename T>
	class RtWeakPtr
	{
	public:

		RtWeakPtr()
		{
			data1 = NULL;
			data2 = NULL;
		}

		~RtWeakPtr()
		{
			typedef void(*func)(RtWeakPtr*, int64_t);
			func pSetValue = (func)getActualOffset(OFFSET_RTWEAKPTR_SETWEAKPTR);
			pSetValue(this, 0LL);
		}

		RtWeakPtr(RtWeakPtr* other) {
			data1 = NULL;
			data2 = NULL;

			this->FromOther(other);
		}

		bool equals(RtWeakPtr<T>* other) {
			typedef bool(*eqOp)(RtWeakPtr<T>*, RtWeakPtr<T>*);
			eqOp pEqOp = (eqOp)getActualOffset(OFFSET_RTWEAKPTR_EQUALS);
			return pEqOp(this, other);
		}

		bool is_invalid()
		{
			return data1 == NULL;
		}

		bool is_valid()
		{
			typedef bool(*func)(RtWeakPtr<T>*);
			func pFunc = (func)getActualOffset(OFFSET_RTWEAKPTR_ISVALIDPTR);
			return pFunc(this);
		}

		int type_id()
		{
			return (this->data2 >> 0x11) & 0x3FFF;
		}

		int GetSomething()
		{
			return this->data2 >> 0x1F;
		}

		void FromOther(RtWeakPtr* other)
		{
			typedef void(*func)(RtWeakPtr*, RtWeakPtr*);
			func pSetValue = (func)getActualOffset(OFFSET_RTWEAKPTR_FROMOTHER);
			pSetValue(this, other);
		}

		// Get the object this points to, if valid.
		T* get()
		{
			typedef void* (*getV8)();
			getV8 pGetRegistry = (getV8)getActualOffset(OFFSET_RTWEAKPTR_GETREGISTRY);

			void* registry = pGetRegistry();

			typedef void (*sub10DFA5C)(RtWeakPtr*, void*, RtWeakPtr*);
			sub10DFA5C pSub10DFA5C = (sub10DFA5C)getActualOffset(OFFSET_RTWEAKPTR_UNKFUNC1);

			RtWeakPtr<T> temp;
			pSub10DFA5C(&temp, registry, this);

			LOGI("Every check passed so far %d %d", data1, data2);

			T* retVal = NULL;
			if (!temp.is_invalid())
			{
				LOGI("Not invalid");
				INT typeId = temp.type_id();

				typedef void* (*sub10DF644)(void*, INT);
				sub10DF644 pSub10DF644 = (sub10DF644)getActualOffset(OFFSET_RTWEAKPTR_UNKFUNC2);

				void* v11 = pSub10DF644(registry, typeId);
				if (v11)
				{
					LOGI("is correct type");
					typedef T* (*sub10DB9C0)(void*, RtWeakPtr*);
					sub10DB9C0 pResolveWeakPtr = (sub10DB9C0)getActualOffset(OFFSET_RTWEAKPTR_RESOLVE);
					retVal = pResolveWeakPtr(v11, &temp);
				}
			}

			return retVal;
		}

	private:
		INT data1;
		INT data2;
	};
}

INT getWeakPtr(INT addr);