#pragma once

/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#include <imagine/config/defs.hh>
#include <imagine/input/config.hh>
#include <imagine/input/inputDefs.hh>
#include <imagine/input/AxisKeyEmu.hh>
#include <imagine/time/Time.hh>
#include <imagine/util/bit.hh>
#include <imagine/util/DelegateFunc.hh>
#include <imagine/util/utility.h>
#include <string>
#include <string_view>
#include <span>
#include <memory>

namespace IG
{
class ApplicationContext;
}

namespace IG::Input
{

class Axis;

struct KeyNameFlags
{
	bool basicModifiers{};
};

class Device
{
public:
	using Subtype = DeviceSubtype;
	using TypeBits = DeviceTypeBits;

	static constexpr TypeBits
		TYPE_BIT_KEY_MISC = bit(0),
		TYPE_BIT_KEYBOARD = bit(1),
		TYPE_BIT_GAMEPAD = bit(2),
		TYPE_BIT_JOYSTICK = bit(3),
		TYPE_BIT_VIRTUAL = bit(4),
		TYPE_BIT_MOUSE = bit(5),
		TYPE_BIT_TOUCHSCREEN = bit(6),
		TYPE_BIT_POWER_BUTTON = bit(7);

	static constexpr uint32_t
		AXIS_BIT_X = bit(0), AXIS_BIT_Y = bit(1), AXIS_BIT_Z = bit(2),
		AXIS_BIT_RX = bit(3), AXIS_BIT_RY = bit(4), AXIS_BIT_RZ = bit(5),
		AXIS_BIT_HAT_X = bit(6), AXIS_BIT_HAT_Y = bit(7),
		AXIS_BIT_LTRIGGER = bit(8), AXIS_BIT_RTRIGGER = bit(9),
		AXIS_BIT_RUDDER = bit(10), AXIS_BIT_WHEEL = bit(11),
		AXIS_BIT_GAS = bit(12), AXIS_BIT_BRAKE = bit(13);

	static constexpr uint32_t
		AXIS_BITS_STICK_1 = AXIS_BIT_X | AXIS_BIT_Y,
		AXIS_BITS_STICK_2 = AXIS_BIT_Z | AXIS_BIT_RZ,
		AXIS_BITS_HAT = AXIS_BIT_HAT_X | AXIS_BIT_HAT_Y;

	Device() = default;
	Device(int id, Map map, TypeBits, std::string name);
	virtual ~Device() = default;

	bool hasKeyboard() const
	{
		return typeBits() & TYPE_BIT_KEYBOARD;
	}

	bool hasGamepad() const
	{
		return typeBits() & TYPE_BIT_GAMEPAD;
	}

	bool hasJoystick() const
	{
		return typeBits() & TYPE_BIT_JOYSTICK;
	}

	bool isVirtual() const
	{
		return typeBits() & TYPE_BIT_VIRTUAL;
	}

	bool hasKeys() const
	{
		return hasKeyboard() || hasGamepad() || typeBits() & TYPE_BIT_KEY_MISC;
	}

	bool isPowerButton() const
	{
		return typeBits() & TYPE_BIT_POWER_BUTTON;
	}

	constexpr bool isModifierKey(Key k) const
	{
		if(map() != Map::SYSTEM)
			return false;
		using namespace Keycode;
		switch(k)
		{
			case LALT:
			case RALT:
			case LSHIFT:
			case RSHIFT:
			case LCTRL:
			case RCTRL:
				return true;
		}
		return false;
	}

	constexpr Key swapModifierKey(Key k) const
	{
		if(map() != Map::SYSTEM)
			return false;
		using namespace Keycode;
		switch(k)
		{
			case LALT: return RALT;
			case RALT: return LALT;
			case LSHIFT: return RSHIFT;
			case RSHIFT: return LSHIFT;
			case LCTRL: return RCTRL;
			case RCTRL: return LCTRL;
		}
		return k;
	}

	int id() const { return id_; }
	uint8_t enumId() const { return enumId_; }
	void setEnumId(uint8_t id) { enumId_ = id; }
	std::string_view name() const { return name_; }
	Map map() const;
	TypeBits typeBits() const;
	Subtype subtype() const { return subtype_; }
	void setSubtype(Subtype s) { subtype_ = s; }

	bool operator ==(Device const& rhs) const = default;

	virtual void setICadeMode(bool on);
	[[nodiscard]]
	virtual bool iCadeMode() const;
	void setJoystickAxisAsDpadBits(uint32_t axisMask);
	uint32_t joystickAxisAsDpadBits();
	Axis *motionAxis(AxisId);
	virtual std::span<Axis> motionAxes();

	virtual const char *keyName(Key k) const;
	std::string keyString(Key k, KeyNameFlags flags = {}) const;

	// TODO
	//bool isDisconnectable() { return 0; }
	//void disconnect() { }

	static bool anyTypeBitsPresent(ApplicationContext, TypeBits);

	template <class T>
	T &makeAppData(auto &&...args)
	{
		appDataPtr = std::make_shared<T>(IG_forward(args)...);
		return *appData<T>();
	}

	template<class T>
	T *appData() const
	{
		return static_cast<T*>(appDataPtr.get());
	}

protected:
	std::shared_ptr<void> appDataPtr;
	std::string name_;
	int id_{};
	TypeBits typeBits_{};
	uint8_t enumId_{};
	Map map_{Map::UNKNOWN};
	Subtype subtype_{};

	void updateGamepadSubtype(std::string_view name, uint32_t vendorProductId);
};

class Axis
{
public:
	constexpr Axis() = default;
	Axis(const Device &, AxisId id, float scaler = 1.f);
	void setEmulatesDirectionKeys(const Device &, bool);
	bool emulatesDirectionKeys() const;
	constexpr AxisId id() const { return id_; }
	uint32_t idBit() const;
	bool update(float pos, Map map, SteadyClockTimePoint time, const Device &, Window &, bool normalized = false);

protected:
	float scaler{};
	AxisKeyEmu keyEmu{};
	AxisId id_{};
};

}
