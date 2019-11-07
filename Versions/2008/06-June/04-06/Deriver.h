/**
 *	Author: Ben Cowley, 2008.
 *	Header for Deriver.cpp;
 */
#ifndef INC_DERIVER_H
#define INC_DERIVER_H

#include "universals.h"

//Feature derivation functions - AGGREGATE
float Aggregate1_LivesSDev( deque<PacState> arg );
float Aggregate2_ChryBoolSum( deque<PacState> arg );
float Aggregate3_CycleCountPerStateSDev( deque<PacState> arg );

//Feature derivation functions - SIMPLE
deque<float> Simple1_Levels( deque<PacState> arg );
deque<float> Simple2_LivesChng( deque<PacState> arg );
float Simple4_TeleportUse( deque<PacState> arg );

//Feature derivation functions - AGGRESSION
float Aggression4_HuntAfterPill( deque<PacState> arg );

//Feature derivation functions - PLANNING


//Feature derivation functions - CAUTION
float Caution4_CaughtAfterHunt( deque<PacState> arg );
float Caution5_MovesWithoutGain( deque<PacState> arg );

//Feature derivation functions - SPEED


//Feature derivation functions - DECISIVENESS
float Decisive2_Osclltng( deque<PacState> arg );

//Feature derivation functions - RESOURCE
deque<float> Resource_TimeAndNumChry( deque<PacState> arg );

//Feature derivation functions - THOROUGHNESS


//Feature derivation functions - CONTROL SKILL


#endif /* INC_DERIVER_H */