static ref KeyCard_Base_System g_Keycard;
static KeyCard_Base_System GetKeyCard()
{
    if ( !g_Keycard )
         g_Keycard = new KeyCard_Base_System();
    
    return g_Keycard;
}

class KeyCard_Base_System
{	
	ref KeyCardSettings m_KeyCardSettings;
	ref array< ref KeyCardLockedLocations > DoorLockedLocations;
	
	protected ref Timer KeyCardClosure;
	protected ref Timer MappingTimer;
	protected ref Timer StartupTimer;

	void ~KeyCard_Base_System()
	{	
	
	}

	KeyCardSettings GetConfig()
	{
		return m_KeyCardSettings;
	}
	
	void KeyCard_Base_System()
	{
		m_KeyCardSettings = KeyCardSettings.Load();

		//TestGenerateRewardBarrelHolesBlue();
		
		DoorLockedLocations = new ref array< ref KeyCardLockedLocations >;
			
		MappingTimer = new Timer();
		StartupTimer = new Timer();
			
		if (m_KeyCardSettings.ServerUsingCustomMapping)
		{
			Init_RunCardTimer(/*m_KeyCardSettings.MinimumRestockTime, */m_KeyCardSettings.ServerUsingCustomMapping);
		}
		else
		{
			StartupTimer.Run(30, this, "InitKeyCardSystem", NULL, false);
		}
	}

/* 	void GenerateRewards(array<ref KeyCardDoorContainer> containers)
	{
		foreach(KeyCardDoorContainer container: containers)
		{
			GenerateReward(container);
		}
	} */

	void GenerateRewards(array<ref KeyCardDoorContainer> containers, bool containersOnly)
	{
		if (containersOnly && m_KeyCardSettings.enableRandomContainerRespawn) // Jeśli kontenery tylko i respawn losowy włączony
		{
			int maxContainers = m_KeyCardSettings.maxRandomContainers; // Odczytujemy limit z konfiguracji
			array<int> availableIndices = new array<int>;

			// Tworzymy listę indeksów kontenerów do losowego wyboru
			for (int i = 0; i < containers.Count(); i++)
			{
				availableIndices.Insert(i);
			}

			int spawnedCount = 0;
			while (spawnedCount < maxContainers && availableIndices.Count() > 0)
			{
				int randomIndex = Math.RandomInt(0, availableIndices.Count());
				KeyCardDoorContainer selectedContainer = containers.Get(availableIndices.Get(randomIndex));
				GenerateReward(selectedContainer);

				// Usuwamy użyty indeks
				availableIndices.Remove(randomIndex);
				spawnedCount++;
			}
		}
		else
		{
			// Jeśli containersOnly == false lub respawn losowy jest wyłączony
			foreach (KeyCardDoorContainer containerToSpawn : containers)
			{
				GenerateReward(containerToSpawn);
			}
		}
	}

	void TestGenerateRewardBarrelHolesBlue()
	{
		Print("=== Starting TestGenerateRewardBarrelHolesBlue ===");
		
		// Create test container data
		KeyCardDoorContainer testContainer = new KeyCardDoorContainer();
		testContainer.className = "BarrelHoles_Blue";
		testContainer.position = "7500 300 7500"; // Example position
		testContainer.orientation = Vector(0, 0, 0);
		testContainer.lootPoolName = "Stash"; // Assuming this is the correct pool name
		testContainer.lootMin = 3;
		testContainer.lootMax = 5;
		
		Print(string.Format("Test Container Setup - Class: %1, Position: %2, LootPool: %3", 
			testContainer.className, testContainer.position, testContainer.lootPoolName));
		
		// Verify loot pool exists before generating
		KeyCardLootPool testLootPool = m_KeyCardSettings.GetLootPoolFromName(testContainer.lootPoolName);
		if (!testLootPool)
		{
			Print("ERROR: LootPool not found - Test cannot continue!");
			return;
		}
		
		Print(string.Format("LootPool found - Contains %1 possible rewards", testLootPool.rewards.Count()));
		
		// Generate the reward
		Print("Generating rewards...");
		GenerateReward(testContainer);
		
		Print("=== TestGenerateRewardBarrelHolesBlue Complete ===");
	}

