[EntityEditorProps(category: "Gray", description: "Roulette Manager")]
class GRAY_RouletteManagerClass : GenericEntityClass
{
}

/* Todo :
Find better way to balence teams
Spawn water and flatness check
Get rid of blufor scopes
scope level? None, some, all?
Better spawn space detection. spawn vehicles first then check?
Assets
*/

enum GRAY_eScenarios
{
	AttackAndDefend
}

class GRAY_RouletteManager : GenericEntity
{
	[Attribute(defvalue: "60", uiwidget: UIWidgets.EditBox, desc: "Target minimum player count")]
    int m_minPlayerCount;
	
	[Attribute(defvalue: "2", uiwidget: UIWidgets.EditBox, desc: "Target m_ratio, 1 : ?")]
    int m_ratio;
	
	[Attribute(defvalue: "1000", uiwidget: UIWidgets.EditBox, desc: "The width of the AO limit", category: "Attack and Defend")]
    int m_aoLimitWidth;
	
	[Attribute(defvalue: "1500", uiwidget: UIWidgets.EditBox, desc: "The minimum length of the AO limit", category: "Attack and Defend")]
    int m_minAoLimitLength;
	
	[Attribute(defvalue: "2500", uiwidget: UIWidgets.EditBox, desc: "The maximum length of the AO limit", category: "Attack and Defend")]
    int m_maxAoLimitLength;
	
	[Attribute(defvalue: "3", uiwidget: UIWidgets.EditBox, desc: "The minimum amount of buildings needed nearby", category: "Attack and Defend")]
    int m_minBuildingCount;
	
	[Attribute(defvalue: "10", uiwidget: UIWidgets.EditBox, desc: "The maximum amount of buildings to consider when creating the objective size", category: "Attack and Defend")]
    int m_maxBuildingCount;
	
	[Attribute(defvalue: "75", uiwidget: UIWidgets.EditBox, desc: "The maximum distance for buildings to consider apart of the objective building", category: "Attack and Defend")]
    int m_maxBuildingDistance;
	
