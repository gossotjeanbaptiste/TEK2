package name.modid.item;

import java.util.List;
import java.util.function.Function;

import net.fabricmc.fabric.api.creativetab.v1.CreativeModeTabEvents;
import net.fabricmc.fabric.api.registry.FabricPotionBrewingBuilder;

import net.minecraft.ChatFormatting;
import net.minecraft.core.HolderLookup;
import net.minecraft.core.Registry;
import net.minecraft.core.component.DataComponents;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.network.chat.Component;
import net.minecraft.resources.Identifier;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.item.BucketItem;
import net.minecraft.world.item.CreativeModeTabs;
import net.minecraft.world.item.Item;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.item.Items;
import net.minecraft.world.item.LingeringPotionItem;
import net.minecraft.world.item.Rarity;
import net.minecraft.world.item.SmithingTemplateItem;
import net.minecraft.world.item.SpawnEggItem;
import net.minecraft.world.item.SplashPotionItem;
import net.minecraft.world.item.alchemy.PotionContents;
import net.minecraft.world.item.component.Consumable;
import net.minecraft.world.item.component.Consumables;
import net.minecraft.world.item.enchantment.ItemEnchantments;
import net.minecraft.world.item.equipment.ArmorType;

import name.modid.EchoingVoid;
import name.modid.enchantment.ModEnchantments;
import name.modid.entity.ModEntities;
import name.modid.fluid.ModFluids;
import name.modid.tag.ModItemTags;

public class ModItems {
	private static final float LINGERING_DURATION_SCALE = 0.25F;

	public static final Item VOID_POISONED_POTION = register("void_poisoned_potion",
		properties -> new VoidPoisonedPotionItem(properties
			.stacksTo(1)
			.usingConvertsTo(Items.GLASS_BOTTLE)
			.component(DataComponents.CONSUMABLE, voidPoisonedConsumable())
			.component(DataComponents.POTION_CONTENTS, voidPoisonedPotionContents())));

	public static final Item VOID_POISONED_SPLASH_POTION = register("void_poisoned_splash_potion",
		properties -> new SplashPotionItem(properties
			.stacksTo(1)
			.component(DataComponents.POTION_CONTENTS, voidPoisonedPotionContents())));

	public static final Item VOID_POISONED_LINGERING_POTION = register("void_poisoned_lingering_potion",
		properties -> new LingeringPotionItem(properties
			.stacksTo(1)
			.component(DataComponents.POTION_CONTENTS, voidPoisonedPotionContents())
			.component(DataComponents.POTION_DURATION_SCALE, LINGERING_DURATION_SCALE)));

	// Not obtainable/creative-tab listed - only exists so BlastlingGooEntity has an ItemStack to
	// hand to ThrownItemRenderer (the projectile renders as a flat thrown-item billboard).
	public static final Item BLASTLING_GOO = register("blastling_goo", Item::new);

	// Beyond-netherite material chain: enderite_debris (block, world ore) smelts into scrap,
	// scrap refines into the ingot. Fire resistant like their netherite equivalents.
	public static final Item ENDERITE_SCRAP = register("enderite_scrap",
		properties -> new Item(properties.fireResistant().rarity(Rarity.UNCOMMON)));
	public static final Item ENDERITE_INGOT = register("enderite_ingot",
		properties -> new Item(properties.fireResistant().rarity(Rarity.RARE)));

	// Unique weapons from Minecraft Dungeons' Echoing Void DLC. Repaired on an anvil with an
	// enderite ingot, same as the rest of the enderite gear below. Both carry an innate Void
	// Strike I - GrindstoneMenuMixin re-applies it after grinding since it isn't a curse.
	public static final Item THE_BEGINNING_OF_THE_END = register("the_beginning_of_the_end",
		properties -> new Item(properties
			.sword(ModToolMaterials.ENDERITE, 3.0F, -1.6F)
			.rarity(Rarity.EPIC)
			.repairable(ModItemTags.ENDERITE_REPAIR)
			.delayedComponent(DataComponents.ENCHANTMENTS, ModItems::innateVoidStrike)));

	public static final Item CALL_OF_THE_VOID = register("call_of_the_void",
		properties -> new CallOfTheVoidBowItem(properties
			.durability(900)
			.enchantable(15)
			.rarity(Rarity.EPIC)
			.repairable(ModItemTags.ENDERITE_REPAIR)
			.delayedComponent(DataComponents.ENCHANTMENTS, ModItems::innateVoidStrike)));

