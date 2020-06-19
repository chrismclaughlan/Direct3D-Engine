#include "Keyboard.h"

bool Keyboard::KeyIsPressed(uint8 keycode) const noexcept
{
	return keyStates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (keyBuff.size() > 0u)
	{
		Keyboard::Event e = keyBuff.front();
		keyBuff.pop();
		return e;
	}
	else
	{
		return Keyboard::Event();
	}
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return keyBuff.empty();
}

char Keyboard::ReadChar() noexcept
{
	if (charBuff.size() > 0u)
	{
		uint8 charCode = charBuff.front();
		charBuff.pop();
		return charCode;
	}
	else
	{
		return 0;
	}
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return charBuff.empty();
}

void Keyboard::FlushKey() noexcept
{
	keyBuff = std::queue<Event>();
}

void Keyboard::FlushChar() noexcept
{
	charBuff = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
	autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
	autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return autorepeatEnabled;
}

void Keyboard::OnKeyPressed(uint8 keycode) noexcept
{
	keyStates[keycode] = true;
	keyBuff.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(keyBuff);
}

void Keyboard::OnKeyReleased(uint8 keycode) noexcept
{
	keyStates[keycode] = false;
	keyBuff.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(keyBuff);
}

void Keyboard::OnChar(char character) noexcept
{
	charBuff.push(character);
	TrimBuffer(charBuff);
}

void Keyboard::ClearState() noexcept
{
	keyStates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > buffSize)
	{
		buffer.pop();
	}
}