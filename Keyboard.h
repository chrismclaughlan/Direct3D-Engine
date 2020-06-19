#pragma once
#include "common_types.h"
#include <queue>
#include <bitset>

class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press, Release, Invalid
		};
	private:
		Type type;
		uint8 code;
	public:
		Event() : type(Type::Invalid), code(0u) {}
		Event(Type type, uint8 code) noexcept : type(type), code(code) {}
		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		uint8 GetCode() const noexcept
		{
			return code;
		}
	};
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	// Key event
	bool KeyIsPressed(uint8 keycode) const noexcept;
	Event ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;
	// Char event
	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	
	void Flush() noexcept;
	// Autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;
private:
	void OnKeyPressed(uint8 keycode) noexcept;
	void OnKeyReleased(uint8 keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
private:
	static constexpr uint32 nKeys = 256u;
	static constexpr uint32 buffSize = 16u;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keyStates;
	std::queue<Event> keyBuff;
	std::queue<char> charBuff;
};