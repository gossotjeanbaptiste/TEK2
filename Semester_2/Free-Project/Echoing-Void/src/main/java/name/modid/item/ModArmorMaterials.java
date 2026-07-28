package name.modid.item;

import java.util.Map;

import net.minecraft.resources.ResourceKey;
import net.minecraft.sounds.SoundEvents;
import net.minecraft.world.item.equipment.ArmorMaterial;
import net.minecraft.world.item.equipment.ArmorType;
import net.minecraft.world.item.equipment.EquipmentAssets;

import name.modid.EchoingVoid;
import name.modid.tag.ModItemTags;

public class ModArmorMaterials {
	// Beyond-netherite armor. Netherite's own material is durability 37, defense
	// {boots 3, leggings 6, chestplate 8, helmet 3}, enchantmentValue 15, toughness 3.0F,
	// knockbackResistance 0.1F - every figure here is bumped past that, same as
	// ModToolMaterials.ENDERITE is bumped past netherite's tool material.
	public static final ArmorMaterial ENDERITE = new ArmorMaterial(
		48,
		Map.of(
			ArmorType.BOOTS, 4,
			ArmorType.LEGGINGS, 7,
			ArmorType.CHESTPLATE, 9,
			ArmorType.HELMET, 4),
		20,
		SoundEvents.ARMOR_EQUIP_NETHERITE,
		4.0F,
		0.15F,
		ModItemTags.ENDERITE_REPAIR,
		ResourceKey.create(EquipmentAssets.ROOT_ID, EchoingVoid.id("enderite")));
}
