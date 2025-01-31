class KeyCardSettings 
{	
	bool ServerUsingCustomMapping;
    bool enableRandomContainerRespawn; // Nowe pole: włączenie/wyłączenie losowego respawnu kontenerów
    int maxRandomContainers; // Pole do limitu kontenerów
//	bool ServerAutomaticRelock;
	
/*	int MinimumUnlockTime;
	int MaximumUnlockTime;
	int MinimumRestockTime;
	int MaximumRestockTime;*/
	
	bool debug_build;
	bool admin_log;
	//bool disable_building_damage_manually;
	
	ref array< ref KeyCardLocations > KeyCard_Locations;
	ref array< ref KeyCardLootPool> lootPools;
	
	void KeyCardSettings()
	{
		ServerUsingCustomMapping = false;
		enableRandomContainerRespawn = true;
		maxRandomContainers = 50;
	//	ServerAutomaticRelock = false;
		
/*		MinimumUnlockTime = 300;
		MaximumUnlockTime = 600;
		MinimumRestockTime = 1800;
		MaximumRestockTime = 2700;*/
		
		debug_build = false;
		admin_log = false;
		
		//disable_building_damage_manually = false;
		
		KeyCard_Locations = new array< ref KeyCardLocations >;
		lootPools = new array< ref KeyCardLootPool >;
	}

	KeyCardLootPool GetLootPoolFromName(string name)
	{
		foreach(KeyCardLootPool lootPool: lootPools)
		{
			if(lootPool.lootPoolName == name)
				return lootPool;
		}

		return null;
	}
	
	void Init_Config()
	{	
		if (GetGame().IsServer() && !GetGame().IsClient())
		{								
			KeyCard_Locations.Insert(new KeyCardLocations());
			KeyCard_Locations[0].containers.Insert(new KeyCardDoorContainer());
			KeyCard_Locations[0].containers.Insert(new KeyCardDoorContainer());

			lootPools.Insert(new KeyCardLootPool("Tier 1"));
        	lootPools[0].rewards.Insert(new KeyCardItem("AKM", 0.5, 1, 1));
        	lootPools[0].rewards[0].attachments.Insert(new KeyCardAttachment("Mag_AKM_Drum75Rnd", 1));
        	lootPools[0].rewards[0].attachments.Insert(new KeyCardAttachment("AK_WoodBttstck", 1));
        	lootPools[0].rewards.Insert(new KeyCardItem("AKM", 1.0, 1, 1));
        	lootPools[0].rewards[1].attachments.Insert(new KeyCardAttachment("Mag_AKM_Drum75Rnd", 1));
        	lootPools[0].rewards[1].attachments.Insert(new KeyCardAttachment("UniversalLight", 1));
        	lootPools.Insert(new KeyCardLootPool("Tier 2"));
        	lootPools[1].rewards.Insert(new KeyCardItem("FAMAS", 0.3, 1, 1));
        	lootPools[1].rewards[0].attachments.Insert(new KeyCardAttachment("Mag_FAMAS_25Rnd", 1));
        	lootPools[1].rewards.Insert(new KeyCardItem("SVD", 0.25, 1, 1));
        	lootPools[1].rewards[1].attachments.Insert(new KeyCardAttachment("Mag_SVD_10Rnd", 1));

		
			Print("KeyCard loaded " + KeyCard_Locations.Count() + " Locations");
			
			if (!FileExist(m_ProfileDirectory_Keycards + ConfigDir_Keycards + "/"))
				MakeDirectory(m_ProfileDirectory_Keycards + ConfigDir_Keycards + "/");
						
			JsonFileLoader<KeyCardSettings>.JsonSaveFile( PATH_Keycards, this );
		}
	}

	static ref KeyCardSettings Load()
	{
		if (GetGame().IsServer() && !GetGame().IsClient())
		{
			KeyCardSettings settings = new KeyCardSettings();
	
			if (!FileExist(ConfigDir_Keycards))
           	 	MakeDirectory(ConfigDir_Keycards);
			
			if (FileExist(PATH_Keycards)) {
				JsonFileLoader<KeyCardSettings>.JsonLoadFile(PATH_Keycards, settings);
				foreach (KeyCardLootPool pool : settings.lootPools) {
					Print(string.Format("[DEBUG] Loaded lootPool: %1 with %2 rewards", pool.lootPoolName, pool.rewards.Count()));
				}
			} else {
				Print("[ERROR] Config file does not exist. Initializing default configuration.");
				settings.Init_Config();
			}
	
			return settings;
		}
		
		return null;
	}
};