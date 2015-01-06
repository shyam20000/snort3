//--------------------------------------------------------------------------
// Copyright (C) 2014-2015 Cisco and/or its affiliates. All rights reserved.
// Copyright (C) 2005-2013 Sourcefire, Inc.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------

#ifndef NORM_H
#define NORM_H

#include "normalize.h"
#include "protocols/packet_manager.h"
#include "utils/stats.h"

struct NormalizerConfig;
struct Packet;

// all normalizers look like this:
// the return is 1 if packet was changed, else 0
typedef int (*NormalFunc)(  // FIXIT-L why is this exposed?
    struct NormalizerConfig*, Packet*, uint8_t layer, int changes);

extern const PegInfo norm_names[];

struct NormalizerConfig
{
    uint32_t normalizer_flags;
    uint8_t normalizer_options[32];

    // these must be in the same order PROTO_IDs are defined!
    // if entry is NULL, proto doesn't have normalization or it is disabled
    NormalFunc normalizers[PacketManager::max_protocols()];
};

int Norm_SetConfig(NormalizerConfig*);
int Norm_Packet(NormalizerConfig*, Packet*);

static inline void Norm_Enable(NormalizerConfig* nc, NormFlags nf)
{
    nc->normalizer_flags |= nf;
}

static inline void Norm_Disable(NormalizerConfig* nc, NormFlags nf)
{
    nc->normalizer_flags &= ~nf;
}

static inline void Norm_Set(NormalizerConfig* nc, NormFlags nf, bool enable)
{
    if ( enable )
        Norm_Enable(nc, nf);
    else
        Norm_Disable(nc, nf);
}

static inline int Norm_IsEnabled(const NormalizerConfig* nc, NormFlags nf)
{
    return ( (nc->normalizer_flags & nf) != 0 );
}

static inline void Norm_TcpPassOption(NormalizerConfig* nc, uint8_t opt)
{
    uint8_t byte = (opt >> 3), bit = (1 << (opt & 0x07));
    nc->normalizer_options[byte] |= bit;
}

static inline void Norm_TcpDropOption(NormalizerConfig* nc, uint8_t opt)
{
    uint8_t byte = (opt >> 3), bit = (1 << (opt & 0x07));
    nc->normalizer_options[byte] &= ~bit;
}

static inline int Norm_TcpIsOptional(const NormalizerConfig* nc, uint8_t opt)
{
    uint8_t byte = (opt >> 3), bit = (1 << (opt & 0x07));
    return ( (nc->normalizer_options[byte] & bit) != 0 );
}

const PegInfo* Norm_GetPegs();
PegCount* Norm_GetCounts(unsigned&);

#endif