	void GenerateReward(KeyCardDoorContainer container)
	{
		int spawnedLoot = 0;

		Print(string.Format("[DEBUG] Loot pool check for: %1", container.lootPoolName));
		KeyCardLootPool lootPool = m_KeyCardSettings.GetLootPoolFromName(container.lootPoolName);
		if (!lootPool || lootPool.rewards.Count() == 0) {
			Print(string.Format("[ERROR] Invalid or empty loot pool: %1", container.lootPoolName));
			return;
		}

		Print(string.Format("[DEBUG] Attempting to create container: %1 at position: %2", container.className, container.position));
		EntityAI containerBox = EntityAI.Cast(GetGame().CreateObjectEx(container.className, container.position, ECE_SETUP | ECE_CREATEPHYSICS | ECE_NOLIFETIME));

		if (!containerBox) {
			Print(string.Format("[ERROR] Failed to create container of class %1 at position %2", container.className, container.position));
			return;
		}

		containerBox.SetPosition(container.position);
		containerBox.SetOrientation(container.orientation);
		containerBox.Update();

		Container_Base containerBase = Container_Base.Cast(containerBox);
		if (containerBase) {
			containerBase.Open();
		}

		// Determine loot count
		int spawnCount = Math.Max(1, Math.RandomIntInclusive(container.lootMin, container.lootMax));
		Print(string.Format("Generating between %1 and %2 items. Selected count: %3", container.lootMin, container.lootMax, spawnCount));

		int attempts = 0; // Licznik prób ogólnych
		while (spawnedLoot < spawnCount && attempts < spawnCount * 3) { // Maksymalnie 3 próby na przedmiot
			float totalChance = 0;
			foreach (KeyCardItem items : lootPool.rewards) {
				totalChance += items.chanceToSpawn;
			}

			if (totalChance <= 0) {
				Print("[ERROR] Total spawn chance is zero. Skipping loot generation.");
				break;
			}

			float roll = Math.RandomFloatInclusive(0, totalChance);
			float cumulativeChance = 0;

			foreach (KeyCardItem item : lootPool.rewards) {
				cumulativeChance += item.chanceToSpawn;

				if (roll <= cumulativeChance) {
					int ece = ECE_SETUP;
					if (g_Game.IsClient()) {
						ece |= ECE_LOCAL;
					}

					vector newPosition = Vector(0, -100, 0);
					EntityAI itemToSpawn = EntityAI.Cast(GetGame().CreateObjectEx(item.name, newPosition, ece));

					if (itemToSpawn) {
						int quantity = Math.RandomIntInclusive(item.quantityMin, item.quantityMax);
						itemToSpawn = KeyCardHelper.SetQuantity(itemToSpawn, quantity);
						itemToSpawn.Update();

						Print(string.Format("Spawned item: %1 with quantity: %2 at position %3", item.name, quantity, newPosition));

						Weapon_Base wpn = Weapon_Base.Cast(itemToSpawn);
						foreach (KeyCardAttachment attachment : item.attachments) {
							EntityAI attach = EntityAI.Cast(GetGame().CreateObjectEx(attachment.name, newPosition, ece));
							if (!attach) {
								Print(string.Format("Failed to create attach: %1", attachment.name));
								continue;
							}

							if (attachment.IsInherited(Magazine) && !attachment.IsInherited(Ammunition_Base)) {
								wpn.SpawnAttachedMagazine(attachment.name);
								Print(string.Format("Mag %1 has been attached to weapon", attachment.name));
								GetGame().ObjectDelete(attach);
							} else {
								itemToSpawn.GetInventory().CreateAttachment(attachment.name);
								Print(string.Format("Attachment %1 has been attached to item", attachment.name));
								GetGame().ObjectDelete(attach);
							}
						}

						itemToSpawn.Update();
						containerBox.GetInventory().AddEntityToInventory(itemToSpawn);
						spawnedLoot++;
						break; // Wyjście po udanym wygenerowaniu przedmiotu
					} else {
						Print(string.Format("[ERROR] Failed to create item: %1", item.name));
					}
				}
			}

			attempts++;
		}

		Print(string.Format("Generated %1 items in container %2", spawnedLoot, container.className));

		containerBox.SetPosition(container.position);
		containerBox.SetOrientation(container.orientation);
		containerBox.Update();

		if (containerBase) {
			containerBase.Close();
		}
	}
	
