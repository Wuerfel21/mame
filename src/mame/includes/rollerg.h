// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
/*************************************************************************

    Rollergames

*************************************************************************/
#ifndef MAME_INCLUDES_ROLLERG_H
#define MAME_INCLUDES_ROLLERG_H

#pragma once

#include "cpu/m6809/konami.h" /* for the callback and the firq irq definition */
#include "machine/k053252.h"
#include "video/k051316.h"
#include "video/k053244_k053245.h"
#include "video/konami_helper.h"

class rollerg_state : public driver_device
{
public:
	rollerg_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_k053244(*this, "k053244"),
		m_k051316(*this, "k051316"),
		m_k053252(*this, "k053252")
	{ }

	void rollerg(machine_config &config);

private:
	enum
	{
		TIMER_NMI
	};

	/* misc */
	int        m_readzoomroms = 0;
	emu_timer *m_nmi_timer = nullptr;

	/* devices */
	required_device<konami_cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	required_device<k05324x_device> m_k053244;
	required_device<k051316_device> m_k051316;
	required_device<k053252_device> m_k053252;
	void rollerg_0010_w(uint8_t data);
	uint8_t rollerg_k051316_r(offs_t offset);
	void soundirq_w(uint8_t data);
	void sound_arm_nmi_w(uint8_t data);
	uint8_t pip_r();
	DECLARE_WRITE_LINE_MEMBER(rollerg_irq_ack_w);
	virtual void machine_start() override;
	virtual void machine_reset() override;
	uint32_t screen_update_rollerg(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	K05324X_CB_MEMBER(sprite_callback);
	K051316_CB_MEMBER(zoom_callback);
	void banking_callback(uint8_t data);

	void rollerg_map(address_map &map);
	void rollerg_sound_map(address_map &map);

	virtual void device_timer(emu_timer &timer, device_timer_id id, int param) override;
};

#endif // MAME_INCLUDES_ROLLERG_H
