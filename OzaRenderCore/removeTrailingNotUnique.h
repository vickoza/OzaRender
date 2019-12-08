#pragma once
#include <algorithm>
#include <xmemory>

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,3)
#pragma push_macro("new")
#undef new
#pragma warning(disable: 4244)

_STD_BEGIN
// TEMPLATE FUNCTION remove_trailing_not_unique
template<class _FwdIt, class _Pr> inline
_FwdIt _Remove_trailing_not_unique(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
{	// remove each matching _Val
	if (_First != _Last)
	{
		_FwdIt _Firstb = _First;
		_FwdIt _Firstc = _First;
		for (; ++_First != _Last; )
			if (!_Pred(*_Firstb, *_First))
				*++_Firstc = _STD move(*_First);
		return (/*_Rechecked*/(++_Firstb, _Remove_trailing_not_unique(/*_Unchecked*/(_Firstb), /*_Unchecked*/(++_Firstc), _Pred)));
	}
	return (_Last);
}

template<class _FwdIt, class _Pr> inline
_FwdIt remove_trailing_not_unique(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
{	// remove 
	//_DEBUG_RANGE(_First, _Last);
	//_DEBUG_POINTER_IF(_First != _Last && _STD next(_First) != _Last, _Pred);
	return (/*_Rechecked*/(_First,
		_Remove_trailing_not_unique(/*_Unchecked*/(_First),/* _Unchecked*/(_Last), _Pred)));
}

template<class _FwdIt> inline
_FwdIt remove_trailing_not_unique(_FwdIt _First, _FwdIt _Last)
{
	return (_STD remove_trailing_not_unique(_First, _Last, equal_to<>()));
}

_STD_END
#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop)