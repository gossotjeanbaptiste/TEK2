package name.modid.enchantment;

import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.item.enchantment.Enchantment;

import name.modid.EchoingVoid;

// Void Strike is fully data-driven (data/echoing_void/enchantment/void_strike.json) - this key is
// only needed to reference it from code (loot table injection).
public class ModEnchantments {
	public static final ResourceKey<Enchantment> VOID_STRIKE = ResourceKey.create(Registries.ENCHANTMENT, EchoingVoid.id("void_strike"));
}