	void UpdateDoor(bool should_remove, int index)
	{
		for (int i = 0; i <  m_KeyCardSettings.KeyCard_Locations.Count(); i++)
		{
			if (i == index)
			{
				Print("[Keycard] Deleting Door " + i + " " + m_KeyCardSettings.KeyCard_Locations.Get(i).Location_Name);
				
				string keycard_location_names = m_KeyCardSettings.KeyCard_Locations.Get(i).BuildingClassName;
					
				vector door_locations;
						
				door_locations[0] = m_KeyCardSettings.KeyCard_Locations.Get(i).x;
				door_locations[1] = m_KeyCardSettings.KeyCard_Locations.Get(i).y;
				door_locations[2] = m_KeyCardSettings.KeyCard_Locations.Get(i).z;
						
				vector building_locations;
					
				building_locations[0] = m_KeyCardSettings.KeyCard_Locations.Get(i).building_x;
				building_locations[1] = m_KeyCardSettings.KeyCard_Locations.Get(i).building_y;
				building_locations[2] = m_KeyCardSettings.KeyCard_Locations.Get(i).building_z;
					
				Building building;
		
				array<Object> objects = new array<Object>;
				array<CargoBase> proxy = new array<CargoBase>;
				
				GetGame().GetObjectsAtPosition(door_locations, 5, objects, proxy);	
			
				for ( int o = 0; o < objects.Count(); o++ ) 
				{
					Object obj = objects.Get(o);
										
					if (obj.IsTree() || obj.IsRock() || obj.IsBush() || obj.IsInventoryItem())
						continue;
																						
					if (obj.GetType() == m_KeyCardSettings.KeyCard_Locations.Get(i).BuildingClassName)
					{
						obj.CastTo(building, obj);	
							
						if (building && building.GetPosition() == building_locations)
						{
							if (building.IsDoorLocked(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index) && !building.IsDoorOpen(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index))
							{
								building.UnlockDoor(building.IsDoorOpen(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index));
								building.OpenDoor(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index);
							}
							
							building.OpenDoor(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index);
						}										
					}
				}
			
				//remove the index door!
				m_KeyCardSettings.KeyCard_Locations.Remove(i);
				
				JsonFileLoader<KeyCardSettings>.JsonSaveFile( PATH_Keycards, m_KeyCardSettings );
				
				//Refresh our config live!
				RefreshCFG();
			}		
		}
	}
	
	void EnableDebugBuild_Svr(bool should_enable)
	{
		if (GetGame().IsServer() && !GetGame().IsClient())
		{
			if ( FileExist( PATH_Keycards ) ) // we check if the file exists if it does not we create inside of Init_Config
			{
				KeyCardSettings m_KeyCardSettings;
								
				m_KeyCardSettings = GetKeyCard().m_KeyCardSettings.Load();
								
				if (m_KeyCardSettings)
				{				
					m_KeyCardSettings.debug_build = should_enable;
					JsonFileLoader<KeyCardSettings>.JsonSaveFile( PATH_Keycards, m_KeyCardSettings );	
					
					RefreshCFG();
				}
			}
		}
	}
	
