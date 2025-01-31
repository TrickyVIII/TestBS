modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{	
		super.RegisterActions(actions); //call the original
		actions.Insert(ActionUnlockKeyCardDoor); //add our custom action for the locked doors //adds this action
	}
};