	[Attribute(defvalue: "200", uiwidget: UIWidgets.EditBox, desc: "The distance to search for buildings within", category: "Advanced")]
    int m_searchDistance;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourceNamePicker, desc: "Blacklist of buildings to not consider", category: "Advanced")]
    ref array<ResourceName> m_buildingBlackList;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.EditBox, desc: "Blacklist of keywords for buildings to not consider. Case sensitive!!", category: "Advanced")]
    ref array<string> m_buildingBlackListKeyword;
	
	[Attribute(defvalue: "", UIWidgets.Object)]
    ref array<ref GRAY_RouletteTeamData> m_teamsList;

	protected ref array<IEntity> m_buildingList = new array<IEntity>();
	
	protected ref RandomGenerator random = new RandomGenerator();
	
	static protected GRAY_RouletteManager m_instance;
	
	void GRAY_RouletteManager(IEntitySource src, IEntity parent)
	{
		m_instance = this;
		SetEventMask(EntityEvent.INIT);
	}
	
	static GRAY_RouletteManager GetInstance()
	{
		return m_instance;
	}
	
	override protected void EOnInit(IEntity owner)
	{
		Print("GRAY_RouletteManager.EOnInit");

		if(Replication.IsClient() || SCR_Global.IsEditMode(owner))
			return;

		GetGame().GetCallqueue().CallLater(SelectScenario, random.RandInt(5,500), false);	
	}
	
	void SelectScenario()
	{
		Print("GRAY_RouletteManager.SelectScenario");
		
		array<GRAY_eScenarios> scenarios = {};
		SCR_Enum.GetEnumValues(GRAY_eScenarios, scenarios);


		int scenario = random.RandInt(1, scenarios.Count());
		switch (scenario)
		{
			case 1: new GRAY_RouletteAD(); // Attack and Defend
		}	
	}
	
	void SetupObjective(vector position, float size, GRAY_RouletteTeamData attackingTeam)
	{
		//Create and move the objective
		ResourceName prefab = "{145F6522D0DD766C}Prefabs/Roulette/Capture_Area.et";
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.Transform[3] = position;
		TILW_FactionTriggerEntity objective = TILW_FactionTriggerEntity.Cast(GetGame().SpawnEntityPrefab(Resource.Load(prefab), null, spawnParams));
		objective.SetRadius(size);
		objective.SetOwnerFaction(attackingTeam.GetFaction());
	}
	
	void SpawnBriefing(string title, string text, GRAY_RouletteTeamData team = null, int order = 0)
	{
		ResourceName prefab = "{94B01B942436D850}Prefabs/Roulette/Briefing.et";
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		GRAY_MissionDescription briefing = GRAY_MissionDescription.Cast(GetGame().SpawnEntityPrefab(Resource.Load(prefab), null, spawnParams));
		briefing.SetTitle(title);
		briefing.SetTextData(text);
		
		if(team)
		{
			briefing.SetVisibleForFactionKey(team.GetFaction(), true);
			briefing.SetVisibleForEmptyFaction(false);
			briefing.SetShowAnyFaction(false);
		}
		
		if(order)
			briefing.SetOrder(order);
	}
	
	PS_ManualMarker SpawnMarker(vector position, string imageSet, string quadName, string text = string.Empty, float size = 0, Color color = Color.White, bool worldScale = true)
	{
		ResourceName prefab = "{CD85ADE9E0F54679}PrefabsEditable/Markers/EditableMarker.et";
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.Transform[3] = position;
		PS_ManualMarker marker = PS_ManualMarker.Cast(GetGame().SpawnEntityPrefab(Resource.Load(prefab), null, spawnParams));
		
		marker.SetVisibleForEmptyFaction(true);
		marker.m_bShowForAnyFaction = true;

		marker.SetUseWorldScale(worldScale);
		marker.SetImageSetGlow("");
		marker.SetImageSet(imageSet);
		marker.SetQuadName(quadName);
		
		marker.SetColor(color);
		
		Print("GRAY_RouletteAD. marker color = " + color);
		
		if(size)
			marker.SetSize(size);

		if(text)
			marker.SetDescription(text);
		
		return marker;
	}

	void SetupAOLimit(vector position1, vector position2, float width)
	{
		Print("GRAY_RouletteManager.SetupAOLimit");
		position1[1] = 0;
		position2[1] = 0;
		array<vector> points = new array<vector>;
		vector direction = vector.Direction(position1, position2).Normalized();
		//IEntitySource test = IEntitySource.Cast(this);
		
		// Calculate perpendicular directions at 45 degrees
	    vector perpendicular1 = Vector(direction[2], 0, -direction[0]); // Rotate 90 degrees clockwise
	    vector perpendicular2 = Vector(-direction[2], 0, direction[0]); // Rotate 90 degrees counterclockwise
	
	    // Scale perpendicular vectors to 500 meters
	    vector offset1 = (-direction + perpendicular1) * (width / 2); // 45 degrees clockwise
	    vector offset2 = (-direction + perpendicular2) * (width / 2); // 45 degrees counterclockwise
		vector offset3 = (direction + perpendicular2) * (width / 2); // Opposite direction + 45 degrees counterclockwise
	    vector offset4 = (direction + perpendicular1) * (width / 2); // Opposite direction + 45 degrees clockwise

	    // Calculate points
	    points.Insert(position1 + offset1);
	    points.Insert(position1 + offset2);
	    points.Insert(position2 + offset3);
	    points.Insert(position2 + offset4);
		
		ResourceName prefabPath = "{3E528AFFB74CEEE5}Prefabs/Roulette/AO_Limit.et";
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		IEntity entity = GetGame().SpawnEntityPrefab(Resource.Load(prefabPath), null, spawnParams);
		TILW_AOLimitComponent AOLimitComp = TILW_AOLimitComponent.Cast(entity.FindComponent(TILW_AOLimitComponent));
		Print("GRAY_RouletteManager.SetupAOLimit - AO Limit entity" + entity);

		AOLimitComp.SetPoints(points);
	}
	
	void SpawnTeam(GRAY_RouletteTeamData team, vector position, int targetCount, int minCount = 1, vector offset = Vector(0,0,6))
	{
		array<ref array<ResourceName>> prefabsToSpawnByCompany;
		GetTeamPrefabs(prefabsToSpawnByCompany, team, targetCount, minCount);
		
		foreach (array<ResourceName> companyPrefabs : prefabsToSpawnByCompany)
		{
			foreach (ResourceName prefab : companyPrefabs)
			{
				SpawnPrefab(prefab, position + offset);
				offset[2] = offset[2] - 2;
			}
		}
	}
	
	int GetTeamPrefabs(out array<ref array<ResourceName>> prefabsToSpawnByCompany, GRAY_RouletteTeamData team, int targetCount, int minCount = 1)
	{
		int currentCount = 0;
		prefabsToSpawnByCompany = {};
	
		map<string, int> elementCounts;
		
		foreach (GRAY_RouletteCompany company : team.GetCompanies())
		{
			array<ResourceName> companyPrefabs = {};
			bool companyAdded = false;
	
			foreach (GRAY_RoulettePlatoon platoon : company.GetPlatoons())
			{
				array<ResourceName> platoonPrefabs = {};
				bool platoonAdded = false;
	
				foreach (GRAY_RouletteSquad squad : platoon.GetSquads())
				{
					int squadCount = squad.GetPlayerCount();
	
					// Ensure we add a company prefab at the very top before anything else
					if (!companyAdded && !companyPrefabs.IsEmpty())
					{
						int companyCount = company.GetPlayerCount();
						if (IsCloserTo(currentCount + companyCount, currentCount, targetCount) || currentCount < minCount)
						{
							companyPrefabs.InsertAt(company.GetPrefab(), 0); // Insert company prefab first!
							currentCount += companyCount;
							companyAdded = true;
						}
					}
	
					// Ensure we add a platoon prefab before inserting squads
					if (!platoonAdded && !platoonPrefabs.IsEmpty())
					{
						int platoonCount = platoon.GetPlayerCount();
						if (IsCloserTo(currentCount + platoonCount, currentCount, targetCount) || currentCount < minCount)
						{
							platoonPrefabs.InsertAt(platoon.GetPrefab(), 0); // Insert platoon prefab first!
							currentCount += platoonCount;
							platoonAdded = true;
						}
					}
					
					if (IsCloserTo(currentCount, currentCount + squadCount, targetCount) && currentCount > minCount)
						break;
	
					// Insert the squad prefab
					platoonPrefabs.Insert(squad.GetPrefab());
					currentCount += squadCount;
				}
	
				// Add Platoon + Squads to Company hierarchy only if a squad was added
				companyPrefabs.InsertAll(platoonPrefabs);
	
				// Stop if we've reached the target count
				if (currentCount >= targetCount && currentCount > minCount)
					break;
			}
	
			// Insert the company level data into the final array
			prefabsToSpawnByCompany.Insert(companyPrefabs);
	
			if (currentCount >= targetCount && currentCount > minCount)
				break;
		}

		return currentCount;
	}


	
	//Print("GRAY_RouletteManager.GetTeamPrefabs currentCount = " + currentCount);

	void SelectTeams(out array<GRAY_RouletteTeamData> teams, int count = 2)
	{
		Print("GRAY_RouletteManager.SelectTeams");
		
		// Setup team pools
		map<GRAY_eScopeType, ref array<GRAY_RouletteTeamData>> teamPool = new map<GRAY_eScopeType, ref array<GRAY_RouletteTeamData>>();
		array<GRAY_eScopeType> enumValues = {};
		SCR_Enum.GetEnumValues(GRAY_eScopeType, enumValues);
		foreach(GRAY_eScopeType value : enumValues) teamPool.Insert(value, {});
		
		foreach(GRAY_RouletteTeamData team : m_teamsList)
		{
			GRAY_eScopeType scopeType = team.GetScopeType();
			array<GRAY_RouletteTeamData> teamArray = teamPool.Get(scopeType);
			teamArray.Insert(team);
		}
		
		foreach(GRAY_eScopeType scopeType, array<GRAY_RouletteTeamData> teamArray : teamPool)
		{
			if(teamArray.Count() < count)
				teamPool.Remove(scopeType);
		}
		
		if(teamPool.IsEmpty())
			return Print("GRAY_RouletteManager.SetupTeams | Not enough teams!", LogLevel.ERROR);
		
		int randomIndex = Math.RandomInt(1,teamPool.Count()) - 1;
		array<GRAY_RouletteTeamData> selectedTeams = teamPool.GetElement(randomIndex);
		teams = {};
		for (int i = 0; i < count; i++)
		{
			GRAY_RouletteTeamData team  = selectedTeams.GetRandomElement();
		    teams.Insert(team);
			selectedTeams.RemoveItem(team);
		}
	}
	
	
	bool FindEmptySpaceLine(out vector outPosition, vector startPosition, float searchRadius, float searchSize, float minDistance, float maxDistance)
	{
		//Print("GRAY_RouletteManager.FindEmptySpace");
		
		float angle = random.RandFloatXY(0, Math.PI2);
		
		float stepDistance = searchRadius * 2;
		float distance = minDistance;
		int count = 0;
		
		while(count < 100 && distance < maxDistance)
		{
			float directionX = Math.Cos(angle);
	    	float directionZ = Math.Sin(angle);
			vector searchPosition = Vector(directionX, 0, directionZ) * distance + startPosition;
			
			bool isWater = IsWaterOnLine(startPosition, searchPosition, 10, 2);
			if(isWater)
				return false;

			bool found = FindEmptyPosition(outPosition, searchPosition, searchRadius, searchSize);
			if(found)
				return true;

			distance += stepDistance;
			count++;
		}
		
		return false;
	}
	
	bool FindCaptureObjective(out vector position, out float size)
	{
		Print("GRAY_RouletteManager.FindCaptureObjective");
		BaseWorld world = GetGame().GetWorld();
		
		protected vector worldMin;
		protected vector worldMax;
		world.GetBoundBox(worldMin, worldMax);

		float randomX = random.RandFloatXY(worldMin[0], worldMax[0]);
		float randomZ = random.RandFloatXY(worldMin[2], worldMax[2]);	
		float randomY = world.GetSurfaceY(randomX, randomZ);
		vector randomPosition = Vector(randomX, randomY, randomZ);
		
		//Check for buildings
		m_buildingList.Clear();
		GetGame().GetWorld().QueryEntitiesBySphere(randomPosition, m_searchDistance, BuildingFilter, null, EQueryEntitiesFlags.STATIC);
		if(m_buildingList.IsEmpty())
			return false;
		
		ref IEntity objBuilding = m_buildingList.GetRandomElement();
		position = objBuilding.GetOrigin();
		
		m_buildingList.Clear();
		GetGame().GetWorld().QueryEntitiesBySphere(objBuilding.GetOrigin(), m_maxBuildingDistance, BuildingFilter, null, EQueryEntitiesFlags.STATIC);
		if(m_buildingList.Count() < m_minBuildingCount)
			return false;
		
		int buildingCount = m_buildingList.Count();
		Print("GRAY_RouletteManager.FindCaptureObjective - building count=" + buildingCount);
		//entity = building;

		size = 0;
		int count;
		foreach(IEntity building : m_buildingList)
		{
			if(count > m_maxBuildingCount)
				break;

			vector buildingPosition = building.GetOrigin();
			
			float distance = vector.Distance(position, buildingPosition);
			if(distance > size)
				size = distance / 2;
			
			count++;
		}
		
		return true;
	}
	
	
	bool FindBuildings(out vector position, out float size, int minBuildingCount, int maxBuildingCount, float maxBuildingDistance)
	{
		Print("GRAY_RouletteManager.FindCaptureObjective");
		BaseWorld world = GetGame().GetWorld();
		
		protected vector worldMin;
		protected vector worldMax;
		world.GetBoundBox(worldMin, worldMax);

		float randomX = random.RandFloatXY(worldMin[0], worldMax[0]);
		float randomZ = random.RandFloatXY(worldMin[2], worldMax[2]);	
		float randomY = world.GetSurfaceY(randomX, randomZ);
		vector randomPosition = Vector(randomX, randomY, randomZ);
		
		//Check for buildings
		m_buildingList.Clear();
		GetGame().GetWorld().QueryEntitiesBySphere(randomPosition, m_searchDistance, BuildingFilter, null, EQueryEntitiesFlags.STATIC);
		if(m_buildingList.IsEmpty())
			return false;
		
		ref IEntity objBuilding = m_buildingList.GetRandomElement();
		position = objBuilding.GetOrigin();
		
		m_buildingList.Clear();
		GetGame().GetWorld().QueryEntitiesBySphere(objBuilding.GetOrigin(), maxBuildingDistance, BuildingFilter, null, EQueryEntitiesFlags.STATIC);
		if(m_buildingList.Count() < minBuildingCount)
			return false;
		
		int buildingCount = m_buildingList.Count();
		size = 0;
		int count;
		foreach(IEntity building : m_buildingList)
		{
			if(count > maxBuildingCount)
				break;

			vector buildingPosition = building.GetOrigin();
			
			float distance = vector.Distance(position, buildingPosition);
			if(distance > size)
				size = distance / 2;
			
			count++;
		}
		
		return true;
	}
	
	bool FindEmptyPosition(out vector outPosition, vector areaCenter, float areaRadius, float sphereRadius)
	{
		//Print("GRAY_RouletteManager.FindEmptyPosition");
		//--- Incorrect params
		if (areaRadius <= 0 || sphereRadius <= 0)
		{
			outPosition = areaCenter;
			return false;
		}

		BaseWorld world = GetGame().GetWorld();

		//--- Precalculate vars
		float cellW = sphereRadius * Math.Sqrt(3);
		float cellH = sphereRadius * 2;
		int rMax = Math.Ceil(areaRadius / sphereRadius / Math.Sqrt(3));

		TraceParam trace = new TraceParam();
		trace.Flags = TraceFlags.ENTS | TraceFlags.OCEAN | TraceFlags.WORLD;
		vector traceOffset = Vector(0, 10, 0);

		float posX, posY;
		int yMin, yMax, yStep;
		float traceCoef;
		for (int r; r < rMax; r++)
		{
			for (int x = -r; x <= r; x++)
			{
				posX = cellW * x;
				posY = cellH * (x - SCR_Math.fmod(x, 1)) * 0.5;

				yMin = Math.Max(-r - x, -r);
				yMax = Math.Min(r - x, r);
				if (Math.AbsInt(x) == r)
					yStep = 1;
				else
					yStep = yMax - yMin;

				for (int y = yMin; y <= yMax; y += yStep)
				{
					outPosition = areaCenter + Vector(posX, 0, posY + cellH * y);
					if (vector.DistanceXZ(outPosition, areaCenter) > areaRadius - sphereRadius)
						continue;
					
					//--- Find nearest surface below (make sure it's not underground)
					outPosition[1] = world.GetSurfaceY(outPosition[0], outPosition[2]);
				
					isEmpty = true;
					world.QueryEntitiesBySphere(outPosition, sphereRadius, EmptyFilter, null, EQueryEntitiesFlags.ALL);
					if (isEmpty)
					{
						if(IsAboveWater(outPosition))
							continue;
						Print("GRAY_RouletteManager.FindEmptyPosition found! - " + outPosition);
						return true;
					}
				}
			}
		}
		outPosition = areaCenter;
		return false;
	}
	
	bool isEmpty = true;
	protected bool EmptyFilter(IEntity entity)
	{
		if (!entity)
			return true;

		if(!entity.GetPhysics())
		{
			return true;
		}
		
		isEmpty = false;
		
		return false;
	}
	
	protected bool BuildingFilter(IEntity entity)
	{
		if (!entity)
			return false;
		
		if(entity.GetRootParent() != entity)
			return true;
		
		if(Building.Cast(entity))
		{
			EntityPrefabData buildingPrefab = entity.GetPrefabData();

			if(m_buildingBlackList.Contains(buildingPrefab.GetPrefabName()))
				return true;
			
			foreach(string needle : m_buildingBlackListKeyword)
			{
				string name = buildingPrefab.GetPrefabName();
				if(name.Contains(needle))
					return true;
			}

			m_buildingList.Insert(entity);
		}

		return true;
	}
	
	bool IsWaterOnLine(vector position1, vector position2, int stepSize, int stepFailCount)
	{
		vector direction = vector.Direction(position1, position2).Normalized();
		float distance = vector.Distance(position1, position2);
		BaseWorld world = GetGame().GetWorld();
		
		float stepDistance = stepSize;
		int failCount = 0;
		while(stepDistance < distance && failCount < stepFailCount )
		{
			vector position = position1 + direction * stepDistance;
			position[1] = world.GetSurfaceY(position[0], position[2]);
			
			bool isWater = IsAboveWater(position);
			if(isWater)
				failCount++;
			
			stepDistance += stepSize;
		}
		
		if(failCount >= stepFailCount)
			return true;
		
		return false;
	}
	
	bool IsAboveWater(vector position)
	{
		//Print("GRAY_RouletteManager.IsAboveWater");
		//Add to Y vector so trace works properly
		position[1] = position[1] + 0.1;
		
		TraceParam param = new TraceParam;
		param.Start = position;
		param.End = position + (vector.Up * -1000);
		param.LayerMask = EPhysicsLayerDefs.Projectile;
		param.Flags = TraceFlags.ANY_CONTACT | TraceFlags.WORLD | TraceFlags.OCEAN | TraceFlags.ENTS;

		float percent = GetGame().GetWorld().TraceMove(param, null);
		if(param.SurfaceProps.GetName().Contains("water"))
			return true;
		
		return false;
	}
	
	bool IsCloserTo(float x, float y, float target)
	{
		return Math.AbsFloat(x - target) < Math.AbsFloat(y - target);
	}
	
	IEntity SpawnPrefab(ResourceName prefab, vector position)
	{
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.Transform[3] = position;
		IEntity entity = GetGame().SpawnEntityPrefab(Resource.Load(prefab), null, spawnParams);
		return entity;
	}
}