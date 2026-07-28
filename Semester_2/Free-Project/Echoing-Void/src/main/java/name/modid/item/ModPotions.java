package name.modid.item;

import net.minecraft.core.Holder;
import net.minecraft.core.Registry;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.effect.MobEffectInstance;
import net.minecraft.world.item.alchemy.Potion;

import name.modid.EchoingVoid;
import name.modid.effect.ModMobEffects;

public class ModPotions {
	public static final Holder<Potion> VOID_POISONED = register("void_poisoned",
		new Potion("void_poisoned", new MobEffectInstance(ModMobEffects.VOID_POISONED, ModMobEffects.VOID_POISONED_DEFAULT_DURATION_TICKS)));

	private static Holder<Potion> register(String name, Potion potion) {
		ResourceKey<Potion> key = ResourceKey.create(Registries.POTION, EchoingVoid.id(name));
		return Registry.registerForHolder(BuiltInRegistries.POTION, key, potion);
	}

	public static void init() {
	}
}
