class GRAY_RouletteAD
{
	GRAY_RouletteManager rouletteManager;
	ref array<GRAY_RouletteTeamData> teams;
	
	void GRAY_RouletteAD()
	{
		rouletteManager = GRAY_RouletteManager.GetInstance();
		Scenario();
	}
	
	void Scenario()
	{
		Print("GRAY_RouletteAD.Scenario | Attack and Defend selected!");

		// Find Objective
		float objectiveSize;
		vector objectivePosition = FindObjective(objectiveSize);
		
		// Get Teams
		rouletteManager.SelectTeams(teams);
		Print("GRAY_RouletteAD.Scenario teams = " + teams);
		
		// Find defender spawn
		vector defenderSpawn;
		bool defenderSpawnFound = FindSpawn(defenderSpawn, objectivePosition, 0, rouletteManager.m_aoLimitWidth - 50);
		if(!defenderSpawnFound)
			return Scenario();
		
		// Find attacker spawn
		vector attackerSpawn;
		bool attackerSpawnFound = FindSpawn(attackerSpawn, objectivePosition, rouletteManager.m_minAoLimitLength, rouletteManager.m_maxAoLimitLength);
		if(!attackerSpawnFound)
			return Scenario();
		
		// Setup Briefing
		SetupBriefing(teams[0], teams[1]);
		
		// Setup Gamemode
		SetupGamemode(teams[0], teams[1]);
		
		// Setup AO Limit
		rouletteManager.SetupAOLimit(objectivePosition, attackerSpawn, rouletteManager.m_aoLimitWidth);
		
		SetupMarkers(defenderSpawn, attackerSpawn, objectivePosition, objectiveSize);
		
		rouletteManager.SpawnTeam(teams[0], defenderSpawn, 13, 50);
	}
	
	
	vector FindObjective(out float objectiveSize)
	{
		Print("GRAY_RouletteAD.FindObjective");

		vector objectivePosition;
		bool objectiveFound = false;
		while(!objectiveFound)
		{
			objectiveFound = rouletteManager.FindBuildings(objectivePosition, objectiveSize, rouletteManager.m_minBuildingCount, 
			rouletteManager.m_maxBuildingCount, rouletteManager.m_maxBuildingDistance)
		}
		
		return objectivePosition;
	}
	
	bool FindSpawn(out vector spawnPosition, vector startPosition, float minDistance, float maxDistance)
	{
		int count = 0;
		bool found = false;
		while(!found && count < 100)
		{
			found = rouletteManager.FindEmptySpaceLine(spawnPosition, startPosition, 250, 12, minDistance, maxDistance);
			count++;
		}
		
		if(!found)
		{
			Print("GRAY_RouletteManager.ScenarioCapture - no defender pos can be found");
			return false;
		}
		
		return true;
	}
	
	void SetupBriefing(GRAY_RouletteTeamData defendingTeam, GRAY_RouletteTeamData attackingTeam)
	{
		// Notes
		string description = "Blufor(attackers) are the " + defendingTeam.GetName() + ".\nOpfor(defenders) are the " + defendingTeam.GetName() + ".\n\nVI. End Conditions:\nTime Limit - 70 minutes\nBlufor Win - Capture Marked objective(red circle)\nLoss - 100 Percent player casualties\n\nm_ratio 2 : 1";
		rouletteManager.SpawnBriefing("Briefing: Notes", description);
		
		// Defender Situation
		description = "I. Situation:\nEveron, 2020.\nLocal Time: Day\nWeather: Clear\n\nIa. Friendly Forces:\n1x " + attackingTeam.GetName() + " platoon. (Marked)\n\nIb. Enemy Forces:\n1x Platoon of " + defendingTeam.GetName() + " (Marked)";
		rouletteManager.SpawnBriefing("Briefing: Situation", description, defendingTeam, 2);
		
		// Defender Mission
		description = "II. Mission:\nOur platoon is to DEFEND the strategic building marked with a red circle.\nIII. Execution:\nAs per CO's intent.\n\nIV. Command/Signals:\nPlatoon HQ - 10\n1st Squad - 11\n2nd Squad - 12\n3rd Squad - 13\n4th Squad - 14\n\nV. Service/Support:\nNo Coy/Btn Fire Support.\nNo Resupply.";
		rouletteManager.SpawnBriefing("Briefing: Mission", description, defendingTeam, 3);
		

		// Attacker Situation
		description = "I. Situation:\nEveron, 2020.\nLocal Time: Day\nWeather: Clear\n\nIa. Friendly Forces:\n1x " + defendingTeam.GetName() + " platoon. (Marked)\n\nIb. Enemy Forces:\n1x Platoon of " + attackingTeam.GetName() + " (Marked)";
		rouletteManager.SpawnBriefing("Briefing: Situation", description, attackingTeam, 2);
		
		// Attacker Mission
		description = "II. Mission:\nOur platoon is to CAPTURE the strategic building marked with a red circle.\nIII. Execution:\nAs per CO's intent.\n\nIV. Command/Signals:\nPlatoon HQ - 10\n1st Squad - 11\n2nd Squad - 12\n3rd Squad - 13\n4th Squad - 14\n\nV. Service/Support:\nNo Coy/Btn Fire Support.\nNo Resupply.";
		rouletteManager.SpawnBriefing("Briefing: Mission", description, attackingTeam, 3);
	}
	
