class keycard_base_tool: Inventory_Base 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionUnlockKeyCardDoor);
	}
};