// Copyright(c) Burning Galaxy LTD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


struct FBaseAttributes
{
protected:
	float Value = 0.0f;
	float MaxValue = 0.0f;
public:
	FBaseAttributes(){};
	FBaseAttributes(const FBaseAttributes& rhs)
	 : Value(rhs.Value), MaxValue(rhs.MaxValue)
	{};
	FBaseAttributes(const float& Amount)		
	{
		Init(Amount);
	};
	void Init(const float& Amount)
	{
		Value += Amount;
		MaxValue += Amount;
	}

	void SetMax(const float& maxValue)
	{		
		MaxValue = maxValue;
	}

	// 값 추가
	virtual void Add(const float& Amount)
	{
		Value += Amount;
	}

	// 값 제거
	void Remove(const float& Amount)
	{
		Value -= Amount;

		// 값이 음수가 되지 않도록 보정
		if (Value < 0.0f)
		{
			Value = 0.0f;
		}
	}

	// 값이 0인지 확인
	bool IsZero() const
	{
		return Value == 0.0f;
	}

	const float GetValue() const {return Value;}
	const float GetMaxValue() const {return MaxValue;}
};




struct FHealthPoint : public FBaseAttributes
{
public:
	FHealthPoint() = default;
	FHealthPoint(const FHealthPoint& rhs)
		: FBaseAttributes(rhs)
	{};
	FHealthPoint(const float& Amount)
	{
		Init(Amount);
	};
};



//DECLARE_DELEGATE(FOverExperienceDelegate);
struct FExperiencePoint : public FBaseAttributes
{
public:
	FExperiencePoint() = default;
	FExperiencePoint(const FExperiencePoint& rhs)
		: FBaseAttributes(rhs)
	{};
	FExperiencePoint(const float& Amount)
	{
		Init(Amount);
	};

	//레벨업시 true!
	bool AddExpr(const float& Amount)
	{
		Value += Amount;
		if (MaxValue <= Value)
		{			
			Value = Value - MaxValue;
			return true;
		}

		return false;
	}

};