	void UpdateCFG(bool using_custom_mapping, /*bool automatic_relock, int min_unlock_time, int max_unlock_time, int min_restock_main_time, int max_restock_main_time,*/bool debug_build, bool admin_log)
	{
		if (GetGame().IsServer() && !GetGame().IsClient())
		{
			Print("[Keycard] Updating CFG!");
			
			if ( FileExist( PATH_Keycards ) ) // we check if the file exists if it does not we create inside of Init_Config
			{
				KeyCardSettings m_KeyCardSettings;
				m_KeyCardSettings = GetKeyCard().m_KeyCardSettings.Load();
								
				if (m_KeyCardSettings)
				{
					//set the received values to the server for the client to receive
					m_KeyCardSettings.ServerUsingCustomMapping = using_custom_mapping;
					//m_KeyCardSettings.ServerAutomaticRelock = automatic_relock;
					/*m_KeyCardSettings.MinimumUnlockTime = min_unlock_time;
					m_KeyCardSettings.MaximumUnlockTime = max_unlock_time;
					m_KeyCardSettings.MinimumRestockTime = min_restock_main_time;
					m_KeyCardSettings.MaximumRestockTime = max_restock_main_time;*/
					m_KeyCardSettings.debug_build = debug_build;
					m_KeyCardSettings.admin_log = admin_log;

					//save the config values :)
					JsonFileLoader<KeyCardSettings>.JsonSaveFile( PATH_Keycards, m_KeyCardSettings );
										
					//Reload the config values to update on the server
					RefreshCFG();
				}		
			}
		
		}
	}
	
	void RefreshCFG()
	{
		if (GetGame().IsServer() && !GetGame().IsClient())
		{
			m_KeyCardSettings = KeyCardSettings.Load();
		}
	}
	
	void Init_RunCardTimer(/*int time, */bool using_custom_mapping)
	{
		if (using_custom_mapping) //we run this timer 30 seconds after the server has started to force lock the door
			MappingTimer.Run(1 * 30, this, "InitKeyCardSystem", NULL, false);
	}
	
	void ForceOpenDoor(int selected_index)
	{
		for (int i = 0; i <  m_KeyCardSettings.KeyCard_Locations.Count(); i++)
		{
			if (i == selected_index)
			{
				Print("[Keycard] Forced Door " + i + " " + m_KeyCardSettings.KeyCard_Locations.Get(i).Location_Name);
				
				string keycard_location_names = m_KeyCardSettings.KeyCard_Locations.Get(i).BuildingClassName;
					
				vector door_locations;
						
				door_locations[0] = m_KeyCardSettings.KeyCard_Locations.Get(i).x;
				door_locations[1] = m_KeyCardSettings.KeyCard_Locations.Get(i).z;
				door_locations[2] = m_KeyCardSettings.KeyCard_Locations.Get(i).y;
						
				vector building_locations;
					
				building_locations[0] = m_KeyCardSettings.KeyCard_Locations.Get(i).building_x;
				building_locations[1] = m_KeyCardSettings.KeyCard_Locations.Get(i).building_z;
				building_locations[2] = m_KeyCardSettings.KeyCard_Locations.Get(i).building_y;
					
				Building building;
		
				array<Object> objects = new array<Object>;
				array<CargoBase> proxy = new array<CargoBase>;
				
				GetGame().GetObjectsAtPosition(door_locations, 5, objects, proxy);	
			
				for ( int o = 0; o < objects.Count(); o++ ) 
				{
					Object obj = objects.Get(o);
										
					if (obj.IsTree() || obj.IsRock() || obj.IsBush() || obj.IsInventoryItem())
						continue;
																						
					if (obj.GetType() == m_KeyCardSettings.KeyCard_Locations.Get(i).BuildingClassName)
					{
						obj.CastTo(building, obj);	
							
						if (building && building.GetPosition() == building_locations)
						{									
							if (building.IsDoorLocked(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index) && !building.IsDoorOpen(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index))
							{
								building.UnlockDoor(building.IsDoorOpen(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index));
								building.OpenDoor(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index);
							}
							
							building.OpenDoor(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index);
						}										
					}
				}
			}		
		}
	}

