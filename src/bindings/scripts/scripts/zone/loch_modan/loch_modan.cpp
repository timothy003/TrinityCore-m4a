/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Loch_Modan
SD%Complete: 100
SDComment: Quest support: 3181 (only to argue with pebblebitty to get to searing gorge, before quest rewarded)
SDCategory: Loch Modan
EndScriptData */

/* ContentData
npc_mountaineer_pebblebitty
EndContentData */

#include "precompiled.h"

/*######
## npc_mountaineer_pebblebitty
######*/

bool GossipHello_npc_mountaineer_pebblebitty(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    if (!player->GetQuestRewardStatus(3181) == 1)
        player->ADD_GOSSIP_ITEM( 0, "Open the gate please, i need to get to Searing Gorge", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_mountaineer_pebblebitty(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM( 0, "But i need to get there, now open the gate!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(1833, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM( 0, "Ok, so what is this other way?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->SEND_GOSSIP_MENU(1834, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->ADD_GOSSIP_ITEM( 0, "Doesn't matter, i'm invulnerable.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            player->SEND_GOSSIP_MENU(1835, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            player->ADD_GOSSIP_ITEM( 0, "Yes...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            player->SEND_GOSSIP_MENU(1836, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            player->ADD_GOSSIP_ITEM( 0, "Ok, i'll try to remember that.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            player->SEND_GOSSIP_MENU(1837, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            player->ADD_GOSSIP_ITEM( 0, "A key? Ok!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            player->SEND_GOSSIP_MENU(1838, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            player->CLOSE_GOSSIP_MENU();
            break;
    }
    return true;
}

void AddSC_loch_modan()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="npc_mountaineer_pebblebitty";
    newscript->pGossipHello =  &GossipHello_npc_mountaineer_pebblebitty;
    newscript->pGossipSelect = &GossipSelect_npc_mountaineer_pebblebitty;
    m_scripts[nrscripts++] = newscript;
}