	// Enderite tool/weapon/armor set - all smithing-table upgrades from their netherite
	// equivalent (see data/echoing_void/recipe/enderite_*_smithing.json), using an Enderite
	// Upgrade Smithing Template + an enderite ingot. Base attack numbers are kept identical to
	// their netherite counterparts (see the real netherite_* registrations for those figures);
	// the improvement over netherite comes entirely from ModToolMaterials.ENDERITE/ArmorMaterials.ENDERITE
	// (more durability, more damage/defense, better enchantability), same as how
	// THE_BEGINNING_OF_THE_END only swaps material rather than reinventing sword numbers.
	public static final Item ENDERITE_PICKAXE = register("enderite_pickaxe",
		properties -> new Item(properties.pickaxe(ModToolMaterials.ENDERITE, 1.0F, -2.8F).fireResistant()));
	public static final Item ENDERITE_AXE = register("enderite_axe",
		properties -> new Item(properties.axe(ModToolMaterials.ENDERITE, 5.0F, -3.0F).fireResistant()));
	public static final Item ENDERITE_SHOVEL = register("enderite_shovel",
		properties -> new Item(properties.shovel(ModToolMaterials.ENDERITE, 1.5F, -3.0F).fireResistant()));
	public static final Item ENDERITE_HOE = register("enderite_hoe",
		properties -> new Item(properties.hoe(ModToolMaterials.ENDERITE, 0.0F, -4.0F).fireResistant()));
	// The generic spear tier (netherite_spear's own 9 tuning figures, untouched - only the
	// material changes) - distinct from the two named legendary weapons above.
	public static final Item ENDERITE_SPEAR = register("enderite_spear",
		properties -> new Item(properties
			.spear(ModToolMaterials.ENDERITE, 1.15F, 1.2F, 0.4F, 2.5F, 9.0F, 5.5F, 5.1F, 8.75F, 4.6F)
			.fireResistant()));

	public static final Item ENDERITE_HELMET = register("enderite_helmet",
		properties -> new Item(properties.humanoidArmor(ModArmorMaterials.ENDERITE, ArmorType.HELMET).fireResistant()));
	public static final Item ENDERITE_CHESTPLATE = register("enderite_chestplate",
		properties -> new Item(properties.humanoidArmor(ModArmorMaterials.ENDERITE, ArmorType.CHESTPLATE).fireResistant()));
	public static final Item ENDERITE_LEGGINGS = register("enderite_leggings",
		properties -> new Item(properties.humanoidArmor(ModArmorMaterials.ENDERITE, ArmorType.LEGGINGS).fireResistant()));
	public static final Item ENDERITE_BOOTS = register("enderite_boots",
		properties -> new Item(properties.humanoidArmor(ModArmorMaterials.ENDERITE, ArmorType.BOOTS).fireResistant()));

	// A real SmithingTemplateItem (not a plain Item) so the smithing table shows the "Applies
	// to"/"Ingredients" tooltip and the ghost-icon hints in the base/addition slots, same as the
	// vanilla netherite upgrade template. Reuses vanilla's generic slot-shape sprites - there's no
	// "bow" one, so the base slot rotation only previews the 10 tool/armor pieces, not the bow.
	public static final Item ENDERITE_UPGRADE_SMITHING_TEMPLATE = register("enderite_upgrade_smithing_template",
		properties -> new SmithingTemplateItem(
			Component.translatable("item.echoing_void.smithing_template.enderite_upgrade.applies_to")
				.withStyle(ChatFormatting.BLUE),
			Component.translatable("item.echoing_void.smithing_template.enderite_upgrade.ingredients")
				.withStyle(ChatFormatting.BLUE),
			Component.translatable("item.echoing_void.smithing_template.enderite_upgrade.base_slot_description")
				.withStyle(ChatFormatting.BLUE),
			Component.translatable("item.echoing_void.smithing_template.enderite_upgrade.additions_slot_description")
				.withStyle(ChatFormatting.BLUE),
			List.of(
				Identifier.withDefaultNamespace("container/slot/helmet"),
				Identifier.withDefaultNamespace("container/slot/chestplate"),
				Identifier.withDefaultNamespace("container/slot/leggings"),
				Identifier.withDefaultNamespace("container/slot/boots"),
				Identifier.withDefaultNamespace("container/slot/pickaxe"),
				Identifier.withDefaultNamespace("container/slot/axe"),
				Identifier.withDefaultNamespace("container/slot/shovel"),
				Identifier.withDefaultNamespace("container/slot/hoe"),
				Identifier.withDefaultNamespace("container/slot/sword"),
				Identifier.withDefaultNamespace("container/slot/spear")),
			List.of(Identifier.withDefaultNamespace("container/slot/ingot")),
			properties));

	// Real functional spawn eggs (spawn the mob on use, like vanilla) - creative/command only,
	// same as every vanilla spawn egg, not craftable.
	public static final Item WATCHLING_SPAWN_EGG = register("watchling_spawn_egg",
		properties -> new SpawnEggItem(properties.spawnEgg(ModEntities.WATCHLING)));
	public static final Item BLASTLING_SPAWN_EGG = register("blastling_spawn_egg",
		properties -> new SpawnEggItem(properties.spawnEgg(ModEntities.BLASTLING)));

	public static final Item VOID_LIQUID_BUCKET = register("void_liquid_bucket",
		properties -> new BucketItem(ModFluids.VOID_LIQUID, properties.craftRemainder(Items.BUCKET).stacksTo(1)));

