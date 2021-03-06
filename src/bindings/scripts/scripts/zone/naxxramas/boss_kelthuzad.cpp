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
SDName: Boss_KelThuzud
SD%Complete: 0
SDComment: VERIFY SCRIPT
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"

//***THIS SCRIPTS IS UNDER DEVELOPMENT***
/*
DATA.
This script has been made with info taken from wowwikki... so there are things wrong...
like spell timers and Says. Also there's another major problem as there is no aggroed list
I cannot make Kel'thuzad to target specific party members, that is needed for spells like
Mana Detonation... so what I'm doing untill now is just to cast everything on my main aggroed
target. Sorry for him.
Another bug is that there are spells that are actually NOT working... I have to implement
them first.
Need DISPELL efect
I also don't know the emotes
*/

//Kel'thuzad. Still don't know where to use these...
//Dialog
//8878 dialog01 - Our preparations continue as planned, master.
//8881 lich_naxx_dialog01 - It is good that you serve me so faithfully. Soon, all will serve the Lich King and in the end, you shall be rewarded...so long as you do not falter.
//8879 dialog02 - I see no complications... Wait... What is this?
//8882 lich_naxx_dialog03 - Your security measures have failed! See to this interruption immediately!
//8880 dialog03 - Yes, master!
//Bigglesworth death
//No!!! A curse upon you, interlopers! The armies of the Lich King will hunt you down. You will not escape your fate...
//slay
//8818

//common needed defines
#define NAXXRAMAS_MAP                533
//Positional defines
#define ADDX_LEFT_FAR               3783.272705
#define ADDY_LEFT_FAR               -5062.697266
#define ADDZ_LEFT_FAR               143.711203
#define ADDO_LEFT_FAR               3.617599

#define ADDX_LEFT_MIDDLE            3730.291260
#define ADDY_LEFT_MIDDLE            -5027.239258
#define ADDZ_LEFT_MIDDLE            143.956909
#define ADDO_LEFT_MIDDLE            4.461900

#define ADDX_LEFT_NEAR              3683.868652
#define ADDY_LEFT_NEAR              -5057.281250
#define ADDZ_LEFT_NEAR              143.183884
#define ADDO_LEFT_NEAR              5.237086

#define ADDX_RIGHT_FAR              3759.355225
#define ADDY_RIGHT_FAR              -5174.128418
#define ADDZ_RIGHT_FAR              143.802383
#define ADDO_RIGHT_FAR              2.170104

#define ADDX_RIGHT_MIDDLE           370.724365
#define ADDY_RIGHT_MIDDLE           -5185.123047
#define ADDZ_RIGHT_MIDDLE           143.928024
#define ADDO_RIGHT_MIDDLE           1.309310

#define ADDX_RIGHT_NEAR             3665.121094
#define ADDY_RIGHT_NEAR             -5138.679199
#define ADDZ_RIGHT_NEAR             143.183212
#define ADDO_RIGHT_NEAR             0.604023

#define WALKX_LEFT_FAR              3754.431396
#define WALKY_LEFT_FAR              -5080.727734
#define WALKZ_LEFT_FAR              142.036316
#define WALKO_LEFT_FAR              3.736189

#define WALKX_LEFT_MIDDLE           3724.396484
#define WALKY_LEFT_MIDDLE           -5061.330566
#define WALKZ_LEFT_MIDDLE           142.032700
#define WALKO_LEFT_MIDDLE           4.564785

#define WALKX_LEFT_NEAR             3687.158424
#define WALKY_LEFT_NEAR             -5076.834473
#define WALKZ_LEFT_NEAR             142.017319
#define WALKO_LEFT_NEAR             5.237086

#define WALKX_RIGHT_FAR             3687.571777
#define WALKY_RIGHT_FAR             -5126.831055
#define WALKZ_RIGHT_FAR             142.017807
#define WALKO_RIGHT_FAR             0.604023

#define WALKX_RIGHT_MIDDLE          3707.990733
#define WALKY_RIGHT_MIDDLE          -5151.450195
#define WALKZ_RIGHT_MIDDLE          142.032562
#define WALKO_RIGHT_MIDDLE          1.376855

#define WALKX_RIGHT_NEAR            3739.500000
#define WALKY_RIGHT_NEAR            -5141.883989
#define WALKZ_RIGHT_NEAR            142.0141130
#define WALKO_RIGHT_NEAR            2.121412

