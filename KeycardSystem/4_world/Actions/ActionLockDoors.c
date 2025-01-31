/* modded class ActionLockDoors: ActionContinuousBase
{
    ref TStringArray KeycardBuilding = {"Land_Tisy_Garages", "Land_Mil_Barracks5", "Land_Mil_Barracks3", "Land_Tisy_HQ", "Land_Airfield_ServiceHangar_R", "Land_City_PoliceStation", "Land_Village_HealthCare", "Land_Garage_Office", "Land_Garage_Row_Small", "Land_Factory_Lathes", "Land_Container_1Mo", "Land_Village_Pub", "Land_Village_PoliceStation", "Land_House_2B04", "Land_Prison_Side", "Land_Mine_Building", "Land_Mil_Airfield_HQ", "Land_Airfield_Small_Control", "Land_Shed_Closed2", "Land_Village_store", "Land_Mil_Guardhouse", "Land_Slum_House1", "Land_FuelStation_Build", "Land_Shed_W6", "Land_Wreck_Caravan_MRust", "Land_Workshop_FuelStation", "Land_Mil_Barracks3", "Land_Village_HealthCare_Enoch", "Land_Container_1Aoh", "Land_Container_1Bo", "Land_Tisy_RadarB_Base", "Land_Mil_Guardhouse1", "Land_Mil_Barracks4", "Land_TunnelParts_Entrance_Complete", "Land_Prison_Main", "Land_Village_PoliceStation_Enoch", "Land_City_PoliceStation_Enoch", "Land_Underground_WaterMaintenance"};

    string m_InteractionText;

    void ActionLockDoors()
	{
		m_InteractionText = "#lock_door";
	}

    override string GetText()
	{
		return m_InteractionText;
	}

    override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if( !target ) return false;
		if( !IsBuilding(target) ) return false;
		if( !IsInReach(player, target, UAMaxDistances.DEFAULT) ) return false;

		Building building;
		if( Class.CastTo(building, target.GetObject()) )
		{
			int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
			if (KeycardBuilding.Find(building.GetType()) != -1)
            {
                m_InteractionText = "You cannot close this door";
                return true;
            }
			else if ( doorIndex != -1 )
			{
				return (!building.IsDoorOpen(doorIndex) && !building.IsDoorLocked(doorIndex));
			}

            m_InteractionText = "#lock_door";
		}
		return false;
	}

    override void OnFinishProgressServer( ActionData action_data )
	{
		Building building;
        Object BuildingObject = Object.Cast( action_data.m_Target.GetObject() );
        Class.CastTo(building, action_data.m_Target.GetObject());

        if (KeycardBuilding.Find(building.GetType()) != -1)
			return;

        super.OnFinishProgressServer(action_data);
	}
}; */

modded class ActionUnlockDoors: ActionContinuousBase {
	const float APPLIED_DMG = 6;
	ref KeyCardLocations best_door;
	override void OnFinishProgressServer( ActionData action_data ) {
		if (!action_data.m_Player)
			return;
		if ( GetKeyCard().m_KeyCardSettings.ServerUsingCustomMapping ) {
			Building building;
			int closest_number = -1;
			PlayerBase player = PlayerBase.Cast(action_data.m_Player);
			HumanInventory inv = action_data.m_Player.GetHumanInventory();
			EntityAI hand = inv.GetEntityInHands();
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
			if ( Class.CastTo(building, action_data.m_Target.GetObject()) ) {
				int doorIndex = building.GetDoorIndex(action_data.m_Target.GetComponentIndex());
				if ( doorIndex != -1 ) {
					for ( int c = 0; c < GetKeyCard().DoorLockedLocations.Count(); c++ ) {
						if (GetKeyCard().DoorLockedLocations.Get(c).door_pos == building.GetDoorSoundPos(doorIndex)) {
							if (hand && hand.GetType() != best_door.keycard_class_name) {
								Param1<string> m_MessageParam_ = new Param1<string>("You cannot close this door..." );
								GetGame().RPCSingleParam(action_data.m_Player, ERPCs.RPC_USER_ACTION_MESSAGE, m_MessageParam_, true, action_data.m_Player.GetIdentity());
								return;
							}
						} else {
							UnlockDoor(action_data.m_Target);
							MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, APPLIED_DMG);
						}
					}
				}
			}
		} else {
			super.OnFinishProgressServer(action_data);
		}
	}
}