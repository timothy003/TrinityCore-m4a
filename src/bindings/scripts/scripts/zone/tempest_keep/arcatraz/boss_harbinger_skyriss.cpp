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
SDName: Boss_Harbinger_Skyriss
SD%Complete: 45
SDComment: CombatAI not fully implemented. Timers will need adjustments. Need better method to "kill" the warden. Need more docs on how event fully work. Reset all event and force start over if fail at one point?
SDCategory: Tempest Keep, The Arcatraz
EndScriptData */

/* ContentData
boss_harbinger_skyriss
boss_harbinger_skyriss_illusion
EndContentData */

#include "precompiled.h"
#include "def_arcatraz.h"

#define SAY_INTRO       "It is a small matter to control the mind of the weak... for I bear allegiance to powers untouched by time, unmoved by fate. No force on this world or beyond harbors the strength to bend our knee... not even the mighty Legion!"
#define SOUND_INTRO     11122

#define SAY_AGGRO       "Bear witness to the agent of your demise!"
#define SOUND_AGGRO     11123

#define SAY_KILL_1      "Your fate is written!"
#define SOUND_KILL_1    11124
#define SAY_KILL_2      "The chaos I have sown here is but a taste...."
#define SOUND_KILL_2    11125

#define SAY_MIND_1      "You will do my bidding, weakling."
#define SOUND_MIND_1    11127
#define SAY_MIND_2      "Your will is no longer your own."
#define SOUND_MIND_2    11128

#define SAY_FEAR_1      "Flee in terror!"
#define SOUND_FEAR_1    11129
#define SAY_FEAR_2      "I will show you horrors undreamed of!"
#define SOUND_FEAR_2    11130

#define SAY_IMAGE       "We span the universe, as countless as the stars!"
#define SOUND_IMAGE     11131

#define SAY_DEATH       "I am merely one of... infinite multitudes."
#define SOUND_DEATH     11126

#define SPELL_FEAR              39415

#define SPELL_MIND_REND         36924
#define H_SPELL_MIND_REND       39017

#define SPELL_DOMINATION        37162
#define H_SPELL_DOMINATION      39019

#define H_SPELL_MANA_BURN       39020

#define SPELL_66_ILLUSION       36931                       //entry 21466
#define SPELL_33_ILLUSION       36932                       //entry 21467

struct TRINITY_DLL_DECL boss_harbinger_skyrissAI : public ScriptedAI
{
    boss_harbinger_skyrissAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        HeroicMode = m_creature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance *pInstance;
    bool HeroicMode;

    bool Intro;
    bool IsImage33;
    bool IsImage66;