//spells to be casted
#define SPELL_FROST_BOLT            28478
#define SPELL_FROST_BOLT_NOVA       28479
#define SPELL_CHAINS_OF_KELTHUZAD   28410
#define SPELL_MANA_DETONATION       27819
#define SPELL_SHADOW_FISURE         27810
#define SPELL_FROST_BLAST           27808

//On Aggro
#define SAY_ARRIVAL1                "PRAY FOR MERCY!"
#define SOUND_ARRIVAL1              8809
#define SAY_ARRIVAL3                "SCREAM YOR DYING BREATH!"
#define SOUND_ARRIVAL3              8810
#define SAY_ARRIVAL5                "THE END IS UPON YOU!"
#define SOUND_ARRIVAL5              8811

//On Summon
#define SAY_REINFORCEMENTS1         "MINIONS, SERVANTS, SOLDIERS OF THE COLD DARK, OBEY THE CALL OF KEL'THUZAD!"
#define SOUND_REINFORCEMENTS1       8819
#define SAY_REINFORCEMENTS2         "MASTER, I REQUIRE AID!"
#define SOUND_REINFORCEMENTS2       8816
#define SAY_LICH_NAXX_SUMMON        "VERY WELL. WARRIORS OF THE FROZEN WASTES RISE UP!. I COMMAND YOU TO FIGHT, KILL AND DIE AND DIE FOR YOUR MASTER! LET NONE SURVIVE!"
#define SOUND_LICH_NAXX_SUMMON      8824

//Random 1/4 taunt said when player enters 300 yards
#define SAY_TAUNT01                 "WHO DARES VIOLATE THE SACTITY OF MY DOMAIN? BE WARNED, ALL WHO TRASPASS HERE ARE DOOMED"
#define SOUND_TAUNT01               8820
#define SAY_TAUNT02                 "FOOLS, YOU THINK YOURSELVES TRIUMPHANT? YOU HAVE ONLY TAKEN ONE STEP CLOSER TO THE ABYSS!"
#define SOUND_TAUNT02               8821
#define SAY_TAUNT03                 "I GROW TIRED OF THESE GAMES. PROCEED, AND I WILL BANISH YOUR SOULS TO OBLIVION!"
#define SOUND_TAUNT03               8822
#define SAY_TAUNT04                 "YOU HAVE NO IDEA WHAT HORRORS LIE AHEAD. YOU HAVE SEEN NOTHING! THE FROZEN HEART OF NAXXRAMAS AWAITS YOU!"
#define SOUND_TAUNT04               8823

//On kill unit
#define SAY_SLAY                    "THE DARK VOID AWAITS YOU!"
#define SOUND_SLAY                  8817

//Specials
#define SAY_FROST                   "I WILL FREEZE THE BLOOD IN YOUR VEINS!"
#define SOUND_FROST                 8815
#define SAY_CHAIN1                  "YOUR SOUL IS BOUND TO ME NOW!"
#define SOUND_CHAIN1                8812
#define SAY_CHAIN2                  "THERE WILL BE NO ESCAPE!"
#define SOUND_CHAIN2                8813
#define SAY_SPECIAL1                "YOUR PETTY MAGICS ARE NO CHALLENGE TO THE MIGTH OF THE SCOURGE"
#define SOUND_SPECIAL1              9088
#define SAY_SPECIAL2                "ENOUGH! I GROW TIRED OF THESE DISTRACTIONS!"
#define SOUND_SPECIAL2              9090
#define SAY_DISPEL                  "FOOLS, YOU HAVE SPREAD YOUR POWERS TOO THIN. BE FREE, MY MINIONS!"
#define SOUND_DISPEL                9089

//On death
#define SAY_DEATH                   "DO NOT REJOICE... YOUR VICTORY IS A HOLLOW ONE... FOR I SHALL RETURN WITH POWERS BEYOND YOUR IMAGINING!"
#define SOUND_DEATH                 8814

struct TRINITY_DLL_DECL boss_kelthuzadAI : public ScriptedAI
{
    boss_kelthuzadAI(Creature* c) : ScriptedAI(c)
    {
        GuardiansOfIcecrown[0] = 0;
        GuardiansOfIcecrown[1] = 0;
        GuardiansOfIcecrown[2] = 0;
        GuardiansOfIcecrown[3] = 0;
        GuardiansOfIcecrown[4] = 0;
        GuardiansOfIcecrown_Count = 0;
        Reset();
    }

