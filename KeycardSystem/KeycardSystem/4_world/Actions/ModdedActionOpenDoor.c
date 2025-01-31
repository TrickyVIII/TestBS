modded class ActionOpenDoors: ActionInteractBase
{
	override void OnStartServer( ActionData action_data )
	{
		Building building;
		if( Class.CastTo(building, action_data.m_Target.GetObject()) )
		{
			int doorIndex = building.GetDoorIndex(action_data.m_Target.GetComponentIndex());
			if( doorIndex != -1 )
			{
				if (GetGame().IsServer() && !GetGame().IsClient() && GetKeyCard())
				{
					if (GetKeyCard().m_KeyCardSettings.debug_build)
					{
						if ( FileExist( PATH_Keycards ) ) // we check if the file exists if it does not we create inside of Init_Config
						{
							KeyCardSettings m_KeyCardSettings = new KeyCardSettings();
		
							m_KeyCardSettings = KeyCardSettings.Load();
								
							if (m_KeyCardSettings)
							{							
								string automatic = "New Door ServerTime " + GetGame().GetTime();
								int min_unlock_time = 180;
								int max_unlock_time = 300;
								int min_restock_time = 1800;
								int max_restock_time = 2700;
								Print("Keycard before add " + m_KeyCardSettings.KeyCard_Locations.Count() + " Locations");
								
								m_KeyCardSettings.KeyCard_Locations.Insert( new KeyCardLocations(building.GetDoorSoundPos(doorIndex)[0], building.GetDoorSoundPos(doorIndex)[2], building.GetDoorSoundPos(doorIndex)[1], building.GetPosition()[0], building.GetPosition()[2],building.GetPosition()[1], doorIndex, false, "", automatic, building.GetType(), "Key_Classname", /*"Alarm_Military",*/ false, min_unlock_time, max_unlock_time, min_restock_time, max_restock_time, /*true,*/ false));	

								JsonFileLoader<KeyCardSettings>.JsonSaveFile( PATH_Keycards, m_KeyCardSettings );
									
								GetKeyCard().RefreshCFG();
							}
								
						}
					}
				}
				
				building.OpenDoor(doorIndex);
			}
		}
	}
}