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
SDName: boss_astromancer
SD%Complete: 75
SDComment:
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "precompiled.h"
#include "def_the_eye.h"

#define SPELL_ARCANE_MISSILES                 33031
#define SPELL_MARK_OF_THE_ASTROMANCER         33045
#define MARK_OF_SOLARIAN                      33023
#define SPELL_BLINDING_LIGHT                  33009
#define SPELL_FEAR                            29321
#define SPELL_VOID_BOLT                       39329

#define SAY_PHASE1                           "Tal anu'men no Sin'dorei!"
#define SOUND_PHASE1                         11134
#define SAY_PHASE2                           "I will crush your delusions of grandeur!"
#define SOUND_PHASE2                         11140
#define SAY_PHASE21                          "Ha ha ha! You are hopelessly outmatched!"
#define SOUND_PHASE21                        11139

#define SAY_VOID                             "Enough of this! Now I call upon the fury of the cosmos itself."
//#define SOUND_VOID                         Not found :(
#define SAY_VOID1                            "I become ONE... with the VOID!"
//#define SOUND_VOID1                        Not found :(

#define SAY_KILL1                            "Your soul belongs to the Abyss!"
#define SOUND_KILL1                          11136
#define SAY_KILL2                            "By the blood of the Highborne!"
#define SOUND_KILL2                          11137
#define SAY_KILL3                            "For the Sunwell!"
#define SOUND_KILL3                          11138
#define SAY_DEATH                            "The warmth of the sun... awaits."
#define SOUND_DEATH                          11135

#define CENTER_X                             432.909f
#define CENTER_Y                             -373.424f
#define CENTER_Z                             17.9608f
#define CENTER_O                             1.06421f
#define SMALL_PORTAL_RADIUS                  12.6f
#define LARGE_PORTAL_RADIUS                  26.0f
#define PORTAL_Z                             17.005f

#define SOLARIUM_AGENT                       18925
#define SOLARIUM_PRIEST                      18806
#define ASTROMANCER_SOLARIAN_SPOTLIGHT       18928
#define SPELL_SPOTLIGHT                      25824
#define MODEL_HUMAN                          18239
#define MODEL_VOIDWALKER                     18988

#define SOLARIUM_HEAL                        41378
#define SOLARIUM_SMITE                       31740
#define SOLARIUM_SILENCE                     37160

#define WV_ARMOR                    31000
#define MIN_RANGE_FOR_DOT_JUMP      20.0f

