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

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (!target) return false;
        if (!IsBuilding(target)) return false;
        if (!IsInReach(player, target, UAMaxDistances.DEFAULT)) return false;

        Building building;

        if (Class.CastTo(building, target.GetObject()))
        {
            int doorIndex = building.GetDoorIndex(target.GetComponentIndex());

            if (doorIndex != -1)
            {
                return building.IsDoorLocked(doorIndex);
            }
        }

        return false;
    }

    static void LogToCFTools(Man player, string item, string target, string action)
    {
#ifdef GameLabs
        ref _LogPlayerEx logObjectPlayer = new _LogPlayerEx(player);
        action = action + " ";
        ref _Payload_ItemInteract payload = new _Payload_ItemInteract(logObjectPlayer, item, target, action);
        GetGameLabs().GetApi().ItemInteract(new _Callback(), payload);
#endif
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        if (!action_data.m_Player)
            return;

        int closest_number = -1;

        for (int d = 0; d < GetKeyCard().m_KeyCardSettings.KeyCard_Locations.Count(); d++)
        {
            vector door_locations;

            door_locations[0] = GetKeyCard().m_KeyCardSettings.KeyCard_Locations.Get(d).x;
            door_locations[1] = GetKeyCard().m_KeyCardSettings.KeyCard_Locations.Get(d).z;
            door_locations[2] = GetKeyCard().m_KeyCardSettings.KeyCard_Locations.Get(d).y;

            float distance = Math.Round(vector.Distance(action_data.m_Player.GetPosition(), door_locations));

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

        HumanInventory inv = action_data.m_Player.GetHumanInventory();
        EntityAI hand = inv.GetEntityInHands();

        Building building;

        bool is_a_keycard_door = false;
        bool display_message = false;
        bool has_run_unlock = false;

        // Declarar la variable m_MessageParam_ fuera del bloque condicional
        Param1<string> m_MessageParam_;

        if (Class.CastTo(building, action_data.m_Target.GetObject()))
        {
            int doorIndex = building.GetDoorIndex(action_data.m_Target.GetComponentIndex());
            if (doorIndex != -1)
            {
                if (closest_number <= 3)
                {
                    for (int c = 0; c < GetKeyCard().DoorLockedLocations.Count(); c++)
                    {
                        if (GetKeyCard().DoorLockedLocations.Get(c).door_pos == building.GetDoorSoundPos(doorIndex))
                        {
                            is_a_keycard_door = true;

                            if (hand && hand.GetType() != best_door.keycard_class_name)
                            {
                                if (!display_message)
                                {
                                    // Obtener el objeto de la keycard correcta
                                    ItemBase correctKeycard = ItemBase.Cast(GetGame().CreateObjectEx(best_door.keycard_class_name, Vector(0, 0, 0), ECE_NONE));

                                    // Verificar si el objeto se creó correctamente
                                    if (correctKeycard)
                                    {
                                        // Obtener el display name del item
                                        string displayName = correctKeycard.GetDisplayName();

                                        // Crear el mensaje con el display name
                                        m_MessageParam_ = new Param1<string>("Esta puerta requiere una " + displayName);
                                        display_message = true;
                                        GetGame().RPCSingleParam(action_data.m_Player, ERPCs.RPC_USER_ACTION_MESSAGE, m_MessageParam_, true, action_data.m_Player.GetIdentity());

                                        // Eliminar el objeto creado para evitar fugas de memoria
                                        GetGame().ObjectDelete(correctKeycard);
                                    }
                                    else
                                    {
                                        // Si no se puede obtener el display name, mostrar un mensaje genérico
                                        m_MessageParam_ = new Param1<string>("Esta puerta requiere una keycard específica.");
                                        display_message = true;
                                        GetGame().RPCSingleParam(action_data.m_Player, ERPCs.RPC_USER_ACTION_MESSAGE, m_MessageParam_, true, action_data.m_Player.GetIdentity());
                                    }
                                }
                            }
                            else
                            {
                                if (!has_run_unlock)
                                {
                                    has_run_unlock = true;

                                    if (action_data.m_MainItem)
                                    {
                                        GetGame().ObjectDelete(action_data.m_MainItem);
                                    }

                                    if (GetKeyCard().m_KeyCardSettings.admin_log)
                                        Print("[Keycard] SteamID " + action_data.m_Player.GetIdentity().GetPlainId() + " Abierta" + best_door.Location_Name + " Usando " + best_door.keycard_class_name);

                                    if (best_door.use_alarm)
                                        building.PlaySoundLoop("Alarm_Military", 100, false);
                                        building.PlaySound("Sound_Door_Accepted", 5, false);

                                    if (best_door.containers)
                                        GetKeyCard().GenerateRewards(best_door.containers, false);

                                    int RandomUnlockTime = Math.RandomIntInclusive(best_door.min_unlock_time * 1000, best_door.max_unlock_time * 1000);
                                    int RandomRestockTime = Math.RandomIntInclusive(best_door.min_restock_time * 1000, best_door.max_restock_time * 1000);

                                    if (best_door.use_announcement)
                                    {
                                        if (best_door.announcement_text == "")
                                        {
                                            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(building.UnlockDoor, RandomUnlockTime, false, doorIndex);
                                            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetKeyCard().CloseDoor, RandomRestockTime, false, best_door);
                                            if (g_Game.IsServer())
                                            {
                                                LogToCFTools(action_data.m_Player, "", string.Format("used %1 to open door in %2", action_data.m_MainItem.GetType(), action_data.m_Target.GetObject().GetType()), "");
                                            }
                                        }
                                        else
                                        {
                                            string text_to_use = best_door.announcement_text;

                                            NotificationSystem.Create(new StringLocaliser("Alerta"), new StringLocaliser(text_to_use), "DayZExpansion/Core/GUI/icons/hud/skull_2_64x64.edds", ARGB(255, 221, 38, 38), 10, NULL);

                                            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(building.UnlockDoor, RandomUnlockTime, false, doorIndex);
                                            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetKeyCard().CloseDoor, RandomRestockTime, false, best_door);
                                            if (g_Game.IsServer())
                                            {
                                                LogToCFTools(action_data.m_Player, "", string.Format("used %1 to open door in %2", action_data.m_MainItem.GetType(), action_data.m_Target.GetObject().GetType()), "");
                                            }
                                        }
                                    }
                                    else
                                    {
                                        building.UnlockDoor(doorIndex);

                                        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetKeyCard().CloseDoor, RandomRestockTime, false, best_door);
                                        if (g_Game.IsServer())
                                        {
                                            LogToCFTools(action_data.m_Player, "", string.Format("used %1 to open door in %2", action_data.m_MainItem.GetType(), action_data.m_Target.GetObject().GetType()), "");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (!is_a_keycard_door)
                    building.UnlockDoor(doorIndex);
            }
        }
    }
};