.set LOCALID_NURSE, 1

OldaleTown_PokemonCenter_1F_MapScripts:: @ 81FC006
	map_script MAP_SCRIPT_ON_TRANSITION, OldaleTown_PokemonCenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

OldaleTown_PokemonCenter_1F_OnTransition: @ 81FC011
	setrespawn HEAL_LOCATION_OLDALE_TOWN
	call Common_EventScript_UpdateBrineyLocation
	end

OldaleTown_PokemonCenter_1F_EventScript_Nurse:: @ 81FC01A
	setvar VAR_0x800B, LOCALID_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

OldaleTown_PokemonCenter_1F_EventScript_Gentleman:: @ 81FC028
	msgbox OldaleTown_PokemonCenter_1F_Text_TrainersCanUsePC, MSGBOX_NPC
	end

OldaleTown_PokemonCenter_1F_EventScript_Boy:: @ 81FC031
	lock
	faceplayer
	goto_if_set FLAG_NUZLOCKE_ITEMS, OldaleTown_PokemonCenter_1F_Say_PokemonCentersAreGreat @ si ce flag est set, on va au script donné
	setflag FLAG_NUZLOCKE_ITEMS @ si le flag n'est pas set, on donne les items
	additem ITEM_RARE_CANDY, 999
	additem ITEM_RARE_CANDY, 999
	additem ITEM_RARE_CANDY, 999
	additem ITEM_BIG_PEARL, 999
	additem ITEM_MIRACLE_SEED, 10
	additem ITEM_CHARCOAL, 10
	additem ITEM_MENTAL_HERB, 100
	additem ITEM_POWER_HERB, 100
	additem ITEM_WHITE_HERB, 100
	additem ITEM_ELECTRIC_SEED, 100
	additem ITEM_PSYCHIC_SEED, 100
	additem ITEM_GRASSY_SEED, 100
	additem ITEM_MISTY_SEED, 100
	additem ITEM_LEEK, 10
	additem ITEM_BIG_ROOT, 10
	additem ITEM_LEFTOVERS, 10
	additem ITEM_SITRUS_BERRY, 500
	additem ITEM_BRIGHT_POWDER, 10
	additem ITEM_HARD_STONE, 10
	additem ITEM_CHERI_BERRY
	additem ITEM_CHESTO_BERRY
	additem ITEM_PECHA_BERRY
	additem ITEM_RAWST_BERRY
	additem ITEM_PERSIM_BERRY
	additem ITEM_OCCA_BERRY
	additem ITEM_PASSHO_BERRY
	additem ITEM_WACAN_BERRY
	additem ITEM_RINDO_BERRY
	additem ITEM_YACHE_BERRY
	additem ITEM_CHOPLE_BERRY
	additem ITEM_KEBIA_BERRY
	additem ITEM_SHUCA_BERRY
	additem ITEM_COBA_BERRY
	additem ITEM_PAYAPA_BERRY
	additem ITEM_TANGA_BERRY
	additem ITEM_CHARTI_BERRY
	additem ITEM_KASIB_BERRY
	additem ITEM_HABAN_BERRY
	additem ITEM_COLBUR_BERRY
	additem ITEM_BABIRI_BERRY
	additem ITEM_CHILAN_BERRY
	additem ITEM_ROSELI_BERRY
	msgbox OldaleTown_PokemonCenter_1F_Text_Give_Items, MSGBOX_NPC @ et on appelle le text giveitems
	release
	end

OldaleTown_PokemonCenter_1F_EventScript_Girl:: @ 81FC03A
	lock
	faceplayer
	goto_if_set FLAG_SYS_POKEDEX_GET, OldaleTown_PokemonCenter_1F_EventScript_WirelessClubAvailable
	msgbox OldaleTown_PokemonCenter_1F_Text_WirelessClubNotAvailable, MSGBOX_DEFAULT
	release
	end

OldaleTown_PokemonCenter_1F_EventScript_WirelessClubAvailable:: @ 81FC04F
	msgbox OldaleTown_PokemonCenter_1F_Text_TradedInWirelessClub, MSGBOX_DEFAULT
	release
	end

OldaleTown_PokemonCenter_1F_Text_TrainersCanUsePC: @ 81FC059
	.string "That PC in the corner there is\n"
	.string "for any Pokémon Trainer to use.\p"
	.string "Naturally, that means you're welcome\n"
	.string "to use it, too.$"

OldaleTown_PokemonCenter_1F_Text_PokemonCentersAreGreat: @ définition du message Great centers
	.string "Enjoy!$"

OldaleTown_PokemonCenter_1F_Text_WirelessClubNotAvailable: @ 81FC148
	.string "The Pokémon Wireless Club on\n"
	.string "the second floor was built recently.\p"
	.string "But they say they're still making\n"
	.string "adjustments.$"

OldaleTown_PokemonCenter_1F_Text_TradedInWirelessClub: @ 81FC1B9
	.string "The Pokémon Wireless Club on\n"
	.string "the second floor was built recently.\p"
	.string "I traded Pokémon right away.$"
	
OldaleTown_PokemonCenter_1F_Text_Give_Items: @ définition du text giveitems
	.string "Here are your items!$"

OldaleTown_PokemonCenter_1F_Say_PokemonCentersAreGreat: @ le script donné va juste display un message, qui est défini plus bas, et laisse le joueur partir
	msgbox OldaleTown_PokemonCenter_1F_Text_PokemonCentersAreGreat, MSGBOX_NPC @ appelle du message GreatCenters
	release
	end