struct TRINITY_DLL_DECL boss_high_astromancer_solarianAI : public ScriptedAI
{
    boss_high_astromancer_solarianAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());

        defaultarmor=m_creature->GetArmor();
        defaultsize=m_creature->GetFloatValue(OBJECT_FIELD_SCALE_X);
        Reset();
    }

    ScriptedInstance *pInstance;

    uint32 ArcaneMissiles_Timer;
    uint32 MarkOfTheAstromancer_Timer;
    uint32 BlindingLight_Timer;
    uint32 Fear_Timer;
    uint32 VoidBolt_Timer;
    uint32 Phase1_Timer;
    uint32 Phase2_Timer;
    uint32 Phase3_Timer;
    uint32 AppearDelay_Timer;
    uint32 MarkOfTheSolarian_Timer;
    uint32 Jump_Timer;
    uint32 defaultarmor;

    float defaultsize;

    bool AppearDelay;

    uint8 Phase;

    uint64 WrathTarget;

    float Portals[3][3];

    void Reset()
    {
        WrathTarget=0;
        ArcaneMissiles_Timer = 2000;
        MarkOfTheAstromancer_Timer = 15000;
        BlindingLight_Timer = 41000;
        Fear_Timer = 20000;
        VoidBolt_Timer = 10000;
        Phase1_Timer = 50000;
        Phase2_Timer = 10000;
        Phase3_Timer = 15000;
        AppearDelay_Timer = 2000;
        AppearDelay = false;
        MarkOfTheSolarian_Timer=45000;
        Jump_Timer=8000;
        Phase = 1;

        if(pInstance)
            pInstance->SetData(DATA_HIGHASTROMANCERSOLARIANEVENT, NOT_STARTED);

        m_creature->SetArmor(defaultarmor);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetVisibility(VISIBILITY_ON);
        m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, defaultsize);
        m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, MODEL_HUMAN);
    }

    void StartEvent()
    {
        DoYell(SAY_PHASE1, LANG_UNIVERSAL, NULL);
        DoPlaySoundToSet(m_creature, SOUND_PHASE1);

        if(pInstance)
            pInstance->SetData(DATA_HIGHASTROMANCERSOLARIANEVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit *victim)
    {
        switch(rand()%3)
        {
            case 0:
                DoYell(SAY_KILL1, LANG_UNIVERSAL, NULL);
                DoPlaySoundToSet(m_creature, SOUND_KILL1);
                break;
            case 1:
                DoYell(SAY_KILL2, LANG_UNIVERSAL, NULL);
                DoPlaySoundToSet(m_creature, SOUND_KILL2);
                break;
            case 2:
                DoYell(SAY_KILL3, LANG_UNIVERSAL, NULL);
                DoPlaySoundToSet(m_creature, SOUND_KILL3);
                break;
        }
    }

    void JustDied(Unit *victim)
    {
        m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, defaultsize);
        m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, MODEL_HUMAN);
        DoYell(SAY_DEATH, LANG_UNIVERSAL, NULL);
        DoPlaySoundToSet(m_creature, SOUND_DEATH);

        if(pInstance)
            pInstance->SetData(DATA_HIGHASTROMANCERSOLARIANEVENT, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        StartEvent();
    }

    void SummonMinion(uint32 entry, float x, float y, float z)
    {
        Creature* Summoned = m_creature->SummonCreature(entry, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
        if(Summoned)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
            if(target)
                Summoned->AI()->AttackStart(target);
        }
    }

    float Portal_X(float radius)
    {
        if ((rand()%2)==1) radius = -radius;
        return (radius * (float)(rand()%100)/100.0f + CENTER_X);
    }

    float Portal_Y(float x, float radius)
    {
        float z;

        switch(rand()%2)
        {
            case 0: z = 1; break;
            case 1: z = -1; break;
        }
        return (z*sqrt(radius*radius - (x - CENTER_X)*(x - CENTER_X)) + CENTER_Y);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;

        if (AppearDelay)
        {
            m_creature->StopMoving();
            m_creature->AttackStop();
            if (AppearDelay_Timer < diff)
            {
                AppearDelay = false;
                if (Phase == 2)
                {
                    switch (rand()%2)
                    {
                        case 0:
                            DoYell(SAY_PHASE2, LANG_UNIVERSAL, NULL);
                            DoPlaySoundToSet(m_creature, SOUND_PHASE2);
                            break;
                        case 1:
                            DoYell(SAY_PHASE21, LANG_UNIVERSAL, NULL);
                            DoPlaySoundToSet(m_creature, SOUND_PHASE21);
                            break;
                    }
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->SetVisibility(VISIBILITY_OFF);
                }
                if (Phase == 3)
                    Phase = 1;

                AppearDelay_Timer = 2000;
            }else AppearDelay_Timer -= diff;
        }

        //the jumping of the dot part of the wrath of the astromancer
        if(WrathTarget)
        {
            if(Jump_Timer< diff)
            {
                std::list<HostilReference*>& m_threatlist = m_creature->getThreatManager().getThreatList();
                bool hasJumped = false;

                Unit* target = Unit::GetUnit((*m_creature),WrathTarget);
                if(target && target->isAlive())
                {
                    for(std::list<HostilReference*>::iterator iter = m_threatlist.begin();iter!=m_threatlist.end();++iter)
                    {
                        Unit* currentUnit=Unit::GetUnit((*m_creature),(*iter)->getUnitGuid());
                        if(currentUnit)
                        {
                            if(currentUnit->IsWithinDistInMap(target,MIN_RANGE_FOR_DOT_JUMP)&&currentUnit->isAlive()&&currentUnit!=target)
                            {
                                m_creature->CastSpell(currentUnit,SPELL_MARK_OF_THE_ASTROMANCER, false,0,0,m_creature->GetGUID());

                                Jump_Timer=8000;
                                WrathTarget=currentUnit->GetGUID();
                                hasJumped = true;
                                break;
                            }
                        }
                    }
                }

                if(!hasJumped)
                    WrathTarget = 0;
            }else Jump_Timer -= diff;
        }

        if (Phase == 1)
        {
            //ArcaneMissiles_Timer
            if (ArcaneMissiles_Timer < diff)
            {
                //Solarian casts Arcane Missiles on on random targets in the raid.
                Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (!m_creature->HasInArc(2.5f, target))
                    target = m_creature->getVictim();
                if (target)
                    DoCast(target, SPELL_ARCANE_MISSILES);

                ArcaneMissiles_Timer = 3000;
            }else ArcaneMissiles_Timer -= diff;

            //MarkOfTheSolarian_Timer
            if (MarkOfTheSolarian_Timer < diff)
            {
                DoCast(m_creature->getVictim(), MARK_OF_SOLARIAN);
                MarkOfTheSolarian_Timer = 45000;
            }else MarkOfTheSolarian_Timer -= diff;

            //MarkOfTheAstromancer_Timer
            if (MarkOfTheAstromancer_Timer < diff)
            {
                //A debuff that lasts for 5 seconds, cast several times each phase on a random raid member, but not the main tank
                Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 1);

                if(target)
                {
                    DoCast(target, SPELL_MARK_OF_THE_ASTROMANCER);

                    WrathTarget=target->GetGUID();
                    Jump_Timer=8000;
                }

                MarkOfTheAstromancer_Timer = 15000;
            }else MarkOfTheAstromancer_Timer -= diff;

            //BlindingLight_Timer
            if (BlindingLight_Timer < diff)
            {
                //She casts this spell every 45 seconds. It is a kind of Moonfire spell, which she strikes down on the whole raid simultaneously. It hits everyone in the raid for 2280 to 2520 arcane damage.
                DoCast(m_creature->getVictim(), SPELL_BLINDING_LIGHT);

                BlindingLight_Timer = 45000;
            }else BlindingLight_Timer -= diff;

            //Phase1_Timer
            if (Phase1_Timer < diff)
            {
                Phase = 2;
                Phase1_Timer = 50000;
                //After these 50 seconds she portals to the middle of the room and disappears, leaving 3 light portals behind.
                m_creature->GetMotionMaster()->Clear();
                m_creature->Relocate(CENTER_X, CENTER_Y, CENTER_Z, CENTER_O);
                for(int i=0; i<=2; i++)
                {
                    if (!i)
                    {
                        Portals[i][0] = Portal_X(SMALL_PORTAL_RADIUS);
                        Portals[i][1] = Portal_Y(Portals[i][0], SMALL_PORTAL_RADIUS);
                        Portals[i][2] = CENTER_Z;
                    }else
                    {
                        Portals[i][0] = Portal_X(LARGE_PORTAL_RADIUS);
                        Portals[i][1] = Portal_Y(Portals[i][0], LARGE_PORTAL_RADIUS);
                        Portals[i][2] = PORTAL_Z;
                    }
                }
                if((abs(Portals[2][0] - Portals[1][0]) < 7)
                    && (abs(Portals[2][1] - Portals[1][1]) < 7))
                {
                    int i=1;
                    if(abs(CENTER_X + 26.0f - Portals[2][0]) < 7)
                        i = -1;
                    Portals[2][0] = Portals[2][0]+7*i;
                    Portals[2][1] = Portal_Y(Portals[2][0], LARGE_PORTAL_RADIUS);
                }
                for (int i=0; i<=2; i++)
                {
                    Creature* Summoned = m_creature->SummonCreature(ASTROMANCER_SOLARIAN_SPOTLIGHT, Portals[i][0], Portals[i][1], Portals[i][2], CENTER_O, TEMPSUMMON_TIMED_DESPAWN, Phase2_Timer+Phase3_Timer+AppearDelay_Timer+1700);
                    if(Summoned)
                    {
                        Summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Summoned->CastSpell(Summoned, SPELL_SPOTLIGHT, false);
                    }
                }
                AppearDelay = true;
            }else Phase1_Timer-=diff;
        }
        else if(Phase == 2)
        {
            //10 seconds after Solarian disappears, 12 mobs spawn out of the three portals.
            m_creature->AttackStop();
            m_creature->StopMoving();
            if (Phase2_Timer < diff)
            {
                Phase = 3;
                for (int i=0; i<=2; i++)
                    for (int j=1; j<=4; j++)
                        SummonMinion(SOLARIUM_AGENT, Portals[i][0], Portals[i][1], Portals[i][2]);
                Phase2_Timer = 10000;
            } else Phase2_Timer -= diff;
        }
        else if(Phase == 3)
        {
            //Check Phase3_Timer
            if(Phase3_Timer < diff)
            {
                //15 seconds later Solarian reappears out of one of the 3 portals. Simultaneously, 2 healers appear in the two other portals.
                m_creature->AttackStop();
                m_creature->StopMoving();
                int i = rand()%3;
                m_creature->GetMotionMaster()->Clear();
                m_creature->Relocate(Portals[i][0], Portals[i][1], Portals[i][2], CENTER_O);

                for (int j=0; j<=2; j++)
                    if (j!=i)
                        SummonMinion(SOLARIUM_PRIEST, Portals[j][0], Portals[j][1], Portals[j][2]);

                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetVisibility(VISIBILITY_ON);

                DoYell(SAY_PHASE1, LANG_UNIVERSAL, NULL);
                DoPlaySoundToSet(m_creature, SOUND_PHASE1);
                AppearDelay = true;
                Phase3_Timer = 15000;
            }else Phase3_Timer -= diff;
        }
        else if(Phase == 4)
        {
            //Fear_Timer
            if (Fear_Timer < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_FEAR);
                Fear_Timer = 20000;
            }else Fear_Timer -= diff;

            //VoidBolt_Timer
            if (VoidBolt_Timer < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_VOID_BOLT);
                VoidBolt_Timer = 10000;
            }else VoidBolt_Timer -= diff;
        }

        //When Solarian reaches 20% she will transform into a huge void walker.
        if(Phase != 4 && ((m_creature->GetHealth()*100 / m_creature->GetMaxHealth())<20))
        {
            Phase = 4;

            //To make sure she wont be invisible or not selecatble
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetVisibility(VISIBILITY_ON);
            switch(rand()%2)
            {
                case 0:
                    DoYell(SAY_VOID, LANG_UNIVERSAL, NULL);
                    break;
                case 1:
                    DoYell(SAY_VOID1, LANG_UNIVERSAL, NULL);
                    break;
            }

            m_creature->SetArmor(WV_ARMOR);
            m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, MODEL_VOIDWALKER);
            m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, defaultsize*2.5f);
        }
        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL mob_solarium_priestAI : public ScriptedAI
{
    mob_solarium_priestAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }

    ScriptedInstance *pInstance;

    uint32 healTimer;
    uint32 holysmiteTimer;
    uint32 aoesilenceTimer;

    void Reset()
    {
        healTimer = 9000;
        holysmiteTimer = 1;
        aoesilenceTimer = 15000;
    }

    void Aggro(Unit *who)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (healTimer < diff)
        {
            Unit* target = NULL;

            switch(rand()%2)
            {
                case 0:
                    if(pInstance)
                        target = Unit::GetUnit((*m_creature), pInstance->GetData64(DATA_ASTROMANCER));
                    break;
                case 1:
                    target = m_creature;
                    break;
            }

            if(target)
            {
                DoCast(target,SOLARIUM_HEAL);
                healTimer = 9000;
            }
        } else healTimer -= diff;

        if(holysmiteTimer < diff)
        {
            DoCast(m_creature->getVictim(), SOLARIUM_SMITE);
            holysmiteTimer = 4000;
        } else holysmiteTimer -= diff;

        if (aoesilenceTimer < diff)
        {
            DoCast(m_creature->getVictim(), SOLARIUM_SILENCE);
            aoesilenceTimer = 13000;
        } else aoesilenceTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_solarium_priest(Creature *_Creature)
{
    return new mob_solarium_priestAI (_Creature);
}

CreatureAI* GetAI_boss_high_astromancer_solarian(Creature *_Creature)
{
    return new boss_high_astromancer_solarianAI (_Creature);
}

void AddSC_boss_high_astromancer_solarian()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_high_astromancer_solarian";
    newscript->GetAI = GetAI_boss_high_astromancer_solarian;
    m_scripts[nrscripts++] = newscript;

    newscript = new Script;
    newscript->Name="mob_solarium_priest";
    newscript->GetAI = GetAI_mob_solarium_priest;
    m_scripts[nrscripts++] = newscript;
}
