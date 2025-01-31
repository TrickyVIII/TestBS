class ActionUnlockKeyCardDoor: ActionContinuousBase
{	
	ref KeyCardLocations best_door;
	
	void ActionUnlockKeyCardDoor()
	{		
		m_CallbackClass = ActionUnlockDoorsCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNone;
	}
		
	override string GetText()
	{
		return "#unlock";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{		
		if( !target ) return false;
		if( !IsBuilding(target) ) return false;
		if( !IsInReach(player, target, UAMaxDistances.DEFAULT) ) return false;
		
		Building building;
				
		if ( Class.CastTo(building, target.GetObject()) )
		{
			int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
			
			if ( doorIndex != -1 )
			{
				return building.IsDoorLocked(doorIndex);
			}
		}	
			
		return false;
	}

	/*int GetMinimumUnlockTime()
	{
		return GetKeyCard().GetConfig().MinimumUnlockTime * 1000;
	}
	
	int GetMaximumUnlockTime()
	{
		return GetKeyCard().GetConfig().MaximumUnlockTime * 1000;
	}
	
	int GetRandomUnlockTime()
	{
		return Math.RandomIntInclusive(GetMinimumUnlockTime(), GetMaximumUnlockTime());
	}

	int GetMinimumRestockTime()
	{
		return GetKeyCard().GetConfig().MinimumRestockTime * 1000;
	}
	
	int GetMaximumRestockTime()
	{
		return GetKeyCard().GetConfig().MaximumRestockTime * 1000;
	}
	
	int GetRandomRestockTime()
	{
		return Math.RandomIntInclusive(GetMinimumRestockTime(), GetMaximumRestockTime());
	}*/

	override void OnFinishProgressServer( ActionData action_data )
	{			
		if (!action_data.m_Player)
			return;
		
		//todo, clean this code up currently a "ghetto" way to get closest door
		int closest_number = -1;
		
		//when we walk up to the door we want to check if the door we are at is a "keycard" door this could be optimized but doesn't cause any performance related issues 
		for ( int d = 0; d < GetKeyCard().m_KeyCardSettings.KeyCard_Locations.Count(); d++ )
		{
			vector door_locations;
				
			door_locations[0] = GetKeyCard().m_KeyCardSettings.KeyCard_Locations.Get(d).x;
			door_locations[1] = GetKeyCard().m_KeyCardSettings.KeyCard_Locations.Get(d).z;
			door_locations[2] = GetKeyCard().m_KeyCardSettings.KeyCard_Locations.Get(d).y;
			
			float distance =  Math.Round(vector.Distance(action_data.m_Player.GetPosition(), door_locations));
						
			if (closest_number <= 0)
			{
				closest_number = distance;
				best_door = GetKeyCard().m_KeyCardSettings.KeyCard_Locations.Get(d);
			}
				
			if (distance < closest_number)
			{
				closest_number = distance;
				best_door = GetKeyCard().m_KeyCardSettings.KeyCard_Locations.Get(d);
			}
		}
		
		//Print(closest_number);
							
		HumanInventory inv = action_data.m_Player.GetHumanInventory();
		EntityAI hand = inv.GetEntityInHands();
		
		Building building;
		
		bool is_a_keycard_door = false;

		bool display_message = false;
		bool has_run_unlock = false;

		//Print("Keycard under has_run_unlock");
				
		if ( Class.CastTo(building, action_data.m_Target.GetObject()) )
		{
			int doorIndex = building.GetDoorIndex(action_data.m_Target.GetComponentIndex());
			if ( doorIndex != -1 )
			{							
				if (closest_number <= 3)
				{	
					for ( int c = 0; c < GetKeyCard().DoorLockedLocations.Count(); c++ )
					{	
						if (GetKeyCard().DoorLockedLocations.Get(c).door_pos == building.GetDoorSoundPos(doorIndex))
						{							
							is_a_keycard_door = true;
							
							//this code checks the .json keycard = this would be the classname of the keycard if it is not the same item in the players hand it
							if (hand && hand.GetType() != best_door.keycard_class_name)
							{								
								if (!display_message)
								{			
									Param1<string> m_MessageParam_ = new Param1<string>("This Door Requires a " + best_door.keycard_class_name);
	
									display_message = true;
									
									//prevent spamming of message		
									GetGame().RPCSingleParam(action_data.m_Player, ERPCs.RPC_USER_ACTION_MESSAGE, m_MessageParam_, true, action_data.m_Player.GetIdentity());
					
								    if (best_door.use_alarm)
										building.PlaySound("Sound_Door_Denied", 5, false);
																		
									//do not return here lets just continue
									break;
									
								}
							}
							else
							{
								if (!has_run_unlock)
								{
									array<Object> objects = new array<Object>;
									array<CargoBase> proxy = new array<CargoBase>;
									
									has_run_unlock = true;
											
									GetGame().GetObjectsAtPosition(building.GetDoorSoundPos(doorIndex), 5, objects, proxy);	
									
									//end of comment if you need  this
									//Unlocks the door - Deletes the keycard item
									/*if (best_door.single_use && action_data.m_MainItem) //maybe multi use card
									{
									  	GetGame().ObjectDelete(action_data.m_MainItem);
									}*/
									
									if (action_data.m_MainItem)
									{
									  	GetGame().ObjectDelete(action_data.m_MainItem);
									}

									if (GetKeyCard().m_KeyCardSettings.admin_log)
										Print("[Keycard] SteamID " + action_data.m_Player.GetIdentity().GetPlainId() + " Opened " + best_door.Location_Name + " Using " + best_door.keycard_class_name);

									if (best_door.use_alarm)
										//building.PlaySoundLoop(best_door.alarm_sound, 100, false);
										building.PlaySoundLoop("Alarm_Military", 100, false);
										building.PlaySound("Sound_Door_Accepted", 5, false);
									
									if (best_door.containers)
										GetKeyCard().GenerateRewards(best_door.containers, false);

									int RandomUnlockTime = Math.RandomIntInclusive(best_door.min_unlock_time * 1000,best_door.max_unlock_time * 1000);
									int RandomRestockTime = Math.RandomIntInclusive(best_door.min_restock_time * 1000,best_door.max_restock_time * 1000);

									if (best_door.use_announcement)
									{
										if (best_door.announcement_text == "")
										{
											GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(building.UnlockDoor, RandomUnlockTime, false, doorIndex);
											GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetKeyCard().CloseDoor, RandomRestockTime, false, best_door);
											if (g_Game.IsServer()) {
												LogToCFTools(action_data.m_Player,"",string.Format("used %1 to open door in %2", action_data.m_MainItem.GetType(), action_data.m_Target.GetObject().GetType()),"");
											}
										}
										else
										{
											string text_to_use = best_door.announcement_text;

											NotificationSystem.Create( new StringLocaliser( "Military Alarm" ), new StringLocaliser(text_to_use), "DayZExpansion/Core/GUI/icons/hud/skull_2_64x64.edds", ARGB(255, 221, 38, 38), 10, NULL );

											GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(building.UnlockDoor, RandomUnlockTime, false, doorIndex);
											GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetKeyCard().CloseDoor, RandomRestockTime, false, best_door);
											if (g_Game.IsServer()) {
												LogToCFTools(action_data.m_Player,"",string.Format("used %1 to open door in %2", action_data.m_MainItem.GetType(), action_data.m_Target.GetObject().GetType()),"");
											}
										}
										//has_run_unlock = true;
									}
									else
									{
										building.UnlockDoor(doorIndex);

										//GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetKeyCard().CloseDoor, GetKeyCard().GetConfig().AutomaticRelockTime*60000, false, best_door);	
										GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetKeyCard().CloseDoor, RandomRestockTime, false, best_door);
										if (g_Game.IsServer()) {
											LogToCFTools(action_data.m_Player,"",string.Format("used %1 to open door in %2", action_data.m_MainItem.GetType(), action_data.m_Target.GetObject().GetType()),"");
										}
										//has_run_unlock = true;
									}
								}
							}
						}
					}
				}		
				
				if (!is_a_keycard_door)
				   building.UnlockDoor(doorIndex);
				
				//Print("Keycard under if (!is_a_keycard_door) building.UnlockDoor(doorIndex) ;");				
			}
		}
		
		//Damage the Lockpick as we override the default lockpick unlock action we need to replicate the code here 