	void CloseDoor(KeyCardLocations doorLocation)
	{
		Print("CloseDoor");
		if(!doorLocation)
		{
			Print("!doorLocation");
			return;
		}
			
		Print("doorLocation: " + doorLocation.GetBuildingPosition());
		KeyCardHelper.TeleportAnyPlayerCloseToDoor(doorLocation.GetDoorsPosition());

		Building building;
		array<Object> objects = new array<Object>;
		array<CargoBase> proxy = new array<CargoBase>;
		GetGame().GetObjectsAtPosition(doorLocation.GetBuildingPosition(), 30, objects, proxy);	
		Print("objects count: " + objects.Count());
		for ( int o = 0; o < objects.Count(); o++ ) 
		{
			Object obj = objects.Get(o);
								
			if (obj.IsTree() || obj.IsRock() || obj.IsBush() || obj.IsInventoryItem())
				continue;
														
			if (obj.GetType() == doorLocation.BuildingClassName && vector.Distance(obj.GetPosition(),doorLocation.GetBuildingPosition()) < 1)
			{	
				obj.CastTo(building, obj);	
							
				Print("[Keycard] Locking door");
				//if (m_KeyCardSettings.disable_building_damage_manually)
				building.SetAllowDamage(false);
						
				vector door_sound_pos;
						
				door_sound_pos[0] = building.GetDoorSoundPos(doorLocation.door_index)[0];
				door_sound_pos[1] = building.GetDoorSoundPos(doorLocation.door_index)[1];
				door_sound_pos[2] = building.GetDoorSoundPos(doorLocation.door_index)[2];
						
				if (building.IsDoorOpen(doorLocation.door_index))
					building.CloseDoor(doorLocation.door_index);
									
				if (!building.IsDoorLocked(doorLocation.door_index) && !building.IsDoorOpen(doorLocation.door_index))
					building.LockDoor(doorLocation.door_index);
																															
				DeleteContainers(doorLocation.containers);
										
				building.SetSynchDirty();

				//we insert the locked door 
				DoorLockedLocations.Insert(new KeyCardLockedLocations(building.GetDoorSoundPos(doorLocation.door_index)));
			}
		}
	}

	void DeleteContainers(array<ref KeyCardDoorContainer> containers)
	{
		foreach(KeyCardDoorContainer container: containers)
		{
			DeleteContainer(container);
		}
	}

	void DeleteContainer(KeyCardDoorContainer container)
	{
		array<Object> objects_crate = new array<Object>;
		array<CargoBase> proxy_crate = new array<CargoBase>;										
		GetGame().GetObjectsAtPosition(container.position, 5, objects_crate, proxy_crate);								
		for ( int v = 0; v < objects_crate.Count(); v++ ) 
		{ 
			Object obj_c = objects_crate.Get(v);										
			if (obj_c && obj_c.GetType() == container.className)
			{
				GetGame().ObjectDelete(obj_c);
				Print("Deleting Loot Crate Object Already Exists");
			}
		}
	}

	/*void TeleportAnyPlayerCloseToDoor(vector position)
	{
		Print("TeleportAnyPlayerCloseToDoor");
		array<Man> players = new array<Man>;
		GetGame().GetPlayers( players );
					
		for ( int p = 0; p < players.Count(); p++ )
		{
			PlayerBase player;
			Class.CastTo(player, players.Get(p));
						
			if ( player && player.IsAlive())
			{
				float distance = vector.Distance(position, player.GetPosition());
				if(distance < 5)
				{
					Print("teleport player away");
					vector playerPosition = KeyCardHelper.GetRandomPos(position, 7);
					player.SetPosition(playerPosition);
				}
			}
		}
	}*/

/* 	void TeleportAnyPlayerCloseToDoor(vector position)
	{
		array<Object> objects = new array<Object>;
		array<CargoBase> proxy = new array<CargoBase>;

		GetGame().GetObjectsAtPosition(position, 5, objects, proxy);
		for (int v = 0; v < objects.Count(); v++) 
		{ 
			PlayerBase player;
			if (Class.CastTo(player, objects.Get(v))) // Tylko gracze
			{
				Print("[Keycard] Teleporting player near restricted door.");
				vector playerPosition = KeyCardHelper.GetRandomPos(position, 7);
				player.SetPosition(playerPosition);
			}
		}
	} */
	
