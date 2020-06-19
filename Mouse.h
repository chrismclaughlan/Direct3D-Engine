#pragma once
#include "common_types.h"
#include <optional>
#include <queue>

class Mouse
{
	friend class Window;
public:
	//struct RawDelta
	//{
	//	int32 x, y;
	//};
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};
	private:
		Type type;
		bool leftIsPressed;
		bool rightIsPressed;
		int32 x;
		int32 y;
	public:
		Event() noexcept
			: type(Type::Invalid),
			leftIsPressed(false), rightIsPressed(false),
			x(0), y(0) {}
		Event(Type type, const Mouse& parent) noexcept
			: type(type),
			leftIsPressed(parent.leftIsPressed),
			rightIsPressed(parent.rightIsPressed),
			x(parent.x), y(parent.y) {}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		Type GetType() const noexcept
		{
			return type;
		}
		std::pair<int32, int32> GetPos() const noexcept
		{
			return{ x,y };
		}
		int32 GetPosX() const noexcept
		{
			return x;
		}
		int32 GetPosY() const noexcept
		{
			return y;
		}
		bool LeftIsPressed() const noexcept
		{
			return leftIsPressed;
		}
		bool RightIsPressed() const noexcept
		{
			return rightIsPressed;
		}
	};
public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<int32, int32> GetPos() const noexcept;
	//std::optional<RawDelta> ReadRawDelta() noexcept;
	int32 GetPosX() const noexcept;
	int32 GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	Mouse::Event Read() noexcept;
	//std::optional<Mouse::Event> Read() noexcept;
	bool IsEmpty() const noexcept
	{
		return buffer.empty();
	}
	void Flush() noexcept;
	//void EnableRaw() noexcept;
	//void DisableRaw() noexcept;
	//bool RawEnabled() const noexcept;
private:
	void OnMouseMove(int32 x, int32 y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	//void OnRawDelta(int32 dx, int32 dy) noexcept;
	void OnLeftPressed(int32 x, int32 y) noexcept;
	void OnLeftReleased(int32 x, int32 y) noexcept;
	void OnRightPressed(int32 x, int32 y) noexcept;
	void OnRightReleased(int32 x, int32 y) noexcept;
	void OnWheelUp(int32 x, int32 y) noexcept;
	void OnWheelDown(int32 x, int32 y) noexcept;
	void TrimBuffer() noexcept;
	//void TrimRawInputBuffer() noexcept;
	void OnWheelDelta(int32 x, int32 y, int32 delta) noexcept;
private:
	static constexpr uint32 bufferSize = 16u;
	int32 x;
	int32 y;
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	bool isInWindow = false;
	int wheelDeltaCarry = 0;
	//bool rawEnabled = false;
	std::queue<Event> buffer;
	//std::queue<RawDelta> rawDeltaBuffer;
};