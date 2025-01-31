////////////////////////////////////////////////////////////////////
//DeRap: config.bin
//Produced from mikero's Dos Tools Dll version 9.89
//https://mikero.bytex.digital/Downloads
//'now' is Tue Jan 21 06:50:36 2025 : 'file' last modified on Tue Jan 21 00:38:29 2025
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class KeycardSystem
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Gear_Consumables","DZ_Gear_Camping"};
	};
};
class CfgMods
{
	class KeycardSystem
	{
		dir = "KeycardSystem";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 1;
		name = "KeycardSystem";
		credits = "";
		author = "CJ";
		inputs = "";
		authorID = "";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game","World","Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"KeycardSystem/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"KeycardSystem/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"KeycardSystem/5_Mission"};
			};
		};
	};
};
class CfgSounds
{
	class default
	{
		name = "";
		titles[] = {};
	};
	class Sound_Door_Accepted: default
	{
		sound[] = {"KeycardSystem\data\Door_Accepted",1,1,1000};
	};
	class Sound_Door_Denied: default
	{
		sound[] = {"KeycardSystem\data\Door_Denied",1,1,1000};
	};
	class Alarm_Military: default
	{
		sound[] = {"KeycardSystem\data\Alarm_Military",1,1,1000};
	};
	class Alarm_Military2: default
	{
		sound[] = {"KeycardSystem\data\Alarm_Military2",1,1,1000};
	};
	class Alarm_Bank: default
	{
		sound[] = {"KeycardSystem\data\Alarm_Bank",1,1,1000};
	};
};
