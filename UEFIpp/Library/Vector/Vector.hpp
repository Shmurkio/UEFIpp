#pragma once

#include <cstdint>
#include "../Memory/Memory.hpp"

template<typename T>
class Vector
{
private:
    T* Data_;
    uint64_t Size_;
    uint64_t Capacity_;

private:
    static auto Allocate(uint64_t Capacity) -> T*
    {
        if (!Capacity)
        {
            return nullptr;
        }

        void* Allocation = nullptr;
        auto NumberOfBytes = Capacity * sizeof(T);

        if (!Memory::AllocatePool(Allocation, NumberOfBytes, false, false))
        {
            return nullptr;
        }

        return Cast::To<T*>(Allocation);
    }

    static auto Free(T* Buffer) -> void
    {
        if (!Buffer)
        {
            return;
        }

        void* Allocation = Cast::To<void*>(Buffer);
        Memory::FreePool(Allocation, false);
    }

    template<typename... TArgs>
    static auto Construct(T* Address, TArgs&&... Args) -> void
    {
        new (Address) T(Memory::Forward<TArgs>(Args)...);
    }

    static auto Destroy(T* Address) -> void
    {
        if (!Address)
        {
            return;
        }

        Address->~T();
    }

    static auto DestroyRange(T* Data, uint64_t Count) -> void
    {
        if (!Data)
        {
            return;
        }

        for (uint64_t i = 0; i < Count; ++i)
        {
            Destroy(&Data[i]);
        }
    }

public:
    Vector() : Data_(nullptr), Size_(0), Capacity_(0)
    {
    }

    Vector(const Vector&) = delete;
    auto operator=(const Vector&) -> Vector & = delete;

    Vector(Vector&& Other) noexcept : Data_(Other.Data_), Size_(Other.Size_), Capacity_(Other.Capacity_)
    {
        Other.Data_ = nullptr;
        Other.Size_ = 0;
        Other.Capacity_ = 0;
    }

    auto operator=(Vector&& Other) noexcept -> Vector&
    {
        if (this == &Other)
        {
            return *this;
        }

        Clear();
        Free(Data_);

        Data_ = Other.Data_;
        Size_ = Other.Size_;
        Capacity_ = Other.Capacity_;

        Other.Data_ = nullptr;
        Other.Size_ = 0;
        Other.Capacity_ = 0;

        return *this;
    }

    ~Vector()
    {
        Clear();
        Free(Data_);
        Data_ = nullptr;
        Capacity_ = 0;
    }

    auto Size() const -> uint64_t
    {
        return Size_;
    }

    auto Capacity() const -> uint64_t
    {
        return Capacity_;
    }

    auto Empty() const -> bool
    {
        return Size_ == 0;
    }

    auto Data() -> T*
    {
        return Data_;
    }

    auto Data() const -> const T*
    {
        return Data_;
    }

    auto begin() -> T*
    {
        return Data_;
    }

    auto end() -> T*
    {
        return Data_ + Size_;
    }

    auto begin() const -> const T*
    {
        return Data_;
    }

    auto end() const -> const T*
    {
        return Data_ + Size_;
    }

    auto operator[](uint64_t Index) -> T&
    {
        return Data_[Index];
    }

    auto operator[](uint64_t Index) const -> const T&
    {
        return Data_[Index];
    }

    auto Front() -> T&
    {
        return Data_[0];
    }

    auto Front() const -> const T&
    {
        return Data_[0];
    }

    auto Back() -> T&
    {
        return Data_[Size_ - 1];
    }

    auto Back() const -> const T&
    {
        return Data_[Size_ - 1];
    }

    auto Clear() -> void
    {
        DestroyRange(Data_, Size_);
        Size_ = 0;
    }

    auto Reserve(uint64_t NewCapacity) -> bool
    {
        if (NewCapacity <= Capacity_)
        {
            return true;
        }

        auto NewData = Allocate(NewCapacity);

        if (!NewData)
        {
            return false;
        }

        for (uint64_t i = 0; i < Size_; ++i)
        {
            Construct(&NewData[i], Memory::Move(Data_[i]));
            Destroy(&Data_[i]);
        }

        Free(Data_);
        Data_ = NewData;
        Capacity_ = NewCapacity;

        return true;
    }

    auto PushBack(const T& Value) -> bool
    {
        if (Size_ >= Capacity_)
        {
            auto NewCapacity = Capacity_ ? (Capacity_ * 2) : 4;

            if (!Reserve(NewCapacity))
            {
                return false;
            }
        }

        Construct(&Data_[Size_], Value);
        ++Size_;

        return true;
    }

    auto PushBack(T&& Value) -> bool
    {
        if (Size_ >= Capacity_)
        {
            auto NewCapacity = Capacity_ ? (Capacity_ * 2) : 4;

            if (!Reserve(NewCapacity))
            {
                return false;
            }
        }

        Construct(&Data_[Size_], Memory::Move(Value));
        ++Size_;

        return true;
    }

    auto PopBack() -> bool
    {
        if (!Size_)
        {
            return false;
        }

        --Size_;
        Destroy(&Data_[Size_]);

        return true;
    }

    auto Resize(uint64_t NewSize) -> bool
    {
        if (NewSize < Size_)
        {
            for (uint64_t i = NewSize; i < Size_; ++i)
            {
                Destroy(&Data_[i]);
            }

            Size_ = NewSize;
            return true;
        }

        if (NewSize > Capacity_)
        {
            if (!Reserve(NewSize))
            {
                return false;
            }
        }

        for (uint64_t i = Size_; i < NewSize; ++i)
        {
            Construct(&Data_[i]);
        }

        Size_ = NewSize;
        return true;
    }

    auto Append(const T* Source, uint64_t Count) -> bool
    {
        if (!Source || !Count)
        {
            return true;
        }

        if (Size_ + Count > Capacity_)
        {
            uint64_t NewCapacity = Capacity_ ? Capacity_ : 4;
            while (NewCapacity < Size_ + Count)
            {
                NewCapacity *= 2;
            }

            if (!Reserve(NewCapacity))
            {
                return false;
            }
        }

        for (uint64_t i = 0; i < Count; ++i)
        {
            Construct(&Data_[Size_ + i], Source[i]);
        }

        Size_ += Count;
        return true;
    }

    auto RemovePrefix(uint64_t Count) -> bool
    {
        if (Count > Size_)
        {
            return false;
        }

        if (Count == 0)
        {
            return true;
        }

        const uint64_t Remaining = Size_ - Count;

        for (uint64_t i = 0; i < Remaining; ++i)
        {
            Data_[i] = Memory::Move(Data_[i + Count]);
        }

        for (uint64_t i = Remaining; i < Size_; ++i)
        {
            Destroy(&Data_[i]);
        }

        Size_ = Remaining;
        return true;
    }

    auto AppendByte(const T& Value) -> bool
    {
        return PushBack(Value);
    }

    auto AppendByte(T&& Value) -> bool
    {
        return PushBack(Memory::Move(Value));
    }

    auto Append(const Vector<T>& Other) -> bool
    {
        return Append(Other.Data(), Other.Size());
    }

    auto Assign(const T* Source, uint64_t Count) -> bool
    {
        Clear();
        return Append(Source, Count);
    }
};