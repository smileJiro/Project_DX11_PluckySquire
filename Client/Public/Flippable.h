#pragma once
#include "Base.h"

BEGIN(Client)

class CFlippable
{
protected:
	_int	m_iFlipAnim1 = { 0 };
	_int	m_iFlipAnim1End = { 0 };
	_int	m_iFlipAnim2 = { 0 };
	_int	m_iFlipAnim2End = { 0 };
	_int	m_iCurAnim = { 0 };
};

END