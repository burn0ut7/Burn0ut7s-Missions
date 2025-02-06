enum GRAY_eInfantryType
{
	Company,
	Platoon,
	Squad
}

enum GRAY_eScopeType
{
	None,
	Some,
	All
}
 
[BaseContainerProps()]
class GRAY_RouletteTeamData
{
	[Attribute("", UIWidgets.EditBox)]
	protected string name;

	[Attribute("", UIWidgets.EditBox, desc: "What is this teams faction key in the faction manager?")]
	protected FactionKey factionKey;
	
	[Attribute("", UIWidgets.ComboBox, desc: "Does this team have scopes? Teams will only vs other teams with same scope type.", "", ParamEnumArray.FromEnum(GRAY_eScopeType))]
	protected GRAY_eScopeType scopeType;
	
	[Attribute("", UIWidgets.Auto)]
	protected int platoonsPerCompany;
	
	[Attribute("", UIWidgets.Auto)]
	protected int squadsPerPlatoon;
	
	[Attribute(defvalue: "", UIWidgets.Object)]
    protected ref array<ref GRAY_RouletteCompany> companies;

	string GetName()
	{
		return name; 
	}
	
	FactionKey GetFaction()
	{
		return factionKey;
	}

	GRAY_eScopeType GetScopeType()
	{ 
		return scopeType; 
	
	}

	array<ref GRAY_RouletteCompany> GetCompanies()
	{
		return companies;
	}
}

[BaseContainerProps()]
class GRAY_RouletteCompany : GRAY_RouletteSquad
{
	[Attribute(defvalue: "", UIWidgets.Object)]
    protected ref array<ref GRAY_RoulettePlatoon> platoons;
	
	void GRAY_RouletteCompany()
	{
		foreach(GRAY_RoulettePlatoon platoon : platoons)
		{
			totalCount += platoon.GetTotalCount();
		}

		Print("GRAY_RouletteCompany count = " + GetTotalCount());
	}
	
	array<ref GRAY_RoulettePlatoon> GetPlatoons()
	{
		return platoons;
	}
}

[BaseContainerProps()]
class GRAY_RoulettePlatoon : GRAY_RouletteSquad
{
	[Attribute(defvalue: "", UIWidgets.Object)]
    protected ref array<ref GRAY_RouletteSquad> squads;
	
	void GRAY_RoulettePlatoon()
	{
		foreach(GRAY_RouletteSquad squad : squads)
		{
			totalCount += squad.GetTotalCount();
		}
		
		Print("GRAY_RoulettePlatoon count = " + GetTotalCount());
	}
	
	array<ref GRAY_RouletteSquad> GetSquads()
	{
		return squads;
	}
}

[BaseContainerProps()]
class GRAY_RouletteSquad
{
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Prefab to spawn in for this element.")]
	protected ResourceName prefab;
	
	[Attribute("", UIWidgets.EditBox, desc: "How many players are ONLY in this element prefab above?")]
	protected int playerCount;
	
	protected int totalCount = 0;
	
	void GRAY_RouletteSquad()
	{
		totalCount += playerCount;
	}
	ResourceName GetPrefab()
	{
		return prefab;
	}
	
	int GetPlayerCount()
	{
		return playerCount;
	}
	
	int GetTotalCount()
	{
		return totalCount;
	}
}

/*
enum GRAY_EAssetType
{
	Ground,
	Air
	//Support?
}

[BaseContainerProps()]
class GRAY_RouletteAssetsData
{
	[Attribute("", UIWidgets.EditBox)]
	protected string name;

	[Attribute("", UIWidgets.EditBox, desc: "At the very least spawn this many")]
	protected int minSpawn;
	
	[Attribute("", UIWidgets.EditBox, desc: "Ratio per player. IE 10 = forvery 10 players spawn 1 asset prefab")]
	protected int ratioToPlayers;
	
	[Attribute("", UIWidgets.ComboBox, desc: "Does this team have scopes? Teams will only vs other teams with scopes.", "", ParamEnumArray.FromEnum(GRAY_EAssetType))]
	protected GRAY_EAssetType assetType;
	
	[Attribute("", UIWidgets.EditBox, desc: "How many players are in this prefab?")]
	protected int playerCount;
	
	[Attribute("", UIWidgets.Coords, desc: "Offset to the infantry spawn position")]
	protected vector offsetRelativeToInfantry;
	
	[Attribute("", UIWidgets.Coords, desc: "Offset for each following prefab spawn")]
	protected vector offsetRelativeToPrefab;
	
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "The prefab to spawn in.")]
	protected ResourceName assetPrefab;
	
	
	string GetName() { return name; }
}
*/