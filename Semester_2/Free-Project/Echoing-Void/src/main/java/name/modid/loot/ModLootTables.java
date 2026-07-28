package name.modid.loot;

import net.fabricmc.fabric.api.loot.v3.LootTableEvents;

import net.minecraft.core.Holder;
import net.minecraft.core.registries.Registries;
import net.minecraft.world.item.Items;
import net.minecraft.world.item.enchantment.Enchantment;
import net.minecraft.world.level.ItemLike;
import net.minecraft.world.level.storage.loot.BuiltInLootTables;
import net.minecraft.world.level.storage.loot.LootPool;
import net.minecraft.world.level.storage.loot.entries.EmptyLootItem;
import net.minecraft.world.level.storage.loot.entries.LootItem;
import net.minecraft.world.level.storage.loot.entries.LootPoolSingletonContainer;
import net.minecraft.world.level.storage.loot.functions.SetEnchantmentsFunction;
import net.minecraft.world.level.storage.loot.providers.number.ConstantValue;
import net.minecraft.world.level.storage.loot.providers.number.UniformGenerator;

import name.modid.enchantment.ModEnchantments;

// Void Strike is only found naturally in End City chests: an enchanted book pool and a weapon pool,
// each mostly empty so the enchantment stays rare. It is also enchanting-table obtainable, see
// data/echoing_void/enchantment/void_strike.json.
public class ModLootTables {
	public static void init() {
		LootTableEvents.MODIFY.register((key, tableBuilder, source, registries) -> {
			if (!BuiltInLootTables.END_CITY_TREASURE.equals(key) || !source.isBuiltin()) {
				return;
			}

			Holder<Enchantment> voidStrike = registries.lookupOrThrow(Registries.ENCHANTMENT).getOrThrow(ModEnchantments.VOID_STRIKE);

			tableBuilder.withPool(
				LootPool.lootPool()
					.setRolls(ConstantValue.exactly(1.0F))
					.add(EmptyLootItem.emptyItem().setWeight(6))
					.add(bookEntry(voidStrike))
			);

			tableBuilder.withPool(
				LootPool.lootPool()
					.setRolls(ConstantValue.exactly(1.0F))
					.add(EmptyLootItem.emptyItem().setWeight(12))
					.add(weaponEntry(Items.DIAMOND_SWORD, voidStrike))
					.add(weaponEntry(Items.DIAMOND_AXE, voidStrike))
					.add(weaponEntry(Items.DIAMOND_SPEAR, voidStrike))
					.add(weaponEntry(Items.BOW, voidStrike))
			);
		});
	}

	private static LootPoolSingletonContainer.Builder<?> bookEntry(Holder<Enchantment> voidStrike) {
		return LootItem.lootTableItem(Items.BOOK).setWeight(1).apply(setVoidStrike(voidStrike));
	}

	private static LootPoolSingletonContainer.Builder<?> weaponEntry(ItemLike item, Holder<Enchantment> voidStrike) {
		return LootItem.lootTableItem(item).setWeight(1).apply(setVoidStrike(voidStrike));
	}

	private static SetEnchantmentsFunction.Builder setVoidStrike(Holder<Enchantment> voidStrike) {
		return new SetEnchantmentsFunction.Builder().withEnchantment(voidStrike, UniformGenerator.between(1.0F, 3.0F));
	}
}