	private static Consumable voidPoisonedConsumable() {
		// No explicit onConsume effect here: PotionContents (attached below) is itself a
		// ConsumableListener and applies its effects automatically when the item is drunk.
		return Consumables.defaultDrink()
			.hasConsumeParticles(false)
			.build();
	}

	private static PotionContents voidPoisonedPotionContents() {
		return new PotionContents(ModPotions.VOID_POISONED);
	}

	private static ItemEnchantments innateVoidStrike(HolderLookup.Provider provider) {
		ItemEnchantments.Mutable enchantments = new ItemEnchantments.Mutable(ItemEnchantments.EMPTY);
		enchantments.set(provider.lookupOrThrow(Registries.ENCHANTMENT).getOrThrow(ModEnchantments.VOID_STRIKE), 1);
		return enchantments.toImmutable();
	}

	private static boolean isVanillaVoidPoisonedContainer(ItemStack stack) {
		if (!stack.is(Items.POTION) && !stack.is(Items.SPLASH_POTION) && !stack.is(Items.LINGERING_POTION)
			&& !stack.is(Items.TIPPED_ARROW)) {
			return false;
		}
		PotionContents contents = stack.getOrDefault(DataComponents.POTION_CONTENTS, PotionContents.EMPTY);
		return contents.is(ModPotions.VOID_POISONED);
	}

	private static Item register(String name, Function<Item.Properties, Item> factory) {
		ResourceKey<Item> key = ResourceKey.create(Registries.ITEM, EchoingVoid.id(name));
		return Registry.register(BuiltInRegistries.ITEM, key, factory.apply(new Item.Properties().setId(key)));
	}

	public static void init() {
		CreativeModeTabEvents.modifyOutputEvent(CreativeModeTabs.FOOD_AND_DRINKS)
			.register(output -> {
				output.accept(VOID_POISONED_POTION);
				output.accept(VOID_POISONED_SPLASH_POTION);
				output.accept(VOID_POISONED_LINGERING_POTION);
				// Registering ModPotions.VOID_POISONED globally makes vanilla's own
				// generatePotionEffectTypes list it again under the generic minecraft:potion/
				// splash_potion/lingering_potion items - remove those duplicates.
				output.getDisplayStacks().removeIf(ModItems::isVanillaVoidPoisonedContainer);
				output.getSearchTabStacks().removeIf(ModItems::isVanillaVoidPoisonedContainer);
			});
		CreativeModeTabEvents.modifyOutputEvent(CreativeModeTabs.SPAWN_EGGS)
			.register(output -> {
				output.accept(WATCHLING_SPAWN_EGG);
				output.accept(BLASTLING_SPAWN_EGG);
			});
		CreativeModeTabEvents.modifyOutputEvent(CreativeModeTabs.COMBAT)
			.register(output -> {
				output.accept(THE_BEGINNING_OF_THE_END);
				output.accept(CALL_OF_THE_VOID);
				output.accept(ENDERITE_SPEAR);
				output.accept(ENDERITE_HELMET);
				output.accept(ENDERITE_CHESTPLATE);
				output.accept(ENDERITE_LEGGINGS);
				output.accept(ENDERITE_BOOTS);
				// generatePotionEffectTypes also lists ModPotions.VOID_POISONED on Items.TIPPED_ARROW here.
				output.getDisplayStacks().removeIf(ModItems::isVanillaVoidPoisonedContainer);
				output.getSearchTabStacks().removeIf(ModItems::isVanillaVoidPoisonedContainer);
			});
		CreativeModeTabEvents.modifyOutputEvent(CreativeModeTabs.TOOLS_AND_UTILITIES)
			.register(output -> {
				output.accept(ENDERITE_PICKAXE);
				output.accept(ENDERITE_AXE);
				output.accept(ENDERITE_SHOVEL);
				output.accept(ENDERITE_HOE);
				output.accept(VOID_LIQUID_BUCKET);
			});
		CreativeModeTabEvents.modifyOutputEvent(CreativeModeTabs.INGREDIENTS)
			.register(output -> {
				output.accept(ENDERITE_SCRAP);
				output.accept(ENDERITE_INGOT);
				output.accept(ENDERITE_UPGRADE_SMITHING_TEMPLATE);
			});

		// Same container-conversion mechanism vanilla uses for Potion -> Splash -> Lingering.
		FabricPotionBrewingBuilder.BUILD.register(builder -> {
			builder.addContainer(VOID_POISONED_POTION);
			builder.addContainer(VOID_POISONED_SPLASH_POTION);
			builder.addContainer(VOID_POISONED_LINGERING_POTION);
			builder.addContainerRecipe(VOID_POISONED_POTION, Items.GUNPOWDER, VOID_POISONED_SPLASH_POTION);
			builder.addContainerRecipe(VOID_POISONED_SPLASH_POTION, Items.DRAGON_BREATH, VOID_POISONED_LINGERING_POTION);
		});
	}
}