	void SetupGamemode(GRAY_RouletteTeamData defendingTeam, GRAY_RouletteTeamData attackingTeam)
	{
		TILW_MissionFrameworkEntity framework = TILW_MissionFrameworkEntity.GetInstance();
		
		string defenderKey = defendingTeam.GetFaction();
		string attackerKey = attackingTeam.GetFaction();
		
		//Create flags
		TILW_FactionPlayersKilledFlag defenderFlag = TILW_FactionPlayersKilledFlag();
		defenderFlag.SetFlag("defender");
		defenderFlag.SetKey(defenderKey);
		defenderFlag.SetCasualtyRatio(1);

		TILW_FactionPlayersKilledFlag attackerFlag = TILW_FactionPlayersKilledFlag();
		attackerFlag.SetFlag("defender");
		attackerFlag.SetKey(defenderKey);
		attackerFlag.SetCasualtyRatio(1);
		
		array<ref TILW_FactionPlayersKilledFlag> flags = {defenderFlag, attackerFlag};
		
		//Instructions
		TILW_EndGameInstruction defenderInstruction = TILW_EndGameInstruction();
		defenderInstruction.SetGameOverType(EGameOverTypes.EDITOR_FACTION_VICTORY);
		defenderInstruction.SetKey(attackerKey);		
		
		TILW_EndGameInstruction attackerInstruction = TILW_EndGameInstruction();
		attackerInstruction.SetGameOverType(EGameOverTypes.EDITOR_FACTION_VICTORY);
		attackerInstruction.SetKey(defenderKey);

		TILW_EndGameInstruction timelimitInstruction = TILW_EndGameInstruction();
		timelimitInstruction.SetGameOverType(EGameOverTypes.EDITOR_FACTION_VICTORY);
		timelimitInstruction.SetKey(defenderKey);
		timelimitInstruction.SetExecutionDelay(4200);
		
		TILW_EndGameInstruction objectiveInstruction = TILW_EndGameInstruction();
		attackerInstruction.SetGameOverType(EGameOverTypes.EDITOR_FACTION_VICTORY);
		attackerInstruction.SetKey(attackerKey);
		
		//Conditions
		TILW_LiteralTerm defenderTerm = TILW_LiteralTerm();
		defenderTerm.SetFlag("defender");
		
		TILW_LiteralTerm atttackerTerm = TILW_LiteralTerm();
		atttackerTerm.SetFlag("attacker");
		
		TILW_LiteralTerm timeLimitTerm = TILW_LiteralTerm();
		timeLimitTerm.SetFlag("timelimit");
		timeLimitTerm.SetInvert(true);

		TILW_LiteralTerm objectiveTerm = TILW_LiteralTerm();
		objectiveTerm.SetFlag("objective");
		
		//Create events
		TILW_MissionEvent defenderEvent = TILW_MissionEvent();
		defenderEvent.SetName("defender event");
		defenderEvent.SetInstructions({defenderInstruction});
		defenderEvent.SetCondition(defenderTerm);
		
		TILW_MissionEvent attackerEvent = TILW_MissionEvent();
		attackerEvent.SetName("attacker event");
		attackerEvent.SetInstructions({attackerInstruction});
		attackerEvent.SetCondition(atttackerTerm);
		
		TILW_MissionEvent timeLimitEvent = TILW_MissionEvent();
		timeLimitEvent.SetName("timeLimit event");
		timeLimitEvent.SetInstructions({timelimitInstruction});
		timeLimitEvent.SetCondition(timeLimitTerm);
		
		TILW_MissionEvent objectiveEvent = TILW_MissionEvent();
		objectiveEvent.SetName("objective event");
		objectiveEvent.SetInstructions({objectiveInstruction});
		objectiveEvent.SetCondition(objectiveTerm);
		
		array<ref TILW_MissionEvent> events = new array<ref TILW_MissionEvent>;
		events.Insert(defenderEvent);
		events.Insert(attackerEvent);
		events.Insert(timeLimitEvent);
		events.Insert(objectiveEvent);
		
		framework.SetMissionEvents(events);
		framework.SetPlayersKilledFlags(flags);
	}

	void SetupMarkers(vector defenderSpawn, vector attackerSpawn, vector objective, float size)
	{
		// Objective
		rouletteManager.SpawnMarker(objective, "{E23427CAC80DA8B7}UI/Textures/Icons/icons_mapMarkersUI.imageset", "circle-2", string.Empty, size * 2, Color.Red);
		
		// Spawns
		FactionManager fm = GetGame().GetFactionManager();
		
		// Defenders
		SCR_Faction defenders = SCR_Faction.Cast(fm.GetFactionByKey(teams[0].GetFaction()));
		PS_ManualMarker marker = rouletteManager.SpawnMarker(defenderSpawn, defenders.GetFactionFlag(), "", teams[0].GetName(), 40, Color.White, false);
		marker.SetAngles(Vector(0, 90, 0));
		
		// Attackers
		SCR_Faction attackers = SCR_Faction.Cast(fm.GetFactionByKey(teams[1].GetFaction()));
		marker = rouletteManager.SpawnMarker(attackerSpawn, attackers.GetFactionFlag(), "", teams[1].GetName(), 40, Color.White, false);
		marker.SetAngles(Vector(0, 90, 0));
	}
}