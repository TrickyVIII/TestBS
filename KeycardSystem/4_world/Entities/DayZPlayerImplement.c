modded class DayZPlayerImplement
{
	/*KEYCARD DATA*/
	
	ref KeyCardSettings Keycard_CFGData;
	
	ref array<vector> locked_locations;
	
	/*END OF KEYCARD DATA*/
	
	void DayZPlayerImplement()
	{
		Keycard_CFGData = new KeyCardSettings();
		
		locked_locations = new array<vector>;
	}
	
	void ServerRPC_Calls(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{


	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		//Print(rpc_type);
		
		if (GetGame().IsServer() && !GetGame().IsClient())
		{
			if (rpc_type == DAYZ_REFRESH_CONFIG)
			{
				Param3<bool, /*bool, int, int, int, int, */bool, bool> config_data = new Param3<bool, /*bool, int, int, int, int,*/bool, bool>( false, /*false, 0, 0, 0, 0, */false, false);
				ctx.Read( config_data );
	
				GetKeyCard().UpdateCFG(config_data.param1, config_data.param2, config_data.param3/*, config_data.param4, config_data.param5, config_data.param6, config_data.param7, config_data.param8*/);
			}
			
			if (rpc_type == DAYZ_KEYCARD_UPDATEDOOR)
			{
				//true/false/index of door!
				Param2<bool, int> update_door_data = new Param2<bool, int>(false, 0);
				ctx.Read( update_door_data );
				
				GetKeyCard().UpdateDoor(update_door_data.param1, update_door_data.param2);		
			}
			
			if (rpc_type == DAYZ_KEYCARD_FORCELOCK)
			{
				Param1<bool> force_lock_data = new Param1<bool>(false);
				ctx.Read( force_lock_data );
			
				GetKeyCard().InitKeyCardSystem();
			}
			
			if (rpc_type == DAYZ_KEYCARD_FORCE_OPEN)
			{
				Param1<int> open_door_data = new Param1<int>(0);
				ctx.Read( open_door_data );
			
				GetKeyCard().ForceOpenDoor(open_door_data.param1);
			}
		}
		else
		{
			if (rpc_type == DAYZ_KEYCARD_DATA)
			{			
				Param3<bool, vector, KeyCardSettings> keycard_data = new Param3<bool, vector, KeyCardSettings>( false, Vector(0, 0, 0), null);
				ctx.Read( keycard_data );
				
				Print("[Keycard RPC] LockedDoorsLocation + " + keycard_data.param2);
				locked_locations.Insert(keycard_data.param2);

				Print("[Keycard RPC] Keycard Data " + keycard_data.param3);
				Keycard_CFGData = keycard_data.param3;
			}		
		}
	}
};