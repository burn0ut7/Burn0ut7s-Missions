class GRAY_MissionDescriptionClass : PS_MissionDescriptionClass
{

}

class GRAY_MissionDescription : PS_MissionDescription
{
	void SetVisibleForFactionKey(FactionKey faction, bool visible)
	{
		RPC_SetVisibleForFaction_ByKey(faction, visible);
		Rpc(RPC_SetVisibleForFaction_ByKey, faction, visible);
	}
	
	void SetShowAnyFaction(bool show)
	{
		RPC_SetOrder(show);
		Rpc(RPC_SetShowAnyFaction, show);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_SetShowAnyFaction(bool show)
	{
		m_bShowForAnyFaction = show;
	}

	void SetOrder(int order)
	{
		RPC_SetOrder(order);
		Rpc(RPC_SetOrder, order);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_SetOrder(int order)
	{
		m_iOrder = order;
	}
	
	override bool RplSave(ScriptBitWriter writer)
	{
		// Pack every changeable variable
		writer.WriteString(m_sTitle);
		writer.WriteString(m_sDescriptionLayout);
		writer.WriteString(m_sTextData);
		writer.WriteBool(m_bEmptyFactionVisibility);
		writer.WriteBool(m_bShowForAnyFaction);
		writer.WriteInt(m_iOrder);
		
		string factions = "";
		foreach (FactionKey factionKey: m_aVisibleForFactions)
		{
			if (factions != "") factions += ",";
			factions += factionKey;
		}
		writer.WriteString(factions);
		
		return true;
	}
	
	override bool RplLoad(ScriptBitReader reader)
	{
		// Unpack every changeable variable
		reader.ReadString(m_sTitle);
		reader.ReadString(m_sDescriptionLayout);
		reader.ReadString(m_sTextData);
		reader.ReadBool(m_bEmptyFactionVisibility);
		reader.ReadBool(m_bShowForAnyFaction);
		reader.ReadInt(m_iOrder);
		
		string factions;
		reader.ReadString(factions);
		GetGame().GetCallqueue().CallLater(FactionsInit, 0, false, factions);
		
		return true;
	}
}

