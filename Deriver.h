/**
 *	Author: Ben Cowley, 2008.
 *	Header for Deriver.cpp;
 */
#ifndef INC_DERIVER_H
#define INC_DERIVER_H

//Feature derivation functions - AGGREGATE
float Aggregate1_LivesSDev( deque<PacState> arg );
float Aggregate2_ChryBoolSum( deque<PacState> arg );
float Aggregate3_CycleCountPerStateSDev( deque<PacState> arg );

//Feature derivation functions - SIMPLE
deque<float> Simple1_Levels( deque<PacState> arg );
deque<float> Simple2_LivesChng( deque<PacState> arg );
float Simple4_TeleportUse( deque<PacState> arg );

//Feature derivation functions - AGGRESSION
float Aggression1_AtkGstHse( deque<PacState> arg );
float Aggression3_GhostKills( deque<PacState> arg );
float Aggression4_HuntAfterPill( deque<PacState> arg );
float Aggression6_Chase¬Dots( deque<PacState> arg );

//Feature derivation functions - PLANNING
deque<float> Planning1_LureToPill( deque<PacState> arg );
float Planning3_CntDtsB4Pll1( deque<PacState> arg );
deque<float> Planning4_SpeedOfHunt( deque<PacState> arg );
float Planning7_PutOffGstHse( deque<PacState> arg );

//Feature derivation functions - CAUTION
deque<float> Caution1_ThreatPrcptn( deque<PacState> arg );
float Caution2_AvgDstPacAtkGstCtrd( deque<PacState> arg );
float Caution3_CloseCalls( deque<PacState> arg );
float Caution4_CaughtAfterHunt( deque<PacState> arg );
float Caution5_MovesWithoutGain( deque<PacState> arg );
float Caution6_PntsOvrLives( deque<PacState> arg );
float Caution7_KilledAtHouse( deque<PacState> arg );

//Feature derivation functions - SPEED


//Feature derivation functions - DECISIVENESS
float Decisive2_Osclltng( deque<PacState> arg );
float Decisive1_CyclesPrMv( deque<PacState> arg );

//Feature derivation functions - RESOURCE
deque<float> Resource_TimeAndNumChry( deque<PacState> arg );

//Feature derivation functions - THOROUGHNESS

//Feature derivation functions - CONTROL SKILL


#endif /* INC_DERIVER_H */