    uint64 GuardiansOfIcecrown[5];
    uint32 GuardiansOfIcecrown_Count;
    uint32 GuardiansOfIcecrown_Timer;
    uint32 FrostBolt_Timer;
    uint32 FrostBoltNova_Timer;
    uint32 ChainsOfKelthuzad_Timer;
    uint32 ManaDetonation_Timer;
    uint32 ShadowFisure_Timer;
    uint32 FrostBlast_Timer;
    uint32 ChainsOfKelthuzad_Targets;
    uint32 Phase1_Timer;
    bool Phase2;
    bool Phase3;

    void Reset()
    {
        FrostBolt_Timer = (rand()%60)*1000;                 //It won't be more than a minute without cast it
        FrostBoltNova_Timer = 15000;                        //Cast every 15 seconds
        ChainsOfKelthuzad_Timer = (rand()%30+30)*1000;      //Cast no sooner than once every 30 seconds
        ManaDetonation_Timer = 20000;                       //Seems to cast about every 20 seconds
        ShadowFisure_Timer = 25000;                         //25 seconds
        FrostBlast_Timer = (rand()%30+30)*1000;             //Random time between 30-60 seconds
        GuardiansOfIcecrown_Timer = 5000;                   //5 seconds for summoning each Guardian of Icecrown in phase 3

        for(int i=0; i<5; i++)
            if(GuardiansOfIcecrown[i])
        {
            //delete creature
            Unit* pUnit = Unit::GetUnit((*m_creature), GuardiansOfIcecrown[i]);
            if (pUnit && pUnit->isAlive())
                pUnit->DealDamage(pUnit, pUnit->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            GuardiansOfIcecrown[i] = 0;
        }

        Phase1_Timer = 310000;                              //Phase 1 lasts 5 minutes and 10 seconds
        Phase2 = false;
        Phase3 = false;
    }

    void KilledUnit()
    {
        if (rand()%5)
            return;

        DoYell(SAY_SLAY, LANG_UNIVERSAL, NULL);
        DoPlaySoundToSet(m_creature, SOUND_SLAY);
    }

    void JustDied(Unit* Killer)
    {
        DoYell(SAY_DEATH,LANG_UNIVERSAL,NULL);
        DoPlaySoundToSet(m_creature,SOUND_DEATH);
        for(int i=0; i<5; i++)
            if(GuardiansOfIcecrown[i])
        {
            Unit* pUnit = Unit::GetUnit((*m_creature), GuardiansOfIcecrown[i]);
            if (!pUnit || !pUnit->isAlive())
                continue;

            pUnit->CombatStop();
            float Walk_Pos_X;
            float Walk_Pos_Y;
            float Walk_Pos_Z;
            switch(rand()%6)
            {
                case 0:
                    Walk_Pos_X = ADDX_LEFT_FAR;
                    Walk_Pos_Y = ADDY_LEFT_FAR;
                    Walk_Pos_Z = ADDZ_LEFT_FAR;
                    break;
                case 1:
                    Walk_Pos_X = ADDX_LEFT_MIDDLE;
                    Walk_Pos_Y = ADDY_LEFT_MIDDLE;
                    Walk_Pos_Z = ADDZ_LEFT_MIDDLE;
                    break;
                case 2:
                    Walk_Pos_X = ADDX_LEFT_NEAR;
                    Walk_Pos_Y = ADDY_LEFT_NEAR;
                    Walk_Pos_Z = ADDZ_LEFT_NEAR;
                    break;
                case 3:
                    Walk_Pos_X = ADDX_RIGHT_FAR;
                    Walk_Pos_Y = ADDY_RIGHT_FAR;
                    Walk_Pos_Z = ADDZ_RIGHT_FAR;
                    break;
                case 4:
                    Walk_Pos_X = ADDX_RIGHT_MIDDLE;
                    Walk_Pos_Y = ADDY_RIGHT_MIDDLE;
                    Walk_Pos_Z = ADDZ_RIGHT_MIDDLE;
                    break;
                case 5:
                    Walk_Pos_X = ADDX_RIGHT_NEAR;
                    Walk_Pos_Y = ADDY_RIGHT_NEAR;
                    Walk_Pos_Z = ADDZ_RIGHT_NEAR;
                    break;
            }
            pUnit->SendMonsterMoveWithSpeed(Walk_Pos_X, Walk_Pos_Y, Walk_Pos_Z,MOVEMENTFLAG_WALK_MODE);
        }
    }

    void SayInitialAggro()                                  //randomly select 1 of 3 say for aggro
    {
        switch(rand()%3)
        {
            case 0:
                DoYell(SAY_ARRIVAL1,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature,SOUND_ARRIVAL1);
                break;
            case 1:
                DoYell(SAY_ARRIVAL3,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature,SOUND_ARRIVAL3);
                break;
            case 2:
                DoYell(SAY_ARRIVAL5,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature,SOUND_ARRIVAL5);
                break;
        }
    }

    void Aggro(Unit* who)
    {
        switch(rand()%3)
        {
            case 0:
                DoYell(SAY_ARRIVAL1,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature,SOUND_ARRIVAL1);
                break;
            case 1:
                DoYell(SAY_ARRIVAL3,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature,SOUND_ARRIVAL3);
                break;
            case 2:
                DoYell(SAY_ARRIVAL5,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature,SOUND_ARRIVAL5);
                break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget())
            return;

        if(m_creature->getVictim() && m_creature->isAlive())
        {
            //Check for Frost Bolt
            if(FrostBolt_Timer < diff)
            {
                DoCast(m_creature->getVictim(),SPELL_FROST_BOLT);

                if(rand()%2 == 0)
                {
                    DoYell(SAY_FROST,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature,SOUND_FROST);
                }
                //Cast again on time
                FrostBolt_Timer = (rand()%60)*1000;
            }
            else FrostBolt_Timer -= diff;

            //Check for Frost Bolt Nova
            if(FrostBoltNova_Timer < diff)
            {
                DoCast(m_creature->getVictim(),SPELL_FROST_BOLT_NOVA);

                if(rand()%2 == 0)
                {
                    DoYell(SAY_FROST,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature,SOUND_FROST);
                }

                FrostBoltNova_Timer = 15000;
            }
            else FrostBoltNova_Timer -= diff;

            //Check for Chains Of Kelthuzad
            if(ChainsOfKelthuzad_Timer < diff)
            {
                DoCast(m_creature->getVictim(),SPELL_CHAINS_OF_KELTHUZAD);

                if(rand()%2 == 0)
                    if(rand()%2 == 0)
                {
                    DoYell(SAY_CHAIN1,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature,SOUND_CHAIN1);
                }
                else
                {
                    DoYell(SAY_CHAIN2,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature,SOUND_CHAIN2);
                }

                //cast again on time
                ChainsOfKelthuzad_Timer = (rand()%30+30)*1000;
            }
            else ChainsOfKelthuzad_Timer -= diff;

            //Check for Mana Detonation
            if(ManaDetonation_Timer < diff)
            {
                //time to cast
                //DoCast(m_creature->getVictim(),SPELL_MANA_DETONATION);

                if(rand()%2 == 0)
                {
                    DoYell(SAY_SPECIAL1,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature,SOUND_SPECIAL1);
                }
                ManaDetonation_Timer = 20000;
            }
            else ManaDetonation_Timer -= diff;

            //Check for Shadow Fissure
            if(ShadowFisure_Timer < diff)
            {
                DoCast(m_creature->getVictim(),SPELL_SHADOW_FISURE);

                if(rand()%2 == 0)
                {
                    DoYell(SAY_SPECIAL2,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature,SOUND_SPECIAL2);
                }

                ShadowFisure_Timer = 25000;
            }
            else ShadowFisure_Timer -= diff;

            //Check for Frost Blast
            if(FrostBlast_Timer < diff)
            {
                //time to cast
                //DoCast(m_creature->getVictim(),SPELL_FROST_BLAST);

                if(rand()%2 == 0)
                {
                    DoYell(SAY_FROST,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature,SOUND_FROST);
                }

                FrostBlast_Timer = (rand()%30+30)*1000;
            }
            else FrostBlast_Timer -= diff;

            //start phase 3 when we are 40% health
            if(!Phase3 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 40)
            {
                Phase3 = true;
                switch(rand()%2)
                {
                    case 1:
                        DoYell(SAY_REINFORCEMENTS1,LANG_UNIVERSAL,NULL);
                        DoPlaySoundToSet(m_creature,SOUND_REINFORCEMENTS1);
                        break;
                    case 2:
                        DoYell(SAY_REINFORCEMENTS2,LANG_UNIVERSAL,NULL);
                        DoPlaySoundToSet(m_creature,SOUND_REINFORCEMENTS2);
                        break;
                }
                //here Lich King should respond to KelThuzad but I don't know which creature to make talk
                //so for now just make Kelthuzad says it.
                DoPlaySoundToSet(m_creature,SOUND_LICH_NAXX_SUMMON);
            }

            if(Phase3 && (GuardiansOfIcecrown_Count < 5))
                if(GuardiansOfIcecrown_Timer < diff)
            {
                //Summon a Guardian of Icecrown in a random alcove (Creature # 16441)
                //uint32 TimeToWalk;
                Unit* pUnit = NULL;
                float Walk_Pos_X;
                float Walk_Pos_Y;
                float Walk_Pos_Z;
                switch(rand()%6)
                {
                    case 0:
                        pUnit = m_creature->SummonCreature(16441,ADDX_LEFT_FAR,ADDY_LEFT_FAR,ADDZ_LEFT_FAR,ADDO_LEFT_FAR,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000);
                        //Setting walk position
                        Walk_Pos_X = WALKX_LEFT_FAR;
                        Walk_Pos_Y = WALKY_LEFT_FAR;
                        Walk_Pos_Z = WALKZ_LEFT_FAR;
                        break;
                    case 1:
                        pUnit = m_creature->SummonCreature(16441,ADDX_LEFT_MIDDLE,ADDY_LEFT_MIDDLE,ADDZ_LEFT_MIDDLE,ADDO_LEFT_MIDDLE,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_LEFT_MIDDLE;
                        Walk_Pos_Y = WALKY_LEFT_MIDDLE;
                        Walk_Pos_Z = WALKZ_LEFT_MIDDLE;
                        break;
                    case 2:
                        pUnit = m_creature->SummonCreature(16441,ADDX_LEFT_NEAR,ADDY_LEFT_NEAR,ADDZ_LEFT_NEAR,ADDO_LEFT_NEAR,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_LEFT_NEAR;
                        Walk_Pos_Y = WALKY_LEFT_NEAR;
                        Walk_Pos_Z = WALKZ_LEFT_NEAR;
                        break;
                    case 3:

                        pUnit = m_creature->SummonCreature(16441,ADDX_RIGHT_FAR,ADDY_RIGHT_FAR,ADDZ_RIGHT_FAR,ADDO_RIGHT_FAR,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_RIGHT_FAR;
                        Walk_Pos_Y = WALKY_RIGHT_FAR;
                        Walk_Pos_Z = WALKZ_RIGHT_FAR;
                        break;
                    case 4:
                        pUnit = m_creature->SummonCreature(16441,ADDX_RIGHT_MIDDLE,ADDY_RIGHT_MIDDLE,ADDZ_RIGHT_MIDDLE,ADDO_RIGHT_MIDDLE,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_RIGHT_MIDDLE;
                        Walk_Pos_Y = WALKY_RIGHT_MIDDLE;
                        Walk_Pos_Z = WALKZ_RIGHT_MIDDLE;
                        break;
                    case 5:
                        pUnit = m_creature->SummonCreature(16441,ADDX_RIGHT_NEAR,ADDY_RIGHT_NEAR,ADDZ_RIGHT_NEAR,ADDO_RIGHT_NEAR,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_RIGHT_NEAR;
                        Walk_Pos_Y = WALKY_RIGHT_NEAR;
                        Walk_Pos_Z = WALKZ_RIGHT_NEAR;
                        break;
                }

                if (pUnit)
                {
                    //if we find no one to figth walk to the center
                    if(!pUnit->isInCombat())
                        pUnit->SendMonsterMoveWithSpeed(Walk_Pos_X,Walk_Pos_Y,Walk_Pos_Z,MOVEMENTFLAG_WALK_MODE);

                    //Safe storing of creatures
                    GuardiansOfIcecrown[GuardiansOfIcecrown_Count] = pUnit->GetGUID();

                    //Update guardian count
                    GuardiansOfIcecrown_Count++;

                }
                //5 seconds until summoning next guardian
                GuardiansOfIcecrown_Timer = 5000;
            }
            else GuardiansOfIcecrown_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_boss_kelthuzadAI(Creature *_Creature)
{
    return new boss_kelthuzadAI (_Creature);
}

void AddSC_boss_kelthuzad()
{
    //This script is disabled until it has been throughly tested

    /*
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_kelthuzad";
    newscript->GetAI = GetAI_boss_kelthuzadAI;
    m_scripts[nrscripts++] = newscript;
    */
}
