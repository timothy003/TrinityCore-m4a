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
SDName: Boss_Mother_Shahraz
SD%Complete: 80
SDComment: Saber Lash missing, Fatal Attraction slightly incorrect; need to damage only if affected players are within range of each other
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "def_black_temple.h"

//Spells
#define SPELL_BEAM_SINISTER     40859
#define SPELL_BEAM_VILE         40860
#define SPELL_BEAM_WICKED       40861
#define SPELL_BEAM_SINFUL       40827
#define SPELL_ATTRACTION        40871
#define SPELL_SILENCING_SHRIEK  40823
#define SPELL_ENRAGE            23537
#define SPELL_SABER_LASH        43267
#define SPELL_SABER_LASH_IMM    43690
#define SPELL_TELEPORT_VISUAL   40869
#define SPELL_BERSERK           45078

uint32 PrismaticAuras[]=
{
    40880,                                                  // Shadow
    40882,                                                  // Fire
    40883,                                                  // Nature
    40891,                                                  // Arcane
    40896,                                                  // Frost
    40897,                                                  // Holy
};

//Speech'n'Sounds
#define SAY_TAUNT1          "You play, you pay."
#define SOUND_TAUNT1        11501

#define SAY_TAUNT2          "I'm not impressed."
#define SOUND_TAUNT2        11502

#define SAY_TAUNT3          "Enjoying yourselves?"
#define SOUND_TAUNT3        11503

#define SAY_AGGRO           "So, business... Or pleasure?"
#define SOUND_AGGRO         11504

#define SAY_SPELL1          "You seem a little tense."
#define SOUND_SPELL1        11505

#define SAY_SPELL2          "Don't be shy."
#define SOUND_SPELL2        11506

#define SAY_SPELL3          "I'm all... yours."
#define SOUND_SPELL3        11507

#define SAY_SLAY1           "Easy come, easy go."
#define SOUND_SLAY1         11508

#define SAY_SLAY2           "So much for a happy ending."
#define SOUND_SLAY2         11509

#define SAY_ENRAGE          "Stop toying with my emotions!"
#define SOUND_ENRAGE        11510

#define SAY_DEATH           "I wasn't... finished."
#define SOUND_DEATH         11511

struct Locations
{
    float x,y,z;
};

static Locations TeleportPoint[]=
{
    {959.996, 212.576, 193.843},
    {932.537, 231.813, 193.838},
    {958.675, 254.767, 193.822},
    {946.955, 201.316, 192.535},
    {944.294, 149.676, 197.551},
    {930.548, 284.888, 193.367},
    {965.997, 278.398, 195.777}
};

