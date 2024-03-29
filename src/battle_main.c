#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_arena.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "battle_main.h"
#include "battle_message.h"
#include "battle_pyramid.h"
#include "battle_scripts.h"
#include "battle_setup.h"
#include "battle_tower.h"
#include "battle_util.h"
#include "berry.h"
#include "bg.h"
#include "data.h"
#include "decompress.h"
#include "dma3.h"
#include "event_data.h"
#include "evolution_scene.h"
#include "graphics.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "item.h"
#include "link.h"
#include "link_rfu.h"
#include "load_save.h"
#include "main.h"
#include "malloc.h"
#include "m4a.h"
#include "palette.h"
#include "party_menu.h"
#include "pokeball.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "recorded_battle.h"
#include "roamer.h"
#include "safari_zone.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "trig.h"
#include "tv.h"
#include "util.h"
#include "wild_encounter.h"
#include "window.h"
#include "constants/abilities.h"
#include "constants/battle_config.h"
#include "constants/battle_move_effects.h"
#include "constants/battle_string_ids.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/party_menu.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/trainers.h"
#include "constants/opponents.h"
#include "constants/spreads.h"
#include "cable_club.h"




extern struct MusicPlayerInfo gMPlayInfo_SE1;
extern struct MusicPlayerInfo gMPlayInfo_SE2;

extern const struct BgTemplate gBattleBgTemplates[];
extern const struct WindowTemplate *const gBattleWindowTemplates[];

// this file's functions
static void CB2_InitBattleInternal(void);
static void CB2_PreInitMultiBattle(void);
static void CB2_PreInitIngamePlayerPartnerBattle(void);
static void CB2_HandleStartMultiPartnerBattle(void);
static void CB2_HandleStartMultiBattle(void);
static void CB2_HandleStartBattle(void);
static void TryCorrectShedinjaLanguage(struct Pokemon *mon);
static u8 CreateNPCTrainerParty(struct Pokemon *party, u16 trainerNum, bool8 firstTrainer);
static void BattleMainCB1(void);
static void sub_8038538(struct Sprite *sprite);
static void CB2_EndLinkBattle(void);
static void EndLinkBattleInSteps(void);
static void sub_80392A8(void);
static void sub_803937C(void);
static void sub_803939C(void);
static void SpriteCb_MoveWildMonToRight(struct Sprite *sprite);
static void SpriteCb_WildMonShowHealthbox(struct Sprite *sprite);
static void SpriteCb_WildMonAnimate(struct Sprite *sprite);
static void sub_80398D0(struct Sprite *sprite);
static void SpriteCB_AnimFaintOpponent(struct Sprite *sprite);
static void SpriteCb_BlinkVisible(struct Sprite *sprite);
static void SpriteCallbackDummy_3(struct Sprite *sprite);
static void SpriteCB_BattleSpriteSlideLeft(struct Sprite *sprite);
static void TurnValuesCleanUp(bool8 var0);
static void SpriteCB_BounceEffect(struct Sprite *sprite);
static void BattleStartClearSetData(void);
static void DoBattleIntro(void);
static void TryDoEventsBeforeFirstTurn(void);
static void HandleTurnActionSelectionState(void);
static void RunTurnActionsFunctions(void);
static void SetActionsAndBattlersTurnOrder(void);
static void sub_803CDF8(void);
static bool8 AllAtActionConfirmed(void);
static void TryChangeTurnOrder(void);
static void CheckFocusPunch_ClearVarsBeforeTurnStarts(void);
static void CheckMegaEvolutionBeforeTurn(void);
static void CheckQuickClaw_CustapBerryActivation(void);
static void FreeResetData_ReturnToOvOrDoEvolutions(void);
static void ReturnFromBattleToOverworld(void);
static void TryEvolvePokemon(void);
static void WaitForEvoSceneToFinish(void);
static void HandleEndTurn_ContinueBattle(void);
static void HandleEndTurn_BattleWon(void);
static void HandleEndTurn_BattleLost(void);
static void HandleEndTurn_RanFromBattle(void);
static void HandleEndTurn_MonFled(void);
static void HandleEndTurn_FinishBattle(void);
static u8 getRole(u16 species);
static u16 getStabMove (u16 ID, u8 i, u16 atk, u16 spAtk);
static u16 getHeldItem (u16 species);
static u16 getAverageBSTEnemyParty(void);
static u8 getNewPokemonLevel(u16 species, u8 currentPokemonLevel);
static u16 pickOldplayerGeneralSpecies(u8 i);


// EWRAM vars
EWRAM_DATA u16 gBattle_BG0_X = 0;
EWRAM_DATA u16 gBattle_BG0_Y = 0;
EWRAM_DATA u16 gBattle_BG1_X = 0;
EWRAM_DATA u16 gBattle_BG1_Y = 0;
EWRAM_DATA u16 gBattle_BG2_X = 0;
EWRAM_DATA u16 gBattle_BG2_Y = 0;
EWRAM_DATA u16 gBattle_BG3_X = 0;
EWRAM_DATA u16 gBattle_BG3_Y = 0;
EWRAM_DATA u16 gBattle_WIN0H = 0;
EWRAM_DATA u16 gBattle_WIN0V = 0;
EWRAM_DATA u16 gBattle_WIN1H = 0;
EWRAM_DATA u16 gBattle_WIN1V = 0;
EWRAM_DATA u8 gDisplayedStringBattle[400] = {0};
EWRAM_DATA u8 gBattleTextBuff1[TEXT_BUFF_ARRAY_COUNT] = {0};
EWRAM_DATA u8 gBattleTextBuff2[TEXT_BUFF_ARRAY_COUNT] = {0};
EWRAM_DATA u8 gBattleTextBuff3[TEXT_BUFF_ARRAY_COUNT] = {0};
EWRAM_DATA u32 gBattleTypeFlags = 0;
EWRAM_DATA u8 gBattleTerrain = 0;
EWRAM_DATA u32 gUnusedFirstBattleVar1 = 0; // Never read
EWRAM_DATA struct UnknownPokemonStruct4 gMultiPartnerParty[MULTI_PARTY_SIZE] = {0};
EWRAM_DATA static struct UnknownPokemonStruct4* sMultiPartnerPartyBuffer = NULL;
EWRAM_DATA u8 *gUnknown_0202305C = NULL;
EWRAM_DATA u8 *gUnknown_02023060 = NULL;
EWRAM_DATA u8 gActiveBattler = 0;
EWRAM_DATA u32 gBattleControllerExecFlags = 0;
EWRAM_DATA u8 gBattlersCount = 0;
EWRAM_DATA u16 gBattlerPartyIndexes[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gBattlerPositions[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gActionsByTurnOrder[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gBattlerByTurnOrder[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gCurrentTurnActionNumber = 0;
EWRAM_DATA u8 gCurrentActionFuncId = 0;
EWRAM_DATA struct BattlePokemon gBattleMons[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gBattlerSpriteIds[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gCurrMovePos = 0;
EWRAM_DATA u8 gChosenMovePos = 0;
EWRAM_DATA u16 gCurrentMove = 0;
EWRAM_DATA u16 gChosenMove = 0;
EWRAM_DATA u16 gCalledMove = 0;
EWRAM_DATA s32 gBattleMoveDamage = 0;
EWRAM_DATA s32 gHpDealt = 0;
EWRAM_DATA s32 gTakenDmg[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLastUsedItem = 0;
EWRAM_DATA u16 gLastUsedAbility = 0;
EWRAM_DATA u8 gBattlerAttacker = 0;
EWRAM_DATA u8 gBattlerTarget = 0;
EWRAM_DATA u8 gBattlerFainted = 0;
EWRAM_DATA u8 gEffectBattler = 0;
EWRAM_DATA u8 gPotentialItemEffectBattler = 0;
EWRAM_DATA u8 gAbsentBattlerFlags = 0;
EWRAM_DATA u8 gIsCriticalHit = FALSE;
EWRAM_DATA u8 gMultiHitCounter = 0;
EWRAM_DATA const u8 *gBattlescriptCurrInstr = NULL;
EWRAM_DATA u8 gChosenActionByBattler[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA const u8 *gSelectionBattleScripts[MAX_BATTLERS_COUNT] = {NULL};
EWRAM_DATA const u8 *gPalaceSelectionBattleScripts[MAX_BATTLERS_COUNT] = {NULL};
EWRAM_DATA u16 gLastPrintedMoves[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLastMoves[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLastLandedMoves[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLastHitByType[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLastResultingMoves[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLockedMoves[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gLastUsedMove = 0;
EWRAM_DATA u8 gLastHitBy[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gChosenMoveByBattler[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gMoveResultFlags = 0;
EWRAM_DATA u32 gHitMarker = 0;
EWRAM_DATA u8 gTakenDmgByBattler[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gUnusedFirstBattleVar2 = 0; // Never read
EWRAM_DATA u32 gSideStatuses[2] = {0};
EWRAM_DATA struct SideTimer gSideTimers[2] = {0};
EWRAM_DATA u32 gStatuses3[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u32 gStatuses4[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA struct DisableStruct gDisableStructs[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gPauseCounterBattle = 0;
EWRAM_DATA u16 gPaydayMoney = 0;
EWRAM_DATA u16 gRandomTurnNumber = 0;
EWRAM_DATA u8 gBattleCommunication[BATTLE_COMMUNICATION_ENTRIES_COUNT] = {0};
EWRAM_DATA u8 gBattleOutcome = 0;
EWRAM_DATA struct ProtectStruct gProtectStructs[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA struct SpecialStatus gSpecialStatuses[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u16 gBattleWeather = 0;
EWRAM_DATA struct WishFutureKnock gWishFutureKnock = {0};
EWRAM_DATA u16 gIntroSlideFlags = 0;
EWRAM_DATA u8 gSentPokesToOpponent[2] = {0};
EWRAM_DATA u16 gExpShareExp = 0;
EWRAM_DATA struct BattleEnigmaBerry gEnigmaBerries[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA struct BattleScripting gBattleScripting = {0};
EWRAM_DATA struct BattleStruct *gBattleStruct = NULL;
EWRAM_DATA u8 *gLinkBattleSendBuffer = NULL;
EWRAM_DATA u8 *gLinkBattleRecvBuffer = NULL;
EWRAM_DATA struct BattleResources *gBattleResources = NULL;
EWRAM_DATA u8 gActionSelectionCursor[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gMoveSelectionCursor[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gBattlerStatusSummaryTaskId[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gBattlerInMenuId = 0;
EWRAM_DATA bool8 gDoingBattleAnim = FALSE;
EWRAM_DATA u32 gTransformedPersonalities[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u8 gPlayerDpadHoldFrames = 0;
EWRAM_DATA struct BattleSpriteData *gBattleSpritesDataPtr = NULL;
EWRAM_DATA struct MonSpritesGfx *gMonSpritesGfxPtr = NULL;
EWRAM_DATA struct BattleHealthboxInfo *gBattleControllerOpponentHealthboxData = NULL; // Never read
EWRAM_DATA struct BattleHealthboxInfo *gBattleControllerOpponentFlankHealthboxData = NULL; // Never read
EWRAM_DATA u16 gBattleMovePower = 0;
EWRAM_DATA u16 gMoveToLearn = 0;
EWRAM_DATA u8 gBattleMonForms[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA u32 gFieldStatuses = 0;
EWRAM_DATA struct FieldTimer gFieldTimers = {0};
EWRAM_DATA u8 gBattlerAbility = 0;
EWRAM_DATA u16 gPartnerSpriteId = 0;
EWRAM_DATA struct TotemBoost gTotemBoosts[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA bool8 gHasFetchedBall = FALSE;
EWRAM_DATA u8 gLastUsedBall = 0;
EWRAM_DATA u16 gLastThrownBall = 0;
EWRAM_DATA u8 gMaxPartyLevel = 1;
EWRAM_DATA bool8 gSwapDamageCategory = FALSE; // Photon Geyser, Shell Side Arm, Light That Burns the Sky

// IWRAM common vars
void (*gPreBattleCallback1)(void);
void (*gBattleMainFunc)(void);
struct BattleResults gBattleResults;
u8 gLeveledUpInBattle;
void (*gBattlerControllerFuncs[MAX_BATTLERS_COUNT])(void);
u8 gHealthboxSpriteIds[MAX_BATTLERS_COUNT];
u8 gMultiUsePlayerCursor;
u8 gNumberOfMovesToChoose;
u8 gBattleControllerData[MAX_BATTLERS_COUNT]; // Used by the battle controllers to store misc sprite/task IDs for each battler

// rom const data
static const struct ScanlineEffectParams sIntroScanlineParams16Bit =
{
    (void *)REG_ADDR_BG3HOFS, SCANLINE_EFFECT_DMACNT_16BIT, 1
};

// unused
static const struct ScanlineEffectParams sIntroScanlineParams32Bit =
{
    (void *)REG_ADDR_BG3HOFS, SCANLINE_EFFECT_DMACNT_32BIT, 1
};

const struct SpriteTemplate gUnknown_0831AC88 =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &gDummyOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_8038528,
};

static const u8 sText_ShedinjaJpnName[] = _("ヌケニン"); // Nukenin

const struct OamData gOamData_BattleSpriteOpponentSide =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

const struct OamData gOamData_BattleSpritePlayerSide =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 2,
    .affineParam = 0,
};

static const s8 gUnknown_0831ACE0[] ={-32, -16, -16, -32, -32, 0, 0, 0};

const u8 gTypeNames[NUMBER_OF_MON_TYPES][TYPE_NAME_LENGTH + 1] =
{
    [TYPE_NORMAL] = _("Normal"),
    [TYPE_FIGHTING] = _("Fight"),
    [TYPE_FLYING] = _("Flying"),
    [TYPE_POISON] = _("Poison"),
    [TYPE_GROUND] = _("Ground"),
    [TYPE_ROCK] = _("Rock"),
    [TYPE_BUG] = _("Bug"),
    [TYPE_GHOST] = _("Ghost"),
    [TYPE_STEEL] = _("Steel"),
    [TYPE_MYSTERY] = _("???"),
    [TYPE_FIRE] = _("Fire"),
    [TYPE_WATER] = _("Water"),
    [TYPE_GRASS] = _("Grass"),
    [TYPE_ELECTRIC] = _("Electr"),
    [TYPE_PSYCHIC] = _("Psychc"),
    [TYPE_ICE] = _("Ice"),
    [TYPE_DRAGON] = _("Dragon"),
    [TYPE_DARK] = _("Dark"),
    [TYPE_FAIRY] = _("Fairy"),
};

// This is a factor in how much money you get for beating a trainer.
const struct TrainerMoney gTrainerMoneyTable[] =
{
    {TRAINER_CLASS_PKMN_TRAINER_1, 50},
    {TRAINER_CLASS_TEAM_AQUA, 5},
    {TRAINER_CLASS_AQUA_ADMIN, 10},
    {TRAINER_CLASS_AQUA_LEADER, 20},
    {TRAINER_CLASS_AROMA_LADY, 10},
    {TRAINER_CLASS_RUIN_MANIAC, 15},
    {TRAINER_CLASS_INTERVIEWER, 12},
    {TRAINER_CLASS_TUBER_F, 1},
    {TRAINER_CLASS_TUBER_M, 1},
    {TRAINER_CLASS_SIS_AND_BRO, 3},
    {TRAINER_CLASS_COOLTRAINER, 12},
    {TRAINER_CLASS_HEX_MANIAC, 6},
    {TRAINER_CLASS_LADY, 50},
    {TRAINER_CLASS_BEAUTY, 20},
    {TRAINER_CLASS_RICH_BOY, 50},
    {TRAINER_CLASS_POKEMANIAC, 15},
    {TRAINER_CLASS_SWIMMER_M, 2},
    {TRAINER_CLASS_BLACK_BELT, 8},
    {TRAINER_CLASS_GUITARIST, 8},
    {TRAINER_CLASS_KINDLER, 8},
    {TRAINER_CLASS_CAMPER, 4},
    {TRAINER_CLASS_OLD_COUPLE, 10},
    {TRAINER_CLASS_BUG_MANIAC, 15},
    {TRAINER_CLASS_PSYCHIC, 6},
    {TRAINER_CLASS_GENTLEMAN, 20},
    {TRAINER_CLASS_ELITE_FOUR, 25},
    {TRAINER_CLASS_LEADER, 25},
    {TRAINER_CLASS_SCHOOL_KID, 5},
    {TRAINER_CLASS_SR_AND_JR, 4},
    {TRAINER_CLASS_POKEFAN, 20},
    {TRAINER_CLASS_EXPERT, 10},
    {TRAINER_CLASS_YOUNGSTER, 4},
    {TRAINER_CLASS_CHAMPION, 50},
    {TRAINER_CLASS_FISHERMAN, 10},
    {TRAINER_CLASS_TRIATHLETE, 10},
    {TRAINER_CLASS_DRAGON_TAMER, 12},
    {TRAINER_CLASS_BIRD_KEEPER, 8},
    {TRAINER_CLASS_NINJA_BOY, 3},
    {TRAINER_CLASS_BATTLE_GIRL, 6},
    {TRAINER_CLASS_PARASOL_LADY, 10},
    {TRAINER_CLASS_SWIMMER_F, 2},
    {TRAINER_CLASS_PICNICKER, 4},
    {TRAINER_CLASS_TWINS, 3},
    {TRAINER_CLASS_SAILOR, 8},
    {TRAINER_CLASS_COLLECTOR, 15},
    {TRAINER_CLASS_PKMN_TRAINER_3, 15},
    {TRAINER_CLASS_PKMN_BREEDER, 10},
    {TRAINER_CLASS_PKMN_RANGER, 12},
    {TRAINER_CLASS_TEAM_MAGMA, 5},
    {TRAINER_CLASS_MAGMA_ADMIN, 10},
    {TRAINER_CLASS_MAGMA_LEADER, 20},
    {TRAINER_CLASS_LASS, 4},
    {TRAINER_CLASS_BUG_CATCHER, 4},
    {TRAINER_CLASS_HIKER, 10},
    {TRAINER_CLASS_YOUNG_COUPLE, 8},
    {TRAINER_CLASS_WINSTRATE, 10},
    {TRAINER_CLASS_MAGIKARP_GUY, 20},
    {TRAINER_CLASS_PKMN_TRAINER_4, 30},
    {TRAINER_CLASS_PKMN_TRAINER_5, 50},
    {TRAINER_CLASS_BUFFEL, 45},
    {TRAINER_CLASS_JOHTO_CHAMP, 50},
    {0xFF, 5},
};

// Determines which Poke Ball type is used by each trainer class
const struct TrainerBall gTrainerBallTable[] =
{
    {TRAINER_CLASS_PKMN_TRAINER_1, ITEM_ULTRA_BALL},
    {TRAINER_CLASS_TEAM_AQUA, ITEM_NET_BALL},
    {TRAINER_CLASS_AQUA_ADMIN, ITEM_NET_BALL},
    {TRAINER_CLASS_AQUA_LEADER, ITEM_MASTER_BALL},
    {TRAINER_CLASS_AROMA_LADY, ITEM_FRIEND_BALL},
    {TRAINER_CLASS_RUIN_MANIAC, ITEM_DUSK_BALL},
    {TRAINER_CLASS_INTERVIEWER, ITEM_REPEAT_BALL},
    {TRAINER_CLASS_TUBER_F, ITEM_DIVE_BALL},
    {TRAINER_CLASS_TUBER_M, ITEM_DIVE_BALL},
    {TRAINER_CLASS_SIS_AND_BRO, ITEM_POKE_BALL},
    {TRAINER_CLASS_COOLTRAINER, ITEM_ULTRA_BALL},
    {TRAINER_CLASS_HEX_MANIAC, ITEM_DUSK_BALL},
    {TRAINER_CLASS_LADY, ITEM_LUXURY_BALL},
    {TRAINER_CLASS_BEAUTY, ITEM_LOVE_BALL},
    {TRAINER_CLASS_RICH_BOY, ITEM_LUXURY_BALL},
    {TRAINER_CLASS_POKEMANIAC, ITEM_MOON_BALL},
    {TRAINER_CLASS_SWIMMER_M, ITEM_DIVE_BALL},
    {TRAINER_CLASS_BLACK_BELT, ITEM_HEAVY_BALL},
    {TRAINER_CLASS_GUITARIST, ITEM_FAST_BALL},
    {TRAINER_CLASS_KINDLER, ITEM_POKE_BALL},
    {TRAINER_CLASS_CAMPER, ITEM_NEST_BALL},
    {TRAINER_CLASS_OLD_COUPLE, ITEM_TIMER_BALL},
    {TRAINER_CLASS_BUG_MANIAC, ITEM_NET_BALL},
    {TRAINER_CLASS_PSYCHIC, ITEM_DREAM_BALL},
    {TRAINER_CLASS_GENTLEMAN, ITEM_LUXURY_BALL},
    {TRAINER_CLASS_ELITE_FOUR, ITEM_ULTRA_BALL},
    {TRAINER_CLASS_LEADER, ITEM_ULTRA_BALL},
    {TRAINER_CLASS_SCHOOL_KID, ITEM_POKE_BALL},
    {TRAINER_CLASS_SR_AND_JR, ITEM_POKE_BALL},
    {TRAINER_CLASS_POKEFAN, ITEM_POKE_BALL},
    {TRAINER_CLASS_EXPERT, ITEM_ULTRA_BALL},
    {TRAINER_CLASS_YOUNGSTER, ITEM_POKE_BALL},
    {TRAINER_CLASS_CHAMPION, ITEM_CHERISH_BALL},
    {TRAINER_CLASS_FISHERMAN, ITEM_LURE_BALL},
    {TRAINER_CLASS_TRIATHLETE, ITEM_FAST_BALL},
    {TRAINER_CLASS_DRAGON_TAMER, ITEM_ULTRA_BALL},
    {TRAINER_CLASS_BIRD_KEEPER, ITEM_QUICK_BALL},
    {TRAINER_CLASS_NINJA_BOY, ITEM_QUICK_BALL},
    {TRAINER_CLASS_BATTLE_GIRL, ITEM_HEAVY_BALL},
    {TRAINER_CLASS_PARASOL_LADY, ITEM_POKE_BALL},
    {TRAINER_CLASS_SWIMMER_F, ITEM_DIVE_BALL},
    {TRAINER_CLASS_PICNICKER, ITEM_FRIEND_BALL},
    {TRAINER_CLASS_TWINS, ITEM_POKE_BALL},
    {TRAINER_CLASS_SAILOR, ITEM_DIVE_BALL},
    {TRAINER_CLASS_COLLECTOR, ITEM_REPEAT_BALL},
    {TRAINER_CLASS_PKMN_TRAINER_3, ITEM_PREMIER_BALL},
    {TRAINER_CLASS_PKMN_BREEDER, ITEM_FRIEND_BALL},
    {TRAINER_CLASS_PKMN_RANGER, ITEM_SAFARI_BALL},
    {TRAINER_CLASS_TEAM_MAGMA, ITEM_NEST_BALL},
    {TRAINER_CLASS_MAGMA_ADMIN, ITEM_NEST_BALL},
    {TRAINER_CLASS_MAGMA_LEADER, ITEM_MASTER_BALL},
    {TRAINER_CLASS_LASS, ITEM_POKE_BALL},
    {TRAINER_CLASS_BUG_CATCHER, ITEM_NET_BALL},
    {TRAINER_CLASS_HIKER, ITEM_HEAVY_BALL},
    {TRAINER_CLASS_YOUNG_COUPLE, ITEM_LOVE_BALL},
    {TRAINER_CLASS_WINSTRATE, ITEM_GREAT_BALL},
    {TRAINER_CLASS_PKMN_TRAINER_2, ITEM_HEAVY_BALL},
    {TRAINER_CLASS_MAGIKARP_GUY, ITEM_LOVE_BALL},
    {TRAINER_CLASS_PKMN_TRAINER_4, ITEM_CHERISH_BALL},
    {TRAINER_CLASS_PKMN_TRAINER_5, ITEM_HEAVY_BALL},
    {TRAINER_CLASS_BUFFEL, ITEM_CHERISH_BALL},
    {TRAINER_CLASS_JOHTO_CHAMP, ITEM_ULTRA_BALL},
    {0xFF, ITEM_POKE_BALL},
};

#include "data/text/abilities.h"

static void (* const sTurnActionsFuncsTable[])(void) =
{
    [B_ACTION_USE_MOVE] = HandleAction_UseMove,
    [B_ACTION_USE_ITEM] = HandleAction_UseItem,
    [B_ACTION_SWITCH] = HandleAction_Switch,
    [B_ACTION_RUN] = HandleAction_Run,
    [B_ACTION_SAFARI_WATCH_CAREFULLY] = HandleAction_WatchesCarefully,
    [B_ACTION_SAFARI_BALL] = HandleAction_SafariZoneBallThrow,
    [B_ACTION_SAFARI_POKEBLOCK] = HandleAction_ThrowPokeblock,
    [B_ACTION_SAFARI_GO_NEAR] = HandleAction_GoNear,
    [B_ACTION_SAFARI_RUN] = HandleAction_SafariZoneRun,
    [B_ACTION_WALLY_THROW] = HandleAction_WallyBallThrow,
    [B_ACTION_EXEC_SCRIPT] = HandleAction_RunBattleScript,
    [B_ACTION_TRY_FINISH] = HandleAction_TryFinish,
    [B_ACTION_FINISHED] = HandleAction_ActionFinished,
    [B_ACTION_NOTHING_FAINTED] = HandleAction_NothingIsFainted,
    [B_ACTION_THROW_BALL] = HandleAction_ThrowBall,
};

static void (* const sEndTurnFuncsTable[])(void) =
{
    [0] = HandleEndTurn_ContinueBattle, //B_OUTCOME_NONE?
    [B_OUTCOME_WON] = HandleEndTurn_BattleWon,
    [B_OUTCOME_LOST] = HandleEndTurn_BattleLost,
    [B_OUTCOME_DREW] = HandleEndTurn_BattleLost,
    [B_OUTCOME_RAN] = HandleEndTurn_RanFromBattle,
    [B_OUTCOME_PLAYER_TELEPORTED] = HandleEndTurn_FinishBattle,
    [B_OUTCOME_MON_FLED] = HandleEndTurn_MonFled,
    [B_OUTCOME_CAUGHT] = HandleEndTurn_FinishBattle,
    [B_OUTCOME_NO_SAFARI_BALLS] = HandleEndTurn_FinishBattle,
    [B_OUTCOME_FORFEITED] = HandleEndTurn_FinishBattle,
    [B_OUTCOME_MON_TELEPORTED] = HandleEndTurn_FinishBattle,
};

const u8 gStatusConditionString_PoisonJpn[8] = _("どく$$$$$");
const u8 gStatusConditionString_SleepJpn[8] = _("ねむり$$$$");
const u8 gStatusConditionString_ParalysisJpn[8] = _("まひ$$$$$");
const u8 gStatusConditionString_BurnJpn[8] = _("やけど$$$$");
const u8 gStatusConditionString_IceJpn[8] = _("こおり$$$$");
const u8 gStatusConditionString_ConfusionJpn[8] = _("こんらん$$$");
const u8 gStatusConditionString_LoveJpn[8] = _("メロメロ$$$");

const u8 * const gStatusConditionStringsTable[7][2] =
{
    {gStatusConditionString_PoisonJpn, gText_Poison},
    {gStatusConditionString_SleepJpn, gText_Sleep},
    {gStatusConditionString_ParalysisJpn, gText_Paralysis},
    {gStatusConditionString_BurnJpn, gText_Burn},
    {gStatusConditionString_IceJpn, gText_Ice},
    {gStatusConditionString_ConfusionJpn, gText_Confusion},
    {gStatusConditionString_LoveJpn, gText_Love}
};

// code
void CB2_InitBattle(void)
{
    MoveSaveBlocks_ResetHeap();
    AllocateBattleResources();
    AllocateBattleSpritesData();
    AllocateMonSpritesGfx();
    RecordedBattle_ClearFrontierPassFlag();

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
        {
            CB2_InitBattleInternal();
        }
        else if (!(gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER))
        {
            HandleLinkBattleSetup();
            SetMainCallback2(CB2_PreInitMultiBattle);
        }
        else
        {
            SetMainCallback2(CB2_PreInitIngamePlayerPartnerBattle);
        }
        gBattleCommunication[MULTIUSE_STATE] = 0;
    }
    else
    {
        CB2_InitBattleInternal();
    }
}

static void CB2_InitBattleInternal(void)
{
    s32 i;

    SetHBlankCallback(NULL);
    SetVBlankCallback(NULL);

    CpuFill32(0, (void*)(VRAM), VRAM_SIZE);

    SetGpuReg(REG_OFFSET_MOSAIC, 0);
    SetGpuReg(REG_OFFSET_WIN0H, DISPLAY_WIDTH);
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(DISPLAY_HEIGHT / 2, DISPLAY_HEIGHT / 2 + 1));
    SetGpuReg(REG_OFFSET_WININ, 0);
    SetGpuReg(REG_OFFSET_WINOUT, 0);

    gBattle_WIN0H = DISPLAY_WIDTH;

    if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER && gPartnerTrainerId != TRAINER_STEVEN_PARTNER && gPartnerTrainerId < TRAINER_CUSTOM_PARTNER)
    {
        gBattle_WIN0V = DISPLAY_HEIGHT - 1;
        gBattle_WIN1H = DISPLAY_WIDTH;
        gBattle_WIN1V = 32;
    }
    else
    {
        gBattle_WIN0V = WIN_RANGE(DISPLAY_HEIGHT / 2, DISPLAY_HEIGHT / 2 + 1);
        ScanlineEffect_Clear();

        i = 0;
        while (i < 80)
        {
            gScanlineEffectRegBuffers[0][i] = 0xF0;
            gScanlineEffectRegBuffers[1][i] = 0xF0;
            i++;
        }

        while (i < 160)
        {
            gScanlineEffectRegBuffers[0][i] = 0xFF10;
            gScanlineEffectRegBuffers[1][i] = 0xFF10;
            i++;
        }

        ScanlineEffect_SetParams(sIntroScanlineParams16Bit);
    }

    ResetPaletteFade();
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gBattle_BG2_X = 0;
    gBattle_BG2_Y = 0;
    gBattle_BG3_X = 0;
    gBattle_BG3_Y = 0;

    gBattleTerrain = BattleSetup_GetTerrainId();
    if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
        gBattleTerrain = BATTLE_TERRAIN_BUILDING;

    InitBattleBgsVideo();
    LoadBattleTextboxAndBackground();
    ResetSpriteData();
    ResetTasks();
    DrawBattleEntryBackground();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 4;
    SetVBlankCallback(VBlankCB_Battle);
    SetUpBattleVarsAndBirchZigzagoon();

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI && gBattleTypeFlags & BATTLE_TYPE_BATTLE_TOWER)
        SetMainCallback2(CB2_HandleStartMultiPartnerBattle);
    else if (gBattleTypeFlags & BATTLE_TYPE_MULTI && gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
        SetMainCallback2(CB2_HandleStartMultiPartnerBattle);
    else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        SetMainCallback2(CB2_HandleStartMultiBattle);
    else
        SetMainCallback2(CB2_HandleStartBattle);

    if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED)))
    {
        CreateNPCTrainerParty(&gEnemyParty[0], gTrainerBattleOpponent_A, TRUE);
        if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS && !BATTLE_TWO_VS_ONE_OPPONENT)
            CreateNPCTrainerParty(&gEnemyParty[3], gTrainerBattleOpponent_B, FALSE);
        SetWildMonHeldItem();
    }

    gMain.inBattle = TRUE;
    gSaveBlock2Ptr->frontier.disableRecordBattle = FALSE;

    for (i = 0; i < PARTY_SIZE; i++)
        AdjustFriendship(&gPlayerParty[i], FRIENDSHIP_EVENT_LEAGUE_BATTLE);

    gBattleCommunication[MULTIUSE_STATE] = 0;
}

#define BUFFER_PARTY_VS_SCREEN_STATUS(party, flags, i)              \
    for ((i) = 0; (i) < PARTY_SIZE; (i)++)                          \
    {                                                               \
        u16 species = GetMonData(&(party)[(i)], MON_DATA_SPECIES2); \
        u16 hp = GetMonData(&(party)[(i)], MON_DATA_HP);            \
        u32 status = GetMonData(&(party)[(i)], MON_DATA_STATUS);    \
                                                                    \
        if (species == SPECIES_NONE)                                \
            continue;                                               \
                                                                    \
        /* Is healthy mon? */                                       \
        if (species != SPECIES_EGG && hp != 0 && status == 0)       \
            (flags) |= 1 << (i) * 2;                                \
                                                                    \
        if (species == SPECIES_NONE) /* Redundant */                \
            continue;                                               \
                                                                    \
        /* Is Egg or statused? */                                   \
        if (hp != 0 && (species == SPECIES_EGG || status != 0))     \
            (flags) |= 2 << (i) * 2;                                \
                                                                    \
        if (species == SPECIES_NONE) /* Redundant */                \
            continue;                                               \
                                                                    \
        /* Is fainted? */                                           \
        if (species != SPECIES_EGG && hp == 0)                      \
            (flags) |= 3 << (i) * 2;                                \
    }

// For Vs Screen at link battle start
static void BufferPartyVsScreenHealth_AtStart(void)
{
    u16 flags = 0;
    s32 i;

    BUFFER_PARTY_VS_SCREEN_STATUS(gPlayerParty, flags, i);
    gBattleStruct->multiBuffer.linkBattlerHeader.vsScreenHealthFlagsLo = flags;
    *(&gBattleStruct->multiBuffer.linkBattlerHeader.vsScreenHealthFlagsHi) = flags >> 8;
    gBattleStruct->multiBuffer.linkBattlerHeader.vsScreenHealthFlagsHi |= FlagGet(FLAG_SYS_FRONTIER_PASS) << 7;
}

static void SetPlayerBerryDataInBattleStruct(void)
{
    s32 i;
    struct BattleStruct *battleStruct = gBattleStruct;
    struct BattleEnigmaBerry *battleBerry = &battleStruct->multiBuffer.linkBattlerHeader.battleEnigmaBerry;

    if (IsEnigmaBerryValid() == TRUE)
    {
        for (i = 0; i < BERRY_NAME_LENGTH; i++)
            battleBerry->name[i] = gSaveBlock1Ptr->enigmaBerry.berry.name[i];
        battleBerry->name[i] = EOS;

        for (i = 0; i < BERRY_ITEM_EFFECT_COUNT; i++)
            battleBerry->itemEffect[i] = gSaveBlock1Ptr->enigmaBerry.itemEffect[i];

        battleBerry->holdEffect = gSaveBlock1Ptr->enigmaBerry.holdEffect;
        battleBerry->holdEffectParam = gSaveBlock1Ptr->enigmaBerry.holdEffectParam;
    }
    else
    {
        const struct Berry *berryData = GetBerryInfo(ItemIdToBerryType(ITEM_ENIGMA_BERRY));

        for (i = 0; i < BERRY_NAME_LENGTH; i++)
            battleBerry->name[i] = berryData->name[i];
        battleBerry->name[i] = EOS;

        for (i = 0; i < BERRY_ITEM_EFFECT_COUNT; i++)
            battleBerry->itemEffect[i] = 0;

        battleBerry->holdEffect = HOLD_EFFECT_NONE;
        battleBerry->holdEffectParam = 0;
    }
}

static void SetAllPlayersBerryData(void)
{
    s32 i;
    s32 j;

    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        if (IsEnigmaBerryValid() == TRUE)
        {
            for (i = 0; i < BERRY_NAME_LENGTH; i++)
            {
                gEnigmaBerries[0].name[i] = gSaveBlock1Ptr->enigmaBerry.berry.name[i];
                gEnigmaBerries[2].name[i] = gSaveBlock1Ptr->enigmaBerry.berry.name[i];
            }
            gEnigmaBerries[0].name[i] = EOS;
            gEnigmaBerries[2].name[i] = EOS;

            for (i = 0; i < BERRY_ITEM_EFFECT_COUNT; i++)
            {
                gEnigmaBerries[0].itemEffect[i] = gSaveBlock1Ptr->enigmaBerry.itemEffect[i];
                gEnigmaBerries[2].itemEffect[i] = gSaveBlock1Ptr->enigmaBerry.itemEffect[i];
            }

            gEnigmaBerries[0].holdEffect = gSaveBlock1Ptr->enigmaBerry.holdEffect;
            gEnigmaBerries[2].holdEffect = gSaveBlock1Ptr->enigmaBerry.holdEffect;
            gEnigmaBerries[0].holdEffectParam = gSaveBlock1Ptr->enigmaBerry.holdEffectParam;
            gEnigmaBerries[2].holdEffectParam = gSaveBlock1Ptr->enigmaBerry.holdEffectParam;
        }
        else
        {
            const struct Berry *berryData = GetBerryInfo(ItemIdToBerryType(ITEM_ENIGMA_BERRY));

            for (i = 0; i < BERRY_NAME_LENGTH; i++)
            {
                gEnigmaBerries[0].name[i] = berryData->name[i];
                gEnigmaBerries[2].name[i] = berryData->name[i];
            }
            gEnigmaBerries[0].name[i] = EOS;
            gEnigmaBerries[2].name[i] = EOS;

            for (i = 0; i < BERRY_ITEM_EFFECT_COUNT; i++)
            {
                gEnigmaBerries[0].itemEffect[i] = 0;
                gEnigmaBerries[2].itemEffect[i] = 0;
            }

            gEnigmaBerries[0].holdEffect = 0;
            gEnigmaBerries[2].holdEffect = 0;
            gEnigmaBerries[0].holdEffectParam = 0;
            gEnigmaBerries[2].holdEffectParam = 0;
        }
    }
    else
    {
        s32 numPlayers;
        struct BattleEnigmaBerry *src;
        u8 battlerId;

        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_BATTLE_TOWER)
                numPlayers = 2;
            else
                numPlayers = 4;

            for (i = 0; i < numPlayers; i++)
            {
                src = (struct BattleEnigmaBerry *)(gBlockRecvBuffer[i] + 2);
                battlerId = gLinkPlayers[i].id;

                for (j = 0; j < BERRY_NAME_LENGTH; j++)
                    gEnigmaBerries[battlerId].name[j] = src->name[j];
                gEnigmaBerries[battlerId].name[j] = EOS;

                for (j = 0; j < BERRY_ITEM_EFFECT_COUNT; j++)
                    gEnigmaBerries[battlerId].itemEffect[j] = src->itemEffect[j];

                gEnigmaBerries[battlerId].holdEffect = src->holdEffect;
                gEnigmaBerries[battlerId].holdEffectParam = src->holdEffectParam;
            }
        }
        else
        {
            for (i = 0; i < 2; i++)
            {
                src = (struct BattleEnigmaBerry *)(gBlockRecvBuffer[i] + 2);

                for (j = 0; j < BERRY_NAME_LENGTH; j++)
                {
                    gEnigmaBerries[i].name[j] = src->name[j];
                    gEnigmaBerries[i + 2].name[j] = src->name[j];
                }
                gEnigmaBerries[i].name[j] = EOS;
                gEnigmaBerries[i + 2].name[j] = EOS;

                for (j = 0; j < BERRY_ITEM_EFFECT_COUNT; j++)
                {
                    gEnigmaBerries[i].itemEffect[j] = src->itemEffect[j];
                    gEnigmaBerries[i + 2].itemEffect[j] = src->itemEffect[j];
                }

                gEnigmaBerries[i].holdEffect = src->holdEffect;
                gEnigmaBerries[i + 2].holdEffect = src->holdEffect;
                gEnigmaBerries[i].holdEffectParam = src->holdEffectParam;
                gEnigmaBerries[i + 2].holdEffectParam = src->holdEffectParam;
            }
        }
    }
}

// This was inlined in Ruby/Sapphire
static void FindLinkBattleMaster(u8 numPlayers, u8 multiPlayerId)
{
    u8 found = 0;

    // If player 1 is playing the minimum version, player 1 is master.
    if (gBlockRecvBuffer[0][0] == 0x100)
    {
        if (multiPlayerId == 0)
            gBattleTypeFlags |= BATTLE_TYPE_IS_MASTER | BATTLE_TYPE_TRAINER;
        else
            gBattleTypeFlags |= BATTLE_TYPE_TRAINER;
        found++;
    }

    if (found == 0)
    {
        // If multiple different versions are being used, player 1 is master.
        s32 i;

        for (i = 0; i < numPlayers; i++)
        {
            if (gBlockRecvBuffer[0][0] != gBlockRecvBuffer[i][0])
                break;
        }

        if (i == numPlayers)
        {
            if (multiPlayerId == 0)
                gBattleTypeFlags |= BATTLE_TYPE_IS_MASTER | BATTLE_TYPE_TRAINER;
            else
                gBattleTypeFlags |= BATTLE_TYPE_TRAINER;
            found++;
        }

        if (found == 0)
        {
            // Lowest index player with the highest game version is master.
            for (i = 0; i < numPlayers; i++)
            {
                if (gBlockRecvBuffer[i][0] == 0x300 && i != multiPlayerId)
                {
                    if (i < multiPlayerId)
                        break;
                }
                if (gBlockRecvBuffer[i][0] > 0x300 && i != multiPlayerId)
                    break;
            }

            if (i == numPlayers)
                gBattleTypeFlags |= BATTLE_TYPE_IS_MASTER | BATTLE_TYPE_TRAINER;
            else
                gBattleTypeFlags |= BATTLE_TYPE_TRAINER;
        }
    }
}

static void CB2_HandleStartBattle(void)
{
    u8 playerMultiplayerId;
    u8 enemyMultiplayerId;

    RunTasks();
    AnimateSprites();
    BuildOamBuffer();

    playerMultiplayerId = GetMultiplayerId();
    gBattleScripting.multiplayerId = playerMultiplayerId;
    enemyMultiplayerId = playerMultiplayerId ^ BIT_SIDE;

    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        if (!IsDma3ManagerBusyWithBgCopy())
        {
            ShowBg(0);
            ShowBg(1);
            ShowBg(2);
            ShowBg(3);
            sub_805EF14();
            gBattleCommunication[MULTIUSE_STATE] = 1;
        }
        if (gWirelessCommType)
            LoadWirelessStatusIndicatorSpriteGfx();
        break;
    case 1:
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            if (gReceivedRemoteLinkPlayers != 0)
            {
                if (IsLinkTaskFinished())
                {
                    // 0x300
                    *(&gBattleStruct->multiBuffer.linkBattlerHeader.versionSignatureLo) = 0;
                    *(&gBattleStruct->multiBuffer.linkBattlerHeader.versionSignatureHi) = 3;
                    BufferPartyVsScreenHealth_AtStart();
                    SetPlayerBerryDataInBattleStruct();

                    if (gTrainerBattleOpponent_A == TRAINER_UNION_ROOM)
                    {
                        gLinkPlayers[0].id = 0;
                        gLinkPlayers[1].id = 1;
                    }

                    SendBlock(bitmask_all_link_players_but_self(), &gBattleStruct->multiBuffer.linkBattlerHeader, sizeof(gBattleStruct->multiBuffer.linkBattlerHeader));
                    gBattleCommunication[MULTIUSE_STATE] = 2;
                }
                if (gWirelessCommType)
                    CreateWirelessStatusIndicatorSprite(0, 0);
            }
        }
        else
        {
            if (!(gBattleTypeFlags & BATTLE_TYPE_RECORDED))
                gBattleTypeFlags |= BATTLE_TYPE_IS_MASTER;
            gBattleCommunication[MULTIUSE_STATE] = 15;
            SetAllPlayersBerryData();
        }
        break;
    case 2:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            u8 taskId;

            ResetBlockReceivedFlags();
            FindLinkBattleMaster(2, playerMultiplayerId);
            SetAllPlayersBerryData();
            taskId = CreateTask(InitLinkBattleVsScreen, 0);
            gTasks[taskId].data[1] = 0x10E;
            gTasks[taskId].data[2] = 0x5A;
            gTasks[taskId].data[5] = 0;
            gTasks[taskId].data[3] = gBattleStruct->multiBuffer.linkBattlerHeader.vsScreenHealthFlagsLo | (gBattleStruct->multiBuffer.linkBattlerHeader.vsScreenHealthFlagsHi << 8);
            gTasks[taskId].data[4] = gBlockRecvBuffer[enemyMultiplayerId][1];
            RecordedBattle_SetFrontierPassFlagFromHword(gBlockRecvBuffer[playerMultiplayerId][1]);
            RecordedBattle_SetFrontierPassFlagFromHword(gBlockRecvBuffer[enemyMultiplayerId][1]);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 3:
        if (IsLinkTaskFinished())
        {
            SendBlock(bitmask_all_link_players_but_self(), gPlayerParty, sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 4:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            memcpy(gEnemyParty, gBlockRecvBuffer[enemyMultiplayerId], sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 7:
        if (IsLinkTaskFinished())
        {
            SendBlock(bitmask_all_link_players_but_self(), gPlayerParty + 2, sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 8:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            memcpy(gEnemyParty + 2, gBlockRecvBuffer[enemyMultiplayerId], sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 11:
        if (IsLinkTaskFinished())
        {
            SendBlock(bitmask_all_link_players_but_self(), gPlayerParty + 4, sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 12:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            memcpy(gEnemyParty + 4, gBlockRecvBuffer[enemyMultiplayerId], sizeof(struct Pokemon) * 2);
            TryCorrectShedinjaLanguage(&gEnemyParty[0]);
            TryCorrectShedinjaLanguage(&gEnemyParty[1]);
            TryCorrectShedinjaLanguage(&gEnemyParty[2]);
            TryCorrectShedinjaLanguage(&gEnemyParty[3]);
            TryCorrectShedinjaLanguage(&gEnemyParty[4]);
            TryCorrectShedinjaLanguage(&gEnemyParty[5]);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 15:
        InitBattleControllers();
        sub_8184E58();
        gBattleCommunication[SPRITES_INIT_STATE1] = 0;
        gBattleCommunication[SPRITES_INIT_STATE2] = 0;
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            s32 i;

            for (i = 0; i < 2 && (gLinkPlayers[i].version & 0xFF) == VERSION_EMERALD; i++);

            if (i == 2)
                gBattleCommunication[MULTIUSE_STATE] = 16;
            else
                gBattleCommunication[MULTIUSE_STATE] = 18;
        }
        else
        {
            gBattleCommunication[MULTIUSE_STATE] = 18;
        }
        break;
    case 16:
        if (IsLinkTaskFinished())
        {
            SendBlock(bitmask_all_link_players_but_self(), &gRecordedBattleRngSeed, sizeof(gRecordedBattleRngSeed));
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 17:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            if (!(gBattleTypeFlags & BATTLE_TYPE_IS_MASTER))
                memcpy(&gRecordedBattleRngSeed, gBlockRecvBuffer[enemyMultiplayerId], sizeof(gRecordedBattleRngSeed));
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 18:
        if (BattleInitAllSprites(&gBattleCommunication[SPRITES_INIT_STATE1], &gBattleCommunication[SPRITES_INIT_STATE2]))
        {
            gPreBattleCallback1 = gMain.callback1;
            gMain.callback1 = BattleMainCB1;
            SetMainCallback2(BattleMainCB2);
            if (gBattleTypeFlags & BATTLE_TYPE_LINK)
            {
                gBattleTypeFlags |= BATTLE_TYPE_LINK_IN_BATTLE;
            }
        }
        break;
    case 5:
    case 9:
    case 13:
        gBattleCommunication[MULTIUSE_STATE]++;
        gBattleCommunication[1] = 1;
    case 6:
    case 10:
    case 14:
        if (--gBattleCommunication[1] == 0)
            gBattleCommunication[MULTIUSE_STATE]++;
        break;
    }
}

static void CB2_HandleStartMultiPartnerBattle(void)
{
    u8 playerMultiplayerId;
    u8 enemyMultiplayerId;

    RunTasks();
    AnimateSprites();
    BuildOamBuffer();

    playerMultiplayerId = GetMultiplayerId();
    gBattleScripting.multiplayerId = playerMultiplayerId;
    enemyMultiplayerId = playerMultiplayerId ^ BIT_SIDE;

    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        if (!IsDma3ManagerBusyWithBgCopy())
        {
            ShowBg(0);
            ShowBg(1);
            ShowBg(2);
            ShowBg(3);
            sub_805EF14();
            gBattleCommunication[MULTIUSE_STATE] = 1;
        }
        if (gWirelessCommType)
            LoadWirelessStatusIndicatorSpriteGfx();
        // fall through
    case 1:
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            if (gReceivedRemoteLinkPlayers != 0)
            {
                u8 language;

                gLinkPlayers[0].id = 0;
                gLinkPlayers[1].id = 2;
                gLinkPlayers[2].id = 1;
                gLinkPlayers[3].id = 3;
                GetFrontierTrainerName(gLinkPlayers[2].name, gTrainerBattleOpponent_A);
                GetFrontierTrainerName(gLinkPlayers[3].name, gTrainerBattleOpponent_B);
                GetBattleTowerTrainerLanguage(&language, gTrainerBattleOpponent_A);
                gLinkPlayers[2].language = language;
                GetBattleTowerTrainerLanguage(&language, gTrainerBattleOpponent_B);
                gLinkPlayers[3].language = language;

                if (IsLinkTaskFinished())
                {
                    // 0x300
                    *(&gBattleStruct->multiBuffer.linkBattlerHeader.versionSignatureLo) = 0;
                    *(&gBattleStruct->multiBuffer.linkBattlerHeader.versionSignatureHi) = 3;
                    BufferPartyVsScreenHealth_AtStart();
                    SetPlayerBerryDataInBattleStruct();
                    SendBlock(bitmask_all_link_players_but_self(), &gBattleStruct->multiBuffer.linkBattlerHeader, sizeof(gBattleStruct->multiBuffer.linkBattlerHeader));
                    gBattleCommunication[MULTIUSE_STATE] = 2;
                }

                if (gWirelessCommType)
                    CreateWirelessStatusIndicatorSprite(0, 0);
            }
        }
        else
        {
            if (!(gBattleTypeFlags & BATTLE_TYPE_RECORDED))
                gBattleTypeFlags |= BATTLE_TYPE_IS_MASTER;
            gBattleCommunication[MULTIUSE_STATE] = 13;
            SetAllPlayersBerryData();
        }
        break;
    case 2:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            u8 taskId;

            ResetBlockReceivedFlags();
            FindLinkBattleMaster(2, playerMultiplayerId);
            SetAllPlayersBerryData();
            taskId = CreateTask(InitLinkBattleVsScreen, 0);
            gTasks[taskId].data[1] = 0x10E;
            gTasks[taskId].data[2] = 0x5A;
            gTasks[taskId].data[5] = 0;
            gTasks[taskId].data[3] = 0x145;
            gTasks[taskId].data[4] = 0x145;
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 3:
        if (IsLinkTaskFinished())
        {
            SendBlock(bitmask_all_link_players_but_self(), gPlayerParty, sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 4:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            if (gLinkPlayers[playerMultiplayerId].id != 0)
            {
                memcpy(gPlayerParty, gBlockRecvBuffer[enemyMultiplayerId], sizeof(struct Pokemon) * 2);
                memcpy(gPlayerParty + MULTI_PARTY_SIZE, gBlockRecvBuffer[playerMultiplayerId], sizeof(struct Pokemon) * 2);
            }
            else
            {
                memcpy(gPlayerParty, gBlockRecvBuffer[playerMultiplayerId], sizeof(struct Pokemon) * 2);
                memcpy(gPlayerParty + MULTI_PARTY_SIZE, gBlockRecvBuffer[enemyMultiplayerId], sizeof(struct Pokemon) * 2);
            }
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 5:
        if (IsLinkTaskFinished())
        {
            SendBlock(bitmask_all_link_players_but_self(), gPlayerParty + 2, sizeof(struct Pokemon));
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 6:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            if (gLinkPlayers[playerMultiplayerId].id != 0)
            {
                memcpy(gPlayerParty + 2, gBlockRecvBuffer[enemyMultiplayerId], sizeof(struct Pokemon));
                memcpy(gPlayerParty + 5, gBlockRecvBuffer[playerMultiplayerId], sizeof(struct Pokemon));
            }
            else
            {
                memcpy(gPlayerParty + 2, gBlockRecvBuffer[playerMultiplayerId], sizeof(struct Pokemon));
                memcpy(gPlayerParty + 5, gBlockRecvBuffer[enemyMultiplayerId], sizeof(struct Pokemon));
            }
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 7:
        if (IsLinkTaskFinished())
        {
            SendBlock(bitmask_all_link_players_but_self(), gEnemyParty, sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 8:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            if (GetMultiplayerId() != 0)
            {
                memcpy(gEnemyParty, gBlockRecvBuffer[0], sizeof(struct Pokemon) * 2);
            }
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 9:
        if (IsLinkTaskFinished())
        {
            SendBlock(bitmask_all_link_players_but_self(), gEnemyParty + 2, sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 10:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            if (GetMultiplayerId() != 0)
            {
                memcpy(gEnemyParty + 2, gBlockRecvBuffer[0], sizeof(struct Pokemon) * 2);
            }
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 11:
        if (IsLinkTaskFinished())
        {
            SendBlock(bitmask_all_link_players_but_self(), gEnemyParty + 4, sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 12:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            if (GetMultiplayerId() != 0)
                memcpy(gEnemyParty + 4, gBlockRecvBuffer[0], sizeof(struct Pokemon) * 2);
            TryCorrectShedinjaLanguage(&gPlayerParty[0]);
            TryCorrectShedinjaLanguage(&gPlayerParty[1]);
            TryCorrectShedinjaLanguage(&gPlayerParty[2]);
            TryCorrectShedinjaLanguage(&gPlayerParty[3]);
            TryCorrectShedinjaLanguage(&gPlayerParty[4]);
            TryCorrectShedinjaLanguage(&gPlayerParty[5]);
            TryCorrectShedinjaLanguage(&gEnemyParty[0]);
            TryCorrectShedinjaLanguage(&gEnemyParty[1]);
            TryCorrectShedinjaLanguage(&gEnemyParty[2]);
            TryCorrectShedinjaLanguage(&gEnemyParty[3]);
            TryCorrectShedinjaLanguage(&gEnemyParty[4]);
            TryCorrectShedinjaLanguage(&gEnemyParty[5]);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 13:
        InitBattleControllers();
        sub_8184E58();
        gBattleCommunication[SPRITES_INIT_STATE1] = 0;
        gBattleCommunication[SPRITES_INIT_STATE2] = 0;
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            gBattleCommunication[MULTIUSE_STATE] = 14;
        }
        else
        {
            gBattleCommunication[MULTIUSE_STATE] = 16;
        }
        break;
    case 14:
        if (IsLinkTaskFinished())
        {
            SendBlock(bitmask_all_link_players_but_self(), &gRecordedBattleRngSeed, sizeof(gRecordedBattleRngSeed));
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 15:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            if (!(gBattleTypeFlags & BATTLE_TYPE_IS_MASTER))
                memcpy(&gRecordedBattleRngSeed, gBlockRecvBuffer[enemyMultiplayerId], sizeof(gRecordedBattleRngSeed));
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 16:
        if (BattleInitAllSprites(&gBattleCommunication[SPRITES_INIT_STATE1], &gBattleCommunication[SPRITES_INIT_STATE2]))
        {
            TrySetLinkBattleTowerEnemyPartyLevel();
            gPreBattleCallback1 = gMain.callback1;
            gMain.callback1 = BattleMainCB1;
            SetMainCallback2(BattleMainCB2);
            if (gBattleTypeFlags & BATTLE_TYPE_LINK)
            {
                gBattleTypeFlags |= BATTLE_TYPE_LINK_IN_BATTLE;
            }
        }
        break;
    }
}

static void sub_80379F8(u8 arrayIdPlus)
{
    s32 i;

    for (i = 0; i < (int)ARRAY_COUNT(gMultiPartnerParty); i++)
    {
        gMultiPartnerParty[i].species     = GetMonData(&gPlayerParty[arrayIdPlus + i], MON_DATA_SPECIES);
        gMultiPartnerParty[i].heldItem    = GetMonData(&gPlayerParty[arrayIdPlus + i], MON_DATA_HELD_ITEM);
        GetMonData(&gPlayerParty[arrayIdPlus + i], MON_DATA_NICKNAME, gMultiPartnerParty[i].nickname);
        gMultiPartnerParty[i].level       = GetMonData(&gPlayerParty[arrayIdPlus + i], MON_DATA_LEVEL);
        gMultiPartnerParty[i].hp          = GetMonData(&gPlayerParty[arrayIdPlus + i], MON_DATA_HP);
        gMultiPartnerParty[i].maxhp       = GetMonData(&gPlayerParty[arrayIdPlus + i], MON_DATA_MAX_HP);
        gMultiPartnerParty[i].status      = GetMonData(&gPlayerParty[arrayIdPlus + i], MON_DATA_STATUS);
        gMultiPartnerParty[i].personality = GetMonData(&gPlayerParty[arrayIdPlus + i], MON_DATA_PERSONALITY);
        gMultiPartnerParty[i].gender      = GetMonGender(&gPlayerParty[arrayIdPlus + i]);
        StripExtCtrlCodes(gMultiPartnerParty[i].nickname);
        if (GetMonData(&gPlayerParty[arrayIdPlus + i], MON_DATA_LANGUAGE) != LANGUAGE_JAPANESE)
            PadNameString(gMultiPartnerParty[i].nickname, CHAR_SPACE);
    }
    memcpy(sMultiPartnerPartyBuffer, gMultiPartnerParty, sizeof(gMultiPartnerParty));
}

static void CB2_PreInitMultiBattle(void)
{
    s32 i;
    u8 playerMultiplierId;
    s32 numPlayers = 4;
    u8 r4 = 0xF;
    u32 *savedBattleTypeFlags;
    void (**savedCallback)(void);

    if (gBattleTypeFlags & BATTLE_TYPE_BATTLE_TOWER)
    {
        numPlayers = 2;
        r4 = 3;
    }

    playerMultiplierId = GetMultiplayerId();
    gBattleScripting.multiplayerId = playerMultiplierId;
    savedCallback = &gBattleStruct->savedCallback;
    savedBattleTypeFlags = &gBattleStruct->savedBattleTypeFlags;

    RunTasks();
    AnimateSprites();
    BuildOamBuffer();

    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        if (gReceivedRemoteLinkPlayers != 0 && IsLinkTaskFinished())
        {
            sMultiPartnerPartyBuffer = Alloc(sizeof(struct UnknownPokemonStruct4) * ARRAY_COUNT(gMultiPartnerParty));
            sub_80379F8(0);
            SendBlock(bitmask_all_link_players_but_self(), sMultiPartnerPartyBuffer, sizeof(struct UnknownPokemonStruct4) * ARRAY_COUNT(gMultiPartnerParty));
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 1:
        if ((GetBlockReceivedStatus() & r4) == r4)
        {
            ResetBlockReceivedFlags();
            for (i = 0; i < numPlayers; i++)
            {
                if (i == playerMultiplierId)
                    continue;

                if (numPlayers == MAX_LINK_PLAYERS)
                {
                    if ((!(gLinkPlayers[i].id & 1) && !(gLinkPlayers[playerMultiplierId].id & 1))
                        || (gLinkPlayers[i].id & 1 && gLinkPlayers[playerMultiplierId].id & 1))
                    {
                        memcpy(gMultiPartnerParty, gBlockRecvBuffer[i], sizeof(struct UnknownPokemonStruct4) * ARRAY_COUNT(gMultiPartnerParty));
                    }
                }
                else
                {
                    memcpy(gMultiPartnerParty, gBlockRecvBuffer[i], sizeof(struct UnknownPokemonStruct4) * ARRAY_COUNT(gMultiPartnerParty));
                }
            }
            gBattleCommunication[MULTIUSE_STATE]++;
            *savedCallback = gMain.savedCallback;
            *savedBattleTypeFlags = gBattleTypeFlags;
            gMain.savedCallback = CB2_PreInitMultiBattle;
            ShowPartyMenuToShowcaseMultiBattleParty();
        }
        break;
    case 2:
        if (IsLinkTaskFinished() && !gPaletteFade.active)
        {
            gBattleCommunication[MULTIUSE_STATE]++;
            if (gWirelessCommType)
                SetLinkStandbyCallback();
            else
                SetCloseLinkCallback();
        }
        break;
    case 3:
        if (gWirelessCommType)
        {
            if (IsLinkRfuTaskFinished())
            {
                gBattleTypeFlags = *savedBattleTypeFlags;
                gMain.savedCallback = *savedCallback;
                SetMainCallback2(CB2_InitBattleInternal);
                Free(sMultiPartnerPartyBuffer);
                sMultiPartnerPartyBuffer = NULL;
            }
        }
        else if (gReceivedRemoteLinkPlayers == 0)
        {
            gBattleTypeFlags = *savedBattleTypeFlags;
            gMain.savedCallback = *savedCallback;
            SetMainCallback2(CB2_InitBattleInternal);
            Free(sMultiPartnerPartyBuffer);
            sMultiPartnerPartyBuffer = NULL;
        }
        break;
    }
}

static void CB2_PreInitIngamePlayerPartnerBattle(void)
{
    u32 *savedBattleTypeFlags;
    void (**savedCallback)(void);

    savedCallback = &gBattleStruct->savedCallback;
    savedBattleTypeFlags = &gBattleStruct->savedBattleTypeFlags;

    RunTasks();
    AnimateSprites();
    BuildOamBuffer();

    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        sMultiPartnerPartyBuffer = Alloc(sizeof(struct UnknownPokemonStruct4) * ARRAY_COUNT(gMultiPartnerParty));
        sub_80379F8(3);
        gBattleCommunication[MULTIUSE_STATE]++;
        *savedCallback = gMain.savedCallback;
        *savedBattleTypeFlags = gBattleTypeFlags;
        gMain.savedCallback = CB2_PreInitIngamePlayerPartnerBattle;
        ShowPartyMenuToShowcaseMultiBattleParty();
        break;
    case 1:
        if (!gPaletteFade.active)
        {
            gBattleCommunication[MULTIUSE_STATE] = 2;
            gBattleTypeFlags = *savedBattleTypeFlags;
            gMain.savedCallback = *savedCallback;
            SetMainCallback2(CB2_InitBattleInternal);
            Free(sMultiPartnerPartyBuffer);
            sMultiPartnerPartyBuffer = NULL;
        }
        break;
    }
}

static void CB2_HandleStartMultiBattle(void)
{
    u8 playerMultiplayerId;
    s32 id;
    u8 var;

    playerMultiplayerId = GetMultiplayerId();
    gBattleScripting.multiplayerId = playerMultiplayerId;

    RunTasks();
    AnimateSprites();
    BuildOamBuffer();

    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        if (!IsDma3ManagerBusyWithBgCopy())
        {
            ShowBg(0);
            ShowBg(1);
            ShowBg(2);
            ShowBg(3);
            sub_805EF14();
            gBattleCommunication[MULTIUSE_STATE] = 1;
        }
        if (gWirelessCommType)
            LoadWirelessStatusIndicatorSpriteGfx();
        break;
    case 1:
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            if (gReceivedRemoteLinkPlayers != 0)
            {
                if (IsLinkTaskFinished())
                {
                    // 0x300
                    *(&gBattleStruct->multiBuffer.linkBattlerHeader.versionSignatureLo) = 0;
                    *(&gBattleStruct->multiBuffer.linkBattlerHeader.versionSignatureHi) = 3;
                    BufferPartyVsScreenHealth_AtStart();
                    SetPlayerBerryDataInBattleStruct();

                    SendBlock(bitmask_all_link_players_but_self(), &gBattleStruct->multiBuffer.linkBattlerHeader, sizeof(gBattleStruct->multiBuffer.linkBattlerHeader));
                    gBattleCommunication[MULTIUSE_STATE]++;
                }
                if (gWirelessCommType)
                    CreateWirelessStatusIndicatorSprite(0, 0);
            }
        }
        else
        {
            if (!(gBattleTypeFlags & BATTLE_TYPE_RECORDED))
                gBattleTypeFlags |= BATTLE_TYPE_IS_MASTER;
            gBattleCommunication[MULTIUSE_STATE] = 7;
            SetAllPlayersBerryData();
        }
        break;
    case 2:
        if ((GetBlockReceivedStatus() & 0xF) == 0xF)
        {
            ResetBlockReceivedFlags();
            FindLinkBattleMaster(4, playerMultiplayerId);
            SetAllPlayersBerryData();
            var = CreateTask(InitLinkBattleVsScreen, 0);
            gTasks[var].data[1] = 0x10E;
            gTasks[var].data[2] = 0x5A;
            gTasks[var].data[5] = 0;
            gTasks[var].data[3] = 0;
            gTasks[var].data[4] = 0;

            for (id = 0; id < MAX_LINK_PLAYERS; id++)
            {
                RecordedBattle_SetFrontierPassFlagFromHword(gBlockRecvBuffer[id][1]);
                switch (gLinkPlayers[id].id)
                {
                case 0:
                    gTasks[var].data[3] |= gBlockRecvBuffer[id][1] & 0x3F;
                    break;
                case 1:
                    gTasks[var].data[4] |= gBlockRecvBuffer[id][1] & 0x3F;
                    break;
                case 2:
                    gTasks[var].data[3] |= (gBlockRecvBuffer[id][1] & 0x3F) << 6;
                    break;
                case 3:
                    gTasks[var].data[4] |= (gBlockRecvBuffer[id][1] & 0x3F) << 6;
                    break;
                }
            }
            ZeroEnemyPartyMons();
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        else
            break;
        // fall through
    case 3:
        if (IsLinkTaskFinished())
        {
            SendBlock(bitmask_all_link_players_but_self(), gPlayerParty, sizeof(struct Pokemon) * 2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 4:
        if ((GetBlockReceivedStatus() & 0xF) == 0xF)
        {
            ResetBlockReceivedFlags();
            for (id = 0; id < MAX_LINK_PLAYERS; id++)
            {
                if (id == playerMultiplayerId)
                {
                    switch (gLinkPlayers[id].id)
                    {
                    case 0:
                    case 3:
                        memcpy(gPlayerParty, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                        break;
                    case 1:
                    case 2:
                        memcpy(gPlayerParty + MULTI_PARTY_SIZE, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                        break;
                    }
                }
                else
                {
                    if ((!(gLinkPlayers[id].id & 1) && !(gLinkPlayers[playerMultiplayerId].id & 1))
                     || ((gLinkPlayers[id].id & 1) && (gLinkPlayers[playerMultiplayerId].id & 1)))
                    {
                        switch (gLinkPlayers[id].id)
                        {
                        case 0:
                        case 3:
                            memcpy(gPlayerParty, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                            break;
                        case 1:
                        case 2:
                            memcpy(gPlayerParty + MULTI_PARTY_SIZE, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                            break;
                        }
                    }
                    else
                    {
                        switch (gLinkPlayers[id].id)
                        {
                        case 0:
                        case 3:
                            memcpy(gEnemyParty, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                            break;
                        case 1:
                        case 2:
                            memcpy(gEnemyParty + MULTI_PARTY_SIZE, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                            break;
                        }
                    }
                }
            }
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 5:
        if (IsLinkTaskFinished())
        {
            SendBlock(bitmask_all_link_players_but_self(), gPlayerParty + 2, sizeof(struct Pokemon));
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 6:
        if ((GetBlockReceivedStatus() & 0xF) == 0xF)
        {
            ResetBlockReceivedFlags();
            for (id = 0; id < MAX_LINK_PLAYERS; id++)
            {
                if (id == playerMultiplayerId)
                {
                    switch (gLinkPlayers[id].id)
                    {
                    case 0:
                    case 3:
                        memcpy(gPlayerParty + 2, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                        break;
                    case 1:
                    case 2:
                        memcpy(gPlayerParty + 5, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                        break;
                    }
                }
                else
                {
                    if ((!(gLinkPlayers[id].id & 1) && !(gLinkPlayers[playerMultiplayerId].id & 1))
                     || ((gLinkPlayers[id].id & 1) && (gLinkPlayers[playerMultiplayerId].id & 1)))
                    {
                        switch (gLinkPlayers[id].id)
                        {
                        case 0:
                        case 3:
                            memcpy(gPlayerParty + 2, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                            break;
                        case 1:
                        case 2:
                            memcpy(gPlayerParty + 5, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                            break;
                        }
                    }
                    else
                    {
                        switch (gLinkPlayers[id].id)
                        {
                        case 0:
                        case 3:
                            memcpy(gEnemyParty + 2, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                            break;
                        case 1:
                        case 2:
                            memcpy(gEnemyParty + 5, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                            break;
                        }
                    }
                }
            }
            TryCorrectShedinjaLanguage(&gPlayerParty[0]);
            TryCorrectShedinjaLanguage(&gPlayerParty[1]);
            TryCorrectShedinjaLanguage(&gPlayerParty[2]);
            TryCorrectShedinjaLanguage(&gPlayerParty[3]);
            TryCorrectShedinjaLanguage(&gPlayerParty[4]);
            TryCorrectShedinjaLanguage(&gPlayerParty[5]);

            TryCorrectShedinjaLanguage(&gEnemyParty[0]);
            TryCorrectShedinjaLanguage(&gEnemyParty[1]);
            TryCorrectShedinjaLanguage(&gEnemyParty[2]);
            TryCorrectShedinjaLanguage(&gEnemyParty[3]);
            TryCorrectShedinjaLanguage(&gEnemyParty[4]);
            TryCorrectShedinjaLanguage(&gEnemyParty[5]);

            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 7:
        InitBattleControllers();
        sub_8184E58();
        gBattleCommunication[SPRITES_INIT_STATE1] = 0;
        gBattleCommunication[SPRITES_INIT_STATE2] = 0;
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            for (id = 0; id < MAX_LINK_PLAYERS && (gLinkPlayers[id].version & 0xFF) == VERSION_EMERALD; id++);

            if (id == MAX_LINK_PLAYERS)
                gBattleCommunication[MULTIUSE_STATE] = 8;
            else
                gBattleCommunication[MULTIUSE_STATE] = 10;
        }
        else
        {
            gBattleCommunication[MULTIUSE_STATE] = 10;
        }
        break;
    case 8:
        if (IsLinkTaskFinished())
        {
            u32* ptr = gBattleStruct->multiBuffer.battleVideo;
            ptr[0] = gBattleTypeFlags;
            ptr[1] = gRecordedBattleRngSeed; // UB: overwrites berry data
            SendBlock(bitmask_all_link_players_but_self(), ptr, sizeof(gBattleStruct->multiBuffer.battleVideo));
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 9:
        if ((GetBlockReceivedStatus() & 0xF) == 0xF)
        {
            ResetBlockReceivedFlags();
            for (var = 0; var < 4; var++)
            {
                u32 blockValue = gBlockRecvBuffer[var][0];
                if (blockValue & 4)
                {
                    memcpy(&gRecordedBattleRngSeed, &gBlockRecvBuffer[var][2], sizeof(gRecordedBattleRngSeed));
                    break;
                }
            }

            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 10:
        if (BattleInitAllSprites(&gBattleCommunication[SPRITES_INIT_STATE1], &gBattleCommunication[SPRITES_INIT_STATE2]))
        {
            gPreBattleCallback1 = gMain.callback1;
            gMain.callback1 = BattleMainCB1;
            SetMainCallback2(BattleMainCB2);
            if (gBattleTypeFlags & BATTLE_TYPE_LINK)
            {
                gTrainerBattleOpponent_A = TRAINER_LINK_OPPONENT;
                gBattleTypeFlags |= BATTLE_TYPE_LINK_IN_BATTLE;
            }
        }
        break;
    }
}

void BattleMainCB2(void)
{
    AnimateSprites();
    BuildOamBuffer();
    RunTextPrinters();
    UpdatePaletteFade();
    RunTasks();

    if (JOY_HELD(B_BUTTON) && gBattleTypeFlags & BATTLE_TYPE_RECORDED && sub_8186450())
    {
        gSpecialVar_Result = gBattleOutcome = B_OUTCOME_PLAYER_TELEPORTED;
        ResetPaletteFadeControl();
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
        SetMainCallback2(CB2_QuitRecordedBattle);
    }
}

static void FreeRestoreBattleData(void)
{
    gMain.callback1 = gPreBattleCallback1;
    gScanlineEffect.state = 3;
    gMain.inBattle = 0;
    ZeroEnemyPartyMons();
    m4aSongNumStop(SE_LOW_HEALTH);
    FreeMonSpritesGfx();
    FreeBattleSpritesData();
    FreeBattleResources();
}

void CB2_QuitRecordedBattle(void)
{
    UpdatePaletteFade();
    if (!gPaletteFade.active)
    {
        m4aMPlayStop(&gMPlayInfo_SE1);
        m4aMPlayStop(&gMPlayInfo_SE2);
        FreeRestoreBattleData();
        FreeAllWindowBuffers();
        SetMainCallback2(gMain.savedCallback);
    }
}

void sub_8038528(struct Sprite* sprite)
{
    sprite->data[0] = 0;
    sprite->callback = sub_8038538;
}

static void sub_8038538(struct Sprite *sprite)
{
    u16 *arr = (u16*)(gDecompressionBuffer);

    switch (sprite->data[0])
    {
    case 0:
        sprite->data[0]++;
        sprite->data[1] = 0;
        sprite->data[2] = 0x281;
        sprite->data[3] = 0;
        sprite->data[4] = 1;
        // fall through
    case 1:
        sprite->data[4]--;
        if (sprite->data[4] == 0)
        {
            s32 i;
            s32 r2;
            s32 r0;

            sprite->data[4] = 2;
            r2 = sprite->data[1] + sprite->data[3] * 32;
            r0 = sprite->data[2] - sprite->data[3] * 32;
            for (i = 0; i < 29; i += 2)
            {
                arr[r2 + i] = 0x3D;
                arr[r0 + i] = 0x3D;
            }
            sprite->data[3]++;
            if (sprite->data[3] == 21)
            {
                sprite->data[0]++;
                sprite->data[1] = 32;
            }
        }
        break;
    case 2:
        sprite->data[1]--;
        if (sprite->data[1] == 20)
            SetMainCallback2(CB2_InitBattle);
        break;
    }
}

static u8 CreateNPCTrainerParty(struct Pokemon *party, u16 trainerNum, bool8 firstTrainer)
{
    u32 nameHash = 0;
    u32 personalityValue;
    u8 fixedIV;
    s32 i, j = 0;
    u8 monsCount;
    u8 level;
    u8 friendship;
    u8 difficultySetting = gSaveBlock2Ptr->gameDifficulty;
    	
    u16 move = 1;
    u16 species = 1;
    u16 heldItem = ITEM_NONE;
    u8 ability = 0;

    if (trainerNum == TRAINER_SECRET_BASE)
        return 0;

    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && !(gBattleTypeFlags & (BATTLE_TYPE_FRONTIER
                                                                        | BATTLE_TYPE_EREADER_TRAINER
                                                                        | BATTLE_TYPE_TRAINER_HILL)))
    {
        if (firstTrainer == TRUE)
            ZeroEnemyPartyMons();

        if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
        {
            if (gTrainers[trainerNum].partySize > 3)
                monsCount = 3;
            else
                monsCount = gTrainers[trainerNum].partySize;
        }
        else
        {
            monsCount = gTrainers[trainerNum].partySize;
        }

        for (i = 0; i < monsCount; i++)
        {

            if (gTrainers[trainerNum].doubleBattle == TRUE)
                personalityValue = 0x80;
            else if (gTrainers[trainerNum].encounterMusic_gender & F_TRAINER_FEMALE)
                personalityValue = 0x78; // Use personality more likely to result in a female Pokémon
            else
                personalityValue = 0x88; // Use personality more likely to result in a male Pokémon

            for (j = 0; gTrainers[trainerNum].trainerName[j] != EOS; j++)
                nameHash += gTrainers[trainerNum].trainerName[j];

            switch (gTrainers[trainerNum].partyFlags)
            {
            case 0:
            {
                const struct TrainerMonNoItemDefaultMoves *partyData = gTrainers[trainerNum].party.NoItemDefaultMoves;

                for (j = 0; gSpeciesNames[partyData[i].species][j] != EOS; j++)
                    nameHash += gSpeciesNames[partyData[i].species][j];

                personalityValue += nameHash << 8;
                fixedIV = partyData[i].iv * MAX_PER_STAT_IVS / 255;
                CreateMon(&party[i], partyData[i].species, partyData[i].lvl, fixedIV, TRUE, personalityValue, OT_ID_RANDOM_NO_SHINY, 0);
                break;
            }
            case F_TRAINER_PARTY_CUSTOM_MOVESET:
            {
                const struct TrainerMonNoItemCustomMoves *partyData = gTrainers[trainerNum].party.NoItemCustomMoves;

                for (j = 0; gSpeciesNames[partyData[i].species][j] != EOS; j++)
                    nameHash += gSpeciesNames[partyData[i].species][j];

                personalityValue += nameHash << 8;
                fixedIV = partyData[i].iv * MAX_PER_STAT_IVS / 255;
                CreateMon(&party[i], partyData[i].species, partyData[i].lvl, fixedIV, TRUE, personalityValue, OT_ID_RANDOM_NO_SHINY, 0);

                for (j = 0; j < MAX_MON_MOVES; j++)
                {
                    SetMonData(&party[i], MON_DATA_MOVE1 + j, &partyData[i].moves[j]);
                    SetMonData(&party[i], MON_DATA_PP1 + j, &gBattleMoves[partyData[i].moves[j]].pp);
                }
                break;
            }
            case F_TRAINER_PARTY_HELD_ITEM:
            {
                const struct TrainerMonItemDefaultMoves *partyData = gTrainers[trainerNum].party.ItemDefaultMoves;

                for (j = 0; gSpeciesNames[partyData[i].species][j] != EOS; j++)
                    nameHash += gSpeciesNames[partyData[i].species][j];

                personalityValue += nameHash << 8;
                fixedIV = partyData[i].iv * MAX_PER_STAT_IVS / 255;
                CreateMon(&party[i], partyData[i].species, partyData[i].lvl, fixedIV, TRUE, personalityValue, OT_ID_RANDOM_NO_SHINY, 0);

                SetMonData(&party[i], MON_DATA_HELD_ITEM, &partyData[i].heldItem);
                break;
            }
            case F_TRAINER_PARTY_CUSTOM_MOVESET | F_TRAINER_PARTY_HELD_ITEM:
            {
                const struct TrainerMonItemCustomMoves *partyData = gTrainers[trainerNum].party.ItemCustomMoves;

                for (j = 0; gSpeciesNames[partyData[i].species][j] != EOS; j++)
                    nameHash += gSpeciesNames[partyData[i].species][j];

                personalityValue += nameHash << 8;

                level = GetHighestLevelInPlayerParty();
                if (level + partyData[i].lvl > 100)
                {
                    level = 100;
                }
                else if (level + partyData[i].lvl < 1)
                {
                    level = 1;
                }
                else
                {
                    level = level + partyData[i].lvl;
                }
		if (trainerNum == TRAINER_OLDPLAYER)
		{			
			species = pickOldplayerGeneralSpecies(i);
			CreateMon(&party[i], species, getNewPokemonLevel(species, 50), 31, TRUE, personalityValue, OT_ID_RANDOM_NO_SHINY, 0);
			heldItem = getHeldItem(species);
			SetMonData(&party[i], MON_DATA_HELD_ITEM, &heldItem);
			ability = Random() % 3;
                	SetMonData(&party[i], MON_DATA_ABILITY_NUM, &ability);
		}
		else
		{
			CreateMon(&party[i], partyData[i].species, level, 31, TRUE, personalityValue, OT_ID_RANDOM_NO_SHINY, 0);
			SetMonData(&party[i], MON_DATA_HELD_ITEM, &partyData[i].heldItem);
			SetMonData(&party[i], MON_DATA_ABILITY_NUM, &partyData[i].ability);
		}
		SetMonData(&party[i], MON_DATA_NATURE, &gSets[partyData[i].spread].nature);

                // Set IVs from premade spreads
                for (j = 0; j < 6; j++)
                {
                    SetMonData(&party[i], MON_DATA_HP_IV + j, &gSets[partyData[i].spread].IVs[j]);
                }

                // Set EVs from premade spreads if difficulty is Hard or higher
                // Separate loops so that difficulty is not checked in each loop
                if (difficultySetting > DIFFICULTY_NORMAL)
                {
                    for (j = 0; j < 6; j++)
                    {
                        SetMonData(&party[i], MON_DATA_HP_EV + j, &gSets[partyData[i].spread].EVs[j]);
                    }
                }

                CalculateMonStats(&party[i]); // called twice; fix in future
		    
		if (trainerNum == TRAINER_OLDPLAYER)
		{
			for (j = 0; j < MAX_MON_MOVES; j++)
			{
			    move = selectMoves(species, j, GetMonData(&party[i], MON_DATA_ATK, NULL), GetMonData(&party[i], MON_DATA_SPATK, NULL));		
			    SetMonData(&party[i], MON_DATA_MOVE1 + j, &move);
			    SetMonData(&party[i], MON_DATA_PP1 + j, &gBattleMoves[GetMonData(&party[i], MON_DATA_MOVE1 + j, NULL)].pp);
			}
			if (GetMonData(&party[i], MON_DATA_MOVE2, NULL) == MOVE_REST)
			{
			    move = MOVE_SLEEP_TALK;
			    SetMonData(&party[i], MON_DATA_MOVE3, &move);
			    SetMonData(&party[i], MON_DATA_PP3, &gBattleMoves[move].pp);
			}			
		}
		else
		{
			for (j = 0; j < MAX_MON_MOVES; j++)
			{
			    SetMonData(&party[i], MON_DATA_MOVE1 + j, &partyData[i].moves[j]);
			    SetMonData(&party[i], MON_DATA_PP1 + j, &gBattleMoves[partyData[i].moves[j]].pp);
			}
		}
                // Set max friendship if trainer mon knows Return /  Added Veevee Volley and Pika Papow to the list
                if (MonKnowsMove(&party[i], MOVE_RETURN) || MonKnowsMove(&party[i], MOVE_VEEVEE_VOLLEY) || MonKnowsMove(&party[i], MOVE_PIKA_PAPOW))
                {
                    friendship = MAX_FRIENDSHIP;
                    SetMonData(&party[i], MON_DATA_FRIENDSHIP, &friendship);
                }
                break;
            }
            }
            for (j = 0; gTrainerBallTable[j].classId != 0xFF; j++)
            {
                if (gTrainerBallTable[j].classId == gTrainers[trainerNum].trainerClass)
                    break;
            }
            SetMonData(&party[i], MON_DATA_POKEBALL, &gTrainerBallTable[j].Ball);
        }

        gBattleTypeFlags |= gTrainers[trainerNum].doubleBattle;
    }
    return gTrainers[trainerNum].partySize;
}

void VBlankCB_Battle(void)
{
    // Change gRngSeed every vblank unless the battle could be recorded.
    if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_FRONTIER | BATTLE_TYPE_RECORDED)))
        Random();

    SetGpuReg(REG_OFFSET_BG0HOFS, gBattle_BG0_X);
    SetGpuReg(REG_OFFSET_BG0VOFS, gBattle_BG0_Y);
    SetGpuReg(REG_OFFSET_BG1HOFS, gBattle_BG1_X);
    SetGpuReg(REG_OFFSET_BG1VOFS, gBattle_BG1_Y);
    SetGpuReg(REG_OFFSET_BG2HOFS, gBattle_BG2_X);
    SetGpuReg(REG_OFFSET_BG2VOFS, gBattle_BG2_Y);
    SetGpuReg(REG_OFFSET_BG3HOFS, gBattle_BG3_X);
    SetGpuReg(REG_OFFSET_BG3VOFS, gBattle_BG3_Y);
    SetGpuReg(REG_OFFSET_WIN0H, gBattle_WIN0H);
    SetGpuReg(REG_OFFSET_WIN0V, gBattle_WIN0V);
    SetGpuReg(REG_OFFSET_WIN1H, gBattle_WIN1H);
    SetGpuReg(REG_OFFSET_WIN1V, gBattle_WIN1V);
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ScanlineEffect_InitHBlankDmaTransfer();
}

void SpriteCB_VsLetterDummy(struct Sprite *sprite)
{

}

static void SpriteCB_VsLetter(struct Sprite *sprite)
{
    if (sprite->data[0] != 0)
        sprite->x = sprite->data[1] + ((sprite->data[2] & 0xFF00) >> 8);
    else
        sprite->x = sprite->data[1] - ((sprite->data[2] & 0xFF00) >> 8);

    sprite->data[2] += 0x180;

    if (sprite->affineAnimEnded)
    {
        FreeSpriteTilesByTag(ANIM_SPRITES_START);
        FreeSpritePaletteByTag(ANIM_SPRITES_START);
        FreeSpriteOamMatrix(sprite);
        DestroySprite(sprite);
    }
}

void SpriteCB_VsLetterInit(struct Sprite *sprite)
{
    StartSpriteAffineAnim(sprite, 1);
    sprite->callback = SpriteCB_VsLetter;
    PlaySE(SE_MUGSHOT);
}

static void BufferPartyVsScreenHealth_AtEnd(u8 taskId)
{
    struct Pokemon *party1 = NULL;
    struct Pokemon *party2 = NULL;
    u8 multiplayerId = gBattleScripting.multiplayerId;
    u32 flags;
    s32 i;

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        switch (gLinkPlayers[multiplayerId].id)
        {
        case 0:
        case 2:
            party1 = gPlayerParty;
            party2 = gEnemyParty;
            break;
        case 1:
        case 3:
            party1 = gEnemyParty;
            party2 = gPlayerParty;
            break;
        }
    }
    else
    {
        party1 = gPlayerParty;
        party2 = gEnemyParty;
    }

    flags = 0;
    BUFFER_PARTY_VS_SCREEN_STATUS(party1, flags, i);
    gTasks[taskId].data[3] = flags;

    flags = 0;
    BUFFER_PARTY_VS_SCREEN_STATUS(party2, flags, i);
    gTasks[taskId].data[4] = flags;
}

void CB2_InitEndLinkBattle(void)
{
    s32 i;
    u8 taskId;

    SetHBlankCallback(NULL);
    SetVBlankCallback(NULL);
    gBattleTypeFlags &= ~(BATTLE_TYPE_LINK_IN_BATTLE);

    if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
    {
        SetMainCallback2(gMain.savedCallback);
        FreeBattleResources();
        FreeBattleSpritesData();
        FreeMonSpritesGfx();
    }
    else
    {
        CpuFill32(0, (void*)(VRAM), VRAM_SIZE);
        SetGpuReg(REG_OFFSET_MOSAIC, 0);
        SetGpuReg(REG_OFFSET_WIN0H, DISPLAY_WIDTH);
        SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(DISPLAY_HEIGHT / 2, DISPLAY_HEIGHT / 2 + 1));
        SetGpuReg(REG_OFFSET_WININ, 0);
        SetGpuReg(REG_OFFSET_WINOUT, 0);
        gBattle_WIN0H = DISPLAY_WIDTH;
        gBattle_WIN0V = WIN_RANGE(DISPLAY_HEIGHT / 2, DISPLAY_HEIGHT / 2 + 1);
        ScanlineEffect_Clear();

        i = 0;
        while (i < 80)
        {
            gScanlineEffectRegBuffers[0][i] = 0xF0;
            gScanlineEffectRegBuffers[1][i] = 0xF0;
            i++;
        }

        while (i < 160)
        {
            gScanlineEffectRegBuffers[0][i] = 0xFF10;
            gScanlineEffectRegBuffers[1][i] = 0xFF10;
            i++;
        }

        ResetPaletteFade();

        gBattle_BG0_X = 0;
        gBattle_BG0_Y = 0;
        gBattle_BG1_X = 0;
        gBattle_BG1_Y = 0;
        gBattle_BG2_X = 0;
        gBattle_BG2_Y = 0;
        gBattle_BG3_X = 0;
        gBattle_BG3_Y = 0;

        InitBattleBgsVideo();
        LoadCompressedPalette(gBattleTextboxPalette, 0, 64);
        LoadBattleMenuWindowGfx();
        ResetSpriteData();
        ResetTasks();
        DrawBattleEntryBackground();
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2 | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR);
        FreeAllSpritePalettes();
        gReservedSpritePaletteCount = 4;
        SetVBlankCallback(VBlankCB_Battle);

        // Show end Vs screen with battle results
        taskId = CreateTask(InitLinkBattleVsScreen, 0);
        gTasks[taskId].data[1] = 0x10E;
        gTasks[taskId].data[2] = 0x5A;
        gTasks[taskId].data[5] = 1;
        BufferPartyVsScreenHealth_AtEnd(taskId);

        SetMainCallback2(CB2_EndLinkBattle);
        gBattleCommunication[MULTIUSE_STATE] = 0;
    }
}

static void CB2_EndLinkBattle(void)
{
    EndLinkBattleInSteps();
    AnimateSprites();
    BuildOamBuffer();
    RunTextPrinters();
    UpdatePaletteFade();
    RunTasks();
}

static void EndLinkBattleInSteps(void)
{
    s32 i;

    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        gBattleCommunication[1] = 0xFF;
        gBattleCommunication[MULTIUSE_STATE]++;
        break;
    case 1:
        if (--gBattleCommunication[1] == 0)
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 2:
        if (!gPaletteFade.active)
        {
            u8 monsCount;

            gMain.anyLinkBattlerHasFrontierPass = RecordedBattle_GetFrontierPassFlag();

            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                monsCount = 4;
            else
                monsCount = 2;

            for (i = 0; i < monsCount && (gLinkPlayers[i].version & 0xFF) == VERSION_EMERALD; i++);

            if (!gSaveBlock2Ptr->frontier.disableRecordBattle && i == monsCount)
            {
                if (FlagGet(FLAG_SYS_FRONTIER_PASS))
                {
                    FreeAllWindowBuffers();
                    SetMainCallback2(sub_80392A8);
                }
                else if (!gMain.anyLinkBattlerHasFrontierPass)
                {
                    SetMainCallback2(gMain.savedCallback);
                    FreeBattleResources();
                    FreeBattleSpritesData();
                    FreeMonSpritesGfx();
                }
                else if (gReceivedRemoteLinkPlayers == 0)
                {
                    CreateTask(Task_ReconnectWithLinkPlayers, 5);
                    gBattleCommunication[MULTIUSE_STATE]++;
                }
                else
                {
                    gBattleCommunication[MULTIUSE_STATE]++;
                }
            }
            else
            {
                SetMainCallback2(gMain.savedCallback);
                FreeBattleResources();
                FreeBattleSpritesData();
                FreeMonSpritesGfx();
            }
        }
        break;
    case 3:
        CpuFill32(0, (void*)(VRAM), VRAM_SIZE);

        for (i = 0; i < 2; i++)
            LoadChosenBattleElement(i);

        BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
        gBattleCommunication[MULTIUSE_STATE]++;
        break;
    case 4:
        if (!gPaletteFade.active)
            gBattleCommunication[MULTIUSE_STATE]++;
        break;
    case 5:
        if (!FuncIsActiveTask(Task_ReconnectWithLinkPlayers))
            gBattleCommunication[MULTIUSE_STATE]++;
        break;
    case 6:
        if (IsLinkTaskFinished() == TRUE)
        {
            SetLinkStandbyCallback();
            BattlePutTextOnWindow(gText_LinkStandby3, 0);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 7:
        if (!IsTextPrinterActive(0))
        {
            if (IsLinkTaskFinished() == TRUE)
                gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 8:
        if (!gWirelessCommType)
            SetCloseLinkCallback();
        gBattleCommunication[MULTIUSE_STATE]++;
        break;
    case 9:
        if (!gMain.anyLinkBattlerHasFrontierPass || gWirelessCommType || gReceivedRemoteLinkPlayers != 1)
        {
            gMain.anyLinkBattlerHasFrontierPass = 0;
            SetMainCallback2(gMain.savedCallback);
            FreeBattleResources();
            FreeBattleSpritesData();
            FreeMonSpritesGfx();
        }
        break;
    }
}

u32 GetBattleBgTemplateData(u8 arrayId, u8 caseId)
{
    u32 ret = 0;

    switch (caseId)
    {
    case 0:
        ret = gBattleBgTemplates[arrayId].bg;
        break;
    case 1:
        ret = gBattleBgTemplates[arrayId].charBaseIndex;
        break;
    case 2:
        ret = gBattleBgTemplates[arrayId].mapBaseIndex;
        break;
    case 3:
        ret = gBattleBgTemplates[arrayId].screenSize;
        break;
    case 4:
        ret = gBattleBgTemplates[arrayId].paletteMode;
        break;
    case 5: // Only this case is used
        ret = gBattleBgTemplates[arrayId].priority;
        break;
    case 6:
        ret = gBattleBgTemplates[arrayId].baseTile;
        break;
    }

    return ret;
}

static void sub_80392A8(void)
{
    s32 i;

    SetHBlankCallback(NULL);
    SetVBlankCallback(NULL);
    CpuFill32(0, (void*)(VRAM), VRAM_SIZE);
    ResetPaletteFade();
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gBattle_BG2_X = 0;
    gBattle_BG2_Y = 0;
    gBattle_BG3_X = 0;
    gBattle_BG3_Y = 0;
    InitBattleBgsVideo();
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    LoadBattleMenuWindowGfx();

    for (i = 0; i < 2; i++)
        LoadChosenBattleElement(i);

    ResetSpriteData();
    ResetTasks();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 4;
    SetVBlankCallback(VBlankCB_Battle);
    SetMainCallback2(sub_803937C);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
    gBattleCommunication[MULTIUSE_STATE] = 0;
}

static void sub_803937C(void)
{
    sub_803939C();
    AnimateSprites();
    BuildOamBuffer();
    RunTextPrinters();
    UpdatePaletteFade();
    RunTasks();
}

static void sub_803939C(void)
{
    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        gBattleCommunication[MULTIUSE_STATE]++;
        break;
    case 1:
        if (gMain.anyLinkBattlerHasFrontierPass && gReceivedRemoteLinkPlayers == 0)
            CreateTask(Task_ReconnectWithLinkPlayers, 5);
        gBattleCommunication[MULTIUSE_STATE]++;
        break;
    case 2:
        if (!FuncIsActiveTask(Task_ReconnectWithLinkPlayers))
            gBattleCommunication[MULTIUSE_STATE]++;
        break;
    case 3:
        if (!gPaletteFade.active)
        {
            BattlePutTextOnWindow(gText_RecordBattleToPass, 0);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 4:
        if (!IsTextPrinterActive(0))
        {
            HandleBattleWindow(0x18, 8, 0x1D, 0xD, 0);
            BattlePutTextOnWindow(gText_BattleYesNoChoice, 0xC);
            gBattleCommunication[CURSOR_POSITION] = 1;
            BattleCreateYesNoCursorAt(1);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 5:
        if (JOY_NEW(DPAD_UP))
        {
            if (gBattleCommunication[CURSOR_POSITION] != 0)
            {
                PlaySE(SE_SELECT);
                BattleDestroyYesNoCursorAt(gBattleCommunication[CURSOR_POSITION]);
                gBattleCommunication[CURSOR_POSITION] = 0;
                BattleCreateYesNoCursorAt(0);
            }
        }
        else if (JOY_NEW(DPAD_DOWN))
        {
            if (gBattleCommunication[CURSOR_POSITION] == 0)
            {
                PlaySE(SE_SELECT);
                BattleDestroyYesNoCursorAt(gBattleCommunication[CURSOR_POSITION]);
                gBattleCommunication[CURSOR_POSITION] = 1;
                BattleCreateYesNoCursorAt(1);
            }
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (gBattleCommunication[CURSOR_POSITION] == 0)
            {
                HandleBattleWindow(0x18, 8, 0x1D, 0xD, WINDOW_CLEAR);
                gBattleCommunication[1] = MoveRecordedBattleToSaveData();
                gBattleCommunication[MULTIUSE_STATE] = 10;
            }
            else
            {
                gBattleCommunication[MULTIUSE_STATE]++;
            }
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 6:
        if (IsLinkTaskFinished() == TRUE)
        {
            HandleBattleWindow(0x18, 8, 0x1D, 0xD, WINDOW_CLEAR);
            if (gMain.anyLinkBattlerHasFrontierPass)
            {
                SetLinkStandbyCallback();
                BattlePutTextOnWindow(gText_LinkStandby3, 0);
            }
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 8:
        if (--gBattleCommunication[1] == 0)
        {
            if (gMain.anyLinkBattlerHasFrontierPass && !gWirelessCommType)
                SetCloseLinkCallback();
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 9:
        if (!gMain.anyLinkBattlerHasFrontierPass || gWirelessCommType || gReceivedRemoteLinkPlayers != 1)
        {
            gMain.anyLinkBattlerHasFrontierPass = 0;
            if (!gPaletteFade.active)
            {
                SetMainCallback2(gMain.savedCallback);
                FreeBattleResources();
                FreeBattleSpritesData();
                FreeMonSpritesGfx();
            }
        }
        break;
    case 10:
        if (gBattleCommunication[1] == 1)
        {
            PlaySE(SE_SAVE);
            BattleStringExpandPlaceholdersToDisplayedString(gText_BattleRecordedOnPass);
            BattlePutTextOnWindow(gDisplayedStringBattle, 0);
            gBattleCommunication[1] = 0x80;
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        else
        {
            BattleStringExpandPlaceholdersToDisplayedString(BattleFrontier_BattleTowerBattleRoom_Text_RecordCouldntBeSaved);
            BattlePutTextOnWindow(gDisplayedStringBattle, 0);
            gBattleCommunication[1] = 0x80;
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 11:
        if (IsLinkTaskFinished() == TRUE && !IsTextPrinterActive(0) && --gBattleCommunication[1] == 0)
        {
            if (gMain.anyLinkBattlerHasFrontierPass)
            {
                SetLinkStandbyCallback();
                BattlePutTextOnWindow(gText_LinkStandby3, 0);
            }
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 12:
    case 7:
        if (!IsTextPrinterActive(0))
        {
            if (gMain.anyLinkBattlerHasFrontierPass)
            {
                if (IsLinkTaskFinished() == TRUE)
                {
                    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
                    gBattleCommunication[1] = 0x20;
                    gBattleCommunication[MULTIUSE_STATE] = 8;
                }

            }
            else
            {
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
                gBattleCommunication[1] = 0x20;
                gBattleCommunication[MULTIUSE_STATE] = 8;
            }
        }
        break;
    }
}

static void TryCorrectShedinjaLanguage(struct Pokemon *mon)
{
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    u8 language = LANGUAGE_JAPANESE;

    if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_SHEDINJA
     && GetMonData(mon, MON_DATA_LANGUAGE) != language)
    {
        GetMonData(mon, MON_DATA_NICKNAME, nickname);
        if (StringCompareWithoutExtCtrlCodes(nickname, sText_ShedinjaJpnName) == 0)
            SetMonData(mon, MON_DATA_LANGUAGE, &language);
    }
}

u32 GetBattleWindowTemplatePixelWidth(u32 setId, u32 tableId)
{
    return gBattleWindowTemplates[setId][tableId].width * 8;
}

#define sBattler            data[0]
#define sSpeciesId          data[2]

void SpriteCb_WildMon(struct Sprite *sprite)
{
    sprite->callback = SpriteCb_MoveWildMonToRight;
    StartSpriteAnimIfDifferent(sprite, 0);
    if (WILD_DOUBLE_BATTLE)
        BeginNormalPaletteFade((0x10000 << sprite->sBattler) | (0x10000 << BATTLE_PARTNER(sprite->sBattler)), 0, 10, 10, RGB(8, 8, 8));
    else
        BeginNormalPaletteFade((0x10000 << sprite->sBattler), 0, 10, 10, RGB(8, 8, 8));
}

static void SpriteCb_MoveWildMonToRight(struct Sprite *sprite)
{
    if ((gIntroSlideFlags & 1) == 0)
    {
        sprite->x2 += 2;
        if (sprite->x2 == 0)
        {
            sprite->callback = SpriteCb_WildMonShowHealthbox;
        }
    }
}

static void SpriteCb_WildMonShowHealthbox(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        StartHealthboxSlideIn(sprite->sBattler);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[sprite->sBattler]);
        sprite->callback = SpriteCb_WildMonAnimate;
        StartSpriteAnimIfDifferent(sprite, 0);
        if (WILD_DOUBLE_BATTLE)
            BeginNormalPaletteFade((0x10000 << sprite->sBattler) | (0x10000 << BATTLE_PARTNER(sprite->sBattler)), 0, 10, 0, RGB(8, 8, 8));
        else
            BeginNormalPaletteFade((0x10000 << sprite->sBattler), 0, 10, 0, RGB(8, 8, 8));
    }
}

static void SpriteCb_WildMonAnimate(struct Sprite *sprite)
{
    if (!gPaletteFade.active)
    {
        BattleAnimateFrontSprite(sprite, sprite->sSpeciesId, FALSE, 1);
    }
}

void SpriteCallbackDummy_2(struct Sprite *sprite)
{

}

static void sub_80398D0(struct Sprite *sprite)
{
    sprite->data[4]--;
    if (sprite->data[4] == 0)
    {
        sprite->data[4] = 8;
        sprite->invisible ^= 1;
        sprite->data[3]--;
        if (sprite->data[3] == 0)
        {
            sprite->invisible = FALSE;
            sprite->callback = SpriteCallbackDummy_2;
            // sUnusedUnknownArray[0] = 0;
        }
    }
}

extern const struct MonCoords gMonFrontPicCoords[];
extern const struct MonCoords gCastformFrontSpriteCoords[];

void SpriteCB_FaintOpponentMon(struct Sprite *sprite)
{
    u8 battler = sprite->sBattler;
    u32 personality = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_PERSONALITY);
    u16 species;
    u8 yOffset;

    if (gBattleSpritesDataPtr->battlerData[battler].transformSpecies != 0)
        species = gBattleSpritesDataPtr->battlerData[battler].transformSpecies;
    else
        species = sprite->sSpeciesId;

    GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_PERSONALITY);  // Unused return value.

    if (species == SPECIES_UNOWN)
    {
        species = GetUnownSpeciesId(personality);
        yOffset = gMonFrontPicCoords[species].y_offset;
    }
    else if (species == SPECIES_CASTFORM)
    {
        yOffset = gCastformFrontSpriteCoords[gBattleMonForms[battler]].y_offset;
    }
    else if (species > NUM_SPECIES)
    {
        yOffset = gMonFrontPicCoords[SPECIES_NONE].y_offset;
    }
    else
    {
        yOffset = gMonFrontPicCoords[species].y_offset;
    }

    sprite->data[3] = 8 - yOffset / 8;
    sprite->data[4] = 1;
    sprite->callback = SpriteCB_AnimFaintOpponent;
}

static void SpriteCB_AnimFaintOpponent(struct Sprite *sprite)
{
    s32 i;

    if (--sprite->data[4] == 0)
    {
        sprite->data[4] = 2;
        sprite->y2 += 8; // Move the sprite down.
        if (--sprite->data[3] < 0)
        {
            FreeSpriteOamMatrix(sprite);
            DestroySprite(sprite);
        }
        else // Erase bottom part of the sprite to create a smooth illusion of mon falling down.
        {
            u8* dst = gMonSpritesGfxPtr->sprites.byte[GetBattlerPosition(sprite->sBattler)] + (gBattleMonForms[sprite->sBattler] << 11) + (sprite->data[3] << 8);

            for (i = 0; i < 0x100; i++)
                *(dst++) = 0;

            StartSpriteAnim(sprite, gBattleMonForms[sprite->sBattler]);
        }
    }
}

// Used when selecting a move, which can hit multiple targets, in double battles.
void SpriteCb_ShowAsMoveTarget(struct Sprite *sprite)
{
    sprite->data[3] = 8;
    sprite->data[4] = sprite->invisible;
    sprite->callback = SpriteCb_BlinkVisible;
}

static void SpriteCb_BlinkVisible(struct Sprite *sprite)
{
    if (--sprite->data[3] == 0)
    {
        sprite->invisible ^= 1;
        sprite->data[3] = 8;
    }
}

void SpriteCb_HideAsMoveTarget(struct Sprite *sprite)
{
    sprite->invisible = sprite->data[4];
    sprite->data[4] = FALSE;
    sprite->callback = SpriteCallbackDummy_2;
}

void SpriteCb_OpponentMonFromBall(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
    {
        if (!(gHitMarker & HITMARKER_NO_ANIMATIONS) || gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
        {
            if (HasTwoFramesAnimation(sprite->sSpeciesId))
                StartSpriteAnim(sprite, 1);
        }
        BattleAnimateFrontSprite(sprite, sprite->sSpeciesId, TRUE, 1);
    }
}

// This callback is frequently overwritten by SpriteCB_TrainerSlideIn
void SpriteCB_BattleSpriteStartSlideLeft(struct Sprite *sprite)
{
    sprite->callback = SpriteCB_BattleSpriteSlideLeft;
}

static void SpriteCB_BattleSpriteSlideLeft(struct Sprite *sprite)
{
    if (!(gIntroSlideFlags & 1))
    {
        sprite->x2 -= 2;
        if (sprite->x2 == 0)
        {
            sprite->callback = SpriteCallbackDummy_3;
            sprite->data[1] = 0;
        }
    }
}

// Unused
static void sub_80105DC(struct Sprite *sprite)
{
    sprite->callback = SpriteCallbackDummy_3;
}

static void SpriteCallbackDummy_3(struct Sprite *sprite)
{
}

#define sSpeedX data[1]
#define sSpeedY data[2]

void SpriteCB_FaintSlideAnim(struct Sprite *sprite)
{
    if (!(gIntroSlideFlags & 1))
    {
        sprite->x2 += sprite->sSpeedX;
        sprite->y2 += sprite->sSpeedY;
    }
}

#undef sSpeedX
#undef sSpeedY

#define sSinIndex           data[3]
#define sDelta              data[4]
#define sAmplitude          data[5]
#define sBouncerSpriteId    data[6]
#define sWhich              data[7]

void DoBounceEffect(u8 battler, u8 which, s8 delta, s8 amplitude)
{
    u8 invisibleSpriteId;
    u8 bouncerSpriteId;

    switch (which)
    {
    case BOUNCE_HEALTHBOX:
    default:
        if (gBattleSpritesDataPtr->healthBoxesData[battler].healthboxIsBouncing)
            return;
        break;
    case BOUNCE_MON:
        if (gBattleSpritesDataPtr->healthBoxesData[battler].battlerIsBouncing)
            return;
        break;
    }

    invisibleSpriteId = CreateInvisibleSpriteWithCallback(SpriteCB_BounceEffect);
    if (which == BOUNCE_HEALTHBOX)
    {
        bouncerSpriteId = gHealthboxSpriteIds[battler];
        gBattleSpritesDataPtr->healthBoxesData[battler].healthboxBounceSpriteId = invisibleSpriteId;
        gBattleSpritesDataPtr->healthBoxesData[battler].healthboxIsBouncing = 1;
        gSprites[invisibleSpriteId].sSinIndex = 128; // 0
    }
    else
    {
        bouncerSpriteId = gBattlerSpriteIds[battler];
        gBattleSpritesDataPtr->healthBoxesData[battler].battlerBounceSpriteId = invisibleSpriteId;
        gBattleSpritesDataPtr->healthBoxesData[battler].battlerIsBouncing = 1;
        gSprites[invisibleSpriteId].sSinIndex = 192; // -1
    }
    gSprites[invisibleSpriteId].sDelta = delta;
    gSprites[invisibleSpriteId].sAmplitude = amplitude;
    gSprites[invisibleSpriteId].sBouncerSpriteId = bouncerSpriteId;
    gSprites[invisibleSpriteId].sWhich = which;
    gSprites[invisibleSpriteId].sBattler = battler;
    gSprites[bouncerSpriteId].x2 = 0;
    gSprites[bouncerSpriteId].y2 = 0;
}

void EndBounceEffect(u8 battler, u8 which)
{
    u8 bouncerSpriteId;

    if (which == BOUNCE_HEALTHBOX)
    {
        if (!gBattleSpritesDataPtr->healthBoxesData[battler].healthboxIsBouncing)
            return;

        bouncerSpriteId = gSprites[gBattleSpritesDataPtr->healthBoxesData[battler].healthboxBounceSpriteId].sBouncerSpriteId;
        DestroySprite(&gSprites[gBattleSpritesDataPtr->healthBoxesData[battler].healthboxBounceSpriteId]);
        gBattleSpritesDataPtr->healthBoxesData[battler].healthboxIsBouncing = 0;
    }
    else
    {
        if (!gBattleSpritesDataPtr->healthBoxesData[battler].battlerIsBouncing)
            return;

        bouncerSpriteId = gSprites[gBattleSpritesDataPtr->healthBoxesData[battler].battlerBounceSpriteId].sBouncerSpriteId;
        DestroySprite(&gSprites[gBattleSpritesDataPtr->healthBoxesData[battler].battlerBounceSpriteId]);
        gBattleSpritesDataPtr->healthBoxesData[battler].battlerIsBouncing = 0;
    }

    gSprites[bouncerSpriteId].x2 = 0;
    gSprites[bouncerSpriteId].y2 = 0;
}

static void SpriteCB_BounceEffect(struct Sprite *sprite)
{
    u8 bouncerSpriteId = sprite->sBouncerSpriteId;
    s32 index = sprite->sSinIndex;
    s32 y = Sin(index, sprite->sAmplitude) + sprite->sAmplitude;

    gSprites[bouncerSpriteId].y2 = y;
    sprite->sSinIndex = (sprite->sSinIndex + sprite->sDelta) & 0xFF;

    bouncerSpriteId = GetMegaIndicatorSpriteId(sprite->sBouncerSpriteId);
    if (sprite->sWhich == BOUNCE_HEALTHBOX && bouncerSpriteId != 0xFF)
        gSprites[bouncerSpriteId].y2 = y;
}

#undef sSinIndex
#undef sDelta
#undef sAmplitude
#undef sBouncerSpriteId
#undef sWhich

void SpriteCb_PlayerMonFromBall(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
        BattleAnimateBackSprite(sprite, sprite->sSpeciesId);
}

void sub_8039E60(struct Sprite *sprite)
{
    sub_8039E9C(sprite);
    if (sprite->animEnded)
        sprite->callback = SpriteCallbackDummy_3;
}

void SpriteCB_TrainerThrowObject(struct Sprite *sprite)
{
    StartSpriteAnim(sprite, 1);
    sprite->callback = sub_8039E60;
}

void sub_8039E9C(struct Sprite *sprite)
{
    if (sprite->animDelayCounter == 0)
        sprite->centerToCornerVecX = gUnknown_0831ACE0[sprite->animCmdIndex];
}

void BeginBattleIntroDummy(void)
{

}

void BeginBattleIntro(void)
{
    BattleStartClearSetData();
    gBattleCommunication[1] = 0;
    gBattleStruct->introState = 0;
    gBattleMainFunc = DoBattleIntro;
}

static void BattleMainCB1(void)
{
    gBattleMainFunc();

    for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
        gBattlerControllerFuncs[gActiveBattler]();
}

static void BattleStartClearSetData(void)
{
    s32 i;

    TurnValuesCleanUp(FALSE);
    SpecialStatusesClear();

    memset(&gDisableStructs, 0, sizeof(gDisableStructs));
    memset(&gFieldTimers, 0, sizeof(gFieldTimers));
    memset(&gSideStatuses, 0, sizeof(gSideStatuses));
    memset(&gSideTimers, 0, sizeof(gSideTimers));
    memset(&gWishFutureKnock, 0, sizeof(gWishFutureKnock));
    memset(&gBattleResults, 0, sizeof(gBattleResults));

    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        gStatuses3[i] = 0;
        gStatuses4[i] = 0;
        gDisableStructs[i].isFirstTurn = 2;
        gLastMoves[i] = 0;
        gLastLandedMoves[i] = 0;
        gLastHitByType[i] = 0;
        gLastResultingMoves[i] = 0;
        gLastHitBy[i] = 0xFF;
        gLockedMoves[i] = 0;
        gLastPrintedMoves[i] = 0;
        gBattleResources->flags->flags[i] = 0;
        gPalaceSelectionBattleScripts[i] = 0;
        gBattleStruct->lastTakenMove[i] = 0;
        gBattleStruct->choicedMove[i] = 0;
        gBattleStruct->changedItems[i] = 0;
        gBattleStruct->lastTakenMoveFrom[i][0] = 0;
        gBattleStruct->lastTakenMoveFrom[i][1] = 0;
        gBattleStruct->lastTakenMoveFrom[i][2] = 0;
        gBattleStruct->lastTakenMoveFrom[i][3] = 0;
        gBattleStruct->AI_monToSwitchIntoId[i] = PARTY_SIZE;
    }

    gLastUsedMove = 0;
    gFieldStatuses = 0;

    gHasFetchedBall = FALSE;
    gLastUsedBall = 0;

    gBattlerAttacker = 0;
    gBattlerTarget = 0;
    gBattleWeather = 0;
    gHitMarker = 0;

    if (!(gBattleTypeFlags & BATTLE_TYPE_RECORDED))
    {
        if (!(gBattleTypeFlags & BATTLE_TYPE_LINK) && gSaveBlock2Ptr->optionsBattleSceneOff == TRUE)
            gHitMarker |= HITMARKER_NO_ANIMATIONS;
    }
    else if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK)) && GetBattleSceneInRecordedBattle())
    {
        gHitMarker |= HITMARKER_NO_ANIMATIONS;
    }

    gBattleScripting.battleStyle = gSaveBlock2Ptr->optionsBattleStyle;
	gBattleScripting.expOnCatch = (B_EXP_CATCH >= GEN_6);
	gBattleScripting.monCaught = FALSE;

    gMultiHitCounter = 0;
    gBattleOutcome = 0;
    gBattleControllerExecFlags = 0;
    gPaydayMoney = 0;
    gBattleResources->battleScriptsStack->size = 0;
    gBattleResources->battleCallbackStack->size = 0;

    for (i = 0; i < BATTLE_COMMUNICATION_ENTRIES_COUNT; i++)
        gBattleCommunication[i] = 0;

    gPauseCounterBattle = 0;
    gBattleMoveDamage = 0;
    gIntroSlideFlags = 0;
    gBattleScripting.animTurn = 0;
    gBattleScripting.animTargetsHit = 0;
    gLeveledUpInBattle = 0;
    gAbsentBattlerFlags = 0;
    gBattleStruct->runTries = 0;
    gBattleStruct->safariGoNearCounter = 0;
    gBattleStruct->safariPkblThrowCounter = 0;
    gBattleStruct->safariCatchFactor = gBaseStats[GetMonData(&gEnemyParty[0], MON_DATA_SPECIES)].catchRate * 100 / 1275;
    gBattleStruct->safariEscapeFactor = 3;
    gBattleStruct->wildVictorySong = 0;
    gBattleStruct->moneyMultiplier = 1;

    gBattleStruct->givenExpMons = 0;
    gBattleStruct->palaceFlags = 0;

    gRandomTurnNumber = Random();

    gBattleResults.shinyWildMon = IsMonShiny(&gEnemyParty[0]);

    gBattleStruct->arenaLostPlayerMons = 0;
    gBattleStruct->arenaLostOpponentMons = 0;

    gBattleStruct->mega.triggerSpriteId = 0xFF;
    
    gBattleStruct->stickyWebUser = 0xFF;
    gBattleStruct->appearedInBattle = 0;
    
    for (i = 0; i < PARTY_SIZE; i++)
    {
        gBattleStruct->usedHeldItems[i][0] = 0;
        gBattleStruct->usedHeldItems[i][1] = 0;
        gBattleStruct->itemStolen[i].originalItem = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);
    }

    gSwapDamageCategory = FALSE; // Photon Geyser, Shell Side Arm, Light That Burns the Sky
}

void SwitchInClearSetData(void)
{
    s32 i;
    struct DisableStruct disableStructCopy = gDisableStructs[gActiveBattler];

    ClearIllusionMon(gActiveBattler);
    if (gBattleMoves[gCurrentMove].effect != EFFECT_BATON_PASS)
    {
        for (i = 0; i < NUM_BATTLE_STATS; i++)
            gBattleMons[gActiveBattler].statStages[i] = DEFAULT_STAT_STAGE;
        for (i = 0; i < gBattlersCount; i++)
        {
            if ((gBattleMons[i].status2 & STATUS2_ESCAPE_PREVENTION) && gDisableStructs[i].battlerPreventingEscape == gActiveBattler)
                gBattleMons[i].status2 &= ~STATUS2_ESCAPE_PREVENTION;
            if ((gStatuses3[i] & STATUS3_ALWAYS_HITS) && gDisableStructs[i].battlerWithSureHit == gActiveBattler)
            {
                gStatuses3[i] &= ~STATUS3_ALWAYS_HITS;
                gDisableStructs[i].battlerWithSureHit = 0;
            }
        }
    }
    if (gBattleMoves[gCurrentMove].effect == EFFECT_BATON_PASS)
    {
        gBattleMons[gActiveBattler].status2 &= (STATUS2_CONFUSION | STATUS2_FOCUS_ENERGY | STATUS2_SUBSTITUTE | STATUS2_ESCAPE_PREVENTION | STATUS2_CURSED);
        gStatuses3[gActiveBattler] &= (STATUS3_LEECHSEED_BATTLER | STATUS3_LEECHSEED | STATUS3_ALWAYS_HITS | STATUS3_PERISH_SONG | STATUS3_ROOTED
                                       | STATUS3_GASTRO_ACID | STATUS3_EMBARGO | STATUS3_TELEKINESIS | STATUS3_MAGNET_RISE | STATUS3_HEAL_BLOCK
                                       | STATUS3_AQUA_RING | STATUS3_POWER_TRICK);

        for (i = 0; i < gBattlersCount; i++)
        {
            if (GetBattlerSide(gActiveBattler) != GetBattlerSide(i)
             && (gStatuses3[i] & STATUS3_ALWAYS_HITS) != 0
             && (gDisableStructs[i].battlerWithSureHit == gActiveBattler))
            {
                gStatuses3[i] &= ~(STATUS3_ALWAYS_HITS);
                gStatuses3[i] |= STATUS3_ALWAYS_HITS_TURN(2);
            }
        }
        if (gStatuses3[gActiveBattler] & STATUS3_POWER_TRICK)
            SWAP(gBattleMons[gActiveBattler].attack, gBattleMons[gActiveBattler].defense, i);
    }
    else
    {
        gBattleMons[gActiveBattler].status2 = 0;
        gStatuses3[gActiveBattler] = 0;
    }
    
    gStatuses4[gActiveBattler] = 0;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (gBattleMons[i].status2 & STATUS2_INFATUATED_WITH(gActiveBattler))
            gBattleMons[i].status2 &= ~(STATUS2_INFATUATED_WITH(gActiveBattler));
        if ((gBattleMons[i].status2 & STATUS2_WRAPPED) && *(gBattleStruct->wrappedBy + i) == gActiveBattler)
            gBattleMons[i].status2 &= ~(STATUS2_WRAPPED);
    }

    gActionSelectionCursor[gActiveBattler] = 0;
    gMoveSelectionCursor[gActiveBattler] = 0;

    memset(&gDisableStructs[gActiveBattler], 0, sizeof(struct DisableStruct));

    if (gBattleMoves[gCurrentMove].effect == EFFECT_BATON_PASS)
    {
        gDisableStructs[gActiveBattler].substituteHP = disableStructCopy.substituteHP;
        gDisableStructs[gActiveBattler].battlerWithSureHit = disableStructCopy.battlerWithSureHit;
        gDisableStructs[gActiveBattler].perishSongTimer = disableStructCopy.perishSongTimer;
        gDisableStructs[gActiveBattler].perishSongTimerStartValue = disableStructCopy.perishSongTimerStartValue;
        gDisableStructs[gActiveBattler].battlerPreventingEscape = disableStructCopy.battlerPreventingEscape;
    }

    gMoveResultFlags = 0;
    gDisableStructs[gActiveBattler].isFirstTurn = 2;
    gDisableStructs[gActiveBattler].truantSwitchInHack = disableStructCopy.truantSwitchInHack;
    gLastMoves[gActiveBattler] = 0;
    gLastLandedMoves[gActiveBattler] = 0;
    gLastHitByType[gActiveBattler] = 0;
    gLastResultingMoves[gActiveBattler] = 0;
    gLastPrintedMoves[gActiveBattler] = 0;
    gLastHitBy[gActiveBattler] = 0xFF;

    gBattleStruct->lastTakenMove[gActiveBattler] = 0;
    gBattleStruct->sameMoveTurns[gActiveBattler] = 0;
    gBattleStruct->lastTakenMoveFrom[gActiveBattler][0] = 0;
    gBattleStruct->lastTakenMoveFrom[gActiveBattler][1] = 0;
    gBattleStruct->lastTakenMoveFrom[gActiveBattler][2] = 0;
    gBattleStruct->lastTakenMoveFrom[gActiveBattler][3] = 0;
    gBattleStruct->lastMoveFailed &= ~(gBitTable[gActiveBattler]);
    gBattleStruct->palaceFlags &= ~(gBitTable[gActiveBattler]);
    
    if (gActiveBattler == gBattleStruct->stickyWebUser)
        gBattleStruct->stickyWebUser = 0xFF;    // Switched into sticky web user slot so reset it

    for (i = 0; i < gBattlersCount; i++)
    {
        if (i != gActiveBattler && GetBattlerSide(i) != GetBattlerSide(gActiveBattler))
            gBattleStruct->lastTakenMove[i] = 0;

        gBattleStruct->lastTakenMoveFrom[i][gActiveBattler] = 0;
    }

    gBattleStruct->choicedMove[gActiveBattler] = 0;
    gBattleResources->flags->flags[gActiveBattler] = 0;
    gCurrentMove = 0;
    gBattleStruct->arenaTurnCounter = 0xFF;
    
    // Reset damage to prevent things like red card activating if the switched-in mon is holding it
    gSpecialStatuses[gActiveBattler].physicalDmg = 0;
    gSpecialStatuses[gActiveBattler].specialDmg = 0;

    ClearBattlerMoveHistory(gActiveBattler);
    ClearBattlerAbilityHistory(gActiveBattler);
    ClearBattlerItemEffectHistory(gActiveBattler);
}

void FaintClearSetData(void)
{
    s32 i;

    for (i = 0; i < NUM_BATTLE_STATS; i++)
        gBattleMons[gActiveBattler].statStages[i] = DEFAULT_STAT_STAGE;

    gBattleMons[gActiveBattler].status2 = 0;
    gStatuses3[gActiveBattler] = 0;
    gStatuses4[gActiveBattler] = 0;

    for (i = 0; i < gBattlersCount; i++)
    {
        if ((gBattleMons[i].status2 & STATUS2_ESCAPE_PREVENTION) && gDisableStructs[i].battlerPreventingEscape == gActiveBattler)
            gBattleMons[i].status2 &= ~STATUS2_ESCAPE_PREVENTION;
        if (gBattleMons[i].status2 & STATUS2_INFATUATED_WITH(gActiveBattler))
            gBattleMons[i].status2 &= ~(STATUS2_INFATUATED_WITH(gActiveBattler));
        if ((gBattleMons[i].status2 & STATUS2_WRAPPED) && *(gBattleStruct->wrappedBy + i) == gActiveBattler)
            gBattleMons[i].status2 &= ~(STATUS2_WRAPPED);
    }

    gActionSelectionCursor[gActiveBattler] = 0;
    gMoveSelectionCursor[gActiveBattler] = 0;

    memset(&gDisableStructs[gActiveBattler], 0, sizeof(struct DisableStruct));

    gProtectStructs[gActiveBattler].protected = FALSE;
    gProtectStructs[gActiveBattler].spikyShielded = FALSE;
    gProtectStructs[gActiveBattler].kingsShielded = FALSE;
    gProtectStructs[gActiveBattler].banefulBunkered = FALSE;
    gProtectStructs[gActiveBattler].obstructed = FALSE;
    gProtectStructs[gActiveBattler].endured = FALSE;
    gProtectStructs[gActiveBattler].noValidMoves = FALSE;
    gProtectStructs[gActiveBattler].helpingHand = FALSE;
    gProtectStructs[gActiveBattler].bounceMove = FALSE;
    gProtectStructs[gActiveBattler].stealMove = FALSE;
    gProtectStructs[gActiveBattler].prlzImmobility = FALSE;
    gProtectStructs[gActiveBattler].confusionSelfDmg = FALSE;
    gProtectStructs[gActiveBattler].targetAffected = FALSE;
    gProtectStructs[gActiveBattler].chargingTurn = FALSE;
    gProtectStructs[gActiveBattler].fleeFlag = 0;
    gProtectStructs[gActiveBattler].usedImprisonedMove = FALSE;
    gProtectStructs[gActiveBattler].loveImmobility = FALSE;
    gProtectStructs[gActiveBattler].usedDisabledMove = FALSE;
    gProtectStructs[gActiveBattler].usedTauntedMove = FALSE;
    gProtectStructs[gActiveBattler].flag2Unknown = FALSE;
    gProtectStructs[gActiveBattler].flinchImmobility = FALSE;
    gProtectStructs[gActiveBattler].notFirstStrike = FALSE;
    gProtectStructs[gActiveBattler].usedHealBlockedMove = FALSE;
    gProtectStructs[gActiveBattler].usesBouncedMove = FALSE;
    gProtectStructs[gActiveBattler].usedGravityPreventedMove = FALSE;
    gProtectStructs[gActiveBattler].usedThroatChopPreventedMove = FALSE;
    gProtectStructs[gActiveBattler].statRaised = FALSE;
    gProtectStructs[gActiveBattler].statFell = FALSE;
    gProtectStructs[gActiveBattler].pranksterElevated = FALSE;

    gDisableStructs[gActiveBattler].isFirstTurn = 2;

    gLastMoves[gActiveBattler] = 0;
    gLastLandedMoves[gActiveBattler] = 0;
    gLastHitByType[gActiveBattler] = 0;
    gLastResultingMoves[gActiveBattler] = 0;
    gLastPrintedMoves[gActiveBattler] = 0;
    gLastHitBy[gActiveBattler] = 0xFF;

    gBattleStruct->choicedMove[gActiveBattler] = 0;
    gBattleStruct->sameMoveTurns[gActiveBattler] = 0;
    gBattleStruct->lastTakenMove[gActiveBattler] = 0;
    gBattleStruct->lastTakenMoveFrom[gActiveBattler][0] = 0;
    gBattleStruct->lastTakenMoveFrom[gActiveBattler][1] = 0;
    gBattleStruct->lastTakenMoveFrom[gActiveBattler][2] = 0;
    gBattleStruct->lastTakenMoveFrom[gActiveBattler][3] = 0;

    gBattleStruct->palaceFlags &= ~(gBitTable[gActiveBattler]);
    
    if (gActiveBattler == gBattleStruct->stickyWebUser)
        gBattleStruct->stickyWebUser = 0xFF;    // User of sticky web fainted, so reset the stored battler ID

    for (i = 0; i < gBattlersCount; i++)
    {
        if (i != gActiveBattler && GetBattlerSide(i) != GetBattlerSide(gActiveBattler))
            gBattleStruct->lastTakenMove[i] = 0;

        gBattleStruct->lastTakenMoveFrom[i][gActiveBattler] = 0;
    }

    gBattleResources->flags->flags[gActiveBattler] = 0;

    gBattleMons[gActiveBattler].type1 = gBaseStats[gBattleMons[gActiveBattler].species].type1;
    gBattleMons[gActiveBattler].type2 = gBaseStats[gBattleMons[gActiveBattler].species].type2;
    gBattleMons[gActiveBattler].type3 = TYPE_MYSTERY;

    ClearBattlerMoveHistory(gActiveBattler);
    ClearBattlerAbilityHistory(gActiveBattler);
    ClearBattlerItemEffectHistory(gActiveBattler);
    UndoFormChange(gBattlerPartyIndexes[gActiveBattler], GET_BATTLER_SIDE(gActiveBattler), FALSE);
    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
        UndoMegaEvolution(gBattlerPartyIndexes[gActiveBattler]);
}

static void DoBattleIntro(void)
{
    s32 i;
    u8 *state = &gBattleStruct->introState;

    switch (*state)
    {
    case 0: // Get Data of all battlers.
        gActiveBattler = gBattleCommunication[1];
        BtlController_EmitGetMonData(0, REQUEST_ALL_BATTLE, 0);
        MarkBattlerForControllerExec(gActiveBattler);
        (*state)++;
        break;
    case 1: // Loop through all battlers.
        if (!gBattleControllerExecFlags)
        {
            if (++gBattleCommunication[1] == gBattlersCount)
                (*state)++;
            else
                *state = 0;
        }
        break;
    case 2: // Start graphical intro slide.
        if (!gBattleControllerExecFlags)
        {
            gActiveBattler = GetBattlerAtPosition(0);
            BtlController_EmitIntroSlide(0, gBattleTerrain);
            MarkBattlerForControllerExec(gActiveBattler);
            gBattleCommunication[0] = 0;
            gBattleCommunication[1] = 0;
            (*state)++;
        }
        break;
    case 3: // Wait for intro slide.
        if (!gBattleControllerExecFlags)
            (*state)++;
        break;
    case 4: // Copy battler data gotten in cases 0 and 1. Draw trainer/mon sprite.
        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
        {
            if ((gBattleTypeFlags & BATTLE_TYPE_SAFARI) && GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
            {
                memset(&gBattleMons[gActiveBattler], 0, sizeof(struct BattlePokemon));
            }
            else
            {
                memcpy(&gBattleMons[gActiveBattler], &gBattleResources->bufferB[gActiveBattler][4], sizeof(struct BattlePokemon));
                gBattleMons[gActiveBattler].type1 = gBaseStats[gBattleMons[gActiveBattler].species].type1;
                gBattleMons[gActiveBattler].type2 = gBaseStats[gBattleMons[gActiveBattler].species].type2;
                gBattleMons[gActiveBattler].type3 = TYPE_MYSTERY;
                gBattleMons[gActiveBattler].ability = GetAbilityBySpecies(gBattleMons[gActiveBattler].species, gBattleMons[gActiveBattler].abilityNum);
                gBattleStruct->hpOnSwitchout[GetBattlerSide(gActiveBattler)] = gBattleMons[gActiveBattler].hp;
                gBattleMons[gActiveBattler].status2 = 0;
                for (i = 0; i < NUM_BATTLE_STATS; i++)
                    gBattleMons[gActiveBattler].statStages[i] = 6;
            }

            // Draw sprite.
            switch (GetBattlerPosition(gActiveBattler))
            {
            case B_POSITION_PLAYER_LEFT: // player sprite
                BtlController_EmitDrawTrainerPic(0);
                MarkBattlerForControllerExec(gActiveBattler);
                break;
            case B_POSITION_OPPONENT_LEFT:
                if (gBattleTypeFlags & BATTLE_TYPE_TRAINER) // opponent 1 sprite
                {
                    BtlController_EmitDrawTrainerPic(0);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
                else // wild mon 1
                {
                    BtlController_EmitLoadMonSprite(0);
                    MarkBattlerForControllerExec(gActiveBattler);
                    gBattleResults.lastOpponentSpecies = GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES, NULL);
                }
                break;
            case B_POSITION_PLAYER_RIGHT:
                if (gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER)) // partner sprite
                {
                    BtlController_EmitDrawTrainerPic(0);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
                break;
            case B_POSITION_OPPONENT_RIGHT:
                if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                {
                    if (gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_TWO_OPPONENTS) && !BATTLE_TWO_VS_ONE_OPPONENT) // opponent 2 if exists
                    {
                        BtlController_EmitDrawTrainerPic(0);
                        MarkBattlerForControllerExec(gActiveBattler);
                    }
                }
                else // wild mon 2
                {
                    BtlController_EmitLoadMonSprite(0);
                    MarkBattlerForControllerExec(gActiveBattler);
                    gBattleResults.lastOpponentSpecies = GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES, NULL);
                }
                break;
            }

            if (gBattleTypeFlags & BATTLE_TYPE_ARENA)
                BattleArena_InitPoints();
        }

        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
        {
            (*state)++;
        }
        else // Skip party summary since it is a wild battle.
        {
            if (B_FAST_INTRO)
                *state = 7; // Don't wait for sprite, print message at the same time.
            else
                *state = 6; // Wait for sprite to load.
        }
        break;
    case 5: // draw party summary in trainer battles
        if (!gBattleControllerExecFlags)
        {
            struct HpAndStatus hpStatus[PARTY_SIZE];

            for (i = 0; i < PARTY_SIZE; i++)
            {
                if (GetMonData(&gEnemyParty[i], MON_DATA_SPECIES2) == SPECIES_NONE
                 || GetMonData(&gEnemyParty[i], MON_DATA_SPECIES2) == SPECIES_EGG)
                {
                    hpStatus[i].hp = 0xFFFF;
                    hpStatus[i].status = 0;
                }
                else
                {
                    hpStatus[i].hp = GetMonData(&gEnemyParty[i], MON_DATA_HP);
                    hpStatus[i].status = GetMonData(&gEnemyParty[i], MON_DATA_STATUS);
                }
            }

            gActiveBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            BtlController_EmitDrawPartyStatusSummary(0, hpStatus, 0x80);
            MarkBattlerForControllerExec(gActiveBattler);

            for (i = 0; i < PARTY_SIZE; i++)
            {
                if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2) == SPECIES_NONE
                 || GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2) == SPECIES_EGG)
                {
                    hpStatus[i].hp = 0xFFFF;
                    hpStatus[i].status = 0;
                }
                else
                {
                    hpStatus[i].hp = GetMonData(&gPlayerParty[i], MON_DATA_HP);
                    hpStatus[i].status = GetMonData(&gPlayerParty[i], MON_DATA_STATUS);
                }
            }

            gActiveBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
            BtlController_EmitDrawPartyStatusSummary(0, hpStatus, 0x80);
            MarkBattlerForControllerExec(gActiveBattler);

            (*state)++;
        }
        break;
    case 6: // wait for previous action to complete
        if (!gBattleControllerExecFlags)
            (*state)++;
        break;
    case 7: // print battle intro message
        if (!IsBattlerMarkedForControllerExec(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)))
        {
            PrepareStringBattle(STRINGID_INTROMSG, GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
            (*state)++;
        }
        break;
    case 8: // wait for intro message to be printed
        if (!IsBattlerMarkedForControllerExec(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)))
        {
            if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
            {
                (*state)++;
            }
            else
            {
                if (B_FAST_INTRO)
                    *state = 15; // Wait for text to be printed.
                else
                    *state = 14; // Wait for text and sprite.
            }
        }
        break;
    case 9: // print opponent sends out
        if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK && !(gBattleTypeFlags & BATTLE_TYPE_RECORDED_IS_MASTER))
            PrepareStringBattle(STRINGID_INTROSENDOUT, GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
        else
            PrepareStringBattle(STRINGID_INTROSENDOUT, GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT));
        (*state)++;
        break;
    case 10: // wait for opponent sends out text
        if (!gBattleControllerExecFlags)
            (*state)++;
        break;
    case 11: // first opponent's mon send out animation
        if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK && !(gBattleTypeFlags & BATTLE_TYPE_RECORDED_IS_MASTER))
            gActiveBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        else
            gActiveBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);

        BtlController_EmitIntroTrainerBallThrow(0);
        MarkBattlerForControllerExec(gActiveBattler);
        (*state)++;
        break;
    case 12: // nothing
        (*state)++;
    case 13: // second opponent's mon send out
        if (gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_TWO_OPPONENTS) && !BATTLE_TWO_VS_ONE_OPPONENT)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK && !(gBattleTypeFlags & BATTLE_TYPE_RECORDED_IS_MASTER))
                gActiveBattler = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
            else
                gActiveBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);

            BtlController_EmitIntroTrainerBallThrow(0);
            MarkBattlerForControllerExec(gActiveBattler);
        }
        if (B_FAST_INTRO && !(gBattleTypeFlags & (BATTLE_TYPE_RECORDED | BATTLE_TYPE_RECORDED_LINK | BATTLE_TYPE_RECORDED_IS_MASTER | BATTLE_TYPE_LINK)))
            *state = 15; // Print at the same time as trainer sends out second mon.
        else
            (*state)++;
        break;
    case 14: // wait for opponent 2 send out
        if (!gBattleControllerExecFlags)
            (*state)++;
        break;
    case 15: // wait for wild battle message
        if (!IsBattlerMarkedForControllerExec(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)))
            (*state)++;
        break;
    case 16: // print player sends out
        if (!(gBattleTypeFlags & BATTLE_TYPE_SAFARI))
        {
            if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK && !(gBattleTypeFlags & BATTLE_TYPE_RECORDED_IS_MASTER))
                gActiveBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            else
                gActiveBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);

            // A hack that makes fast intro work in trainer battles too.
            if (B_FAST_INTRO
                && gBattleTypeFlags & BATTLE_TYPE_TRAINER
                && !(gBattleTypeFlags & (BATTLE_TYPE_RECORDED | BATTLE_TYPE_RECORDED_LINK | BATTLE_TYPE_RECORDED_IS_MASTER | BATTLE_TYPE_LINK))
                && gSprites[gHealthboxSpriteIds[gActiveBattler ^ BIT_SIDE]].callback == SpriteCallbackDummy)
            {
                return;
            }

            PrepareStringBattle(STRINGID_INTROSENDOUT, gActiveBattler);
        }
        (*state)++;
        break;
    case 17: // wait for player send out message
        if (!(gBattleTypeFlags & BATTLE_TYPE_LINK && gBattleControllerExecFlags))
        {
            if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK && !(gBattleTypeFlags & BATTLE_TYPE_RECORDED_IS_MASTER))
                gActiveBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            else
                gActiveBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);

            if (!IsBattlerMarkedForControllerExec(gActiveBattler))
                (*state)++;
        }
        break;
    case 18: // player 1 send out
        if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK && !(gBattleTypeFlags & BATTLE_TYPE_RECORDED_IS_MASTER))
            gActiveBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        else
            gActiveBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);

        BtlController_EmitIntroTrainerBallThrow(0);
        MarkBattlerForControllerExec(gActiveBattler);
        (*state)++;
        break;
    case 19: // player 2 send out
        if (gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER))
        {
            if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK && !(gBattleTypeFlags & BATTLE_TYPE_RECORDED_IS_MASTER))
                gActiveBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
            else
                gActiveBattler = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);

            BtlController_EmitIntroTrainerBallThrow(0);
            MarkBattlerForControllerExec(gActiveBattler);
        }
        (*state)++;
        break;
    case 20: // set dex and battle vars
        if (!gBattleControllerExecFlags)
        {
            for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            {
                if (GetBattlerSide(gActiveBattler) == B_SIDE_OPPONENT
                 && !(gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER
                                          | BATTLE_TYPE_FRONTIER
                                          | BATTLE_TYPE_LINK
                                          | BATTLE_TYPE_RECORDED_LINK
                                          | BATTLE_TYPE_TRAINER_HILL)))
                {
                    HandleSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[gActiveBattler].species), FLAG_SET_SEEN, gBattleMons[gActiveBattler].personality);
                }
            }

            gBattleStruct->switchInAbilitiesCounter = 0;
            gBattleStruct->switchInItemsCounter = 0;
            gBattleStruct->overworldWeatherDone = FALSE;

            gBattleMainFunc = TryDoEventsBeforeFirstTurn;
        }
        break;
    }
}

static void TryDoEventsBeforeFirstTurn(void)
{
    s32 i, j;

    if (gBattleControllerExecFlags)
        return;

    // Set invalid mons as absent(for example when starting a double battle with only one pokemon).
    if (!(gBattleTypeFlags & BATTLE_TYPE_SAFARI))
    {
        for (i = 0; i < gBattlersCount; i++)
        {
            if (gBattleMons[i].hp == 0 || gBattleMons[i].species == SPECIES_NONE)
                gAbsentBattlerFlags |= gBitTable[i];
        }
    }

    if (gBattleStruct->switchInAbilitiesCounter == 0)
    {
        for (i = 0; i < gBattlersCount; i++)
            gBattlerByTurnOrder[i] = i;
        for (i = 0; i < gBattlersCount - 1; i++)
        {
            for (j = i + 1; j < gBattlersCount; j++)
            {
                if (GetWhoStrikesFirst(gBattlerByTurnOrder[i], gBattlerByTurnOrder[j], TRUE) != 0)
                    SwapTurnOrder(i, j);
            }
        }
    }
    if (!gBattleStruct->overworldWeatherDone
        && AbilityBattleEffects(0, 0, 0, ABILITYEFFECT_SWITCH_IN_WEATHER, 0) != 0)
    {
        gBattleStruct->overworldWeatherDone = TRUE;
        return;
    }

    if (!gBattleStruct->terrainDone && AbilityBattleEffects(0, 0, 0, ABILITYEFFECT_SWITCH_IN_TERRAIN, 0) != 0)
    {
        gBattleStruct->terrainDone = TRUE;
        return;
    }

    // Totem boosts
    for (i = 0; i < gBattlersCount; i++)
    {
        if (gTotemBoosts[i].stats != 0)
        {
            gBattlerAttacker = i;
            BattleScriptExecute(BattleScript_TotemVar);
            return;
        }
    }
    memset(gTotemBoosts, 0, sizeof(gTotemBoosts));  // erase all totem boosts just to be safe

    // Primal Reversion
    for (i = 0; i < gBattlersCount; i++)
    {
        if (CanMegaEvolve(i)
         && GetBattlerHoldEffect(i, TRUE) == HOLD_EFFECT_PRIMAL_ORB)
        {
            gBattlerAttacker = i;
            BattleScriptExecute(BattleScript_PrimalReversion);
            return;
        }
    }
    
    // Check neutralizing gas
    if (AbilityBattleEffects(ABILITYEFFECT_NEUTRALIZINGGAS, 0, 0, 0, 0) != 0)
        return;
    
    // Check all switch in abilities happening from the fastest mon to slowest.
    while (gBattleStruct->switchInAbilitiesCounter < gBattlersCount)
    {
        gBattlerAttacker = gBattlerByTurnOrder[gBattleStruct->switchInAbilitiesCounter++];
        if (AbilityBattleEffects(ABILITYEFFECT_ON_SWITCHIN, gBattlerAttacker, 0, 0, 0) != 0)
            return;
    }
    if (AbilityBattleEffects(ABILITYEFFECT_INTIMIDATE1, 0, 0, 0, 0) != 0)
        return;
    if (AbilityBattleEffects(ABILITYEFFECT_TRACE1, 0, 0, 0, 0) != 0)
        return;
    // Check all switch in items having effect from the fastest mon to slowest.
    while (gBattleStruct->switchInItemsCounter < gBattlersCount)
    {
        if (ItemBattleEffects(ITEMEFFECT_ON_SWITCH_IN, gBattlerByTurnOrder[gBattleStruct->switchInItemsCounter++], FALSE))
            return;
    }

    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        *(gBattleStruct->monToSwitchIntoId + i) = PARTY_SIZE;
        gChosenActionByBattler[i] = B_ACTION_NONE;
        gChosenMoveByBattler[i] = MOVE_NONE;
        // Record party slots of player's mons that appeared in battle
        if (!IsBattlerAIControlled(i))
            gBattleStruct->appearedInBattle |= gBitTable[gBattlerPartyIndexes[i]];
    }
    TurnValuesCleanUp(FALSE);
    SpecialStatusesClear();
    *(&gBattleStruct->field_91) = gAbsentBattlerFlags;
    BattlePutTextOnWindow(gText_EmptyString3, 0);
    gBattleMainFunc = HandleTurnActionSelectionState;
    ResetSentPokesToOpponentValue();

    for (i = 0; i < BATTLE_COMMUNICATION_ENTRIES_COUNT; i++)
        gBattleCommunication[i] = 0;

    for (i = 0; i < gBattlersCount; i++)
        gBattleMons[i].status2 &= ~(STATUS2_FLINCHED);

    *(&gBattleStruct->turnEffectsTracker) = 0;
    *(&gBattleStruct->turnEffectsBattlerId) = 0;
    *(&gBattleStruct->wishPerishSongState) = 0;
    *(&gBattleStruct->wishPerishSongBattlerId) = 0;
    gBattleScripting.moveendState = 0;
    gBattleStruct->faintedActionsState = 0;
    gBattleStruct->turnCountersTracker = 0;
    gMoveResultFlags = 0;

    gRandomTurnNumber = Random();

    if (gBattleTypeFlags & BATTLE_TYPE_ARENA)
    {
        StopCryAndClearCrySongs();
        BattleScriptExecute(BattleScript_ArenaTurnBeginning);
    }
}

static void HandleEndTurn_ContinueBattle(void)
{
    s32 i;

    if (gBattleControllerExecFlags == 0)
    {
        gBattleMainFunc = BattleTurnPassed;
        for (i = 0; i < BATTLE_COMMUNICATION_ENTRIES_COUNT; i++)
            gBattleCommunication[i] = 0;
        for (i = 0; i < gBattlersCount; i++)
        {
            gBattleMons[i].status2 &= ~(STATUS2_FLINCHED);
            if ((gBattleMons[i].status1 & STATUS1_SLEEP) && (gBattleMons[i].status2 & STATUS2_MULTIPLETURNS))
                CancelMultiTurnMoves(i);
        }
        gBattleStruct->turnEffectsTracker = 0;
        gBattleStruct->turnEffectsBattlerId = 0;
        gBattleStruct->wishPerishSongState = 0;
        gBattleStruct->wishPerishSongBattlerId = 0;
        gBattleStruct->turnCountersTracker = 0;
        gMoveResultFlags = 0;
    }
}

void BattleTurnPassed(void)
{
    s32 i;

    TurnValuesCleanUp(TRUE);
    if (gBattleOutcome == 0)
    {
        if (DoFieldEndTurnEffects())
            return;
        if (DoBattlerEndTurnEffects())
            return;
    }
    if (HandleFaintedMonActions())
        return;
    gBattleStruct->faintedActionsState = 0;
    if (HandleWishPerishSongOnTurnEnd())
        return;

    TurnValuesCleanUp(FALSE);
    gHitMarker &= ~(HITMARKER_NO_ATTACKSTRING);
    gHitMarker &= ~(HITMARKER_UNABLE_TO_USE_MOVE);
    gHitMarker &= ~(HITMARKER_x400000);
    gHitMarker &= ~(HITMARKER_PASSIVE_DAMAGE);
    gBattleScripting.animTurn = 0;
    gBattleScripting.animTargetsHit = 0;
    gBattleScripting.moveendState = 0;
    gBattleMoveDamage = 0;
    gMoveResultFlags = 0;

    for (i = 0; i < 5; i++)
        gBattleCommunication[i] = 0;

    if (gBattleOutcome != 0)
    {
        gCurrentActionFuncId = B_ACTION_FINISHED;
        gBattleMainFunc = RunTurnActionsFunctions;
        return;
    }

    if (gBattleResults.battleTurnCounter < 0xFF)
    {
        gBattleResults.battleTurnCounter++;
        gBattleStruct->arenaTurnCounter++;
    }

    for (i = 0; i < gBattlersCount; i++)
    {
        gChosenActionByBattler[i] = B_ACTION_NONE;
        gChosenMoveByBattler[i] = MOVE_NONE;
    }

    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
        *(gBattleStruct->monToSwitchIntoId + i) = PARTY_SIZE;

    *(&gBattleStruct->field_91) = gAbsentBattlerFlags;
    BattlePutTextOnWindow(gText_EmptyString3, 0);
    gBattleMainFunc = HandleTurnActionSelectionState;
    gRandomTurnNumber = Random();

    if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
        BattleScriptExecute(BattleScript_PalacePrintFlavorText);
    else if (gBattleTypeFlags & BATTLE_TYPE_ARENA && gBattleStruct->arenaTurnCounter == 0)
        BattleScriptExecute(BattleScript_ArenaTurnBeginning);
    else if (ShouldDoTrainerSlide(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), gTrainerBattleOpponent_A, TRAINER_SLIDE_LAST_LOW_HP))
        BattleScriptExecute(BattleScript_TrainerSlideMsgEnd2);
}

u8 IsRunningFromBattleImpossible(void)
{
    u32 holdEffect, i;

    if (gBattleMons[gActiveBattler].item == ITEM_ENIGMA_BERRY)
        holdEffect = gEnigmaBerries[gActiveBattler].holdEffect;
    else
        holdEffect = ItemId_GetHoldEffect(gBattleMons[gActiveBattler].item);

    gPotentialItemEffectBattler = gActiveBattler;

    if (gBattleTypeFlags & BATTLE_TYPE_FIRST_BATTLE) // Cannot ever run from saving Birch's battle.
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_DONT_LEAVE_BIRCH;
        return 1;
    }
    if (GetBattlerPosition(gActiveBattler) == B_POSITION_PLAYER_RIGHT && WILD_DOUBLE_BATTLE
        && IsBattlerAlive(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT))) // The second pokemon cannot run from a double wild battle, unless it's the only alive mon.
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CANT_ESCAPE;
        return 1;
    }

    if (holdEffect == HOLD_EFFECT_CAN_ALWAYS_RUN)
        return 0;
    #if B_GHOSTS_ESCAPE >= GEN_6
        if (IS_BATTLER_OF_TYPE(gActiveBattler, TYPE_GHOST))
            return 0;
    #endif
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        return 0;
    if (GetBattlerAbility(gActiveBattler) == ABILITY_RUN_AWAY)
        return 0;

    if ((i = IsAbilityPreventingEscape(gActiveBattler)))
    {
        gBattleScripting.battler = i - 1;
        gLastUsedAbility = gBattleMons[i - 1].ability;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_PREVENTS_ESCAPE;
        return 2;
    }

    if (!CanBattlerEscape(gActiveBattler))
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CANT_ESCAPE;
        return 1;
    }
    return 0;
}

void SwitchPartyOrder(u8 battler)
{
    s32 i;
    u8 partyId1;
    u8 partyId2;

    // gBattleStruct->field_60[battler][i]

    for (i = 0; i < (int)ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
        gBattlePartyCurrentOrder[i] = *(battler * 3 + i + (u8*)(gBattleStruct->field_60));

    partyId1 = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[battler]);
    partyId2 = GetPartyIdFromBattlePartyId(*(gBattleStruct->monToSwitchIntoId + battler));
    SwitchPartyMonSlots(partyId1, partyId2);

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        for (i = 0; i < (int)ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
        {
            *(battler * 3 + i + (u8*)(gBattleStruct->field_60)) = gBattlePartyCurrentOrder[i];
            *(BATTLE_PARTNER(battler) * 3 + i + (u8*)(gBattleStruct->field_60)) = gBattlePartyCurrentOrder[i];
        }
    }
    else
    {
        for (i = 0; i < (int)ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
        {
            *(battler * 3 + i + (u8*)(gBattleStruct->field_60)) = gBattlePartyCurrentOrder[i];
        }
    }
}

enum
{
    STATE_TURN_START_RECORD,
    STATE_BEFORE_ACTION_CHOSEN,
    STATE_WAIT_ACTION_CHOSEN,
    STATE_WAIT_ACTION_CASE_CHOSEN,
    STATE_WAIT_ACTION_CONFIRMED_STANDBY,
    STATE_WAIT_ACTION_CONFIRMED,
    STATE_SELECTION_SCRIPT,
    STATE_WAIT_SET_BEFORE_ACTION,
    STATE_SELECTION_SCRIPT_MAY_RUN
};

static void HandleTurnActionSelectionState(void)
{
    s32 i;

    gBattleCommunication[ACTIONS_CONFIRMED_COUNT] = 0;
    for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
    {
        u8 position = GetBattlerPosition(gActiveBattler);
        switch (gBattleCommunication[gActiveBattler])
        {
        case STATE_TURN_START_RECORD: // Recorded battle related action on start of every turn.
            RecordedBattle_CopyBattlerMoves();
            gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
            break;
        case STATE_BEFORE_ACTION_CHOSEN: // Choose an action.
            *(gBattleStruct->monToSwitchIntoId + gActiveBattler) = PARTY_SIZE;
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI
                || (position & BIT_FLANK) == B_FLANK_LEFT
                || gBattleStruct->field_91 & gBitTable[GetBattlerAtPosition(BATTLE_PARTNER(position))]
                || gBattleCommunication[GetBattlerAtPosition(BATTLE_PARTNER(position))] == STATE_WAIT_ACTION_CONFIRMED)
            {
                if (gBattleStruct->field_91 & gBitTable[gActiveBattler])
                {
                    gChosenActionByBattler[gActiveBattler] = B_ACTION_NOTHING_FAINTED;
                    if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI))
                        gBattleCommunication[gActiveBattler] = STATE_WAIT_ACTION_CONFIRMED;
                    else
                        gBattleCommunication[gActiveBattler] = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                }
                else
                {
                    if (gBattleMons[gActiveBattler].status2 & STATUS2_MULTIPLETURNS
                        || gBattleMons[gActiveBattler].status2 & STATUS2_RECHARGE)
                    {
                        gChosenActionByBattler[gActiveBattler] = B_ACTION_USE_MOVE;
                        gBattleCommunication[gActiveBattler] = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                    }
                    else if (WILD_DOUBLE_BATTLE
                             && position == B_POSITION_PLAYER_RIGHT
                             && (gBattleStruct->throwingPokeBall || gChosenActionByBattler[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] == B_ACTION_RUN))
                    {
                        gBattleStruct->throwingPokeBall = FALSE;
                        gChosenActionByBattler[gActiveBattler] = B_ACTION_NOTHING_FAINTED; // Not fainted, but it cannot move, because of the throwing ball.
                        gBattleCommunication[gActiveBattler] = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                    }
                    else
                    {
                        BtlController_EmitChooseAction(0, gChosenActionByBattler[0], gBattleResources->bufferB[0][1] | (gBattleResources->bufferB[0][2] << 8));
                        MarkBattlerForControllerExec(gActiveBattler);
                        gBattleCommunication[gActiveBattler]++;
                    }
                }
            }
            break;
        case STATE_WAIT_ACTION_CHOSEN: // Try to perform an action.
            if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler]) | (0xF << 28) | (gBitTable[gActiveBattler] << 4) | (gBitTable[gActiveBattler] << 8) | (gBitTable[gActiveBattler] << 12))))
            {
                RecordedBattle_SetBattlerAction(gActiveBattler, gBattleResources->bufferB[gActiveBattler][1]);
                gChosenActionByBattler[gActiveBattler] = gBattleResources->bufferB[gActiveBattler][1];

                switch (gBattleResources->bufferB[gActiveBattler][1])
                {
                case B_ACTION_USE_MOVE:
                    if (AreAllMovesUnusable())
                    {
                        gBattleCommunication[gActiveBattler] = STATE_SELECTION_SCRIPT;
                        *(gBattleStruct->selectionScriptFinished + gActiveBattler) = FALSE;
                        *(gBattleStruct->stateIdAfterSelScript + gActiveBattler) = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                        *(gBattleStruct->moveTarget + gActiveBattler) = gBattleResources->bufferB[gActiveBattler][3];
                        return;
                    }
                    else if (gDisableStructs[gActiveBattler].encoredMove != 0)
                    {
                        gChosenMoveByBattler[gActiveBattler] = gDisableStructs[gActiveBattler].encoredMove;
                        *(gBattleStruct->chosenMovePositions + gActiveBattler) = gDisableStructs[gActiveBattler].encoredMovePos;
                        gBattleCommunication[gActiveBattler] = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                        return;
                    }
                    else
                    {
                        struct ChooseMoveStruct moveInfo;

                        moveInfo.mega = gBattleStruct->mega;
                        moveInfo.species = gBattleMons[gActiveBattler].species;
                        moveInfo.monType1 = gBattleMons[gActiveBattler].type1;
                        moveInfo.monType2 = gBattleMons[gActiveBattler].type2;
                        moveInfo.monType3 = gBattleMons[gActiveBattler].type3;

                        for (i = 0; i < MAX_MON_MOVES; i++)
                        {
                            moveInfo.moves[i] = gBattleMons[gActiveBattler].moves[i];
                            moveInfo.currentPp[i] = gBattleMons[gActiveBattler].pp[i];
                            moveInfo.maxPp[i] = CalculatePPWithBonus(
                                                            gBattleMons[gActiveBattler].moves[i],
                                                            gBattleMons[gActiveBattler].ppBonuses,
                                                            i);
                        }

                        BtlController_EmitChooseMove(0, (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) != 0, FALSE, &moveInfo);
                        MarkBattlerForControllerExec(gActiveBattler);
                    }
                    break;
                case B_ACTION_USE_ITEM:
                    if (gBattleTypeFlags & (BATTLE_TYPE_LINK
                                            | BATTLE_TYPE_FRONTIER_NO_PYRAMID
                                            | BATTLE_TYPE_EREADER_TRAINER
                                            | BATTLE_TYPE_RECORDED_LINK)
                        || (gSaveBlock2Ptr->gameDifficulty == DIFFICULTY_CHALLENGE && (gBattleTypeFlags & BATTLE_TYPE_TRAINER)))
                    {
                        RecordedBattle_ClearBattlerAction(gActiveBattler, 1);
                        gSelectionBattleScripts[gActiveBattler] = BattleScript_ActionSelectionItemsCantBeUsed;
                        gBattleCommunication[gActiveBattler] = STATE_SELECTION_SCRIPT;
                        *(gBattleStruct->selectionScriptFinished + gActiveBattler) = FALSE;
                        *(gBattleStruct->stateIdAfterSelScript + gActiveBattler) = STATE_BEFORE_ACTION_CHOSEN;
                        return;
                    }
                    else
                    {
                        BtlController_EmitChooseItem(0, gBattleStruct->field_60[gActiveBattler]);
                        MarkBattlerForControllerExec(gActiveBattler);
                    }
                    break;
                case B_ACTION_SWITCH:
                    *(gBattleStruct->field_58 + gActiveBattler) = gBattlerPartyIndexes[gActiveBattler];
                    if (gBattleTypeFlags & BATTLE_TYPE_ARENA
                        || !CanBattlerEscape(gActiveBattler))
                    {
                        BtlController_EmitChoosePokemon(0, PARTY_ACTION_CANT_SWITCH, PARTY_SIZE, ABILITY_NONE, gBattleStruct->field_60[gActiveBattler]);
                    }
                    else if ((i = IsAbilityPreventingEscape(gActiveBattler)))
                    {
                        BtlController_EmitChoosePokemon(0, ((i - 1) << 4) | PARTY_ACTION_ABILITY_PREVENTS, PARTY_SIZE, gBattleMons[i - 1].ability, gBattleStruct->field_60[gActiveBattler]);
                    }
                    else
                    {
                        if (gActiveBattler == 2 && gChosenActionByBattler[0] == B_ACTION_SWITCH)
                            BtlController_EmitChoosePokemon(0, PARTY_ACTION_CHOOSE_MON, *(gBattleStruct->monToSwitchIntoId + 0), ABILITY_NONE, gBattleStruct->field_60[gActiveBattler]);
                        else if (gActiveBattler == 3 && gChosenActionByBattler[1] == B_ACTION_SWITCH)
                            BtlController_EmitChoosePokemon(0, PARTY_ACTION_CHOOSE_MON, *(gBattleStruct->monToSwitchIntoId + 1), ABILITY_NONE, gBattleStruct->field_60[gActiveBattler]);
                        else
                            BtlController_EmitChoosePokemon(0, PARTY_ACTION_CHOOSE_MON, PARTY_SIZE, ABILITY_NONE, gBattleStruct->field_60[gActiveBattler]);
                    }
                    MarkBattlerForControllerExec(gActiveBattler);
                    break;
                case B_ACTION_SAFARI_BALL:
                    if (IsPlayerPartyAndPokemonStorageFull())
                    {
                        gSelectionBattleScripts[gActiveBattler] = BattleScript_PrintFullBox;
                        gBattleCommunication[gActiveBattler] = STATE_SELECTION_SCRIPT;
                        *(gBattleStruct->selectionScriptFinished + gActiveBattler) = FALSE;
                        *(gBattleStruct->stateIdAfterSelScript + gActiveBattler) = STATE_BEFORE_ACTION_CHOSEN;
                        return;
                    }
                    break;
                case B_ACTION_SAFARI_POKEBLOCK:
                    BtlController_EmitChooseItem(0, gBattleStruct->field_60[gActiveBattler]);
                    MarkBattlerForControllerExec(gActiveBattler);
                    break;
                case B_ACTION_CANCEL_PARTNER:
                    gBattleCommunication[gActiveBattler] = STATE_WAIT_SET_BEFORE_ACTION;
                    gBattleCommunication[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))] = STATE_BEFORE_ACTION_CHOSEN;
                    RecordedBattle_ClearBattlerAction(gActiveBattler, 1);
                    if (gBattleMons[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))].status2 & STATUS2_MULTIPLETURNS
                        || gBattleMons[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))].status2 & STATUS2_RECHARGE)
                    {
                        BtlController_EmitEndBounceEffect(0);
                        MarkBattlerForControllerExec(gActiveBattler);
                        return;
                    }
                    else if (gChosenActionByBattler[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))] == B_ACTION_SWITCH)
                    {
                        RecordedBattle_ClearBattlerAction(GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler))), 2);
                    }
                    else if (gChosenActionByBattler[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))] == B_ACTION_RUN)
                    {
                        RecordedBattle_ClearBattlerAction(GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler))), 1);
                    }
                    else if (gChosenActionByBattler[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))] == B_ACTION_USE_MOVE
                             && (gProtectStructs[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))].noValidMoves
                                || gDisableStructs[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))].encoredMove))
                    {
                        RecordedBattle_ClearBattlerAction(GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler))), 1);
                    }
                    else if (gBattleTypeFlags & BATTLE_TYPE_PALACE
                             && gChosenActionByBattler[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))] == B_ACTION_USE_MOVE)
                    {
                        gRngValue = gBattlePalaceMoveSelectionRngValue;
                        RecordedBattle_ClearBattlerAction(GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler))), 1);
                    }
                    else
                    {
                        RecordedBattle_ClearBattlerAction(GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler))), 3);
                    }

                    gBattleStruct->mega.toEvolve &= ~(gBitTable[BATTLE_PARTNER(GetBattlerPosition(gActiveBattler))]);
                    BtlController_EmitEndBounceEffect(0);
                    MarkBattlerForControllerExec(gActiveBattler);
                    return;
                case B_ACTION_DEBUG:
                    BtlController_EmitDebugMenu(0);
                    MarkBattlerForControllerExec(gActiveBattler);
                    break;
                }

                if (gBattleTypeFlags & BATTLE_TYPE_TRAINER
                    && gBattleTypeFlags & (BATTLE_TYPE_FRONTIER | BATTLE_TYPE_TRAINER_HILL)
                    && gBattleResources->bufferB[gActiveBattler][1] == B_ACTION_RUN)
                {
                    gSelectionBattleScripts[gActiveBattler] = BattleScript_AskIfWantsToForfeitMatch;
                    gBattleCommunication[gActiveBattler] = STATE_SELECTION_SCRIPT_MAY_RUN;
                    *(gBattleStruct->selectionScriptFinished + gActiveBattler) = FALSE;
                    *(gBattleStruct->stateIdAfterSelScript + gActiveBattler) = STATE_BEFORE_ACTION_CHOSEN;
                    return;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER
                         && !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
                         && gBattleResources->bufferB[gActiveBattler][1] == B_ACTION_RUN)
                {
                    BattleScriptExecute(BattleScript_PrintCantRunFromTrainer);
                    gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                }
                else if (IsRunningFromBattleImpossible()
                         && gBattleResources->bufferB[gActiveBattler][1] == B_ACTION_RUN)
                {
                    gSelectionBattleScripts[gActiveBattler] = BattleScript_PrintCantEscapeFromBattle;
                    gBattleCommunication[gActiveBattler] = STATE_SELECTION_SCRIPT;
                    *(gBattleStruct->selectionScriptFinished + gActiveBattler) = FALSE;
                    *(gBattleStruct->stateIdAfterSelScript + gActiveBattler) = STATE_BEFORE_ACTION_CHOSEN;
                    return;
                }
                else
                {
                    gBattleCommunication[gActiveBattler]++;
                }
            }
            break;
        case STATE_WAIT_ACTION_CASE_CHOSEN:
            if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler]) | (0xF << 28) | (gBitTable[gActiveBattler] << 4) | (gBitTable[gActiveBattler] << 8) | (gBitTable[gActiveBattler] << 12))))
            {
                switch (gChosenActionByBattler[gActiveBattler])
                {
                case B_ACTION_USE_MOVE:
                    switch (gBattleResources->bufferB[gActiveBattler][1])
                    {
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                        gChosenActionByBattler[gActiveBattler] = gBattleResources->bufferB[gActiveBattler][1];
                        return;
                    case 15:
                        gChosenActionByBattler[gActiveBattler] = B_ACTION_SWITCH;
                        sub_803CDF8();
                        return;
                    default:
                        sub_818603C(2);
                        if ((gBattleResources->bufferB[gActiveBattler][2] | (gBattleResources->bufferB[gActiveBattler][3] << 8)) == 0xFFFF)
                        {
                            gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                            RecordedBattle_ClearBattlerAction(gActiveBattler, 1);
                        }
                        else if (TrySetCantSelectMoveBattleScript())
                        {
                            RecordedBattle_ClearBattlerAction(gActiveBattler, 1);
                            gBattleCommunication[gActiveBattler] = STATE_SELECTION_SCRIPT;
                            *(gBattleStruct->selectionScriptFinished + gActiveBattler) = FALSE;
                            gBattleResources->bufferB[gActiveBattler][1] = B_ACTION_USE_MOVE;
                            *(gBattleStruct->stateIdAfterSelScript + gActiveBattler) = STATE_WAIT_ACTION_CHOSEN;
                            return;
                        }
                        else
                        {
                            if (!(gBattleTypeFlags & BATTLE_TYPE_PALACE))
                            {
                                RecordedBattle_SetBattlerAction(gActiveBattler, gBattleResources->bufferB[gActiveBattler][2]);
                                RecordedBattle_SetBattlerAction(gActiveBattler, gBattleResources->bufferB[gActiveBattler][3]);
                            }
                            *(gBattleStruct->chosenMovePositions + gActiveBattler) = gBattleResources->bufferB[gActiveBattler][2] & ~(RET_MEGA_EVOLUTION);
                            gChosenMoveByBattler[gActiveBattler] = gBattleMons[gActiveBattler].moves[*(gBattleStruct->chosenMovePositions + gActiveBattler)];
                            *(gBattleStruct->moveTarget + gActiveBattler) = gBattleResources->bufferB[gActiveBattler][3];
                            if (gBattleResources->bufferB[gActiveBattler][2] & RET_MEGA_EVOLUTION)
                                gBattleStruct->mega.toEvolve |= gBitTable[gActiveBattler];
                            gBattleCommunication[gActiveBattler]++;
                        }
                        break;
                    }
                    break;
                case B_ACTION_USE_ITEM:
                    if ((gBattleResources->bufferB[gActiveBattler][1] | (gBattleResources->bufferB[gActiveBattler][2] << 8)) == 0)
                    {
                        gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                    }
                    else
                    {
                        gLastUsedItem = (gBattleResources->bufferB[gActiveBattler][1] | (gBattleResources->bufferB[gActiveBattler][2] << 8));
                        if (ItemId_GetPocket(gLastUsedItem) == POCKET_POKE_BALLS)
                            gBattleStruct->throwingPokeBall = TRUE;
                        gBattleCommunication[gActiveBattler]++;
                    }
                    break;
                case B_ACTION_SWITCH:
                    if (gBattleResources->bufferB[gActiveBattler][1] == PARTY_SIZE)
                    {
                        gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                        RecordedBattle_ClearBattlerAction(gActiveBattler, 1);
                    }
                    else
                    {
                        sub_803CDF8();
                        gBattleCommunication[gActiveBattler]++;
                    }
                    break;
                case B_ACTION_RUN:
                    gHitMarker |= HITMARKER_RUN;
                    gBattleCommunication[gActiveBattler]++;
                    break;
                case B_ACTION_SAFARI_WATCH_CAREFULLY:
                    gBattleCommunication[gActiveBattler]++;
                    break;
                case B_ACTION_SAFARI_BALL:
                    gBattleCommunication[gActiveBattler]++;
                    break;
                case B_ACTION_THROW_BALL:
                    gBattleStruct->throwingPokeBall = TRUE;
                    gBattleCommunication[gActiveBattler]++;
                    break;
                case B_ACTION_SAFARI_POKEBLOCK:
                    if ((gBattleResources->bufferB[gActiveBattler][1] | (gBattleResources->bufferB[gActiveBattler][2] << 8)) != 0)
                    {
                        gBattleCommunication[gActiveBattler]++;
                    }
                    else
                    {
                        gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                    }
                    break;
                case B_ACTION_SAFARI_GO_NEAR:
                    gBattleCommunication[gActiveBattler]++;
                    break;
                case B_ACTION_SAFARI_RUN:
                    gHitMarker |= HITMARKER_RUN;
                    gBattleCommunication[gActiveBattler]++;
                    break;
                case B_ACTION_WALLY_THROW:
                    gBattleCommunication[gActiveBattler]++;
                    break;
                case B_ACTION_DEBUG:
                    gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                    break;
                }
            }
            break;
        case STATE_WAIT_ACTION_CONFIRMED_STANDBY:
            if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler])
                                                | (0xF << 28)
                                                | (gBitTable[gActiveBattler] << 4)
                                                | (gBitTable[gActiveBattler] << 8)
                                                | (gBitTable[gActiveBattler] << 12))))
            {
                if (AllAtActionConfirmed())
                    i = TRUE;
                else
                    i = FALSE;

                if (((gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE)) != BATTLE_TYPE_DOUBLE)
                    || (position & BIT_FLANK) != B_FLANK_LEFT
                    || (*(&gBattleStruct->field_91) & gBitTable[GetBattlerAtPosition(position ^ BIT_FLANK)]))
                {
                    BtlController_EmitLinkStandbyMsg(0, 0, i);
                }
                else
                {
                    BtlController_EmitLinkStandbyMsg(0, 1, i);
                }
                MarkBattlerForControllerExec(gActiveBattler);
                gBattleCommunication[gActiveBattler]++;
            }
            break;
        case STATE_WAIT_ACTION_CONFIRMED:
            if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler]) | (0xF << 28) | (gBitTable[gActiveBattler] << 4) | (gBitTable[gActiveBattler] << 8) | (gBitTable[gActiveBattler] << 12))))
            {
                gBattleCommunication[ACTIONS_CONFIRMED_COUNT]++;
            }
            break;
        case STATE_SELECTION_SCRIPT:
            if (*(gBattleStruct->selectionScriptFinished + gActiveBattler))
            {
                gBattleCommunication[gActiveBattler] = *(gBattleStruct->stateIdAfterSelScript + gActiveBattler);
            }
            else
            {
                gBattlerAttacker = gActiveBattler;
                gBattlescriptCurrInstr = gSelectionBattleScripts[gActiveBattler];
                if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler]) | (0xF << 28) | (gBitTable[gActiveBattler] << 4) | (gBitTable[gActiveBattler] << 8) | (gBitTable[gActiveBattler] << 12))))
                {
                    gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
                }
                gSelectionBattleScripts[gActiveBattler] = gBattlescriptCurrInstr;
            }
            break;
        case STATE_WAIT_SET_BEFORE_ACTION:
            if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler]) | (0xF << 28) | (gBitTable[gActiveBattler] << 4) | (gBitTable[gActiveBattler] << 8) | (gBitTable[gActiveBattler] << 12))))
            {
                gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
            }
            break;
        case STATE_SELECTION_SCRIPT_MAY_RUN:
            if (*(gBattleStruct->selectionScriptFinished + gActiveBattler))
            {
                if (gBattleResources->bufferB[gActiveBattler][1] == B_ACTION_NOTHING_FAINTED)
                {
                    gHitMarker |= HITMARKER_RUN;
                    gChosenActionByBattler[gActiveBattler] = B_ACTION_RUN;
                    gBattleCommunication[gActiveBattler] = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                }
                else
                {
                    RecordedBattle_ClearBattlerAction(gActiveBattler, 1);
                    gBattleCommunication[gActiveBattler] = *(gBattleStruct->stateIdAfterSelScript + gActiveBattler);
                }
            }
            else
            {
                gBattlerAttacker = gActiveBattler;
                gBattlescriptCurrInstr = gSelectionBattleScripts[gActiveBattler];
                if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler]) | (0xF << 28) | (gBitTable[gActiveBattler] << 4) | (gBitTable[gActiveBattler] << 8) | (gBitTable[gActiveBattler] << 12))))
                {
                    gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
                }
                gSelectionBattleScripts[gActiveBattler] = gBattlescriptCurrInstr;
            }
            break;
        }
    }

    // Check if everyone chose actions.
    if (gBattleCommunication[ACTIONS_CONFIRMED_COUNT] == gBattlersCount)
    {
        sub_818603C(1);
        
        if (WILD_DOUBLE_BATTLE && gBattleStruct->throwingPokeBall) {
            // if we choose to throw a ball with our second mon, skip the action of the first
            // (if we have chosen throw ball with first, second's is already skipped)
            gChosenActionByBattler[B_POSITION_PLAYER_LEFT] = B_ACTION_NOTHING_FAINTED;
        }
        
        gBattleMainFunc = SetActionsAndBattlersTurnOrder;

        if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
        {
            for (i = 0; i < gBattlersCount; i++)
            {
                if (gChosenActionByBattler[i] == B_ACTION_SWITCH)
                    SwitchPartyOrderInGameMulti(i, *(gBattleStruct->monToSwitchIntoId + i));
            }
        }
    }
}

static bool8 AllAtActionConfirmed(void)
{
    s32 i, count;

    for (count = 0, i = 0; i < gBattlersCount; i++)
    {
        if (gBattleCommunication[i] == STATE_WAIT_ACTION_CONFIRMED)
            count++;
    }

    if (count + 1 == gBattlersCount)
        return TRUE;
    else
        return FALSE;
}

static void sub_803CDF8(void)
{
    *(gBattleStruct->monToSwitchIntoId + gActiveBattler) = gBattleResources->bufferB[gActiveBattler][1];
    RecordedBattle_SetBattlerAction(gActiveBattler, gBattleResources->bufferB[gActiveBattler][1]);

    if (gBattleTypeFlags & BATTLE_TYPE_LINK && gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        *(gActiveBattler * 3 + (u8*)(gBattleStruct->field_60) + 0) &= 0xF;
        *(gActiveBattler * 3 + (u8*)(gBattleStruct->field_60) + 0) |= (gBattleResources->bufferB[gActiveBattler][2] & 0xF0);
        *(gActiveBattler * 3 + (u8*)(gBattleStruct->field_60) + 1) = gBattleResources->bufferB[gActiveBattler][3];

        *((gActiveBattler ^ BIT_FLANK) * 3 + (u8*)(gBattleStruct->field_60) + 0) &= (0xF0);
        *((gActiveBattler ^ BIT_FLANK) * 3 + (u8*)(gBattleStruct->field_60) + 0) |= (gBattleResources->bufferB[gActiveBattler][2] & 0xF0) >> 4;
        *((gActiveBattler ^ BIT_FLANK) * 3 + (u8*)(gBattleStruct->field_60) + 2) = gBattleResources->bufferB[gActiveBattler][3];
    }
}

void SwapTurnOrder(u8 id1, u8 id2)
{
    u32 temp;

    SWAP(gActionsByTurnOrder[id1], gActionsByTurnOrder[id2], temp);
    SWAP(gBattlerByTurnOrder[id1], gBattlerByTurnOrder[id2], temp);
}

u32 GetBattlerTotalSpeedStat(u8 battlerId)
{
    u32 speed = gBattleMons[battlerId].speed;
    u32 ability = GetBattlerAbility(battlerId);
    u32 holdEffect = GetBattlerHoldEffect(battlerId, TRUE);

    // weather abilities
    if (WEATHER_HAS_EFFECT)
    {
        if (ability == ABILITY_SWIFT_SWIM       && holdEffect != HOLD_EFFECT_UTILITY_UMBRELLA && gBattleWeather & WEATHER_RAIN_ANY)
            speed *= 2;
        else if (ability == ABILITY_CHLOROPHYLL && holdEffect != HOLD_EFFECT_UTILITY_UMBRELLA && gBattleWeather & WEATHER_SUN_ANY)
            speed *= 2;
        else if (ability == ABILITY_SAND_RUSH   && gBattleWeather & WEATHER_SANDSTORM_ANY)
            speed *= 2;
        else if (ability == ABILITY_SLUSH_RUSH  && gBattleWeather & WEATHER_HAIL_ANY)
            speed *= 2;
    }

    // other abilities
    if (ability == ABILITY_QUICK_FEET && gBattleMons[battlerId].status1 & STATUS1_ANY)
        speed = (speed * 150) / 100;
    else if (ability == ABILITY_SURGE_SURFER && gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN)
        speed *= 2;
    else if (ability == ABILITY_SLOW_START && gDisableStructs[battlerId].slowStartTimer != 0)
        speed /= 2;

    // stat stages
    speed *= gStatStageRatios[gBattleMons[battlerId].statStages[STAT_SPEED]][0];
    speed /= gStatStageRatios[gBattleMons[battlerId].statStages[STAT_SPEED]][1];

    // item effects
    if (holdEffect == HOLD_EFFECT_MACHO_BRACE || holdEffect == HOLD_EFFECT_POWER_ITEM)
        speed /= 2;
    else if (holdEffect == HOLD_EFFECT_IRON_BALL)
        speed /= 2;
    else if (holdEffect == HOLD_EFFECT_CHOICE_SCARF)
        speed = (speed * 150) / 100;
    else if (holdEffect == HOLD_EFFECT_QUICK_POWDER && gBattleMons[battlerId].species == SPECIES_DITTO && !(gBattleMons[battlerId].status2 & STATUS2_TRANSFORMED))
        speed *= 2;

    // various effects
    if (gSideStatuses[GET_BATTLER_SIDE(battlerId)] & SIDE_STATUS_TAILWIND)
        speed *= 2;
    if (gBattleResources->flags->flags[battlerId] & RESOURCE_FLAG_UNBURDEN)
        speed *= 2;

    // paralysis drop
    if (gBattleMons[battlerId].status1 & STATUS1_PARALYSIS && ability != ABILITY_QUICK_FEET)
        speed /= (B_PARALYSIS_SPEED >= GEN_7 ? 2 : 4);

    return speed;
}

s8 GetChosenMovePriority(u32 battlerId)
{
    u16 move;

    gProtectStructs[battlerId].pranksterElevated = 0;
    if (gProtectStructs[battlerId].noValidMoves)
        move = MOVE_STRUGGLE;
    else
        move = gBattleMons[battlerId].moves[*(gBattleStruct->chosenMovePositions + battlerId)];

    return GetMovePriority(battlerId, move);
}

s8 GetMovePriority(u32 battlerId, u16 move)
{
    s8 priority;

    priority = gBattleMoves[move].priority;
    if (GetBattlerAbility(battlerId) == ABILITY_GALE_WINGS
        && gBattleMoves[move].type == TYPE_FLYING
        && (B_GALE_WINGS <= GEN_6 || BATTLER_MAX_HP(battlerId)))
    {
        priority++;
    }
    else if (GetBattlerAbility(battlerId) == ABILITY_PRANKSTER && IS_MOVE_STATUS(move))
    {
        gProtectStructs[battlerId].pranksterElevated = 1;
        priority++;
    }
    else if (gBattleMoves[move].effect == EFFECT_GRASSY_GLIDE && gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN && IsBattlerGrounded(battlerId))
    {
        priority++;
    }
    else if (GetBattlerAbility(battlerId) == ABILITY_TRIAGE)
    {
        switch (gBattleMoves[move].effect)
        {
        case EFFECT_RESTORE_HP:
        case EFFECT_REST:
        case EFFECT_MORNING_SUN:
        case EFFECT_MOONLIGHT:
        case EFFECT_SYNTHESIS:
        case EFFECT_HEAL_PULSE:
        case EFFECT_HEALING_WISH:
        case EFFECT_SWALLOW:
        case EFFECT_WISH:
        case EFFECT_SOFTBOILED:
        case EFFECT_ABSORB:
        case EFFECT_ROOST:
            priority += 3;
            break;
        }
    }
    else if (GetBattlerAbility(battlerId) == ABILITY_BLITZ_BOXER && (gBattleMoves[move].flags & FLAG_IRON_FIST_BOOST))
    {
        priority++;
    }

    return priority;
}

u8 GetWhoStrikesFirst(u8 battler1, u8 battler2, bool8 ignoreChosenMoves)
{
    u8 strikesFirst = 0;
    u32 speedBattler1 = 0, speedBattler2 = 0;
    u32 holdEffectBattler1 = 0, holdEffectBattler2 = 0;
    s8 priority1 = 0, priority2 = 0;

    // Battler 1
    speedBattler1 = GetBattlerTotalSpeedStat(battler1);
    holdEffectBattler1 = GetBattlerHoldEffect(battler1, TRUE);
    // Quick Draw
    if (!ignoreChosenMoves && GetBattlerAbility(battler1) == ABILITY_QUICK_DRAW && !IS_MOVE_STATUS(gChosenMoveByBattler[battler1]) && Random() % 100 < 30)
        gProtectStructs[battler1].quickDraw = TRUE;
    // Quick Claw and Custap Berry
    if (!gProtectStructs[battler1].quickDraw
     && ((holdEffectBattler1 == HOLD_EFFECT_QUICK_CLAW && gRandomTurnNumber < (0xFFFF * GetBattlerHoldEffectParam(battler1)) / 100)
     || (!IsAbilityOnOpposingSide(battler1, ABILITY_UNNERVE)
      && holdEffectBattler1 == HOLD_EFFECT_CUSTAP_BERRY
      && HasEnoughHpToEatBerry(battler1, 4, gBattleMons[battler1].item))))
        gProtectStructs[battler1].usedCustapBerry = TRUE;

    // Battler 2
    speedBattler2 = GetBattlerTotalSpeedStat(battler2);
    holdEffectBattler2 = GetBattlerHoldEffect(battler2, TRUE);
    // Quick Draw
    if (!ignoreChosenMoves && GetBattlerAbility(battler2) == ABILITY_QUICK_DRAW && !IS_MOVE_STATUS(gChosenMoveByBattler[battler2]) && Random() % 100 < 30)
        gProtectStructs[battler2].quickDraw = TRUE;
    // Quick Claw and Custap Berry
    if (!gProtectStructs[battler2].quickDraw
     && ((holdEffectBattler2 == HOLD_EFFECT_QUICK_CLAW && gRandomTurnNumber < (0xFFFF * GetBattlerHoldEffectParam(battler2)) / 100)
     || (!IsAbilityOnOpposingSide(battler2, ABILITY_UNNERVE)
      && holdEffectBattler2 == HOLD_EFFECT_CUSTAP_BERRY
      && HasEnoughHpToEatBerry(battler2, 4, gBattleMons[battler2].item))))
        gProtectStructs[battler2].usedCustapBerry = TRUE;

    if (!ignoreChosenMoves)
    {
        if (gChosenActionByBattler[battler1] == B_ACTION_USE_MOVE)
            priority1 = GetChosenMovePriority(battler1);
        if (gChosenActionByBattler[battler2] == B_ACTION_USE_MOVE)
            priority2 = GetChosenMovePriority(battler2);
    }

    if (priority1 == priority2)
    {
        // QUICK CLAW / CUSTAP - always first
        // LAGGING TAIL - always last
        // STALL - always last
        
        if (gProtectStructs[battler1].quickDraw && !gProtectStructs[battler2].quickDraw)
            strikesFirst = 0;
        else if (!gProtectStructs[battler1].quickDraw && gProtectStructs[battler2].quickDraw)
            strikesFirst = 1;
        else if (gProtectStructs[battler1].usedCustapBerry && !gProtectStructs[battler2].usedCustapBerry)
            strikesFirst = 0;
        else if (gProtectStructs[battler2].usedCustapBerry && !gProtectStructs[battler1].usedCustapBerry)
            strikesFirst = 1;
        else if (holdEffectBattler1 == HOLD_EFFECT_LAGGING_TAIL && holdEffectBattler2 != HOLD_EFFECT_LAGGING_TAIL)
            strikesFirst = 1;
        else if (holdEffectBattler2 == HOLD_EFFECT_LAGGING_TAIL && holdEffectBattler1 != HOLD_EFFECT_LAGGING_TAIL)
            strikesFirst = 0;
        else if (GetBattlerAbility(battler1) == ABILITY_STALL && GetBattlerAbility(battler2) != ABILITY_STALL)
            strikesFirst = 1;
        else if (GetBattlerAbility(battler2) == ABILITY_STALL && GetBattlerAbility(battler1) != ABILITY_STALL)
            strikesFirst = 0;
        else
        {
            if (speedBattler1 == speedBattler2 && Random() & 1)
            {
                strikesFirst = 2; // same speeds, same priorities
            }
            else if (speedBattler1 < speedBattler2)
            {
                // battler2 has more speed
                if (gFieldStatuses & STATUS_FIELD_TRICK_ROOM)
                    strikesFirst = 0;
                else
                    strikesFirst = 1;
            }
            else
            {
                // battler1 has more speed
                if (gFieldStatuses & STATUS_FIELD_TRICK_ROOM)
                    strikesFirst = 1;
                else
                    strikesFirst = 0;
            }
        }
    }
    else if (priority1 < priority2)
    {
        strikesFirst = 1; // battler2's move has greater priority
    }
    else
    {
        strikesFirst = 0; // battler1's move has greater priority
    }

    return strikesFirst;
}

static void SetActionsAndBattlersTurnOrder(void)
{
    s32 turnOrderId = 0;
    s32 i, j;

    if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
    {
        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
        {
            gActionsByTurnOrder[turnOrderId] = gChosenActionByBattler[gActiveBattler];
            gBattlerByTurnOrder[turnOrderId] = gActiveBattler;
            turnOrderId++;
        }
    }
    else
    {
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            {
                if (gChosenActionByBattler[gActiveBattler] == B_ACTION_RUN)
                {
                    turnOrderId = 5;
                    break;
                }
            }
        }
        else
        {
            if (gChosenActionByBattler[0] == B_ACTION_RUN)
            {
                gActiveBattler = 0;
                turnOrderId = 5;
            }
            if (gChosenActionByBattler[2] == B_ACTION_RUN)
            {
                gActiveBattler = 2;
                turnOrderId = 5;
            }
        }

        if (turnOrderId == 5) // One of battlers wants to run.
        {
            gActionsByTurnOrder[0] = gChosenActionByBattler[gActiveBattler];
            gBattlerByTurnOrder[0] = gActiveBattler;
            turnOrderId = 1;
            for (i = 0; i < gBattlersCount; i++)
            {
                if (i != gActiveBattler)
                {
                    gActionsByTurnOrder[turnOrderId] = gChosenActionByBattler[i];
                    gBattlerByTurnOrder[turnOrderId] = i;
                    turnOrderId++;
                }
            }
            gBattleMainFunc = CheckMegaEvolutionBeforeTurn;
            gBattleStruct->mega.battlerId = 0;
            return;
        }
        else
        {
            for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            {
                if (gChosenActionByBattler[gActiveBattler] == B_ACTION_USE_ITEM
                  || gChosenActionByBattler[gActiveBattler] == B_ACTION_SWITCH
                  || gChosenActionByBattler[gActiveBattler] == B_ACTION_THROW_BALL)
                {
                    gActionsByTurnOrder[turnOrderId] = gChosenActionByBattler[gActiveBattler];
                    gBattlerByTurnOrder[turnOrderId] = gActiveBattler;
                    turnOrderId++;
                }
            }
            for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            {
                if (gChosenActionByBattler[gActiveBattler] != B_ACTION_USE_ITEM
                  && gChosenActionByBattler[gActiveBattler] != B_ACTION_SWITCH
                  && gChosenActionByBattler[gActiveBattler] != B_ACTION_THROW_BALL)
                {
                    gActionsByTurnOrder[turnOrderId] = gChosenActionByBattler[gActiveBattler];
                    gBattlerByTurnOrder[turnOrderId] = gActiveBattler;
                    turnOrderId++;
                }
            }
            for (i = 0; i < gBattlersCount - 1; i++)
            {
                for (j = i + 1; j < gBattlersCount; j++)
                {
                    u8 battler1 = gBattlerByTurnOrder[i];
                    u8 battler2 = gBattlerByTurnOrder[j];
                    if (gActionsByTurnOrder[i] != B_ACTION_USE_ITEM
                        && gActionsByTurnOrder[j] != B_ACTION_USE_ITEM
                        && gActionsByTurnOrder[i] != B_ACTION_SWITCH
                        && gActionsByTurnOrder[j] != B_ACTION_SWITCH
                        && gActionsByTurnOrder[i] != B_ACTION_THROW_BALL
                        && gActionsByTurnOrder[j] != B_ACTION_THROW_BALL)
                    {
                        if (GetWhoStrikesFirst(battler1, battler2, FALSE))
                            SwapTurnOrder(i, j);
                    }
                }
            }
        }
    }
    gBattleMainFunc = CheckMegaEvolutionBeforeTurn;
    gBattleStruct->mega.battlerId = 0;
}

static void TurnValuesCleanUp(bool8 var0)
{
    s32 i;

    for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
    {
        if (var0)
        {
            gProtectStructs[gActiveBattler].protected = FALSE;
            gProtectStructs[gActiveBattler].spikyShielded = FALSE;
            gProtectStructs[gActiveBattler].kingsShielded = FALSE;
            gProtectStructs[gActiveBattler].banefulBunkered = FALSE;
        }
        else
        {
            memset(&gProtectStructs[gActiveBattler], 0, sizeof(struct ProtectStruct));

            if (gDisableStructs[gActiveBattler].isFirstTurn)
                gDisableStructs[gActiveBattler].isFirstTurn--;

            if (gDisableStructs[gActiveBattler].rechargeTimer)
            {
                gDisableStructs[gActiveBattler].rechargeTimer--;
                if (gDisableStructs[gActiveBattler].rechargeTimer == 0)
                    gBattleMons[gActiveBattler].status2 &= ~(STATUS2_RECHARGE);
            }
        }

        if (gDisableStructs[gActiveBattler].substituteHP == 0)
            gBattleMons[gActiveBattler].status2 &= ~(STATUS2_SUBSTITUTE);

        gSpecialStatuses[gActiveBattler].parentalBondOn = 0;
    }

    gSideStatuses[0] &= ~(SIDE_STATUS_QUICK_GUARD | SIDE_STATUS_WIDE_GUARD | SIDE_STATUS_CRAFTY_SHIELD | SIDE_STATUS_MAT_BLOCK);
    gSideStatuses[1] &= ~(SIDE_STATUS_QUICK_GUARD | SIDE_STATUS_WIDE_GUARD | SIDE_STATUS_CRAFTY_SHIELD | SIDE_STATUS_MAT_BLOCK);
    gSideTimers[0].followmeTimer = 0;
    gSideTimers[1].followmeTimer = 0;
}

void SpecialStatusesClear(void)
{
    memset(&gSpecialStatuses, 0, sizeof(gSpecialStatuses));
}

static void CheckMegaEvolutionBeforeTurn(void)
{
    if (!(gHitMarker & HITMARKER_RUN))
    {
        while (gBattleStruct->mega.battlerId < gBattlersCount)
        {
            gActiveBattler = gBattlerAttacker = gBattleStruct->mega.battlerId;
            gBattleStruct->mega.battlerId++;
            if (gBattleStruct->mega.toEvolve & gBitTable[gActiveBattler]
                && !(gProtectStructs[gActiveBattler].noValidMoves))
            {
                gBattleStruct->mega.toEvolve &= ~(gBitTable[gActiveBattler]);
                gLastUsedItem = gBattleMons[gActiveBattler].item;
                if (gBattleStruct->mega.isWishMegaEvo == TRUE)
                    BattleScriptExecute(BattleScript_WishMegaEvolution);
                else
                    BattleScriptExecute(BattleScript_MegaEvolution);
                return;
            }
        }
    }

    #if B_MEGA_EVO_TURN_ORDER <= GEN_6
        gBattleMainFunc = CheckFocusPunch_ClearVarsBeforeTurnStarts;
        gBattleStruct->focusPunchBattlerId = 0;
    #else
        gBattleMainFunc = TryChangeTurnOrder; // This will just do nothing if no mon has mega evolved
    #endif  
}

// In gen7, priority and speed are recalculated during the turn in which a pokemon mega evolves
static void TryChangeTurnOrder(void)
{
    s32 i, j;
    for (i = 0; i < gBattlersCount - 1; i++)
    {
        for (j = i + 1; j < gBattlersCount; j++)
        {
            u8 battler1 = gBattlerByTurnOrder[i];
            u8 battler2 = gBattlerByTurnOrder[j];
            if (gActionsByTurnOrder[i] == B_ACTION_USE_MOVE
                && gActionsByTurnOrder[j] == B_ACTION_USE_MOVE)
            {
                if (GetWhoStrikesFirst(battler1, battler2, FALSE))
                    SwapTurnOrder(i, j);
            }
        }
    }
    gBattleMainFunc = CheckFocusPunch_ClearVarsBeforeTurnStarts;
    gBattleStruct->focusPunchBattlerId = 0;
}


static void CheckFocusPunch_ClearVarsBeforeTurnStarts(void)
{
    u32 i;

    if (!(gHitMarker & HITMARKER_RUN))
    {
        while (gBattleStruct->focusPunchBattlerId < gBattlersCount)
        {
            gActiveBattler = gBattlerAttacker = gBattleStruct->focusPunchBattlerId;
            gBattleStruct->focusPunchBattlerId++;
            if (gChosenMoveByBattler[gActiveBattler] == MOVE_FOCUS_PUNCH
                && !(gBattleMons[gActiveBattler].status1 & STATUS1_SLEEP)
                && !(gDisableStructs[gBattlerAttacker].truantCounter)
                && !(gProtectStructs[gActiveBattler].noValidMoves))
            {
                BattleScriptExecute(BattleScript_FocusPunchSetUp);
                return;
            }
        }
    }

    gBattleMainFunc = CheckQuickClaw_CustapBerryActivation;
    gBattleStruct->quickClawBattlerId = 0;
}

static void CheckQuickClaw_CustapBerryActivation(void)
{
    u32 i;

    if (!(gHitMarker & HITMARKER_RUN))
    {
        while (gBattleStruct->quickClawBattlerId < gBattlersCount)
        {
            gActiveBattler = gBattlerAttacker = gBattleStruct->quickClawBattlerId;
            gBattleStruct->quickClawBattlerId++;
            if (gChosenActionByBattler[gActiveBattler] == B_ACTION_USE_MOVE
             && gChosenMoveByBattler[gActiveBattler] != MOVE_FOCUS_PUNCH   // quick claw message doesn't need to activate here
             && (gProtectStructs[gActiveBattler].usedCustapBerry || gProtectStructs[gActiveBattler].quickDraw)
             && !(gBattleMons[gActiveBattler].status1 & STATUS1_SLEEP)
             && !(gDisableStructs[gBattlerAttacker].truantCounter)
             && !(gProtectStructs[gActiveBattler].noValidMoves))
            {
                if (gProtectStructs[gActiveBattler].usedCustapBerry)
                {
                    gProtectStructs[gActiveBattler].usedCustapBerry = FALSE;
                    gLastUsedItem = gBattleMons[gActiveBattler].item;
                    PREPARE_ITEM_BUFFER(gBattleTextBuff1, gLastUsedItem);
                    if (GetBattlerHoldEffect(gActiveBattler, FALSE) == HOLD_EFFECT_CUSTAP_BERRY)
                    {
                        // don't record berry since its gone now
                        BattleScriptExecute(BattleScript_CustapBerryActivation);
                    }
                    else
                    {
                        RecordItemEffectBattle(gActiveBattler, GetBattlerHoldEffect(gActiveBattler, FALSE));
                        BattleScriptExecute(BattleScript_QuickClawActivation);
                    }
                }
                else if (gProtectStructs[gActiveBattler].quickDraw)
                {
                    gProtectStructs[gActiveBattler].quickDraw = FALSE;
                    gLastUsedAbility = gBattleMons[gActiveBattler].ability;
                    PREPARE_ABILITY_BUFFER(gBattleTextBuff1, gLastUsedAbility);
                    RecordAbilityBattle(gActiveBattler, gLastUsedAbility);
                    BattleScriptExecute(BattleScript_QuickDrawActivation);
                }
                return;
            }
        }
    }

    // setup stuff before turns/actions
    TryClearRageAndFuryCutter();
    gCurrentTurnActionNumber = 0;
    gCurrentActionFuncId = gActionsByTurnOrder[0];
    gBattleStruct->dynamicMoveType = 0;
    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        gBattleStruct->ateBoost[i] = FALSE;
        gSpecialStatuses[i].gemBoost = FALSE;
    }

    gBattleMainFunc = RunTurnActionsFunctions;
    gBattleCommunication[3] = 0;
    gBattleCommunication[4] = 0;
    gBattleScripting.multihitMoveEffect = 0;
    gBattleResources->battleScriptsStack->size = 0;
}

static void RunTurnActionsFunctions(void)
{
    if (gBattleOutcome != 0)
        gCurrentActionFuncId = B_ACTION_FINISHED;

    *(&gBattleStruct->savedTurnActionNumber) = gCurrentTurnActionNumber;
    sTurnActionsFuncsTable[gCurrentActionFuncId]();

    if (gCurrentTurnActionNumber >= gBattlersCount) // everyone did their actions, turn finished
    {
        gHitMarker &= ~(HITMARKER_PASSIVE_DAMAGE);
        gBattleMainFunc = sEndTurnFuncsTable[gBattleOutcome & 0x7F];
    }
    else
    {
        if (gBattleStruct->savedTurnActionNumber != gCurrentTurnActionNumber) // action turn has been done, clear hitmarker bits for another battlerId
        {
            gHitMarker &= ~(HITMARKER_NO_ATTACKSTRING);
            gHitMarker &= ~(HITMARKER_UNABLE_TO_USE_MOVE);
        }
    }
}

static void HandleEndTurn_BattleWon(void)
{
    gCurrentActionFuncId = 0;

    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
    {
        gSpecialVar_Result = gBattleOutcome;
        gBattleTextBuff1[0] = gBattleOutcome;
        gBattlerAttacker = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        gBattlescriptCurrInstr = BattleScript_LinkBattleWonOrLost;
        gBattleOutcome &= ~(B_OUTCOME_LINK_BATTLE_RAN);
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER
            && gBattleTypeFlags & (BATTLE_TYPE_FRONTIER | BATTLE_TYPE_TRAINER_HILL | BATTLE_TYPE_EREADER_TRAINER))
    {
        BattleStopLowHpSound();
        gBattlescriptCurrInstr = BattleScript_FrontierTrainerBattleWon;

        if (gTrainerBattleOpponent_A == TRAINER_FRONTIER_BRAIN)
            PlayBGM(MUS_VICTORY_GYM_LEADER);
        else
            PlayBGM(MUS_VICTORY_TRAINER);
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && !(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        BattleStopLowHpSound();
        gBattlescriptCurrInstr = BattleScript_LocalTrainerBattleWon;

        switch (gTrainers[gTrainerBattleOpponent_A].trainerClass)
        {
        case TRAINER_CLASS_ELITE_FOUR:
        case TRAINER_CLASS_CHAMPION:
            PlayBGM(MUS_VICTORY_LEAGUE);
            break;
        case TRAINER_CLASS_TEAM_AQUA:
        case TRAINER_CLASS_TEAM_MAGMA:
        case TRAINER_CLASS_AQUA_ADMIN:
        case TRAINER_CLASS_AQUA_LEADER:
        case TRAINER_CLASS_MAGMA_ADMIN:
        case TRAINER_CLASS_MAGMA_LEADER:
            PlayBGM(MUS_VICTORY_AQUA_MAGMA);
            break;
        case TRAINER_CLASS_LEADER:
            PlayBGM(MUS_VICTORY_GYM_LEADER);
            break;
        case TRAINER_CLASS_PKMN_TRAINER_1:
            PlayBGM(DP_SEQ_WINCHAMP);
            break;
        default:
            PlayBGM(MUS_VICTORY_TRAINER);
            break;
        }
    }
    else
    {
        gBattlescriptCurrInstr = BattleScript_PayDayMoneyAndPickUpItems;
    }

    gBattleMainFunc = HandleEndTurn_FinishBattle;
}

static void HandleEndTurn_BattleLost(void)
{
    gCurrentActionFuncId = 0;

    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
    {
        if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
        {
            if (gBattleOutcome & B_OUTCOME_LINK_BATTLE_RAN)
            {
                gBattlescriptCurrInstr = BattleScript_PrintPlayerForfeitedLinkBattle;
                gBattleOutcome &= ~(B_OUTCOME_LINK_BATTLE_RAN);
                gSaveBlock2Ptr->frontier.disableRecordBattle = TRUE;
            }
            else
            {
                gBattlescriptCurrInstr = BattleScript_FrontierLinkBattleLost;
                gBattleOutcome &= ~(B_OUTCOME_LINK_BATTLE_RAN);
            }
        }
        else
        {
            gBattleTextBuff1[0] = gBattleOutcome;
            gBattlerAttacker = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
            gBattlescriptCurrInstr = BattleScript_LinkBattleWonOrLost;
            gBattleOutcome &= ~(B_OUTCOME_LINK_BATTLE_RAN);
        }
    }
    else
    {
        gBattlescriptCurrInstr = BattleScript_LocalBattleLost;
    }

    gBattleMainFunc = HandleEndTurn_FinishBattle;
}

static void HandleEndTurn_RanFromBattle(void)
{
    gCurrentActionFuncId = 0;

    if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER && gBattleTypeFlags & BATTLE_TYPE_TRAINER)
    {
        gBattlescriptCurrInstr = BattleScript_PrintPlayerForfeited;
        gBattleOutcome = B_OUTCOME_FORFEITED;
        gSaveBlock2Ptr->frontier.disableRecordBattle = TRUE;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
    {
        gBattlescriptCurrInstr = BattleScript_PrintPlayerForfeited;
        gBattleOutcome = B_OUTCOME_FORFEITED;
    }
    else
    {
        switch (gProtectStructs[gBattlerAttacker].fleeFlag)
        {
        default:
            gBattlescriptCurrInstr = BattleScript_GotAwaySafely;
            break;
        case 1:
            gBattlescriptCurrInstr = BattleScript_SmokeBallEscape;
            break;
        case 2:
            gBattlescriptCurrInstr = BattleScript_RanAwayUsingMonAbility;
            break;
        }
    }

    gBattleMainFunc = HandleEndTurn_FinishBattle;
}

static void HandleEndTurn_MonFled(void)
{
    gCurrentActionFuncId = 0;

    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gBattlerAttacker, gBattlerPartyIndexes[gBattlerAttacker]);
    gBattlescriptCurrInstr = BattleScript_WildMonFled;

    gBattleMainFunc = HandleEndTurn_FinishBattle;
}

static void HandleEndTurn_FinishBattle(void)
{
    u32 i;

    if (gCurrentActionFuncId == B_ACTION_TRY_FINISH || gCurrentActionFuncId == B_ACTION_FINISHED)
    {
        if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK
                                  | BATTLE_TYPE_RECORDED_LINK
                                  | BATTLE_TYPE_FIRST_BATTLE
                                  | BATTLE_TYPE_SAFARI
                                  | BATTLE_TYPE_EREADER_TRAINER
                                  | BATTLE_TYPE_WALLY_TUTORIAL
                                  | BATTLE_TYPE_FRONTIER)))
        {
            for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            {
                if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
                {
                    if (gBattleResults.playerMon1Species == SPECIES_NONE)
                    {
                        gBattleResults.playerMon1Species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES, NULL);
                        GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_NICKNAME, gBattleResults.playerMon1Name);
                    }
                    else
                    {
                        gBattleResults.playerMon2Species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES, NULL);
                        GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_NICKNAME, gBattleResults.playerMon2Name);
                    }
                }
            }
            TryPutPokemonTodayOnAir();
        }

        if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK
                                  | BATTLE_TYPE_RECORDED_LINK
                                  | BATTLE_TYPE_TRAINER
                                  | BATTLE_TYPE_FIRST_BATTLE
                                  | BATTLE_TYPE_SAFARI
                                  | BATTLE_TYPE_FRONTIER
                                  | BATTLE_TYPE_EREADER_TRAINER
                                  | BATTLE_TYPE_WALLY_TUTORIAL))
            && gBattleResults.shinyWildMon)
        {
            TryPutBreakingNewsOnAir();
        }

        sub_8186444();
        BeginFastPaletteFade(3);
        FadeOutMapMusic(5);
        #if B_TRAINERS_KNOCK_OFF_ITEMS
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
            TryRestoreStolenItems();
        #endif
        for (i = 0; i < PARTY_SIZE; i++)
        {
            UndoMegaEvolution(i);
            UndoFormChange(i, B_SIDE_PLAYER, FALSE);
            DoBurmyFormChange(i);
        }
    #if B_RECALCULATE_STATS >= GEN_5
        // Recalculate the stats of every party member before the end
        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2) != SPECIES_NONE
             && GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2) != SPECIES_EGG)
            {
                CalculateMonStats(&gPlayerParty[i]);
            }
        }
    #endif
        gBattleMainFunc = FreeResetData_ReturnToOvOrDoEvolutions;
        gCB2_AfterEvolution = BattleMainCB2;
    }
    else
    {
        if (gBattleControllerExecFlags == 0)
            gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
    }
}

static void FreeResetData_ReturnToOvOrDoEvolutions(void)
{
    if (!gPaletteFade.active)
    {
        gIsFishingEncounter = FALSE;
        gIsSurfingEncounter = FALSE;
        ResetSpriteData();
        if (gLeveledUpInBattle && (gBattleOutcome == B_OUTCOME_WON || gBattleOutcome == B_OUTCOME_CAUGHT))
        {
            gBattleMainFunc = TryEvolvePokemon;
        }
        else
        {
            gBattleMainFunc = ReturnFromBattleToOverworld;
            return;
        }
    }

    FreeAllWindowBuffers();
    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        FreeMonSpritesGfx();
        FreeBattleResources();
        FreeBattleSpritesData();
    }
}

static void TryEvolvePokemon(void)
{
    s32 i;

    while (gLeveledUpInBattle != 0)
    {
        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (gLeveledUpInBattle & gBitTable[i])
            {
                u16 species;
                u8 levelUpBits = gLeveledUpInBattle;

                levelUpBits &= ~(gBitTable[i]);
                gLeveledUpInBattle = levelUpBits;

                species = GetEvolutionTargetSpecies(&gPlayerParty[i], EVO_MODE_NORMAL, levelUpBits, SPECIES_NONE);
                if (species != SPECIES_NONE)
                {
                    FreeAllWindowBuffers();
                    gBattleMainFunc = WaitForEvoSceneToFinish;
                    EvolutionScene(&gPlayerParty[i], species, TRUE, i);
                    return;
                }
            }
        }
    }

    gBattleMainFunc = ReturnFromBattleToOverworld;
}

static void WaitForEvoSceneToFinish(void)
{
    if (gMain.callback2 == BattleMainCB2)
        gBattleMainFunc = TryEvolvePokemon;
}

static void ReturnFromBattleToOverworld(void)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        RandomlyGivePartyPokerus(gPlayerParty);
        PartySpreadPokerus(gPlayerParty);
    }

    if (gBattleTypeFlags & BATTLE_TYPE_LINK && gReceivedRemoteLinkPlayers != 0)
        return;

    gSpecialVar_Result = gBattleOutcome;
    gMain.inBattle = 0;
    gMain.callback1 = gPreBattleCallback1;

    if (gBattleTypeFlags & BATTLE_TYPE_ROAMER)
    {
        UpdateRoamerHPStatus(&gEnemyParty[0]);

#ifndef BUGFIX
        if ((gBattleOutcome & B_OUTCOME_WON) || gBattleOutcome == B_OUTCOME_CAUGHT)
#else
        if ((gBattleOutcome == B_OUTCOME_WON) || gBattleOutcome == B_OUTCOME_CAUGHT) // Bug: When Roar is used by roamer, gBattleOutcome is B_OUTCOME_PLAYER_TELEPORTED (5).
#endif                                                                               // & with B_OUTCOME_WON (1) will return TRUE and deactivates the roamer.
            SetRoamerInactive();
    }

    m4aSongNumStop(SE_LOW_HEALTH);
    SetMainCallback2(gMain.savedCallback);
}

void RunBattleScriptCommands_PopCallbacksStack(void)
{
    if (gCurrentActionFuncId == B_ACTION_TRY_FINISH || gCurrentActionFuncId == B_ACTION_FINISHED)
    {
        if (gBattleResources->battleCallbackStack->size != 0)
            gBattleResources->battleCallbackStack->size--;
        gBattleMainFunc = gBattleResources->battleCallbackStack->function[gBattleResources->battleCallbackStack->size];
    }
    else
    {
        if (gBattleControllerExecFlags == 0)
            gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
    }
}

void RunBattleScriptCommands(void)
{
    if (gBattleControllerExecFlags == 0)
        gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
}

void SetTypeBeforeUsingMove(u16 move, u8 battlerAtk)
{
    u32 moveType, ateType, attackerAbility;
    u16 holdEffect = GetBattlerHoldEffect(battlerAtk, TRUE);

    if (move == MOVE_STRUGGLE)
        return;

    gBattleStruct->dynamicMoveType = 0;
    gBattleStruct->ateBoost[battlerAtk] = 0;
    gSpecialStatuses[battlerAtk].gemBoost = FALSE;

    if (gBattleMoves[move].effect == EFFECT_WEATHER_BALL)
    {
        if (WEATHER_HAS_EFFECT)
        {
            if (gBattleWeather & WEATHER_RAIN_ANY && holdEffect != HOLD_EFFECT_UTILITY_UMBRELLA)
                gBattleStruct->dynamicMoveType = TYPE_WATER | 0x80;
            else if (gBattleWeather & WEATHER_SANDSTORM_ANY)
                gBattleStruct->dynamicMoveType = TYPE_ROCK | 0x80;
            else if (gBattleWeather & WEATHER_SUN_ANY && holdEffect != HOLD_EFFECT_UTILITY_UMBRELLA)
                gBattleStruct->dynamicMoveType = TYPE_FIRE | 0x80;
            else if (gBattleWeather & WEATHER_HAIL_ANY)
                gBattleStruct->dynamicMoveType = TYPE_ICE | 0x80;
            else
                gBattleStruct->dynamicMoveType = TYPE_NORMAL | 0x80;
        }
    }
    else if (gBattleMoves[move].effect == EFFECT_HIDDEN_POWER)
    {
        u8 typeBits  = ((gBattleMons[battlerAtk].hpIV & 1) << 0)
                     | ((gBattleMons[battlerAtk].attackIV & 1) << 1)
                     | ((gBattleMons[battlerAtk].defenseIV & 1) << 2)
                     | ((gBattleMons[battlerAtk].speedIV & 1) << 3)
                     | ((gBattleMons[battlerAtk].spAttackIV & 1) << 4)
                     | ((gBattleMons[battlerAtk].spDefenseIV & 1) << 5);

        gBattleStruct->dynamicMoveType = (15 * typeBits) / 63 + 1;
        if (gBattleStruct->dynamicMoveType >= TYPE_MYSTERY)
            gBattleStruct->dynamicMoveType++;
        gBattleStruct->dynamicMoveType |= 0xC0;
    }
    else if (gBattleMoves[move].effect == EFFECT_CHANGE_TYPE_ON_ITEM)
    {
        if (holdEffect == gBattleMoves[move].argument)
            gBattleStruct->dynamicMoveType = ItemId_GetSecondaryId(gBattleMons[battlerAtk].item) | 0x80;
    }
    else if (gBattleMoves[move].effect == EFFECT_REVELATION_DANCE)
    {
        if (gBattleMons[battlerAtk].type1 != TYPE_MYSTERY)
            gBattleStruct->dynamicMoveType = gBattleMons[battlerAtk].type1 | 0x80;
        else if (gBattleMons[battlerAtk].type2 != TYPE_MYSTERY)
            gBattleStruct->dynamicMoveType = gBattleMons[battlerAtk].type2 | 0x80;
        else if (gBattleMons[battlerAtk].type3 != TYPE_MYSTERY)
            gBattleStruct->dynamicMoveType = gBattleMons[battlerAtk].type3 | 0x80;
    }
    else if (gBattleMoves[move].effect == EFFECT_NATURAL_GIFT)
    {
        if (ItemId_GetPocket(gBattleMons[battlerAtk].item) == POCKET_BERRIES)
            gBattleStruct->dynamicMoveType = gNaturalGiftTable[ITEM_TO_BERRY(gBattleMons[battlerAtk].item)].type;
    }
    else if (gBattleMoves[move].effect == EFFECT_TERRAIN_PULSE)
    {
        if (IsBattlerTerrainAffected(battlerAtk, STATUS_FIELD_TERRAIN_ANY))
        {
            if (gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN)
                gBattleStruct->dynamicMoveType = TYPE_ELECTRIC | 0x80;
            else if (gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN)
                gBattleStruct->dynamicMoveType = TYPE_GRASS | 0x80;
            else if (gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN)
                gBattleStruct->dynamicMoveType = TYPE_FAIRY | 0x80;
            else if (gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN)
                gBattleStruct->dynamicMoveType = TYPE_PSYCHIC | 0x80;
            else //failsafe
                gBattleStruct->dynamicMoveType = TYPE_NORMAL | 0x80;
        }
    }

    attackerAbility = GetBattlerAbility(battlerAtk);
    GET_MOVE_TYPE(move, moveType);
    if ((gFieldStatuses & STATUS_FIELD_ION_DELUGE && moveType == TYPE_NORMAL)
        || gStatuses4[battlerAtk] & STATUS4_ELECTRIFIED)
    {
        gBattleStruct->dynamicMoveType = 0x80 | TYPE_ELECTRIC;
    }
    else if (gBattleMoves[move].type == TYPE_NORMAL
             && gBattleMoves[move].effect != EFFECT_HIDDEN_POWER
             && gBattleMoves[move].effect != EFFECT_WEATHER_BALL
             && gBattleMoves[move].effect != EFFECT_CHANGE_TYPE_ON_ITEM
             && gBattleMoves[move].effect != EFFECT_NATURAL_GIFT
             && ((attackerAbility == ABILITY_PIXILATE && (ateType = TYPE_FAIRY))
                 || (attackerAbility == ABILITY_REFRIGERATE && (ateType = TYPE_ICE))
                 || (attackerAbility == ABILITY_AERILATE && (ateType = TYPE_FLYING))
                 || ((attackerAbility == ABILITY_GALVANIZE) && (ateType = TYPE_ELECTRIC))
                )
             )
    {
        gBattleStruct->dynamicMoveType = 0x80 | ateType;
        gBattleStruct->ateBoost[battlerAtk] = 1;
    }
    else if (gBattleMoves[move].type != TYPE_NORMAL
             && gBattleMoves[move].effect != EFFECT_HIDDEN_POWER
             && gBattleMoves[move].effect != EFFECT_WEATHER_BALL
             && attackerAbility == ABILITY_NORMALIZE)
    {
        gBattleStruct->dynamicMoveType = 0x80 | TYPE_NORMAL;
        gBattleStruct->ateBoost[battlerAtk] = 1;
    }
    else if (gBattleMoves[move].flags & FLAG_SOUND
             && attackerAbility == ABILITY_LIQUID_VOICE)
    {
        gBattleStruct->dynamicMoveType = 0x80 | TYPE_WATER;
    }
    else if (gBattleMoves[move].flags & FLAG_SOUND && attackerAbility == ABILITY_SAND_SONG)
    {
        gBattleStruct->dynamicMoveType = 0x80 | TYPE_GROUND;
    }
    else if (gStatuses4[battlerAtk] & STATUS4_PLASMA_FISTS && moveType == TYPE_NORMAL)
    {
        gBattleStruct->dynamicMoveType = 0x80 | TYPE_ELECTRIC;
    }
    else if (move == MOVE_AURA_WHEEL && gBattleMons[battlerAtk].species == SPECIES_MORPEKO_HANGRY)
    {
        gBattleStruct->dynamicMoveType = 0x80 | TYPE_DARK;
    }

    // Check if a gem should activate.
    GET_MOVE_TYPE(move, moveType);
    if (holdEffect == HOLD_EFFECT_GEMS
        && moveType == ItemId_GetSecondaryId(gBattleMons[battlerAtk].item))
    {
        gSpecialStatuses[battlerAtk].gemParam = GetBattlerHoldEffectParam(battlerAtk);
        gSpecialStatuses[battlerAtk].gemBoost = TRUE;
    }
}

// special to set a field's totem boost(s)
// inputs:
//  var8000: battlerId
//  var8001 - var8007: stat changes
void SetTotemBoost(void)
{
    u8 battlerId = gSpecialVar_0x8000;
    u8 i;

    for (i = 0; i < (NUM_BATTLE_STATS - 1); i++)
    {
        if (*(&gSpecialVar_0x8001 + i))
        {
            gTotemBoosts[battlerId].stats |= (1 << i);
            gTotemBoosts[battlerId].statChanges[i] = *(&gSpecialVar_0x8001 + i);
            gTotemBoosts[battlerId].stats |= 0x80;  // used as a flag for the "totem flared to life" script
        }
    }
}




static u8 getRole (u16 species)
{
	u16 max = 0;
	u16 atk = gBaseStats[species].baseAttack;
	u16 spAtk = gBaseStats[species].baseSpAttack;
	u16 def = gBaseStats[species].baseDefense;
	u16 spDef = gBaseStats[species].baseSpDefense;
	u16 HP = gBaseStats[species].baseHP;
		
		
		
	if (atk >= spAtk)
		max = atk;
	else
		max = spAtk;
	
	if ((max * 3) >= ((def + spDef + HP) * 0.9))
		return 0;
	else
		return 1;	
};





static u16 getHeldItem (u16 species)
{
	u16 hp = gBaseStats[species].baseHP;
	u16 atk = gBaseStats[species].baseAttack;
	u16 spAtk = gBaseStats[species].baseSpAttack;
	u16 def = gBaseStats[species].baseDefense;
	u16 spDef = gBaseStats[species].baseSpDefense;
	u16 speed = gBaseStats[species].baseSpeed;
	u16 maxOffense = 0;
	u16 maxDefense = 0;
	
	if (atk > spAtk)
		maxOffense = atk;
	else
		maxOffense = spAtk;
	
	if (def > hp)
		maxDefense = def;
	else
		maxDefense = hp;
	
	if (getRole(species) == 0)
	{
		if ((9 * maxOffense) > (4 * (hp + def + spDef)))
			return ITEM_FOCUS_SASH;
		if ((spDef + def + hp) >= (speed + atk + spAtk))
		    return ITEM_ASSAULT_VEST;
		return ITEM_LIFE_ORB;
	}
	else
	{
		if (gBaseStats[species].type1 == TYPE_POISON || gBaseStats[species].type2 == TYPE_POISON)
			return ITEM_BLACK_SLUDGE;
		else
		{
			if ((Random() % 2) == 0)
				return ITEM_ROCKY_HELMET;
			else
				return ITEM_LEFTOVERS;
		}
	}
	
};




static u16 getStabMove (u16 ID, u8 i, u16 atk, u16 spAtk)
{
	//generated by code
	static const u16 oldPlayerMoveTypeArrays[][3] = 
	{
		{0, 4, 15}, //(0)Type One: Normal, Type Two: Normal, Result: Normal, Ground, Ice - Score: 597
		{0, 1, 4}, //(1)Type One: Normal, Type Two: Fighting, Result: Normal, Fighting, Ground - Score: 564
		{0, 2, 4}, //(2)Type One: Normal, Type Two: Flying, Result: Normal, Flying, Ground - Score: 576
		{0, 3, 4}, //(3)Type One: Normal, Type Two: Poison, Result: Normal, Poison, Ground - Score: 533
		{0, 4, 15}, //(4)Type One: Normal, Type Two: Ground, Result: Normal, Ground, Ice - Score: 597
		{0, 4, 5}, //(5)Type One: Normal, Type Two: Rock, Result: Normal, Ground, Rock - Score: 588
		{0, 4, 6}, //(6)Type One: Normal, Type Two: Bug, Result: Normal, Ground, Bug - Score: 546
		{0, 4, 7}, //(7)Type One: Normal, Type Two: Ghost, Result: Normal, Ground, Ghost - Score: 533
		{0, 4, 8}, //(8)Type One: Normal, Type Two: Steel, Result: Normal, Ground, Steel - Score: 550
		{0, 0, 0}, //(9)Type One: Normal, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{0, 4, 10}, //(10)Type One: Normal, Type Two: Fire, Result: Normal, Ground, Fire - Score: 583
		{0, 10, 11}, //(11)Type One: Normal, Type Two: Water, Result: Normal, Fire, Water - Score: 529
		{0, 4, 12}, //(12)Type One: Normal, Type Two: Grass, Result: Normal, Ground, Grass - Score: 529
		{0, 4, 13}, //(13)Type One: Normal, Type Two: Electric, Result: Normal, Ground, Electric - Score: 536
		{0, 4, 14}, //(14)Type One: Normal, Type Two: Psychic, Result: Normal, Ground, Psychic - Score: 519
		{0, 4, 15}, //(15)Type One: Normal, Type Two: Ice, Result: Normal, Ground, Ice - Score: 597
		{0, 4, 16}, //(16)Type One: Normal, Type Two: Dragon, Result: Normal, Ground, Dragon - Score: 511
		{0, 0, 0}, //(17)Type One: Normal, Type Two: Dark, Result: Normal, Normal, Normal - Score: 0
		{0, 4, 18}, //(18)Type One: Normal, Type Two: Fairy, Result: Normal, Ground, Fairy - Score: 569
		{0, 1, 4}, //(19)Type One: Fighting, Type Two: Normal, Result: Normal, Fighting, Ground - Score: 564
		{1, 4, 15}, //(20)Type One: Fighting, Type Two: Fighting, Result: Fighting, Ground, Ice - Score: 661
		{1, 2, 4}, //(21)Type One: Fighting, Type Two: Flying, Result: Fighting, Flying, Ground - Score: 644
		{1, 3, 4}, //(22)Type One: Fighting, Type Two: Poison, Result: Fighting, Poison, Ground - Score: 605
		{1, 4, 15}, //(23)Type One: Fighting, Type Two: Ground, Result: Fighting, Ground, Ice - Score: 661
		{1, 4, 5}, //(24)Type One: Fighting, Type Two: Rock, Result: Fighting, Ground, Rock - Score: 656
		{1, 4, 6}, //(25)Type One: Fighting, Type Two: Bug, Result: Fighting, Ground, Bug - Score: 595
		{1, 4, 7}, //(26)Type One: Fighting, Type Two: Ghost, Result: Fighting, Ground, Ghost - Score: 612
		{1, 4, 8}, //(27)Type One: Fighting, Type Two: Steel, Result: Fighting, Ground, Steel - Score: 607
		{0, 0, 0}, //(28)Type One: Fighting, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{1, 4, 10}, //(29)Type One: Fighting, Type Two: Fire, Result: Fighting, Ground, Fire - Score: 639
		{1, 5, 11}, //(30)Type One: Fighting, Type Two: Water, Result: Fighting, Rock, Water - Score: 605
		{1, 5, 12}, //(31)Type One: Fighting, Type Two: Grass, Result: Fighting, Rock, Grass - Score: 604
		{1, 4, 13}, //(32)Type One: Fighting, Type Two: Electric, Result: Fighting, Ground, Electric - Score: 603
		{1, 5, 14}, //(33)Type One: Fighting, Type Two: Psychic, Result: Fighting, Rock, Psychic - Score: 608
		{1, 4, 15}, //(34)Type One: Fighting, Type Two: Ice, Result: Fighting, Ground, Ice - Score: 661
		{1, 4, 16}, //(35)Type One: Fighting, Type Two: Dragon, Result: Fighting, Ground, Dragon - Score: 582
		{1, 5, 17}, //(36)Type One: Fighting, Type Two: Dark, Result: Fighting, Rock, Dark - Score: 605
		{1, 4, 18}, //(37)Type One: Fighting, Type Two: Fairy, Result: Fighting, Ground, Fairy - Score: 624
		{0, 2, 4}, //(38)Type One: Flying, Type Two: Normal, Result: Normal, Flying, Ground - Score: 576
		{1, 2, 4}, //(39)Type One: Flying, Type Two: Fighting, Result: Fighting, Flying, Ground - Score: 644
		{2, 4, 15}, //(40)Type One: Flying, Type Two: Flying, Result: Flying, Ground, Ice - Score: 651
		{2, 3, 4}, //(41)Type One: Flying, Type Two: Poison, Result: Flying, Poison, Ground - Score: 597
		{2, 4, 15}, //(42)Type One: Flying, Type Two: Ground, Result: Flying, Ground, Ice - Score: 651
		{2, 4, 5}, //(43)Type One: Flying, Type Two: Rock, Result: Flying, Ground, Rock - Score: 646
		{2, 4, 6}, //(44)Type One: Flying, Type Two: Bug, Result: Flying, Ground, Bug - Score: 615
		{2, 4, 7}, //(45)Type One: Flying, Type Two: Ghost, Result: Flying, Ground, Ghost - Score: 610
		{2, 4, 8}, //(46)Type One: Flying, Type Two: Steel, Result: Flying, Ground, Steel - Score: 626
		{0, 0, 0}, //(47)Type One: Flying, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{2, 4, 10}, //(48)Type One: Flying, Type Two: Fire, Result: Flying, Ground, Fire - Score: 622
		{2, 4, 11}, //(49)Type One: Flying, Type Two: Water, Result: Flying, Ground, Water - Score: 607
		{2, 4, 12}, //(50)Type One: Flying, Type Two: Grass, Result: Flying, Ground, Grass - Score: 616
		{2, 4, 13}, //(51)Type One: Flying, Type Two: Electric, Result: Flying, Ground, Electric - Score: 618
		{2, 4, 14}, //(52)Type One: Flying, Type Two: Psychic, Result: Flying, Ground, Psychic - Score: 583
		{2, 4, 15}, //(53)Type One: Flying, Type Two: Ice, Result: Flying, Ground, Ice - Score: 651
		{2, 4, 16}, //(54)Type One: Flying, Type Two: Dragon, Result: Flying, Ground, Dragon - Score: 594
		{2, 4, 17}, //(55)Type One: Flying, Type Two: Dark, Result: Flying, Ground, Dark - Score: 610
		{2, 4, 18}, //(56)Type One: Flying, Type Two: Fairy, Result: Flying, Ground, Fairy - Score: 625
		{0, 3, 4}, //(57)Type One: Poison, Type Two: Normal, Result: Normal, Poison, Ground - Score: 533
		{1, 3, 4}, //(58)Type One: Poison, Type Two: Fighting, Result: Fighting, Poison, Ground - Score: 605
		{2, 3, 4}, //(59)Type One: Poison, Type Two: Flying, Result: Flying, Poison, Ground - Score: 597
		{3, 4, 5}, //(60)Type One: Poison, Type Two: Poison, Result: Poison, Ground, Rock - Score: 622
		{3, 4, 5}, //(61)Type One: Poison, Type Two: Ground, Result: Poison, Ground, Rock - Score: 622
		{3, 4, 5}, //(62)Type One: Poison, Type Two: Rock, Result: Poison, Ground, Rock - Score: 622
		{3, 4, 6}, //(63)Type One: Poison, Type Two: Bug, Result: Poison, Ground, Bug - Score: 575
		{3, 4, 7}, //(64)Type One: Poison, Type Two: Ghost, Result: Poison, Ground, Ghost - Score: 577
		{3, 4, 8}, //(65)Type One: Poison, Type Two: Steel, Result: Poison, Ground, Steel - Score: 577
		{0, 0, 0}, //(66)Type One: Poison, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{3, 4, 10}, //(67)Type One: Poison, Type Two: Fire, Result: Poison, Ground, Fire - Score: 608
		{3, 4, 11}, //(68)Type One: Poison, Type Two: Water, Result: Poison, Ground, Water - Score: 572
		{3, 4, 12}, //(69)Type One: Poison, Type Two: Grass, Result: Poison, Ground, Grass - Score: 575
		{3, 4, 13}, //(70)Type One: Poison, Type Two: Electric, Result: Poison, Ground, Electric - Score: 579
		{3, 4, 14}, //(71)Type One: Poison, Type Two: Psychic, Result: Poison, Ground, Psychic - Score: 563
		{3, 4, 15}, //(72)Type One: Poison, Type Two: Ice, Result: Poison, Ground, Ice - Score: 622
		{3, 4, 16}, //(73)Type One: Poison, Type Two: Dragon, Result: Poison, Ground, Dragon - Score: 557
		{3, 4, 17}, //(74)Type One: Poison, Type Two: Dark, Result: Poison, Ground, Dark - Score: 575
		{3, 4, 18}, //(75)Type One: Poison, Type Two: Fairy, Result: Poison, Ground, Fairy - Score: 604
		{0, 4, 15}, //(76)Type One: Ground, Type Two: Normal, Result: Normal, Ground, Ice - Score: 597
		{1, 4, 15}, //(77)Type One: Ground, Type Two: Fighting, Result: Fighting, Ground, Ice - Score: 661
		{2, 4, 15}, //(78)Type One: Ground, Type Two: Flying, Result: Flying, Ground, Ice - Score: 651
		{3, 4, 5}, //(79)Type One: Ground, Type Two: Poison, Result: Poison, Ground, Rock - Score: 622
		{4, 5, 15}, //(80)Type One: Ground, Type Two: Ground, Result: Ground, Rock, Ice - Score: 669
		{4, 5, 15}, //(81)Type One: Ground, Type Two: Rock, Result: Ground, Rock, Ice - Score: 669
		{4, 5, 6}, //(82)Type One: Ground, Type Two: Bug, Result: Ground, Rock, Bug - Score: 637
		{4, 7, 15}, //(83)Type One: Ground, Type Two: Ghost, Result: Ground, Ghost, Ice - Score: 625
		{4, 8, 15}, //(84)Type One: Ground, Type Two: Steel, Result: Ground, Steel, Ice - Score: 646
		{0, 0, 0}, //(85)Type One: Ground, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 10, 15}, //(86)Type One: Ground, Type Two: Fire, Result: Ground, Fire, Ice - Score: 665
		{4, 5, 11}, //(87)Type One: Ground, Type Two: Water, Result: Ground, Rock, Water - Score: 614
		{4, 5, 12}, //(88)Type One: Ground, Type Two: Grass, Result: Ground, Rock, Grass - Score: 629
		{4, 10, 13}, //(89)Type One: Ground, Type Two: Electric, Result: Ground, Fire, Electric - Score: 626
		{4, 14, 15}, //(90)Type One: Ground, Type Two: Psychic, Result: Ground, Psychic, Ice - Score: 616
		{4, 5, 15}, //(91)Type One: Ground, Type Two: Ice, Result: Ground, Rock, Ice - Score: 669
		{4, 10, 16}, //(92)Type One: Ground, Type Two: Dragon, Result: Ground, Fire, Dragon - Score: 607
		{4, 15, 17}, //(93)Type One: Ground, Type Two: Dark, Result: Ground, Ice, Dark - Score: 621
		{4, 5, 18}, //(94)Type One: Ground, Type Two: Fairy, Result: Ground, Rock, Fairy - Score: 657
		{0, 4, 5}, //(95)Type One: Rock, Type Two: Normal, Result: Normal, Ground, Rock - Score: 588
		{1, 4, 5}, //(96)Type One: Rock, Type Two: Fighting, Result: Fighting, Ground, Rock - Score: 656
		{2, 4, 5}, //(97)Type One: Rock, Type Two: Flying, Result: Flying, Ground, Rock - Score: 646
		{3, 4, 5}, //(98)Type One: Rock, Type Two: Poison, Result: Poison, Ground, Rock - Score: 622
		{4, 5, 15}, //(99)Type One: Rock, Type Two: Ground, Result: Ground, Rock, Ice - Score: 669
		{4, 5, 15}, //(100)Type One: Rock, Type Two: Rock, Result: Ground, Rock, Ice - Score: 669
		{4, 5, 6}, //(101)Type One: Rock, Type Two: Bug, Result: Ground, Rock, Bug - Score: 637
		{4, 5, 7}, //(102)Type One: Rock, Type Two: Ghost, Result: Ground, Rock, Ghost - Score: 620
		{4, 5, 8}, //(103)Type One: Rock, Type Two: Steel, Result: Ground, Rock, Steel - Score: 623
		{0, 0, 0}, //(104)Type One: Rock, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 5, 10}, //(105)Type One: Rock, Type Two: Fire, Result: Ground, Rock, Fire - Score: 641
		{4, 5, 11}, //(106)Type One: Rock, Type Two: Water, Result: Ground, Rock, Water - Score: 614
		{4, 5, 12}, //(107)Type One: Rock, Type Two: Grass, Result: Ground, Rock, Grass - Score: 629
		{4, 5, 13}, //(108)Type One: Rock, Type Two: Electric, Result: Ground, Rock, Electric - Score: 607
		{4, 5, 14}, //(109)Type One: Rock, Type Two: Psychic, Result: Ground, Rock, Psychic - Score: 615
		{4, 5, 15}, //(110)Type One: Rock, Type Two: Ice, Result: Ground, Rock, Ice - Score: 669
		{4, 5, 16}, //(111)Type One: Rock, Type Two: Dragon, Result: Ground, Rock, Dragon - Score: 605
		{4, 5, 17}, //(112)Type One: Rock, Type Two: Dark, Result: Ground, Rock, Dark - Score: 615
		{4, 5, 18}, //(113)Type One: Rock, Type Two: Fairy, Result: Ground, Rock, Fairy - Score: 657
		{0, 4, 6}, //(114)Type One: Bug, Type Two: Normal, Result: Normal, Ground, Bug - Score: 546
		{1, 4, 6}, //(115)Type One: Bug, Type Two: Fighting, Result: Fighting, Ground, Bug - Score: 595
		{2, 4, 6}, //(116)Type One: Bug, Type Two: Flying, Result: Flying, Ground, Bug - Score: 615
		{3, 4, 6}, //(117)Type One: Bug, Type Two: Poison, Result: Poison, Ground, Bug - Score: 575
		{4, 5, 6}, //(118)Type One: Bug, Type Two: Ground, Result: Ground, Rock, Bug - Score: 637
		{4, 5, 6}, //(119)Type One: Bug, Type Two: Rock, Result: Ground, Rock, Bug - Score: 637
		{4, 5, 6}, //(120)Type One: Bug, Type Two: Bug, Result: Ground, Rock, Bug - Score: 637
		{4, 6, 7}, //(121)Type One: Bug, Type Two: Ghost, Result: Ground, Bug, Ghost - Score: 580
		{4, 6, 8}, //(122)Type One: Bug, Type Two: Steel, Result: Ground, Bug, Steel - Score: 600
		{0, 0, 0}, //(123)Type One: Bug, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 6, 10}, //(124)Type One: Bug, Type Two: Fire, Result: Ground, Bug, Fire - Score: 618
		{4, 6, 11}, //(125)Type One: Bug, Type Two: Water, Result: Ground, Bug, Water - Score: 579
		{4, 6, 12}, //(126)Type One: Bug, Type Two: Grass, Result: Ground, Bug, Grass - Score: 570
		{4, 6, 13}, //(127)Type One: Bug, Type Two: Electric, Result: Ground, Bug, Electric - Score: 593
		{4, 6, 14}, //(128)Type One: Bug, Type Two: Psychic, Result: Ground, Bug, Psychic - Score: 580
		{4, 6, 15}, //(129)Type One: Bug, Type Two: Ice, Result: Ground, Bug, Ice - Score: 633
		{4, 6, 16}, //(130)Type One: Bug, Type Two: Dragon, Result: Ground, Bug, Dragon - Score: 564
		{4, 6, 17}, //(131)Type One: Bug, Type Two: Dark, Result: Ground, Bug, Dark - Score: 571
		{4, 6, 18}, //(132)Type One: Bug, Type Two: Fairy, Result: Ground, Bug, Fairy - Score: 607
		{0, 4, 7}, //(133)Type One: Ghost, Type Two: Normal, Result: Normal, Ground, Ghost - Score: 533
		{1, 4, 7}, //(134)Type One: Ghost, Type Two: Fighting, Result: Fighting, Ground, Ghost - Score: 612
		{2, 4, 7}, //(135)Type One: Ghost, Type Two: Flying, Result: Flying, Ground, Ghost - Score: 610
		{3, 4, 7}, //(136)Type One: Ghost, Type Two: Poison, Result: Poison, Ground, Ghost - Score: 577
		{4, 7, 15}, //(137)Type One: Ghost, Type Two: Ground, Result: Ground, Ghost, Ice - Score: 625
		{4, 5, 7}, //(138)Type One: Ghost, Type Two: Rock, Result: Ground, Rock, Ghost - Score: 620
		{4, 6, 7}, //(139)Type One: Ghost, Type Two: Bug, Result: Ground, Bug, Ghost - Score: 580
		{4, 7, 15}, //(140)Type One: Ghost, Type Two: Ghost, Result: Ground, Ghost, Ice - Score: 625
		{4, 7, 8}, //(141)Type One: Ghost, Type Two: Steel, Result: Ground, Ghost, Steel - Score: 587
		{0, 0, 0}, //(142)Type One: Ghost, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 7, 10}, //(143)Type One: Ghost, Type Two: Fire, Result: Ground, Ghost, Fire - Score: 616
		{1, 7, 11}, //(144)Type One: Ghost, Type Two: Water, Result: Fighting, Ghost, Water - Score: 571
		{4, 7, 12}, //(145)Type One: Ghost, Type Two: Grass, Result: Ground, Ghost, Grass - Score: 563
		{4, 7, 13}, //(146)Type One: Ghost, Type Two: Electric, Result: Ground, Ghost, Electric - Score: 571
		{1, 7, 14}, //(147)Type One: Ghost, Type Two: Psychic, Result: Fighting, Ghost, Psychic - Score: 573
		{4, 7, 15}, //(148)Type One: Ghost, Type Two: Ice, Result: Ground, Ghost, Ice - Score: 625
		{4, 7, 16}, //(149)Type One: Ghost, Type Two: Dragon, Result: Ground, Ghost, Dragon - Score: 552
		{4, 7, 17}, //(150)Type One: Ghost, Type Two: Dark, Result: Ground, Ghost, Dark - Score: 534
		{4, 7, 18}, //(151)Type One: Ghost, Type Two: Fairy, Result: Ground, Ghost, Fairy - Score: 606
		{0, 4, 8}, //(152)Type One: Steel, Type Two: Normal, Result: Normal, Ground, Steel - Score: 550
		{1, 4, 8}, //(153)Type One: Steel, Type Two: Fighting, Result: Fighting, Ground, Steel - Score: 607
		{2, 4, 8}, //(154)Type One: Steel, Type Two: Flying, Result: Flying, Ground, Steel - Score: 626
		{3, 4, 8}, //(155)Type One: Steel, Type Two: Poison, Result: Poison, Ground, Steel - Score: 577
		{4, 8, 15}, //(156)Type One: Steel, Type Two: Ground, Result: Ground, Steel, Ice - Score: 646
		{4, 5, 8}, //(157)Type One: Steel, Type Two: Rock, Result: Ground, Rock, Steel - Score: 623
		{4, 6, 8}, //(158)Type One: Steel, Type Two: Bug, Result: Ground, Bug, Steel - Score: 600
		{4, 7, 8}, //(159)Type One: Steel, Type Two: Ghost, Result: Ground, Ghost, Steel - Score: 587
		{4, 8, 15}, //(160)Type One: Steel, Type Two: Steel, Result: Ground, Steel, Ice - Score: 646
		{0, 0, 0}, //(161)Type One: Steel, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 8, 10}, //(162)Type One: Steel, Type Two: Fire, Result: Ground, Steel, Fire - Score: 619
		{4, 8, 11}, //(163)Type One: Steel, Type Two: Water, Result: Ground, Steel, Water - Score: 575
		{4, 8, 12}, //(164)Type One: Steel, Type Two: Grass, Result: Ground, Steel, Grass - Score: 588
		{4, 8, 13}, //(165)Type One: Steel, Type Two: Electric, Result: Ground, Steel, Electric - Score: 593
		{4, 8, 14}, //(166)Type One: Steel, Type Two: Psychic, Result: Ground, Steel, Psychic - Score: 577
		{4, 8, 15}, //(167)Type One: Steel, Type Two: Ice, Result: Ground, Steel, Ice - Score: 646
		{4, 8, 16}, //(168)Type One: Steel, Type Two: Dragon, Result: Ground, Steel, Dragon - Score: 571
		{4, 8, 17}, //(169)Type One: Steel, Type Two: Dark, Result: Ground, Steel, Dark - Score: 587
		{4, 8, 18}, //(170)Type One: Steel, Type Two: Fairy, Result: Ground, Steel, Fairy - Score: 618
		{0, 0, 0}, //(171)Type One: 9, Type Two: Normal, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(172)Type One: 9, Type Two: Fighting, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(173)Type One: 9, Type Two: Flying, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(174)Type One: 9, Type Two: Poison, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(175)Type One: 9, Type Two: Ground, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(176)Type One: 9, Type Two: Rock, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(177)Type One: 9, Type Two: Bug, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(178)Type One: 9, Type Two: Ghost, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(179)Type One: 9, Type Two: Steel, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(180)Type One: 9, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(181)Type One: 9, Type Two: Fire, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(182)Type One: 9, Type Two: Water, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(183)Type One: 9, Type Two: Grass, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(184)Type One: 9, Type Two: Electric, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(185)Type One: 9, Type Two: Psychic, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(186)Type One: 9, Type Two: Ice, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(187)Type One: 9, Type Two: Dragon, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(188)Type One: 9, Type Two: Dark, Result: Normal, Normal, Normal - Score: 0
		{0, 0, 0}, //(189)Type One: 9, Type Two: Fairy, Result: Normal, Normal, Normal - Score: 0
		{0, 4, 10}, //(190)Type One: Fire, Type Two: Normal, Result: Normal, Ground, Fire - Score: 583
		{1, 4, 10}, //(191)Type One: Fire, Type Two: Fighting, Result: Fighting, Ground, Fire - Score: 639
		{2, 4, 10}, //(192)Type One: Fire, Type Two: Flying, Result: Flying, Ground, Fire - Score: 622
		{3, 4, 10}, //(193)Type One: Fire, Type Two: Poison, Result: Poison, Ground, Fire - Score: 608
		{4, 10, 15}, //(194)Type One: Fire, Type Two: Ground, Result: Ground, Fire, Ice - Score: 665
		{4, 5, 10}, //(195)Type One: Fire, Type Two: Rock, Result: Ground, Rock, Fire - Score: 641
		{4, 6, 10}, //(196)Type One: Fire, Type Two: Bug, Result: Ground, Bug, Fire - Score: 618
		{4, 7, 10}, //(197)Type One: Fire, Type Two: Ghost, Result: Ground, Ghost, Fire - Score: 616
		{4, 8, 10}, //(198)Type One: Fire, Type Two: Steel, Result: Ground, Steel, Fire - Score: 619
		{0, 0, 0}, //(199)Type One: Fire, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 10, 15}, //(200)Type One: Fire, Type Two: Fire, Result: Ground, Fire, Ice - Score: 665
		{4, 10, 11}, //(201)Type One: Fire, Type Two: Water, Result: Ground, Fire, Water - Score: 613
		{4, 10, 12}, //(202)Type One: Fire, Type Two: Grass, Result: Ground, Fire, Grass - Score: 624
		{4, 10, 13}, //(203)Type One: Fire, Type Two: Electric, Result: Ground, Fire, Electric - Score: 626
		{4, 10, 14}, //(204)Type One: Fire, Type Two: Psychic, Result: Ground, Fire, Psychic - Score: 605
		{4, 10, 15}, //(205)Type One: Fire, Type Two: Ice, Result: Ground, Fire, Ice - Score: 665
		{4, 10, 16}, //(206)Type One: Fire, Type Two: Dragon, Result: Ground, Fire, Dragon - Score: 607
		{4, 10, 17}, //(207)Type One: Fire, Type Two: Dark, Result: Ground, Fire, Dark - Score: 612
		{4, 10, 18}, //(208)Type One: Fire, Type Two: Fairy, Result: Ground, Fire, Fairy - Score: 652
		{0, 10, 11}, //(209)Type One: Water, Type Two: Normal, Result: Normal, Fire, Water - Score: 529
		{1, 5, 11}, //(210)Type One: Water, Type Two: Fighting, Result: Fighting, Rock, Water - Score: 605
		{2, 4, 11}, //(211)Type One: Water, Type Two: Flying, Result: Flying, Ground, Water - Score: 607
		{3, 4, 11}, //(212)Type One: Water, Type Two: Poison, Result: Poison, Ground, Water - Score: 572
		{4, 5, 11}, //(213)Type One: Water, Type Two: Ground, Result: Ground, Rock, Water - Score: 614
		{4, 5, 11}, //(214)Type One: Water, Type Two: Rock, Result: Ground, Rock, Water - Score: 614
		{4, 6, 11}, //(215)Type One: Water, Type Two: Bug, Result: Ground, Bug, Water - Score: 579
		{1, 7, 11}, //(216)Type One: Water, Type Two: Ghost, Result: Fighting, Ghost, Water - Score: 571
		{4, 8, 11}, //(217)Type One: Water, Type Two: Steel, Result: Ground, Steel, Water - Score: 575
		{0, 0, 0}, //(218)Type One: Water, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 10, 11}, //(219)Type One: Water, Type Two: Fire, Result: Ground, Fire, Water - Score: 613
		{4, 5, 11}, //(220)Type One: Water, Type Two: Water, Result: Ground, Rock, Water - Score: 614
		{10, 11, 12}, //(221)Type One: Water, Type Two: Grass, Result: Fire, Water, Grass - Score: 551
		{10, 11, 13}, //(222)Type One: Water, Type Two: Electric, Result: Fire, Water, Electric - Score: 569
		{10, 11, 14}, //(223)Type One: Water, Type Two: Psychic, Result: Fire, Water, Psychic - Score: 563
		{4, 11, 15}, //(224)Type One: Water, Type Two: Ice, Result: Ground, Water, Ice - Score: 612
		{10, 11, 16}, //(225)Type One: Water, Type Two: Dragon, Result: Fire, Water, Dragon - Score: 560
		{1, 11, 17}, //(226)Type One: Water, Type Two: Dark, Result: Fighting, Water, Dark - Score: 564
		{10, 11, 18}, //(227)Type One: Water, Type Two: Fairy, Result: Fire, Water, Fairy - Score: 603
		{0, 4, 12}, //(228)Type One: Grass, Type Two: Normal, Result: Normal, Ground, Grass - Score: 529
		{1, 5, 12}, //(229)Type One: Grass, Type Two: Fighting, Result: Fighting, Rock, Grass - Score: 604
		{2, 4, 12}, //(230)Type One: Grass, Type Two: Flying, Result: Flying, Ground, Grass - Score: 616
		{3, 4, 12}, //(231)Type One: Grass, Type Two: Poison, Result: Poison, Ground, Grass - Score: 575
		{4, 5, 12}, //(232)Type One: Grass, Type Two: Ground, Result: Ground, Rock, Grass - Score: 629
		{4, 5, 12}, //(233)Type One: Grass, Type Two: Rock, Result: Ground, Rock, Grass - Score: 629
		{4, 6, 12}, //(234)Type One: Grass, Type Two: Bug, Result: Ground, Bug, Grass - Score: 570
		{4, 7, 12}, //(235)Type One: Grass, Type Two: Ghost, Result: Ground, Ghost, Grass - Score: 563
		{4, 8, 12}, //(236)Type One: Grass, Type Two: Steel, Result: Ground, Steel, Grass - Score: 588
		{0, 0, 0}, //(237)Type One: Grass, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 10, 12}, //(238)Type One: Grass, Type Two: Fire, Result: Ground, Fire, Grass - Score: 624
		{10, 11, 12}, //(239)Type One: Grass, Type Two: Water, Result: Fire, Water, Grass - Score: 551
		{4, 5, 12}, //(240)Type One: Grass, Type Two: Grass, Result: Ground, Rock, Grass - Score: 629
		{4, 12, 13}, //(241)Type One: Grass, Type Two: Electric, Result: Ground, Grass, Electric - Score: 556
		{5, 12, 14}, //(242)Type One: Grass, Type Two: Psychic, Result: Rock, Grass, Psychic - Score: 555
		{4, 12, 15}, //(243)Type One: Grass, Type Two: Ice, Result: Ground, Grass, Ice - Score: 627
		{4, 12, 16}, //(244)Type One: Grass, Type Two: Dragon, Result: Ground, Grass, Dragon - Score: 552
		{4, 12, 17}, //(245)Type One: Grass, Type Two: Dark, Result: Ground, Grass, Dark - Score: 556
		{4, 12, 18}, //(246)Type One: Grass, Type Two: Fairy, Result: Ground, Grass, Fairy - Score: 603
		{0, 4, 13}, //(247)Type One: Electric, Type Two: Normal, Result: Normal, Ground, Electric - Score: 536
		{1, 4, 13}, //(248)Type One: Electric, Type Two: Fighting, Result: Fighting, Ground, Electric - Score: 603
		{2, 4, 13}, //(249)Type One: Electric, Type Two: Flying, Result: Flying, Ground, Electric - Score: 618
		{3, 4, 13}, //(250)Type One: Electric, Type Two: Poison, Result: Poison, Ground, Electric - Score: 579
		{4, 10, 13}, //(251)Type One: Electric, Type Two: Ground, Result: Ground, Fire, Electric - Score: 626
		{4, 5, 13}, //(252)Type One: Electric, Type Two: Rock, Result: Ground, Rock, Electric - Score: 607
		{4, 6, 13}, //(253)Type One: Electric, Type Two: Bug, Result: Ground, Bug, Electric - Score: 593
		{4, 7, 13}, //(254)Type One: Electric, Type Two: Ghost, Result: Ground, Ghost, Electric - Score: 571
		{4, 8, 13}, //(255)Type One: Electric, Type Two: Steel, Result: Ground, Steel, Electric - Score: 593
		{0, 0, 0}, //(256)Type One: Electric, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 10, 13}, //(257)Type One: Electric, Type Two: Fire, Result: Ground, Fire, Electric - Score: 626
		{10, 11, 13}, //(258)Type One: Electric, Type Two: Water, Result: Fire, Water, Electric - Score: 569
		{4, 12, 13}, //(259)Type One: Electric, Type Two: Grass, Result: Ground, Grass, Electric - Score: 556
		{4, 10, 13}, //(260)Type One: Electric, Type Two: Electric, Result: Ground, Fire, Electric - Score: 626
		{4, 13, 14}, //(261)Type One: Electric, Type Two: Psychic, Result: Ground, Electric, Psychic - Score: 560
		{4, 13, 15}, //(262)Type One: Electric, Type Two: Ice, Result: Ground, Electric, Ice - Score: 626
		{4, 13, 16}, //(263)Type One: Electric, Type Two: Dragon, Result: Ground, Electric, Dragon - Score: 559
		{4, 13, 17}, //(264)Type One: Electric, Type Two: Dark, Result: Ground, Electric, Dark - Score: 566
		{4, 13, 18}, //(265)Type One: Electric, Type Two: Fairy, Result: Ground, Electric, Fairy - Score: 610
		{0, 4, 14}, //(266)Type One: Psychic, Type Two: Normal, Result: Normal, Ground, Psychic - Score: 519
		{1, 5, 14}, //(267)Type One: Psychic, Type Two: Fighting, Result: Fighting, Rock, Psychic - Score: 608
		{2, 4, 14}, //(268)Type One: Psychic, Type Two: Flying, Result: Flying, Ground, Psychic - Score: 583
		{3, 4, 14}, //(269)Type One: Psychic, Type Two: Poison, Result: Poison, Ground, Psychic - Score: 563
		{4, 14, 15}, //(270)Type One: Psychic, Type Two: Ground, Result: Ground, Psychic, Ice - Score: 616
		{4, 5, 14}, //(271)Type One: Psychic, Type Two: Rock, Result: Ground, Rock, Psychic - Score: 615
		{4, 6, 14}, //(272)Type One: Psychic, Type Two: Bug, Result: Ground, Bug, Psychic - Score: 580
		{1, 7, 14}, //(273)Type One: Psychic, Type Two: Ghost, Result: Fighting, Ghost, Psychic - Score: 573
		{4, 8, 14}, //(274)Type One: Psychic, Type Two: Steel, Result: Ground, Steel, Psychic - Score: 577
		{0, 0, 0}, //(275)Type One: Psychic, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 10, 14}, //(276)Type One: Psychic, Type Two: Fire, Result: Ground, Fire, Psychic - Score: 605
		{10, 11, 14}, //(277)Type One: Psychic, Type Two: Water, Result: Fire, Water, Psychic - Score: 563
		{5, 12, 14}, //(278)Type One: Psychic, Type Two: Grass, Result: Rock, Grass, Psychic - Score: 555
		{4, 13, 14}, //(279)Type One: Psychic, Type Two: Electric, Result: Ground, Electric, Psychic - Score: 560
		{4, 14, 15}, //(280)Type One: Psychic, Type Two: Psychic, Result: Ground, Psychic, Ice - Score: 616
		{4, 14, 15}, //(281)Type One: Psychic, Type Two: Ice, Result: Ground, Psychic, Ice - Score: 616
		{4, 14, 16}, //(282)Type One: Psychic, Type Two: Dragon, Result: Ground, Psychic, Dragon - Score: 540
		{1, 14, 17}, //(283)Type One: Psychic, Type Two: Dark, Result: Fighting, Psychic, Dark - Score: 571
		{4, 14, 18}, //(284)Type One: Psychic, Type Two: Fairy, Result: Ground, Psychic, Fairy - Score: 579
		{0, 4, 15}, //(285)Type One: Ice, Type Two: Normal, Result: Normal, Ground, Ice - Score: 597
		{1, 4, 15}, //(286)Type One: Ice, Type Two: Fighting, Result: Fighting, Ground, Ice - Score: 661
		{2, 4, 15}, //(287)Type One: Ice, Type Two: Flying, Result: Flying, Ground, Ice - Score: 651
		{3, 4, 15}, //(288)Type One: Ice, Type Two: Poison, Result: Poison, Ground, Ice - Score: 622
		{4, 5, 15}, //(289)Type One: Ice, Type Two: Ground, Result: Ground, Rock, Ice - Score: 669
		{4, 5, 15}, //(290)Type One: Ice, Type Two: Rock, Result: Ground, Rock, Ice - Score: 669
		{4, 6, 15}, //(291)Type One: Ice, Type Two: Bug, Result: Ground, Bug, Ice - Score: 633
		{4, 7, 15}, //(292)Type One: Ice, Type Two: Ghost, Result: Ground, Ghost, Ice - Score: 625
		{4, 8, 15}, //(293)Type One: Ice, Type Two: Steel, Result: Ground, Steel, Ice - Score: 646
		{0, 0, 0}, //(294)Type One: Ice, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 10, 15}, //(295)Type One: Ice, Type Two: Fire, Result: Ground, Fire, Ice - Score: 665
		{4, 11, 15}, //(296)Type One: Ice, Type Two: Water, Result: Ground, Water, Ice - Score: 612
		{4, 12, 15}, //(297)Type One: Ice, Type Two: Grass, Result: Ground, Grass, Ice - Score: 627
		{4, 13, 15}, //(298)Type One: Ice, Type Two: Electric, Result: Ground, Electric, Ice - Score: 626
		{4, 14, 15}, //(299)Type One: Ice, Type Two: Psychic, Result: Ground, Psychic, Ice - Score: 616
		{4, 5, 15}, //(300)Type One: Ice, Type Two: Ice, Result: Ground, Rock, Ice - Score: 669
		{4, 15, 16}, //(301)Type One: Ice, Type Two: Dragon, Result: Ground, Ice, Dragon - Score: 601
		{4, 15, 17}, //(302)Type One: Ice, Type Two: Dark, Result: Ground, Ice, Dark - Score: 621
		{4, 15, 18}, //(303)Type One: Ice, Type Two: Fairy, Result: Ground, Ice, Fairy - Score: 645
		{0, 4, 16}, //(304)Type One: Dragon, Type Two: Normal, Result: Normal, Ground, Dragon - Score: 511
		{1, 4, 16}, //(305)Type One: Dragon, Type Two: Fighting, Result: Fighting, Ground, Dragon - Score: 582
		{2, 4, 16}, //(306)Type One: Dragon, Type Two: Flying, Result: Flying, Ground, Dragon - Score: 594
		{3, 4, 16}, //(307)Type One: Dragon, Type Two: Poison, Result: Poison, Ground, Dragon - Score: 557
		{4, 10, 16}, //(308)Type One: Dragon, Type Two: Ground, Result: Ground, Fire, Dragon - Score: 607
		{4, 5, 16}, //(309)Type One: Dragon, Type Two: Rock, Result: Ground, Rock, Dragon - Score: 605
		{4, 6, 16}, //(310)Type One: Dragon, Type Two: Bug, Result: Ground, Bug, Dragon - Score: 564
		{4, 7, 16}, //(311)Type One: Dragon, Type Two: Ghost, Result: Ground, Ghost, Dragon - Score: 552
		{4, 8, 16}, //(312)Type One: Dragon, Type Two: Steel, Result: Ground, Steel, Dragon - Score: 571
		{0, 0, 0}, //(313)Type One: Dragon, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 10, 16}, //(314)Type One: Dragon, Type Two: Fire, Result: Ground, Fire, Dragon - Score: 607
		{10, 11, 16}, //(315)Type One: Dragon, Type Two: Water, Result: Fire, Water, Dragon - Score: 560
		{4, 12, 16}, //(316)Type One: Dragon, Type Two: Grass, Result: Ground, Grass, Dragon - Score: 552
		{4, 13, 16}, //(317)Type One: Dragon, Type Two: Electric, Result: Ground, Electric, Dragon - Score: 559
		{4, 14, 16}, //(318)Type One: Dragon, Type Two: Psychic, Result: Ground, Psychic, Dragon - Score: 540
		{4, 15, 16}, //(319)Type One: Dragon, Type Two: Ice, Result: Ground, Ice, Dragon - Score: 601
		{4, 10, 16}, //(320)Type One: Dragon, Type Two: Dragon, Result: Ground, Fire, Dragon - Score: 607
		{4, 16, 17}, //(321)Type One: Dragon, Type Two: Dark, Result: Ground, Dragon, Dark - Score: 545
		{4, 16, 18}, //(322)Type One: Dragon, Type Two: Fairy, Result: Ground, Dragon, Fairy - Score: 569
		{0, 0, 0}, //(323)Type One: Dark, Type Two: Normal, Result: Normal, Normal, Normal - Score: 0
		{1, 5, 17}, //(324)Type One: Dark, Type Two: Fighting, Result: Fighting, Rock, Dark - Score: 605
		{2, 4, 17}, //(325)Type One: Dark, Type Two: Flying, Result: Flying, Ground, Dark - Score: 610
		{3, 4, 17}, //(326)Type One: Dark, Type Two: Poison, Result: Poison, Ground, Dark - Score: 575
		{4, 15, 17}, //(327)Type One: Dark, Type Two: Ground, Result: Ground, Ice, Dark - Score: 621
		{4, 5, 17}, //(328)Type One: Dark, Type Two: Rock, Result: Ground, Rock, Dark - Score: 615
		{4, 6, 17}, //(329)Type One: Dark, Type Two: Bug, Result: Ground, Bug, Dark - Score: 571
		{4, 7, 17}, //(330)Type One: Dark, Type Two: Ghost, Result: Ground, Ghost, Dark - Score: 534
		{4, 8, 17}, //(331)Type One: Dark, Type Two: Steel, Result: Ground, Steel, Dark - Score: 587
		{0, 0, 0}, //(332)Type One: Dark, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 10, 17}, //(333)Type One: Dark, Type Two: Fire, Result: Ground, Fire, Dark - Score: 612
		{1, 11, 17}, //(334)Type One: Dark, Type Two: Water, Result: Fighting, Water, Dark - Score: 564
		{4, 12, 17}, //(335)Type One: Dark, Type Two: Grass, Result: Ground, Grass, Dark - Score: 556
		{4, 13, 17}, //(336)Type One: Dark, Type Two: Electric, Result: Ground, Electric, Dark - Score: 566
		{1, 14, 17}, //(337)Type One: Dark, Type Two: Psychic, Result: Fighting, Psychic, Dark - Score: 571
		{4, 15, 17}, //(338)Type One: Dark, Type Two: Ice, Result: Ground, Ice, Dark - Score: 621
		{4, 16, 17}, //(339)Type One: Dark, Type Two: Dragon, Result: Ground, Dragon, Dark - Score: 545
		{4, 15, 17}, //(340)Type One: Dark, Type Two: Dark, Result: Ground, Ice, Dark - Score: 621
		{4, 17, 18}, //(341)Type One: Dark, Type Two: Fairy, Result: Ground, Dark, Fairy - Score: 606
		{0, 4, 18}, //(342)Type One: Fairy, Type Two: Normal, Result: Normal, Ground, Fairy - Score: 569
		{1, 4, 18}, //(343)Type One: Fairy, Type Two: Fighting, Result: Fighting, Ground, Fairy - Score: 624
		{2, 4, 18}, //(344)Type One: Fairy, Type Two: Flying, Result: Flying, Ground, Fairy - Score: 625
		{3, 4, 18}, //(345)Type One: Fairy, Type Two: Poison, Result: Poison, Ground, Fairy - Score: 604
		{4, 5, 18}, //(346)Type One: Fairy, Type Two: Ground, Result: Ground, Rock, Fairy - Score: 657
		{4, 5, 18}, //(347)Type One: Fairy, Type Two: Rock, Result: Ground, Rock, Fairy - Score: 657
		{4, 6, 18}, //(348)Type One: Fairy, Type Two: Bug, Result: Ground, Bug, Fairy - Score: 607
		{4, 7, 18}, //(349)Type One: Fairy, Type Two: Ghost, Result: Ground, Ghost, Fairy - Score: 606
		{4, 8, 18}, //(350)Type One: Fairy, Type Two: Steel, Result: Ground, Steel, Fairy - Score: 618
		{0, 0, 0}, //(351)Type One: Fairy, Type Two: 9, Result: Normal, Normal, Normal - Score: 0
		{4, 10, 18}, //(352)Type One: Fairy, Type Two: Fire, Result: Ground, Fire, Fairy - Score: 652
		{10, 11, 18}, //(353)Type One: Fairy, Type Two: Water, Result: Fire, Water, Fairy - Score: 603
		{4, 12, 18}, //(354)Type One: Fairy, Type Two: Grass, Result: Ground, Grass, Fairy - Score: 603
		{4, 13, 18}, //(355)Type One: Fairy, Type Two: Electric, Result: Ground, Electric, Fairy - Score: 610
		{4, 14, 18}, //(356)Type One: Fairy, Type Two: Psychic, Result: Ground, Psychic, Fairy - Score: 579
		{4, 15, 18}, //(357)Type One: Fairy, Type Two: Ice, Result: Ground, Ice, Fairy - Score: 645
		{4, 16, 18}, //(358)Type One: Fairy, Type Two: Dragon, Result: Ground, Dragon, Fairy - Score: 569
		{4, 17, 18}, //(359)Type One: Fairy, Type Two: Dark, Result: Ground, Dark, Fairy - Score: 606
		{4, 5, 18} //(360)Type One: Fairy, Type Two: Fairy, Result: Ground, Rock, Fairy - Score: 657
	};
	
	static const u16 oldPlayerTypeMove [][2][3] = 
	{
	    {//Normal
		{MOVE_MULTI_ATTACK, MOVE_HEAD_CHARGE, MOVE_BODY_SLAM},//Physical
		{MOVE_BOOMBURST, MOVE_JUDGMENT, MOVE_TRI_ATTACK},//Special
	    },
	    {//Fighting
		{MOVE_CLOSE_COMBAT, MOVE_FLYING_PRESS, MOVE_SACRED_SWORD},//Physical
		{MOVE_FOCUS_BLAST, MOVE_SECRET_SWORD, MOVE_AURA_SPHERE},//Special
	    },
	    {//Flying
		{MOVE_DRAGON_ASCENT, MOVE_BRAVE_BIRD, MOVE_DRILL_PECK},//Physical
		{MOVE_HURRICANE, MOVE_AEROBLAST, MOVE_OBLIVION_WING},//Special
	    },
	    {//Poison
		{MOVE_GUNK_SHOT, MOVE_POISON_JAB, MOVE_POISON_JAB},//Physical
		{MOVE_SLUDGE_WAVE, MOVE_SLUDGE_BOMB, MOVE_SLUDGE_BOMB},//Special
	    },
	    {//Ground
		{MOVE_EARTHQUAKE, MOVE_THOUSAND_ARROWS, MOVE_HIGH_HORSEPOWER},//Physical
		{MOVE_EARTH_POWER, MOVE_EARTH_POWER, MOVE_EARTH_POWER},//Special
	    },
	    {//Rock
		{MOVE_HEAD_SMASH, MOVE_STONE_EDGE, MOVE_DIAMOND_STORM},//Physical
		{MOVE_POWER_GEM, MOVE_POWER_GEM, MOVE_ANCIENT_POWER},//Special
	    },
	    {//Bug
		{MOVE_MEGAHORN, MOVE_ATTACK_ORDER, MOVE_X_SCISSOR},//Physical
		{MOVE_BUG_BUZZ, MOVE_BUG_BUZZ, MOVE_SIGNAL_BEAM},//Special
	    },
	    {//Ghost
		{MOVE_SPECTRAL_THIEF, MOVE_SHADOW_BONE, MOVE_SPIRIT_SHACKLE},//Physical
		{MOVE_MOONGEIST_BEAM, MOVE_SHADOW_BALL, MOVE_SHADOW_BALL},//Special
	    },
	    {//Steel
		{MOVE_IRON_TAIL, MOVE_IRON_HEAD, MOVE_METEOR_MASH},//Physical
		{MOVE_DOOM_DESIRE, MOVE_FLASH_CANNON, MOVE_FLASH_CANNON},//Special
	    },
	    {//9
		{MOVE_NONE, MOVE_NONE, MOVE_NONE},//Physical
		{MOVE_NONE, MOVE_NONE, MOVE_NONE},//Special
	    },
	    {//Fire
		{MOVE_FLARE_BLITZ, MOVE_SACRED_FIRE, MOVE_FIRE_LASH},//Physical
		{MOVE_BLUE_FLARE, MOVE_FUSION_FLARE, MOVE_FLAMETHROWER},//Special
	    },
	    {//Water
		{MOVE_CRABHAMMER, MOVE_LIQUIDATION, MOVE_FISHIOUS_REND},//Physical
		{MOVE_SURF, MOVE_STEAM_ERUPTION, MOVE_ORIGIN_PULSE},//Special
	    },
	    {//Grass
		{MOVE_WOOD_HAMMER, MOVE_POWER_WHIP, MOVE_LEAF_BLADE},//Physical
		{MOVE_SEED_FLARE, MOVE_ENERGY_BALL, MOVE_GIGA_DRAIN},//Special
	    },
	    {//Electric
		{MOVE_BOLT_STRIKE, MOVE_VOLT_TACKLE, MOVE_FUSION_BOLT},//Physical
		{MOVE_THUNDER, MOVE_THUNDERBOLT, MOVE_THUNDERBOLT},//Special
	    },
	    {//Psychic
		{MOVE_PSYCHIC_FANGS, MOVE_PSYCHIC_FANGS, MOVE_ZEN_HEADBUTT},//Physical
		{MOVE_PSYSTRIKE, MOVE_PSYCHIC, MOVE_PHOTON_GEYSER},//Special
	    },
	    {//Ice
		{MOVE_ICE_HAMMER, MOVE_ICE_HAMMER, MOVE_ICICLE_CRASH},//Physical
		{MOVE_BLIZZARD, MOVE_ICE_BEAM, MOVE_ICE_BEAM},//Special
	    },
	    {//Dragon
		{MOVE_DRAGON_RUSH, MOVE_DRAGON_HAMMER, MOVE_DRAGON_CLAW},//Physical
		{MOVE_CLANGING_SCALES, MOVE_SPACIAL_REND, MOVE_CORE_ENFORCER},//Special
	    },
	    {//Dark
		{MOVE_HYPERSPACE_HOLE, MOVE_DARKEST_LARIAT, MOVE_CRUNCH},//Physical
		{MOVE_NIGHT_DAZE, MOVE_NIGHT_DAZE, MOVE_DARK_PULSE},//Special
	    },
	    {//Fairy
		{MOVE_PLAY_ROUGH, MOVE_PLAY_ROUGH, MOVE_PLAY_ROUGH},//Physical
		{MOVE_LIGHT_OF_RUIN, MOVE_MOONBLAST, MOVE_MOONBLAST},//Special
	    }
	};
	
	u8 moveType = 0;
	u8 randomMove = Random() % 3;
	
	u8 split = 0;
	u32 caster = 1;
	u16 randomValue = Random() % 1000;
	u32 chanceValue = (caster * 500 * atk * atk *atk) / (caster * spAtk * spAtk * spAtk);
	
	if (atk > spAtk)
	{
		chanceValue = (caster * 500 * spAtk * spAtk *spAtk) / (caster * atk * atk * atk);
		if ((atk * 3) > (spAtk * 4))
			split = 0;
		else if (randomValue < chanceValue)
			split = 1;
		else
			split = 0;
	}
	else
	{
		chanceValue = (caster * 500 * atk * atk *atk) / (caster * spAtk * spAtk * spAtk);
		if ((spAtk * 3) > (atk * 4))
			split = 1;
		else if (randomValue < chanceValue)
			split = 0;
		else 
			split = 1;
	}  
	
	if (i >= 3)
		return oldPlayerTypeMove[ID][split][randomMove];
	else
	{
		moveType = oldPlayerMoveTypeArrays[ID][i];
		return oldPlayerTypeMove[moveType][split][randomMove];
	}
};




u16 selectMoves (u16 species, u8 i, u16 atk, u16 spAtk)
{		
	u8 j = 0;
	u8 type1 = gBaseStats[species].type1;
	u8 type2 = gBaseStats[species].type2;
	u8 role = getRole(species);	
	u8 typeUtilityMove = 50;
	u16 ID = (type1 * 19) + type2;
	
	const u16 randomUtilityMove [11][1] = 
	{
		[0] = {MOVE_TOXIC},
		[1] = {MOVE_WILL_O_WISP},
		[2] = {MOVE_SPORE},
		[3] = {MOVE_NUZZLE},
		[4] = {MOVE_TEETER_DANCE},
		[5] = {MOVE_CONFUSE_RAY},
		[6] = {MOVE_DEFEND_ORDER},
		[7] = {MOVE_ACID_ARMOR},
		[8] = {MOVE_AMNESIA},
		[9] = {MOVE_COUNTER},
		[10] = {MOVE_MIRROR_COAT},
	};
	
	
	if (role == 0) // Damage Dealer
	{
		if (i == MAX_MON_MOVES - 1)
		{
			if (type1 == TYPE_NORMAL || type2 == TYPE_NORMAL)
				return MOVE_EXTREME_SPEED;
			else if (type1 == TYPE_DARK || type2 == TYPE_DARK)
				return MOVE_SUCKER_PUNCH;
			else if (type1 == TYPE_BUG || type2 == TYPE_BUG)
				return MOVE_U_TURN ;
			else if (type1 == TYPE_ELECTRIC || type2 == TYPE_ELECTRIC)
				return MOVE_VOLT_SWITCH ;
			else if (type1 == TYPE_WATER || type2 == TYPE_WATER)
				if (atk > spAtk)
					return MOVE_AQUA_JET;
				else
					return MOVE_WATER_SHURIKEN;
			else if (type1 == TYPE_FIGHTING || type2 == TYPE_FIGHTING)
				if (atk > spAtk)
					return MOVE_MACH_PUNCH;
				else
					return MOVE_VACUUM_WAVE;
			else if (type1 == TYPE_STEEL || type2 == TYPE_STEEL)
				return MOVE_BULLET_PUNCH;
			else if (type1 == TYPE_ICE || type2 == TYPE_ICE)
				return MOVE_ICE_SHARD;
			else if (type1 == TYPE_ROCK || type2 == TYPE_ROCK)
				return MOVE_ACCELEROCK;
			else if (type1 == TYPE_GHOST || type2 == TYPE_GHOST)
				return MOVE_SHADOW_SNEAK;	

			for (j = 0; j < MAX_MON_MOVES - 1; j++)
			{
				if (getStabMove(ID, j, atk, spAtk) == TYPE_DARK)
					return MOVE_EXTREME_SPEED;
				else if (getStabMove(ID, j, atk, spAtk) == TYPE_NORMAL)
					return MOVE_SUCKER_PUNCH;
			}

			if (Random() % 2 == 0)
				return MOVE_SUCKER_PUNCH;
			else
				return MOVE_EXTREME_SPEED;

			return MOVE_EXTREME_SPEED;
		}
		return getStabMove(ID, i, atk, spAtk);
	}
	else // Bulky
	{
		switch (i)
		{
			case 0:
				return getStabMove(type1, 3, atk, spAtk);
			case 1:
				if ((Random() % 100) < 16) 
					return MOVE_REST;
				if (type1 == TYPE_BUG || type2 == TYPE_BUG)
					return MOVE_HEAL_ORDER;
				else if ((type1 == TYPE_GROUND || type2 == TYPE_GROUND) || (type1 == TYPE_ROCK || type2 == TYPE_ROCK) 
					|| (type1 == TYPE_FIRE || type2 == TYPE_FIRE))
					return MOVE_SHORE_UP;
				else if (type1 == TYPE_FLYING || type2 == TYPE_FLYING)
					return MOVE_ROOST;
				else if ((type1 == TYPE_GRASS || type2 == TYPE_GRASS) || (type1 == TYPE_FAIRY || type2 == TYPE_FAIRY))
					return MOVE_SYNTHESIS;
				else if (type1 == TYPE_NORMAL || type2 == TYPE_NORMAL)
					return MOVE_SOFT_BOILED;
				else if ((type1 == TYPE_PSYCHIC || type2 == TYPE_PSYCHIC) || (type1 == TYPE_WATER || type2 == TYPE_WATER) 
					|| (type1 == TYPE_ELECTRIC || type2 == TYPE_ELECTRIC) || (type1 == TYPE_ICE || type2 == TYPE_ICE)
					|| (type1 == TYPE_GHOST || type2 == TYPE_GHOST))
					return MOVE_RECOVER;
				else if ((type1 == TYPE_STEEL || type2 == TYPE_STEEL) || (type1 == TYPE_FIGHTING || type2 == TYPE_FIGHTING)
					|| (type1 == TYPE_POISON || type2 == TYPE_POISON) || (type1 == TYPE_DARK || type2 == TYPE_DARK)
					|| (type1 == TYPE_DRAGON || type2 == TYPE_DRAGON))
					return MOVE_SLACK_OFF;
				else
					return MOVE_RECOVER;					
			case 2:
				if ((Random() % 100)  < typeUtilityMove)
				{
					if (type1 == TYPE_BUG || type2 == TYPE_BUG)
						return MOVE_DEFEND_ORDER;
					else if (type1 == TYPE_ELECTRIC || type2 == TYPE_ELECTRIC)
						return MOVE_NUZZLE;
					else if (type1 == TYPE_FIRE || type2 == TYPE_FIRE)
						return MOVE_WILL_O_WISP;
					else if (type1 == TYPE_GRASS || type2 == TYPE_GRASS)
						return MOVE_LEECH_SEED;
					else if (type1 == TYPE_POISON || type2 == TYPE_POISON)
						return MOVE_TOXIC;
					else if (type1 == TYPE_FIGHTING || type2 == TYPE_FIGHTING)
						return MOVE_COUNTER;
					else if (type1 == TYPE_PSYCHIC || type2 == TYPE_PSYCHIC)
						return MOVE_MIRROR_COAT;
				}
				return randomUtilityMove[Random() % ARRAY_COUNT(randomUtilityMove)][0];
			case 3:
				return MOVE_SEISMIC_TOSS;
		}
	}
}











static u16 getAverageBSTEnemyParty(void)
{
	u8 i = 0;
	u16 sum = 0;
	u8 count = 0;
	for (i = 0; i < PARTY_SIZE; i++)
	{
		if (GetBoxMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE
		&& GetBoxMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_EGG)
		{
			sum += GetTotalBaseStat(GetBoxMonData(&gPlayerParty[i], MON_DATA_SPECIES));
			count++;
		}
	}
        //sum the base stat totals of all the pokemons in the enemy party and divide by the amount of pokemon in the enemy party
        return sum/count;
};

static u8 getNewPokemonLevel(u16 species, u8 currentPokemonLevel)
{
        u16 baseStatTotalToScaleTo = 0;
        u16 scaledStat = -1;
        u16 currentPokemonBaseStat = -1; // unused??
	
	u8 EV = 80;
        u8 EV_DISABLED = GetBoxMonDataAt(TOTAL_BOXES_COUNT-1, IN_BOX_COUNT-1, MON_DATA_ATK_IV); // EV settings, 1 == disabled
	
        u16 scaledHp = 0;
        u16 scaledAtk = 0;
        u16 scaledDef = 0;
        u16 scaledSpA = 0;
        u16 scaledSpD = 0;
        u16 scaledSpeed = 0;
	
	s32 Hp = gBaseStats[species].baseHP;
	u16 Atk = gBaseStats[species].baseAttack;
	u16 SpA = gBaseStats[species].baseSpAttack;
	u16 Def = gBaseStats[species].baseDefense;
	u16 SpD = gBaseStats[species].baseSpDefense;
	u16 Speed = gBaseStats[species].baseSpeed;
        
        u16 baseStatTotalCurrentPokemon = 0;
        u16 scaledToBaseStatsRealStatsSum = 0;
        u16 newLevel = 255;
        u16 start = 1;
        u16 mid = 0;
        u16 end = 255;
        u16 scaledToDesiredBaseStatsWithLevelSum = 0;
	
	if(EV_DISABLED == 1)
		EV = 0;

        if(GetTotalBaseStat(SPECIES_MEW) > baseStatTotalToScaleTo)
		baseStatTotalToScaleTo = GetTotalBaseStat(SPECIES_MEW);
        if(GetTotalBaseStat(SPECIES_DRAGONITE) > baseStatTotalToScaleTo)
		baseStatTotalToScaleTo = GetTotalBaseStat(SPECIES_DRAGONITE);
        if(GetTotalBaseStat(SPECIES_MELMETAL) > baseStatTotalToScaleTo)
		baseStatTotalToScaleTo = GetTotalBaseStat(SPECIES_MELMETAL);
        if(getAverageBSTEnemyParty() > baseStatTotalToScaleTo)
		baseStatTotalToScaleTo = getAverageBSTEnemyParty();
	
        baseStatTotalCurrentPokemon = GetTotalBaseStat(species);

        if(baseStatTotalCurrentPokemon < baseStatTotalToScaleTo) 
	{
            scaledHp = (Hp * baseStatTotalToScaleTo) / baseStatTotalCurrentPokemon;
            scaledAtk = (Atk * baseStatTotalToScaleTo) / baseStatTotalCurrentPokemon;
            scaledDef = (Def * baseStatTotalToScaleTo) / baseStatTotalCurrentPokemon;
            scaledSpA = (SpA * baseStatTotalToScaleTo) / baseStatTotalCurrentPokemon;
            scaledSpD = (SpD * baseStatTotalToScaleTo) / baseStatTotalCurrentPokemon;
            scaledSpeed = (Speed * baseStatTotalToScaleTo) / baseStatTotalCurrentPokemon;
        }
	else
	{
            scaledHp = Hp;
            scaledAtk = Atk;
            scaledDef = Def;
            scaledSpA = SpA;
            scaledSpD = SpD;
            scaledSpeed = Speed;
        }
	
        scaledToBaseStatsRealStatsSum += (((2 * scaledHp + 31 + EV / 4) * currentPokemonLevel) / 100) + currentPokemonLevel + 10;
        scaledToBaseStatsRealStatsSum += (((((2 * scaledAtk + 31 + EV / 4) * currentPokemonLevel) / 100) + 5)*110)/100;
        scaledToBaseStatsRealStatsSum += (((((2 * scaledDef + 31 + EV / 4) * currentPokemonLevel) / 100) + 5)*110)/100;
        scaledToBaseStatsRealStatsSum += (((((2 * scaledSpA + 31 + EV / 4) * currentPokemonLevel) / 100) + 5)*110)/100;
        scaledToBaseStatsRealStatsSum += (((((2 * scaledSpD + 31 + EV / 4) * currentPokemonLevel) / 100) + 5)*110)/100;
        scaledToBaseStatsRealStatsSum += (((((2 * scaledSpeed + 31 + EV / 4) * currentPokemonLevel) / 100) + 5)*110)/100;
        
        while(start <= end)
	{
            mid = (start + end) / 2;
            scaledToDesiredBaseStatsWithLevelSum = 0;

            scaledToDesiredBaseStatsWithLevelSum += (((2 * Hp + 31 + EV / 4) * mid) / 100) + mid + 10;
            scaledToDesiredBaseStatsWithLevelSum += (((2 * Atk + 31 + EV / 4) * mid) / 100) + 5;
            scaledToDesiredBaseStatsWithLevelSum += (((2 * Def + 31 + EV / 4) * mid) / 100) + 5;
            scaledToDesiredBaseStatsWithLevelSum += (((2 * SpA + 31 + EV / 4) * mid) / 100) + 5;
            scaledToDesiredBaseStatsWithLevelSum += (((2 * SpD + 31 + EV / 4) * mid) / 100) + 5;
            scaledToDesiredBaseStatsWithLevelSum += (((2 * Speed + 31 + EV / 4) * mid) / 100) + 5;
		    
            if (scaledToDesiredBaseStatsWithLevelSum <= scaledToBaseStatsRealStatsSum) 
	    {
                start = mid + 1;
            }
	    else 
	    {
                newLevel = mid-1;
                end = mid - 1;
            }
        }
	if (newLevel > 100)
		newLevel = 100;
	return newLevel;
}


static u16 pickOldplayerGeneralSpecies(u8 i) 
{
	static const u16 OldplayerGeneralSpeciesArray [][1] = 
	{
		//Gen 1 start
		{SPECIES_VENUSAUR}, //3 (Venusaur) BST: [525]
		{SPECIES_CHARIZARD}, //6 (Charizard) BST: [534]
		{SPECIES_BLASTOISE}, //9 (Blastoise) BST: [530]
		{SPECIES_BUTTERFREE}, //12 (Butterfree) BST: [425]
		{SPECIES_BEEDRILL}, //15 (Beedrill) BST: [420]
		{SPECIES_PIDGEOT}, //18 (Pidgeot) BST: [494]
		{SPECIES_RATICATE}, //20 (Raticate) BST: [438]
		{SPECIES_FEAROW}, //22 (Fearow) BST: [482]
		{SPECIES_ARBOK}, //24 (Arbok) BST: [488]
		{SPECIES_RAICHU}, //26 (Raichu) BST: [490]
		{SPECIES_SANDSLASH}, //28 (Sandslash) BST: [480]
		{SPECIES_NIDOQUEEN}, //31 (Nidoqueen) BST: [505]
		{SPECIES_NIDOKING}, //34 (Nidoking) BST: [505]
		{SPECIES_CLEFABLE}, //36 (Clefable) BST: [483]
		{SPECIES_NINETALES}, //38 (Ninetales) BST: [515]
		{SPECIES_WIGGLYTUFF}, //40 (Wigglytuff) BST: [445]
		{SPECIES_VILEPLUME}, //45 (Vileplume) BST: [510]
		{SPECIES_PARASECT}, //47 (Parasect) BST: [475]
		{SPECIES_VENOMOTH}, //49 (Venomoth) BST: [470]
		{SPECIES_DUGTRIO}, //51 (Dugtrio) BST: [425]
		{SPECIES_PERSIAN}, //53 (Persian) BST: [480]
		{SPECIES_GOLDUCK}, //55 (Golduck) BST: [530]
		{SPECIES_PRIMEAPE}, //57 (Primeape) BST: [485]
		{SPECIES_ARCANINE}, //59 (Arcanine) BST: [555]
		{SPECIES_POLIWRATH}, //62 (Poliwrath) BST: [530]
		{SPECIES_ALAKAZAM}, //65 (Alakazam) BST: [500]
		{SPECIES_MACHAMP}, //68 (Machamp) BST: [505]
		{SPECIES_VICTREEBEL}, //71 (Victreebel) BST: [490]
		{SPECIES_TENTACRUEL}, //73 (Tentacruel) BST: [515]
		{SPECIES_GOLEM}, //76 (Golem) BST: [520]
		{SPECIES_RAPIDASH}, //78 (Rapidash) BST: [530]
		{SPECIES_SLOWBRO}, //80 (Slowbro) BST: [490]
		{SPECIES_FARFETCHD}, //83 (Farfetch’d) BST: [417]
		{SPECIES_DODRIO}, //85 (Dodrio) BST: [490]
		{SPECIES_DEWGONG}, //87 (Dewgong) BST: [520]
		{SPECIES_MUK}, //89 (Muk) BST: [520]
		{SPECIES_CLOYSTER}, //91 (Cloyster) BST: [525]
		{SPECIES_GENGAR}, //94 (Gengar) BST: [500]
		{SPECIES_HYPNO}, //97 (Hypno) BST: [535]
		{SPECIES_KINGLER}, //99 (Kingler) BST: [500]
		{SPECIES_ELECTRODE}, //101 (Electrode) BST: [490]
		{SPECIES_EXEGGUTOR}, //103 (Exeggutor) BST: [530]
		{SPECIES_MAROWAK}, //105 (Marowak) BST: [440]
		{SPECIES_HITMONLEE}, //106 (Hitmonlee) BST: [485]
		{SPECIES_HITMONCHAN}, //107 (Hitmonchan) BST: [485]
		{SPECIES_WEEZING}, //110 (Weezing) BST: [500]
		{SPECIES_KANGASKHAN}, //115 (Kangaskhan) BST: [490]
		{SPECIES_SEAKING}, //119 (Seaking) BST: [520]
		{SPECIES_STARMIE}, //121 (Starmie) BST: [520]
		{SPECIES_MR_MIME}, //122 (Mr. Mime) BST: [480]
		{SPECIES_SCYTHER}, //123 (Scyther) BST: [500]
		{SPECIES_JYNX}, //124 (Jynx) BST: [475]
		{SPECIES_PINSIR}, //127 (Pinsir) BST: [500]
		{SPECIES_TAUROS}, //128 (Tauros) BST: [510]
		{SPECIES_GYARADOS}, //130 (Gyarados) BST: [540]
		{SPECIES_LAPRAS}, //131 (Lapras) BST: [545]
		{SPECIES_VAPOREON}, //134 (Vaporeon) BST: [525]
		{SPECIES_JOLTEON}, //135 (Jolteon) BST: [525]
		{SPECIES_FLAREON}, //136 (Flareon) BST: [525]
		{SPECIES_OMASTAR}, //139 (Omastar) BST: [495]
		{SPECIES_KABUTOPS}, //141 (Kabutops) BST: [495]
		{SPECIES_AERODACTYL}, //142 (Aerodactyl) BST: [515]
		{SPECIES_SNORLAX}, //143 (Snorlax) BST: [540]
		{SPECIES_ARTICUNO}, //144 (Articuno) BST: [580]
		{SPECIES_ZAPDOS}, //145 (Zapdos) BST: [580]
		{SPECIES_MOLTRES}, //146 (Moltres) BST: [580]
		{SPECIES_DRAGONITE}, //149 (Dragonite) BST: [600]
		//Gen 2 start
		{SPECIES_MEGANIUM}, //154 (Meganium) BST: [535]
		{SPECIES_TYPHLOSION}, //157 (Typhlosion) BST: [534]
		{SPECIES_FERALIGATR}, //160 (Feraligatr) BST: [530]
		{SPECIES_FURRET}, //162 (Furret) BST: [455]
		{SPECIES_NOCTOWL}, //164 (Noctowl) BST: [482]
		{SPECIES_LEDIAN}, //166 (Ledian) BST: [450]
		{SPECIES_ARIADOS}, //168 (Ariados) BST: [450]
		{SPECIES_CROBAT}, //169 (Crobat) BST: [535]
		{SPECIES_LANTURN}, //171 (Lanturn) BST: [480]
		{SPECIES_XATU}, //178 (Xatu) BST: [500]
		{SPECIES_AMPHAROS}, //181 (Ampharos) BST: [510]
		{SPECIES_BELLOSSOM}, //182 (Bellossom) BST: [505]
		{SPECIES_AZUMARILL}, //184 (Azumarill) BST: [420]
		{SPECIES_SUDOWOODO}, //185 (Sudowoodo) BST: [430]
		{SPECIES_POLITOED}, //186 (Politoed) BST: [530]
		{SPECIES_JUMPLUFF}, //189 (Jumpluff) BST: [500]
		{SPECIES_SUNFLORA}, //192 (Sunflora) BST: [465]
		{SPECIES_QUAGSIRE}, //195 (Quagsire) BST: [450]
		{SPECIES_ESPEON}, //196 (Espeon) BST: [525]
		{SPECIES_UMBREON}, //197 (Umbreon) BST: [525]
		{SPECIES_SLOWKING}, //199 (Slowking) BST: [490]
		{SPECIES_WOBBUFFET}, //202 (Wobbuffet) BST: [405]
		{SPECIES_GIRAFARIG}, //203 (Girafarig) BST: [495]
		{SPECIES_FORRETRESS}, //205 (Forretress) BST: [475]
		{SPECIES_DUNSPARCE}, //206 (Dunsparce) BST: [455]
		{SPECIES_STEELIX}, //208 (Steelix) BST: [530]
		{SPECIES_GRANBULL}, //210 (Granbull) BST: [480]
		{SPECIES_QWILFISH}, //211 (Qwilfish) BST: [470]
		{SPECIES_SCIZOR}, //212 (Scizor) BST: [500]
		{SPECIES_SHUCKLE}, //213 (Shuckle) BST: [505]
		{SPECIES_HERACROSS}, //214 (Heracross) BST: [500]
		{SPECIES_URSARING}, //217 (Ursaring) BST: [520]
		{SPECIES_MAGCARGO}, //219 (Magcargo) BST: [430]
		{SPECIES_CORSOLA}, //222 (Corsola) BST: [450]
		{SPECIES_OCTILLERY}, //224 (Octillery) BST: [500]
		{SPECIES_DELIBIRD}, //225 (Delibird) BST: [380]	
		{SPECIES_MANTINE}, //226 (Mantine) BST: [485]
		{SPECIES_SKARMORY}, //227 (Skarmory) BST: [465]
		{SPECIES_HOUNDOOM}, //229 (Houndoom) BST: [510]
		{SPECIES_KINGDRA}, //230 (Kingdra) BST: [540]
		{SPECIES_DONPHAN}, //232 (Donphan) BST: [520]
		{SPECIES_STANTLER}, //234 (Stantler) BST: [495]
		{SPECIES_HITMONTOP}, //237 (Hitmontop) BST: [485]
		{SPECIES_MILTANK}, //241 (Miltank) BST: [490]
		{SPECIES_BLISSEY}, //242 (Blissey) BST: [540]
		{SPECIES_RAIKOU}, //243 (Raikou) BST: [580]
		{SPECIES_ENTEI}, //244 (Entei) BST: [580]
		{SPECIES_SUICUNE}, //245 (Suicune) BST: [580]
		{SPECIES_TYRANITAR}, //248 (Tyranitar) BST: [600]
		//Gen 3 start
		{SPECIES_SCEPTILE}, //254 (Sceptile) BST: [530]
		{SPECIES_BLAZIKEN}, //257 (Blaziken) BST: [530]
		{SPECIES_SWAMPERT}, //260 (Swampert) BST: [535]
		{SPECIES_MIGHTYENA}, //262 (Mightyena) BST: [460]
		{SPECIES_LINOONE}, //264 (Linoone) BST: [460]
		{SPECIES_BEAUTIFLY}, //267 (Beautifly) BST: [425]
		{SPECIES_DUSTOX}, //269 (Dustox) BST: [425]
		{SPECIES_LUDICOLO}, //272 (Ludicolo) BST: [500]
		{SPECIES_SHIFTRY}, //275 (Shiftry) BST: [500]
		{SPECIES_SWELLOW}, //277 (Swellow) BST: [465]
		{SPECIES_PELIPPER}, //279 (Pelipper) BST: [440]
		{SPECIES_GARDEVOIR}, //282 (Gardevoir) BST: [533]
		{SPECIES_MASQUERAIN}, //284 (Masquerain) BST: [474]
		{SPECIES_BRELOOM}, //286 (Breloom) BST: [460]
		{SPECIES_SLAKING}, //289 (Slaking) BST: [670]
		{SPECIES_NINJASK}, //291 (Ninjask) BST: [466]
		{SPECIES_SHEDINJA}, //292 (Shedinja) BST: [316]
		{SPECIES_EXPLOUD}, //295 (Exploud) BST: [520]
		{SPECIES_HARIYAMA}, //297 (Hariyama) BST: [504]
		{SPECIES_DELCATTY}, //301 (Delcatty) BST: [470]
		{SPECIES_SABLEYE}, //302 (Sableye) BST: [410]
		{SPECIES_MAWILE}, //303 (Mawile) BST: [410]
		{SPECIES_AGGRON}, //306 (Aggron) BST: [530]
		{SPECIES_MEDICHAM}, //308 (Medicham) BST: [410]
		{SPECIES_MANECTRIC}, //310 (Manectric) BST: [495]
		{SPECIES_PLUSLE}, //311 (Plusle) BST: [435]
		{SPECIES_MINUN}, //312 (Minun) BST: [435]
		{SPECIES_VOLBEAT}, //313 (Volbeat) BST: [470]
		{SPECIES_ILLUMISE}, //314 (Illumise) BST: [470]
		{SPECIES_SWALOT}, //317 (Swalot) BST: [507]
		{SPECIES_SHARPEDO}, //319 (Sharpedo) BST: [480]
		{SPECIES_WAILORD}, //321 (Wailord) BST: [540]
		{SPECIES_CAMERUPT}, //323 (Camerupt) BST: [500]
		{SPECIES_TORKOAL}, //324 (Torkoal) BST: [470]
		{SPECIES_GRUMPIG}, //326 (Grumpig) BST: [500]
		{SPECIES_SPINDA}, //327 (Spinda) BST: [450]
		{SPECIES_FLYGON}, //330 (Flygon) BST: [540]
		{SPECIES_CACTURNE}, //332 (Cacturne) BST: [505]
		{SPECIES_ALTARIA}, //334 (Altaria) BST: [510]
		{SPECIES_ZANGOOSE}, //335 (Zangoose) BST: [493]
		{SPECIES_SEVIPER}, //336 (Seviper) BST: [493]
		{SPECIES_LUNATONE}, //337 (Lunatone) BST: [520]
		{SPECIES_SOLROCK}, //338 (Solrock) BST: [520]
		{SPECIES_WHISCASH}, //340 (Whiscash) BST: [488]
		{SPECIES_CRAWDAUNT}, //342 (Crawdaunt) BST: [468]
		{SPECIES_CLAYDOL}, //344 (Claydol) BST: [530]
		{SPECIES_CRADILY}, //346 (Cradily) BST: [515]
		{SPECIES_ARMALDO}, //348 (Armaldo) BST: [515]
		{SPECIES_MILOTIC}, //350 (Milotic) BST: [540]
		{SPECIES_CASTFORM}, //351 (Castform) BST: [480]
		{SPECIES_KECLEON}, //352 (Kecleon) BST: [470]
		{SPECIES_BANETTE}, //354 (Banette) BST: [485]
		{SPECIES_TROPIUS}, //357 (Tropius) BST: [500]
		{SPECIES_CHIMECHO}, //358 (Chimecho) BST: [485]
		{SPECIES_ABSOL}, //359 (Absol) BST: [485]
		{SPECIES_GLALIE}, //362 (Glalie) BST: [500]
		{SPECIES_WALREIN}, //365 (Walrein) BST: [530]
		{SPECIES_HUNTAIL}, //367 (Huntail) BST: [515]
		{SPECIES_GOREBYSS}, //368 (Gorebyss) BST: [515]
		{SPECIES_RELICANTH}, //369 (Relicanth) BST: [505]
		{SPECIES_LUVDISC}, //370 (Luvdisc) BST: [385]
		{SPECIES_SALAMENCE}, //373 (Salamence) BST: [600]
		{SPECIES_METAGROSS}, //376 (Metagross) BST: [600]
		{SPECIES_REGIROCK}, //377 (Regirock) BST: [580]
		{SPECIES_REGICE}, //378 (Regice) BST: [580]
		{SPECIES_REGISTEEL}, //379 (Registeel) BST: [580]
		//Gen 4 start
		{SPECIES_TORTERRA}, //389 (Torterra) BST: [534]
		{SPECIES_INFERNAPE}, //392 (Infernape) BST: [534]
		{SPECIES_EMPOLEON}, //395 (Empoleon) BST: [530]
		{SPECIES_STARAPTOR}, //398 (Staraptor) BST: [485]
		{SPECIES_BIBAREL}, //400 (Bibarel) BST: [440]
		{SPECIES_KRICKETUNE}, //402 (Kricketune) BST: [429]
		{SPECIES_LUXRAY}, //405 (Luxray) BST: [533]
		{SPECIES_ROSERADE}, //407 (Roserade) BST: [515]
		{SPECIES_RAMPARDOS}, //409 (Rampardos) BST: [515]
		{SPECIES_BASTIODON}, //411 (Bastiodon) BST: [515]

		{SPECIES_WORMADAM}, //413 (Wormadam) BST: [464]
		//	{SPECIES_WORMADAM_SANDY_CLOAK}, //1036 (Wormadam) BST: [464]
		//  {SPECIES_WORMADAM_TRASH_CLOAK}, //1037 (Wormadam) BST: [464]

		{SPECIES_MOTHIM}, //414 (Mothim) BST: [444]
		{SPECIES_VESPIQUEN}, //416 (Vespiquen) BST: [494]
		{SPECIES_PACHIRISU}, //417 (Pachirisu) BST: [455]
		{SPECIES_FLOATZEL}, //419 (Floatzel) BST: [525]
		{SPECIES_CHERRIM}, //421 (Cherrim) BST: [490]

		{SPECIES_GASTRODON}, //423 (Gastrodon) BST: [475]
		//  {SPECIES_GASTRODON_EAST_SEA}, //1040 (Gastrodon) BST: [475]

		{SPECIES_AMBIPOM}, //424 (Ambipom) BST: [492]
		{SPECIES_DRIFBLIM}, //426 (Drifblim) BST: [528]
		{SPECIES_LOPUNNY}, //428 (Lopunny) BST: [500]
		{SPECIES_MISMAGIUS}, //429 (Mismagius) BST: [510]
		{SPECIES_HONCHKROW}, //430 (Honchkrow) BST: [505]
		{SPECIES_PURUGLY}, //432 (Purugly) BST: [499]
		{SPECIES_SKUNTANK}, //435 (Skuntank) BST: [499]
		{SPECIES_BRONZONG}, //437 (Bronzong) BST: [500]
		{SPECIES_CHATOT}, //441 (Chatot) BST: [431]
		{SPECIES_SPIRITOMB}, //442 (Spiritomb) BST: [537]
		{SPECIES_GARCHOMP}, //445 (Garchomp) BST: [600]
		{SPECIES_LUCARIO}, //448 (Lucario) BST: [525]
		{SPECIES_HIPPOWDON}, //450 (Hippowdon) BST: [525]
		{SPECIES_DRAPION}, //452 (Drapion) BST: [520]
		{SPECIES_TOXICROAK}, //454 (Toxicroak) BST: [500]
		{SPECIES_CARNIVINE}, //455 (Carnivine) BST: [494]
		{SPECIES_LUMINEON}, //457 (Lumineon) BST: [500]
		{SPECIES_ABOMASNOW}, //460 (Abomasnow) BST: [514]
		{SPECIES_WEAVILE}, //461 (Weavile) BST: [510]
		{SPECIES_MAGNEZONE}, //462 (Magnezone) BST: [535]
		{SPECIES_LICKILICKY}, //463 (Lickilicky) BST: [535]
		{SPECIES_RHYPERIOR}, //464 (Rhyperior) BST: [535]
		{SPECIES_TANGROWTH}, //465 (Tangrowth) BST: [535]
		{SPECIES_ELECTIVIRE}, //466 (Electivire) BST: [540]
		{SPECIES_MAGMORTAR}, //467 (Magmortar) BST: [540]
		{SPECIES_TOGEKISS}, //468 (Togekiss) BST: [545]
		{SPECIES_YANMEGA}, //469 (Yanmega) BST: [525]
		{SPECIES_LEAFEON}, //470 (Leafeon) BST: [525]
		{SPECIES_GLACEON}, //471 (Glaceon) BST: [525]
		{SPECIES_GLISCOR}, //472 (Gliscor) BST: [510]
		{SPECIES_MAMOSWINE}, //473 (Mamoswine) BST: [530]
		{SPECIES_PORYGON_Z}, //474 (Porygon-Z) BST: [535]
		{SPECIES_GALLADE}, //475 (Gallade) BST: [533]
		{SPECIES_PROBOPASS}, //476 (Probopass) BST: [545]
		{SPECIES_DUSKNOIR}, //477 (Dusknoir) BST: [540]
		{SPECIES_FROSLASS}, //478 (Froslass) BST: [500]

		{SPECIES_ROTOM}, //479 (Rotom) BST: [440]
		//  {SPECIES_ROTOM_HEAT}, //1041 (Rotom) BST: [520]
		//  {SPECIES_ROTOM_WASH}, //1042 (Rotom) BST: [520]
		//  {SPECIES_ROTOM_FROST}, //1043 (Rotom) BST: [520]
		//  {SPECIES_ROTOM_FAN}, //1044 (Rotom) BST: [520]
		//  {SPECIES_ROTOM_MOW}, //1045 (Rotom) BST: [520]

		{SPECIES_UXIE}, //480 (Uxie) BST: [580]
		{SPECIES_MESPRIT}, //481 (Mesprit) BST: [580]
		{SPECIES_AZELF}, //482 (Azelf) BST: [580]
		{SPECIES_HEATRAN}, //485 (Heatran) BST: [600]
		{SPECIES_REGIGIGAS}, //486 (Regigigas) BST: [670]
		{SPECIES_CRESSELIA}, //488 (Cresselia) BST: [600]
		{SPECIES_PHIONE}, //489 (Phione) BST: [480]
		//Gen 5 start
		{SPECIES_SERPERIOR}, //497 (Serperior) BST: [528]
		{SPECIES_EMBOAR}, //500 (Emboar) BST: [528]
		{SPECIES_SAMUROTT}, //503 (Samurott) BST: [528]
		{SPECIES_WATCHOG}, //505 (Watchog) BST: [465]
		{SPECIES_STOUTLAND}, //508 (Stoutland) BST: [510]
		{SPECIES_LIEPARD}, //510 (Liepard) BST: [466]
		{SPECIES_SIMISAGE}, //512 (Simisage) BST: [518]
		{SPECIES_SIMISEAR}, //514 (Simisear) BST: [518]
		{SPECIES_SIMIPOUR}, //516 (Simipour) BST: [518]
		{SPECIES_MUSHARNA}, //518 (Musharna) BST: [497]
		{SPECIES_UNFEZANT}, //521 (Unfezant) BST: [508]
		{SPECIES_ZEBSTRIKA}, //523 (Zebstrika) BST: [517]
		{SPECIES_GIGALITH}, //526 (Gigalith) BST: [515]
		{SPECIES_SWOOBAT}, //528 (Swoobat) BST: [445]
		{SPECIES_EXCADRILL}, //530 (Excadrill) BST: [508]
		{SPECIES_AUDINO}, //531 (Audino) BST: [485]
		{SPECIES_CONKELDURR}, //534 (Conkeldurr) BST: [505]
		{SPECIES_SEISMITOAD}, //537 (Seismitoad) BST: [529]
		{SPECIES_THROH}, //538 (Throh) BST: [465]
		{SPECIES_SAWK}, //539 (Sawk) BST: [465]
		{SPECIES_LEAVANNY}, //542 (Leavanny) BST: [520]
		{SPECIES_SCOLIPEDE}, //545 (Scolipede) BST: [485]
		{SPECIES_WHIMSICOTT}, //547 (Whimsicott) BST: [480]
		{SPECIES_LILLIGANT}, //549 (Lilligant) BST: [480]

		{SPECIES_BASCULIN}, //550 (Basculin) BST: [480]
		//   {SPECIES_BASCULIN_BLUE_STRIPED}, //1065 (Basculin) BST: [480]

		{SPECIES_KROOKODILE}, //553 (Krookodile) BST: [519]
		{SPECIES_DARMANITAN}, //555 (Darmanitan) BST: [480]
		{SPECIES_MARACTUS}, //556 (Maractus) BST: [471]
		{SPECIES_CRUSTLE}, //558 (Crustle) BST: [505]
		{SPECIES_SCRAFTY}, //560 (Scrafty) BST: [488]
		{SPECIES_SIGILYPH}, //561 (Sigilyph) BST: [490]
		{SPECIES_COFAGRIGUS}, //563 (Cofagrigus) BST: [483]
		{SPECIES_CARRACOSTA}, //565 (Carracosta) BST: [515]
		{SPECIES_ARCHEOPS}, //567 (Archeops) BST: [567]
		{SPECIES_GARBODOR}, //569 (Garbodor) BST: [494]
		{SPECIES_ZOROARK}, //571 (Zoroark) BST: [510]
		{SPECIES_CINCCINO}, //573 (Cinccino) BST: [490]
		{SPECIES_GOTHITELLE}, //576 (Gothitelle) BST: [490]
		{SPECIES_REUNICLUS}, //579 (Reuniclus) BST: [490]
		{SPECIES_SWANNA}, //581 (Swanna) BST: [498]
		{SPECIES_VANILLUXE}, //584 (Vanilluxe) BST: [535]

		{SPECIES_SAWSBUCK}, //586 (Sawsbuck) BST: [495]
		//  {SPECIES_SAWSBUCK_SUMMER}, //1071 (Sawsbuck) BST: [495]
		//  {SPECIES_SAWSBUCK_AUTUMN}, //1072 (Sawsbuck) BST: [495]
		//  {SPECIES_SAWSBUCK_WINTER}, //1073 (Sawsbuck) BST: [495]

		{SPECIES_EMOLGA}, //587 (Emolga) BST: [468]
		{SPECIES_ESCAVALIER}, //589 (Escavalier) BST: [495]
		{SPECIES_AMOONGUSS}, //591 (Amoonguss) BST: [464]
		{SPECIES_JELLICENT}, //593 (Jellicent) BST: [480]
		{SPECIES_ALOMOMOLA}, //594 (Alomomola) BST: [470]
		{SPECIES_GALVANTULA}, //596 (Galvantula) BST: [482]
		{SPECIES_FERROTHORN}, //598 (Ferrothorn) BST: [489]
		{SPECIES_KLINKLANG}, //601 (Klinklang) BST: [520]
		{SPECIES_EELEKTROSS}, //604 (Eelektross) BST: [535]
		{SPECIES_BEHEEYEM}, //606 (Beheeyem) BST: [485]
		{SPECIES_CHANDELURE}, //609 (Chandelure) BST: [520]
		{SPECIES_HAXORUS}, //612 (Haxorus) BST: [540]
		{SPECIES_BEARTIC}, //614 (Beartic) BST: [525]
		{SPECIES_CRYOGONAL}, //615 (Cryogonal) BST: [515]
		{SPECIES_ACCELGOR}, //617 (Accelgor) BST: [495]
		{SPECIES_STUNFISK}, //618 (Stunfisk) BST: [471]
		{SPECIES_MIENSHAO}, //620 (Mienshao) BST: [510]
		{SPECIES_DRUDDIGON}, //621 (Druddigon) BST: [505]
		{SPECIES_GOLURK}, //623 (Golurk) BST: [513]
		{SPECIES_BISHARP}, //625 (Bisharp) BST: [490]
		{SPECIES_BOUFFALANT}, //626 (Bouffalant) BST: [490]
		{SPECIES_BRAVIARY}, //628 (Braviary) BST: [510]
		{SPECIES_MANDIBUZZ}, //630 (Mandibuzz) BST: [510]
		{SPECIES_HEATMOR}, //631 (Heatmor) BST: [524]
		{SPECIES_DURANT}, //632 (Durant) BST: [514]
		{SPECIES_HYDREIGON}, //635 (Hydreigon) BST: [600]
		{SPECIES_VOLCARONA}, //637 (Volcarona) BST: [550]
		{SPECIES_COBALION}, //638 (Cobalion) BST: [580]
		{SPECIES_TERRAKION}, //639 (Terrakion) BST: [580]
		{SPECIES_VIRIZION}, //640 (Virizion) BST: [580]

		{SPECIES_TORNADUS}, //641 (Tornadus) BST: [580]
		//  {SPECIES_TORNADUS_THERIAN}, //1074 (Tornadus) BST: [580]

		{SPECIES_THUNDURUS}, //642 (Thundurus) BST: [580]
		//  {SPECIES_THUNDURUS_THERIAN}, //1075 (Thundurus) BST: [580]

		{SPECIES_LANDORUS}, //645 (Landorus) BST: [600]
		//  {SPECIES_LANDORUS_THERIAN}, //1076 (Landorus) BST: [600]

		{SPECIES_KELDEO}, //647 (Keldeo) BST: [580]
		//  {SPECIES_KELDEO_RESOLUTE}, //1079 (Keldeo) BST: [580]

		//Gen 6 Start
		{SPECIES_CHESNAUGHT}, //652 (Chesnaught) BST: [530]
		{SPECIES_DELPHOX}, //655 (Delphox) BST: [534]
		{SPECIES_GRENINJA}, //658 (Greninja) BST: [530]
		{SPECIES_DIGGERSBY}, //660 (Diggersby) BST: [423]
		{SPECIES_TALONFLAME}, //663 (Talonflame) BST: [499]

		{SPECIES_VIVILLON}, //666 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_POLAR}, //1087 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_TUNDRA}, //1088 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_CONTINENTAL}, //1089 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_GARDEN}, //1090 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_ELEGANT}, //1091 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_MEADOW}, //1092 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_MODERN}, //1093 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_MARINE}, //1094 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_ARCHIPELAGO}, //1095 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_HIGH_PLAINS}, //1096 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_SANDSTORM}, //1097 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_RIVER}, //1098 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_MONSOON}, //1099 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_SAVANNA}, //1100 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_SUN}, //1101 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_OCEAN}, //1102 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_JUNGLE}, //1103 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_FANCY}, //1104 (Vivillon) BST: [411]
		//  {SPECIES_VIVILLON_POKE_BALL}, //1105 (Vivillon) BST: [411]

		{SPECIES_PYROAR}, //668 (Pyroar) BST: [527]
		{SPECIES_FLOETTE_ETERNAL_FLOWER}, //1114 (Floette) BST: [551]

		{SPECIES_FLORGES}, //671 (Florges) BST: [552]
		//  {SPECIES_FLORGES_YELLOW_FLOWER}, //1115 (Florges) BST: [552]
		//  {SPECIES_FLORGES_ORANGE_FLOWER}, //1116 (Florges) BST: [552]
		//  {SPECIES_FLORGES_BLUE_FLOWER}, //1117 (Florges) BST: [552]
		//  {SPECIES_FLORGES_WHITE_FLOWER}, //1118 (Florges) BST: [552]

		{SPECIES_GOGOAT}, //673 (Gogoat) BST: [540]
		{SPECIES_PANGORO}, //675 (Pangoro) BST: [495]

		{SPECIES_FURFROU}, //676 (Furfrou) BST: [472]
		//  {SPECIES_FURFROU_HEART_TRIM}, //1119 (Furfrou) BST: [472]
		//  {SPECIES_FURFROU_STAR_TRIM}, //1120 (Furfrou) BST: [472]
		//  {SPECIES_FURFROU_DIAMOND_TRIM}, //1121 (Furfrou) BST: [472]
		//  {SPECIES_FURFROU_DEBUTANTE_TRIM}, //1122 (Furfrou) BST: [472]
		//  {SPECIES_FURFROU_MATRON_TRIM}, //1123 (Furfrou) BST: [472]
		//  {SPECIES_FURFROU_DANDY_TRIM}, //1124 (Furfrou) BST: [472]
		//  {SPECIES_FURFROU_LA_REINE_TRIM}, //1125 (Furfrou) BST: [472]
		//  {SPECIES_FURFROU_KABUKI_TRIM}, //1126 (Furfrou) BST: [472]
		//  {SPECIES_FURFROU_PHARAOH_TRIM}, //1127 (Furfrou) BST: [472]

		{SPECIES_MEOWSTIC}, //678 (Meowstic) BST: [506]
		{SPECIES_AEGISLASH_BLADE}, //681 (Aegislash) BST: [500]
		{SPECIES_AROMATISSE}, //683 (Aromatisse) BST: [462]
		{SPECIES_SLURPUFF}, //685 (Slurpuff) BST: [480]
		{SPECIES_MALAMAR}, //687 (Malamar) BST: [482]
		{SPECIES_BARBARACLE}, //689 (Barbaracle) BST: [500]
		{SPECIES_DRAGALGE}, //691 (Dragalge) BST: [494]
		{SPECIES_CLAWITZER}, //693 (Clawitzer) BST: [520]
		{SPECIES_HELIOLISK}, //695 (Heliolisk) BST: [491]
		{SPECIES_TYRANTRUM}, //697 (Tyrantrum) BST: [541]
		{SPECIES_AURORUS}, //699 (Aurorus) BST: [541]
		{SPECIES_SYLVEON}, //700 (Sylveon) BST: [525]
		{SPECIES_HAWLUCHA}, //701 (Hawlucha) BST: [500]
		{SPECIES_DEDENNE}, //702 (Dedenne) BST: [471]
		{SPECIES_CARBINK}, //703 (Carbink) BST: [500]
		{SPECIES_GOODRA}, //706 (Goodra) BST: [600]
		{SPECIES_KLEFKI}, //707 (Klefki) BST: [470]
		{SPECIES_TREVENANT}, //709 (Trevenant) BST: [494]

		{SPECIES_GOURGEIST}, //711 (Gourgeist) BST: [494]
		//  {SPECIES_GOURGEIST_SMALL}, //1133 (Gourgeist) BST: [494]
		//  {SPECIES_GOURGEIST_LARGE}, //1134 (Gourgeist) BST: [494]
		//  {SPECIES_GOURGEIST_SUPER}, //1135 (Gourgeist) BST: [494]

		{SPECIES_AVALUGG}, //713 (Avalugg) BST: [514]
		{SPECIES_NOIVERN}, //715 (Noivern) BST: [535]
		//Gen 7 start
		{SPECIES_DECIDUEYE}, //724 (Decidueye) BST: [530]
		{SPECIES_INCINEROAR}, //727 (Incineroar) BST: [530]
		{SPECIES_PRIMARINA}, //730 (Primarina) BST: [530]
		{SPECIES_TOUCANNON}, //733 (Toucannon) BST: [525]
		{SPECIES_GUMSHOOS}, //735 (Gumshoos) BST: [458]
		{SPECIES_VIKAVOLT}, //738 (Vikavolt) BST: [600]
		{SPECIES_CRABOMINABLE}, //740 (Crabminabl) BST: [518]

		{SPECIES_ORICORIO}, //741 (Oricorio) BST: [496]
		//  {SPECIES_ORICORIO_POM_POM}, //1142 (Oricorio) BST: [496]
		//  {SPECIES_ORICORIO_PAU}, //1143 (Oricorio) BST: [496]
		//  {SPECIES_ORICORIO_SENSU}, //1144 (Oricorio) BST: [496]

		{SPECIES_RIBOMBEE}, //743 (Ribombee) BST: [464]

		{SPECIES_LYCANROC}, //745 (Lycanroc) BST: [507]
		//  {SPECIES_LYCANROC_MIDNIGHT}, //1146 (Lycanroc) BST: [507]
		//  {SPECIES_LYCANROC_DUSK}, //1147 (Lycanroc) BST: [507]

		{SPECIES_TOXAPEX}, //748 (Toxapex) BST: [495]
		{SPECIES_MUDSDALE}, //750 (Mudsdale) BST: [500]
		{SPECIES_ARAQUANID}, //752 (Araquanid) BST: [474]
		{SPECIES_LURANTIS}, //754 (Lurantis) BST: [510]
		{SPECIES_SHIINOTIC}, //756 (Shiinotic) BST: [445]
		{SPECIES_SALAZZLE}, //758 (Salazzle) BST: [480]
		{SPECIES_BEWEAR}, //760 (Bewear) BST: [500]
		{SPECIES_TSAREENA}, //763 (Tsareena) BST: [530]
		{SPECIES_COMFEY}, //764 (Comfey) BST: [485]
		{SPECIES_ORANGURU}, //765 (Oranguru) BST: [490]
		{SPECIES_PASSIMIAN}, //766 (Passimian) BST: [490]
		{SPECIES_GOLISOPOD}, //768 (Golisopod) BST: [530]
		{SPECIES_PALOSSAND}, //770 (Palossand) BST: [500]
		{SPECIES_PYUKUMUKU}, //771 (Pyukumuku) BST: [410]
		{SPECIES_SILVALLY}, //773 (Silvally) BST: [600]
		{SPECIES_MINIOR}, //774 (Minior) BST: [440]
		{SPECIES_KOMALA}, //775 (Komala) BST: [510]
		{SPECIES_TURTONATOR}, //776 (Turtonator) BST: [505]
		{SPECIES_TOGEDEMARU}, //777 (Togedemaru) BST: [475]
		{SPECIES_MIMIKYU}, //778 (Mimikyu) BST: [476]
		{SPECIES_BRUXISH}, //779 (Bruxish) BST: [475]
		{SPECIES_DRAMPA}, //780 (Drampa) BST: [485]
		{SPECIES_DHELMISE}, //781 (Dhelmise) BST: [527]
		{SPECIES_KOMMO_O}, //784 (Kommo-o) BST: [600]
		{SPECIES_TAPU_KOKO}, //785 (Tapu Koko) BST: [570]
		{SPECIES_TAPU_LELE}, //786 (Tapu Lele) BST: [570]
		{SPECIES_TAPU_BULU}, //787 (Tapu Bulu) BST: [570]
		{SPECIES_TAPU_FINI}, //788 (Tapu Fini) BST: [570]
		//Gen 8 start
		{SPECIES_RILLABOOM}, //812 (Rillaboom) BST: [530]
		{SPECIES_CINDERACE}, //815 (Cinderace) BST: [530]
		{SPECIES_INTELEON}, //818 (Inteleon) BST: [530]
		{SPECIES_GREEDENT}, //820 (Greedent) BST: [460]
		{SPECIES_CORVIKNIGHT}, //823 (Corviknigh) BST: [495]
		{SPECIES_THIEVUL}, //828 (Thievul) BST: [455]
		{SPECIES_DUBWOOL}, //832 (Dubwool) BST: [490]
		{SPECIES_DREDNAW}, //834 (Drednaw) BST: [485]
		{SPECIES_BOLTUND}, //836 (Boltund) BST: [490]
		{SPECIES_FLAPPLE}, //841 (Flapple) BST: [485]
		{SPECIES_APPLETUN}, //842 (Appletun) BST: [485]

		{SPECIES_TOXTRICITY}, //849 (Toxtricity) BST: [502]
		//	{SPECIES_TOXTRICITY_LOW_KEY}, //1186 (Toxtricity) BST: [502]

		{SPECIES_CENTISKORCH}, //851 (Centiskorc) BST: [525]
		{SPECIES_OBSTAGOON}, //862 (Obstagoon) BST: [520]
		{SPECIES_PERRSERKER}, //863 (Perrserker) BST: [440]
		{SPECIES_CURSOLA}, //864 (Cursola) BST: [510]
		{SPECIES_SIRFETCHD}, //865 (Sirfetch’d) BST: [507]
		{SPECIES_MR_RIME}, //866 (Mr. Rime) BST: [520]
		{SPECIES_FALINKS}, //870 (Falinks) BST: [470]
		{SPECIES_PINCURCHIN}, //871 (Pincurchin) BST: [435]
		{SPECIES_FROSMOTH}, //873 (Frosmoth) BST: [475]
		{SPECIES_DURALUDON}, //884 (Duraludon) BST: [535]
		{SPECIES_DRAGAPULT}, //887 (Dragapult) BST: [600]
		{SPECIES_REGIELEKI}, //894 (Regieleki) BST: [580]
		{SPECIES_REGIDRAGO}, //895 (Regidrago) BST: [580]
		//Alolan Forms
		{SPECIES_RATICATE_ALOLAN}, //950 (Raticate) BST: [438]
		{SPECIES_RAICHU_ALOLAN}, //951 (Raichu) BST: [490]
		{SPECIES_SANDSLASH_ALOLAN}, //953 (Sandslash) BST: [480]
		{SPECIES_NINETALES_ALOLAN}, //955 (Ninetales) BST: [515]
		{SPECIES_DUGTRIO_ALOLAN}, //957 (Dugtrio) BST: [425]
		{SPECIES_PERSIAN_ALOLAN}, //959 (Persian) BST: [480]
		{SPECIES_GOLEM_ALOLAN}, //962 (Golem) BST: [520]
		{SPECIES_MUK_ALOLAN}, //964 (Muk) BST: [500]
		{SPECIES_EXEGGUTOR_ALOLAN}, //965 (Exeggutor) BST: [530]
		{SPECIES_MAROWAK_ALOLAN}, //966 (Marowak) BST: [440]
		//Galar Forms
		{SPECIES_RAPIDASH_GALARIAN}, //969 (Rapidash) BST: [500]
		{SPECIES_SLOWBRO_GALARIAN}, //971 (Slowbro) BST: [490]
		{SPECIES_WEEZING_GALARIAN}, //973 (Weezing) BST: [490]
		{SPECIES_ARTICUNO_GALARIAN}, //975 (Articuno) BST: [580]
		{SPECIES_ZAPDOS_GALARIAN}, //976 (Zapdos) BST: [580]
		{SPECIES_MOLTRES_GALARIAN}, //977 (Moltres) BST: [580]
		{SPECIES_SLOWKING_GALARIAN}, //978 (Slowking) BST: [490]
		{SPECIES_DARMANITAN_GALARIAN}, //983 (Darmanitan) BST: [480]
		{SPECIES_STUNFISK_GALARIAN} //985 (Stunfisk) BST: [471]
	};
	
	
	static const u16 OldplayerMegaSpeciesArray [][1] =
	{
		{SPECIES_MEW}, //151 (Mew) BST: [600]
		{SPECIES_MEWTWO}, //150 (Mewtwo) BST: [680]
		//  {SPECIES_MEWTWO_MEGA_X}, //912 (Mewtwo) BST: [780]
		//  {SPECIES_MEWTWO_MEGA_Y}, //913 (Mewtwo) BST: [780]
		{SPECIES_LUGIA}, //249 (Lugia) BST: [680]
		{SPECIES_HO_OH}, //250 (Ho-Oh) BST: [680]
		{SPECIES_CELEBI}, //251 (Celebi) BST: [600]
		{SPECIES_LATIAS}, //380 (Latias) BST: [600]
		//  {SPECIES_LATIAS_MEGA}, //937 (Latias) BST: [700]
		{SPECIES_LATIOS}, //381 (Latios) BST: [600]
		//  {SPECIES_LATIOS_MEGA}, //938 (Latios) BST: [700]
		{SPECIES_GROUDON}, //not final evo
		//  {SPECIES_GROUDON_PRIMAL}, //948 (Groudon) BST: [770]
		{SPECIES_KYOGRE}, //not final evo
		//  {SPECIES_KYOGRE_PRIMAL}, //947 (Kyogre) BST: [770]
		{SPECIES_RAYQUAZA}, //not final evo
		//  {SPECIES_RAYQUAZA_MEGA}, //946 (Rayquaza) BST: [780]
		{SPECIES_JIRACHI}, //385 (Jirachi) BST: [600]
		{SPECIES_DEOXYS}, //386 (Deoxys) BST: [600]
	    	//  {SPECIES_DEOXYS_ATTACK}, //1031 (Deoxys) BST: [600]
		//  {SPECIES_DEOXYS_DEFENSE}, //1032 (Deoxys) BST: [600]
		//  {SPECIES_DEOXYS_SPEED}, //1033 (Deoxys) BST: [600]
		{SPECIES_DIALGA}, //483 (Dialga) BST: [680]
		{SPECIES_PALKIA}, //484 (Palkia) BST: [680]
		{SPECIES_GIRATINA}, //487 (Giratina) BST: [680]
		//  {SPECIES_GIRATINA_ORIGIN}, //1046 (Giratina) BST: [680]
		{SPECIES_MANAPHY}, //490 (Manaphy) BST: [600]
		{SPECIES_DARKRAI}, //491 (Darkrai) BST: [600]
		{SPECIES_SHAYMIN}, //492 (Shaymin) BST: [600]
		//  {SPECIES_SHAYMIN_SKY}, //1047 (Shaymin) BST: [600]
		{SPECIES_ARCEUS}, //493 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_FIGHTING}, //1048 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_FLYING}, //1049 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_POISON}, //1050 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_GROUND}, //1051 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_ROCK}, //1052 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_BUG}, //1053 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_GHOST}, //1054 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_STEEL}, //1055 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_FIRE}, //1056 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_WATER}, //1057 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_GRASS}, //1058 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_ELECTRIC}, //1059 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_PSYCHIC}, //1060 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_ICE}, //1061 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_DRAGON}, //1062 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_DARK}, //1063 (Arceus) BST: [720]
		//  {SPECIES_ARCEUS_FAIRY}, //1064 (Arceus) BST: [720]	
		{SPECIES_VICTINI}, //494 (Victini) BST: [600]
		{SPECIES_RESHIRAM}, //643 (Reshiram) BST: [680]
		{SPECIES_ZEKROM}, //644 (Zekrom) BST: [680]
		{SPECIES_KYUREM}, //646 (Kyurem) BST: [660]
		//  {SPECIES_KYUREM_WHITE}, //1077 (Kyurem) BST: [700]
		//  {SPECIES_KYUREM_BLACK}, //1078 (Kyurem) BST: [700]	
		{SPECIES_MELOETTA}, //648 (Meloetta) BST: [600]
		//  {SPECIES_MELOETTA_PIROUETTE}, //1080 (Meloetta) BST: [600]
		{SPECIES_GENESECT}, //649 (Genesect) BST: [600]
		//  {SPECIES_GENESECT_DOUSE_DRIVE}, //1081 (Genesect) BST: [600]
		//  {SPECIES_GENESECT_SHOCK_DRIVE}, //1082 (Genesect) BST: [600]
		//  {SPECIES_GENESECT_BURN_DRIVE}, //1083 (Genesect) BST: [600]
		//  {SPECIES_GENESECT_CHILL_DRIVE}, //1084 (Genesect) BST: [600]
		{SPECIES_XERNEAS}, //716 (Xerneas) BST: [680]
		{SPECIES_YVELTAL}, //717 (Yveltal) BST: [680]
		{SPECIES_ZYGARDE}, //718 (Zygarde) BST: [600]
		//  {SPECIES_ZYGARDE_COMPLETE}, //1140 (Zygarde) BST: [708]
		{SPECIES_DIANCIE}, //719 (Diancie) BST: [600]
		//  {SPECIES_DIANCIE_MEGA}, //945 (Diancie) BST: [700]
		{SPECIES_HOOPA}, //720 (Hoopa) BST: [600]
		//  {SPECIES_HOOPA_UNBOUND}, //1141 (Hoopa) BST: [680]
		{SPECIES_VOLCANION}, //721 (Volcanion) BST: [600]
		{SPECIES_SOLGALEO}, //791 (Solgaleo) BST: [680]
		{SPECIES_LUNALA}, //792 (Lunala) BST: [680]
		{SPECIES_NECROZMA}, //800 (Necrozma) BST: [600]
		//  {SPECIES_NECROZMA_DUSK_MANE}, //1180 (Necrozma) BST: [680]
		//  {SPECIES_NECROZMA_DAWN_WINGS}, //1181 (Necrozma) BST: [680]
		//  {SPECIES_NECROZMA_ULTRA}, //1182 (Necrozma) BST: [754]
		{SPECIES_MAGEARNA}, //801 (Magearna) BST: [600]
		//  {SPECIES_MAGEARNA_ORIGINAL_COLOR}, //1183 (Magearna) BST: [600]
		{SPECIES_MARSHADOW}, //802 (Marshadow) BST: [600]
		{SPECIES_ZERAORA}, //807 (Zeraora) BST: [600]
		{SPECIES_MELMETAL}, //809 (Melmetal) BST: [600]

		//Megas
		{SPECIES_VENUSAUR_MEGA}, //899 (Venusaur) BST: [625]
		{SPECIES_CHARIZARD_MEGA_X}, //900 (Charizard) BST: [634]
		{SPECIES_CHARIZARD_MEGA_Y}, //901 (Charizard) BST: [634]
		{SPECIES_BLASTOISE_MEGA}, //902 (Blastoise) BST: [630]
		{SPECIES_BEEDRILL_MEGA}, //903 (Beedrill) BST: [525]
		{SPECIES_PIDGEOT_MEGA}, //904 (Pidgeot) BST: [594]
		{SPECIES_ALAKAZAM_MEGA}, //905 (Alakazam) BST: [600]
		{SPECIES_SLOWBRO_MEGA}, //906 (Slowbro) BST: [590]
		{SPECIES_GENGAR_MEGA}, //907 (Gengar) BST: [600]
		{SPECIES_KANGASKHAN_MEGA}, //908 (Kangaskhan) BST: [590]
		{SPECIES_PINSIR_MEGA}, //909 (Pinsir) BST: [600]
		{SPECIES_GYARADOS_MEGA}, //910 (Gyarados) BST: [640]
		{SPECIES_AERODACTYL_MEGA}, //911 (Aerodactyl) BST: [615]
		{SPECIES_AMPHAROS_MEGA}, //914 (Ampharos) BST: [610]
		{SPECIES_STEELIX_MEGA}, //915 (Steelix) BST: [630]
		{SPECIES_SCIZOR_MEGA}, //916 (Scizor) BST: [600]
		{SPECIES_HERACROSS_MEGA}, //917 (Heracross) BST: [600]
		{SPECIES_HOUNDOOM_MEGA}, //918 (Houndoom) BST: [610]
		{SPECIES_TYRANITAR_MEGA}, //919 (Tyranitar) BST: [700]
		{SPECIES_SCEPTILE_MEGA}, //920 (Sceptile) BST: [630]
		{SPECIES_BLAZIKEN_MEGA}, //921 (Blaziken) BST: [630]
		{SPECIES_SWAMPERT_MEGA}, //922 (Swampert) BST: [635]
		{SPECIES_GARDEVOIR_MEGA}, //923 (Gardevoir) BST: [633]
		{SPECIES_SABLEYE_MEGA}, //924 (Sableye) BST: [510]
		{SPECIES_MAWILE_MEGA}, //925 (Mawile) BST: [510]
		{SPECIES_AGGRON_MEGA}, //926 (Aggron) BST: [630]
		{SPECIES_MEDICHAM_MEGA}, //927 (Medicham) BST: [510]
		{SPECIES_MANECTRIC_MEGA}, //928 (Manectric) BST: [595]
		{SPECIES_SHARPEDO_MEGA}, //929 (Sharpedo) BST: [580]
		{SPECIES_CAMERUPT_MEGA}, //930 (Camerupt) BST: [600]
		{SPECIES_ALTARIA_MEGA}, //931 (Altaria) BST: [610]
		{SPECIES_BANETTE_MEGA}, //932 (Banette) BST: [585]
		{SPECIES_ABSOL_MEGA}, //933 (Absol) BST: [585]
		{SPECIES_GLALIE_MEGA}, //934 (Glalie) BST: [600]
		{SPECIES_SALAMENCE_MEGA}, //935 (Salamence) BST: [700]
		{SPECIES_METAGROSS_MEGA}, //936 (Metagross) BST: [700]
		{SPECIES_LOPUNNY_MEGA}, //939 (Lopunny) BST: [600]
		{SPECIES_GARCHOMP_MEGA}, //940 (Garchomp) BST: [700]
		{SPECIES_LUCARIO_MEGA}, //941 (Lucario) BST: [625]
		{SPECIES_ABOMASNOW_MEGA}, //942 (Abomasnow) BST: [614]
		{SPECIES_GALLADE_MEGA}, //943 (Gallade) BST: [633]
		{SPECIES_AUDINO_MEGA}, //944 (Audino) BST: [585]
		{SPECIES_MILOTIC_MEGA}, //1207 (Milotic) BST: [640]
		{SPECIES_BUTTERFREE_MEGA}, //1208 (Butterfree) BST: [525]
		{SPECIES_MACHAMP_MEGA}, //1209 (Machamp) BST: [605]
		{SPECIES_KINGLER_MEGA}, //1210 (Kingler) BST: [600]
		{SPECIES_LAPRAS_MEGA}, //1211 (Lapras) BST: [645]
		{SPECIES_FLYGON_MEGA}, //1212 (Flygon) BST: [640]
		{SPECIES_KINGDRA_MEGA}, //1213 (Kingdra) BST: [640]
	};
	
	
	static const u16 OldplayerFavoriteSpeciesArray [][1] =
	{
		    //Starters
		{SPECIES_VENUSAUR}, //3 (Venusaur) BST: [525]
		{SPECIES_CHARIZARD}, //6 (Charizard) BST: [534]
		{SPECIES_BLASTOISE}, //9 (Blastoise) BST: [530]
		{SPECIES_MEGANIUM}, //154 (Meganium) BST: [535]
		{SPECIES_TYPHLOSION}, //157 (Typhlosion) BST: [534]
		{SPECIES_FERALIGATR}, //160 (Feraligatr) BST: [530]
		{SPECIES_SCEPTILE}, //254 (Sceptile) BST: [530]
		{SPECIES_BLAZIKEN}, //257 (Blaziken) BST: [530]
		{SPECIES_SWAMPERT}, //260 (Swampert) BST: [535]
		{SPECIES_TORTERRA}, //389 (Torterra) BST: [534]
		{SPECIES_INFERNAPE}, //392 (Infernape) BST: [534]
		{SPECIES_EMPOLEON}, //395 (Empoleon) BST: [530]
		{SPECIES_SERPERIOR}, //497 (Serperior) BST: [528]
		{SPECIES_EMBOAR}, //500 (Emboar) BST: [528]
		{SPECIES_SAMUROTT}, //503 (Samurott) BST: [528]
		{SPECIES_CHESNAUGHT}, //652 (Chesnaught) BST: [530]
		{SPECIES_DELPHOX}, //655 (Delphox) BST: [534]
		{SPECIES_GRENINJA}, //658 (Greninja) BST: [530]
		{SPECIES_DECIDUEYE}, //724 (Decidueye) BST: [530]
		{SPECIES_INCINEROAR}, //727 (Incineroar) BST: [530]
		{SPECIES_PRIMARINA}, //730 (Primarina) BST: [530]
		{SPECIES_RILLABOOM}, //812 (Rillaboom) BST: [530]
		{SPECIES_CINDERACE}, //815 (Cinderace) BST: [530]
		{SPECIES_INTELEON}, //818 (Inteleon) BST: [530]
		//Pseudos
		{SPECIES_DRAGONITE}, //149 (Dragonite) BST: [600]
		{SPECIES_TYRANITAR}, //248 (Tyranitar) BST: [600]
		{SPECIES_SALAMENCE}, //373 (Salamence) BST: [600]
		{SPECIES_METAGROSS}, //376 (Metagross) BST: [600]
		{SPECIES_GARCHOMP}, //445 (Garchomp) BST: [600]
		{SPECIES_HYDREIGON}, //635 (Hydreigon) BST: [600]
		{SPECIES_GOODRA}, //706 (Goodra) BST: [600]
		{SPECIES_KOMMO_O}, //784 (Kommo-o) BST: [600]
		{SPECIES_DRAGAPULT}, //887 (Dragapult) BST: [600]
		//Pikachu Clones:
		{SPECIES_PACHIRISU}, //417 (Pachirisu) BST: [455]
		{SPECIES_PACHIRISU}, //417 (Pachirisu) BST: [455]
		//  {SPECIES_EMOLGA}, //587 (Emolga) BST: [468]
		//  {SPECIES_DEDENNE}, //702 (Dedenne) BST: [471]
		//  {SPECIES_TOGEDEMARU}, //777 (Togedemaru) BST: [475]
		//Gen 1 Pokemon:
		{SPECIES_PIKACHU}, //25 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_COSPLAY}, //986 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_ROCK_STAR}, //987 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_BELLE}, //988 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_POP_STAR}, //989 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_PH_D}, //990 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_LIBRE}, //991 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_ORIGINAL_CAP}, //992 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_HOENN_CAP}, //993 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_SINNOH_CAP}, //994 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_UNOVA_CAP}, //995 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_KALOS_CAP}, //996 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_ALOLA_CAP}, //997 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_PARTNER_CAP}, //998 (Pikachu) BST: [320]
		//  {SPECIES_PIKACHU_WORLD_CAP}, //999 (Pikachu) BST: [320]

		{SPECIES_RAICHU}, //26 (Raichu) BST: [490]
		//  {SPECIES_RAICHU_ALOLAN}, //951 (Raichu) BST: [490]

		{SPECIES_ARCANINE}, //59 (Arcanine) BST: [555]
		{SPECIES_GOLEM}, //76 (Golem) BST: [520]
		{SPECIES_GENGAR}, //94 (Gengar) BST: [500]
	   	{SPECIES_STARMIE}, //121 (Starmie) BST: [520]
		{SPECIES_GYARADOS}, //130 (Gyarados) BST: [540]

		{SPECIES_VAPOREON}, //134 (Vaporeon) BST: [525]
		{SPECIES_VAPOREON}, //134 (Vaporeon) BST: [525]
		//  {SPECIES_JOLTEON}, //135 (Jolteon) BST: [525]
		//  {SPECIES_FLAREON}, //136 (Flareon) BST: [525]
		//  {SPECIES_ESPEON}, //196 (Espeon) BST: [525]
		//  {SPECIES_UMBREON}, //197 (Umbreon) BST: [525]
		//  {SPECIES_LEAFEON}, //470 (Leafeon) BST: [525]
		//  {SPECIES_GLACEON}, //471 (Glaceon) BST: [525]
		//  {SPECIES_SYLVEON}, //700 (Sylveon) BST: [525]

		{SPECIES_SNORLAX}, //143 (Snorlax) BST: [540]

		{SPECIES_ARTICUNO}, //144 (Articuno) BST: [580]
		// {SPECIES_ZAPDOS}, //145 (Zapdos) BST: [580]
		// {SPECIES_MOLTRES}, //146 (Moltres) BST: [580]

		{SPECIES_ARTICUNO_GALARIAN}, //975 (Articuno) BST: [580]
		//  {SPECIES_ZAPDOS_GALARIAN}, //976 (Zapdos) BST: [580]
		//  {SPECIES_MOLTRES_GALARIAN}, //977 (Moltres) BST: [580]

		//Gen 2 Pokemon:
		{SPECIES_CROBAT}, //169 (Crobat) BST: [535]
		{SPECIES_SCIZOR}, //212 (Scizor) BST: [500]
		{SPECIES_SKARMORY}, //227 (Skarmory) BST: [465]
		//Gen 3 Pokemon:
		{SPECIES_MIGHTYENA}, //262 (Mightyena) BST: [460]
		{SPECIES_AGGRON}, //306 (Aggron) BST: [530]
		{SPECIES_FLYGON}, //330 (Flygon) BST: [540]
		{SPECIES_MILOTIC}, //350 (Milotic) BST: [540]
		{SPECIES_DUSCLOPS}, //356 (Dusclops) BST: [???]

	    	{SPECIES_REGIROCK}, //377 (Regirock) BST: [580]
		{SPECIES_REGIROCK}, //377 (Regirock) BST: [580]
		//  {SPECIES_REGICE}, //378 (Regice) BST: [580]
		//  {SPECIES_REGISTEEL}, //379 (Registeel) BST: [580]
		//  {SPECIES_REGIELEKI}, //894 (Regieleki) BST: [580]
		//  {SPECIES_REGIDRAGO}, //895 (Regidrago) BST: [580]

		{SPECIES_RAYQUAZA}, //not final evo
		//Gen 4 Pokemon:
		{SPECIES_STARAPTOR}, //398 (Staraptor) BST: [485]
		{SPECIES_BIBAREL}, //400 (Bibarel) BST: [440]
		{SPECIES_LUXRAY}, //405 (Luxray) BST: [533]
		{SPECIES_ROSERADE}, //407 (Roserade) BST: [515]
		{SPECIES_GASTRODON_EAST_SEA}, //1040 (Gastrodon) BST: [475]
		{SPECIES_AMBIPOM}, //424 (Ambipom) BST: [492]
		{SPECIES_LUCARIO}, //448 (Lucario) BST: [525]
		{SPECIES_WEAVILE}, //461 (Weavile) BST: [510]
		{SPECIES_RHYPERIOR}, //464 (Rhyperior) BST: [535]
		{SPECIES_ELECTIVIRE}, //466 (Electivire) BST: [540]
		{SPECIES_MAGMORTAR}, //467 (Magmortar) BST: [540]
		{SPECIES_TOGEKISS}, //468 (Togekiss) BST: [545]
		{SPECIES_GLISCOR}, //472 (Gliscor) BST: [510]

		{SPECIES_ROTOM}, //479 (Rotom) BST: [440]
		//  {SPECIES_ROTOM_HEAT}, //1041 (Rotom) BST: [520]
		//  {SPECIES_ROTOM_WASH}, //1042 (Rotom) BST: [520]
		//  {SPECIES_ROTOM_FROST}, //1043 (Rotom) BST: [520]
		//  {SPECIES_ROTOM_FAN}, //1044 (Rotom) BST: [520]
		//  {SPECIES_ROTOM_MOW}, //1045 (Rotom) BST: [520]

		{SPECIES_UXIE}, //480 (Uxie) BST: [580]
		//  {SPECIES_AZELF}, //482 (Azelf) BST: [580]

		{SPECIES_CRESSELIA}, //488 (Cresselia) BST: [600]
		{SPECIES_SHAYMIN}, //492 (Shaymin) BST: [600]
		{SPECIES_MANAPHY}, //490 (Manaphy) BST: [600]
		//Gen 5 Pokemon:
		{SPECIES_EXCADRILL}, //530 (Excadrill) BST: [508]
		{SPECIES_ZOROARK}, //571 (Zoroark) BST: [510]
		{SPECIES_CHANDELURE}, //609 (Chandelure) BST: [520]
		{SPECIES_VOLCARONA}, //637 (Volcarona) BST: [550]
		//Gen 6 Pokemon:
		{SPECIES_TALONFLAME}, //663 (Talonflame) BST: [499]
		{SPECIES_FLOETTE_ETERNAL_FLOWER}, //1114 (Floette) BST: [551]
		{SPECIES_NOIVERN}, //715 (Noivern) BST: [535]
		//Gen 7 Pokemon:
		{SPECIES_LYCANROC_MIDNIGHT}, //1146 (Lycanroc) BST: [507]
		{SPECIES_MIMIKYU}, //778 (Mimikyu) BST: [476]
		//Gen 8 Pokemon:
		{SPECIES_CORVIKNIGHT}, //823 (Corviknigh) BST: [495]
		{SPECIES_FLAPPLE}, //841 (Flapple) BST: [485]
	};
	
	
	
	
	
	
	u16 countArrayGeneral = ARRAY_COUNT(OldplayerGeneralSpeciesArray);
	u16 countArrayMega = ARRAY_COUNT(OldplayerMegaSpeciesArray);
	u16 countArrayFavorite = ARRAY_COUNT(OldplayerFavoriteSpeciesArray);
	u16 rand = 0;
	u16 species = 0;
	u8 countAltForms = 0;
	
	if (i < 4)
	{
		rand = Random() % countArrayGeneral;
		species = OldplayerGeneralSpeciesArray[rand][0];
	}
	else if (i == 5)
	{
		rand = Random() % countArrayMega;
		species = OldplayerMegaSpeciesArray[rand][0];
	}
	else
	{
		rand = Random() % countArrayFavorite;
		species = OldplayerFavoriteSpeciesArray[rand][0];
	}
	
	
	
	if (species == SPECIES_PACHIRISU && i == 5)
	{
		rand = Random() % 4;
		if (rand == 1)
			species = SPECIES_EMOLGA;
		else if (rand == 2)
			species = SPECIES_DEDENNE;
		else if (rand == 3)
			species = SPECIES_TOGEDEMARU;
	}
	if (species == SPECIES_REGIROCK && i == 5)
	{
		rand = Random() % 5;
		if (rand == 1)
			species = SPECIES_REGICE;
		else if (rand == 2)
			species = SPECIES_REGISTEEL;
		else if (rand == 3)
			species = SPECIES_REGIELEKI;
		else if (rand == 4)
			species = SPECIES_REGIDRAGO;
	}
	if (species == SPECIES_VAPOREON && i == 5)
	{
		rand = Random() % 8;
		if (rand == 1)
			species = SPECIES_JOLTEON;
		else if (rand == 2)
			species = SPECIES_FLAREON;
		else if (rand == 3)
			species = SPECIES_ESPEON;
		else if (rand == 4)
			species = SPECIES_UMBREON;
		else if (rand == 5)
			species = SPECIES_LEAFEON;
		else if (rand == 6)
			species = SPECIES_GLACEON;
		else if (rand == 7)
			species = SPECIES_SYLVEON;
	}
	if (species == SPECIES_ARTICUNO && i == 5)
	{
		rand = Random() % 3;
		if (rand == 1)
			species = SPECIES_ZAPDOS;
		else if (rand == 2)
			species = SPECIES_MOLTRES;
	}
	if (species == SPECIES_ARTICUNO_GALARIAN && i == 5)
	{
		rand = Random() % 3;
		if (rand == 1)
			species = SPECIES_ZAPDOS_GALARIAN;
		else if (rand == 2)
			species = SPECIES_MOLTRES_GALARIAN;
	}
	if (species == SPECIES_UXIE && i == 5)
		if (Random() % 2 == 0)
			species = SPECIES_AZELF;
	if (species == SPECIES_ZYGARDE)
		if (Random() % 2 == 0)
			species = SPECIES_ZYGARDE_COMPLETE;
	    
	    
	if (species == SPECIES_WORMADAM || species == SPECIES_GASTRODON || species == SPECIES_ROTOM || species == SPECIES_BASCULIN || species == SPECIES_SAWSBUCK || 
	   species == SPECIES_TORNADUS || species == SPECIES_THUNDURUS || species == SPECIES_LANDORUS || species == SPECIES_KELDEO || species == SPECIES_VIVILLON || 
	   species == SPECIES_FLORGES || species == SPECIES_FURFROU || species == SPECIES_GOURGEIST || species == SPECIES_ORICORIO || species == SPECIES_LYCANROC || 
	   species == SPECIES_TOXTRICITY || species == SPECIES_MEWTWO || species == SPECIES_LATIAS || species == SPECIES_LATIOS || species == SPECIES_GROUDON || 
	   species == SPECIES_KYOGRE || species == SPECIES_RAYQUAZA || species == SPECIES_DEOXYS || species == SPECIES_GIRATINA || species == SPECIES_SHAYMIN || 
	   species == SPECIES_ARCEUS || species == SPECIES_KYUREM || species == SPECIES_MELOETTA || species == SPECIES_GENESECT || species == SPECIES_DIANCIE || 
	   species == SPECIES_HOOPA || species == SPECIES_NECROZMA || species == SPECIES_PIKACHU || species == SPECIES_RAICHU || species == SPECIES_ROTOM)
	{
		species = getRandomFormSpeciesId(species);
	}
	return species;
};

