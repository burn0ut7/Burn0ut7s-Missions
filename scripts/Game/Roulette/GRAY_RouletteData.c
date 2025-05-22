enum GRAY_eInfantryType
{
	Company,
	Platoon,
	Squad
}

enum GRAY_eScopeType
{
	Disregard,
	None,
	Some,
	All
}

enum GRAY_eBriefingType
{
	Any,
	Defender,
	Attacker
}

[BaseContainerProps()]
class GRAY_RouletteBriefingData
{
	[Attribute("", UIWidgets.EditBox, category: "Attack and Defend")]
	protected string title;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.EditBoxMultiline, desc: "Briefing description. %1 = Defender team, %2 = Attacker team, %3 = Defender Element, %4 = Attacker Element", category: "Attack and Defend")]
	protected string description;
	
	[Attribute("0", UIWidgets.ComboBox, desc: "What side will this briefing appear for?", "", ParamEnumArray.FromEnum(GRAY_eBriefingType))]
	protected GRAY_eBriefingType side;

	string GetTitle()
	{
		return title;
	}
	
	string GetDescription()
	{ 
		return description; 
	}
	
	GRAY_eBriefingType GetSide()
	{
		return side;
	}
}

[BaseContainerProps()]
class GRAY_RouletteTeamData
{
	[Attribute("", UIWidgets.EditBox)]
	protected string name;

	[Attribute("", UIWidgets.EditBox, desc: "What is this teams faction key in the faction manager?")]
	protected FactionKey factionKey;
	
	[Attribute("1", UIWidgets.Auto, desc: "Relative strength. 1 = 1980s baseline. Less = weaker, More = stronger")]
	protected float strength;
	
	[Attribute("0", UIWidgets.EditBox, desc: "Use only squad callsigns from the faction manager")]
	protected bool squadCallsignsOnly;
	
	[Attribute("", UIWidgets.Auto, desc: "This team cannot versus these faction keys.")]
    protected ref array<FactionKey> versusBlacklist;
	
	[Attribute(defvalue: "", UIWidgets.Object)]
    protected ref array<ref GRAY_RouletteCompany> companies;
	
	string GetName()
	{
		return name; 
	}
	
	float GetStrength()
	{
		return strength; 
	}
	
	FactionKey GetFaction()
	{
		return factionKey;
	}

	array<ref GRAY_RouletteCompany> GetCompanies()
	{
		return companies;
	}
	
	array<FactionKey> GetBlacklist()
	{
		return versusBlacklist;
	}
	
	bool GetCallsign()
	{
		return squadCallsignsOnly;
	}
}

[BaseContainerProps()]
class GRAY_RouletteCompany : GRAY_RouletteSquad
{
	[Attribute(defvalue: "", UIWidgets.Object)]
    protected ref array<ref GRAY_RoulettePlatoon> platoons;

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

	ResourceName GetPrefab()
	{
		return prefab;
	}
	
	int GetPlayerCount()
	{
		return playerCount;
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