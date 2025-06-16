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
		
		// Random Time
		rouletteManager.SetRandomTime();
		
		// Find Objective
		float objectiveSize;
		vector objectivePosition = FindObjective(objectiveSize);
		
		// Get Teams
		rouletteManager.SelectTeams(teams);
		Print("GRAY_RouletteAD.Scenario teams = " + teams);
		
		// Find defender spawn
		vector defenderSpawn;
		bool defenderSpawnFound = FindSpawn(defenderSpawn, objectivePosition, 0, rouletteManager.m_aoLimitWidth / 2 - 50);
		if(!defenderSpawnFound)
			return Scenario();
		
		// Find attacker spawn
		vector attackerSpawn;
		bool attackerSpawnFound = FindSpawn(attackerSpawn, objectivePosition, rouletteManager.m_minAoLimitLength, rouletteManager.m_maxAoLimitLength);
		if(!attackerSpawnFound)
			return Scenario();
		
		// Spawn teams
		float ratio = (teams[0].GetStrength() - teams[1].GetStrength()) + rouletteManager.m_ratio;
		Print("GRAY_RouletteAD.Scenario ratio = "+ ratio);

		map<string, int> defenderCounts;
		int defenderRatio = Math.AbsInt(rouletteManager.m_minPlayerCount / (1 + ratio));
		int defenderCount = rouletteManager.SpawnTeam(defenderCounts, teams[0], defenderSpawn, defenderRatio, defenderRatio - 5);
		
		map<string, int> attackerCounts;
		int attackerRatio = Math.AbsInt(defenderCount * ratio);
		int attackerCount = rouletteManager.SpawnTeam(attackerCounts, teams[1], attackerSpawn, attackerRatio, attackerRatio - 5);
		
		// Setup Briefing
		SetupBriefing(teams[0], defenderCounts, teams[1], attackerCounts);
		
		// Setup AO Limit
		rouletteManager.SetupAOLimit(objectivePosition, attackerSpawn, rouletteManager.m_aoLimitWidth);
		
		// Setup Objective
		rouletteManager.SetupObjective(objectivePosition, objectiveSize, teams[1]);
		
		// Setup Markers
		SetupMarkers(defenderSpawn, attackerSpawn, objectivePosition, objectiveSize);
		
		// Setup Gamemode
		SetupGamemode(teams[0], teams[1]);
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
	
	void SetupBriefing(GRAY_RouletteTeamData defendingTeam, map<string, int> defenderCounts, GRAY_RouletteTeamData attackingTeam, map<string, int> attackerCounts)
	{
		array<string> keys = {"squad", "platoon", "company"};
		string elementDefender = "";
		foreach(string key : keys)
		{
			int count = defenderCounts.Get(key);
			if(count == 0)
				continue;
			
			elementDefender = string.Format("%1x %2 %3", count, defendingTeam.GetName(), key);
		}
		
		string elementAttacker = "";
		foreach(string key : keys)
		{
			int count = attackerCounts.Get(key);
			if(count == 0)
				continue;
			
			elementAttacker = string.Format("%1x %2 %3", count, attackingTeam.GetName(), key);
		}
		
		foreach(GRAY_RouletteBriefingData briefing : rouletteManager.m_breifings)
		{
			string description = string.Format(briefing.GetDescription(), defendingTeam.GetName(), attackingTeam.GetName(), elementDefender, elementAttacker);

			switch (briefing.GetSide())
	        {
	            case GRAY_eBriefingType.Defender:
					GRAY_MissionDescription entity = rouletteManager.SpawnBriefing(briefing.GetTitle(), description, 2);
					entity.SetVisibleForFactionKey(defendingTeam.GetFaction(), true);
					break;
	            case GRAY_eBriefingType.Attacker:
	                GRAY_MissionDescription entity = rouletteManager.SpawnBriefing(briefing.GetTitle(), description, 2);
					entity.SetVisibleForFactionKey(attackingTeam.GetFaction(), true);
					break;
	            default:
	                GRAY_MissionDescription entity = rouletteManager.SpawnBriefing(briefing.GetTitle(), description);
					entity.SetVisibleForEmptyFaction(true);
					entity.SetShowAnyFaction(true);
					break;
	        }
		}
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
		PS_ManualMarker marker = rouletteManager.SpawnMarker(objective, "{E23427CAC80DA8B7}UI/Textures/Icons/icons_mapMarkersUI.imageset", "circle-2");
		marker.SetSize(size * 4);
		marker.SetColor(Color.Red);

		// Spawns
		FactionManager fm = GetGame().GetFactionManager();
		
		// Defenders
		SCR_Faction defenders = SCR_Faction.Cast(fm.GetFactionByKey(teams[0].GetFaction()));
		marker = rouletteManager.SpawnMarker(defenderSpawn, defenders.GetFactionFlag(), "", false);
		marker.SetAngles({0, 90, 0});
		marker.SetSize(40);
		
		// Attackers
		SCR_Faction attackers = SCR_Faction.Cast(fm.GetFactionByKey(teams[1].GetFaction()));
		marker = rouletteManager.SpawnMarker(attackerSpawn, attackers.GetFactionFlag(), "", false);
		marker.SetAngles(Vector(0, 90, 0));
		marker.SetSize(40);
	}
}