#include "common_types.h"
#include "Mouse.h"
#include <optional>

#define WHEEL_DELTA 120

std::pair<int32, int32> Mouse::GetPos() const noexcept
{
	return { x,y };
}

//std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
//{
//	if (rawDeltaBuffer.empty())
//	{
//		return std::nullopt;
//	}
//	const RawDelta d = rawDeltaBuffer.front();
//	rawDeltaBuffer.pop();
//	return d;
//}

int32 Mouse::GetPosX() const noexcept
{
	return x;
}

int32 Mouse::GetPosY() const noexcept
{
	return y;
}

bool Mouse::IsInWindow() const noexcept
{
	return isInWindow;
}

bool Mouse::LeftIsPressed() const noexcept
{
	return leftIsPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
	return rightIsPressed;
}

//std::optional<Mouse::Event> Mouse::Read() noexcept
//{
//	if (buffer.size() > 0u)
//	{
//		Mouse::Event e = buffer.front();
//		buffer.pop();
//		return e;
//	}
//	return {};
//}

Mouse::Event Mouse::Read() noexcept
{
	if (buffer.size() > 0u)
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	return {};
}

void Mouse::Flush() noexcept
{
	buffer = std::queue<Event>();
}

//void Mouse::EnableRaw() noexcept
//{
//	rawEnabled = true;
//}

//void Mouse::DisableRaw() noexcept
//{
//	rawEnabled = false;
//}

//bool Mouse::RawEnabled() const noexcept
//{
//	return rawEnabled;
//}

void Mouse::OnMouseMove(int32 newx, int32 newy) noexcept
{
	x = newx;
	y = newy;

	buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	isInWindow = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	isInWindow = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

//void Mouse::OnRawDelta(int32 dx, int32 dy) noexcept
//{
//	rawDeltaBuffer.push({ dx,dy });
//	TrimBuffer();
//}

void Mouse::OnLeftPressed(int32 x, int32 y) noexcept
{
	leftIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(int32 x, int32 y) noexcept
{
	leftIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(int32 x, int32 y) noexcept
{
	rightIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(int32 x, int32 y) noexcept
{
	rightIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(int32 x, int32 y) noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(int32 x, int32 y) noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}

//void Mouse::TrimRawInputBuffer() noexcept
//{
//	while (rawDeltaBuffer.size() > bufferSize)
//	{
//		rawDeltaBuffer.pop();
//	}
//}

void Mouse::OnWheelDelta(int32 x, int32 y, int32 delta) noexcept
{
	wheelDeltaCarry += delta;
	// generate events for every 120 
	while (wheelDeltaCarry >= WHEEL_DELTA)
	{
		wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}
	while (wheelDeltaCarry <= -WHEEL_DELTA)
	{
		wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}