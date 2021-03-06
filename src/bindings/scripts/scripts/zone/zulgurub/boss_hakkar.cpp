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
SDName: Boss_Hakkar
SD%Complete: 95
SDComment: Blood siphon spell buggy cause of Core Issue.
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "def_zulgurub.h"

#define SPELL_BLOODSIPHON            24322
#define SPELL_CORRUPTEDBLOOD         24328
#define SPELL_CAUSEINSANITY          24327                  //Not working disabled.
#define SPELL_WILLOFHAKKAR           24178
#define SPELL_ENRAGE                 24318

// The Aspects of all High Priests
#define SPELL_ASPECT_OF_JEKLIK       24687
#define SPELL_ASPECT_OF_VENOXIS      24688
#define SPELL_ASPECT_OF_MARLI        24686
#define SPELL_ASPECT_OF_THEKAL       24689
#define SPELL_ASPECT_OF_ARLOKK       24690

#define SAY_AGGRO         "PRIDE HERALDS THE END OF YOUR WORLD. COME, MORTALS! FACE THE WRATH OF THE SOULFLAYER!"
#define SOUND_AGGRO       8414

#define SAY_SLAY          "Fleeing will do you no good, mortals!"

struct TRINITY_DLL_DECL boss_hakkarAI : public ScriptedAI
{
    boss_hakkarAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }

    uint32 BloodSiphon_Timer;
    uint32 CorruptedBlood_Timer;
    uint32 CauseInsanity_Timer;
    uint32 WillOfHakkar_Timer;
    uint32 Enrage_Timer;

    uint32 CheckJeklik_Timer;
    uint32 CheckVenoxis_Timer;
    uint32 CheckMarli_Timer;
    uint32 CheckThekal_Timer;
    uint32 CheckArlokk_Timer;

    uint32 AspectOfJeklik_Timer;
    uint32 AspectOfVenoxis_Timer;
    uint32 AspectOfMarli_Timer;
    uint32 AspectOfThekal_Timer;
    uint32 AspectOfArlokk_Timer;

    ScriptedInstance *pInstance;

    bool Enraged;

    void Reset()
    {
        BloodSiphon_Timer = 90000;
        CorruptedBlood_Timer = 25000;
        CauseInsanity_Timer = 17000;
        WillOfHakkar_Timer = 17000;
        Enrage_Timer = 600000;

        CheckJeklik_Timer = 1000;
        CheckVenoxis_Timer = 2000;
        CheckMarli_Timer = 3000;
        CheckThekal_Timer = 4000;
        CheckArlokk_Timer = 5000;

        AspectOfJeklik_Timer = 4000;
        AspectOfVenoxis_Timer = 7000;
        AspectOfMarli_Timer = 12000;
        AspectOfThekal_Timer = 8000;
        AspectOfArlokk_Timer = 18000;

        Enraged = false;
    }

    void Aggro(Unit *who)
    {
        DoYell(SAY_AGGRO,LANG_UNIVERSAL,NULL);
        DoPlaySoundToSet(m_creature,SOUND_AGGRO);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //BloodSiphon_Timer
        if (BloodSiphon_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_BLOODSIPHON);
            BloodSiphon_Timer = 90000;
        }else BloodSiphon_Timer -= diff;

        //CorruptedBlood_Timer
        if (CorruptedBlood_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_CORRUPTEDBLOOD);
            CorruptedBlood_Timer = 30000 + rand()%15000;
        }else CorruptedBlood_Timer -= diff;

        //CauseInsanity_Timer
        //        if (CauseInsanity_Timer < diff)
        //        {
        //
        //            Unit* target = NULL;
        //            target = SelectUnit(SELECT_TARGET_RANDOM,0);

        //            DoCast(target,SPELL_CAUSEINSANITY);

        //            CauseInsanity_Timer = 35000 + rand()%8000;
        //        }else CauseInsanity_Timer -= diff;

        //WillOfHakkar_Timer
        if (WillOfHakkar_Timer < diff)
        {

            Unit* target = NULL;
            target = SelectUnit(SELECT_TARGET_RANDOM,0);

            DoCast(target,SPELL_WILLOFHAKKAR);
            WillOfHakkar_Timer = 25000 + rand()%10000;
        }else WillOfHakkar_Timer -= diff;

        if (!Enraged && Enrage_Timer < diff)
        {
            DoCast(m_creature, SPELL_ENRAGE);
            Enraged = true;
        }else Enrage_Timer -= diff;

        //Checking if Jeklik is dead. If not we cast her Aspect
        if(CheckJeklik_Timer < diff)
        {
            if(pInstance)
            {
                if(!pInstance->GetData(DATA_JEKLIKISDEAD))
                {
                    if (AspectOfJeklik_Timer < diff)
                    {
                        DoCast(m_creature->getVictim(),SPELL_ASPECT_OF_JEKLIK);
                        AspectOfJeklik_Timer = 10000 + rand()%4000;
                    }else AspectOfJeklik_Timer -= diff;
                }
            }
            CheckJeklik_Timer = 1000;
        }else CheckJeklik_Timer -= diff;

        //Checking if Venoxis is dead. If not we cast his Aspect
        if(CheckVenoxis_Timer < diff)
        {
            if(pInstance)
            {
                if(!pInstance->GetData(DATA_VENOXISISDEAD))
                {
                    if (AspectOfVenoxis_Timer < diff)
                    {
                        DoCast(m_creature->getVictim(),SPELL_ASPECT_OF_VENOXIS);
                        AspectOfVenoxis_Timer = 8000;
                    }else AspectOfVenoxis_Timer -= diff;
                }
            }
            CheckVenoxis_Timer = 1000;
        }else CheckVenoxis_Timer -= diff;

        //Checking if Marli is dead. If not we cast her Aspect
        if(CheckMarli_Timer < diff)
        {
            if(pInstance)
            {
                if(!pInstance->GetData(DATA_MARLIISDEAD))
                {
                    if (AspectOfMarli_Timer < diff)
                    {
                        DoCast(m_creature->getVictim(),SPELL_ASPECT_OF_MARLI);
                        AspectOfMarli_Timer = 10000;
                    }else AspectOfMarli_Timer -= diff;

                }
            }
            CheckMarli_Timer = 1000;
        }else CheckMarli_Timer -= diff;

        //Checking if Thekal is dead. If not we cast his Aspect
        if(CheckThekal_Timer < diff)
        {
            if(pInstance)
            {
                if(!pInstance->GetData(DATA_THEKALISDEAD))
                {
                    if (AspectOfThekal_Timer < diff)
                    {
                        DoCast(m_creature,SPELL_ASPECT_OF_THEKAL);
                        AspectOfThekal_Timer = 15000;
                    }else AspectOfThekal_Timer -= diff;
                }
            }
            CheckThekal_Timer = 1000;
        }else CheckThekal_Timer -= diff;

        //Checking if Arlokk is dead. If yes we cast her Aspect
        if(CheckArlokk_Timer < diff)
        {
            if(pInstance)
            {
                if(!pInstance->GetData(DATA_ARLOKKISDEAD))
                {
                    if (AspectOfArlokk_Timer < diff)
                    {
                        DoCast(m_creature,SPELL_ASPECT_OF_ARLOKK);
                        DoResetThreat();

                        AspectOfArlokk_Timer = 10000 + rand()%5000;
                    }else AspectOfArlokk_Timer -= diff;
                }
            }
            CheckArlokk_Timer = 1000;
        }else CheckArlokk_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_hakkar(Creature *_Creature)
{
    return new boss_hakkarAI (_Creature);
}

void AddSC_boss_hakkar()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_hakkar";
    newscript->GetAI = GetAI_boss_hakkar;
    m_scripts[nrscripts++] = newscript;
}