	void InitKeyCardSystem()
	{			
		if (DoorLockedLocations.Count() > 0)
			DoorLockedLocations.Clear();
								
		for ( int i = 0; i < m_KeyCardSettings.KeyCard_Locations.Count(); i++ ) 
		{
			string keycard_location_names = m_KeyCardSettings.KeyCard_Locations.Get(i).BuildingClassName;
			
			vector door_locations;
			vector building_locations;
			
			door_locations[0] = m_KeyCardSettings.KeyCard_Locations.Get(i).x;
			door_locations[1] = m_KeyCardSettings.KeyCard_Locations.Get(i).z;
			door_locations[2] = m_KeyCardSettings.KeyCard_Locations.Get(i).y;
				
			building_locations[0] = m_KeyCardSettings.KeyCard_Locations.Get(i).building_x;
			building_locations[1] = m_KeyCardSettings.KeyCard_Locations.Get(i).building_z;
			building_locations[2] = m_KeyCardSettings.KeyCard_Locations.Get(i).building_y;
			
			Building building;

			array<Object> objects = new array<Object>;
			array<CargoBase> proxy = new array<CargoBase>;
		
			GetGame().GetObjectsAtPosition(building_locations, 50, objects, proxy);	
	
			for ( int o = 0; o < objects.Count(); o++ ) 
			{
			   Object obj = objects.Get(o);
								
				if (obj.IsTree() || obj.IsRock() || obj.IsBush() || obj.IsInventoryItem())
					continue;
						
				if (obj.GetType() == m_KeyCardSettings.KeyCard_Locations.Get(i).BuildingClassName)
				{
					Print(obj.GetPosition());
					Print(building_locations);
				}
														
				if (obj.GetType() == m_KeyCardSettings.KeyCard_Locations.Get(i).BuildingClassName && obj.GetPosition() == building_locations)
				{	
					obj.CastTo(building, obj);	
					
					bool should_lock = true;
					
					array<Man> players = new array<Man>;
					GetGame().GetPlayers( players );
					
					for ( int p = 0; p < players.Count(); p++ )
					{
						PlayerBase player;
						Class.CastTo(player, players.Get(p));
						
						if ( player && player.IsAlive())
						{
							float distance = vector.Distance(door_locations, player.GetPosition());
							
							Print(distance);
							
							if (distance < 5.0)
							{
								Print("[Keycard] Skipping Door due to players inside or close to the door! " + m_KeyCardSettings.KeyCard_Locations.Get(i).Location_Name);

								should_lock = false;
							
								if (building && building.GetPosition() == building_locations)
								{
								   	DoorLockedLocations.Insert(new KeyCardLockedLocations(building.GetDoorSoundPos(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index)));
								}
							}
								
						}
					}
							
					Print("[Keycard] Locking Door Code");
						
					if (building && building.GetPosition() == building_locations)
					{
						if (should_lock)
						{
							if (m_KeyCardSettings.KeyCard_Locations.Get(i).containers_only)
							{
								DeleteContainers(m_KeyCardSettings.KeyCard_Locations.Get(i).containers);
								/*if (Math.RandomFloat01() > 1) {
									continue;
								}*/
								GenerateRewards(m_KeyCardSettings.KeyCard_Locations.Get(i).containers, true);
							}
							else
							{
								vector door_sound_pos;
								
								door_sound_pos[0] = building.GetDoorSoundPos(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index)[0];
								door_sound_pos[1] = building.GetDoorSoundPos(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index)[1];
								door_sound_pos[2] = building.GetDoorSoundPos(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index)[2];

								//if (m_KeyCardSettings.disable_building_damage_manually)
								building.SetAllowDamage(false);

								if (building.IsDoorOpen(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index))
									building.CloseDoor(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index);
								
								if (!building.IsDoorLocked(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index) && !building.IsDoorOpen(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index))
								{
									building.LockDoor(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index);
								}

								DeleteContainers(m_KeyCardSettings.KeyCard_Locations.Get(i).containers);
									
								building.SetSynchDirty();

								//we insert the locked door 
								DoorLockedLocations.Insert(new KeyCardLockedLocations(building.GetDoorSoundPos(m_KeyCardSettings.KeyCard_Locations.Get(i).door_index)));
							}
						}
						else 
						{
							Print("[Keycard] Should_Lock = False, skipping door!");
						}
					}
				}
			}
		}
	}	
};