    uint32 Intro_Phase;
    uint32 Intro_Timer;
    uint32 MindRend_Timer;
    uint32 Fear_Timer;
    uint32 Domination_Timer;
    uint32 ManaBurn_Timer;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_UNKNOWN2);

        if( Intro )
            Intro = true;
        else
            Intro = false;

        IsImage33 = false;
        IsImage66 = false;

        Intro_Phase = 1;
        Intro_Timer = 5000;
        MindRend_Timer = 3000;
        Fear_Timer = 15000;
        Domination_Timer = 30000;
        ManaBurn_Timer = 25000;
    }

    void MoveInLineOfSight(Unit *who)
    {
        if( !Intro )
            return;

        if( !m_creature->getVictim() && who->isTargetableForAttack() && ( m_creature->IsHostileTo( who )) && who->isInAccessablePlaceFor(m_creature) )
        {
            if (!m_creature->canFly() && m_creature->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                return;

            float attackRadius = m_creature->GetAttackDistance(who);
            if( m_creature->IsWithinDistInMap(who, attackRadius) && m_creature->IsWithinLOSInMap(who) )
            {
                DoStartAttackAndMovement(who);
                who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                if (!InCombat)
                {
                    InCombat = true;
                    Aggro(who);
                }
            }
        }
    }

    void AttackStart(Unit* who)
    {
        if( !Intro )
            return;

        if( who->isTargetableForAttack() )
        {
            DoStartAttackAndMovement(who);

            if( !InCombat )
            {
                InCombat = true;
                Aggro(who);
            }
        }
    }

    void Aggro(Unit *who)
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_UNKNOWN2);
    }

    void JustDied(Unit* Killer)
    {
        DoYell(SAY_DEATH, LANG_UNIVERSAL, NULL);
        DoPlaySoundToSet(m_creature,SOUND_DEATH);
        if( pInstance )
            pInstance->SetData(TYPE_HARBINGERSKYRISS,DONE);
    }

    void KilledUnit(Unit* victim)
    {
        /*if( victim->GetEntry() == 21436 )
            return;*/

        switch(rand()%2)
        {
            case 0:
                DoYell(SAY_KILL_1, LANG_UNIVERSAL, NULL);
                DoPlaySoundToSet(m_creature,SOUND_KILL_1);
                break;
            case 1:
                DoYell(SAY_KILL_2, LANG_UNIVERSAL, NULL);
                DoPlaySoundToSet(m_creature,SOUND_KILL_2);
                break;
        }
    }

    void JustSummoned(Creature *summoned)
    {
        summoned->AI()->AttackStart(m_creature->getVictim());
    }

    void DoSplit(uint32 val)
    {
        if( m_creature->IsNonMeleeSpellCasted(false) )
            m_creature->InterruptNonMeleeSpells(false);

        DoYell(SAY_IMAGE, LANG_UNIVERSAL, NULL);
        DoPlaySoundToSet(m_creature,SOUND_IMAGE);

        if( val == 66 )
            DoCast(m_creature, SPELL_66_ILLUSION);
        else
            DoCast(m_creature, SPELL_33_ILLUSION);
    }

    void UpdateAI(const uint32 diff)
    {
        if( !Intro && !InCombat )
        {
            if( !pInstance )
                return;

            if( Intro_Timer < diff )
            {
                switch( Intro_Phase )
                {
                    case 1:
                        DoYell(SAY_INTRO, LANG_UNIVERSAL, NULL);
                        DoPlaySoundToSet(m_creature,SOUND_INTRO);
                        ++Intro_Phase;
                        Intro_Timer = 25000;
                        break;
                    case 2:
                        DoYell(SAY_AGGRO, LANG_UNIVERSAL, NULL);
                        DoPlaySoundToSet(m_creature,SOUND_AGGRO);
                        if( Unit *mellic = Unit::GetUnit(*m_creature,pInstance->GetData64(DATA_MELLICHAR)) )
                        {
                            //should have a better way to do this. possibly spell exist.
                            mellic->setDeathState(JUST_DIED);
                            mellic->SetHealth(0);
                        }
                        ++Intro_Phase;
                        Intro_Timer = 3000;
                        break;
                    case 3:
                        Intro = true;
                        break;
                }
            }else Intro_Timer -=diff;
        }

        if( !m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;

        if( !IsImage66 && ((m_creature->GetHealth()*100) / m_creature->GetMaxHealth() <= 66) )
        {
            DoSplit(66);
            IsImage66 = true;
        }
        if( !IsImage33 && ((m_creature->GetHealth()*100) / m_creature->GetMaxHealth() <= 33) )
        {
            DoSplit(33);
            IsImage33 = true;
        }

        if( MindRend_Timer < diff )
        {
            if( Unit* target = SelectUnit(SELECT_TARGET_RANDOM,1) )
                DoCast(target,HeroicMode ? H_SPELL_MIND_REND : SPELL_MIND_REND);
            else
                DoCast(m_creature->getVictim(),HeroicMode ? H_SPELL_MIND_REND : SPELL_MIND_REND);

            MindRend_Timer = 8000;
        }else MindRend_Timer -=diff;

        if( Fear_Timer < diff )
        {
            if( m_creature->IsNonMeleeSpellCasted(false) )
                return;

            switch(rand()%2)
            {
                case 0:
                    DoYell(SAY_FEAR_1, LANG_UNIVERSAL, NULL);
                    DoPlaySoundToSet(m_creature,SOUND_FEAR_1);
                    break;
                case 1:
                    DoYell(SAY_FEAR_2, LANG_UNIVERSAL, NULL);
                    DoPlaySoundToSet(m_creature,SOUND_FEAR_2);
                    break;
            }

            if( Unit* target = SelectUnit(SELECT_TARGET_RANDOM,1) )
                DoCast(target,SPELL_FEAR);
            else
                DoCast(m_creature->getVictim(),SPELL_FEAR);

            Fear_Timer = 25000;
        }else Fear_Timer -=diff;

        if( Domination_Timer < diff )
        {
            if( m_creature->IsNonMeleeSpellCasted(false) )
                return;

            switch(rand()%2)
            {
                case 0:
                    DoYell(SAY_MIND_1, LANG_UNIVERSAL, NULL);
                    DoPlaySoundToSet(m_creature,SOUND_MIND_1);
                    break;
                case 1:
                    DoYell(SAY_MIND_2, LANG_UNIVERSAL, NULL);
                    DoPlaySoundToSet(m_creature,SOUND_MIND_2);
                    break;
            }

            if( Unit* target = SelectUnit(SELECT_TARGET_RANDOM,1) )
                DoCast(target,HeroicMode ? H_SPELL_DOMINATION : SPELL_DOMINATION);
            else
                DoCast(m_creature->getVictim(),HeroicMode ? H_SPELL_DOMINATION : SPELL_DOMINATION);

            Domination_Timer = 16000+rand()%16000;
        }else Domination_Timer -=diff;

        if( HeroicMode )
        {
            if( ManaBurn_Timer < diff )
            {
                if( m_creature->IsNonMeleeSpellCasted(false) )
                    return;

                if( Unit* target = SelectUnit(SELECT_TARGET_RANDOM,1) )
                    DoCast(target,H_SPELL_MANA_BURN);

                ManaBurn_Timer = 16000+rand()%16000;
            }else ManaBurn_Timer -=diff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_harbinger_skyriss(Creature *_Creature)
{
    return new boss_harbinger_skyrissAI (_Creature);
}

#define SPELL_MIND_REND_IMAGE   36929
#define H_SPELL_MIND_REND_IMAGE 39021

struct TRINITY_DLL_DECL boss_harbinger_skyriss_illusionAI : public ScriptedAI
{
    boss_harbinger_skyriss_illusionAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        HeroicMode = m_creature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance *pInstance;
    bool HeroicMode;

    void Reset() { }

    void Aggro(Unit *who) { }
};

CreatureAI* GetAI_boss_harbinger_skyriss_illusion(Creature *_Creature)
{
    return new boss_harbinger_skyriss_illusionAI (_Creature);
}

void AddSC_boss_harbinger_skyriss()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_harbinger_skyriss";
    newscript->GetAI = GetAI_boss_harbinger_skyriss;
    m_scripts[nrscripts++] = newscript;

    newscript = new Script;
    newscript->Name="boss_harbinger_skyriss_illusion";
    newscript->GetAI = GetAI_boss_harbinger_skyriss_illusion;
    m_scripts[nrscripts++] = newscript;
}