/* 		if (action_data.m_MainItem.GetType() == "Lockpick")
		{
			float dmg = action_data.m_MainItem.GetMaxHealth() * 0.04; //Multiply max health by 'x' amount depending on number of usages wanted (0.04 = 25)
			
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
			
			if (action_data.m_Player.GetSoftSkillsManager().GetSpecialtyLevel() >= 0)
			{
				action_data.m_MainItem.DecreaseHealth("", "", dmg);
				DebugPrint.Log("Damage without soft skill : " + dmg);
			}
			               
			if (action_data.m_Player.GetStatSpecialty().Get() < 0)
			{
				dmg = action_data.m_MainItem.GetMaxHealth() * 0.025; //Multiply by 0.025 to get 40 uses out of pristine lockpick
				action_data.m_MainItem.DecreaseHealth("", "", dmg);
				DebugPrint.Log("Damage with soft skill : " + dmg);
			}	
		} */
	
		//Print("Keycard finished OnFinishProgressServer");
	}
};

static void LogToCFTools(Man player, string item, string target, string action)
{
#ifdef GameLabs
/* 	if (!GetGameLabs() || g_Game.IsClient()) {
		return;
	}
	
	_LogPlayer log_player = new _LogPlayer();

	PlayerBase pb = PlayerBase.Cast(player);
	
	if (!player || !pb) {
		return;
	}

	log_player.position = player.GetPosition();
	log_player.blood =  player.GetHealth("GlobalHealth", "Blood");
	log_player.health = player.GetHealth("GlobalHealth","Health");

	GetGameLabs().GetApi().ItemInteract(new _Callback(), new _Payload_ItemInteract(log_player, item, target, action)); */
        ref _LogPlayerEx logObjectPlayer = new _LogPlayerEx(player);
        action = action + " ";
        ref _Payload_ItemInteract payload = new _Payload_ItemInteract(logObjectPlayer, item, target, action);
        GetGameLabs().GetApi().ItemInteract(new _Callback(), payload);
#endif
}