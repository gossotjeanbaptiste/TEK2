# Echoing Void — Wiki

A Fabric mod for Minecraft **26.1.2** (Fabric Loader 0.19.3+, Java 25+, requires Fabric API). Mod id: `echoing_void`, current version `1.0.0`.

The mod is themed around Minecraft Dungeons' *Echoing Void* DLC, transplanted into the End dimension: unique enemies (Watchling, Blastling), two signature legendary weapons (The Beginning and The End, Call of the Void), a Void Poisoned status effect, and a full beyond-netherite "Enderite" material/gear progression that generates in the End's outer islands.

---

## Table of Contents

1. [Blocks](#blocks)
2. [Items](#items)
3. [Mob Effect: Void Poisoned](#mob-effect-void-poisoned)
4. [Enchantment: Void Strike](#enchantment-void-strike)
5. [Fluid: Void Liquid](#fluid-void-liquid)
6. [Entities](#entities)
7. [Worldgen & Structures](#worldgen--structures) (incl. [Void Islands biome](#void-islands-biome))
8. [Progression Chain Summary](#progression-chain-summary)
9. [Loot Tables](#loot-tables)
10. [Tags Reference](#tags-reference)
11. [Client-Side Features](#client-side-features)
12. [Mixins Reference](#mixins-reference)
13. [Known Gaps / WIP Notes](#known-gaps--wip-notes)

---

## Blocks

| Block | Hardness / Blast Res. | Notes |
|---|---|---|
| **Void Stone** (`void_stone`) | 50.0 / 1200.0 | Requires a diamond+ pickaxe. Naturally obtainable by mining the [Void Islands biome](#void-islands-biome), or crafted (see below). Used as an ingredient in the End Brewing Stand and Enderite Upgrade Template recipes. |
| **End Brewing Stand** (`end_brewing_stand`) | 0.5 | Rarity RARE. A `BrewingStandBlock` subclass with its own block entity; functions as a normal brewing stand plus one exclusive recipe (see below). Naturally replaces any vanilla brewing stand generated inside End City structures. |
| **Enderite Debris** (`enderite_debris`) | 40.0 / 1200.0 | Rarity RARE. Ore block, generates in the outer End islands. Needs a netherite pickaxe or better (see [Tool Gating](#tool-gating)). |
| **Block of Enderite** (`enderite_block`) | 60.0 / 1600.0 | Storage block for 9 Enderite Ingots. Needs a netherite pickaxe or better. |
| **Void Liquid** (`void_liquid`) | 100.0 (fluid) | `LiquidBlock`, no `BlockItem` — bucket-only, like vanilla water/lava. Full behavior in [Fluid: Void Liquid](#fluid-void-liquid). |

### Tool Gating

Minecraft's post-1.21 tool system determines "correct tool" per block via tags, not a strict tier ranking. `enderite_debris` / `enderite_block` are added to `minecraft:needs_diamond_tool` (which every `incorrect_for_<tier>_tool` tag up to diamond references transitively) **and** explicitly to `minecraft:incorrect_for_diamond_tool`. Net effect: diamond and everything below can still swing at the block (at bare-hand speed, since `requiresCorrectToolForDrops()` is set) but only a netherite pickaxe (or the enderite material below) is "correct" — full speed and drops.

### Void Stone — crafting recipe

```
 O 
CEC
 O 
```
`O` = Obsidian, `C` = Crying Obsidian, `E` = End Stone → 1 Void Stone.

### End Brewing Stand — recipe to make one

```
 P
EBV
 P
```
`P` = Ender Pearl, `E` = End Stone, `B` = Brewing Stand, `V` = Void Stone → 1 End Brewing Stand.

---

## Items

### Beyond-Netherite Material Chain

| Item | Source | Notes |
|---|---|---|
| **Enderite Scrap** | Smelt/blast `enderite_debris` (200/100 ticks, 2.0 XP) | Rarity Uncommon, fire resistant. |
| **Enderite Ingot** | Shaped craft: 4× Diamond, 4× Enderite Scrap, 1× Netherite Ingot (center) | Rarity Rare, fire resistant. Also 9-ingot ↔ 1-block compacting/uncompacting recipes with `enderite_block`. |

### Enderite Upgrade Smithing Template

A real `SmithingTemplateItem` (not a generic item) — carries the vanilla-style "Applies to / Ingredients" tooltip and ghost-icon slot hints in the smithing table UI (reusing vanilla's generic tool/armor slot sprites; there is no vanilla "bow" sprite, so the bow upgrade isn't shown in the icon rotation even though the recipe itself works).

- **Obtained**: found rarely in End City chests (see [Loot Tables](#loot-tables)), or duplicated at a crafting table:
  ```
  NSN
  VEV
  NNN
  ```
  `N` = Netherite Ingot, `S` = existing template, `V` = Void Stone, `E` = End Stone → 2 templates (consumes 1 to make 2, net +1 — same mechanic as vanilla's Netherite Upgrade Template duplication).

### Enderite Gear (11 smithing-table upgrades)

All upgraded from their **netherite** equivalent (not diamond, unlike vanilla) at a smithing table using: Enderite Upgrade Template + the netherite item + 1 Enderite Ingot.

| Result | Base item | Notes |
|---|---|---|
| Enderite Pickaxe | `minecraft:netherite_pickaxe` | Same base attack numbers as netherite (1.0 dmg / -2.8 speed) |
| Enderite Axe | `minecraft:netherite_axe` | 5.0 dmg / -3.0 speed |
| Enderite Shovel | `minecraft:netherite_shovel` | 1.5 dmg / -3.0 speed |
| Enderite Hoe | `minecraft:netherite_hoe` | 0.0 dmg / -4.0 speed |
| Enderite Spear | `minecraft:netherite_spear` | Same 9 tuning figures as netherite_spear; distinct from the two legendary weapons below |
| Enderite Helmet/Chestplate/Leggings/Boots | matching `netherite_*` piece | See armor material below |
| **The Beginning and The End** | `minecraft:netherite_sword` | Legendary sword, see below |
| **Call of the Void** | `minecraft:bow` | Legendary bow — base is the plain vanilla bow since bows have no material tiers; the upgrade still works mechanically even though it's not a "typical" smithing pairing |

All improvement over netherite comes purely from the shared **`ModToolMaterials.ENDERITE`** tool material and **`ModArmorMaterials.ENDERITE`** armor material (no reinvented per-item numbers), exceeding real netherite stats:

| Stat | Netherite (vanilla) | Void-Touched / Enderite (this mod) |
|---|---|---|
| Tool durability | 2031 | 3000 |
| Tool attack dmg bonus | 4.0 | 5.0 |
| Tool enchantability | 15 | 20 |
| Armor durability multiplier | 37 | 48 |
| Armor defense (boots/legs/chest/helmet) | 3 / 6 / 8 / 3 | 4 / 7 / 9 / 4 |
| Armor toughness | 3.0 | 4.0 |
| Armor knockback resistance | 0.1 | 0.15 |

Repair material for the whole line (tools, armor, and the two legendary weapons) is the `echoing_void:enderite_repair` item tag, populated with just `enderite_ingot`.

### Legendary Weapons

Both are Epic rarity, carry an innate **Void Strike I** (not a curse — survives grindstone, since `GrindstoneMenuMixin` re-applies level 1 after any grind pass), and are repairable on an anvil with an Enderite Ingot.

- **The Beginning and The End** (`the_beginning_of_the_end`) — sword, `ModToolMaterials.ENDERITE` material, 3.0 dmg / **-1.6** speed (faster than a plain netherite/enderite sword's -2.4). Tagged into vanilla's `minecraft:swords` item tag. Landing 3 hits within 30 ticks plays a distinct "combo finisher" sound (`TheBeginningOfTheEndCombo`), mirroring the Dungeons dual-dagger 3-hit-then-spin identity. Has dedicated equip/pick-up/drop sounds.
- **Call of the Void** (`call_of_the_void`) — custom `CallOfTheVoidBowItem`, 900 durability, enchantability 15. A fully-charged shot (power ≥ 2.99) tags the arrow as a "charged shot" and plays a distinct charged-shot/impact sound pair; a normal shot plays the regular pair. Tagged into vanilla's `enchantable/bow` and `enchantable/durability` tags (so Mending/Unbreaking etc. apply normally, since it isn't natively a `BowItem` subtype recognized by those tags by default). Has its own equip/pick-up/drop sounds too.

### Void Poisoned Potions

- **Potion of Void Poisoned** (`void_poisoned_potion`) — **only** brewable in an **End Brewing Stand**: Awkward Potion + Ender Pearl (the ender pearl is otherwise not a valid ingredient in vanilla's `PotionBrewing` registry; a mixin whitelists it specifically for this block's ingredient slot).
- **Splash** / **Lingering** variants — convertible in *any* brewing stand, vanilla-style: + Gunpowder → Splash, + Dragon Breath → Lingering.
- Applies the [Void Poisoned effect](#mob-effect-void-poisoned) for 260 ticks (13s) by default.

### Spawn Eggs

`watchling_spawn_egg` / `blastling_spawn_egg` — real functional spawn eggs (creative/command only, not craftable), same as vanilla.

### Void Liquid Bucket

`void_liquid_bucket` — plain `BucketItem` for Void Liquid, fills/empties like any vanilla fluid bucket (craft-remainder: empty bucket). See [Fluid: Void Liquid](#fluid-void-liquid) for what the liquid itself does.

---

## Mob Effect: Void Poisoned

Harmful effect, color `#9C1FB0`. Ticks 3.0 damage (`damageSources().magic()`) every `20 >> amplifier` ticks (i.e. faster at higher amplifier). Plays an activate sound on application and a deactivate sound when it wears off (`LivingEntityMixin#onEffectsRemoved`).

**Immune**: anything in the `echoing_void:immune_to_void_poisoned` entity type tag — vanilla Enderman, Watchling, Blastling (all "of the void", so it doesn't poison itself or its own kind).

**Sources**: brewed [Void Poisoned potions](#void-poisoned-potions), or 3 continuous seconds submerged in [Void Liquid](#fluid-void-liquid).

---

## Enchantment: Void Strike

Data-driven enchantment (`data/echoing_void/enchantment/void_strike.json`), max level 3, mainhand slot only, weight 3, anvil cost 4. Applies a damage multiplier: ×1.33 / ×1.66 / ×2.0 by level. Supported items: anything in `echoing_void:enchantable/void_strike` (weapon + bow + crossbow enchantable tags). Explicitly added to `minecraft:in_enchanting_table` so it can roll at the enchanting table (custom enchantments don't show up there without this tag).

Obtainable: enchanting table/anvil books, or naturally on gear found in End City chests (an enchanted book pool and a weapon pool — diamond sword/axe/spear/bow — both added via `ModLootTables` using Fabric's `LootTableEvents.MODIFY`, stacking additively on top of the datapack-level JSON overrides on `end_city_treasure`).

---

## Fluid: Void Liquid

Dark purple, opaque hazard fluid (`void_liquid` source / `flowing_void_liquid` flowing) — own `VoidLiquidFluid`/`VoidLiquidBlock` pair, registered the same way vanilla registers water/lava (no `BlockItem`, world/bucket-only).

- **Physics**: added to `#minecraft:water` (see [Tags Reference](#tags-reference)) purely to inherit water's swim/slowdown movement, submersion detection, and breath handling for free, rather than reimplementing entity-in-fluid physics from scratch.
- **Poisoning**: after **3 continuous seconds submerged** (a per-entity tick counter inside `VoidLiquidFluid`), applies [Void Poisoned](#mob-effect-void-poisoned) for the default 260-tick duration. Respects the same `echoing_void:immune_to_void_poisoned` immunity as the potion.
- **Visuals**: opaque, pre-colored animated still/flow textures (no biome tint, unlike water) plus an overlay texture for submerged blocks — registered client-side via Fabric API's `FluidRenderingRegistry` (vanilla's fluid renderer is otherwise hardcoded to water/lava only). Emits sparse `portal` particles and drips as `dripping_obsidian_tear`.
- **Bucket**: [`void_liquid_bucket`](#void-liquid-bucket) — fills/empties like any vanilla fluid bucket.
- **Bottle**: filling a glass bottle from a Void Liquid source yields a **Potion of Void Poisoned**, not a water bottle. Vanilla's `BottleItem` hardcodes "any `#minecraft:water` source → water potion" (which void_liquid would otherwise trigger via the tag above), so `BottleItemMixin` intercepts before that check specifically when the source is Void Liquid.
- **No self-duplication**: unlike water, `canConvertToSource` always returns false — two adjacent Void Liquid sources never merge to create a third, so it can't be infinitely farmed.
- **Worldgen**: small surface pools in every outer End biome (not the central main island) — see [Void Liquid Pools](#void-liquid-pools).

---

## Entities

### Watchling (`watchling`)

Enderman-flavored monster. 30 HP, 0.3 movement speed, 4.0 attack damage, 0 armor.

- **Goals** (priority order): float → teleport-to-target (when target is out of melee range or line-of-sight is blocked) → melee attack → stare-at-distant-player → wander → look-at-player/random-look.
- **Attack**: 25% chance of a "heavy attack" (×1.65 damage) that also applies Void Poisoned; melee reach 2.5 blocks.
- **Teleport**: on taking damage, 50% chance to teleport-escape (like Enderman); scans downward for solid ground before landing, same portal-particle burst on both ends. Briefly hidden client-side (`teleportHidden`) right after teleporting.
- **Water**: sensitive to water — takes periodic damage while in it and immediately teleport-escapes on first contact.
- **Stare goal**: between 16–32 blocks from a player it can see (and has no combat target), it freezes and stares — a passive "being watched" moment rather than a threat.
- **Rendering**: custom model + glowing eyes layer with 3 blink stages (open / half / closed), hidden during the teleport-hide window (but *not* hidden by real Invisibility, Enderman-style).
- **Loot**: 1–3 ender pearls (weighted like vanilla Enderman).
- **Custom spawner** (`WatchlingSpawner`, not `BiomeModifications.addSpawn`): every 200 ticks, per non-spectator player in the End, attempts a group spawn (3–6 mobs) 28–48 blocks away, snapped to the real column surface height. This deliberately bypasses vanilla `NaturalSpawner`'s uniform-random-Y-then-search approach, which is heavily biased toward low altitude on the End's floating islands (most of that Y range is void under a high island).

### Blastling (`blastling`)

Ranged-only monster, no melee. 16 HP, 0.3 movement speed, 0 armor.

- **Goals**: float → teleport-away (if target closes within 6 blocks) → ranged attack (8–16 tick interval, 16-block radius) → wander → look-at-player/random-look.
- **Ranged attack**: fires a `BlastlingGooEntity` — a flat, non-arcing "spit" (no gravity) aimed directly at the target's predicted position, unlike vanilla's lobbed throwables.
- **Teleport**: always teleports away on taking damage (no % roll, unlike Watchling); same ground-scan/particle/sound treatment.
- **Rendering**: custom model plus a separate animated flame layer (32-frame `flame_01.png`…`flame_32.png` sequence) rendered as an independent second model rather than baked into the main geometry.
- **Loot**: 1–3 ender pearls (same weights as Watchling).
- **Custom spawner** (`BlastlingSpawner`): identical altitude-fix approach to `WatchlingSpawner`, smaller groups (1–3).

### Blastling Goo (`blastling_goo`, projectile)

`ThrowableItemProjectile`, no gravity, 2.0 damage, capped at 60 ticks of life (since a no-gravity miss would otherwise coast forever under the 0.99/tick inertia decay). 1/3 chance to apply 3s of Void Poisoned on a living-entity hit. Bursts into portal particles and a sound on any hit, then discards. Renders client-side as a full-bright flat billboard (like a snowball/pearl), not a physical projectile.

---

## Worldgen & Structures

### Void Islands Biome

`echoing_void:void_islands` — a custom End biome made entirely of Void Stone: tight clusters of small floating islands meant for parkour.

- **Registration**: added via Fabric API's `TheEndBiomes.addSmallIslandsBiome(VOID_ISLANDS, 20.0)` (`ModWorldGen`) — the official Fabric API hook for adding custom biomes into `TheEndBiomeSource`'s outer-island slot, weighted against vanilla Small End Islands' own baseline weight of `1.0`. At weight 20 the new biome generates roughly 95% of the time in that slot; vanilla Small End Islands can still rarely appear, which keeps the "Adventuring Time" advancement (requires visiting `minecraft:small_end_islands` specifically) completable. An early draft of this used a hand-written mixin redirecting `TheEndBiomeSource.create(...)` directly (a full, unconditional replacement); it was dropped in favor of this official, weighted API once discovered, since it composes correctly with other mods and doesn't hard-remove the vanilla biome.
- **Shape** (`VoidIslandFeature`, configured/placed as `echoing_void:void_island` / `void_island_decorated`): the same shrinking-radius-stack algorithm as vanilla's `EndIslandFeature` (the feature behind Small End Islands), scaled down to a ~3-6 block footprint and only 1-2 layers thick, built from Void Stone instead of End Stone. Placement is denser than vanilla's version (`rarity_filter` chance 3 vs. vanilla's 14, count 2-3 vs. 1-2) and uses a tighter height band (Y 60-68 vs. vanilla's 55-70), so neighboring islands end up closer together with smaller vertical steps — favoring parkour-style jumps between them.
- **Enderite Debris** also generates here (see below), same as the other outer End biomes.

### Void Liquid Pools

Small surface pools of [Void Liquid](#fluid-void-liquid), rimmed with Void Stone.

- **Feature**: `echoing_void:void_liquid_pool`, a vanilla `minecraft:lake` feature (the same feature type behind vanilla's rare surface lava pools) — carves a small bowl and fills it with Void Liquid, using Void Stone as the sealing barrier around it instead of stone.
- **Placement**: `rarity_filter` chance 40, at the `WORLD_SURFACE_WG` heightmap, generation step `LAKES`.
- **Biomes**: every *outer* End biome — `end_highlands`, `end_midlands`, `end_barrens`, `small_end_islands`, and `echoing_void:void_islands` — not the central main island (`the_end`).

### Enderite Debris Ore Generation

Registered via Fabric API's `BiomeModifications` (not raw biome-JSON overrides) at the `UNDERGROUND_DECORATION` generation step — the same step real `ore_ancient_debris_*` uses in this version, confirmed by inspecting the actual game jar rather than assumed.

- **Biomes**: `end_highlands`, `end_midlands`, `end_barrens`, `small_end_islands`, `echoing_void:void_islands` — the *outer* End islands only, not the central main island.
- **Two veins per chunk**, mirroring ancient debris in shape: `enderite_debris_large` (vein size 3) and `enderite_debris_small` (vein size 2), each a `minecraft:scattered_ore` feature targeting `minecraft:end_stone` → `echoing_void:enderite_debris`. Unlike ancient debris, `discard_chance_on_air_exposure` is `0.0` (never discarded), so a vein that happens to generate near a thin island's surface or an edge stays visible/exposed instead of vanishing - the intent is that it's occasionally spottable above ground, not perpetually buried.
- **Height**: `minecraft:uniform` distribution between Y=16 and Y=65 (flat probability across that band — no bias toward center or bottom, matching the request that spawn rate be roughly even regardless of layer). This differs from real `ore_ancient_debris_large`, which uses a center-biased `trapezoid`.

### End City Brewing Stand Swap

`EndCityPiecesMixin` injects a custom `StructureProcessor` (`EndBrewingStandProcessor`) into **every** End City structure piece's placement settings. That processor rewrites any vanilla `minecraft:brewing_stand` block found in the structure template into `echoing_void:end_brewing_stand` (preserving the bottle-slot block-state flags), so every End City ship generates with the mod's brewing stand already in place — no loot-table injection needed, it's a structural swap at generation time.

---

## Progression Chain Summary

```
End islands (Y 16-65, outer islands only)
        │  mine with netherite+ pickaxe
        ▼
Enderite Debris ──(smelt/blast)──▶ Enderite Scrap
        │                                  │
        │                                  ▼
        └────────────┬──── 4 Diamond + 4 Scrap + 1 Netherite Ingot
                      ▼
               Enderite Ingot ──(×9 craft)──▶ Block of Enderite
                      │                             │
                      │                        (reversible, ×9 back)
                      ▼
   Enderite Upgrade Smithing Template (End City loot, or dupe: 5 Netherite Ingot +
   2 Void Stone + 1 End Stone + 1 existing template)
                      │
                      ▼
   Smithing table: Template + [netherite item] + Enderite Ingot
                      │
                      ▼
   Enderite Pickaxe / Axe / Shovel / Hoe / Spear / Helmet / Chestplate / Leggings / Boots
   The Beginning and The End (from netherite_sword)   Call of the Void (from bow)
```

---

## Loot Tables

### End City Treasure (`minecraft:chests/end_city_treasure`)

Built from **two independent mechanisms** that stack:

1. **Datapack override** (`data/minecraft/loot_table/chests/end_city_treasure.json`) — a full copy of the vanilla table (required since loot tables replace rather than merge) with additions:
   - Main pool: `enderite_scrap` (1–2, weight 3) and `enderite_debris` (1–2, weight 2), alongside vanilla diamond/iron/gold/emerald/etc.
   - Template pool: `empty` (14) / `spire_armor_trim_smithing_template` (implicit weight 1) / `enderite_upgrade_smithing_template` (weight 2).
2. **Code-side additive modification** (`ModLootTables`, via `LootTableEvents.MODIFY`) — adds two *more* pools on top of whatever the table resolves to:
   - Enchanted book pool: `empty` (6) / a `Void Strike I–III` book (1).
   - Weapon pool: `empty` (12) / Void-Strike-enchanted Diamond Sword, Diamond Axe, Diamond Spear, or Bow (weight 1 each).

### Mob Drops

Watchling and Blastling both drop 1–3 Ender Pearls (weighted 45 empty / 30×1 / 20×2 / 5×3).

### Block Drops

Standard single-item self-drops (with `survives_explosion`) for Void Stone, Enderite Debris, Enderite Block. The End Brewing Stand's drop also copies its custom name (if renamed) from the block entity via `copy_components`.

---

## Tags Reference

| Tag | Namespace/Registry | Contents |
|---|---|---|
| `echoing_void:immune_to_void_poisoned` | entity_type | `minecraft:enderman`, `echoing_void:blastling`, `echoing_void:watchling` |
| `echoing_void:enchantable/void_strike` | item | `#minecraft:enchantable/weapon`, `#minecraft:enchantable/bow`, `#minecraft:enchantable/crossbow` |
| `echoing_void:enderite_repair` | item | `echoing_void:enderite_ingot` — anvil repair material for the whole enderite line |
| `minecraft:in_enchanting_table` (enchantment) | added to | `echoing_void:void_strike` |
| `minecraft:enchantable/bow` | added to | `echoing_void:call_of_the_void` |
| `minecraft:enchantable/durability` | added to | `echoing_void:call_of_the_void` |
| `minecraft:swords` | added to | `echoing_void:the_beginning_of_the_end` |
| `minecraft:needs_diamond_tool` | added to | `echoing_void:void_stone`, `echoing_void:enderite_debris`, `echoing_void:enderite_block` |
| `minecraft:incorrect_for_diamond_tool` | added to | `echoing_void:enderite_debris`, `echoing_void:enderite_block` |
| `minecraft:mineable/pickaxe` | added to | `echoing_void:void_stone`, `echoing_void:enderite_debris`, `echoing_void:enderite_block` |
| `minecraft:water` (fluid) | added to | `echoing_void:void_liquid`, `echoing_void:flowing_void_liquid` — for water-like entity physics only, see [Fluid: Void Liquid](#fluid-void-liquid) |

---

## Client-Side Features

### Rendering

- **Watchling**: custom body model + `WatchlingEyesLayer` (glowing eyes, 3 blink stages, hidden only during the teleport-hide window).
- **Blastling**: custom body model + `BlastlingEyesLayer` + a separate animated `BlastlingFlameLayer` (32-frame flame texture sequence rendered as its own small model layered over the head).
- **Blastling Goo**: rendered via vanilla's generic `ThrownItemRenderer` as a full-bright flat billboard.

### Sky (Nuit / formerly FabricSkyboxes)

Two layered sky configs for `minecraft:the_end`:
- `echoing_void_vanilla_end` (layer 0) — re-declares the vanilla End sky.
- `echoing_void_star_twins_end` (layer 1) — a custom additive-blended "Star Twins" texture, slowly rotating, 24000-tick fade cycle, hidden underwater.

### JEI Integration

Registered as a JEI plugin (`EchoingVoidJeiPlugin`, environment: client-only). Adds:
- A dedicated **End Brewing** recipe category showing Ender Pearl + Awkward Potion → Potion of Void Poisoned (the only step that's exclusive to the End Brewing Stand — registered as its recipe catalyst).
- The Splash/Lingering Void Poisoned conversions folded into JEI's normal shared **Brewing** category (since any brewing stand can do those).
- Cleanup of phantom "generic potion" entries: registering `ModPotions.VOID_POISONED` globally makes JEI's vanilla-potion scanner also discover it on the generic `minecraft:potion` / `splash_potion` / `lingering_potion` / `tipped_arrow` items even though only the mod's own dedicated items are ever produced — those duplicates are explicitly removed at runtime. The same cleanup happens in the creative inventory tab (`ModItems`/`ModCreativeModeTabs`).
- Compiled against a locally-vendored JEI jar (`run/mods/jei-26.1.2-fabric-29.5.0.28.jar`) since no Maven build exists yet for this bleeding-edge Minecraft version — this file lives in the gitignored `run/` folder and is not tracked. The dependency is **optional at build time**: `build.gradle` checks whether the jar exists before adding it, and excludes `name/modid/client/compat/**` from the client source set entirely when it's missing, so `compileClientJava` still succeeds on a fresh clone (or after a wiped `run/` folder) — just without JEI support until the jar is put back.

---

## Mixins Reference

| Mixin | Target | Purpose |
|---|---|---|
| `AbstractArrowMixin` | `AbstractArrow` | Plays Call of the Void's impact/charge-impact sound on arrow-hit-entity/block, based on the arrow's charged-shot tag |
| `AnvilMenuMixin` | `AnvilMenu` | Removes the vanilla 40-level "Too Expensive!" cap entirely (normally bypassed only for creative players); makes Call of the Void ignore enchantment exclusive-set conflicts (e.g. Infinity + Mending) so any combo can be combined onto it |
| `BottleItemMixin` | `BottleItem` | Redirects glass-bottle filling from a Void Liquid source to a Potion of Void Poisoned instead of vanilla's hardcoded water-bottle result |
| `BrewingStandIngredientsSlotMixin` | `BrewingStandMenu$IngredientsSlot` | Allows placing an Ender Pearl in the ingredient slot specifically when the container is an `EndBrewingStandBlockEntity` |
| `EndCityPiecesMixin` | `EndCityPieces$EndCityPiece` | Injects the brewing-stand-swap structure processor into every End City piece |
| `GrindstoneMenuMixin` | `GrindstoneMenu` | Restores innate Void Strike I on the two legendary weapons after a grind pass (it isn't a curse, so it would otherwise be stripped) |
| `InventoryMixin` | `Inventory` | Plays a weapon's equip sound on hotbar slot switch |
| `ItemEntityMixin` | `ItemEntity` | Plays a weapon's pick-up sound on `playerTouch` |
| `LivingEntityMixin` | `LivingEntity` | Blocks Void Poisoned from affecting immune entity types; plays the deactivate sound when it wears off |
| `PlayerMixin` | `Player` | Drives The Beginning and The End's 3-hit combo sound on `itemAttackInteraction`; plays a weapon's drop sound on `drop` |

(Client-only mixins are declared separately via `echoing_void.client.mixins.json` per `fabric.mod.json`.)

---

## Known Gaps / WIP Notes

None currently — see git history for in-progress work.
