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


OldaleTown_PokemonCenter_1F_Text_WirelessClubNotAvailable: @ 81FC148
	.string "The Pokémon Wireless Club on\n"
	.string "the second floor was built recently.\p"
	.string "But they say they're still making\n"
	.string "adjustments.$"

OldaleTown_PokemonCenter_1F_Text_TradedInWirelessClub: @ 81FC1B9
	.string "The Pokémon Wireless Club on\n"
	.string "the second floor was built recently.\p"
	.string "I traded Pokémon right away.$"
	
OldaleTown_PokemonCenter_1F_EventScript_Boy:: @ 81FC031
	lock
	faceplayer
	removeitem ITEM_RARE_CANDY, 400
	removeitem ITEM_BIG_PEARL, 400
	additem ITEM_RARE_CANDY, 400
	additem ITEM_BIG_PEARL, 400
	call_if_set FLAG_BADGE01_GET, Give_Cut_Items_After_Roxanne
	call_if_set FLAG_RECEIVED_TM46, Give_Thief_Items_After_Thief
	call_if_set FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_AQUA_GRUNTS, Give_Thief_Items_After_Museum
	call_if_set FLAG_BADGE03_GET, Give_Thief_Items_After_Wattson
	call_if_set FLAG_BADGE04_GET, Give_Thief_Items_After_Flannery
	call_if_set FLAG_BADGE05_GET, Give_Thief_Items_After_Norman
	release
	end
	
Give_Cut_Items_After_Roxanne:
	removeitem ITEM_MIRACLE_SEED, 10	
	removeitem ITEM_CHARCOAL, 10
	removeitem ITEM_LEEK, 10
	removeitem ITEM_BIG_ROOT, 10
	removeitem ITEM_MENTAL_HERB, 100
	removeitem ITEM_POWER_HERB, 100
	removeitem ITEM_WHITE_HERB, 100
	removeitem ITEM_ELECTRIC_SEED, 100
	removeitem ITEM_PSYCHIC_SEED, 100
	removeitem ITEM_GRASSY_SEED, 100
	removeitem ITEM_MISTY_SEED, 100
	removeitem ITEM_CHERI_BERRY, 100
	removeitem ITEM_CHESTO_BERRY, 100
	removeitem ITEM_PECHA_BERRY, 100
	removeitem ITEM_RAWST_BERRY, 100
	removeitem ITEM_PERSIM_BERRY, 100
	removeitem ITEM_OCCA_BERRY, 100
	removeitem ITEM_PASSHO_BERRY, 100
	removeitem ITEM_WACAN_BERRY, 100
	removeitem ITEM_RINDO_BERRY, 100
	removeitem ITEM_YACHE_BERRY, 100
	removeitem ITEM_CHOPLE_BERRY, 100
	removeitem ITEM_KEBIA_BERRY, 100
	removeitem ITEM_SHUCA_BERRY, 100
	removeitem ITEM_COBA_BERRY, 100
	removeitem ITEM_PAYAPA_BERRY, 100
	removeitem ITEM_TANGA_BERRY, 100
	removeitem ITEM_CHARTI_BERRY, 100
	removeitem ITEM_KASIB_BERRY, 100
	removeitem ITEM_HABAN_BERRY, 100
	removeitem ITEM_COLBUR_BERRY, 100
	removeitem ITEM_BABIRI_BERRY, 100
	removeitem ITEM_CHILAN_BERRY, 100
	removeitem ITEM_ROSELI_BERRY, 100
	additem ITEM_MIRACLE_SEED, 10	
	additem ITEM_CHARCOAL, 10
	additem ITEM_LEEK, 10
	additem ITEM_BIG_ROOT, 10
	additem ITEM_MENTAL_HERB, 100
	additem ITEM_POWER_HERB, 100
	additem ITEM_WHITE_HERB, 100
	additem ITEM_ELECTRIC_SEED, 100
	additem ITEM_PSYCHIC_SEED, 100
	additem ITEM_GRASSY_SEED, 100
	additem ITEM_MISTY_SEED, 100
	additem ITEM_CHERI_BERRY, 100
	additem ITEM_CHESTO_BERRY, 100
	additem ITEM_PECHA_BERRY, 100
	additem ITEM_RAWST_BERRY, 100
	additem ITEM_PERSIM_BERRY, 100
	additem ITEM_OCCA_BERRY, 100
	additem ITEM_PASSHO_BERRY, 100
	additem ITEM_WACAN_BERRY, 100
	additem ITEM_RINDO_BERRY, 100
	additem ITEM_YACHE_BERRY, 100
	additem ITEM_CHOPLE_BERRY, 100
	additem ITEM_KEBIA_BERRY, 100
	additem ITEM_SHUCA_BERRY, 100
	additem ITEM_COBA_BERRY, 100
	additem ITEM_PAYAPA_BERRY, 100
	additem ITEM_TANGA_BERRY, 100
	additem ITEM_CHARTI_BERRY, 100
	additem ITEM_KASIB_BERRY, 100
	additem ITEM_HABAN_BERRY, 100
	additem ITEM_COLBUR_BERRY, 100
	additem ITEM_BABIRI_BERRY, 100
	additem ITEM_CHILAN_BERRY, 100
	additem ITEM_ROSELI_BERRY, 100
	
Give_Thief_Items_After_Thief:
	removeitem ITEM_SITRUS_BERRY, 400
	removeitem ITEM_LEFTOVERs, 10
	removeitem ITEM_BRIGHT_POWDER, 10
	removeitem ITEM_HARD_STONE, 10
	removeitem ITEM_MYSTIC_WATER, 10
	removeitem ITEM_SILVER_POWDER, 10
	removeitem ITEM_POISON_BARB, 10
	removeitem ITEM_SOFT_SAND, 10
	removeitem ITEM_BLACK_BELT, 10
	additem ITEM_SITRUS_BERRY, 400
	additem ITEM_LEFTOVERs, 10
	additem ITEM_BRIGHT_POWDER, 10
	additem ITEM_HARD_STONE, 10
	additem ITEM_MYSTIC_WATER, 10
	additem ITEM_SILVER_POWDER, 10
	additem ITEM_POISON_BARB, 10
	additem ITEM_SOFT_SAND, 10
	additem ITEM_BLACK_BELT, 10


Give_Thief_Items_After_Museum:
	removeitem ITEM_SILK_SCARF, 10
	removeitem ITEM_SHARP_BEAK, 10
	removeitem ITEM_NEVER_MELT_ICE, 10
	removeitem ITEM_TWISTED_SPOON, 10
	additem ITEM_SILK_SCARF, 10
	additem ITEM_SHARP_BEAK, 10
	additem ITEM_NEVER_MELT_ICE, 10
	additem ITEM_TWISTED_SPOON, 10
	
Give_Thief_Items_After_Wattson:
	removeitem ITEM_MAGNET, 10
	additem ITEM_MAGNET, 10


Give_Thief_Items_After_Flannery:
	removeitem ITEM_BLACK_GLASSES, 10
	removeitem ITEM_SPELL_TAG, 10
	additem ITEM_BLACK_GLASSES, 10
	additem ITEM_SPELL_TAG, 10

Give_Thief_Items_After_Norman:
	removeitem ITEM_DRAGON_FANG, 10
	additem ITEM_DRAGON_FANG, 10
	
