/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
 *                 2020 by Luca Cireddu IS0GVH
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "ysf_repeater.hpp"

YSFRepeater::YSFRepeater() :
        m_callsign(),
        m_address(),
        m_port(0U),
        m_timer(1000U, 60U) {
}