struct TRINITY_DLL_DECL boss_shahrazAI : public ScriptedAI
{
    boss_shahrazAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }

    ScriptedInstance* pInstance;

    uint64 TargetGUID[3];
    uint32 BeamTimer;
    uint32 BeamCount;
    uint32 CurrentBeam;
    uint32 PrismaticShieldTimer;
    uint32 FatalAttractionTimer;
    uint32 FatalAttractionExplodeTimer;
    uint32 ShriekTimer;
    uint32 RandomYellTimer;
    uint32 EnrageTimer;
    uint32 ExplosionCount;

    bool Enraged;

    void Reset()
    {
        if(pInstance)
            pInstance->SetData(DATA_MOTHERSHAHRAZEVENT, NOT_STARTED);

        for(uint8 i = 0; i<3; i++)
            TargetGUID[i] = 0;

        BeamTimer = 60000;                                  // Timers may be incorrect
        BeamCount = 0;
        CurrentBeam = 0;                                    // 0 - Sinister, 1 - Vile, 2 - Wicked, 3 - Sinful
        PrismaticShieldTimer = 0;
        FatalAttractionTimer = 60000;
        FatalAttractionExplodeTimer = 70000;
        ShriekTimer = 30000;
        RandomYellTimer = 70000 + rand()%41 * 1000;
        EnrageTimer = 600000;
        ExplosionCount = 0;

        Enraged = false;
    }

    void Aggro(Unit *who)
    {
        if(pInstance)
            pInstance->SetData(DATA_MOTHERSHAHRAZEVENT, IN_PROGRESS);

        DoZoneInCombat();
        DoYell(SAY_AGGRO,LANG_UNIVERSAL,NULL);
        DoPlaySoundToSet(m_creature, SOUND_AGGRO);
    }

    void KilledUnit(Unit *victim)
    {
        switch(rand()%2)
        {
            case 0:
                DoYell(SAY_SLAY1,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature, SOUND_SLAY1);
                break;
            case 1:
                DoYell(SAY_SLAY2,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature, SOUND_SLAY2);
                break;
        }
    }

    void JustDied(Unit *victim)
    {
        if(pInstance)
            pInstance->SetData(DATA_MOTHERSHAHRAZEVENT, DONE);

        DoYell(SAY_DEATH, LANG_UNIVERSAL, NULL);
        DoPlaySoundToSet(m_creature,SOUND_DEATH);
    }

    void TeleportPlayers()
    {
        uint32 random = rand()%7;
        float X = TeleportPoint[random].x;
        float Y = TeleportPoint[random].y;
        float Z = TeleportPoint[random].z;
        for(uint8 i = 0; i < 3; i++)
        {
            Unit* pUnit = SelectUnit(SELECT_TARGET_RANDOM, 1);
            if(pUnit && pUnit->isAlive() && (pUnit->GetTypeId() == TYPEID_PLAYER))
            {
                TargetGUID[i] = pUnit->GetGUID();
                pUnit->CastSpell(pUnit, SPELL_TELEPORT_VISUAL, true);
                DoTeleportPlayer(pUnit, X, Y, Z, pUnit->GetOrientation());
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if(((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 10) && !Enraged)
        {
            Enraged = true;
            DoCast(m_creature, SPELL_ENRAGE, true);
            DoYell(SAY_ENRAGE, LANG_UNIVERSAL, NULL);
            DoPlaySoundToSet(m_creature, SOUND_ENRAGE);
        }

        //Randomly cast one beam.
        if(BeamTimer < diff)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
            if(!target || !target->isAlive())
                return;

            BeamTimer = 9000;

            switch(CurrentBeam)
            {
                case 0:
                    DoCast(target, SPELL_BEAM_SINISTER);
                    break;
                case 1:
                    DoCast(target, SPELL_BEAM_VILE);
                    break;
                case 2:
                    DoCast(target, SPELL_BEAM_WICKED);
                    break;
                case 3:
                    DoCast(target, SPELL_BEAM_SINFUL);
                    break;
            }
            BeamCount++;
            uint32 Beam = CurrentBeam;
            if(BeamCount > 3)
                while(CurrentBeam == Beam)
                    CurrentBeam = rand()%3;

        }else BeamTimer -= diff;

        // Random Prismatic Shield every 15 seconds.
        if(PrismaticShieldTimer < diff)
        {
            uint32 random = rand()%6;
            if(PrismaticAuras[random])
                DoCast(m_creature, PrismaticAuras[random]);
            PrismaticShieldTimer = 15000;
        }else PrismaticShieldTimer -= diff;

        // Select 3 random targets (can select same target more than once), teleport to a random location then make them cast explosions until they get away from each other.
        if(FatalAttractionTimer < diff)
        {
            ExplosionCount = 0;

            TeleportPlayers();

            switch(rand()%2)
            {
                case 0:
                    DoYell(SAY_SPELL2,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature, SOUND_SPELL2);
                    break;
                case 1:
                    DoYell(SAY_SPELL3,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature, SOUND_SPELL3);
                    break;
            }
            FatalAttractionExplodeTimer = 2000;
            FatalAttractionTimer = 40000 + rand()%31 * 1000;
        }else FatalAttractionTimer -= diff;

        if(FatalAttractionExplodeTimer < diff)
        {
            // Just make them explode three times... they're supposed to keep exploding while they are in range, but it'll take too much code. I'll try to think of an efficient way for it later.
            if(ExplosionCount < 3)
            {
                for(uint8 i = 0; i < 4; i++)
                {
                    Unit* pUnit = NULL;
                    if(TargetGUID[i])
                    {
                        pUnit = Unit::GetUnit((*m_creature), TargetGUID[i]);
                        if(pUnit)
                            pUnit->CastSpell(pUnit, SPELL_ATTRACTION, true);
                        TargetGUID[i] = 0;
                    }
                }

                ExplosionCount++;
                FatalAttractionExplodeTimer = 1000;
            }
            else
            {
                FatalAttractionExplodeTimer = FatalAttractionTimer + 2000;
                ExplosionCount = 0;
            }
        }else FatalAttractionExplodeTimer -= diff;

        if(ShriekTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_SILENCING_SHRIEK);
            ShriekTimer = 30000;
        }else ShriekTimer -= diff;

        //Enrage
        if(!m_creature->HasAura(SPELL_BERSERK, 0))
            if(EnrageTimer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            DoYell(SAY_ENRAGE,LANG_UNIVERSAL,NULL);
            DoPlaySoundToSet(m_creature, SOUND_ENRAGE);
        }else EnrageTimer -= diff;

        //Random taunts
        if(RandomYellTimer < diff)
        {
            switch(rand()%3)
            {
                case 0:
                    DoYell(SAY_TAUNT1,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature, SOUND_TAUNT1);
                    break;
                case 1:
                    DoYell(SAY_TAUNT2,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature, SOUND_TAUNT2);
                    break;
                case 2:
                    DoYell(SAY_TAUNT3,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature, SOUND_TAUNT3);
                    break;
            }
            RandomYellTimer = 60000 + rand()%91 * 1000;
        }else RandomYellTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_shahraz(Creature *_Creature)
{
    return new boss_shahrazAI (_Creature);
}

void AddSC_boss_mother_shahraz()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_mother_shahraz";
    newscript->GetAI = GetAI_boss_shahraz;
    m_scripts[nrscripts++] = newscript;
}
