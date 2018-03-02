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
#include <imagine/util/bits.h>
#include <string>

namespace Input
{

class Device
{
public:
	static constexpr uint SUBTYPE_NONE = 0,
		SUBTYPE_XPERIA_PLAY = 1,
		SUBTYPE_PS3_CONTROLLER = 2,
		SUBTYPE_MOTO_DROID_KEYBOARD = 3,
		SUBTYPE_OUYA_CONTROLLER = 4,
		SUBTYPE_PANDORA_HANDHELD = 5,
		SUBTYPE_XBOX_360_CONTROLLER = 6,
		SUBTYPE_NVIDIA_SHIELD = 7,
		SUBTYPE_GENERIC_GAMEPAD = 8,
		SUBTYPE_APPLE_EXTENDED_GAMEPAD = 9;

	static constexpr uint
		TYPE_BIT_KEY_MISC = IG::bit(0),
		TYPE_BIT_KEYBOARD = IG::bit(1),
		TYPE_BIT_GAMEPAD = IG::bit(2),
		TYPE_BIT_JOYSTICK = IG::bit(3),
		TYPE_BIT_VIRTUAL = IG::bit(4),
		TYPE_BIT_MOUSE = IG::bit(5),
		TYPE_BIT_TOUCHSCREEN = IG::bit(6),
		TYPE_BIT_POWER_BUTTON = IG::bit(7);

	static constexpr uint
		AXIS_BIT_X = IG::bit(0), AXIS_BIT_Y = IG::bit(1), AXIS_BIT_Z = IG::bit(2),
		AXIS_BIT_RX = IG::bit(3), AXIS_BIT_RY = IG::bit(4), AXIS_BIT_RZ = IG::bit(5),
		AXIS_BIT_HAT_X = IG::bit(6), AXIS_BIT_HAT_Y = IG::bit(7),
		AXIS_BIT_LTRIGGER = IG::bit(8), AXIS_BIT_RTRIGGER = IG::bit(9),
		AXIS_BIT_RUDDER = IG::bit(10), AXIS_BIT_WHEEL = IG::bit(11),
		AXIS_BIT_GAS = IG::bit(12), AXIS_BIT_BRAKE = IG::bit(13);

	static constexpr uint
		AXIS_BITS_STICK_1 = AXIS_BIT_X | AXIS_BIT_Y,
		AXIS_BITS_STICK_2 = AXIS_BIT_Z | AXIS_BIT_RZ,
		AXIS_BITS_HAT = AXIS_BIT_HAT_X | AXIS_BIT_HAT_Y;

	struct Change
	{
		uint state;
		enum { ADDED, REMOVED, CHANGED, SHOWN, HIDDEN, CONNECT_ERROR };

		constexpr Change(uint state): state(state) {}
		bool added() const { return state == ADDED; }
		bool removed() const { return state == REMOVED; }
		bool changed() const { return state == CHANGED; }
		bool shown() const { return state == SHOWN; }
		bool hidden() const { return state == HIDDEN; }
		bool hadConnectError() const { return state == CONNECT_ERROR; }
	};

	Device() {}
	Device(uint devId, uint map, uint type, const char *name):
		name_{name}, map_{map}, type_{type}, devId{devId} {}
	virtual ~Device() {}

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

	uint enumId() const { return devId; }
	const char *name() const { return name_.c_str(); }
	uint map() const;
	uint typeBits() const;
	uint subtype() const { return subtype_; }

	bool operator ==(Device const& rhs) const;

	virtual void setICadeMode(bool on);
	virtual bool iCadeMode() const { return false; }
	virtual void setJoystickAxisAsDpadBits(uint axisMask) {}
	virtual uint joystickAxisAsDpadBits() { return 0; }
	virtual uint joystickAxisAsDpadBitsDefault() { return 0; }
	virtual uint joystickAxisBits() { return 0; }

	virtual const char *keyName(Key k) const;

	// TODO
	//bool isDisconnectable() { return 0; }
	//void disconnect() { }

	static bool anyTypeBitsPresent(uint typeBits);

protected:
	std::string name_{""};
	uint map_ = 0;
	uint type_ = 0;
	uint devId = 0;
public:
	uint subtype_ = 0;
	uint idx = 0;
};

}
